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
/* Dasm68HC11.h : definition of the Dasm68HC11 class                         */
/*****************************************************************************/

#ifndef __Dasm68HC11_h_defined__
#define __Dasm68HC11_h_defined__

#include "Dasm6801.h"

/*****************************************************************************/
/* Dasm68HC11 : class for a Motorola 68HC11 Processor                        */
/*****************************************************************************/

class Dasm68HC11 : public Dasm6801
  {
  public:
    Dasm68HC11(void);
    virtual ~Dasm68HC11(void);

  // Overrides
  public:
    // return processor long name
    virtual string GetName() { return "Motorola 68HC11"; }

    // Initialize parsing
    virtual bool InitParse(int bus = BusCode);

  protected:
    // additional 68HC11 addressing modes
    enum AddrMode68HC11
      {
      _bd = addrmodes6800_count,        /* bit manipulation direct           */
      _bi,                              /* bit manipulation indexed          */
      _bbd,                             /* branch bitmask direct             */
      _bbi,                             /* branch bitmask indexed            */

      addrmodes68HC11_count
      };

    // additional 68HC11 mnemonics
    enum Mnemonics68HC11
      {
      _aby = mnemo6801_count,
      _bclr,
      _brclr,
      _brset,
      _bset,
      _cpd,
      _cpy,
      _dey,
      _fdiv,
      _idiv,
      _iny,
      _ldy,
      _pshy,
      _puly,
      _stop,
      _sty,
      _test,
      _tsy,
      _tys,
      _xgdx,
      _xgdy,

      mnemo68HC11_count
      };

    static uint8_t m68hc11_codes[512];
    static uint8_t m68hc11_codes18[512];
    static uint8_t m68hc11_codes1a[512];
    static uint8_t m68hc11_codescd[512];
    static OpCode opcodes[mnemo68HC11_count - mnemo6801_count];

    uint8_t *codes18;
    uint8_t *codes1a;
    uint8_t *codescd;

  protected:
    virtual adr_t FetchInstructionDetails(adr_t PC, uint8_t &O, uint8_t &T, uint8_t &M, uint16_t &W, int &MI, const char *&I, string *smnemo = NULL);
    virtual string GetIx8IndexReg(uint8_t O);
    // parse instruction at given memory address for labels
    virtual adr_t ParseCode(adr_t addr, int bus = BusCode);
    // disassemble instruction at given memory address
    virtual adr_t DisassembleCode(adr_t addr, string &smnemo, string &sparm, int bus = BusCode);

  };

#endif // __Dasm68HC11_h_defined__
