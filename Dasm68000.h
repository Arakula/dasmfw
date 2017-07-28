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
/* Dasm68000.h : definition of the Dasm68000 class                           */
/*****************************************************************************/

#ifndef __Dasm68000_h_defined__
#define __Dasm68000_h_defined__

#include "Disassembler.h"


/*****************************************************************************/
/* Dasm68000 : class for a Motorola 68000 processor                          */
/*****************************************************************************/

class Dasm68000 :
  public Disassembler
  {
  public:
    Dasm68000(void);
    virtual ~Dasm68000(void);

  // Overrides
  public:
    // return processor long name
    virtual string GetName() { return "Motorola 68000"; }
    // return whether big- or little-endian
    virtual Endian GetEndianness() { return BigEndian; }
    // return bus width
    virtual int GetBusWidth(int bus = BusCode) { return 24; }
    // return code bits
    virtual int GetCodeBits() { return 32; }
    // return code pointer size in bytes
    virtual int GetCodePtrSize() { return 4; }
    // return highest possible code address
    virtual caddr_t GetHighestCodeAddr() { return 0xffffff; }
    // return data bits
    virtual int GetDataBits() { return 16; }
    // return data pointer size in bytes
    virtual int GetDataPtrSize() { return 4; }
    // return highest possible data address
    virtual daddr_t GetHighestDataAddr() { return 0xffffff; }

  // Options handler
  protected:
    int Set68000Option(string name, string value);
    string Get68000Option(string name);

  // the real disassembler activities
  protected:
    // parse data area for labels
    virtual addr_t ParseData(addr_t addr, int bus = BusCode);
    // parse instruction at given memory address for labels
    virtual addr_t ParseCode(addr_t addr, int bus = BusCode);
    // pass back correct mnemonic and parameters for a label
    virtual bool DisassembleLabel(Label *label, string &slabel, string &smnemo, string &sparm, int bus = BusCode);
    // pass back correct mnemonic and parameters for a DefLabel
    virtual bool DisassembleDefLabel(DefLabel *label, string &slabel, string &smnemo, string &sparm, int bus = BusCode);
    // disassemble data area at given memory address
    virtual addr_t DisassembleData(addr_t addr, addr_t end, uint32_t flags, string &smnemo, string &sparm, int maxparmlen, int bus = BusCode);
    // disassemble instruction at given memory address
    virtual addr_t DisassembleCode(addr_t addr, string &smnemo, string &sparm, int bus = BusCode);

  public:
    // Initialize parsing
    virtual bool InitParse(int bus = BusCode);
    // pass back disassembler-specific state changes before/after a disassembly line
    virtual bool DisassembleChanges(addr_t addr, addr_t prevaddr, addr_t prevsz, bool bAfterLine, vector<LineChange> &changes, int bus = BusCode);
    // create hex / ASCII representation of the current area
    virtual bool DisassembleHexAsc(addr_t addr, addr_t len, addr_t max, string &sHex, string &sAsc, int bus = BusCode)
      {
      // 68000 displays hex in word-sized quantities instead of bytes
      sAsc = "\'";
      addr_t i;
      for (i = 0; i < len; i++)
        {
        uint8_t c = GetUByte(addr + i, bus);
        sHex += sformat("%02X%s", c, (i & 1) ? " " : "");
        sAsc += (isprint(c)) ? c : '.'; 
        }
      sAsc += '\'';
      for (; i < max; i++)
        {
        sHex += (i & 1) ? "   " : "  ";
        sAsc += ' ';
        }
      return true;
      }

  protected:
    virtual bool String2Number(string s, addr_t &value);
    virtual string Number2String(addr_t value, int nDigits, addr_t addr, int bus = BusCode);
    virtual string Address2String(addr_t addr, int bus = BusCode)
      { return sformat("$%06x", addr); }

  protected:
    enum OpType
      {
      optype_unknown,
      optype_01,
      optype_02,
      optype_03,
      optype_04,
      optype_05,
      optype_06,
      optype_07,
      optype_08,
      optype_09,
      optype_10,
      optype_11,
      optype_12,
      optype_13,
      optype_14,
      optype_15,
      optype_16,
      optype_17,
      optype_18,
      optype_19,
      optype_20,
      optype_21,
      optype_22,
      optype_23,
      optype_24,
      optype_25,
      optype_26,
      optype_27,
      optype_28,
      optype_29,

      OpType68000_count
      };
    enum Mnemonics68000
      {
      _ill,                             /* illegal                           */
      _abcd,
      _adda,
      _addi,
      _addq,
      _addx,
      _add,
      _and,
      _andi,
      _asl,
      _asr,
      _bcc,
      _bchg,
      _bclr,
      _bcs,
      _beq,
      _bge,
      _bgt,
      _bhi,
      _ble,
      _bls,
      _blt,
      _bmi,
      _bne,
      _bpl,
      _bra,
      _bset,
      _bsr,
      _btst,
      _bvc,
      _bvs,
      _chk,
      _clr,
      _cmp,
      _cmpa,
      _cmpi,
      _cmpm,
      _dbcc,
      _dbcs,
      _dbeq,
      _dbf,
      _dbge,
      _dbgt,
      _dbhi,
      _dble,
      _dbls,
      _dblt,
      _dbmi,
      _dbne,
      _dbpl,
      _dbt,
      _dbvc,
      _dbvs,
      _divs,
      _divu,
      _eor,
      _eori,
      _exg,
      _ext,
      _jmp,
      _jsr,
      _lea,
      _link,
      _lsl,
      _lsr,
      _move,
      _move_b,
      _move_w,
      _move_l,
      _movem,
      _movep_w,
      _movep_l,
      _moveq,
      _muls,
      _mulu,
      _nbcd,
      _neg,
      _negx,
      _nop,
      _not,
      _or,
      _ori,
      _pea,
      _reset,
      _rol,
      _ror,
      _roxl,
      _roxr,
      _rte,
      _rtr,
      _rts,
      _sbcd,
      _scc,
      _scs,
      _seq,
      _sf,
      _sge,
      _sgt,
      _shi,
      _sle,
      _sls,
      _slt,
      _smi,
      _sne,
      _spl,
      _st,
      _stop,
      _sub,
      _suba,
      _subi,
      _subq,
      _subx,
      _svc,
      _svs,
      _swap,
      _tas,
      _trap,
      _trapv,
      _tst,
      _unlk,

      mnemo68000_count
      };
    struct OpDef
      {
      Mnemonics68000 mnemo;             /* mnemonic                          */
      uint16_t op_code;                 /* base op code                      */
      uint16_t mask;                    /* mask to remove variable data      */
      OpType ot;                        /* op type                           */
      };
    enum
      {
      optblSize = 143
      };
    static OpDef OpTable[optblSize];
    static OpCode opcodes[mnemo68000_count];
    uint8_t *otIndex;
    bool gas;                           /* flag whether GNU assembler output */
    bool useFCC;
    bool closeCC;
    // TEST TETS TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
    bool showInstTypes;

  protected:
    virtual addr_t ParseEffectiveAddress(addr_t instaddr, addr_t addr, uint16_t ea, int16_t index, int op_mode);
    virtual addr_t ParseOptype01(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype02(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype03(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype04(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype05(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype06(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype07(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype08(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype09(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype10(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype11(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype12(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype13(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype14(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype15(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype16(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype17(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype18(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype19(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype20(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype21(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype22(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype23(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype24(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype25(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype26(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype27(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype28(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t ParseOptype29(addr_t instaddr, addr_t addr, uint16_t code, int optable_index);
    virtual addr_t DisassembleEffectiveAddress(addr_t instaddr, addr_t addr, string &s, uint16_t ea, int16_t index, int op_mode);
    virtual addr_t DisassembleOptype01(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype02(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype03(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype04(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype05(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype06(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype07(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype08(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype09(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype10(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype11(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype12(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype13(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype14(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype15(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype16(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype17(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype18(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype19(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype20(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype21(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype22(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype23(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype24(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype25(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype26(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype27(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype28(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
    virtual addr_t DisassembleOptype29(addr_t instaddr, addr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm);
  };


// to my understanding, an 68008 is a crippled 68000 with either 20 or 22 bits address bus

#endif // __Dasm68000_h_defined__
