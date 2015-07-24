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
/* Disassembler.h : definition of the Disassembler abstract base class       */
/*****************************************************************************/

#ifndef __Disassembler_h_defined__
#define __Disassembler_h_defined__

#include "dasmfw.h"

/*****************************************************************************/
/* MemAttributeHandler : abstract base handler for memory attributes         */
/*****************************************************************************/

class MemAttributeHandler
  {
  public:
    MemAttributeHandler() { }
    virtual ~MemAttributeHandler() { }
    virtual bool AddMemory(addr_t addrStart = 0, addr_t memSize = 0, MemoryType memType = Code) = 0;
    virtual MemoryType GetMemType(addr_t addr) = 0;
    virtual void SetMemType(addr_t addr, MemoryType newType = Code) = 0;
    virtual bool IsCellUsed(addr_t addr) = 0;
    virtual void SetCellUsed(addr_t addr, bool bUsed = true) = 0;
    virtual MemAttribute::Type GetCellType(addr_t addr) = 0;
    virtual void SetCellType(addr_t addr, MemAttribute::Type newType) = 0;
    virtual int GetCellSize(addr_t addr) = 0;
    virtual void SetCellSize(addr_t addr, int newSize = 1) = 0;
    virtual MemAttribute::Display GetDisplay(addr_t addr) = 0;
    virtual void SetDisplay(addr_t addr, MemAttribute::Display newDisp = MemAttribute::Hex) = 0;
    virtual bool GetBreakBefore(addr_t addr) = 0;
    virtual void SetBreakBefore(addr_t addr, bool bOn = true) = 0;
    virtual uint32_t GetDisassemblyFlags(addr_t addr, uint8_t mem, Label *plbl) = 0;
    // basic access
    virtual size_t size() = 0;
    virtual addr_t GetStart(int index) = 0;
    virtual size_t size(int index) = 0;
  };

/*****************************************************************************/
/* BasicMemAttributeHandler : memory attribute handler for basics            */
/*****************************************************************************/

enum BasicDataDisassemblyFlags
  {
  // lowest 8 bits are reserved for (cell size - 1); if the need arises for
  // larger cells, this has to be adjusted
  SHMF_DATA  = 1 << 8,                  /* Data byte (not Code)              */
  SHMF_RMB   = (SHMF_DATA << 1),        /* no defined contents               */
  SHMF_TXT   = (SHMF_RMB  << 1),        /* Textual display possible          */
  SHMF_BREAK = (SHMF_TXT << 1),         /* mandatory break before            */

  SHMF_BasicSetMax = SHMF_BREAK
  };

uint32_t GetBasicDisassemblyFlags(MemAttribute *pAttr, uint8_t mem, Label *plbl);
class BasicMemAttributeHandler : public MemAttributeHandler
{
  public:
    BasicMemAttributeHandler()
      { }
    virtual ~BasicMemAttributeHandler()
      { }

    virtual bool AddMemory(addr_t addrStart = 0, addr_t memSize = 0, MemoryType memType = Code)
      { return attr.AddMemory(addrStart, memSize, memType); }
    virtual MemoryType GetMemType(addr_t addr)
      { MemAttribute *pAttr = attr.getat(addr); return pAttr ? pAttr->GetMemType() : Untyped; }
    virtual void SetMemType(addr_t addr, MemoryType newType = Code)
      { MemAttribute *pAttr = attr.getat(addr); if (pAttr) pAttr->SetMemType(newType); }
    virtual bool IsCellUsed(addr_t addr)
      { MemAttribute *pAttr = attr.getat(addr); return pAttr ? pAttr->IsUsed() : false; }
    virtual void SetCellUsed(addr_t addr, bool bUsed = true)
      { MemAttribute *pAttr = attr.getat(addr); if (pAttr) pAttr->SetUsed(bUsed); }
    virtual MemAttribute::Type GetCellType(addr_t addr)
      { MemAttribute *pAttr = attr.getat(addr); return pAttr ? pAttr->GetCellType() : MemAttribute::CellUntyped; }
    virtual void SetCellType(addr_t addr, MemAttribute::Type newType)
      { MemAttribute *pAttr = attr.getat(addr); if (pAttr) pAttr->SetCellType(newType); }
    virtual int GetCellSize(addr_t addr)
      { MemAttribute *pAttr = attr.getat(addr); return pAttr ? pAttr->GetSize() : 0; }
    virtual void SetCellSize(addr_t addr, int newSize = 1)
      { MemAttribute *pAttr = attr.getat(addr); if (pAttr) pAttr->SetSize(newSize); }
    virtual MemAttribute::Display GetDisplay(addr_t addr)
      { MemAttribute *pAttr = attr.getat(addr); return pAttr ? pAttr->GetDisplay() : MemAttribute::CellUndisplayable; }
    virtual void SetDisplay(addr_t addr, MemAttribute::Display newDisp = MemAttribute::Hex)
      { MemAttribute *pAttr = attr.getat(addr); if (pAttr) pAttr->SetDisplay(newDisp); }
    virtual bool GetBreakBefore(addr_t addr)
      { MemAttribute *pAttr = attr.getat(addr); return pAttr ? pAttr->GetBreakBefore() : false; }
    virtual void SetBreakBefore(addr_t addr, bool bOn = true)
      { MemAttribute *pAttr = attr.getat(addr); if (pAttr) pAttr->SetBreakBefore(bOn); }
    virtual uint32_t GetDisassemblyFlags(addr_t addr, uint8_t mem, Label *plbl)
      { return GetBasicDisassemblyFlags(attr.getat(addr), mem, plbl); }
    // basic access
    virtual size_t size() { return (size_t)attr.size(); }
    virtual addr_t GetStart(int index) { return attr[index].GetStart(); }
    virtual size_t size(int index) { return attr[index].size(); }

  protected:
    TMemoryArray <MemAttribute, MemoryType> attr;

};


/*****************************************************************************/
/* OpCode : a basic opcode definition                                        */
/*****************************************************************************/

struct OpCode
  {
  const char *mne;                      /* mnemonic                          */
  MemoryType memType;                   /* referenced memory type            */
  };

/*****************************************************************************/
/* Disassembler : abstract base class for a processor's disassembler         */
/*****************************************************************************/

class Disassembler
  {
  public:
    Disassembler();
    virtual ~Disassembler();

  // Endianness enumeration
  public:
    enum Endian
      {
      BigEndian,                        /* MSB first                         */
      LittleEndian                      /* LSB first                         */
      };
    enum Architecture
      {
      vonNeumann,                       /* Code and data share the same bus  */
      Harvard                           /* Code and data have separate bus   */
      };

  // Information about the processor's capabilities
  public:
    // return processor long name
    virtual std::string GetName() = 0;
    // return whether big- or little-endian
    virtual Endian GetEndianness() = 0;
    // return architecture type
    virtual Architecture GetArchitecture() { return vonNeumann; }  // most are vN
    // return code bits
    virtual int GetCodeBits() = 0;
    // return code pointer size in bytes
    virtual int GetCodePtrSize() = 0;
    // return lowest possible code address
    virtual caddr_t GetLowestCodeAddr() { return 0; }
    // return highest possible code address
    virtual caddr_t GetHighestCodeAddr() = 0;
    // return data bits
    virtual int GetDataBits() = 0;
    // return data pointer size in bytes
    virtual int GetDataPtrSize() = 0;
    // return lowest possible data address
    virtual daddr_t GetLowestDataAddr() { return 0; }
    // return highest possible data address
    virtual daddr_t GetHighestDataAddr() = 0;

  // Setup disassembler after construction
    virtual bool Setup()
      {
      // default to basic memory attribute handlers
      memattr[0] = CreateAttributeHandler();
      memattr[1] = CreateAttributeHandler();
      return memattr[0] && memattr[1];
      }

  // Generic options handler
  protected:
    typedef int (Disassembler::*PSetter)(std::string lname, std::string value);
    typedef std::string (Disassembler::*PGetter)(std::string lname);
    struct OptionHandler
      {
      std::string name;
      std::string help;
      PSetter setter;
      PGetter getter;
      };
    std::vector<OptionHandler *> options;
    bool AddOption(std::string name, std::string help, PSetter setter, PGetter getter);
    // 
    int DisassemblerSetOption(std::string lname, std::string value);
    std::string DisassemblerGetOption(std::string lname);
  public:
    int GetOptionCount() { return options.size(); }
    int FindOption(std::string name);
    std::string GetOptionName(int idx) { return options[idx]->name; }
    std::string GetOptionHelp(int idx) { return options[idx]->help; }
    int SetOption(int idx, std::string value) { return ((*this).*(options[idx]->setter))(options[idx]->name, value); }
    int SetOption(std::string name, std::string value);
    std::string GetOption(int idx) { return ((*this).*(options[idx]->getter))(options[idx]->name); }
    std::string GetOption(std::string name);
    // print disassembler-specific info file help
    virtual std::string InfoHelp() { return ""; }
    // global typed options
    addr_t GetBegin() { return begin; }
    addr_t GetEnd() { return end; }
    addr_t GetOffset() { return offset; }

  // Memory access
  public:
    // Add memory area
    virtual bool AddMemory(addr_t addrStart = 0, addr_t memSize = 0, MemoryType memType = Code, uint8_t *contents = NULL, bool bDataBus = false)
      {
      if (!memory[bDataBus].AddMemory(addrStart, memSize, memType, contents))
        return false;
      return memattr[bDataBus] && memattr[bDataBus]->AddMemory(addrStart, memSize, memType);
      }
    template<class T> T inline getat(addr_t addr, bool bDataBus = false)
      {
      T val;
      if (!getat(addr, (uint8_t *)&val, sizeof(val), bDataBus))
        return 0;
      return val;
      }
    // Direct access to memory areas (... be careful!)
    int GetMemoryArrayCount(bool bDataBus = false) { return memory[bDataBus].size(); }
    TMemory<uint8_t, MemoryType> &GetMemoryArray(int index, bool bDataBus = false) { return memory[bDataBus].at(index); }
    int GetMemAttrArrayCount(bool bDataBus = false) { return memattr[bDataBus] ? memattr[bDataBus]->size() : 0; }
    MemAttributeHandler *GetMemoryAttributeHandler(bool bDataBus = false) { return memattr[bDataBus]; }
    // find memory area index for a given address
    addr_t GetMemIndex(addr_t addr, bool bDataBus = false)
      { return memory[bDataBus].GetMemIndex(addr); }
    // get next loaded memory address
    addr_t GetNextAddr(addr_t addr, bool bDataBus = false)
      {
      addr_t nextAddr = addr + 1;
      while (!IsCellUsed(nextAddr, bDataBus))
        {
        uint8_t *pNext = getat(nextAddr, bDataBus);
        if (!pNext)
          {
          for (int i = 0; i < GetMemoryArrayCount(); i++)
            {
            addr_t nextStart = GetMemoryArray(i, bDataBus).GetStart();
            if (nextStart > nextAddr)
              {
              pNext = getat(nextStart, bDataBus);
              nextAddr = nextStart;
              break;
              }
            if (!pNext)
              return NO_ADDRESS;
            }
          }
        else
          nextAddr++;
        }
      return nextAddr;
      }

    // Get/Set memory byte
    int8_t GetSByte(addr_t addr, bool bDataBus = false)
      { return (int8_t)*getat(addr, bDataBus); }
    uint8_t GetUByte(addr_t addr, bool bDataBus = false)
      { uint8_t *pb = getat(addr, bDataBus); return pb ? *pb : 0; }
    bool SetSByte(addr_t addr, int8_t val, bool bDataBus = false)
      { return setat(addr, (uint8_t)val, bDataBus); }
    bool SetUByte(addr_t addr, uint8_t val, bool bDataBus = false)
      { return setat(addr, val, bDataBus); }
    // Get/Set memory word
    int16_t GetSWord(addr_t addr, bool bDataBus = false)
      { return getat<int16_t>(addr, bDataBus); }
    uint16_t GetUWord(addr_t addr, bool bDataBus = false)
      { return getat<uint16_t>(addr, bDataBus); }
    bool SetSWord(addr_t addr, int16_t val, bool bDataBus = false)
      { return setat(addr, (uint8_t *)&val, sizeof(val), bDataBus); }
    bool SetUWord(addr_t addr, uint16_t val, bool bDataBus = false)
      { return setat(addr, (uint8_t *)&val, sizeof(val), bDataBus); }
    // Get/Set memory dword
    int32_t GetSDWord(addr_t addr, bool bDataBus = false)
      { return getat<int32_t>(addr, bDataBus); }
    uint32_t GetUDWord(addr_t addr, bool bDataBus = false)
      { return getat<uint32_t>(addr, bDataBus); }
    bool SetSDWord(addr_t addr, int32_t val, bool bDataBus = false)
      { return setat(addr, (uint8_t *)&val, sizeof(val), bDataBus); }
    bool SetUDWord(addr_t addr, uint32_t val, bool bDataBus = false)
      { return setat(addr, (uint8_t *)&val, sizeof(val), bDataBus); }
    // Get/Set memory qword
    int64_t GetSQWord(addr_t addr, bool bDataBus = false)
      { return getat<int64_t>(addr, bDataBus); }
    uint64_t GetUQWord(addr_t addr, bool bDataBus = false)
      { return getat<uint64_t>(addr, bDataBus); }
    bool SetSQWord(addr_t addr, int64_t val, bool bDataBus = false)
      { return setat(addr, (uint8_t *)&val, sizeof(val), bDataBus); }
    bool SetUQWord(addr_t addr, uint64_t val, bool bDataBus = false)
      { return setat(addr, (uint8_t *)&val, sizeof(val), bDataBus); }
    // Get/Set 32-bit floating-point IEEE 854 value
    float GetFloat(addr_t addr, bool bDataBus = false)
      { return getat<float>(addr, bDataBus); }
    bool SetFloat(addr_t addr, float val, bool bDataBus = false)
      { return setat(addr, (uint8_t *)&val, sizeof(val), bDataBus); }
    // Get/Set 64-bit floating-point IEEE 854 value
    double GetDouble(addr_t addr, bool bDataBus = false)
      { return getat<double>(addr, bDataBus); }
    bool SetDouble(addr_t addr, double val, bool bDataBus = false)
      { return setat(addr, (uint8_t *)&val, sizeof(val), bDataBus); }
    // Get/set delimited ASCII string
    std::string GetString(addr_t addr, char cTerm = '\0', bool bDataBus = false)
      {
      std::string s;
      char c;
      while ((c = (char)getat(addr++)) != cTerm)
        s += c;
      return s;
      }
    bool SetString(addr_t addr, std::string s, char cTerm = '\0', bool bDataBus = false)
      {
      if (setat(addr, (uint8_t *)s.c_str(), s.size(), bDataBus))
        return setat(addr + s.size(), (uint8_t)cTerm, bDataBus);
      return false;
      }
    // in theory, strings with leading length byte/word might come up at some time
    // we'll deal with it in time.

    // Get/Set cell information
    MemoryType GetMemType(addr_t addr, bool bDataBus = false)
      { return memattr[bDataBus] ? memattr[bDataBus]->GetMemType(addr) : Untyped; }
    void SetMemType(addr_t addr, MemoryType newType = Code, bool bDataBus = false)
      { if (memattr[bDataBus]) memattr[bDataBus]->SetMemType(addr, newType); }
    bool IsCellUsed(addr_t addr, bool bDataBus = false)
      { return memattr[bDataBus] ? memattr[bDataBus]->IsCellUsed(addr) : false; }
    void SetCellUsed(addr_t addr, bool bUsed = true, bool bDataBus = false)
      { if (memattr[bDataBus]) memattr[bDataBus]->SetCellUsed(addr, bUsed); }
    MemAttribute::Type GetCellType(addr_t addr, bool bDataBus = false)
      { return memattr[bDataBus] ? memattr[bDataBus]->GetCellType(addr) : MemAttribute::CellUntyped; }
    void SetCellType(addr_t addr, MemAttribute::Type newType, bool bDataBus = false)
      { if (memattr[bDataBus]) memattr[bDataBus]->SetCellType(addr, newType); }
    int GetCellSize(addr_t addr, bool bDataBus = false)
      { return memattr[bDataBus] ? memattr[bDataBus]->GetCellSize(addr) : 0; }
    void SetCellSize(addr_t addr, int newSize = 1, bool bDataBus = false)
      { if (memattr[bDataBus]) memattr[bDataBus]->SetCellSize(addr, newSize); }
    MemAttribute::Display GetDisplay(addr_t addr, bool bDataBus = false)
      { return memattr[bDataBus] ? memattr[bDataBus]->GetDisplay(addr) : MemAttribute::CellUndisplayable; }
    void SetDisplay(addr_t addr, MemAttribute::Display newDisp = MemAttribute::Hex, bool bDataBus = false)
      { if (memattr[bDataBus]) memattr[bDataBus]->SetDisplay(addr, newDisp); }
    bool GetBreakBefore(addr_t addr, bool bDataBus = false)
      { return memattr[bDataBus] ? memattr[bDataBus]->GetBreakBefore(addr) : false; }
    void SetBreakBefore(addr_t addr, bool bOn = true, bool bDataBus = false)
      { if (memattr[bDataBus]) memattr[bDataBus]->SetBreakBefore(addr, bOn); }
    // get/set default cell display format
    MemAttribute::Display GetDisplay() { return defaultDisplay; }
    void SetDisplay(MemAttribute::Display newDisp) { defaultDisplay = newDisp; }
    // Get Flags for disassembly of data areas
    uint32_t GetDisassemblyFlags(addr_t addr, bool bDataBus = false)
      {
      uint8_t *mem = getat(addr, bDataBus);
      if (!mem) return 0;
      Label *plbl = FindLabel(addr, Untyped, bDataBus);
      return memattr[bDataBus] ? memattr[bDataBus]->GetDisassemblyFlags(addr, *mem, plbl) : 0;
      }

    // convenience functionality for the above
    bool IsCode(addr_t addr, bool bDataBus = false) { return GetMemType(addr, bDataBus) == Code; }
    bool IsData(addr_t addr, bool bDataBus = false) { return GetMemType(addr, bDataBus) == Data; }
    bool IsConst(addr_t addr, bool bDataBus = false) { return GetMemType(addr, bDataBus) == Const; }
    bool IsBss(addr_t addr, bool bDataBus = false) { return GetMemType(addr, bDataBus) == Bss; }
    bool IsBinary(addr_t addr, bool bDataBus = false) { return GetDisplay(addr, bDataBus) == MemAttribute::Binary; }
    bool IsChar(addr_t addr, bool bDataBus = false) { return GetDisplay(addr, bDataBus) == MemAttribute::Char; }
    bool IsOctal(addr_t addr, bool bDataBus = false) { return GetDisplay(addr, bDataBus) == MemAttribute::Octal; }
    bool IsDecimal(addr_t addr, bool bDataBus = false) { return GetDisplay(addr, bDataBus) == MemAttribute::Decimal; }
    bool IsHex(addr_t addr, bool bDataBus = false) { return GetDisplay(addr, bDataBus) == MemAttribute::Hex; }
    bool IsSigned(addr_t addr, bool bDataBus = false) { return GetCellType(addr, bDataBus) == MemAttribute::SignedInt; }
    bool IsUnsigned(addr_t addr, bool bDataBus = false) { return GetCellType(addr, bDataBus) == MemAttribute::UnsignedInt; }
    bool IsFloat(addr_t addr, bool bDataBus = false) { return GetCellType(addr, bDataBus) == MemAttribute::Float; }

  // Relative handling
  public:
    bool AddRelative(addr_t addr, addr_t relsize = 1,  addr_t *contents = NULL, bool bDataBus = false)
      { return Relatives[bDataBus].AddMemory(addr, relsize, 0, contents); }
    addr_t GetRelative(addr_t addr, bool bDataBus = false)
      { addr_t *paddr = Relatives[bDataBus].getat(addr); return paddr ? *paddr : 0; }
    void SetRelative(addr_t addr, addr_t rel, bool bDataBus = false)
      { addr_t *paddr = Relatives[bDataBus].getat(addr); if (paddr) *paddr = rel; }

  // Phase handling
  public:
    bool AddPhase(addr_t addr, addr_t phaseSize = 1, addr_t phase = 0, bool bDataBus = false)
      { return Phases[bDataBus].AddMemory(addr, phaseSize, phase); }
    TMemory<addr_t, addr_t> *FindPhase(addr_t addr, bool bDataBus = false)
      { return Phases[bDataBus].FindMem(addr); }
    addr_t GetPhase(addr_t addr, bool bDataBus = false)
      { TMemory<addr_t, addr_t> *pArea = FindPhase(addr, bDataBus); return pArea ? pArea->GetType() : NO_ADDRESS; }
    void SetPhase(addr_t addr, addr_t phase = NO_ADDRESS, bool bDataBus = false)
      { addr_t *paddr = Phases[bDataBus].getat(addr); if (paddr) *paddr = phase; }
    protected:
      addr_t PhaseInner(addr_t value, addr_t addr, bool bDataBus = false)
        {
        // "phase" an address if it's inside the current range
        TMemory<addr_t, addr_t> *pArea = Phases[bDataBus].FindMem(addr);
        if (pArea)
          {
          addr_t phStart = pArea->GetType();
          addr_t phEnd = phStart + pArea->size() - 1;
          addr_t offset = pArea->at(addr);
          if (value >= phStart && value <= phEnd || offset != NO_ADDRESS)
            {
            addr_t aStart = pArea->GetStart();
            if (offset == NO_ADDRESS)
              offset = phStart - aStart;
            else if (offset == DEFAULT_ADDRESS)
              offset = 0;
            else
              offset -= aStart;
            value -= offset;
            }
          }
        return value;
        }
      addr_t DephaseOuter(addr_t value, addr_t addr, bool bDataBus = false)
        {
        // "de-phase" an address if it's outside the current range
        TMemory<addr_t, addr_t> *pArea = Phases[bDataBus].FindMem(addr);
        if (pArea)
          {
          addr_t aStart = pArea->GetStart();
          addr_t aEnd = pArea->GetEnd();
          addr_t offset = pArea->at(addr);
          if (value < aStart || value > aEnd || offset != NO_ADDRESS)
            {
            addr_t phStart = pArea->GetType();
            if (offset == NO_ADDRESS)
              offset = aStart - phStart;
            else if (offset == DEFAULT_ADDRESS)
              offset = 0;
            else
              offset -= phStart;
            value -= offset;
            }
          }
        return value;
        }

  // Label handling
  public:
    bool AddLabel(addr_t addr, MemoryType memType = Code, std::string sLabel = "", bool bUsed = false, bool bDataBus = false)
      {
      Label *pLbl = FindLabel(addr, Untyped, bDataBus);
      MemoryType oType = pLbl ? pLbl->GetType() : memType;
      if (pLbl && oType != Untyped && oType != memType)
        memType = pLbl->GetType();
      if (!bUsed)
        {
        if (pLbl)
          bUsed = pLbl->IsUsed();
        }
      Labels[bDataBus].insert(new Label(addr, memType, sLabel, bUsed));
      return true;
      }
    Label *FindLabel(addr_t addr, MemoryType memType = Untyped, bool bDataBus = false)
      {
      AddrTextArray::iterator p = Labels[bDataBus].find(addr, memType);
      return (p != Labels[bDataBus].end()) ? (Label *)(*p) : NULL;
      }
    int GetLabelCount(bool bDataBus = false) { return Labels[bDataBus].size(); }
    Label *LabelAt(int index, bool bDataBus = false) { return (Label *)Labels[bDataBus].at(index); }
    void RemoveLabelAt(int index, bool bDataBus = false) { Labels[bDataBus].erase(Labels[bDataBus].begin() + index); }
    // convenience functionality for the above
    std::string GetLabel(addr_t addr, MemoryType memType = Untyped, bool bDataBus = false) { Label *p = FindLabel(addr, memType); return p ? p->GetText() : ""; }
    bool IsLabel(addr_t addr, bool bDataBus = false)  { return !!FindLabel(addr, Untyped, bDataBus); }
    bool IsCLabel(addr_t addr, bool bDataBus = false) { Label *pLabel = FindLabel(addr, Untyped); return pLabel ? pLabel->IsCode() : false; }
    bool IsDLabel(addr_t addr, bool bDataBus = false) { Label *pLabel = FindLabel(addr, Untyped); return pLabel ? pLabel->IsData() : false; }

  // File handling
  public:
    // load a code file; interleave can be >1 for interleaved Low/High EPROM pairs, for example
    bool Load(std::string filename, std::string &sLoadType, int interleave = 1);
    // process an info file line
    virtual bool ProcessInfo(std::string key, std::string value, addr_t &from, addr_t &to, bool bProcInfo = true, bool bDataBus = false) { return false; }

  // the real disassembler activities
  protected:
    // parse data area for labels
    virtual addr_t ParseData(addr_t addr, bool bDataBus = false) = 0;
    // parse instruction at given memory address for labels
    virtual addr_t ParseCode(addr_t addr, bool bDataBus = false) = 0;
    // disassemble data area at given memory address
    virtual addr_t DisassembleData(addr_t addr, addr_t end, uint32_t flags, std::string &smnemo, std::string &sparm, int maxparmlen, bool bDataBus = false) = 0;
    // disassemble instruction at given memory address
    virtual addr_t DisassembleCode(addr_t addr, std::string &smnemo, std::string &sparm, bool bDataBus = false) = 0;
  // globally accessible dispatchers for the above
  public:
    // Initialize parsing
    virtual bool InitParse(bool bDataBus = false)
      {
      if (!bDataBus && load != NO_ADDRESS && bLoadLabel)
        AddLabel(load, Code, "", true);
      return true;
      }
    // Parse memory area for labels
    addr_t Parse(addr_t addr, bool bDataBus = false)
      { return IsCode(addr, bDataBus) ? ParseCode(addr, bDataBus) : ParseData(addr, bDataBus); }
    // Disassemble a line in the memory area
    addr_t Disassemble(addr_t addr, std::string &smnemo, std::string &sparm, int maxparmlen, bool bDataBus = false)
      {
      if (IsCode(addr, bDataBus)) return DisassembleCode(addr, smnemo, sparm, bDataBus);
      uint32_t flags = 0;
      addr_t end = GetConsecutiveData(addr, flags, maxparmlen, bDataBus);
      return DisassembleData(addr, end, flags, smnemo, sparm, maxparmlen, bDataBus);
      }
    // pass back correct mnemonic and parameters for a label
    virtual bool DisassembleLabel(addr_t addr, std::string &smnemo, std::string &sparm, bool bDataBus = false)
      { return false; } // no changes in base implementation
    // pass back disassembler-specific state changes before/after a disassembly line
    struct LineChange
      {
      std::string smnemo;
      std::string sparm;
      };
    virtual bool DisassembleChanges(addr_t addr, addr_t prevaddr, addr_t prevsz, bool bAfterLine, std::vector<LineChange> &changes, bool bDataBus = false)
      { return changes.size() != 0; } // no additional changes in base implementation

  protected:
    static const Endian prgEndian;
    // memory[0]: instruction bus, memory[1]: data bus, if separate
    MemoryArray memory[2];
    // memattr[0]: instruction bus, memattr[1]: data bus, if separate
    MemAttributeHandler *memattr[2];
    // Labels[0]: instruction bus labels, Labels[1]; data bus labels, if separate
    AddrTextArray Labels[2];
    // Relatives[0]: instruction bus relatives, Relatives[1]; data bus relatives, if separate
    TMemoryArray<addr_t> Relatives[2];
    // Phases[0]: instruction bus phases, Phases[1]; data bus phases, if separate
    TMemoryArray<addr_t, addr_t> Phases[2];
    // begin / end / load (i.e., entry point) address
    addr_t begin, end, load, offset;
    // flag whether to honor load address
    bool bLoadLabel;
    // default display format
    MemAttribute::Display defaultDisplay;
    // disassembler-specific comment start character
    std::string commentStart;
    // disassembler-specific label delimiter
    std::string labelDelim;
    // parsing radix (default 10)
    int pbase;

    std::vector<OpCode> mnemo;

    // must not be called from constructor!
    virtual MemAttributeHandler *CreateAttributeHandler() { return new BasicMemAttributeHandler; }
    //
    TMemory<uint8_t, MemoryType> *FindMem(addr_t addr, bool bDataBus = false)
      { return memory[bDataBus].FindMem(addr); }
    // fetch byte at a given address
    uint8_t *getat(addr_t addr, bool bDataBus = false)
      { return memory[bDataBus].getat(addr); }
    // fetch multiple bytes with endianness correction
    bool getat(addr_t addr, uint8_t *val, addr_t len, bool bDataBus = false) 
      { return memory[bDataBus].getat(addr, val, len, (GetEndianness() != prgEndian)); }
    // write byte at a given address
    bool setat(addr_t addr, uint8_t val, bool bDataBus = false) 
      { return memory[bDataBus].setat(addr, val); }
    // write multiple bytes with endianness correction
    bool setat(addr_t addr, uint8_t *val, addr_t len, bool bDataBus = false) 
      { return memory[bDataBus].setat(addr, val, len, (GetEndianness() != prgEndian)); }

    // load opened file (overridable for specific file types)
    virtual bool LoadFile(std::string filename, FILE *f, std::string &sLoadType, int interleave = 1);
    bool LoadIntelHex(std::string filename, FILE *f, std::string &sLoadType, int interleave = 1);
    bool LoadMotorolaHex(std::string filename, FILE *f, std::string &sLoadType, int interleave = 1);
    bool LoadBinary(std::string filename, FILE *f, std::string &sLoadType, int interleave = 1);

    // calculate consecutive data range (i.e., same type for all)
    addr_t GetConsecutiveData(addr_t addr, uint32_t &flags, int maxparmlen, bool bDataBus = false)
      {
      addr_t end, maxaddr = bDataBus ? GetHighestDataAddr() : GetHighestCodeAddr();
                                        /* get flags for current byte        */
      flags = GetDisassemblyFlags(addr, bDataBus) &
              (~SHMF_BREAK);            /* without break flag                */
      // safety fuse - process no more than maxparmlen at a time unless it's
      // RMB. This may still be too much, but should not be too far off.
      if (!(flags & SHMF_RMB) &&
          addr + (addr_t)maxparmlen > addr &&
          addr + (addr_t)maxparmlen <= maxaddr)
        maxaddr = addr + (addr_t)maxparmlen - 1;

      for (end = addr + 1;              /* find end of block                 */
           end > addr && end <= maxaddr;
           end++)
        {
        uint32_t fEnd = GetDisassemblyFlags(end, bDataBus);
        if (fEnd != flags)
          break;
        }
      if (flags & 0xff)                 /* if not 1-sized,                   */
        {
        int dsz = (int)(flags & 0xff) + 1;
        addr_t rest = (end - addr) % dsz;
        if (rest)                       /* don't use incomplete last item    */
          end -= rest;
        if (end == addr)                /* if there's nothing left           */
          end = addr + dsz;             /* use at least 1 item               */
        // NB: defining n-byte fields starting at less than (n-1) bytes
        // before the end of the highest memory address will produce garbage,
        // but... well... GIGO. Should be caught by info file parser anyway.
        }
      return end;                       /* pass back last + 1                */
      }

  public:
    // convert a string to an integer number, (dis)assembler-specific
    virtual bool String2Number(std::string s, addr_t &value)
      {
      // Only thing that should always work...
      // C-style number strings should be universal
      std::string sFmt = s.substr(0, 2);
      if (sFmt == "0x")
        return (sscanf(s.substr(2).c_str(), "%x", &value) == 1);
      // octal (leading 0), however, isn't.
      // So interpret any pure number as having the default base.
      char *end = NULL;
      value = strtoul(s.c_str(), &end, pbase);
      return (s.c_str() != end);
      }
    virtual int String2Range(std::string s, addr_t &from, addr_t &to)
      {
      from = to = NO_ADDRESS;
      std::string::size_type midx = s.find('-');
      if (midx == 0)
        {
        midx = s.substr(1).find('-');
        if (midx != s.npos) midx++;
        }
      if (midx != s.npos)
        return (int)String2Number(s.substr(0, midx), from) +
               (int)String2Number(s.substr(midx + 1), to);
      else
        return (int)String2Number(s, from);
      }
    // convert a string to a floating-point number, (dis)assembler-specific
    virtual bool String2Double(std::string s, double &value)
      {
      // Only thing that should always work...
      value = 0.;
      return (sscanf(s.c_str(), "%lf", &value) == 1);
      }
    // convert a number to a string
    virtual std::string Number2String(addr_t value, int nDigits, addr_t addr, bool bDataBus = false)
      {
      // Only thing that should always work...
      if (pbase == 16)
        return sformat("0x%x", value);
      else
        return sformat("%d", value);
      }
    virtual std::string SignedNumber2String(int32_t value, int nDigits, addr_t addr, bool bDataBus = false)
      {
      std::string s;
      // specialization for things that have to be signed in any case
      if (value < 0)
        {
        s = "-";
        value = -value;
        }
      return s + Number2String((addr_t)value, nDigits, addr, bDataBus);
      }
    virtual std::string Label2String(addr_t value, bool bUseLabel, addr_t addr, bool bDataBus = false);
    // generate text for an unnamed label
    virtual std::string UnnamedLabel(addr_t addr, bool bCode, bool bDataBus = false)
      {
      return sformat("%c%s%0*X",
                     bCode ? 'Z' : 'M', 
                     bDataBus ? "D" : "",
                     (GetCodeBits() > GetDataBits() ? GetCodeBits() : GetDataBits()) / 4,
                     addr);
      }
  };

addr_t GetHex(FILE *f, int nChars, uint8_t *pchks = NULL);

#endif // __Disassembler_h_defined__
