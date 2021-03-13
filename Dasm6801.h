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

#ifndef __Dasm6801_h_defined__
#define __Dasm6801_h_defined__

#include "Dasm6800.h"

/*****************************************************************************/
/* Dasm6801 : class for a Motorola 6801 Processor                            */
/*****************************************************************************/

class Dasm6801 : public Dasm6800
  {
  public:
    Dasm6801(Application *pApp);
    virtual ~Dasm6801(void);

  // Overrides
  public:
    // return processor long name
    virtual string GetName() { return "Motorola 6801"; }

    // Initialize parsing
    virtual bool InitParse(int bus = BusCode);

  protected:
    // additional 6801 mnemonics
    enum Mnemonics6801
      {
      _abx = mnemo6800_count,
      _addd,
      _brn,
      _ldd,
      _mul,
      _pulx,
      _pshx,
      _std,
      _subd,

      mnemo6801_count
      };


    static uint8_t m6801_codes[512];
    static OpCode opcodes[mnemo6801_count - mnemo6800_count];
  };

#endif // __Dasm6801_h_defined__
