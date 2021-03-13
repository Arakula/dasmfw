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
/* Dasm6801.h : definition of the Dasm6801 class                             */
/*****************************************************************************/

#ifndef __Dasm6301_h_defined__
#define __Dasm6301_h_defined__

#include "Dasm6801.h"

/*****************************************************************************/
/* Dasm6301 : class for a Hitachi 6301 Processor                             */
/*****************************************************************************/

class Dasm6301 : public Dasm6801
  {
  public:
    Dasm6301(Application *pApp);
    virtual ~Dasm6301(void);

  // Overrides
  public:
    // return processor long name
    virtual string GetName() { return "Hitachi 6301"; }

  protected:
    // parse instruction at given memory address for labels
    virtual adr_t ParseCode(adr_t addr, int bus = BusCode);
    // disassemble instruction at given memory address
    virtual adr_t DisassembleCode(adr_t addr, string &smnemo, string &sparm, int bus = BusCode);

  protected:
    // additional 6309 addressing modes
    enum AddrMode6301
      {
      _bi = addrmodes6800_count,        /* Bit Manipulation index            */
      _bd ,                             /* Bit Manipulation direct           */
      };
    // additional 6301 mnemonics
    enum Mnemonics6301
      {
      _aim = mnemo6801_count,
      _eim,
      _oim,
      _tim,
      _slp,
      _xgdx,

      mnemo6301_count
      };


    static uint8_t h6301_codes[512];
    static OpCode opcodes[mnemo6301_count - mnemo6801_count];
  };

#endif // __Dasm6301_h_defined__
