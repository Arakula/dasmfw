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
/* DasmAvr8.h : definition of the DasmAvr8 class                             */
/*****************************************************************************/

#ifndef __DasmAvr8_h_defined__
#define __DasmAvr8_h_defined__

#include "Disassembler.h"

enum
  {
  BusData = BaseBusTypes,               /* Data bus (Hardvard architecture)  */
  BusIO,                                /* I/O Bus                           */
  BusEEPROM,                            /* EEPROM bus                        */

  Avr8BusTypes
  };

/*****************************************************************************/
/* MemAttributeAvr8 : description of a memory cell's attributes for AVR8     */
/*****************************************************************************/

struct MemAttributeAvr8 : public MemAttribute
  {
  unsigned relType: 3;
  unsigned relBus:3;
  adr_t addrRelative;

  enum RelType
    {
    RelLow,
    RelHigh,
    RelLowMinus,
    RelHighMinus,
    RelLow2,
    RelHigh2,
    RelUntyped,
    };
  MemAttributeAvr8(MemoryType memType = Code,
       int cellSize = 1,
       bool bUsed = true,
       Type cellType = UnsignedInt,
       Display display = DefaultDisplay,
       bool breakBefore = false,
       RelType relType = RelUntyped,
       int bus = Avr8BusTypes,
       adr_t rel = NO_ADDRESS)
       : MemAttribute(memType, cellSize, bUsed, cellType, display, breakBefore),
         relType(relType), relBus(bus), addrRelative(rel)
    {
    }

  RelType GetRelative(adr_t &rel, int &bus)
    { rel = addrRelative; bus = relBus; return (RelType)relType; }
  void SetRelative(RelType newType = RelUntyped, adr_t rel = NO_ADDRESS, int newBus = Avr8BusTypes)
    { relType = newType; addrRelative = rel; relBus = newBus; }
  };

/*****************************************************************************/
/* MemAttributeAvr8Handler : memory attribute handler for Avr8               */
/*****************************************************************************/

class MemAttributeAvr8Handler : public MemAttributeHandler
{
public:
    MemAttributeAvr8Handler() { }
    virtual ~MemAttributeAvr8Handler() { }

    virtual bool AddMemory(adr_t addrStart = 0, adr_t memSize = 0, MemoryType memType = Code)
      { return attr.AddMemory(addrStart, memSize, memType); }
    virtual MemoryType GetMemType(adr_t addr)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetMemType() : Untyped; }
    virtual void SetMemType(adr_t addr, MemoryType newType = Code)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetMemType(newType); }
    virtual bool IsCellUsed(adr_t addr)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); return pAttr ? pAttr->IsUsed() : false; }
    virtual void SetCellUsed(adr_t addr, bool bUsed = true)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetUsed(bUsed); }
    virtual MemAttribute::Type GetCellType(adr_t addr)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetCellType() : MemAttributeAvr8::CellUntyped; }
    virtual void SetCellType(adr_t addr, MemAttribute::Type newType)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetCellType(newType); }
    virtual int GetCellSize(adr_t addr)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetSize() : 0; }
    virtual void SetCellSize(adr_t addr, int newSize = 1)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetSize(newSize); }
    virtual MemAttribute::Display GetDisplay(adr_t addr)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetDisplay() : MemAttributeAvr8::CellUndisplayable; }
    virtual void SetDisplay(adr_t addr, MemAttribute::Display newDisp = MemAttribute::DefaultDisplay)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetDisplay(newDisp); }
    virtual bool GetBreakBefore(adr_t addr)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetBreakBefore() : false; }
    virtual void SetBreakBefore(adr_t addr, bool bOn = true)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetBreakBefore(bOn); }
    virtual bool GetForcedAddr(adr_t addr)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetForcedAddr() : false; }
    virtual void SetForcedAddr(adr_t addr, bool bOn = true)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetForcedAddr(bOn); }
    virtual uint32_t GetDisassemblyFlags(adr_t addr, uint8_t mem, Label *plbl)
      { return GetBasicDisassemblyFlags(attr.getat(addr), mem, plbl); }
    // basic access
    virtual size_t size() { return (size_t)attr.size(); }
    virtual adr_t GetStart(int index) { return attr[index].GetStart(); }
    virtual size_t size(int index) { return attr[index].size(); }

    // additional attributes for Avr8
    virtual MemAttributeAvr8::RelType GetRelative(adr_t addr, adr_t &rel, int &bus)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetRelative(rel, bus) : MemAttributeAvr8::RelUntyped; }
    virtual void SetRelative(adr_t addr, MemAttributeAvr8::RelType newType = MemAttributeAvr8::RelUntyped, adr_t rel = NO_ADDRESS, int newBus = Avr8BusTypes)
      { MemAttributeAvr8 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetRelative(newType, rel, newBus); }

protected:
    TMemoryArray <MemAttributeAvr8, MemoryType> attr;

};


/*****************************************************************************/
/* Avr8RegLabel : definition of a register label                             */
/*****************************************************************************/

class Avr8RegLabel : public Label
  {
  public:
    Avr8RegLabel(adr_t addr = 0, string sLabel = "", int nRegNum = 0)
      : Label(addr, Const, sLabel, true), nRegNum(nRegNum)
      { }
  // Attributes
  public:
    void SetRegister(int nNewRegNum = 0) { nRegNum = nNewRegNum; }
    int GetRegister() { return nRegNum; }
  protected:
    int nRegNum;
  };

/*****************************************************************************/
/* Avr8RegLabelArray : an array of register labels                           */
/*****************************************************************************/

class Avr8RegLabelArray : public TAddrTypeArray<Avr8RegLabel>
  {
  public:
    Avr8RegLabelArray()
      : TAddrTypeArray<Avr8RegLabel>(true)
      { }
    Avr8RegLabel *GetFirst(adr_t addr, Avr8RegLabelArray::iterator &it)
      {
      it = find(addr, Const);
      return it != end() ? (Avr8RegLabel *)(*it) : NULL;
      }
    Avr8RegLabel *GetNext(adr_t addr, Avr8RegLabelArray::iterator &it)
      {
      it++;
      return (it != end() && (*it)->GetAddress() == addr) ? (Avr8RegLabel *)(*it) : NULL;
      }
  };

/*****************************************************************************/
/* avrInstructionInfo : Structure for each entry in the instruction set      */
/*****************************************************************************/

struct avrInstructionInfo
  {
  uint8_t mnemonic;
  uint8_t width;
  uint16_t instructionMask;
  int16_t numOperands;
  uint16_t operandMasks[2];
  uint8_t operandTypes[2];
  };

/*****************************************************************************/
/* DasmAvr8 : class for an Atmel AVR8 processor                              */
/*****************************************************************************/

class DasmAvr8 :
  public Disassembler
  {
  public:
    DasmAvr8(void);
    virtual ~DasmAvr8(void);

  // Overrides
  public:
    // return processor long name
    virtual string GetName() { return "ATMEL AVR8"; }
    // return whether big- or little-endian
    virtual Endian GetEndianness() { return LittleEndian; }
    // return number of busses
    virtual int GetBusCount() { return Avr8BusTypes; }
    // return bus width
    virtual int GetBusWidth(int bus = BusCode)
      {
      // return the maximum bus with for AVR8 devices
      return (bus == BusCode) ? 22 :
             (bus == BusData) ? 8 :
             (bus == BusIO) ? 6 :
             (bus == BusEEPROM) ? 16 :
             0; }
    // return highest possible bus address
    virtual cadr_t GetHighestBusAddr(int bus = BusCode) { return highaddr[bus]; }
    // return the default memory type for a bus (used in loading)
    virtual MemoryType GetDefaultMemoryType(int bus = BusCode)
      { return (bus == BusCode) ? Code : Data; }

    // return code bits
    virtual int GetCodeBits() { return 16; }
    // return data bits
    virtual int GetDataBits() { return 8; }

    // return code pointer size in bytes
    virtual int GetCodePtrSize() { return 2; }
    // return highest possible code address
    virtual cadr_t GetHighestCodeAddr() { return highaddr[BusCode]; }
    // return data pointer size in bytes
    virtual int GetDataPtrSize() { return 2; }
    // return highest possible data address
    virtual dadr_t GetHighestDataAddr() { return highaddr[BusData]; }

    // Setup disassembler after construction
    virtual bool Setup();

    // print disassembler-specific info file help
    virtual string InfoHelp();

  // Options handler
  protected:
    int SetAvr8Option(string name, string value);
    string GetAvr8Option(string name);

    // Get/Set additional cell information
    bool IsRelative(adr_t addr, int bus = BusCode)
      {
      adr_t rel; int relBus;
      return memattr[bus] ? (((MemAttributeAvr8Handler *)memattr[bus])->GetRelative(addr, rel, relBus) != MemAttributeAvr8::RelUntyped) : false;
      }
    MemAttributeAvr8::RelType GetRelative(adr_t addr, adr_t &rel, int &relBus, int bus = BusCode)
      { return memattr[bus] ? ((MemAttributeAvr8Handler *)memattr[bus])->GetRelative(addr, rel, relBus) : MemAttributeAvr8::RelUntyped; }
    void SetRelative(adr_t addr, MemAttributeAvr8::RelType newType = MemAttributeAvr8::RelUntyped, adr_t rel = NO_ADDRESS, int relBus = Avr8BusTypes, int bus = BusCode)
      { if (memattr[bus]) ((MemAttributeAvr8Handler *)memattr[bus])->SetRelative(addr, newType, rel, relBus); }

    virtual bool ProcessInfo(string key, string value, adr_t &from, adr_t &to, adr_t &step, vector<TMemoryArray<adr_t>> &remaps, bool bProcInfo = true, int bus = BusCode, int tgtbus = BusCode);

  protected:
    // parse data area for labels
    virtual adr_t ParseData(adr_t addr, int bus = BusCode);
    // parse instruction at given memory address for labels
    virtual adr_t ParseCode(adr_t addr, int bus = BusCode);
    // pass back correct mnemonic and parameters for a label
    virtual bool DisassembleLabel(Label *label, string &slabel, string &smnemo, string &sparm, int bus = BusCode);
    // pass back correct mnemonic and parameters for a DefLabel
    virtual bool DisassembleDefLabel(DefLabel *label, string &slabel, string &smnemo, string &sparm, int bus = BusCode);
    // disassemble data area at given memory address
    virtual adr_t DisassembleData(adr_t addr, adr_t end, uint32_t flags, string &smnemo, string &sparm, int maxparmlen, int bus = BusCode);
    // disassemble instruction at given memory address
    virtual adr_t DisassembleCode(adr_t addr, string &smnemo, string &sparm, int bus = BusCode);
  public:
    // Initialize parsing
    virtual bool InitParse(int bus = BusCode);
    // pass back disassembler-specific state changes before/after a disassembly line
    virtual bool DisassembleChanges(adr_t addr, adr_t prevaddr, adr_t prevsz, bool bAfterLine, vector<LineChange> &changes, int bus = BusCode);

  // Register label handling
  protected:
    bool AddRegLabel(adr_t addr, string sText = "", int nRegNum = 0)
      {
      RegLabels.insert(new Avr8RegLabel(addr, sText, nRegNum));
      return true;
      }
    Avr8RegLabel *GetFirstRegLabel(adr_t addr, Avr8RegLabelArray::iterator &it)
      { return RegLabels.GetFirst(addr, it); }
    Avr8RegLabel *GetNextRegLabel(adr_t addr, Avr8RegLabelArray::iterator &it)
      { return RegLabels.GetNext(addr, it); }

  protected:
    bool LoadAtmelGeneric(FILE *f, string &sLoadType, int bus = BusCode);
    virtual bool LoadFile(string filename, FILE *f, string &sLoadType, int interleave = 1, int bus = BusCode);
    virtual bool String2Number(string s, adr_t &value);
    virtual string Number2String(adr_t value, int nDigits, adr_t addr, int bus = BusCode);
    virtual string Address2String(adr_t addr, int bus = BusCode)
      { return sformat("0x%0*x", (busbits[bus] + 3) / 4, addr); }
    string RegName(int regnum, bool with_label = true)
      {
      static const char *reghdr[] = { "R", "r" };
      if (with_label && CurRegLabel[regnum].size())
        return CurRegLabel[regnum];
      return sformat("%s%d", reghdr[avr_gcc], regnum);
      }
    string IORegName(adr_t addr)
      {
      Label *pLbl = FindLabel(addr, Untyped, BusIO);
      return (pLbl)? pLbl->GetText() : sformat("0x%02x", addr);
      }
    // generate text for an unnamed label
    virtual string UnnamedLabel(adr_t addr, bool bCode, int bus = BusCode)
      {
      if (bAutoLabel)
        {
        // auto-label based on previous constant, if possible
        string s = AutoLabel2String(addr, bCode, bus);
        if (s.size())
          return s;
        }

      const char *cType = "L";
      int bits = 32;
      switch (bus)
        {
        case BusCode:
          cType = bCode ? "Z" : "M";
          bits = busbits[bCode ? (int)BusCode : (int)BusData];
          break;
        case BusData :
          // no code here!
          cType = "MD";
          bits = busbits[bus];
          break;
        case BusIO :
          // no code here!
          cType = "IO";
          bits = busbits[bus];
          break;
        case BusEEPROM :
          // no code here!
          cType = "EP";
          bits = busbits[bus];
          break;
        }
      return sformat("%s%0*X", cType, (bits + 3) / 4, addr);
      }

  protected:
    avrInstructionInfo *LookupInstruction(uint16_t opcode);
    uint16_t GetMaskBits(uint16_t data, uint16_t mask)
      {
      uint16_t result = 0;
      int i, j;
      /* Sweep through mask from bits 0 to 15 */
      for (i = 0, j = 0; i < 16; i++)
        {
        if (mask & (1 << i))            /* if mask bit is set                */
          {
          if (data & (1 << i))          /* if data bit is set                */
            result |= (1 << j);         /* set result bit                    */
          j++;
          }
        }
      return result;
      }
    int32_t DecodeOperand(uint32_t operand, uint8_t operandType);
    uint32_t GetDisassemblyTextFlags(adr_t addr, int bus = BusCode)
      {
      uint32_t flags = GetDisassemblyFlags(addr, bus);
      
      if ((avr_gcc) &&  // Doesn't work in AVRASM2, only in avr-gcc
		  (flags & (SHMF_DATA | SHMF_RMB | SHMF_TXT)) == SHMF_DATA &&
          !(flags & (SHMF_NOTXT | 0xff)))
        {
        switch (*getat(addr, bus))
          {
          case '\t' :
          case '\r' :
          case '\n' :
            flags |= SHMF_TXT;
            break;
          }
        }
      return flags;
      }
	int DisassembleString(adr_t addr, adr_t end, uint32_t flags, string &s, int maxparmlen, int bus = BusCode);

  protected:
    // Enumeration for all types of AVR operands
    enum Avr8OperandTypes
      {
      OpndNone,
      OpndReg,
      OpndRegStartR16,
      OpndRegEvenPair,
      OpndRegEvenPStartR24,
      OpndBranchAddr,
      OpndRelAddr,
      OpndLongAbsAddr,
      OpndLongAbsAddrData,
      OpndIOReg,
      OpndData,
      OpndDESRound,
      OpndBit,
      OpndX,
      OpndXPlus,
      OpndMinusX,
      OpndY,
      OpndYPlus,
      OpndMinusY,
      OpndYPlusQ,
      OpndZ,
      OpndZPlus,
      OpndMinusZ,
      OpndZPlusQ,

      operandTypeCount
      };

    // Avr8 mnemonics
    enum MnemonicsAvr8
      {
      _ill,
      _adc,
      _add,
      _adiw,
      _and,
      _andi,
      _asr,
      _bclr,
      _bld,
      _brbc,
      _brbs,
      _brcc,
      _brcs,
      _break,
      _breq,
      _brge,
      _brhc,
      _brhs,
      _brid,
      _brie,
      _brlo,
      _brlt,
      _brmi,
      _brne,
      _brpl,
      _brsh,
      _brtc,
      _brts,
      _brvc,
      _brvs,
      _bset,
      _bst,
      _call,
      _cbi,
      _cbr,
      _clc,
      _clh,
      _cli,
      _cln,
      _clr,
      _cls,
      _clt,
      _clv,
      _clz,
      _com,
      _cp,
      _cpc,
      _cpi,
      _cpse,
      _dec,
      _des,
      _eicall,
      _eijmp,
      _elpm,
      _eor,
      _fmul,
      _fmuls,
      _fmulsu,
      _icall,
      _ijmp,
      _in,
      _inc,
      _jmp,
      _lac,
      _las,
      _lat,
      _ld,
      _ldd,
      _ldi,
      _lds,
      _lpm,
      _lsl,
      _lsr,
      _mov,
      _movw,
      _mul,
      _muls,
      _mulsu,
      _neg,
      _nop,
      _or,
      _ori,
      _out,
      _pop,
      _push,
      _rcall,
      _ret,
      _reti,
      _rjmp,
      _rol,
      _ror,
      _sbc,
      _sbci,
      _sbi,
      _sbic,
      _sbis,
      _sbiw,
      _sbr,
      _sbrc,
      _sbrs,
      _sec,
      _seh,
      _sei,
      _sen,
      _ser,
      _ses,
      _set,
      _sev,
      _sez,
      _sleep,
      _spm,
      _st,
      _std,
      _sts,
      _sub,
      _subi,
      _swap,
      _tst,
      _wdr,
      _xch,
      // directives
      _d_byte,
      _d_cseg,
      _d_db,
      _d_def,
      _d_device,
      _d_dseg,
      _d_dw,
      _d_equ,
      _d_eseg,
      _d_exit,
      _d_include,
      _d_org,
      _d_port,
      _d_set,
	  _d_string,
	  _d_stringz,

      mnemoAvr8_count
      };

    static OpCode opcodes[mnemoAvr8_count];
    static avrInstructionInfo AVR_Instruction_Set[];
    vector<adr_t> highaddr;
    Avr8RegLabelArray RegLabels;
    string CurRegLabel[32];
    bool dbalign, dbalignchg;
    bool avr_gcc;

  protected:
    // must not be called from constructor!
    virtual MemAttributeHandler *CreateAttributeHandler() { return new MemAttributeAvr8Handler; }
  };

#endif // defined(__DasmAvr8_h_defined__)
