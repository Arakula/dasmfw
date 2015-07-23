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
  Untyped = -1,                         /* unknown type                      */
  Code,                                 /* Code                              */
  Data,                                 /* initialized data                  */
  Const,                                /* initialized constant data         */
  Bss,                                  /* uninitialized data                */
  };

/*****************************************************************************/
/* AddrText : a basic Text with address and type                             */
/*****************************************************************************/

class AddrText
  {
  public:
    AddrText(addr_t addr = 0, MemoryType memType = Code, std::string stext = "")
      : addr(addr), memType(memType), stext(stext)
      { }
    virtual ~AddrText() { }

    bool operator<(const AddrText &other)
      {
      if (addr != other.addr)
        return addr < other.addr;
      // allow "Untyped" wildcards
      MemoryType imt = (memType == Untyped) ? other.memType : memType;
      MemoryType omt = (other.memType == Untyped) ? imt : other.memType;
      return imt < omt;
      }
    bool operator<=(const AddrText &other)
      {
      if (addr != other.addr)
        return addr < other.addr;
      // allow "Untyped" wildcards
      MemoryType imt = (memType == Untyped) ? other.memType : memType;
      MemoryType omt = (other.memType == Untyped) ? imt : other.memType;
      return imt <= omt;
      }
    bool operator==(const AddrText &other)
      {
      if (addr != other.addr)
        return false;
      // allow "Untyped" wildcards
      MemoryType imt = (memType == Untyped) ? other.memType : memType;
      MemoryType omt = (other.memType == Untyped) ? imt : other.memType;
      return imt == omt;
      }
    bool operator!=(const AddrText &other) { return !operator==(other); }
    bool operator>=(const AddrText &other) { return !operator<(other); }
    bool operator>(const AddrText &other) { return !operator<=(other); }

  // Attributes
  public:
    // Label Address
    addr_t GetAddress() const { return addr; }
    void SetAddress(addr_t newAddr = 0) { addr = newAddr; }
    // Label Type
    MemoryType GetType() { return memType; }
    void SetType(MemoryType newType = Untyped) { memType = newType; }
    bool IsCode() { return memType == Code; }
    bool IsData() { return memType == Data || memType == Const; }
    bool IsConst() { return memType == Const; }
    bool IsBss() { return memType == Bss; }
    void SetCode() { memType = Code; }
    void SetData() { memType = Data; }
    void SetConst() { memType = Const; }
    bool SetBss() { memType = Bss; }
    // Label Text
    std::string GetText() const { return stext; }
    bool SetText(std::string sNewText = "") { stext = sNewText; return true; }

  protected:
    addr_t addr;
    std::string stext;
    MemoryType memType;

  };

/*****************************************************************************/
/* Label : definition of a label                                             */
/*****************************************************************************/

class Label : public AddrText
  {
  public:
    Label(addr_t addr = 0, MemoryType memType = Code, std::string sLabel = "", bool bUsed = false)
      : AddrText(addr, memType, sLabel), bUsed(bUsed)
      { }
    virtual ~Label() { }
    bool operator<(const Label &other)
      { return AddrText::operator<((const AddrText &)other); }
    bool operator<=(const Label &other)
      { return AddrText::operator<=((const AddrText &)other); }
    bool operator==(const Label &other)
      { return AddrText::operator==((const AddrText &)other); }
    bool operator!=(const Label &other) { return !operator==(other); }
    bool operator>=(const Label &other) { return !operator<(other); }
    bool operator>(const Label &other) { return !operator<=(other); }


  // Attributes
  public:
    // Flag whether it is used
    bool IsUsed() { return bUsed; }
    bool SetUsed(bool bSet = true) { bUsed = bSet; return bSet; }
  
  protected:
    bool bUsed;

  };


/*****************************************************************************/
/* AddrTextArray : an array of texts with address and type                   */
/*****************************************************************************/

class AddrTextArray : public std::vector<AddrText*>
{
  public:
    AddrTextArray(bool bMultipleDefs = false)
      : bMultipleDefs(bMultipleDefs) { }
    virtual ~AddrTextArray(void)
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
        AddrText *p = *cit;
        delete p;
        }
      std::vector<AddrText*>::clear();
      }
    // binary search in sorted array
    iterator find(const AddrText &l)
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
          if (bMultipleDefs)
            {
            // return 1st match for multiple definitions
            while (mid > 0 && *at(mid - 1) == l)
              mid--;
            }
          return begin() + mid;
          }
        }
      return end();
      }
    // convenience - find code and/or data labels
    iterator find(addr_t addr, MemoryType memType = Untyped)
      { return find(AddrText(addr, memType)); }
    // insert into sorted array of labels
    iterator insert(AddrText *pText, bool bAfter = true)
      {
      iterator it;
      // if multiple definitions not allowed, replace old one
      if (!bMultipleDefs)
        {
        it = find(*pText);
        if (it != end())
          {
          if (pText->GetType() == Untyped)
            pText->SetType((*it)->GetType());
          if (pText->GetText().empty())
            pText->SetText((*it)->GetText());
          delete *it;
          *it = pText;
          return it;
          }
        }

      int lo, max = size() - 1, hi = max, mid;
      if (empty() || *pText < *at(0))
        it = begin();
      else if (*pText > *at(hi))
        it = end();
      else
        {
        // binary search for insert pos
        lo = 0;
        it = end();
        while (lo <= hi)
          {
          mid = (hi + lo) / 2;
          if (*at(mid) < *pText)
            lo = mid + 1;
          else if (*at(mid) > *pText)
            hi = mid - 1;
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
                } while (mid > 0 && mid <= max && *at(mid) == *pText);
              }
            break;
            }
          }
        if (mid <= max && *at(mid) < *pText)
          mid++;
        it = (mid > max) ? end() : begin() + mid;
        }
      return std::vector<AddrText*>::insert(it, pText);
      }
    // erase an element
    iterator erase(iterator _Where)
      {
      AddrText *p = *_Where;
      delete p;
      return std::vector<AddrText*>::erase(_Where);
      }
    iterator erase(iterator _First, iterator _Last)
      {
      for (const_iterator cit = _First;
           cit != _Last;
           cit++)
        {
        AddrText *p = *cit;
        delete p;
        }
      return std::vector<AddrText*>::erase(_First, _Last);
      }


  protected:
    bool bMultipleDefs;  // flag whether multiple definitions possible
};

#endif // __Label_h_defined__
