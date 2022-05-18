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
/* Assembler.h : definition of the Assembler class                           */
/*****************************************************************************/

#ifndef __Assembler_h_defined__
#define __Assembler_h_defined__

#include "Disassembler.h"

#if 0
Right now, this is just a scratch pad for gathering ideas.

The basic idea is to provide a class that formats any output according to
the capabilities of a specific assembler.

The disassemblers would then format a line's contents as an array of items
and pass that to the Assembler class to format the output into lines matching
the selected assembler's methods.

Possible Items:
===============

text {cchar}
        Text covering the rest of the line.
        cchar would be a boolean that defines whether a leading comment
          character is to be printed.
        This item, if there, has to be the last in the array.

label {ldchar}
        label for the current instruction.
        ldchar would be a boolean that can be used to force output of the label
          delimiter character. This can be overridden if a hypothetical
          assembler always requires or doesn't support a label delimiter
          character.

instruction
        Assembler instruction (mnemonic or pseudo-op) to use.
        I'm not sure yet how this could be realized in a way that's useful, but
        does not overcomplicate everything. Would it be better to just pass the
        ID of a specific instruction and let the Assembler class generate the
        matching instruction, or should the mnemonic text be passed, and the
        output formatter only decides on upper- and lowercase?
        Presumably the first is better, but configuring that might become a
        nightmare.
        Possible solution: each disassembler for a specific processor gets a
        companion class that subclasses Assembler with a defined set of IDs and
        a default set of mnemonics which could be overridden in a configuration
        file if needed. Doesn't look too bad.

parameter
        One of the parameters used by the instruction.
        This is even trickier than mnemonic above. Not yet sure how to capture
        all the possible ways such a parameter can be passed. Also, what
        exactly is a parameter? Looking at the simple 6809 instruction
          LDA Base+1
        ... is that one parameter, or two with a given concatenation character,
        or is that a set of 3 parameters, the middle one defining an addition?
        Or, if "Base" is a known 16-bit word ... what is this then? A parameter
        plus an offset, or a reference to the low byte of the parameter? Some
        assemblers would be able to handle that, whereas others would require
        the "+1" semantic.
        Also, the addressing mode would have to be passed; this, however, can
        define how to output one parameter or a complete set of parameters -
        but not necessarily all of them.
        Another uncomfortable thing: forced addressing. This can, depending on
        the processor and the assembler, take some quite "interesting" forms,
        where either the mnemonic or the parameter is decorated in some way,
        or even both (like "an add instruction taking an 8- and a 16-bit
        parameter storing the result in a 32-bit register").
        Hmmm. Not easy. Obviously, some kind of hierarchy is needed.

#endif


#endif // __Assembler_h_defined__
