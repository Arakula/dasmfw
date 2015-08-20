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

static Disassembler *CreateAvr8()
{
Disassembler *pDasm = new DasmAvr8;
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

DasmAvr8::DasmAvr8()
{
commentStart = ";";
labelDelim = ":";

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

busnames[BusData]   = "Data";
busnames[BusIO]     = "IO";
busnames[BusEEPROM] = "EEPROM";

return bOK;
}

/*****************************************************************************/
/* InfoHelp : print disassembler-specific info file help                     */
/*****************************************************************************/

std::string DasmAvr8::InfoHelp()
{
return "REGLABEL addr Reg [text]\n"
       "UNREGLABEL addr Reg\n"
       ;
}

/*****************************************************************************/
/* ProcessInfo : process an info file line                                   */
/*****************************************************************************/

bool DasmAvr8::ProcessInfo
    (
    std::string key,                    /* parsed key                        */
    std::string value,                  /* rest of the line                  */
    addr_t &from,                       /* from/to (if range given)          */
    addr_t &to, 
    bool bProcInfo,                     /* flag whether processing           */
    int bus                         /* target bus                        */
    )
{
if (!bProcInfo)                         /* only if processing                */
  return false;

enum InfoCmd
  {
  infoUnknown = -1,                     /* unknown info command              */
  infoRegLabel,                         /* REGLABEL addr Rxx [Text]          */
  infoUnregLabel,                       /* UNREGLABEL addr Rxx               */
  };
static struct                           /* structure to convert key to type  */
  {
  const char *name;
  InfoCmd cmdType;
  } sKey[] =
  {
  { "REGLABEL",     infoRegLabel },
  { "UNREGLABEL",   infoUnregLabel },
  };

InfoCmd cmdType = infoUnknown;
for (int i = 0; i < _countof(sKey); i++)
  if (key == sKey[i].name)
    {
    cmdType = sKey[i].cmdType;
    break;
    }
if (cmdType == infoUnknown)
  return false;

switch (cmdType)
  {
  case infoRegLabel :                   /* REGLABEL addr Rxx [Text]          */
  case infoUnregLabel :                 /* UNREGLABEL addr Rxx               */
    {
    std::string srange;
    std::string::size_type idx = value.find_first_of(" \t");
    if (idx == value.npos) idx = value.size();
    srange = value.substr(0, idx);
    value = (idx == value.size()) ? "" : triminfo(value.substr(idx), true, false);
    idx = value.find_first_of(" \t");
    if (idx == value.npos) idx = value.size();
    std::string sreg = lowercase(trim(value.substr(0, idx)));
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
  }
return true;
}

/*****************************************************************************/
/* SetAvr8Option : sets a disassembler option                                */
/*****************************************************************************/

int DasmAvr8::SetAvr8Option(std::string lname, std::string value)
{
std::string lvalue(lowercase(value));
int bnvalue = (lvalue == "off") ? 0 : (lvalue == "on") ? 1 : atoi(value.c_str());
addr_t avalue;
String2Number(value, avalue);

if (lname == "codebits" && bnvalue >= 8 && bnvalue <= 32)
  busbits[BusCode] = bnvalue;
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

return 1;                               /* name and value consumed           */
}

/*****************************************************************************/
/* GetAvr8Option : retrieves a disassembler option                           */
/*****************************************************************************/

std::string DasmAvr8::GetAvr8Option(std::string lname)
{
if (lname == "codebits") return sformat("%d", busbits[BusCode]);
if (lname == "highcode") return sformat("0x%x", highaddr[BusCode]);
if (lname == "highdata") return sformat("0x%x", highaddr[BusData]);
if (lname == "highio")   return sformat("0x%x", highaddr[BusIO]);
if (lname == "higheeprom") return sformat("0x%x", highaddr[BusEEPROM]);
return "";
}

/*****************************************************************************/
/* LoadAtmelGeneric loads an ATMEL Generic hex file                          */
/*****************************************************************************/

bool DasmAvr8::LoadAtmelGeneric(FILE *f, std::string &sLoadType, int bus)
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
addr_t fbegin = GetHighestBusAddr(bus);
addr_t fend = GetLowestBusAddr(bus);
MemoryType memType = GetDefaultMemoryType(bus);

// ATMEL Generic Hex files consist of CR+LF-terminated lines
// Each line consists of hex values of the following format
// AAAAAA:DD[DD]
//   AAAAAA ... address
//   DDDD ..... (16-bit) data
// ... IOW, a HIGHLY inefficient format.

while (!done && (nBytes >= 0))          /* while there are lines             */
  {
  addr_t nAddr;
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
    addr_t b = GetHex(f, 2);
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
    SetCellUsed(nAddr + i, true, bus);  /* mark as used byte                 */
    SetDisplay(nAddr + i, defaultDisplay, bus);
    }
  while ((c = fgetc(f)) == '\r' || c == '\n')
    ;
  if (c != EOF)
    ungetc(c, f);
  else
    done = true;
  }

fseek(f, nCurPos, SEEK_SET);
if (nBytes >= 0)
  {
  if (fbegin < begin)
    begin = fbegin;
  if (fend > end)
    end = fend;
  }

if (nBytes > 0)
  sLoadType = "Atmel Generic Hex";
return (nBytes > 0);                    /* pass back #bytes interpreted      */
}

/*****************************************************************************/
/* LoadFile : loads an opened file                                           */
/*****************************************************************************/

bool DasmAvr8::LoadFile(std::string filename, FILE *f, std::string &sLoadType, int interleave, int bus)
{
// Atmel special: a .eep file is an Intel Hex file for the EEPROM area
std::string::size_type fnlen = filename.size();
if (fnlen > 4 &&
    lowercase(filename.substr(fnlen - 4)) == ".eep")
  bus = BusEEPROM;

return LoadAtmelGeneric(f, sLoadType, bus) ||  // ATMEL Generic Hex files need no interleave
       Disassembler::LoadFile(filename, f, sLoadType, interleave, bus);
}

/*****************************************************************************/
/* String2Number : convert a string to a number in all known formats         */
/*****************************************************************************/

bool DasmAvr8::String2Number(std::string s, addr_t &value)
{
/* Standard formats for known 680x assemblers :
   - a character is enclosed in '
   - a binary has a leading 0b
   - a hex constant has a leading 0x
*/
s = trim(s);
std::string hdr = s.substr(0, 2);
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
  for (std::string::size_type i = 2; i < s.size(); i++)
    {
    char c = s[i];
    if (c >= '0' && c <= '1')
      value = (value << 1) + (c - '0');
    else
      return false;
    }
  }

// allow base class to check for others
return Disassembler::String2Number(s, value);
}

/*****************************************************************************/
/* Number2String : converts a number to a string in a variety of formats     */
/*****************************************************************************/

std::string DasmAvr8::Number2String
    (
    addr_t value,
    int nDigits,
    addr_t addr,
    int bus
    )
{
std::string s;

/* Standard formats for known AVR8 assemblers :
   - a character is enclosed in '
   - a binary has a leading 0b
   - a hex constant has a leading 0x
*/

MemoryType memType = GetMemType(addr, bus);
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

addr_t DasmAvr8::ParseData
    (
    addr_t addr,
    int bus
    )
{
SetLabelUsed(addr, Const, bus);         /* mark DefLabels as used            */

int csz = GetCellSize(addr, bus);
if (csz == 2)                           /* if WORD data                      */
  {
  if (!IsConst(addr, bus))
    SetLabelUsed(GetUWord(addr, bus), Untyped, bus);
  }
return csz;
}

/*****************************************************************************/
/* DisassembleData : disassemble data area at given memory address           */
/*****************************************************************************/

addr_t DasmAvr8::DisassembleData
    (
    addr_t addr,
    addr_t end,
    uint32_t flags,
    std::string &smnemo,
    std::string &sparm,
    int maxparmlen,
    int bus
    )
{
addr_t done;

if (flags & SHMF_RMB)                   /* if reserved memory block          */
  {
  done = end;                           /* remember it's done completely     */
  smnemo = opcodes[_d_byte].mne;
  sparm = Number2String(end - addr, 4, addr, bus);
  }
else if (flags & 0xff)                  /* if not byte-sized                 */
  {
  // AVRASM can only do byte and word, so assume word entities
  smnemo = opcodes[_d_dw].mne;
                                        /* assemble as many as possible      */
  for (done = addr; done < end; done += 2)
    {
    std::string s = Label2String(GetUWord(done, bus),
                                 !IsConst(done, bus),
                                 done);
    if (sparm.size())                   /* if already something there        */
      {                                 /* if this would become too long     */
      if (sparm.size() + s.size() + 1 > (std::string::size_type)maxparmlen)
        break;                          /* terminate the loop                */
      sparm += ',';                     /* add separator                     */
      }
    sparm += s;                         /* append the byte's representation  */
    }
  }
else                                    /* if single-byte                    */
  {
  Label *deflbl;
  smnemo = opcodes[_d_db].mne;
                                        /* assemble as many as possible      */
  for (done = addr; done < end; done++)
    {
    std::string s;
    flags = GetDisassemblyTextFlags(done, bus);
    // a DefLabel inhibits text output
    deflbl = FindLabel(done, Const, bus);
    if (deflbl)
      flags &= ~SHMF_TXT;

    if (flags & SHMF_TXT)
      {
      s = '"';                          /* start the game                    */
      for ( ; done < end; done++)
        {                               /* if this would become too long     */
        flags = GetDisassemblyTextFlags(done, bus);
        if (flags & SHMF_TXT)
          {
          uint8_t *pc = getat(done, bus);
          std::string sc;
          switch (*pc)
            {
            case '\t' :
              sc = "\\t";
              flags |= SHMF_TXT;
              break;
            case '\r' :
              sc = "\\r";
              flags |= SHMF_TXT;
              break;
            case '\n' :
              sc = "\\n";
              flags |= SHMF_TXT;
              break;
            default :
              sc = (char)*pc;
            }
          // convert some specials
          if (sparm.size() + s.size() + sc.size() + 2 > (std::string::size_type)maxparmlen)
            break;                      /* terminate the loop                */
          s += sc;
          }
        else
          break;
        }
      s += '"';                         /* append delimiter                  */
      done--;
      }
    else if (deflbl)                    /* if deflabel set there             */
      s = deflbl->GetText();            /* take DefLabel text                */
    else
      s = Number2String(*getat(done, bus), 2, done, bus);
    if (sparm.size())                   /* if already something there        */
      {                                 /* if this would become too long     */
      if (sparm.size() + s.size() + 1 > (std::string::size_type)maxparmlen)
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

addr_t DasmAvr8::ParseCode
    (
    addr_t addr,
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
      SetLabelUsed(operand, Untyped, BusIO);
      break;
    case OpndBit:
    case OpndDESRound:
      {
      Label *lbl = FindLabel(addr, Const, bus);
      if (lbl)
        {
        lbl->SetUsed();
        AddDefLabel(addr, lbl->GetText(),
                    Number2String(operand, 2, addr, bus),
                    Const, bus);
        }
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
      Label *lbl = FindLabel(addr, Const, bus);
      if (lbl)
        {
        lbl->SetUsed();
        AddDefLabel(addr, lbl->GetText(),
                    Number2String(operand, 2, addr, bus),
                    Const, bus);
        }
      }
      break;
      // fall thru on purpose
    case OpndLongAbsAddr:
    case OpndLongAbsAddrData:
      bus = (mnemo[ii->mnemonic].memType != Code) ? BusData : BusCode;
      if (!IsConst(addr, BusData /*bus*/))
        AddLabel(operand, mnemo[ii->mnemonic].memType, "", true,
                 bus);
      break;
    case OpndBranchAddr:
    case OpndRelAddr:
      bus = (mnemo[ii->mnemonic].memType != Code) ? BusData : BusCode;
      if (!IsConst(addr, bus))
        AddLabel(operand + addr + 2, mnemo[ii->mnemonic].memType, "", true,
                 bus);
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

addr_t DasmAvr8::DisassembleCode
    (
    addr_t addr,
    std::string &smnemo,
    std::string &sparm,
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
  smnemo = ".dw";
  sparm = Number2String(opcode, 4, addr, bus);
  return 2;
  }

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
  int32_t chk = opnds[1];
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
      sparm += "X";
      break;
    case OpndXPlus:
      sparm += "X+";
      break;
    case OpndMinusX:
      sparm += "-X";
      break;
    case OpndY:
      sparm += "Y";
      break;
    case OpndYPlus:
      sparm += "Y+";
      break;
    case OpndMinusY:
      sparm += "-Y";
      break;
    case OpndZ:
      sparm += "Z";
      break;
    case OpndZPlus:
      sparm += "Z+";
      break;
    case OpndMinusZ:
      sparm += "-Z";
      break;
    case OpndYPlusQ:
      sparm += sformat("Y+%d", operand);
      break;
    case OpndZPlusQ:
      sparm += sformat("Z+%d", operand);
      break;
    case OpndData:
      {
      Label *lbl = FindLabel(addr, Const, bus);
      sparm += lbl ? lbl->GetText() : Number2String(operand, 2, addr, bus);
      }
      break;
    case OpndLongAbsAddrData:
    case OpndLongAbsAddr:
      sparm += Label2String(operand, true, addr,
                            (mnemo[ii->mnemonic].memType != Code) ? BusData : BusCode /*bus*/);
      break;
    case OpndBranchAddr:
    case OpndRelAddr:
      sparm += Label2String(operand + addr + 2, true, addr,
                            (mnemo[ii->mnemonic].memType != Code) ? BusData : BusCode /*bus*/);
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
    std::string &slabel,
    std::string &smnemo,
    std::string &sparm,
    int bus
    )
{
if (label->GetText().find_first_of("+-") == std::string::npos)
  {
  addr_t laddr = label->GetAddress();
  smnemo = opcodes[(bus == BusIO) ? _d_port : _d_equ].mne;
  sparm = Label2String(laddr, true, laddr, bus) + " = " +
          Address2String(laddr, bus);
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
    std::string &slabel,
    std::string &smnemo,
    std::string &sparm,
    int bus
    )
{
// in theory, we could use .set instead of .equ here, but since the
// ATMEL .inc-files for the various AVR processors work with .equ, I
// follow that route that, too.
// Besides, the benefit of .set is that you can redefine symbols, which is
// not done in dasmfw.
smnemo = opcodes[_d_equ].mne;
sparm = label->GetText() + " = " + label->GetDefinition();
return true;
}

/*****************************************************************************/
/* DisassembleChanges : report dasm-specific state changes before/after addr */
/*****************************************************************************/

bool DasmAvr8::DisassembleChanges
    (
    addr_t addr,
    addr_t prevaddr,
    addr_t prevsz,
    bool bAfterLine,
    std::vector<LineChange> &changes,
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
    chg.oper = opcodes[_d_exit].mne;
#if 0
    if (load != NO_ADDRESS &&           /* if entry point address given      */
        bLoadLabel)                     /* and labelling wanted              */
      chg.opnds = Label2String(load, true, load);
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
        chg.oper = opcodes[(bus == BusEEPROM) ? _d_eseg :
                           (bus == BusData) ? _d_dseg :
                           _d_cseg].mne;
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
        chg.oper = opcodes[_d_org].mne;
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
      std::string txt = lbl->GetText();
      CurRegLabel[lbl->GetRegister()] = txt;
      if (txt.size())
        {
        LineChange chg;
        if (!bGotLbl)
          {
          changes.push_back(chg);
          bGotLbl = true;
          }
        chg.oper = opcodes[_d_def].mne;
        chg.opnds = sformat("%s = R%d", txt.c_str(), lbl->GetRegister());
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
for (int i = 0; i < _countof(AVR_Instruction_Set); i++)
  {
  uint16_t instructionBits = opcode;

  /* Mask out the operands from the opcode */
  for (int j = 0; j < AVR_Instruction_Set[i].numOperands; j++)
    instructionBits &= ~(AVR_Instruction_Set[i].operandMasks[j]);

  /* Compare left over instruction bits with the instruction mask */
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
