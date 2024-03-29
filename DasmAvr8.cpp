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
/*                          Copyright (C) Hermann Seib                     *
 * The AVR8 disassembler engine is loosely based on vAVRdisasm (see        *
 * https://github.com/vsergeev/vavrdisasm for that), so                    *
 *                    Parts Copyright (C) 2013 Vanya A. Sergeev            *
 ***************************************************************************/

/*****************************************************************************/
/* DasmAvr8.cpp : avr8 disassembler implementation                           */
/*****************************************************************************/

#include "DasmAvr8.h"

/*****************************************************************************/
/* CreateAvr8 : create an avr8 disassembler handler                          */
/*****************************************************************************/

static Disassembler *CreateAvr8(Application *pApp)
{
Disassembler *pDasm = new DasmAvr8(pApp);
if (pDasm) pDasm->Setup();
return pDasm;
}

/*****************************************************************************/
/* Auto-registration                                                         */
/*****************************************************************************/

static bool bRegistered[] =
  {
  RegisterDisassembler("avr8", CreateAvr8),
  };


/*===========================================================================*/
/* DasmAvr8 class members                                                    */
/*===========================================================================*/

static const char *sdefdelim[] = { " = ", " " };
static const char *sequdelim[] = { " = ", ", " };

/*****************************************************************************/
/* opcodes : Avr8 opcodes array for initialization                           */
/*****************************************************************************/

OpCode DasmAvr8::opcodes[mnemoAvr8_count] =
  {
    { "???",      Data },               /* _ill                              */
    { "adc",      Data },               /* _adc                              */
    { "add",      Data },               /* _add                              */
    { "adiw",     Data },               /* _adiw                             */
    { "and",      Data },               /* _and                              */
    { "andi",     Data },               /* _andi                             */
    { "asr",      Data },               /* _asr                              */
    { "bclr",     Data },               /* _bclr                             */
    { "bld",      Data },               /* _bld                              */
    { "brbc",     Code },               /* _brbc                             */
    { "brbs",     Code },               /* _brbs                             */
    { "brcc",     Code },               /* _brcc                             */
    { "brcs",     Code },               /* _brcs                             */
    { "break",    Data },               /* _break                            */
    { "breq",     Code },               /* _breq                             */
    { "brge",     Code },               /* _brge                             */
    { "brhc",     Code },               /* _brhc                             */
    { "brhs",     Code },               /* _brhs                             */
    { "brid",     Code },               /* _brid                             */
    { "brie",     Code },               /* _brie                             */
    { "brlo",     Code },               /* _brlo                             */
    { "brlt",     Code },               /* _brlt                             */
    { "brmi",     Code },               /* _brmi                             */
    { "brne",     Code },               /* _brne                             */
    { "brpl",     Code },               /* _brpl                             */
    { "brsh",     Code },               /* _brsh                             */
    { "brtc",     Code },               /* _brtc                             */
    { "brts",     Code },               /* _brts                             */
    { "brvc",     Code },               /* _brvc                             */
    { "brvs",     Code },               /* _brvs                             */
    { "bset",     Data },               /* _bset                             */
    { "bst",      Data },               /* _bst                              */
    { "call",     Code },               /* _call                             */
    { "cbi",      Data },               /* _cbi                              */
    { "cbr",      Data },               /* _cbr                              */
    { "clc",      Data },               /* _clc                              */
    { "clh",      Data },               /* _clh                              */
    { "cli",      Data },               /* _cli                              */
    { "cln",      Data },               /* _cln                              */
    { "clr",      Data },               /* _clr                              */
    { "cls",      Data },               /* _cls                              */
    { "clt",      Data },               /* _clt                              */
    { "clv",      Data },               /* _clv                              */
    { "clz",      Data },               /* _clz                              */
    { "com",      Data },               /* _com                              */
    { "cp",       Data },               /* _cp                               */
    { "cpc",      Data },               /* _cpc                              */
    { "cpi",      Data },               /* _cpi                              */
    { "cpse",     Data },               /* _cpse                             */
    { "dec",      Data },               /* _dec                              */
    { "des",      Data },               /* _des                              */
    { "eicall",   Data },               /* _eicall                           */
    { "eijmp",    Data },               /* _eijmp                            */
    { "elpm",     Data },               /* _elpm                             */
    { "eor",      Data },               /* _eor                              */
    { "fmul",     Data },               /* _fmul                             */
    { "fmuls",    Data },               /* _fmuls                            */
    { "fmulsu",   Data },               /* _fmulsu                           */
    { "icall",    Code },               /* _icall                            */
    { "ijmp",     Code },               /* _ijmp                             */
    { "in",       Data },               /* _in                               */
    { "inc",      Data },               /* _inc                              */
    { "jmp",      Code },               /* _jmp                              */
    { "lac",      Data },               /* _lac                              */
    { "las",      Data },               /* _las                              */
    { "lat",      Data },               /* _lat                              */
    { "ld",       Data },               /* _ld                               */
    { "ldd",      Data },               /* _ldd                              */
    { "ldi",      Data },               /* _ldi                              */
    { "lds",      Data },               /* _lds                              */
    { "lpm",      Data },               /* _lpm                              */
    { "lsl",      Data },               /* _lsl                              */
    { "lsr",      Data },               /* _lsr                              */
    { "mov",      Data },               /* _mov                              */
    { "movw",     Data },               /* _movw                             */
    { "mul",      Data },               /* _mul                              */
    { "muls",     Data },               /* _muls                             */
    { "mulsu",    Data },               /* _mulsu                            */
    { "neg",      Data },               /* _neg                              */
    { "nop",      Data },               /* _nop                              */
    { "or",       Data },               /* _or                               */
    { "ori",      Data },               /* _ori                              */
    { "out",      Data },               /* _out                              */
    { "pop",      Data },               /* _pop                              */
    { "push",     Data },               /* _push                             */
    { "rcall",    Code },               /* _rcall                            */
    { "ret",      Code },               /* _ret                              */
    { "reti",     Code },               /* _reti                             */
    { "rjmp",     Code },               /* _rjmp                             */
    { "rol",      Data },               /* _rol                              */
    { "ror",      Data },               /* _ror                              */
    { "sbc",      Data },               /* _sbc                              */
    { "sbci",     Data },               /* _sbci                             */
    { "sbi",      Data },               /* _sbi                              */
    { "sbic",     Data },               /* _sbic                             */
    { "sbis",     Data },               /* _sbis                             */
    { "sbiw",     Data },               /* _sbiw                             */
    { "sbr",      Data },               /* _sbr                              */
    { "sbrc",     Data },               /* _sbrc                             */
    { "sbrs",     Data },               /* _sbrs                             */
    { "sec",      Data },               /* _sec                              */
    { "seh",      Data },               /* _seh                              */
    { "sei",      Data },               /* _sei                              */
    { "sen",      Data },               /* _sen                              */
    { "ser",      Data },               /* _ser                              */
    { "ses",      Data },               /* _ses                              */
    { "set",      Data },               /* _set                              */
    { "sev",      Data },               /* _sev                              */
    { "sez",      Data },               /* _sez                              */
    { "sleep",    Data },               /* _sleep                            */
    { "spm",      Data },               /* _spm                              */
    { "st",       Data },               /* _st                               */
    { "std",      Data },               /* _std                              */
    { "sts",      Data },               /* _sts                              */
    { "sub",      Data },               /* _sub                              */
    { "subi",     Data },               /* _subi                             */
    { "swap",     Data },               /* _swap                             */
    { "tst",      Data },               /* _tst                              */
    { "wdr",      Data },               /* _wdr                              */
    { "xch",      Data },               /* _xch                              */
    // directives
    { ".byte",    Const },              /*  _d_byte                          */
    { ".cseg",    Const },              /*  _d_cseg                          */
    { ".db",      Const },              /* _d_db                             */
    { ".def",     Const },              /* _d_def                            */
    { ".device",  Const },              /* _d_device                         */
    { ".dseg",    Const },              /* _d_dseg                           */
    { ".dw",      Const },              /* _d_dw                             */
    { ".equ",     Const },              /* _d_equ                            */
    { ".eseg",    Const },              /* _d_eseg                           */
    { ".exit",    Const },              /* _d_exit                           */
    { ".include", Const },              /* _d_include                        */
    { ".org",     Const },              /* _d_org                            */
    { ".equ",     Const },              /* _d_port                           */
    { ".set",     Const },              /* _d_set                            */
    { ".db",      Const },              /* _d_string                         */
    { ".db",      Const },              /* _d_stringz                        */

  };

/*****************************************************************************/
/* AVR_Instruction_Set : the instruction set                                 */
/*****************************************************************************/

avrInstructionInfo DasmAvr8::AVR_Instruction_Set[] =
  {
    { _break,  2, 0x9598, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _clc,    2, 0x9488, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _clh,    2, 0x94d8, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _cli,    2, 0x94f8, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _cln,    2, 0x94a8, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _cls,    2, 0x94c8, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _clt,    2, 0x94e8, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _clv,    2, 0x94b8, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _clz,    2, 0x9498, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _eicall, 2, 0x9519, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _eijmp,  2, 0x9419, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _elpm,   2, 0x95d8, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _icall,  2, 0x9509, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _ijmp,   2, 0x9409, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _lpm,    2, 0x95c8, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _nop,    2, 0x0000, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _ret,    2, 0x9508, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _reti,   2, 0x9518, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _sec,    2, 0x9408, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _seh,    2, 0x9458, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _sei,    2, 0x9478, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _sen,    2, 0x9428, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _ses,    2, 0x9448, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _set,    2, 0x9468, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _sev,    2, 0x9438, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _sez,    2, 0x9418, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _sleep,  2, 0x9588, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _spm,    2, 0x95e8, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _spm,    2, 0x95f8, 1, {0x0000, 0x0000}, {OpndZPlus, OpndNone}},
    { _wdr,    2, 0x95a8, 0, {0x0000, 0x0000}, {OpndNone, OpndNone}},
    { _des,    2, 0x940b, 1, {0x00f0, 0x0000}, {OpndDESRound, OpndNone}},
    { _asr,    2, 0x9405, 1, {0x01f0, 0x0000}, {OpndReg, OpndNone}},
    { _bclr,   2, 0x9488, 1, {0x0070, 0x0000}, {OpndBit, OpndNone}},
    { _brcc,   2, 0xf400, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brcs,   2, 0xf000, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _breq,   2, 0xf001, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brge,   2, 0xf404, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brhc,   2, 0xf405, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brhs,   2, 0xf005, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brid,   2, 0xf407, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brie,   2, 0xf007, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brlo,   2, 0xf000, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brlt,   2, 0xf004, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brmi,   2, 0xf002, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brne,   2, 0xf401, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brpl,   2, 0xf402, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brsh,   2, 0xf400, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brtc,   2, 0xf406, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brts,   2, 0xf006, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brvc,   2, 0xf403, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _brvs,   2, 0xf003, 1, {0x03f8, 0x0000}, {OpndBranchAddr, OpndNone}},
    { _bset,   2, 0x9408, 1, {0x0070, 0x0000}, {OpndBit, OpndNone}},
    { _call,   4, 0x940e, 1, {0x01f1, 0x0000}, {OpndLongAbsAddr, OpndNone}},
    { _com,    2, 0x9400, 1, {0x01f0, 0x0000}, {OpndReg, OpndNone}},
    { _dec,    2, 0x940a, 1, {0x01f0, 0x0000}, {OpndReg, OpndNone}},
    { _inc,    2, 0x9403, 1, {0x01f0, 0x0000}, {OpndReg, OpndNone}},
    { _jmp,    4, 0x940c, 1, {0x01f1, 0x0000}, {OpndLongAbsAddr, OpndNone}},
    { _lpm,    2, 0x9004, 2, {0x01f0, 0x0000}, {OpndReg, OpndZ}},
    { _lpm,    2, 0x9005, 2, {0x01f0, 0x0000}, {OpndReg, OpndZPlus}},
    { _lsr,    2, 0x9406, 1, {0x01f0, 0x0000}, {OpndReg, OpndNone}},
    { _neg,    2, 0x9401, 1, {0x01f0, 0x0000}, {OpndReg, OpndNone}},
    { _pop,    2, 0x900f, 1, {0x01f0, 0x0000}, {OpndReg, OpndNone}},
    { _xch,    2, 0x9204, 2, {0x0000, 0x01f0}, {OpndZ, OpndReg}},
    { _las,    2, 0x9205, 2, {0x0000, 0x01f0}, {OpndZ, OpndReg}},
    { _lac,    2, 0x9206, 2, {0x0000, 0x01f0}, {OpndZ, OpndReg}},
    { _lat,    2, 0x9207, 2, {0x0000, 0x01f0}, {OpndZ, OpndReg}},
    { _push,   2, 0x920f, 1, {0x01f0, 0x0000}, {OpndReg, OpndNone}},
    { _rcall,  2, 0xd000, 1, {0x0fff, 0x0000}, {OpndRelAddr, OpndNone}},
    { _rjmp,   2, 0xc000, 1, {0x0fff, 0x0000}, {OpndRelAddr, OpndNone}},
    { _ror,    2, 0x9407, 1, {0x01f0, 0x0000}, {OpndReg, OpndNone}},
    { _ser,    2, 0xef0f, 1, {0x00f0, 0x0000}, {OpndRegStartR16, OpndNone}},
    { _swap,   2, 0x9402, 1, {0x01f0, 0x0000}, {OpndReg, OpndNone}},
    { _adc,    2, 0x1c00, 2, {0x01f0, 0x020f}, {OpndReg, OpndReg}},
    { _add,    2, 0x0c00, 2, {0x01f0, 0x020f}, {OpndReg, OpndReg}},
    { _adiw,   2, 0x9600, 2, {0x0030, 0x00cf}, {OpndRegEvenPStartR24, OpndData}},
    { _and,    2, 0x2000, 2, {0x01f0, 0x020f}, {OpndReg, OpndReg}},
    { _andi,   2, 0x7000, 2, {0x00f0, 0x0f0f}, {OpndRegStartR16, OpndData}},
    { _bld,    2, 0xf800, 2, {0x01f0, 0x0007}, {OpndReg, OpndBit}},
    { _brbc,   2, 0xf400, 2, {0x0007, 0x03f8}, {OpndBit, OpndBranchAddr}},
    { _brbs,   2, 0xf000, 2, {0x0007, 0x03f8}, {OpndBit, OpndBranchAddr}},
    { _bst,    2, 0xfa00, 2, {0x01f0, 0x0007}, {OpndReg, OpndBit}},
    { _cbi,    2, 0x9800, 2, {0x00f8, 0x0007}, {OpndIOReg, OpndBit}},
    { _cp,     2, 0x1400, 2, {0x01f0, 0x020f}, {OpndReg, OpndReg}},
    { _cpc,    2, 0x0400, 2, {0x01f0, 0x020f}, {OpndReg, OpndReg}},
    { _cpi,    2, 0x3000, 2, {0x00f0, 0x0f0f}, {OpndRegStartR16, OpndData}},
    { _cpse,   2, 0x1000, 2, {0x01f0, 0x020f}, {OpndReg, OpndReg}},
    { _elpm,   2, 0x9006, 2, {0x01f0, 0x0000}, {OpndReg, OpndZ}},
    { _elpm,   2, 0x9007, 2, {0x01f0, 0x0000}, {OpndReg, OpndZPlus}},
    { _eor,    2, 0x2400, 2, {0x01f0, 0x020f}, {OpndReg, OpndReg}},
    { _fmul,   2, 0x0308, 2, {0x0070, 0x0007}, {OpndRegStartR16, OpndRegStartR16}},
    { _fmuls,  2, 0x0380, 2, {0x0070, 0x0007}, {OpndRegStartR16, OpndRegStartR16}},
    { _fmulsu, 2, 0x0388, 2, {0x0070, 0x0007}, {OpndRegStartR16, OpndRegStartR16}},
    { _in,     2, 0xb000, 2, {0x01f0, 0x060f}, {OpndReg, OpndIOReg}},
    { _ld,     2, 0x900c, 2, {0x01f0, 0x0000}, {OpndReg, OpndX}},
    { _ld,     2, 0x900d, 2, {0x01f0, 0x0000}, {OpndReg, OpndXPlus}},
    { _ld,     2, 0x900e, 2, {0x01f0, 0x0000}, {OpndReg, OpndMinusX}},
    { _ld,     2, 0x8008, 2, {0x01f0, 0x0000}, {OpndReg, OpndY}},
    { _ld,     2, 0x9009, 2, {0x01f0, 0x0000}, {OpndReg, OpndYPlus}},
    { _ld,     2, 0x900a, 2, {0x01f0, 0x0000}, {OpndReg, OpndMinusY}},
    { _ld,     2, 0x8000, 2, {0x01f0, 0x0000}, {OpndReg, OpndZ}},
    { _ld,     2, 0x9001, 2, {0x01f0, 0x0000}, {OpndReg, OpndZPlus}},
    { _ld,     2, 0x9002, 2, {0x01f0, 0x0000}, {OpndReg, OpndMinusZ}},
    { _ldd,    2, 0x8008, 2, {0x01f0, 0x2c07}, {OpndReg, OpndYPlusQ}},
    { _ldd,    2, 0x8000, 2, {0x01f0, 0x2c07}, {OpndReg, OpndZPlusQ}},
    { _ldi,    2, 0xe000, 2, {0x00f0, 0x0f0f}, {OpndRegStartR16, OpndData}},
    { _lds,    4, 0x9000, 2, {0x01f0, 0x0000}, {OpndReg, OpndLongAbsAddrData}},
    { _lds,    2, 0xa000, 2, {0x00f0, 0x070f}, {OpndRegStartR16, OpndData}},
    { _mov,    2, 0x2c00, 2, {0x01f0, 0x020f}, {OpndReg, OpndReg}},
    { _movw,   2, 0x0100, 2, {0x00f0, 0x000f}, {OpndRegEvenPair, OpndRegEvenPair}},
    { _mul,    2, 0x9c00, 2, {0x01f0, 0x020f}, {OpndReg, OpndReg}},
    { _muls,   2, 0x0200, 2, {0x00f0, 0x000f}, {OpndRegStartR16, OpndRegStartR16}},
    { _mulsu,  2, 0x0300, 2, {0x0070, 0x0007}, {OpndRegStartR16, OpndRegStartR16}},
    { _or,     2, 0x2800, 2, {0x01f0, 0x020f}, {OpndReg, OpndReg}},
    { _ori,    2, 0x6000, 2, {0x00f0, 0x0f0f}, {OpndRegStartR16, OpndData}},
    { _out,    2, 0xb800, 2, {0x060f, 0x01f0}, {OpndIOReg, OpndReg}},
    { _sbc,    2, 0x0800, 2, {0x01f0, 0x020f}, {OpndReg, OpndReg}},
    { _sbci,   2, 0x4000, 2, {0x00f0, 0x0f0f}, {OpndRegStartR16, OpndData}},
    { _sbi,    2, 0x9a00, 2, {0x00f8, 0x0007}, {OpndIOReg, OpndBit}},
    { _sbic,   2, 0x9900, 2, {0x00f8, 0x0007}, {OpndIOReg, OpndBit}},
    { _sbis,   2, 0x9b00, 2, {0x00f8, 0x0007}, {OpndIOReg, OpndBit}},
    { _sbiw,   2, 0x9700, 2, {0x0030, 0x00cf}, {OpndRegEvenPStartR24, OpndData}},
    { _sbr,    2, 0x6000, 2, {0x00f0, 0x0f0f}, {OpndRegStartR16, OpndData}},
    { _sbrc,   2, 0xfc00, 2, {0x01f0, 0x0007}, {OpndReg, OpndBit}},
    { _sbrs,   2, 0xfe00, 2, {0x01f0, 0x0007}, {OpndReg, OpndBit}},
    { _st,     2, 0x920c, 2, {0x0000, 0x01f0}, {OpndX, OpndReg}},
    { _st,     2, 0x920d, 2, {0x0000, 0x01f0}, {OpndXPlus, OpndReg}},
    { _st,     2, 0x920e, 2, {0x0000, 0x01f0}, {OpndMinusX, OpndReg}},
    { _st,     2, 0x8208, 2, {0x0000, 0x01f0}, {OpndY, OpndReg}},
    { _st,     2, 0x9209, 2, {0x0000, 0x01f0}, {OpndYPlus, OpndReg}},
    { _st,     2, 0x920a, 2, {0x0000, 0x01f0}, {OpndMinusY, OpndReg}},
    { _st,     2, 0x8200, 2, {0x0000, 0x01f0}, {OpndZ, OpndReg}},
    { _st,     2, 0x9201, 2, {0x0000, 0x01f0}, {OpndZPlus, OpndReg}},
    { _st,     2, 0x9202, 2, {0x0000, 0x01f0}, {OpndMinusZ, OpndReg}},
    { _std,    2, 0x8208, 2, {0x2c07, 0x01f0}, {OpndYPlusQ, OpndReg}},
    { _std,    2, 0x8200, 2, {0x2c07, 0x01f0}, {OpndZPlusQ, OpndReg}},
    { _sts,    4, 0x9200, 2, {0x0000, 0x01f0}, {OpndLongAbsAddrData, OpndReg}},
    { _sts,    2, 0xA800, 2, {0x00f0, 0x070f}, {OpndRegStartR16, OpndData}},
    { _sub,    2, 0x1800, 2, {0x01f0, 0x020f}, {OpndReg, OpndReg}},
    { _subi,   2, 0x5000, 2, {0x00f0, 0x0f0f}, {OpndRegStartR16, OpndData}},
  };

/*****************************************************************************/
/* DasmAvr8 : constructor                                                    */
/*****************************************************************************/

DasmAvr8::DasmAvr8(Application *pApp)
  : Disassembler(pApp)
{
commentStart = ";";
labelDelim = ":";
dbalign = true;
dbalignchg = false;
avr_gcc = false;

disassemblyFlagMask &= ~SHMF_TXT;       /* FCB and FCC are the same here     */

mnemo.resize(mnemoAvr8_count);          /* set up mnemonics table            */
for (int i = 0; i < mnemoAvr8_count; i++)
  mnemo[i] = opcodes[i];

// set up options table
// class uses one generic option setter/getter pair (not mandatory)
AddOption("codebits", "{number}\tCode address bits",
          static_cast<PSetter>(&DasmAvr8::SetAvr8Option),
          static_cast<PGetter>(&DasmAvr8::GetAvr8Option));
AddOption("highcode", "{addr}\tHighest code address",
          static_cast<PSetter>(&DasmAvr8::SetAvr8Option),
          static_cast<PGetter>(&DasmAvr8::GetAvr8Option));
AddOption("highdata", "{addr}\tHighest data address",
          static_cast<PSetter>(&DasmAvr8::SetAvr8Option),
          static_cast<PGetter>(&DasmAvr8::GetAvr8Option));
#if 0
// invariably 63(? check...)
AddOption("highio", "{addr}\tHighest data address",
          static_cast<PSetter>(&DasmAvr8::SetAvr8Option),
          static_cast<PGetter>(&DasmAvr8::GetAvr8Option));
#endif
AddOption("higheeprom", "{addr}\tHighest EEPROM address",
          static_cast<PSetter>(&DasmAvr8::SetAvr8Option),
          static_cast<PGetter>(&DasmAvr8::GetAvr8Option));
AddOption("dbalign", "{off|on}\tTry to align .db to word boundary",
          static_cast<PSetter>(&DasmAvr8::SetAvr8Option),
          static_cast<PGetter>(&DasmAvr8::GetAvr8Option));
AddOption("avr-gcc", "{off|on}\tCreate avr-gcc compatible output (.sx format)",
          static_cast<PSetter>(&DasmAvr8::SetAvr8Option),
          static_cast<PGetter>(&DasmAvr8::GetAvr8Option));
}

/*****************************************************************************/
/* ~DasmAvr8 : destructor                                                    */
/*****************************************************************************/

DasmAvr8::~DasmAvr8()
{
}

/*****************************************************************************/
/* Setup : Setup disassembler after construction                             */
/*****************************************************************************/

bool DasmAvr8::Setup()
{
highaddr.resize(GetBusCount());
highaddr[BusCode] = 0x3fffff;
highaddr[BusData] = 0xffff;
highaddr[BusIO]   = 0x3f;
highaddr[BusEEPROM] = 0xffff;

bool bOK = Disassembler::Setup();

SetupRegnames(avr_gcc);                 /* initial register name setup       */

busnames[BusData]   = "Data";
busnames[BusIO]     = "IO";
busnames[BusEEPROM] = "EEPROM";

return bOK;
}

/*****************************************************************************/
/* SetupRegnames : set up register names                                     */
/*****************************************************************************/

void DasmAvr8::SetupRegnames(bool newAvrGcc)
{
static const char *reghdr[] = { "R", "r" };
if (!regname.size())
  {
  regname.resize(regAvr8_count);        /* assure space for reg names        */
  regname[_x] = "X";
  regname[_y] = "Y";
  regname[_z] = "Z";
  }
for (int i = _r0; i <= _r31; i++)       /* set up R0..R31                    */
  {
  string regnameold = sformat("%s%d", reghdr[avr_gcc], i);
  if (!regname[i].size() ||
      regname[i] == regnameold)
    regname[i] = sformat("%s%d", reghdr[newAvrGcc], i);
  }
}

/*****************************************************************************/
/* InfoHelp : print disassembler-specific info file help                     */
/*****************************************************************************/

string DasmAvr8::InfoHelp()
{
string busses = GetBusNames();
const char *szb = busses.c_str();
return sformat("REGLABEL addr Reg [text]\n"
               "UNREGLABEL addr Reg\n"
               "HIGH [BUS {%s}] addr[-addr] reladdr\n"
               "LOW [BUS {%s}] addr[-addr] reladdr\n"
               "MHIGH [BUS {%s}] addr[-addr] reladdr\n"
               "MLOW [BUS {%s}] addr[-addr] reladdr\n"
               "HIGH2 [BUS {%s}] addr[-addr] reladdr\n"
               "LOW2 [BUS {%s}] addr[-addr] reladdr\n"
               , szb
               , szb
               , szb
               , szb
               , szb
               , szb
               );
}

/*****************************************************************************/
/* ProcessInfo : process an info file line                                   */
/*****************************************************************************/

bool DasmAvr8::ProcessInfo
    (
    string key,                         /* parsed key                        */
    string value,                       /* rest of the line                  */
    adr_t &from,                       /* from/to (if range given)          */
    adr_t &to, 
    adr_t &step,                       /* step size                         */
    vector<TMemoryArray<adr_t>> &remaps,  /* remaps, if necessary           */
    bool bProcInfo,                     /* flag whether processing           */
    int bus,                            /* target bus for command            */
    int tgtbus                          /* target bus for parameters         */
    )
{
if (!bProcInfo)                         /* only if processing                */
  return false;

enum InfoCmd
  {
  infoUnknown = -1,                     /* unknown info command              */
  infoRegLabel,                         /* REGLABEL addr Rxx [Text]          */
  infoUnregLabel,                       /* UNREGLABEL addr Rxx               */
  infoHigh,                             /* HIGH [BUS bus] addr[-addr] addr   */
  infoLow,                              /* LOW [BUS bus] addr[-addr] addr    */
  infoMHigh,                            /* MHIGH [BUS bus] addr[-addr] addr  */
  infoMLow,                             /* MLOW [BUS bus] addr[-addr] addr   */
  infoHigh2,                            /* HIGH2 [BUS bus] addr[-addr] addr/2*/
  infoLow2,                             /* LOW2 [BUS bus] addr[-addr] addr/2 */
  };
static struct                           /* structure to convert key to type  */
  {
  const char *name;
  InfoCmd cmdType;
  } sKey[] =
  {
  { "REGLABEL",     infoRegLabel },
  { "UNREGLABEL",   infoUnregLabel },
  { "HIGH",         infoHigh },
  { "LOW",          infoLow },
  { "MHIGH",        infoMHigh },
  { "MLOW",         infoMLow },
  { "HIGH2",        infoHigh2 },
  { "LOW2",         infoLow2 },
  };

InfoCmd cmdType = infoUnknown;
for (size_t i = 0; i < _countof(sKey); i++)
  if (key == sKey[i].name)
    {
    cmdType = sKey[i].cmdType;
    break;
    }
if (cmdType == infoUnknown)
  return false;  // we KNOW the parent classes don't do anything else

switch (cmdType)
  {
  case infoRegLabel :                   /* REGLABEL addr Rxx [Text]          */
  case infoUnregLabel :                 /* UNREGLABEL addr Rxx               */
    {
    string srange;
    string::size_type idx = value.find_first_of(" \t");
    if (idx == value.npos) idx = value.size();
    srange = value.substr(0, idx);
    value = (idx == value.size()) ? "" : triminfo(value.substr(idx), true, false);
    idx = value.find_first_of(" \t");
    if (idx == value.npos) idx = value.size();
    string sreg = lowercase(trim(value.substr(0, idx)));
    value = (idx == value.size()) ? "" : triminfo(value.substr(idx), true, false);
    int nRegNum = -1;
    if (sreg.size())
      {
      if (sreg[0] == 'r')
        sreg = sreg.substr(1);
      const char *startp = sreg.c_str();
      char *endp;
      long lres = strtol(startp, &endp, 10);
      if (lres >= 0 && lres <= 31 && endp != startp)
        nRegNum = (int)lres;
      }
    if (bus == BusCode && from != NO_ADDRESS && nRegNum >= 0)
      AddRegLabel(from, value, nRegNum);
    }
    break;
  case infoHigh :                       /* HIGH [BUS bus] addr[-addr] addr   */
  case infoLow :                        /* LOW [BUS bus] addr[-addr] addr    */
  case infoMHigh :                      /* MHIGH [BUS bus] addr[-addr] addr  */
  case infoMLow :                       /* MLOW [BUS bus] addr[-addr] addr   */
  case infoHigh2 :                      /* HIGH2 [BUS bus] addr[-addr] addr/2*/
  case infoLow2 :                       /* LOW2 [BUS bus] addr[-addr] addr/2 */
    {
    string srange;
    string::size_type idx = value.find_first_of(" \t");
    if (idx == value.npos) idx = value.size();
    srange = value.substr(0, idx);
    adr_t rel = NO_ADDRESS;
    value = (idx == value.size()) ? "" : trim(value.substr(idx));
    if (value.size() &&                 /* if addr[-addr] dp                 */
        value[0] != '*')
      String2Number(value, rel);
    
    if (from != NO_ADDRESS)
      {
      MemAttributeAvr8::RelType rt;
      switch (cmdType)
        {
        case infoHigh :
          rt = MemAttributeAvr8::RelHigh;
          break;
        case infoLow :
          rt = MemAttributeAvr8::RelLow;
          break;
        case infoMHigh :
          rt = MemAttributeAvr8::RelHighMinus;
          rel = -((int16_t)rel);
          break;
        case infoMLow :
          rt = MemAttributeAvr8::RelLowMinus;
          rel = -((int16_t)rel);
          break;
        case infoHigh2 :
          rt = MemAttributeAvr8::RelHigh2;
          rel *= 2;
          break;
        case infoLow2 :
          rt = MemAttributeAvr8::RelLow2;
          rel *= 2;
          break;
        default :  // keep gcc happy
          break;
        }

      // range check for rel in tgtbus missing.
      // if (tgtbus != bus)
        {
        adr_t *pmap = remaps[tgtbus].getat(rel);
        if (pmap) rel += *pmap;
        if (rel < GetLowestBusAddr(tgtbus) ||
            rel > GetHighestBusAddr(tgtbus))
          break;
        }

      for (adr_t scanned = from;
                  scanned >= from && scanned <= to;
                  scanned += step)
        SetRelative(scanned, rt, rel, tgtbus, bus);
      }
    }
    break;
  default :  // keep gcc happy
    break;
  }
return true;
}

/*****************************************************************************/
/* SetAvr8Option : sets a disassembler option                                */
/*****************************************************************************/

int DasmAvr8::SetAvr8Option(string lname, string value)
{
bool bnValue = true;                    /* default to "on"                   */
bool bIsBool = ParseBool(value, bnValue);
adr_t avalue;
String2Number(value, avalue);
int iValue = atoi(value.c_str());

if (lname == "codebits" && iValue >= 8 && iValue <= 32)
  busbits[BusCode] = iValue;
else if (lname == "highcode" && avalue >= 512 && avalue <= 0x3fffff)
  {
  highaddr[BusCode] = avalue;
  RecalcBusBits(BusCode);
  }
else if (lname == "highdata" && avalue >= 128 && avalue <= 0xffff)
  {
  highaddr[BusData] = avalue;
  RecalcBusBits(BusData);
  }
else if (lname == "highio" && avalue >= 1 && avalue <= 0x3f)
  highaddr[BusIO] = avalue;
else if (lname == "higheeprom" && avalue >= 128 && avalue <= 0xffff)
  {
  highaddr[BusEEPROM] = avalue;
  RecalcBusBits(BusEEPROM);
  }
else if (lname == "dbalign")
  {
  dbalign = bnValue;
  dbalignchg = true;
  return bIsBool ? 1 : 0;
  }
else if (lname == "avr-gcc")
  {
  static struct
	{
	int idx;
	const char *txt[2];
	} smne[] =
	{
	{ _d_byte,    { ".byte",    ".skip" } },
	{ _d_cseg,    { ".cseg",    ".text" /* ".section" */ } },
	{ _d_db,      { ".db",      ".byte" } },
    { _d_def,     { ".def",     "#define" } },
	// _d_device ".device" is unclear
	{ _d_dseg,    { ".dseg",    ".data" /* ".section" */ } },
	{ _d_dw,      { ".dw",      ".word" } },
	// _d_equ ".equ" should be identical
	// _d_eseg ".eseg" is unclear
	{ _d_exit,    { ".exit",    ".end" } },
	{ _d_include, { ".include", "#include" } },
	// _d_org ".org" should be identical
	// _d_port ".equ" should be identical
	// _d_set ".set" should be identical
	{ _d_string,  { ".db",      ".ascii" } },
	{ _d_stringz, { ".db",      ".asciz" } },
	};
  for (size_t i = 0; i < _countof(smne); i++)
    {
    if (mnemo[smne[i].idx].mne == smne[i].txt[avr_gcc])
      mnemo[smne[i].idx].mne = smne[i].txt[bnValue];
    }
  SetupRegnames(bnValue);
  avr_gcc = bnValue;
  if (!dbalignchg)
    dbalign = !avr_gcc;
  return bIsBool ? 1 : 0;
  }

return 1;                               /* name and value consumed           */
}

/*****************************************************************************/
/* GetAvr8Option : retrieves a disassembler option                           */
/*****************************************************************************/

string DasmAvr8::GetAvr8Option(string lname)
{
if (lname == "codebits") return sformat("%d", busbits[BusCode]);
if (lname == "highcode") return sformat("0x%x", highaddr[BusCode]);
if (lname == "highdata") return sformat("0x%x", highaddr[BusData]);
if (lname == "highio")   return sformat("0x%x", highaddr[BusIO]);
if (lname == "higheeprom") return sformat("0x%x", highaddr[BusEEPROM]);
if (lname == "dbalign") return dbalign ? "on" : "off";
if (lname == "avr-gcc") return avr_gcc ? "on" : "off";
return "";
}

/*****************************************************************************/
/* LoadAtmelGeneric loads an ATMEL Generic hex file                          */
/*****************************************************************************/

bool DasmAvr8::LoadAtmelGeneric(FILE *f, string &sLoadType, int bus)
{
int nCurPos = ftell(f);
int c = 0;

if ((c = fgetc(f)) == EOF)              /* look whether starting with hex chr*/
  return false;
ungetc(c, f);
if (!( (c >= '0' && c <= '9') ||
       (c >= 'a' && c <= 'f') ||
       (c >= 'A' && c <= 'F') ))
  return false;

bool done = false;
int nBytes = 0;
adr_t fend = GetLowestBusAddr(bus);
MemoryType memType = GetDefaultMemoryType(bus);

// ATMEL Generic Hex files consist of CR+LF-terminated lines
// Each line consists of hex values of the following format
// AAAAAA:DD[DD]
//   AAAAAA ... address
//   DDDD ..... (16-bit) data
// ... IOW, a HIGHLY inefficient format.

while (!done && (nBytes >= 0))          /* while there are lines             */
  {
  adr_t nAddr;
  uint16_t value = 0;
  int i;
  uint8_t chks = 0;
  nAddr = GetHex(f, 6);
  if (nAddr == NO_ADDRESS)
    break;
                                        /* if illegal address                */
  if ((nAddr < GetLowestBusAddr(bus)) ||
      (nAddr > GetHighestBusAddr(bus)))
    {
    nBytes = -1;                        /* return with error                 */
    break;
    }
  c = fgetc(f);
  if (c != ':')
    break;
  for (i = 0; i < 2; i++)               /* followed by up to 2 bytes of data */
    {
    adr_t b = GetHex(f, 2);
    if (b == NO_ADDRESS)
      break;
    value = (value << 8) | (uint16_t)b;
    }
  if (!i)
    break;
  AddMemory(nAddr, i, memType, 0, bus);
  nBytes += i;
  if (i == 1)
    setat(nAddr, (uint8_t)value, bus);
  else
    SetUWord(nAddr, value, bus);
  for (; i >= 0; i--)
    {
    // Hmmm. begin/end is a bit inadequate here, without the bus ...
    // Might need an update when this becomes remotely relevant
    bool bUsed = ((begin == NO_ADDRESS || (adr_t)i >= begin) &&
                  (end == NO_ADDRESS || (adr_t)i <= end));
    SetCellUsed(nAddr + i, bUsed, bus); /* mark as (un)used byte             */
    SetDisplay(nAddr + i, defaultDisplay, bus);
    }
  while ((c = fgetc(f)) == '\r' || c == '\n')
    ;
  if (c != EOF)
    ungetc(c, f);
  else
    done = true;

  (void)chks;  // unused ATM
  }

fseek(f, nCurPos, SEEK_SET);
if (nBytes >= 0)
  {
  offset = fend + 1;
  }

if (nBytes > 0)
  sLoadType = "Atmel Generic Hex";

return (nBytes > 0);                    /* pass back #bytes interpreted      */
}

/*****************************************************************************/
/* LoadFile : loads an opened file                                           */
/*****************************************************************************/

bool DasmAvr8::LoadFile(string filename, FILE *f, string &sLoadType, int interleave, int bus)
{
// Atmel special: a .eep file is an Intel Hex file for the EEPROM area
string::size_type fnlen = filename.size();
if (fnlen > 4 &&
    lowercase(filename.substr(fnlen - 4)) == ".eep")
  bus = BusEEPROM;

return LoadAtmelGeneric(f, sLoadType, bus) ||  // ATMEL Generic Hex files need no interleave
       Disassembler::LoadFile(filename, f, sLoadType, interleave, bus);
}

/*****************************************************************************/
/* String2Number : convert a string to a number in all known formats         */
/*****************************************************************************/

bool DasmAvr8::String2Number(string s, adr_t &value)
{
/* Standard formats for AVRASM :
   - a character is enclosed in '
   - a binary has a leading 0b
   - a hex constant has a leading 0x
*/
s = trim(s);
string hdr = s.substr(0, 2);
if (hdr == "0x")
  return (sscanf(s.substr(2).c_str(), "%x", &value) == 1);
else if (s[0] == '$')
  return (sscanf(s.substr(1).c_str(), "%x", &value) == 1);
#if 0
else if (s[0] == '@')
  return (sscanf(s.substr(1).c_str(), "%o", &value) == 1);
#endif
else if (s[0] == '\'' && s.size() > 1)
  {
  // this might be too primitive - typical C style things like '\t'
  // aren't caught correctly.
  value = s[1];
  return true;
  }
else if (hdr == "0b")
  {
  for (string::size_type i = 2; i < s.size(); i++)
    {
    char c = s[i];
    if (c >= '0' && c <= '1')
      value = (value << 1) + (c - '0');
    else
#if 1
      // terminate on invalid binary sequence
      return false;
#else
      // allow base class to check - might be a hex constant without 0x
      break;
#endif
    }
  }

// allow base class to check for others
return Disassembler::String2Number(s, value);
}

/*****************************************************************************/
/* Number2String : converts a number to a string in a variety of formats     */
/*****************************************************************************/

string DasmAvr8::Number2String
    (
    adr_t value,
    int nDigits,
    adr_t addr,
    int bus
    )
{
string s;

/* Standard formats for known AVR8 assemblers :
   - a character is enclosed in '
   - a binary has a leading 0b
   - a hex constant has a leading 0x
*/

MemAttribute::Type memType = GetCellType(addr, bus);
MemAttribute::Display disp;
bool bSigned = false;
if (memType == MemAttribute::CellUntyped)
  disp = MemAttribute::DefaultDisplay;
else
  {
  disp = GetDisplay(addr, bus);
  bSigned = IsSigned(addr, bus);
  }

if (disp == MemAttribute::DefaultDisplay)
  disp = defaultDisplay;

adr_t rel;
int relBus;
MemAttributeAvr8::RelType rt = GetRelative(addr, rel, relBus, bus);
if (rt != MemAttributeAvr8::RelUntyped)
  {
  // deal with LOW()/HIGH() in data (8bit) or code (16bit) format
  if (relBus == Avr8BusTypes)
    relBus = bus;
  Label *lbl = (rel == NO_ADDRESS) ? NULL : FindLabel(rel, Untyped, relBus);
  if (lbl)
    {
    string s = Label2String(rel, 4, true, addr, relBus);

    if ((s.find_first_of("+-") != string::npos) &&
        (rt == MemAttributeAvr8::RelLowMinus ||
         rt == MemAttributeAvr8::RelHighMinus ||
         rt == MemAttributeAvr8::RelLow2 ||
         rt == MemAttributeAvr8::RelHigh2))
      s = "(" + s + ")";

    if (rt == MemAttributeAvr8::RelLow2 ||
        rt == MemAttributeAvr8::RelHigh2)
      {
      if (avr_gcc)
        s = "gs(" + s + ")";
      else
        s += ">>1";
      }

    switch (rt)
      {
      case MemAttributeAvr8::RelLow :
      case MemAttributeAvr8::RelLowMinus :
      case MemAttributeAvr8::RelLow2 :
        s = sformat("%s(%s%s)",
                    MnemoCase(avr_gcc ? "lo8" : "LOW").c_str(),
                    (rt == MemAttributeAvr8::RelLowMinus) ? "-" : "",
                    s.c_str());
        return s;
      case MemAttributeAvr8::RelHigh :
      case MemAttributeAvr8::RelHighMinus :
      case MemAttributeAvr8::RelHigh2 :
        s = sformat("%s(%s%s)",
                    MnemoCase(avr_gcc ? "hi8" : "HIGH").c_str(),
                    (rt == MemAttributeAvr8::RelHighMinus) ? "-" : "",
                    s.c_str());
        return s;
      default :  // keep gcc happy
        break;
      }
    }
  }

if ((nDigits == 2) &&                   /* if 2-digit value                  */
    (disp == MemAttribute::Char))       /* and character output requested    */
  {
  if (isprint(value))
    s = sformat("'%c'", value);
  else
    s = sformat("0x%02x", value);
  }
else if (disp == MemAttribute::Binary)  /* if a binary                       */
  {
  int nBit;

  nDigits *= 4;                         /* convert from digits to bits       */
  s = "0b";                             /* prepare a binary value            */
                                        /* now do for all bits               */
  for (nBit = nDigits - 1; nBit >= 0; nBit--) 
    s.push_back('0' + (!!(value & (1 << nBit))));
  }
else if (disp == MemAttribute::Hex)     /* if hex                            */
  s = sformat("0x%0*x", nDigits, value);/* prepare a hex value               */
#if 0
else if (disp == MemAttribute::Octal)   /* if octal display                  */
  s = sformat("@%0*o", (nDigits * 4) + 2 / 3, value);
#endif
else                                    /* otherwise                         */
  {
  if (bSigned)
    {
    int32_t sval;  // sign extension, simple way
    if (nDigits == 2) sval = (int32_t)((int8_t)value);
    else if (nDigits == 4) sval = (int32_t)((int16_t)value);
    else sval = (int32_t)value;
    s = sformat("%d", sval);            /* prepare signed decimal value      */
    }
  else
    s = sformat("%u", value);           /* prepare unsigned decimal value    */
  }
return s;                               /* pass back generated string        */
}

/*****************************************************************************/
/* InitParse : initialize parsing                                            */
/*****************************************************************************/

bool DasmAvr8::InitParse(int bus)
{
return Disassembler::InitParse(bus);
}

/*****************************************************************************/
/* ParseData : parse data at given memory address for labels                 */
/*****************************************************************************/

adr_t DasmAvr8::ParseData
    (
    adr_t addr,
    int bus
    )
{
                                        /* mark DefLabels as used            */
SetLabelUsed(addr, Const, bus);

int csz = GetCellSize(addr, bus);
if (csz == 2)                           /* if WORD data                      */
  {
  if (!IsConst(addr, bus) && !IsBss(addr))
    SetLabelUsed(GetUWord(addr, bus), Untyped, bus, addr, bus /* TODO: correct?*/);
  }
return csz;
}

/*****************************************************************************/
/* DisassembleString : disassemble text string at given address              */
/*****************************************************************************/

int DasmAvr8::DisassembleString
    (
	adr_t addr,
	adr_t end,
	uint32_t flags,
	string &s,
	int maxparmlen,
	int bus
	)
{
adr_t done;
int sbytes = 0;

s = '"';                                /* start the game                    */
for (done = addr; done < end; done++)
  {                                     /* if this would become too long     */
  flags = GetDisassemblyTextFlags(done, bus);
  adr_t off = done - addr;
  if (flags & SHMF_TXT)
    {
    uint8_t *pc = getat(done, bus);
    string sc;
    // convert some specials
    switch (*pc)
      {
      case '\'' :
        if (avr_gcc)
          {
          sc = (char)*pc;
          break;
          }
        // else fall thru
      case '\"' :
        sc = string("\\") + (char)*pc;
        break;
      case '\t' :
        if (avr_gcc)
          sc = "\\t";
        else
          sc = (char)*pc;
        break;
      case '\r' :
        if (avr_gcc)
          sc = "\\r";
        else
          sc = (char)*pc;
        break;
      case '\n' :
        if (avr_gcc)
          sc = "\\n";
        else
          sc = (char)*pc;
        break;
      default :
        sc = (char)*pc;
      }
    if (s.size() + sc.size() + 2 >
            (string::size_type)maxparmlen &&
        sbytes &&
        (!dbalign || (off && !(off & 1))))
      break;                            /* terminate the loop                */
    sbytes++;
    s += sc;
    }
  else
    break;
  }
s += '"';                               /* append delimiter                  */

return sbytes;
}

/*****************************************************************************/
/* DisassembleData : disassemble data area at given memory address           */
/*****************************************************************************/

adr_t DasmAvr8::DisassembleData
    (
    adr_t addr,
    adr_t end,
    uint32_t flags,
    string &smnemo,
    string &sparm,
    int maxparmlen,
    int bus
    )
{
adr_t done;

if (flags & SHMF_RMB)                   /* if reserved memory block          */
  {
  done = end;                           /* remember it's done completely     */
  smnemo = mnemo[_d_byte].mne;
  sparm = Number2String(end - addr, 4, addr, bus);
  }
else if (flags & 0xff)                  /* if not byte-sized                 */
  {
  // AVRASM can only do byte and word, so assume word entities
  smnemo = mnemo[_d_dw].mne;
                                        /* assemble as many as possible      */
  for (done = addr; done < end; done += 2)
    {
    string s = Label2String(GetUWord(done, bus), 4,
                            !IsConst(done, bus),
                            done);
    if (sparm.size())                   /* if already something there        */
      {                                 /* if this would become too long     */
      if (sparm.size() + s.size() + 1 > (string::size_type)maxparmlen)
        break;                          /* terminate the loop                */
      sparm += ',';                     /* add separator                     */
      }
    sparm += s;                         /* append the byte's representation  */
    }
  }
else if (avr_gcc &&                     /* if .ascii allowed and text        */
         (GetDisassemblyTextFlags(addr, bus) & SHMF_TXT))
  {
  string s;
  smnemo = mnemo[_d_string].mne;        /* make this a string instead        */
  int sbytes = DisassembleString(addr, end, flags, s,
                                 maxparmlen - (int)sparm.size(), bus);
  done = addr + sbytes;
  if (!(GetDisassemblyFlags(done, bus) & (SHMF_BREAK | 0xff)))
    {
    sbytes++;
    done++;
    smnemo = mnemo[_d_stringz].mne;     /* make this a stringz instead       */
    }
  if (sparm.size())
    sparm += ",";
  sparm += s;
  }
else                                    /* if single-byte                    */
  {
  Label *deflbl;
  smnemo = mnemo[_d_db].mne;
                                        /* assemble as many as possible      */
  for (done = addr; done < end; done++)
    {
    adr_t off = done - addr;
    string s;
    flags = GetDisassemblyTextFlags(done, bus);
    // a DefLabel inhibits text output
    deflbl = FindLabel(done, Const, bus);
    if (deflbl)
      flags &= ~SHMF_TXT;

    if (flags & SHMF_TXT)
      {
      if (avr_gcc && (done - addr))     /* in avr-gcc, it's .byte OR .ascii  */
        break;
      int sbytes = DisassembleString(done, end, flags, s,
                                     maxparmlen - (int)sparm.size(),
                                     bus);
      done += sbytes;
      if (sbytes == 1)                  /* convert single character to 'x'   */
        {
        s[0] = '\'';
        s[s.size() - 1] = '\'';
        }
      done--;
      off = done - addr;
      }
    else if (deflbl)                    /* if deflabel set there             */
      s = deflbl->GetText();            /* take DefLabel text                */
    else
      s = Number2String(*getat(done, bus), 2, done, bus);
    if (sparm.size())                   /* if already something there        */
      {                                 /* if this would become too long     */
      if (sparm.size() + s.size() + 1 > (string::size_type)maxparmlen &&
          (!dbalign || (off && !(off & 1))))
        break;                          /* terminate the loop                */
      sparm += ',';                     /* add separator                     */
      }
    sparm += s;                         /* append the byte's representation  */
    }
  }

return done - addr;
}

/*****************************************************************************/
/* ParseCode : parse instruction at given memory address for labels          */
/*****************************************************************************/

adr_t DasmAvr8::ParseCode
    (
    adr_t addr,
    int bus
    )
{
uint16_t opcode = GetUWord(addr, bus);
int32_t opnds[2];
int i;

/* Look up the instruction in our instruction set */
avrInstructionInfo *ii = LookupInstruction(opcode);
if (ii == NULL)
  return 2;

int numOperands = ii->numOperands;
for (i = 0; i < numOperands; i++)
  {
  uint32_t o = GetMaskBits(opcode, ii->operandMasks[i]);
  /* Append the extra bits if it's a long operand */
  if (ii->operandTypes[i] == OpndLongAbsAddr ||
      ii->operandTypes[i] == OpndLongAbsAddrData)
    {
    o <<= 16;
    o |= GetUWord(addr + 2, bus);
    }
  opnds[i] = DecodeOperand(o, ii->operandTypes[i]);
  }

for (i = 0; i < numOperands; i++)
  {
  int32_t operand = opnds[i];
  /* Print the operand */
  switch (ii->operandTypes[i])
    {
    case OpndReg:
    case OpndRegStartR16:
    case OpndRegEvenPair:
    case OpndRegEvenPStartR24:
      break;
    case OpndIOReg:
      SetLabelUsed(operand, Untyped, BusIO, addr);
      break;
    case OpndBit:
    case OpndDESRound:
      {
      Label *lbl = SetDefLabelUsed(addr, bus);
      if (lbl)
        AddDefLabel(addr, lbl->GetText(),
                    Number2String(operand, 2, addr, bus),
                    Const, bus);
      }
      break;
    case OpndX:
      break;
    case OpndXPlus:
      break;
    case OpndMinusX:
      break;
    case OpndY:
      break;
    case OpndYPlus:
      break;
    case OpndMinusY:
      break;
    case OpndZ:
      break;
    case OpndZPlus:
      break;
    case OpndMinusZ:
      break;
    case OpndYPlusQ:
      break;
    case OpndZPlusQ:
      break;
    case OpndData:
      {
      adr_t rel; int relBus;
      // if this is relative, make sure a label for the relation is there
      MemAttributeAvr8::RelType rt = GetRelative(addr, rel, relBus, bus);
      if (rt != MemAttributeAvr8::RelUntyped)
        AddRelativeLabel(rel, addr, relBus ? Data : Code, true, relBus, addr);

      Label *lbl = SetDefLabelUsed(addr, bus);
      if (lbl)
        AddDefLabel(addr, lbl->GetText(),
                    Number2String(operand, 2, addr, bus),
                    Const, bus);
      }
      break;
      // fall thru on purpose
    case OpndLongAbsAddr:
    case OpndLongAbsAddrData:
      bus = (mnemo[ii->mnemonic].memType == Code) ? (int)BusCode : (int)BusData;
      if (!IsConst(addr, BusData /*bus*/))
        AddRelativeLabel(operand, addr, mnemo[ii->mnemonic].memType, true, bus, addr);
      break;
    case OpndBranchAddr:
    case OpndRelAddr:
      bus = (mnemo[ii->mnemonic].memType == Code) ? (int)BusCode : (int)BusData;
      if (!IsConst(addr, bus))
        AddRelativeLabel(operand + addr + 2, addr, mnemo[ii->mnemonic].memType, true, bus, addr);
      break;
    default:
      break;
    }

  }

return ii->width;
}

/*****************************************************************************/
/* DisassembleCode : disassemble code instruction at given memory address    */
/*****************************************************************************/

adr_t DasmAvr8::DisassembleCode
    (
    adr_t addr,
    string &smnemo,
    string &sparm,
    int bus
    )
{
uint16_t opcode = GetUWord(addr, bus);
int32_t opnds[2];
int i;

/* Look up the instruction in our instruction set */
avrInstructionInfo *ii = LookupInstruction(opcode);
if (ii == NULL)                         /* if no instruction,                */
  {
  smnemo = mnemo[_d_dw].mne;
  sparm = Number2String(opcode, 4, addr, bus);
  return 2;
  }

int numOperands = ii->numOperands;
for (i = 0; i < numOperands; i++)
  {
  uint32_t o = GetMaskBits(opcode, ii->operandMasks[i]);
  // Append the extra bits if it's a long operand
  if (ii->operandTypes[i] == OpndLongAbsAddr ||
      ii->operandTypes[i] == OpndLongAbsAddrData)
    {
    o <<= 16;
    o |= GetUWord(addr + 2, bus);
    }
  opnds[i] = DecodeOperand(o, ii->operandTypes[i]);
  }

smnemo = mnemo[ii->mnemonic].mne;
// specials
if (ii->mnemonic == _eor && opnds[0] == opnds[1])
  {
  smnemo = mnemo[_clr].mne;             /* eor r,r -> clr r                  */
  numOperands = 1;
  }
else if (ii->mnemonic == _add && opnds[0] == opnds[1])
  {
  smnemo = mnemo[_lsl].mne;             /* add r,r -> lsl r                  */
  numOperands = 1;
  }
else if (ii->mnemonic == _adc && opnds[0] == opnds[1])
  {
  smnemo = mnemo[_rol].mne;             /* adc r,r -> rol r                  */
  numOperands = 1;
  }
else if (ii->mnemonic == _and && opnds[0] == opnds[1])
  {
  smnemo = mnemo[_tst].mne;             /* and r,r -> tst r                  */
  numOperands = 1;
  }
else if (ii->mnemonic == _andi)
  {
  int bset = 0;
  for (int j = 0; j < 8; j++)
    {
    if (opnds[1] & (1 << j))
      bset++;
    }
  if (bset > 6)                         /* if < 3 bits clear                  */
    {
    smnemo = mnemo[_cbr].mne;           /* andi r,x -> cbr r,~x               */
    opnds[1] ^= 0xff;
    }
  }

Label *lbl;
for (i = 0; i < numOperands; i++)
  {
  int32_t operand = opnds[i];
  if (i)
    sparm += ",";

  /* Print the operand */
  switch (ii->operandTypes[i])
    {
    case OpndReg:
    case OpndRegStartR16:
    case OpndRegEvenPair:
    case OpndRegEvenPStartR24:
      sparm += RegName(operand);
      break;
    case OpndIOReg:
      sparm += IORegName(operand);
      break;
    case OpndBit:
      lbl = FindLabel(addr, Const, bus);
      sparm += lbl ? lbl->GetText() : sformat("%d", operand);
      break;
    case OpndDESRound:
      lbl = FindLabel(addr, Const, bus);
      sparm += lbl ? lbl->GetText() : sformat("0x%x", operand);
      break;
    case OpndX:
      sparm += MnemoCase("X");
      break;
    case OpndXPlus:
      sparm += MnemoCase("X+");
      break;
    case OpndMinusX:
      sparm += MnemoCase("-X");
      break;
    case OpndY:
      sparm += MnemoCase("Y");
      break;
    case OpndYPlus:
      sparm += MnemoCase("Y+");
      break;
    case OpndMinusY:
      sparm += MnemoCase("-Y");
      break;
    case OpndZ:
      sparm += MnemoCase("Z");
      break;
    case OpndZPlus:
      sparm += MnemoCase("Z+");
      break;
    case OpndMinusZ:
      sparm += MnemoCase("-Z");
      break;
    case OpndYPlusQ:
      sparm += sformat("%s+%d", MnemoCase("Y").c_str(), operand);
      break;
    case OpndZPlusQ:
      sparm += sformat("%s+%d", MnemoCase("Z").c_str(), operand);
      break;
    case OpndData:
      {
      Label *lbl = FindLabel(addr, Const, bus);
      sparm += lbl ? lbl->GetText() : Number2String(operand, 2, addr, bus);
      }
      break;
    case OpndLongAbsAddrData:
    case OpndLongAbsAddr:
      sparm += Label2String(operand, 4, true, addr,
          (mnemo[ii->mnemonic].memType == Code) ? (int)BusCode : (int)BusData);
      break;
    case OpndBranchAddr:
    case OpndRelAddr:
      sparm += Label2String(operand + addr + 2, 4, true, addr,
          (mnemo[ii->mnemonic].memType == Code) ? (int)BusCode : (int)BusData);
      break;
    default:
      break;
    }
  }

return ii->width;
}

/*****************************************************************************/
/* DisassembleLabel : disassemble used external labels                       */
/*****************************************************************************/

bool DasmAvr8::DisassembleLabel
    (
    Label *label,
    string &slabel,
    string &smnemo,
    string &sparm,
    int bus
    )
{
(void)slabel;
if (label->GetText().find_first_of("+-") == string::npos)
  {
  adr_t laddr = label->GetAddress();
  smnemo = mnemo[(bus == BusIO) ? _d_port : _d_equ].mne;
  sparm = Label2String(laddr, 4, true, laddr, bus) + sequdelim[avr_gcc] +
          Address2String(laddr, bus);
  Disassembler::DisassembleLabel(label, slabel, smnemo, sparm, bus);
  return true;
  }
return false;
}

/*****************************************************************************/
/* DisassembleDefLabel : pass back mnemonic and parameters for a DefLabel    */
/*****************************************************************************/

bool DasmAvr8::DisassembleDefLabel
    (
    DefLabel *label,
    string &slabel,
    string &smnemo,
    string &sparm,
    int bus
    )
{
(void)bus;
if (avr_gcc)
  {
  // for avr-gcc, it's easier to emit a #define in this case.
  slabel = mnemo[_d_def].mne;
  smnemo = label->GetText();
  sparm = label->GetDefinition();
  }
else
  {
  // in theory, we could use .set instead of .equ here, but since the
  // ATMEL .inc-files for the various AVR processors work with .equ, I
  // follow that route that, too.
  // Besides, the benefit of .set is that you can redefine symbols, which is
  // not done in dasmfw.
  smnemo = mnemo[_d_equ].mne;
  sparm = label->GetText() + sequdelim[avr_gcc] + label->GetDefinition();
  }
Disassembler::DisassembleDefLabel(label, slabel, smnemo, sparm, bus);
return true;
}

/*****************************************************************************/
/* DisassembleChanges : report dasm-specific state changes before/after addr */
/*****************************************************************************/

bool DasmAvr8::DisassembleChanges
    (
    adr_t addr,
    adr_t prevaddr,
    adr_t prevsz,
    bool bAfterLine,
    vector<LineChange> &changes,
    int bus
    )
{
// init / exit
if (addr == NO_ADDRESS && prevaddr == NO_ADDRESS)
  {
  if (!bAfterLine)                      /* start of disassembly output       */
    {
    }
  else                                  /* end of disassembly output         */
    {
#if 0
// not in AVRASM - we could use .exit, but that's not completely the same
// and since there's no entry point to be set anyway, it's unnecessary
    LineChange chg;
    changes.push_back(chg);             /* append empty line before END      */
    chg.oper = MnemoCase(mnemo[_d_exit].mne);
#if 0
    if (load != NO_ADDRESS &&           /* if entry point address given      */
        bLoadLabel)                     /* and labelling wanted              */
      chg.opnds = Label2String(load, 4, true, load);
#endif
    changes.push_back(chg);
#endif
    }
  }
else
  {
  if (prevaddr == NO_ADDRESS)           /* if initial for current bus        */
    {
    if (!bAfterLine)
      {
      LineChange chg;
      changes.push_back(chg);           /* append empty line before segment  */
      if (bus == BusCode || bus == BusData || bus == BusEEPROM)
        {
        chg.oper = MnemoCase(mnemo[(bus == BusEEPROM) ? _d_eseg :
                             (bus == BusData) ? _d_dseg :
                             _d_cseg].mne);
        changes.push_back(chg);
        }
      }
    }
  else if (addr != prevaddr + prevsz)
    {
    if (!bAfterLine)
      {
      // no phasing in AVR8

      LineChange chg;
      changes.push_back(chg);
      if (addr != NO_ADDRESS)
        {
        chg.oper = MnemoCase(mnemo[_d_org].mne);
        chg.opnds = Number2String(addr, 4, NO_ADDRESS, bus);
        changes.push_back(chg);
        changes.push_back(LineChange());
        }
      }
    }
  // process RegLabels
  if (!bAfterLine && bus == BusCode)
    {
    Avr8RegLabelArray::iterator it;
    Avr8RegLabel *lbl = GetFirstRegLabel(addr, it);
    bool bGotLbl = false;
    while (lbl)
      {
      string txt = lbl->GetText();
      CurRegLabel[lbl->GetRegister()] = txt;
      if (txt.size())
        {
        LineChange chg;
        if (!bGotLbl)
          {
          changes.push_back(chg);
          bGotLbl = true;
          }
        if (avr_gcc)
          {
          // this is a specialty, so it's hardcoded here.
          chg.label = "#ifdef";
          chg.oper = txt;
          changes.push_back(chg);
          chg.label = "#undef";
          changes.push_back(chg);
          LineChange chg2;
          chg2.label = "#endif";
          changes.push_back(chg2);
          chg.label = MnemoCase(mnemo[_d_def].mne);
          chg.opnds = RegName(lbl->GetRegister(), false);
          }
        else
          {
          chg.oper = MnemoCase(mnemo[_d_def].mne);
          chg.opnds = sformat("%s%sR%d", txt.c_str(), sdefdelim[avr_gcc], lbl->GetRegister());
          }
        changes.push_back(chg);
        }
      lbl = GetNextRegLabel(addr, it);
      }
    if (bGotLbl)
      changes.push_back(LineChange());
    }
  }

return Disassembler::DisassembleChanges(addr, prevaddr, prevsz, bAfterLine, changes, bus);
}

/*****************************************************************************/
/* LookupInstruction : look up the instruction definition for an opcode      */
/*****************************************************************************/

avrInstructionInfo *DasmAvr8::LookupInstruction(uint16_t opcode)
{
for (size_t i = 0; i < _countof(AVR_Instruction_Set); i++)
  {
  uint16_t instructionBits = opcode;

  // Mask out the operands from the opcode
  for (int j = 0; j < AVR_Instruction_Set[i].numOperands; j++)
    instructionBits &= ~(AVR_Instruction_Set[i].operandMasks[j]);

  // Compare left over instruction bits with the instruction mask
  if (instructionBits == AVR_Instruction_Set[i].instructionMask)
    return &AVR_Instruction_Set[i];
  }

return NULL;
}

/*****************************************************************************/
/* DecodeOperand : decodes an operand, based on its type                     */
/*****************************************************************************/

int32_t DasmAvr8::DecodeOperand(uint32_t operand, uint8_t operandType)
{
int32_t operandDisasm;

switch (operandType)
  {
  case OpndBranchAddr:
    /* Relative branch address is 7 bits, two's complement form */

    /* If the sign bit is set */
    if (operand & (1 << 6))
      {
      /* Manually sign-extend to the 32-bit container */
      operandDisasm = (int32_t) ( ( ~operand + 1 ) & 0x7f );
      operandDisasm = -operandDisasm;
      }
    else
      operandDisasm = (int32_t) ( operand & 0x7f );
    /* Multiply by two to point to a byte address */
    operandDisasm *= 2;
    break;
  case OpndRelAddr:
    /* Relative address is 12 bits, two's complement form */

    /* If the sign bit is set */
    if (operand & (1 << 11))
      {
      /* Manually sign-extend to the 32-bit container */
      operandDisasm = (int32_t) ( ( ~operand + 1 ) & 0xfff );
      operandDisasm = -operandDisasm;
      }
    else
      operandDisasm = (int32_t) ( operand & 0xfff );
    /* Multiply by two to point to a byte address */
    operandDisasm *= 2;
    break;
  case OpndLongAbsAddr:
    /* Multiply by two to point to a byte address */
    operandDisasm = operand * 2;
    break;
  case OpndRegStartR16:
    /* Register offset from R16 */
    operandDisasm = 16 + operand;
    break;
  case OpndRegEvenPair:
    /* Register even */
    operandDisasm = operand * 2;
    break;
  case OpndRegEvenPStartR24:
    /* Register even offset from R24 */
    operandDisasm = 24 + operand * 2;
    break;
  default:
    /* Copy the operand with no additional processing */
    operandDisasm = operand;
    break;
  }

return operandDisasm;
}
