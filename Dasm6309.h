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
/* Dasm6309.h : definition of the Dasm6309 class                             */
/*****************************************************************************/

#ifndef __Dasm6309_h_defined__
#define __Dasm6309_h_defined__

#include "Dasm6809.h"

/*****************************************************************************/
/* Dasm6309 : class for a Hitachi 6309 disassembler                          */
/*****************************************************************************/

class Dasm6309 : public Dasm6809
  {
  public:
    Dasm6309(Application *pApp);
    virtual ~Dasm6309(void);

  // Overrides
  public:
    // return processor long name
    virtual string GetName() { return "Hitachi 6309"; }

    // Initialize parsing
    virtual bool InitParse(int bus = BusCode);

  protected:
    // parse instruction at given memory address for labels
    virtual adr_t ParseCode(adr_t addr, int bus = BusCode);
    // disassemble data area at given memory address
    virtual adr_t DisassembleData(adr_t addr, adr_t end, uint32_t flags, string &smnemo, string &sparm, int maxparmlen, int bus = BusCode);
    // disassemble instruction at given memory address
    virtual adr_t DisassembleCode(adr_t addr, string &smnemo, string &sparm, int bus = BusCode);
    virtual bool SetConvenience(uint8_t instpg, uint16_t u2, string &smnemo, adr_t &PC);

  protected:
    // additional 6309 addressing modes
    enum AddrMode6309
      {
      _bd = addrmodes6809_count,        /* Bit Manipulation direct           */
      _bi ,                             /* Bit Manipulation index            */
      _be ,                             /* Bit Manipulation extended         */
      _bt ,                             /* Bit Transfers direct              */
      _t1 ,                             /* Block Transfer r0+,r1+            */
      _t2 ,                             /* Block Transfer r0-,r1-            */
      _t3 ,                             /* Block Transfer r0+,r1             */
      _t4 ,                             /* Block Transfer r0,r1+             */
      _iml,                             /* immediate 32-bit                  */

      addrmodes6309_count
      };

    // additional 6309 mnemonics
    enum Mnemonics6309
      {
      _aim = mnemo6809_count,
      _eim,
      _oim,
      _tim,
      _band,
      _biand,
      _bor,
      _bior,
      _beor,
      _bieor,
      _ldbt,
      _stbt,
      _tfm,
      _adcd,
      _adcr,
      _adde,
      _addf,
      _addw,
      _addr,
      _andd,
      _andr,
      _asrd,
      _bitd,
      _bitmd,
      _clrd,
      _clre,
      _clrf,
      _clrw,
      _cmpe,
      _cmpf,
      _cmpw,
      _cmpr,
      _comd,
      _come,
      _comf,
      _comw,
      _dece,
      _decf,
      _decw,
      _divd,
      _divq,
      _eord,
      _eorr,
      _ince,
      _incf,
      _incw,
      _lde,
      _ldf,
      _ldq,
      _ldw,
      _ldmd,
      _lsrw,
      _muld,
      _negd,
      _ord,
      _orr,
      _pshsw,
      _pshuw,
      _pulsw,
      _puluw,
      _rold,
      _rolw,
      _rord,
      _rorw,
      _sbcd,
      _sbcr,
      _sexw,
      _ste,
      _stf,
      _stq,
      _stw,
      _sube,
      _subf,
      _subw,
      _subr,
      _tstd,
      _tste,
      _tstf,
      _tstw,

      _fqb,

      mnemo6309_count
      };

    enum Reg6309
      {
      _e = reg6809_count,
      _f,
      _v,
      _w,
      _md,
      _reg0,                            /* not really a register, but hey... */

      reg6309_count
      };

    static CMatrixEntry h6309_codes[256];
    static CMatrixEntry h6309_codes10[256];
    static CMatrixEntry h6309_codes11[256];
    static OpCode opcodes[mnemo6309_count - mnemo6809_count];
    static const char *regnames[reg6309_count - reg6809_count];
    static const int h6309_exg_tfr[];
    static const int bit_r[];
    static const int block_r[];

  protected:
    virtual adr_t IndexParse(int MI, adr_t pc, adr_t instaddr = NO_ADDRESS);
    virtual string IndexString(string &smnemo, adr_t &pc);
  };

#endif // __Dasm6309_h_defined__
