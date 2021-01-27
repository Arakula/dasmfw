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
    TMemory(adr_t addrStart = 0, adr_t memSize = 0, TType newType = (TType)0)
      : memType(newType)
      {
      label.SetAddress(addrStart);
      if (memSize)
        this->resize(static_cast<typename vector<T>::size_type>(memSize));
      }
    TMemory(TMemory const &org) : vector<T>(org)
      { DoCopy(org); }
    TMemory &operator=(TMemory const &org)
      { return DoCopy(org); }

    // Getters / Setters
    void SetStart(adr_t addrStart = 0) { label.SetAddress(addrStart); }
    adr_t GetStart() const { return label.GetAddress(); }
    adr_t GetEnd() const { return GetStart() + vector<T>::size() - 1; }
    string GetLabel() { return label.GetText(); }
    bool SetLabel(string sNewText = "") { return label.SetText(sNewText); }
    TType GetType() { return memType; }
    void SetType(TType newType) { memType = newType; }
    bool TypeMatches(TType chkType) { return memType == chkType; }

    // vector specializations
    typename vector<T>::const_reference at(typename vector<T>::size_type _Pos) const { return vector<T>::at(_Pos - GetStart()); }
    typename vector<T>::reference at(typename vector<T>::size_type _Pos) { return vector<T>::at(_Pos - GetStart()); }
	typename vector<T>::const_reference operator[](typename vector<T>::size_type _Pos) const { return at(_Pos); }
	typename vector<T>::reference operator[](typename vector<T>::size_type _Pos) { return at(_Pos); }

  protected:
    Label label;
    TType memType;

    TMemory &DoCopy(TMemory const &org)
      {
      label = org.label;
      memType = org.memType;
      this->assign(org.begin(), org.end());
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
    bool AddMemory(adr_t addrStart = 0, adr_t memSize = 0, TType memType = (TType)0, T *contents = NULL)
      {
      typename vector<TMemory<T, TType>>::iterator i;
      if (memSize)
        {
        adr_t addrEnd = addrStart + memSize - 1;
        // append to existing array, if possible
        for (i = vector<TMemory<T, TType>>::begin(); i != vector<TMemory<T, TType>>::end(); i++)
          {
          if (i->TypeMatches(memType))
            {
            adr_t curStart = i->GetStart();
            adr_t curEnd = i->GetEnd();
            adr_t curSize = i->size();

            // adjacent or overlapping existing memory block
            if (addrStart <= curEnd + 1 && addrEnd + 1 >= curStart)
              {
              adr_t newStart = (curStart < addrStart) ? curStart : addrStart;
              adr_t newEnd = (curEnd < addrEnd) ? addrEnd : curEnd;
              adr_t newSize = newEnd - newStart + 1;
              i->resize(newSize);
              // if resized overlaps the next one, swallow next one
              while (i + 1 != vector<TMemory<T, TType>>::end() &&
                     newEnd + 1 >= (i + 1)->GetStart())
                {
                adr_t nextStart = (i + 1)->GetStart();
                adr_t nextEnd = (i + 1)->GetEnd();
                if (nextEnd > newEnd)
                  {
                  newEnd = nextEnd;
                  newSize = newEnd - newStart + 1;
                  i->resize(newSize);
                  }
                for (adr_t j = nextStart; j <= nextEnd; j++)
                  i->at(j) = (i + 1)->at(j);
                vector<TMemory<T, TType>>::erase(i + 1);
                }
              // if inserted before current start, some shuffling is needed
              if (newStart != curStart)
                {
                i->SetStart(newStart);
                adr_t diff = curStart - newStart;
                for (adr_t j = curSize; j > 0; j--)
                  i->at(newStart + j - 1 + diff) = i->at(newStart + j - 1);
                }
              if (contents)
                {
                for (adr_t j = 0; j < memSize; j++)
                  i->at(addrStart + j) = contents[j];
                }
              return true;
              }
            }
          }
        }
      // obviously not adjacent to another area
      // do a simple insertion sort - shouldn't be too many areas
      for (i = vector<TMemory<T, TType>>::begin(); i != vector<TMemory<T, TType>>::end(); i++)
        if (addrStart < i->GetStart())
          break;
      TMemory<T, TType> &t =
          *this->insert(i,
                  TMemory<T, TType>(addrStart, 0, memType));
      if (memSize)
        t.resize(memSize);
      ResetLast();
      // done this way to avoid creating a potentially large temporary object
      if (contents && memSize)
        {
        for (adr_t j = 0; j < memSize; j++)
          t.at(addrStart + j) = contents[j];
        }
      return true;
      }
    // find memory area index for a given address
    adr_t GetMemIndex(adr_t addr)
      {
      // little speedup for multiple accesses in scattered environments
      if (addr >= last.start && addr <= last.end) return last.idx;
      for (typename TMemoryArray<T, TType>::size_type i = 0; i < vector<TMemory<T, TType>>::size(); i++)
        {
        TMemory<T, TType> &mem = this->at(i);
        if (mem.GetStart() <= addr && (adr_t)(mem.GetStart() + mem.size()) > addr)
          {
          last.start = mem.GetStart();
          last.end = mem.GetEnd();
          last.idx = i;
          return i;
          }
        }
      return NO_ADDRESS;
      }
    TMemory<T, TType> *FindMem(adr_t addr)
      {
      adr_t idx = GetMemIndex(addr);
      if (idx != NO_ADDRESS)
        return &vector<TMemory<T, TType>>::at(static_cast<typename TMemory<T, TType>::size_type>(idx));
      return NULL;
      }
    // get item at a given address
    T *getat(adr_t addr)
      {
      TMemory<T, TType> *pmem = FindMem(addr);
      return (pmem) ? &pmem->at(addr) : NULL;
      }
    // get multiple bytes with(out) byte reversal
    bool getat(adr_t addr, T *val, adr_t len, bool bReverse = false) 
      {
      adr_t i;
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
    bool setat(adr_t addr, T val)
      {
      TMemory<T, TType> *pmem = FindMem(addr);
      if (!pmem) return false;
      pmem->at(addr) = val;
      return true;
      }
    // write multiple bytes with(out) byte reversal
    bool setat(adr_t addr, T *val, adr_t len, bool bReverse = false) 
      {
      adr_t i;
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
        adr_t start;
        adr_t end;
        adr_t idx;
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
  unsigned forcedAddr:1;                /* forced addressing                 */

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
      bool breakBefore = false,
      bool forcedAddr = false
      )
    : cellSize(cellSize - 1), memType(memType), cellType(cellType),
      display(display), used(used), breakBefore(breakBefore),
      forcedAddr(forcedAddr)
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
  bool GetForcedAddr() { return !!forcedAddr; }
  void SetForcedAddr(bool bOn = true) { forcedAddr = !!bOn; }
  };

/*****************************************************************************/
/* MemoryArray : an array of memory areas                                    */
/*****************************************************************************/

class MemoryArray : public TMemoryArray<uint8_t, MemoryType>
  {
  // currently, no specialization needed
  };


#endif // __Memory_h_defined__
