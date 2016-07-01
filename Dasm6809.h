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
       addr_t dirpage = DEFAULT_ADDRESS)
       : MemAttribute(memType, cellSize, bUsed, cellType, display, breakBefore)
    {
    SetDirectPage(dirpage);
    }
  addr_t GetDirectPage() { return (dirpage == (int16_t)-1) ? NO_ADDRESS : (dirpage == (int16_t)-2) ? DEFAULT_ADDRESS : (addr_t)dirpage << 8; }
  void SetDirectPage(addr_t dp) { dirpage = (int16_t)((dp & 0xff) ? dp : dp >> 8); }
  };

/*****************************************************************************/
/* MemAttribute6809Handler : memory attribute handler for 6809               */
/*****************************************************************************/

class MemAttribute6809Handler : public MemAttributeHandler
{
  public:
    MemAttribute6809Handler() { }
    virtual ~MemAttribute6809Handler() { }

    virtual bool AddMemory(addr_t addrStart = 0, addr_t memSize = 0, MemoryType memType = Code)
      { return attr.AddMemory(addrStart, memSize, memType); }
    virtual MemoryType GetMemType(addr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetMemType() : Untyped; }
    virtual void SetMemType(addr_t addr, MemoryType newType = Code)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetMemType(newType); }
    virtual bool IsCellUsed(addr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->IsUsed() : false; }
    virtual void SetCellUsed(addr_t addr, bool bUsed = true)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetUsed(bUsed); }
    virtual MemAttribute::Type GetCellType(addr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetCellType() : MemAttribute6809::CellUntyped; }
    virtual void SetCellType(addr_t addr, MemAttribute::Type newType)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetCellType(newType); }
    virtual int GetCellSize(addr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetSize() : 0; }
    virtual void SetCellSize(addr_t addr, int newSize = 1)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetSize(newSize); }
    virtual MemAttribute::Display GetDisplay(addr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetDisplay() : MemAttribute6809::CellUndisplayable; }
    virtual void SetDisplay(addr_t addr, MemAttribute::Display newDisp = MemAttribute::DefaultDisplay)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetDisplay(newDisp); }
    virtual bool GetBreakBefore(addr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetBreakBefore() : false; }
    virtual void SetBreakBefore(addr_t addr, bool bOn = true)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetBreakBefore(bOn); }
    virtual uint32_t GetDisassemblyFlags(addr_t addr, uint8_t mem, Label *plbl)
      { return GetBasicDisassemblyFlags(attr.getat(addr), mem, plbl); }
    // basic access
    virtual size_t size() { return (size_t)attr.size(); }
    virtual addr_t GetStart(int index) { return attr[index].GetStart(); }
    virtual size_t size(int index) { return attr[index].size(); }
    // additional attributes for 6809
    virtual addr_t GetDirectPage(addr_t addr)
      { MemAttribute6809 *pAttr = attr.getat(addr); return pAttr ? pAttr->GetDirectPage() : DEFAULT_ADDRESS; }
    virtual void SetDirectPage(addr_t addr, addr_t dp)
      { MemAttribute6809 *pAttr = attr.getat(addr); if (pAttr) pAttr->SetDirectPage(dp); }
  protected:
    TMemoryArray <MemAttribute6809, MemoryType> attr;

};


/*****************************************************************************/
/* Dasm6809 : class for a Motorola 6809 Processor                            */
/*****************************************************************************/

class Dasm6809 : public Dasm6800
  {
  public:
    Dasm6809(void);
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
    addr_t GetDirectPage(addr_t addr, int bus = BusCode)
      {
      addr_t dp = memattr[bus] ? ((MemAttribute6809Handler *)memattr[bus])->GetDirectPage(addr) : DEFAULT_ADDRESS;
      if (dp == DEFAULT_ADDRESS)
        dp = dirpage;
      return dp;
      }
    void SetDirectPage(addr_t addr, addr_t dp, int bus = BusCode)
      { if (memattr[bus]) ((MemAttribute6809Handler *)memattr[bus])->SetDirectPage(addr, dp); }

    virtual bool InitParse(int bus = BusCode);
    virtual bool ProcessInfo(string key, string value, addr_t &from, addr_t &to, bool bProcInfo = true, int bus = BusCode, int tgtbus = BusCode);

  protected:
    // parse instruction at given memory address for labels
    virtual addr_t ParseCode(addr_t addr, int bus = BusCode);
    // disassemble instruction at given memory address
    virtual addr_t DisassembleCode(addr_t addr, string &smnemo, string &sparm, int bus = BusCode);
  public:
    // pass back disassembler-specific state changes before/after a disassembly line
    virtual bool DisassembleChanges(addr_t addr, addr_t prevaddr, addr_t prevsz, bool bAfterLine, vector<LineChange> &changes, int bus = BusCode);

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
      // Convenience mnemonics
      _clf,
      _clif,
      _clz,
      _deu,
      _dey,
      _inu,
      _iny,
      _sef,
      _seif,
      _sez,

      mnemo6809_count
      };

    static uint8_t m6809_codes[512];
    static uint8_t m6809_codes10[512];
    static uint8_t m6809_codes11[512];
    static OpCode opcodes[mnemo6809_count - mnemo6800_count];
    static char *os9_codes[0x100];
    addr_t dirpage;

    uint8_t *codes10;
    uint8_t *codes11;
    char **exg_tfr;
    static char reg[];

    bool os9Patch;
    bool useFlex;

  protected:
    // must not be called from constructor!
    virtual MemAttributeHandler *CreateAttributeHandler() { return new MemAttribute6809Handler; }

    virtual addr_t FetchInstructionDetails(addr_t PC, uint8_t &O, uint8_t &T, uint8_t &M, uint16_t &W, int &MI, const char *&I, string *smnemo = NULL);
    virtual addr_t IndexParse(int MI, addr_t pc);
    virtual string IndexString(addr_t &pc);
    void AddFlexLabels();

  };

#endif // __Dasm6809_h_defined__
