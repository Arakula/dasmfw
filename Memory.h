/***************************************************************************
 * dasmfw -- Disassembler Framework                                        *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program; if not, write to the Free Software             *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               *
 ***************************************************************************/

/*****************************************************************************/
/* Memory.h : definition for the memory classes                              */
/*****************************************************************************/

#ifndef __Memory_h_defined__
#define __Memory_h_defined__

#include "dasmfw.h"

#include "Label.h"

/*
Memory contents have some kinds of attributes:

a) the memory type (Code, Data, Const, BSS)
b) flag whether used or unused
c) if not code, the cell size (1, 2, 4, 8, 10, 16, 32, ...)
d) if not code, the cell type (signed int, unsigned int, float)
e) if not code (or data reference / constant in code),
   the display format (binary, character, octal, decimal, hex)
*/

/*****************************************************************************/
/* TMemory : template for a vector with a start address                      */
/*****************************************************************************/

template<class T, class TType = uint8_t> class TMemory : public vector<T>
  {
  public:
    TMemory(addr_t addrStart = 0, addr_t memSize = 0, TType newType = (TType)0)
      : memType(newType)
      {
      label.SetAddress(addrStart);
      if (memSize)
        resize(memSize);
      }
    TMemory(TMemory const &org)
      { DoCopy(org); }
    TMemory &operator=(TMemory const &org)
      { return DoCopy(org); }

    // Getters / Setters
    void SetStart(addr_t addrStart = 0) { label.SetAddress(addrStart); }
    addr_t GetStart() const { return label.GetAddress(); }
    addr_t GetEnd() const { return GetStart() + size() - 1; }
    string GetLabel() { return label.GetText(); }
    bool SetLabel(string sNewText = "") { return label.SetText(sNewText); }
    TType GetType() { return memType; }
    void SetType(TType newType) { memType = newType; }
    bool TypeMatches(TType chkType) { return memType == chkType; }

    // vector specializations
    const_reference at(size_type _Pos) const { return vector<T>::at(_Pos - GetStart()); }
    reference at(size_type _Pos) { return vector<T>::at(_Pos - GetStart()); }
	const_reference operator[](size_type _Pos) const { return at(_Pos); }
	reference operator[](size_type _Pos) { return at(_Pos); }

  protected:
    Label label;
    TType memType;

    TMemory &DoCopy(TMemory const &org)
      {
      label = org.label;
      memType = org.memType;
      assign(org.begin(), org.end());
      return *this;
      }
  };


/*****************************************************************************/
/* TMemoryArray : template for an array of disjunct vectors with start addr. */
/*****************************************************************************/

template<class T, class TType = uint8_t>
    class TMemoryArray : public vector<TMemory<T, TType>>
  {
  public:
    TMemoryArray() { ResetLast(); }
    bool AddMemory(addr_t addrStart = 0, addr_t memSize = 0, TType memType = (TType)0, T *contents = NULL)
      {
      vector<TMemory<T, TType>>::iterator i;
      if (memSize)
        {
        addr_t addrEnd = addrStart + memSize - 1;
        // append to existing array, if possible
        for (i = begin(); i != end(); i++)
          {
          if (i->TypeMatches(memType))
            {
            addr_t curStart = i->GetStart();
            addr_t curEnd = i->GetEnd();
            addr_t curSize = i->size();

            // adjacent or overlapping existing memory block
            if (addrStart <= curEnd + 1 && addrEnd + 1 >= curStart)
              {
              addr_t newStart = (curStart < addrStart) ? curStart : addrStart;
              addr_t newEnd = (curEnd < addrEnd) ? addrEnd : curEnd;
              addr_t newSize = newEnd - newStart + 1;
              i->resize(newSize);
              // if resized overlaps the next one, swallow next one
              while (i + 1 != end() &&
                     newEnd + 1 >= (i + 1)->GetStart())
                {
                addr_t nextStart = (i + 1)->GetStart();
                addr_t nextEnd = (i + 1)->GetEnd();
                if (nextEnd > newEnd)
                  {
                  newEnd = nextEnd;
                  newSize = newEnd - newStart + 1;
                  i->resize(newSize);
                  }
                for (addr_t j = nextStart; j <= nextEnd; j++)
                  i->at(j) = (i + 1)->at(j);
                erase(i + 1);
                }
              // if inserted before current start, some shuffling is needed
              if (newStart != curStart)
                {
                i->SetStart(newStart);
                addr_t diff = curStart - newStart;
                for (addr_t j = curSize; j > 0; j--)
                  i->at(newStart + j - 1 + diff) = i->at(newStart + j - 1);
                }
              if (contents)
                {
                for (addr_t j = 0; j < memSize; j++)
                  i->at(addrStart + j) = contents[j];
                }
              return true;
              }
            }
          }
        }
      // obviously not adjacent to another area
      // do a simple insertion sort - shouldn't be too many areas
      for (i = begin(); i != end(); i++)
        if (addrStart < i->GetStart())
          break;
      TMemory<T, TType> &t =
          *insert(i,
                  TMemory<T, TType>(addrStart, 0, memType));
      if (memSize)
        t.resize(memSize);
      ResetLast();
      // done this way to avoid creating a potentially large temporary object
      if (contents && memSize)
        {
        for (addr_t j = 0; j < memSize; j++)
          t.at(addrStart + j) = contents[j];
        }
      return true;
      }
    // find memory area index for a given address
    addr_t GetMemIndex(addr_t addr)
      {
      // little speedup for multiple accesses in scattered environments
      if (addr >= last.start && addr <= last.end) return last.idx;
      for (TMemoryArray<T, TType>::size_type i = 0; i < size(); i++)
        {
        TMemory<T, TType> &mem = at(i);
        if (mem.GetStart() <= addr && (addr_t)(mem.GetStart() + mem.size()) > addr)
          {
          last.start = mem.GetStart();
          last.end = mem.GetEnd();
          last.idx = i;
          return i;
          }
        }
      return NO_ADDRESS;
      }
    TMemory<T, TType> *FindMem(addr_t addr)
      {
      addr_t idx = GetMemIndex(addr);
      if (idx != NO_ADDRESS)
        return &at(idx);
      return NULL;
      }
    // get item at a given address
    T *getat(addr_t addr)
      {
      TMemory<T, TType> *pmem = FindMem(addr);
      return (pmem) ? &pmem->at(addr) : NULL;
      }
    // get multiple bytes with(out) byte reversal
    bool getat(addr_t addr, T *val, addr_t len, bool bReverse = false) 
      {
      addr_t i;
      if (bReverse)
        for (i = 0; i < len; i++)
          {
          TMemory<T, TType> *pmem = FindMem(addr + i);
          if (!pmem)
             return false;
          val[len - 1 - i] = pmem->at(addr + i);
          }
      else
        {
        for (i = 0; i < len; i++)
          {
          TMemory<T, TType> *pmem = FindMem(addr + i);
          if (!pmem)
             return false;
          val[i] = pmem->at(addr + i);
          }
        }
      return true;
      }
    // write item at a given address
    bool setat(addr_t addr, T val)
      {
      TMemory<T, TType> *pmem = FindMem(addr);
      if (!pmem) return false;
      pmem->at(addr) = val;
      return true;
      }
    // write multiple bytes with(out) byte reversal
    bool setat(addr_t addr, T *val, addr_t len, bool bReverse = false) 
      {
      addr_t i;
      if (bReverse)
        {
        for (i = 0; i < len; i++)
          if (!setat(addr + i, val[len - 1 - i]))
            return false;
        }
      else
        {
        for (i = 0; i < len; i++)
          if (!setat(addr + i, val[i]))
            return false;
        }
      return true;
      }
    protected:
      struct
        {
        addr_t start;
        addr_t end;
        addr_t idx;
        } last;
      void ResetLast() { last.start = last.end = last.idx = NO_ADDRESS; }
  };


/*****************************************************************************/
/* MemAttribute : basic description of a memory cell's attributes            */
/*****************************************************************************/

struct MemAttribute
  {
  unsigned cellSize: 8;                 /* cell size 1..256 (i.e., +1)       */
  unsigned memType: 3;                  /* MemoryType                        */
  unsigned cellType: 3;                 /* CellType                          */
  unsigned display: 3;                  /* CellDisplay                       */
  unsigned used: 1;                     /* used (or not)                     */
  unsigned breakBefore: 1;              /* line break in disassembly         */

  enum Type
    {
    CellUntyped,
    SignedInt,
    UnsignedInt,
    Float
    };
  enum Display
    {
    CellUndisplayable,
    Binary,
    Char,
    Octal,
    Decimal,
    Hex,
    DefaultDisplay
    };

  MemAttribute
      (
      MemoryType memType = Code,
      int cellSize = 1,
      bool used = true,
      Type cellType = UnsignedInt,
      Display display = DefaultDisplay,
      bool breakBefore = false
      )
    : memType(memType), used(used), cellSize(cellSize - 1),
      cellType(cellType), display(display), breakBefore(breakBefore)
    { }

  // MemAttribute Getters / Setters
  MemoryType GetMemType() { return (MemoryType)memType; }
  void SetMemType(MemoryType newType = Code) { memType = (unsigned)newType; }
  bool IsUsed() { return !!used; }
  void SetUsed(bool bUsed = true) { used = !!bUsed; }
  Type GetCellType() { return (Type)cellType; }
  void SetCellType(Type newType) { cellType = (unsigned)newType; }
  int GetSize() { return cellSize + 1; }
  void SetSize(int newSize = 1) { cellSize = (unsigned)(newSize - 1); }
  Display GetDisplay() { return (Display)display; }
  void SetDisplay(Display newDisp = DefaultDisplay) { display = (unsigned)newDisp; }
  bool GetBreakBefore() { return !!breakBefore; }
  void SetBreakBefore(bool bOn = true) { breakBefore = !!bOn; }
  };

/*****************************************************************************/
/* MemoryArray : an array of memory areas                                    */
/*****************************************************************************/

class MemoryArray : public TMemoryArray<uint8_t, MemoryType>
  {
  // currently, no specialization needed
  };


#endif // __Memory_h_defined__
