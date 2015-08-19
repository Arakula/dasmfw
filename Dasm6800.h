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
/* Dasm6800.h : definition of the Dasm6800 class                             */
/*****************************************************************************/

#ifndef __Dasm6800_h_defined__
#define __Dasm6800_h_defined__

#include "Disassembler.h"

/*
 * Copied from f9dasm ...
 * There's a variant "1.61-RB" available at
 *   https://github.com/gordonjcp/f9dasm
 * (based on my V1.60) which contains some interesting changes by Rainer Buchty.
 * Unfortunately, I only saw that by chance in summer 2015, 6 versions later.
 * Rainer customized it to his own taste, so I didn't directly add his changes
 * to my code, which defaults to TSC Assembler compatible output;
 * you can, however, create a "Rainer Buchty compatible variant" by either
 *     1) setting the -DRB_VARIANT compiler flag or
 *     2) setting the following definition to anything other than 0.
*/

#ifndef RB_VARIANT
  #define RB_VARIANT 0
#endif


/*****************************************************************************/
/* Dasm6800 : class for a Motorola 6800 processor                            */
/*****************************************************************************/

class Dasm6800 :
  public Disassembler
  {
  public:
    Dasm6800(void);
    virtual ~Dasm6800(void);

  // Overrides
  public:
    // return processor long name
    virtual std::string GetName() { return "Motorola 6800"; }
    // return whether big- or little-endian
    virtual Endian GetEndianness() { return BigEndian; }
    // return code bits
    virtual int GetCodeBits() { return 16; }
    // return code pointer size in bytes
    virtual int GetCodePtrSize() { return 2; }
    // return highest possible code address
    virtual caddr_t GetHighestCodeAddr() { return 0xffff; }
    // return data bits
    virtual int GetDataBits() { return 8; }
    // return data pointer size in bytes
    virtual int GetDataPtrSize() { return 2; }
    // return highest possible data address
    virtual daddr_t GetHighestDataAddr() { return 0xffff; }

  // Options handler
  protected:
    int Set6800Option(std::string name, std::string value);
    std::string Get6800Option(std::string name);

  protected:
    // parse data area for labels
    virtual addr_t ParseData(addr_t addr, BusType bus = BusCode);
    // parse instruction at given memory address for labels
    virtual addr_t ParseCode(addr_t addr, BusType bus = BusCode);
    // pass back correct mnemonic and parameters for a label
    virtual bool DisassembleLabel(Label *label, std::string &slabel, std::string &smnemo, std::string &sparm, BusType bus = BusCode);
    // pass back correct mnemonic and parameters for a DefLabel
    virtual bool DisassembleDefLabel(DefLabel *label, std::string &slabel, std::string &smnemo, std::string &sparm, BusType bus = BusCode);
    // disassemble data area at given memory address
    virtual addr_t DisassembleData(addr_t addr, addr_t end, uint32_t flags, std::string &smnemo, std::string &sparm, int maxparmlen, BusType bus = BusCode);
    // disassemble instruction at given memory address
    virtual addr_t DisassembleCode(addr_t addr, std::string &smnemo, std::string &sparm, BusType bus = BusCode);
  public:
    // Initialize parsing
    virtual bool InitParse(BusType bus = BusCode);
    // pass back disassembler-specific state changes before/after a disassembly line
    virtual bool DisassembleChanges(addr_t addr, addr_t prevaddr, addr_t prevsz, bool bAfterLine, std::vector<LineChange> &changes, BusType bus = BusCode);


  protected:
    bool LoadFlex(FILE *f, std::string &sLoadType);
    virtual bool LoadFile(std::string filename, FILE *f, std::string &sLoadType, int interleave = 1);
    virtual bool String2Number(std::string s, addr_t &value);
    virtual std::string Number2String(addr_t value, int nDigits, addr_t addr, BusType bus = BusCode);
    virtual std::string Address2String(addr_t addr, BusType bus = BusCode)
      { return sformat("$%04X", addr); }
    virtual addr_t FetchInstructionDetails(addr_t PC, uint8_t &O, uint8_t &T, uint8_t &M, uint16_t &W, int &MI, const char *&I, std::string *smnemo = NULL);

  protected:
    // 6800 addressing modes
    enum AddrMode6800
      {
      _nom,                             /* no mode                           */
      _imp,                             /* inherent/implied                  */
      _imb,                             /* immediate byte                    */
      _imw,                             /* immediate word                    */
      _dir,                             /* direct                            */
      _ext,                             /* extended                          */
      _ix8,                             /* indexed for 6800 (unsigned)       */
      _reb,                             /* relative byte                     */

      addrmodes6800_count
      };

    // 6800 mnemonics
    enum Mnemonics6800
      {
      _ill,                             /* illegal                           */
      _aba,
      _adca,
      _adcb,
      _adda,
      _addb,
      _anda,
      _andb,
      _asla,
      _aslb,
      _asl,
      _asra,
      _asrb,
      _asr,
      _bcc,
      _bcs,
      _beq,
      _bge,
      _bgt,
      _bhi,
      _bita,
      _bitb,
      _ble,
      _bls,
      _blt,
      _bmi,
      _bne,
      _bpl,
      _bra,
      _bsr,
      _bvc,
      _bvs,
      _cba,
      _cli,
      _clra,
      _clrb,
      _clr,
      _clc,
      _clv,
      _cmpa,
      _cmpb,
      _coma,
      _comb,
      _com,
      _cpx,
      _daa,
      _deca,
      _decb,
      _dec,
      _des,
      _dex,
      _eora,
      _eorb,
      _inca,
      _incb,
      _inc,
      _ins,
      _inx,
      _jmp,
      _jsr,
      _lda,
      _ldb,
      _lds,
      _ldx,
      _lsra,
      _lsrb,
      _lsr,
      _nega,
      _negb,
      _neg,
      _nop,
      _ora,
      _orb,
      _psha,
      _pshb,
      _pula,
      _pulb,
      _rola,
      _rolb,
      _rol,
      _rora,
      _rorb,
      _ror,
      _rti,
      _rts,
      _sba,
      _sbca,
      _sbcb,
      _sec,
      _sei,
      _sev,
      _sta,
      _stb,
      _sts,
      _stx,
      _suba,
      _subb,
      _swi,
      _tab,
      _tap,
      _tba,  
      _tpa,
      _tsta,
      _tstb,
      _tst,
      _tsx,
      _txs,  
      _wai,
      // convenience mnemonics
     _asld,
     _lsrd,

      mnemo6800_count
      };

    static uint8_t m6800_codes[512];

    uint8_t *codes;
    static char *bit_r[];
    static char *block_r[];
    static OpCode opcodes[mnemo6800_count];

    bool useConvenience;
    bool useFCC;
    bool showIndexedModeZeroOperand;
    bool closeCC;
    bool forceExtendedAddr;
    bool forceDirectAddr;
  };


#endif // __Dasm6800_h_defined__
