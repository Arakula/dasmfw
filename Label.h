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
/* Label.h : definitions for the Label classes                               */
/*****************************************************************************/

#ifndef __Label_h_defined__
#define __Label_h_defined__

#include "dasmfw.h"

/*****************************************************************************/
/* MemoryType : the various types of memory that may occur in a module       */
/*****************************************************************************/

enum MemoryType
  {
  Untyped,                              /* unknown type                      */
  Code,                                 /* Code                              */
  Data,                                 /* initialized data                  */
  Const,                                /* initialized constant data         */
  Bss,                                  /* uninitialized data                */
  IOPort,                               /* I/O Port address if separate      */
  };

/*****************************************************************************/
/* AddrType : a basic address / type combination                             */
/*****************************************************************************/

class AddrType
  {
  public:
    AddrType(adr_t addr = 0, MemoryType memType = Code)
      : addr(addr), memType(memType)
      { }
    virtual ~AddrType() { }

    bool operator<(const AddrType &other)
      {
      if (addr != other.addr)
        return addr < other.addr;
      // allow "Untyped" wildcards
      MemoryType imt = (memType == Untyped) ? other.memType : memType;
      MemoryType omt = (other.memType == Untyped) ? imt : other.memType;
      return imt < omt;
      }
    bool operator<=(const AddrType &other)
      {
      if (addr != other.addr)
        return addr < other.addr;
      // allow "Untyped" wildcards
      MemoryType imt = (memType == Untyped) ? other.memType : memType;
      MemoryType omt = (other.memType == Untyped) ? imt : other.memType;
      return imt <= omt;
      }
    bool operator==(const AddrType &other)
      {
      if (addr != other.addr)
        return false;
      // allow "Untyped" wildcards
      MemoryType imt = (memType == Untyped) ? other.memType : memType;
      MemoryType omt = (other.memType == Untyped) ? imt : other.memType;
      return imt == omt;
      }
    bool operator!=(const AddrType &other) { return !operator==(other); }
    bool operator>=(const AddrType &other) { return !operator<(other); }
    bool operator>(const AddrType &other) { return !operator<=(other); }

  // Attributes
  public:
    adr_t GetAddress() const { return addr; }
    void SetAddress(adr_t newAddr = 0) { addr = newAddr; }
    // Label Type
    MemoryType GetType() const { return memType; }
    void SetType(MemoryType newType = Untyped) { memType = newType; }
    bool IsCode() { return memType == Code; }
    bool IsData() { return memType == Data || memType == Const; }
    bool IsConst() { return memType == Const; }
    bool IsBss() { return memType == Bss; }
    bool IsIOPort() { return memType == IOPort; }
    void SetCode() { memType = Code; }
    void SetData() { memType = Data; }
    void SetConst() { memType = Const; }
    void SetBss() { memType = Bss; }
    void SetIOPort() { memType = IOPort; }

    void CopyUnset(const AddrType &other)
      {
      if (addr == NO_ADDRESS) addr = other.addr;
      if (memType == Untyped) memType = other.memType;
      }

  protected:
    adr_t addr;
    MemoryType memType;
  };

/*****************************************************************************/
/* AddrTypeArray : array of address/type-sorted structures                   */
/*****************************************************************************/

class AddrTypeArray : public vector<AddrType*>
  {
  public:
    AddrTypeArray(bool bMultipleDefs = false)
      : bMultipleDefs(bMultipleDefs) { }
    virtual ~AddrTypeArray(void)
      { clear(); }

    void SetMultipleDefs(bool bOn = true) { bMultipleDefs = bOn; }
    bool HasMultipleDefs() { return bMultipleDefs; }

  public:
    void clear()
      {
      for (const_iterator cit = begin();
           cit != end();
           cit++)
        {
        AddrType *p = *cit;
        delete p;
        }
      vector<AddrType*>::clear();
      }
    // binary search in sorted array
    iterator find(const AddrType &l, bool bTypeMatch = false)
      {
      int lo, hi = size() - 1, mid;
      // little optimization if definitely outside range
      if (empty() || *at(0) > l || *at(hi) < l)
        return end();
      lo = 0;
      while (lo <= hi)
        {
        mid = (hi + lo) / 2;
        if (*at(mid) < l)
          lo = mid + 1;
        else if (*at(mid) > l)
          hi = mid - 1;
        else
          {
          // override "Untyped" if specially requested
          if (bTypeMatch && (int)at(mid)->GetType() != (int)l.GetType())
            {
            if (at(mid)->GetType() < l.GetType())
              lo = mid + 1;
            else
              hi = mid - 1;
            }
          else
            {
            // return 1st match for multiple definitions
            while (mid > 0 && *at(mid - 1) == l)
              mid--;
            return begin() + mid;
            }
          }
        }
      return end();
      }
    // binary search in sorted array, returning predecessor if no direct match
    iterator findimprec(const AddrType &l)
      {
      int lo, hi = size() - 1, mid;
      // little optimization if definitely outside range
      if (empty() || *at(0) > l)
        return end();
      if (*at(hi) < l)
        lo = hi;
      else
        lo = 0;
      while (lo <= hi)
        {
        mid = (hi + lo) / 2;
        if (*at(mid) < l)
          lo = mid + 1;
        else if (*at(mid) > l)
          hi = mid - 1;
        else
          break;
        }
      if (hi < lo)
        hi = mid = lo;
      while ((*at(mid) > l) && mid)
        mid--;
      return (*at(mid) > l) ? end() : begin() + mid;
      }
    // convenience - find code and/or data labels
    iterator find(adr_t addr, MemoryType memType = Untyped, bool bTypeMatch = false)
      { return find(AddrType(addr, memType), bTypeMatch); }
    // insert into address/type-sorted array
    iterator insert(AddrType *pNewEl, bool bAfter = true, bool bTypeMatch = false)
      {
      iterator it;
#if 0
      // Copying pre-existing element's data is done by template below
#endif
      int lo, max = size() - 1, hi = max, mid = -1;
      if (empty() || *pNewEl < *at(0))
        it = begin();
      else if (*pNewEl > *at(hi))
        it = end();
      else
        {
        // binary search for insert pos
        lo = 0;
        it = end();
        while (lo <= hi)
          {
          mid = (hi + lo) / 2;
          if (*at(mid) < *pNewEl)
            lo = mid + 1;
          else if (*at(mid) > *pNewEl)
            hi = mid - 1;
          else
            {
            // override "Untyped" if specially requested
            if (bTypeMatch && at(mid)->GetType() != pNewEl->GetType())
              {
              if (at(mid)->GetType() < pNewEl->GetType())
                lo = mid + 1;
              else
                hi = mid - 1;
              }
            else
              {
              if (bMultipleDefs)
                {
                // we found an existing label for this address,
                // so we /might/ check the label name to assure
                // alphanumerically sorted labelling.
                // For now, simply insert as last one (order by sequence).
                do
                  {
                  if (bAfter)
                    mid++;
                  else
                    mid--;
                  if (bTypeMatch && at(mid)->GetType() != pNewEl->GetType())
                    break;
                  } while (mid >= 0 && mid <= max && *at(mid) == *pNewEl);
                }
              break;
              }
            }
          }
        if (mid < 0)
          mid++;
        else if (mid <= max)
          {
          if (bTypeMatch && 
              *at(mid) == *pNewEl &&
              at(mid)->GetType() < pNewEl->GetType())
            mid++;
          else if (*at(mid) < *pNewEl)
            mid++;
          }
        it = (mid > max) ? end() : (begin() + mid);
        }
      return vector<AddrType*>::insert(it, pNewEl);
      }
    // erase an element
    iterator erase(iterator _Where)
      {
      AddrType *p = *_Where;
      delete p;
      return vector<AddrType*>::erase(_Where);
      }
    iterator erase(iterator _First, iterator _Last)
      {
      for (const_iterator cit = _First;
           cit != _Last;
           cit++)
        {
        AddrType *p = *cit;
        delete p;
        }
      return vector<AddrType*>::erase(_First, _Last);
      }


  protected:
    bool bMultipleDefs;  // flag whether multiple definitions possible
  };

/*****************************************************************************/
/* TAddrTypeArray : template for an array of address-sorted structures       */
/*****************************************************************************/

template<class T> class TAddrTypeArray : public AddrTypeArray
  {
  public:
    TAddrTypeArray(bool bMultipleDefs = false)
      : AddrTypeArray(bMultipleDefs) { }
    // vector<> specializations
    T const *at(size_type _Pos) const { return (T const *)AddrTypeArray::at(_Pos); }
    T * at(size_type _Pos) { return (T *)AddrTypeArray::at(_Pos); }
	T const * operator[](size_type _Pos) const { return at(_Pos); }
	T *operator[](size_type _Pos) { return at(_Pos); }
    
    // insert into address/type-sorted array, allowing copying
    iterator insert(T *pNewEl, bool bAfter = true, bool bTypeMatch = false)
      {
      // if multiple definitions not allowed, replace old one
      if (!bMultipleDefs)
        {
        iterator it = AddrTypeArray::find(*pNewEl, bTypeMatch);
        if (it != end())
          {
          // copy unset data from pre-existing element
          pNewEl->CopyUnset((T const &)*(*it));
          delete *it;
          *it = pNewEl;
          return it;
          }
        }
      return AddrTypeArray::insert(pNewEl, bAfter, bTypeMatch);
      }
  };

/*****************************************************************************/
/* AddrText : a basic Text with address and type                             */
/*****************************************************************************/

class AddrText : public AddrType
  {
  public:
    AddrText(adr_t addr = 0, MemoryType memType = Code, string stext = "")
      : AddrType(addr, memType), stext(stext)
      { }
    virtual ~AddrText() { }

  // Attributes
  public:
    // Label Text
    string GetText() const { return stext; }
    bool SetText(string sNewText = "") { stext = sNewText; return true; }
    bool HasText() const { return !!stext.size(); }

    void CopyUnset(const AddrText &other)
      {
      AddrType::CopyUnset((const AddrType &)other);
      if (stext.empty()) stext = other.stext;
      }

  protected:
    string stext;

  };

/*****************************************************************************/
/* AddrTextArray : an array of texts with address and type                   */
/*****************************************************************************/

class AddrTextArray : public TAddrTypeArray<AddrText>
  {
  };

/*****************************************************************************/
/* Label : definition of a label                                             */
/*****************************************************************************/

class Label : public AddrText
  {
  public:

    Label(adr_t addr = 0, MemoryType memType = Code, string sLabel = "", bool bUsed = false)
      : AddrText(addr, memType, sLabel), bUsed(bUsed)
      { }
    virtual ~Label() { }

  // Attributes
  public:
    // Flag whether it is used
    bool IsUsed() { return bUsed; }
    bool SetUsed(bool bSet = true, adr_t ref = NO_ADDRESS, int bus = BusCode)
      {
      bUsed = bSet;
      if (ref != NO_ADDRESS)
        AddRef(ref, bus);
      return bSet;
      }

    bool AddRef(adr_t addr, int bus = BusCode)
      {
      try
        {
        refs.push_back(AddrBus(addr, bus));
        }
      catch(...)
        {
        return false;
        }
      return true;
      }
    size_t RefCount() { return refs.size(); }
    AddrBus &GetRef(size_t index) { return refs[index]; }
    adr_t GetRefAddr(size_t index) { return refs[index].addr; }
    int GetRefBus(size_t index) { return refs[index].bus; }
    void SortRefs()
      {
      AddrBusComp sortfunc;
      if (refs.size() > 1)
        sort(refs.begin(), refs.end(), sortfunc);
      }
  
    void CopyUnset(const Label &other)
      {
      AddrText::CopyUnset((const AddrText &)other);
      if (!bUsed) bUsed = other.bUsed;
      }

  protected:
    bool bUsed;
    vector<AddrBus> refs;

  };

/*****************************************************************************/
/* LabelArray : an array of labels                                           */
/*****************************************************************************/

class LabelArray : public TAddrTypeArray<Label>
  {
  public:
    LabelArray(bool bMultipleDefs = false) : TAddrTypeArray<Label>(bMultipleDefs) { }

    Label *GetFirst(adr_t addr, LabelArray::iterator &it, MemoryType memType = Untyped)
      {
      it = find(addr, memType);
      return (it != end()) ? (Label *)(*it) : NULL;
      }
    Label *GetPrevNamed(adr_t addr, LabelArray::iterator &it, MemoryType memType = Untyped)
      {
      // finds the named label of the given  type at this address or before
      if ((it = findimprec(addr)) == end())
        return NULL;
      while (it != begin())
        {
        Label *pLabel = (Label *)(*it);
        MemoryType lblMemType = pLabel->GetType();
        if (pLabel->HasText() &&
            ((memType == Untyped && lblMemType != Const) || (memType == lblMemType)))
          break;
        it--;
        }
      return (it != end()) ? (Label *)(*it) : NULL;
      }
    // only makes sense if the label array allows multiples
    Label *GetNext(adr_t addr, LabelArray::iterator &it, MemoryType memType = Untyped)
      {
      it++;
      bool bStill = (it != end() && (*it)->GetAddress() == addr);
      if (bStill)
        bStill &= (memType == Untyped || (*it)->GetType() == memType);
      return bStill ? (Label *)(*it) : NULL;
      }
    // little helper for single label arrays
    Label *Find(adr_t addr, MemoryType memType = Untyped)
      {
      LabelArray::iterator p;
      return GetFirst(addr, p, memType);
      }
  };

/*****************************************************************************/
/* DefLabel : definition of an definition label (i.e., label plus definition)*/
/*****************************************************************************/

class DefLabel : public Label
  {
  public:
    DefLabel(adr_t addr = 0, MemoryType memType = Const, string sLabel = "", string sDefinition = "")
      : Label(addr, memType, sLabel, true), sDefinition(sDefinition)
      { }

  // Attributes
  public:
    void SetDefinition(string sTxt = "") { sDefinition = sTxt; }
    string GetDefinition() { return sDefinition; }

  protected:
    string sDefinition;
  };

/*****************************************************************************/
/* DefLabelArray : an array of definition labels                             */
/*****************************************************************************/

class DefLabelArray : public TAddrTypeArray<DefLabel>
  {
  public:
    DefLabelArray(bool caseSensitive = true)
      : TAddrTypeArray<DefLabel>(false), caseSensitive(caseSensitive) { }

    void SetCaseSensitive(bool bOn = true) { caseSensitive = bOn; }
    bool IsCaseSensitive() { return caseSensitive; }

    // this one is definitely one label per address, so no GetFirst()/GetNext()
    DefLabel *Find(adr_t addr, MemoryType memType = Untyped)
      {
      DefLabelArray::iterator p = find(addr, memType);
      return (p != end()) ? (DefLabel *)(*p) : NULL;
      }
    DefLabel *Find(string sLabel)
      {
      // this is not really performant, but I don't think adding
      // a separate index for the names is necessary
      if (!caseSensitive) sLabel = lowercase(sLabel);
      for (LabelArray::iterator p = begin(); p != end(); p++)
        {
        string sChk(((DefLabel*)*p)->GetText());
        if (!caseSensitive)
          sChk = lowercase(sChk);
        if (sChk == sLabel)
          return (DefLabel *)(*p);
        }
      return NULL;
      }
  protected:
    bool caseSensitive;
  };

#endif // __Label_h_defined__
