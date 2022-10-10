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
/* Dasm6809.h : definition of the Dasm6809 class                             */
/*****************************************************************************/

#ifndef __Dasm6809_h_defined__
#define __Dasm6809_h_defined__

#include "Dasm6800.h"

/*****************************************************************************/
/* MemAttribute6809 : description of a memory cell's attributes for 6809     */
/*****************************************************************************/

struct MemAttribute6809 : public MemAttribute
  {
  int16_t dirpage;                      /* direct page                       */
  MemAttribute6809(MemoryType memType = Code,
       int cellSize = 1,
       bool bUsed = true,
       Type cellType = UnsignedInt,
       Display display = DefaultDisplay,
       bool breakBefore = false,
       adr_t dirpage = DEFAULT_ADDRESS)
       : MemAttribute(memType, cellSize, bUsed, cellType, display, breakBefore)
    {
    SetDirectPage(dirpage);
    }
  adr_t GetDirectPage() { return (dirpage == (int16_t)-1) ? NO_ADDRESS : (dirpage == (int16_t)-2) ? DEFAULT_ADDRESS : (adr_t)dirpage << 8; }
  void SetDirectPage(adr_t dp) { dirpage = (int16_t)((dp & 0xff) ? dp : dp >> 8); }
  };

/*****************************************************************************/
/* MemAttribute6809Handler : memory attribute handler for 6809               */
/*****************************************************************************/

class MemAttribute6809Handler : public MemAttributeHandler
{
  public:
    MemAttribute6809Handler() { }
    virtual ~MemAttribute6809Handler() { }

    virtual bool AddMemory(adr_t addrStart = 0, adr_t memSize = 0, MemoryType memType = Code)
      { return attr.AddMemory(addrStart, memSize, memType); }
    virtual MemoryType GetMemType(adr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetMemType() : Untyped; }
    virtual void SetMemType(adr_t addr, MemoryType newType = Code)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetMemType(newType); }
    virtual bool IsCellUsed(adr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->IsUsed() : false; }
    virtual void SetCellUsed(adr_t addr, bool bUsed = true)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetUsed(bUsed); }
    virtual MemAttribute::Type GetCellType(adr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetCellType() : MemAttribute6809::CellUntyped; }
    virtual void SetCellType(adr_t addr, MemAttribute::Type newType)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetCellType(newType); }
    virtual int GetCellSize(adr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetSize() : 0; }
    virtual void SetCellSize(adr_t addr, int newSize = 1)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetSize(newSize); }
    virtual MemAttribute::Display GetDisplay(adr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetDisplay() : MemAttribute6809::CellUndisplayable; }
    virtual void SetDisplay(adr_t addr, MemAttribute::Display newDisp = MemAttribute::DefaultDisplay)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetDisplay(newDisp); }
    virtual bool GetBreakBefore(adr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetBreakBefore() : false; }
    virtual void SetBreakBefore(adr_t addr, bool bOn = true)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetBreakBefore(bOn); }
    virtual bool GetForcedAddr(adr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetForcedAddr() : false; }
    virtual void SetForcedAddr(adr_t addr, bool bOn = true)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetForcedAddr(bOn); }
    virtual bool GetRelConst(adr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetRelConst() : false; }
    virtual void SetRelConst(adr_t addr, bool bOn = true)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetRelConst(bOn); }
    virtual uint32_t GetDisassemblyFlags(adr_t addr, uint8_t mem, Label *plbl)
      { return GetBasicDisassemblyFlags(attr.getat(addr), mem, plbl); }
    // basic access
    virtual size_t size() { return (size_t)attr.size(); }
    virtual adr_t GetStart(int index) { return attr[index].GetStart(); }
    virtual size_t size(int index) { return attr[index].size(); }
    // additional attributes for 6809
    virtual adr_t GetDirectPage(adr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetDirectPage() : DEFAULT_ADDRESS; }
    virtual void SetDirectPage(adr_t addr, adr_t dp)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetDirectPage(dp); }
  protected:
    TMemoryArray <MemAttribute6809, MemoryType> attr;

};


/*****************************************************************************/
/* REL file structures                                                       */
/*****************************************************************************/
// This is based on an incomplete analysis done by William Gee Jr.,
// posted on the Flex Usergroup mailing list, 2003-09-24

#pragma pack(1)

struct RelCapsuleHeader
  {
  uint8_t signature;                    /* Capsule signature - must be 3     */
  uint8_t flags1;                       /* Flags 1; can be one of:           */
                                        /*   0x10 - relative code capsule    */
                                        /*   0x12 - absolute code capsule    */
                                        /*   0x18 - common capsule           */
  uint8_t datalen[2];                   /* Length of data                    */
  uint8_t unknown1[4];                  /* unknown                           */
  uint8_t extsize[2];                   /* Size of external table            */
  uint8_t progstart[2];                 /* Program start address             */
  uint8_t unknown2[2];                  /* unknown                           */
  uint8_t glbsize[2];                   /* Size of global table              */
  uint8_t unknown3[2];                  /* unknown                           */
  uint8_t namelen[2];                   /* Length of module name             */
  uint8_t flags2;                       /* Flags 2; can be one of:           */
                                        /*   0x80 - start address is given   */
  uint8_t unknown4[3];                  /* unknown                           */
  };
#pragma pack()


/*****************************************************************************/
/* OS9 file structures                                                       */
/*****************************************************************************/

#pragma pack(1)
struct OS9ModuleHeader
  {
  uint8_t bSync[2];    // sync bytes $87,$CD
  uint8_t bModSize[2]; // module size as a word, including CRC
  uint8_t bOffName[2]; // offset of module name relative to bSync
  uint8_t bModType;    // module type / language type
  uint8_t bAttRev;     // attributes / revision level
  uint8_t bHdrChk;     // header checksum
  };
// if (bModType & 0xf0) < 0xa0 :
struct OS9UserModuleHeader : public OS9ModuleHeader
  {
  uint8_t bExecOff[2]; // execution offset
  uint8_t bPSReq[2];   // permanent storage requirement
  };
// if (bModType & 0xf0) == 0xf0 :
struct OS9DeviceDescriptorHeader : public OS9ModuleHeader
  {
  uint8_t bFmNOff[2];  // File Manager Name Offset
  uint8_t bDDNOff[2];  // Device Driver Name Offset
  uint8_t bMode;       // Mode byte
  uint8_t bControl[3]; // Device Controller Address (24 bit)
  uint8_t bTabSize;    // Table Size
  };
// if (bModType & 0xf0) == 0xc0 :
struct OS9ConfigModuleHeader : public OS9ModuleHeader
  {
  uint8_t bRAMTop[3];  // top limit of free RAM
  uint8_t bIrqNo;      // IRQ polling entries
  uint8_t bDevNo;      // device entries
  uint8_t bStupOff[2]; // offset to startup mod. name
  uint8_t bSysdrv[2];  // offset to default drive name
  uint8_t bBoot[2];    // offset to bootstrap module name
  };
// if (bModType & 0xf0) == 0x40 :
struct OS9DataModuleHeader : public OS9ModuleHeader
  {
  uint8_t bDataOff[2]; // offset to data
  uint8_t bSize;       // data size
  };
#pragma pack()


/*****************************************************************************/
/* Dasm6809 : class for a Motorola 6809 Processor                            */
/*****************************************************************************/

class Dasm6809 : public Dasm6800
  {
  public:
    Dasm6809(Application *pApp);
    virtual ~Dasm6809(void);

  // Overrides
  public:
    // return processor long name
    virtual string GetName() { return "Motorola 6809"; }

    // print disassembler-specific info file help
    virtual string InfoHelp();

  // Options handler
  protected:
    int Set6809Option(string name, string value);
    string Get6809Option(string name);

    // Get/Set additional cell information
    virtual adr_t GetDirectPage(adr_t addr, int bus = BusCode)
      {
      adr_t dp = memattr[bus] ? ((MemAttribute6809Handler *)memattr[bus])->GetDirectPage(addr) : DEFAULT_ADDRESS;
      if (dp == DEFAULT_ADDRESS)
        dp = dirpage;
      return dp;
      }
    virtual void SetDirectPage(adr_t addr, adr_t dp, int bus = BusCode)
      { if (memattr[bus]) ((MemAttribute6809Handler *)memattr[bus])->SetDirectPage(addr, dp); }

    virtual bool ProcessInfo(string key, string value, adr_t &from, adr_t &to, adr_t &step, vector<TMemoryArray<adr_t>> &remaps, bool bProcInfo = true, int bus = BusCode, int tgtbus = BusCode);

    bool LoadRel(FILE *f, string &sLoadType);
    bool LoadRelCapsule(FILE *f, bool &isCommon);
    bool LoadOS9(FILE *f, string &sLoadType);
    bool SetupOS9(adr_t loadAddr, uint16_t modsize, OS9ModuleHeader &h);
    bool SetupOS9(adr_t loadAddr, OS9UserModuleHeader &h);
    bool SetupOS9(adr_t loadAddr, OS9DataModuleHeader &h);
    bool SetupOS9(adr_t loadAddr, OS9DeviceDescriptorHeader &h);
    bool SetupOS9(adr_t loadAddr, OS9ConfigModuleHeader &h);
    virtual bool LoadFile(string filename, FILE *f, string &sLoadType, int interleave = 1, int bus = BusCode);

  protected:
    // parse instruction at given memory address for labels
    virtual adr_t ParseCode(adr_t addr, int bus = BusCode);
    // disassemble instruction at given memory address
    virtual adr_t DisassembleCode(adr_t addr, string &smnemo, string &sparm, int bus = BusCode);
  public:
    virtual bool InitParse(int bus = BusCode);
    // pass back disassembler-specific state changes before/after a disassembly line
    virtual bool DisassembleChanges(adr_t addr, adr_t prevaddr, adr_t prevsz, bool bAfterLine, vector<LineChange> &changes, int bus = BusCode);

  protected:

    // additional 6809 addressing modes
    enum AddrMode6809
      {
      _ind = addrmodes6800_count,       /* indexed                           */
      _rew,                             /* relative word                     */
      _r1 ,                             /* tfr/exg mode                      */
      _r2 ,                             /* pul/psh system                    */
      _r3 ,                             /* pul/psh user                      */

      addrmodes6809_count
      };

    // additional 6809 mnemonics
    enum Mnemonics6809
      {
      _abx = mnemo6800_count,
      _addd,
      _andcc,
      _brn,
      _cmpd,
      _cmps,
      _cmpu,
      _cmpx,
      _cmpy,
      _cwai,
      _exg,
      _lbcc,
      _lbcs,
      _lbeq,
      _lbge,
      _lbgt,
      _lbhi,
      _lble,
      _lbls,
      _lblt,
      _lbmi,
      _lbne,
      _lbpl,
      _lbra,
      _lbrn,
      _lbsr,
      _lbvc,
      _lbvs,
      _ldd,
      _ldu,
      _ldy,
      _leas,
      _leau,
      _leax,
      _leay,
      _mul,
      _orcc,
      _pshs,
      _pshu,
      _puls,
      _pulu,
      _reset,
      _sex,
      _std,
      _stu,
      _sty,
      _subd,
      _swi2,
      _swi3,
      _sync,
      _tfr,
      _setdp,
      _os9,
      // Convenience mnemonics
      _clf,
      _clif,
      _clz,
      _deu,
      _dey,
      _inu,
      _iny,
      _pshd,
      _pshx,
      _pshy,
      _puld,
      _pulx,
      _puly,
      _sef,
      _seif,
      _sez,

      mnemo6809_count
      };

    static CMatrixEntry m6809_codes[256];
    static CMatrixEntry m6809_codes10[256];
    static CMatrixEntry m6809_codes11[256];
    static OpCode opcodes[mnemo6809_count - mnemo6800_count];
    static const char *os9_codes[0x100];

    CMatrixEntry *codes10;
    CMatrixEntry *codes11;
    const char **exg_tfr;
    static const char reg[];

    bool os9Patch;
    bool os9Comments;
    bool os9ModHeader;
    bool useFlex;

  protected:
    // must not be called from constructor!
    virtual MemAttributeHandler *CreateAttributeHandler() { return new MemAttribute6809Handler; }

    virtual adr_t FetchInstructionDetails(adr_t PC, uint8_t &instpg, uint8_t &instb, uint8_t &mode, int &mnemoIndex);
    virtual bool SetConvenience(uint8_t instpg, uint16_t u2, string &smnemo, adr_t &PC);
    virtual adr_t IndexParse(int mnemoIndex, adr_t pc, adr_t instaddr = NO_ADDRESS);
    virtual string IndexString(string &smnemo, adr_t &pc);
    void AddFlexLabels();

  };

#endif // __Dasm6809_h_defined__
