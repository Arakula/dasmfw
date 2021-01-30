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
/* Dasm650X.h : definition of the Dasm650X class                             */
/*****************************************************************************/

#ifndef __Dasm6500_h_defined__
#define __Dasm6500_h_defined__

#include "Disassembler.h"

/*****************************************************************************/
/* Dasm650X : base class for a MOS Technology 650x processor                 */
/*****************************************************************************/

class Dasm650X :
  public Disassembler
  {
  public:
    Dasm650X(void);
    virtual ~Dasm650X(void);

  // Overrides
  public:
    // return processor long name
    virtual string GetName() { return "MOS 650x"; }
    // return whether big- or little-endian
    virtual Endian GetEndianness() { return LittleEndian; }
    // return bus width
    virtual int GetBusWidth(int bus = BusCode) { (void)bus; return 16; }
    // return code bits
    virtual int GetCodeBits() { return 16; }
    // return code pointer size in bytes
    virtual int GetCodePtrSize() { return 2; }
    // return highest possible code address
    virtual cadr_t GetHighestCodeAddr() { return 0xffff; }
    // return data bits
    virtual int GetDataBits() { return 8; }
    // return data pointer size in bytes
    virtual int GetDataPtrSize() { return 2; }
    // return highest possible data address
    virtual dadr_t GetHighestDataAddr() { return 0xffff; }

  // Options handler
  protected:
    int Set6500Option(string name, string value);
    string Get6500Option(string name);

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
    // Get Flags for disassembly of data areas
    virtual uint32_t GetDisassemblyFlags(adr_t addr, int bus = BusCode);

    // Get/Set additional cell information
    virtual adr_t GetDirectPage(adr_t addr, int bus = BusCode)
      { (void)addr; (void)bus; return 0; }
    virtual void SetDirectPage(adr_t addr, adr_t dp, int bus = BusCode)
      { (void)addr; (void)dp; (void)bus; }

  public:
    // Initialize parsing
    virtual bool InitParse(int bus = BusCode);
    // pass back disassembler-specific state changes before/after a disassembly line
    virtual bool DisassembleChanges(adr_t addr, adr_t prevaddr, adr_t prevsz, bool bAfterLine, vector<LineChange> &changes, int bus = BusCode);


  protected:
    virtual bool String2Number(string s, adr_t &value);
    virtual string Number2String(adr_t value, int nDigits, adr_t addr, int bus = BusCode);
    virtual string Address2String(adr_t addr, int bus = BusCode)
      { (void)bus; return sformat("$%04X", addr); }
    virtual adr_t FetchInstructionDetails(adr_t PC, uint8_t &O, uint8_t &T, uint8_t &M, uint16_t &W, int &MI, const char *&I, string *smnemo = NULL);

  protected:
    // 6500 addressing modes
    enum AddrMode6500
      {
      _nom,                             /* no mode                           */
      _imp,                             /* inherent/implied (no arguments)   */
      _acc,                             /* accumulator                       */
      _imm,                             /* # immediate                       */
      _abs,                             /* absolute                          */
      _abx,                             /* absolute,X                        */
      _aby,                             /* absolute,Y                        */
      _zpg,                             /* zero page                         */
      _zpx,                             /* zero page,X                       */
      _zpy,                             /* zero page,Y                       */
      _ind,                             /* indirect                          */
      _idx,                             /* (ind,X)                           */
      _idy,                             /* (ind),Y                           */
      _rel,                             /* relative                          */

      addrmodes6500_count,
      };

    // 6500 mnemonics
    enum Mnemonics6500
      {
      _ill,                             /* illegal                           */

      _adc,
      _and,
      _asl,
      _bcc,
      _bcs,
      _beq,
      _bit,
      _bmi,
      _bne,
      _bpl,
      _brk,
      _bvc,
      _bvs,
      _clc,
      _cld,
      _cli,
      _clv,
      _cmp,
      _cpx,
      _cpy,
      _dec,
      _dex,
      _dey,
      _eor,
      _inc,
      _inx,
      _iny,
      _jmp,
      _jsr,
      _lda,
      _ldx,
      _ldy,
      _lsr,
      _nop,
      _ora,
      _pha,
      _php,
      _pla,
      _plp,
      _rol,
      _ror,
      _rti,
      _rts,
      _sbc,
      _sec,
      _sed,
      _sei,
      _sta,
      _stx,
      _sty,
      _tax,
      _tay,
      _tsx,
      _txa,
      _txs,
      _tya,

      mnemo6500_count
      };

    static uint8_t m6500_codes[512];

    uint8_t *codes;
    static const char *bit_r[];
    static const char *block_r[];
    static OpCode opcodes[mnemo6500_count];
    adr_t dirpage;

    bool useConvenience;
    bool useFCC;
    bool closeCC;
    bool forceExtendedAddr;
    bool forceDirectAddr;
    bool useDPLabels;
  };


/*****************************************************************************/
/* Dasm6501 : class for a MOS Technology 6501/2 processor                    */
/*****************************************************************************/

class Dasm6501 :
  public Dasm650X
  {
  public:
    Dasm6501(void);

  // Options handler
  protected:
    int Set6501Option(string name, string value);
    string Get6501Option(string name);

  protected:
    virtual adr_t FetchInstructionDetails(adr_t PC, uint8_t &O, uint8_t &T, uint8_t &M, uint16_t &W, int &MI, const char *&I, string *smnemo = NULL);

  protected:
    // 6501 addressing modes
    enum AddrMode6501
      {
      _und = 0x80                       /* additional flag for "undefined"   */
      };

    // 6501 additional mnemonics
    enum Mnemonics6501
      {
      _lax = mnemo6500_count,  // A,X:={adr}
      _dcp,                    // {adr}:={adr}-1 A-{adr}
      _isc,                    // {adr}:={adr}+1 A:=A-{adr}
      _rla,                    // {adr}:={adr}rol A:=A and {adr}
      _rra,                    // {adr}:={adr}ror A:=A adc {adr}
      _sax,                    // {adr}:=A&X
      _slo,                    // {adr}:={adr}*2 A:=A or {adr}
      _sre,                    // {adr}:={adr}/2 A:=A exor {adr}
      _anc,                    // A:=A&#{imm}
      _arr,                    // A:=(A&#{imm})/2
      _alr,                    // A:=(A&#{imm})/2
      _axs,                    // X:=A&X-#{imm}
      _xaa,                    // A:=X&#{imm}
      _tas,                    // S:=A&X {adr}:=S&H
      _ahx,                    // {adr}:=A&X&H
      _las,                    // A,X,S:={adr}&S
      _shx,                    // {adr}:=X&H
      _shy,                    // {adr}:=Y&H

      mnemo6501_count
      };

    static uint8_t m6501_codes[512];
    static OpCode m6501_opcodes[mnemo6501_count - mnemo6500_count];
    bool useUndefined;
  };


/*****************************************************************************/
/* Dasm6503 : class for a MOS Technology 6503, 6505, 6506 processors         */
/*****************************************************************************/

class Dasm6503 :
  public Dasm6501
  {
  public:
    Dasm6503(void);

  // Overrides
  public:
    // return bus width
    virtual int GetBusWidth(int bus = BusCode) { (void)bus; return 12; }
    // return highest possible code address
    virtual cadr_t GetHighestCodeAddr() { return 0x0fff; }
    // return highest possible data address
    virtual dadr_t GetHighestDataAddr() { return 0x0fff; }
  };

/*****************************************************************************/
/* Dasm6504 : class for a MOS Technology 6504, 6507 processors               */
/*****************************************************************************/

class Dasm6504 :
  public Dasm6501
  {
  public:
    Dasm6504(void);

  // Overrides
  public:
    // return bus width
    virtual int GetBusWidth(int bus = BusCode) { (void)bus; return 13; }
    // return highest possible code address
    virtual cadr_t GetHighestCodeAddr() { return 0x1fff; }
    // return highest possible data address
    virtual dadr_t GetHighestDataAddr() { return 0x1fff; }
  };


/*****************************************************************************/
/* Dasm65C02 : class for a 65C02 processor                                   */
/*****************************************************************************/

class Dasm65C02 :
  public Dasm650X
  {
  public:
    Dasm65C02(void);
    // return processor long name
    virtual string GetName() { return "MOS 65C02"; }

  protected:
    // parse instruction at given memory address for labels
    virtual adr_t ParseCode(adr_t addr, int bus = BusCode);
    // disassemble instruction at given memory address
    virtual adr_t DisassembleCode(adr_t addr, string &smnemo, string &sparm, int bus = BusCode);

  protected:
    // 65C02 addressing modes
    enum AddrMode65C02
      {
      _zpi = addrmodes6500_count,       /* zero-page indexed                 */
      _bbt,                             /* branch on bit (Rockwell/WDC)      */
      _zpb,                             /* zero-page bit (Rockwell/WDC)      */
      };

    // 65C02 additional mnemonics
    enum Mnemonics65C02
      {
      _bra = mnemo6500_count,
      _phx,
      _phy,
      _plx,
      _ply,
      _stz,
      _trb,
      _tsb,
      _bbr,                             /* Rockwell / WDC                    */
      _bbs,
      _rmb,
      _smb,
      _stp,
      _wai,

      mnemo65C02_count
      };

    static uint8_t m65c02_codes[512];
    static OpCode m65c02_opcodes[mnemo65C02_count - mnemo6500_count];
    bool useUndefined;
  };




#endif // __Dasm6500_h_defined__
