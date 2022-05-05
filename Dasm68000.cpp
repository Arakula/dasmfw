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
 * The 68000 disassembler engine is loosely based on Jim Patchell's code,  *
 * last found at http://www.oldcrows.net/~patchell/uproc/uproc.html        *
 ***************************************************************************/

/*****************************************************************************/
/* Dasm68000.cpp : 68000 disassembler implementation                         */
/*****************************************************************************/

#include "Dasm68000.h"

/*****************************************************************************/
/* Create68000 : create an 68000 processor handler                           */
/*****************************************************************************/

static Disassembler *Create68000(Application *pApp)
{
Disassembler *pDasm = new Dasm68000(pApp);
if (pDasm) pDasm->Setup();
return pDasm;
}

/*****************************************************************************/
/* Create68008 : create an 68008 processor handler                           */
/*****************************************************************************/

static Disassembler *Create68008(Application *pApp)
{
Disassembler *pDasm = new Dasm68008(pApp);
if (pDasm) pDasm->Setup();
return pDasm;
}

/*****************************************************************************/
/* Auto-registration                                                         */
/*****************************************************************************/

static bool bRegistered[] =
  {
  RegisterDisassembler("68000", Create68000),
  // RegisterDisassembler("68k",   Create68000),
  RegisterDisassembler("68008", Create68008),
  };

/*===========================================================================*/
/* Dasm68000 class members                                                   */
/*===========================================================================*/

Dasm68000::OpDef Dasm68000::OpTable[optblSize] =
  {
    { _abcd,    0xc100, 0xf1f0, optype_14 },    // 0
    { _adda,    0xd0c0, 0xf0c0, optype_03 },
    { _addx,    0xd100, 0xf130, optype_14 },
    { _add,     0xd000, 0xf000, optype_03 },
    { _cmpa,    0xb0c0, 0xf0c0, optype_03 },
    { _cmpm,    0xb108, 0xf138, optype_19 },
    { _eor,     0xb100, 0xf100, optype_03 },
    { _cmp,     0xb000, 0xf100, optype_03 },
    { _reset,   0x4e70, 0xffff, optype_10 },
    { _rte,     0x4e73, 0xffff, optype_10 },
    { _rtr,     0x4e77, 0xffff, optype_10 },    // 10
    { _rts,     0x4e75, 0xffff, optype_10 },
    { _stop,    0x4e72, 0xffff, optype_25 },
    { _trapv,   0x4e76, 0xffff, optype_10 },
    { _ill,     0x4afc, 0xffff, optype_10 },
    { _nop,     0x4e71, 0xffff, optype_10 },
    { _swap,    0x4840, 0xfff8, optype_05 },
    { _unlk,    0x4e58, 0xfff8, optype_09 },
    { _link,    0x4e50, 0xfff8, optype_01 },
    { _trap,    0x4e40, 0xfff0, optype_24 },
    { _move,    0x4e60, 0xfff8, optype_28 },    // 20
    { _move,    0x4e68, 0xfff8, optype_28 },
    { _tas,     0x4ac0, 0xffc0, optype_05 },
    { _jmp,     0x4ec0, 0xffc0, optype_05 },
    { _jsr,     0x4e80, 0xffc0, optype_05 },
    { _move,    0x40c0, 0xf9c0, optype_26 },
    { _nbcd,    0x4800, 0xffc0, optype_15 },
    { _pea,     0x4840, 0xffc0, optype_05 },
    { _ext,     0x4800, 0xfe30, optype_12 },
    { _movem,   0x4880, 0xfb80, optype_23 },
    { _chk,     0x4180, 0xf1c0, optype_16 },    // 30
    { _lea,     0x41c0, 0xf1c0, optype_02 },
    { _dbcc,    0x54c8, 0xfff8, optype_17 },
    { _dbcs,    0x55c8, 0xfff8, optype_17 },
    { _dbeq,    0x57c8, 0xfff8, optype_17 },
    { _dbf,     0x51c8, 0xfff8, optype_17 },
    { _dbge,    0x5cc8, 0xfff8, optype_17 },
    { _dbgt,    0x5ec8, 0xfff8, optype_17 },
    { _dbhi,    0x52c8, 0xfff8, optype_17 },
    { _dble,    0x5fc8, 0xfff8, optype_17 },
    { _dbls,    0x53c8, 0xfff8, optype_17 },    // 40
    { _dblt,    0x5dc8, 0xfff8, optype_17 },
    { _dbmi,    0x5bc8, 0xfff8, optype_17 },
    { _dbne,    0x56c8, 0xfff8, optype_17 },
    { _dbpl,    0x5ac8, 0xfff8, optype_17 },
    { _dbt,     0x50c8, 0xfff8, optype_17 },
    { _dbvc,    0x58c8, 0xfff8, optype_17 },
    { _dbvs,    0x59c8, 0xfff8, optype_17 },
    { _scc,     0x54c0, 0xffc0, optype_05 },
    { _scs,     0x55c0, 0xffc0, optype_05 },
    { _seq,     0x57c0, 0xffc0, optype_05 },    // 50
    { _sf,      0x51c0, 0xffc0, optype_05 },
    { _sge,     0x5cc0, 0xffc0, optype_05 },
    { _sgt,     0x5ec0, 0xffc0, optype_05 },
    { _shi,     0x52c0, 0xffc0, optype_05 },
    { _sle,     0x5fc0, 0xffc0, optype_05 },
    { _sls,     0x53c0, 0xffc0, optype_05 },
    { _slt,     0x5dc0, 0xffc0, optype_05 },
    { _smi,     0x5bc0, 0xffc0, optype_05 },
    { _sne,     0x56c0, 0xffc0, optype_05 },
    { _spl,     0x5ac0, 0xffc0, optype_05 },    // 60
    { _st,      0x50c0, 0xffc0, optype_05 },
    { _svc,     0x58c0, 0xffc0, optype_05 },
    { _svs,     0x59c0, 0xffc0, optype_05 },
    { _addq,    0x5000, 0xf100, optype_06 },
    { _muls,    0xc1c0, 0xf1c0, optype_16 },
    { _mulu,    0xc0c0, 0xf1c0, optype_16 },
    { _exg,     0xc100, 0xf130, optype_18 },
    { _bcc,     0x6400, 0xff00, optype_08 },
    { _bcs,     0x6500, 0xff00, optype_08 },
    { _beq,     0x6700, 0xff00, optype_08 },    // 70
    { _bge,     0x6c00, 0xff00, optype_08 },
    { _bgt,     0x6e00, 0xff00, optype_08 },
    { _bhi,     0x6200, 0xff00, optype_08 },
    { _ble,     0x6f00, 0xff00, optype_08 },
    { _bls,     0x6300, 0xff00, optype_08 },
    { _blt,     0x6d00, 0xff00, optype_08 },
    { _bmi,     0x6b00, 0xff00, optype_08 },
    { _bne,     0x6600, 0xff00, optype_08 },
    { _bpl,     0x6a00, 0xff00, optype_08 },
    { _bvc,     0x6800, 0xff00, optype_08 },    // 80
    { _bvs,     0x6900, 0xff00, optype_08 },
    { _bra,     0x6000, 0xff00, optype_08 },
    { _bsr,     0x6100, 0xff00, optype_08 },
    { _divs,    0x81c0, 0xf1c0, optype_16 },
    { _divu,    0x80c0, 0xf1c0, optype_16 },
    { _rol,     0xe7c0, 0xffc0, optype_11 },
    { _roxl,    0xe5c0, 0xffc0, optype_11 },
    { _lsl,     0xe3c0, 0xffc0, optype_11 },
    { _asl,     0xe100, 0xf118, optype_11 },
    { _asl,     0xe1c0, 0xf1c0, optype_11 },    // 90
    { _ror,     0xe6c0, 0xffc0, optype_11 },
    { _roxr,    0xe4c0, 0xffc0, optype_11 },
    { _lsr,     0xe2c0, 0xffc0, optype_11 },
    { _asr,     0xe000, 0xf118, optype_11 },
    { _asr,     0xe0c0, 0xf1c0, optype_11 },
    { _lsr,     0xe008, 0xf118, optype_11 },
    { _lsl,     0xe108, 0xf118, optype_11 },
    { _rol,     0xe118, 0xf118, optype_11 },
    { _ror,     0xe018, 0xf118, optype_11 },
    { _roxl,    0xe110, 0xf118, optype_11 },    // 100
    { _roxr,    0xe010, 0xf118, optype_11 },
    { _sbcd,    0x8100, 0xf1f0, optype_14 },
    { _moveq,   0x7000, 0xf100, optype_22 },
    { _move_b,  0x1000, 0xf000, optype_04 },
    { _move_w,  0x3000, 0xf000, optype_04 },
    { _move_l,  0x2000, 0xf000, optype_04 },
    { _and,     0xc000, 0xf000, optype_03 },
    { _or,      0x8000, 0xf000, optype_03 },
    { _ori,     0x003c, 0xffff, optype_27 },
    { _ori,     0x007c, 0xffff, optype_27 },    // 110
    { _eori,    0x0a3c, 0xffff, optype_27 },
    { _eori,    0x0a7c, 0xffff, optype_27 },
    { _andi,    0x023c, 0xffff, optype_27 },
    { _andi,    0x027c, 0xffff, optype_27 },
    { _movep_w, 0x0108, 0xf1f8, optype_29 },
    { _movep_w, 0x0188, 0xf1f8, optype_29 },
    { _movep_l, 0x0148, 0xf1f8, optype_29 },
    { _movep_l, 0x01c8, 0xf1f8, optype_29 },
    { _btst,    0x0800, 0xffc0, optype_20 },
    { _bset,    0x08c0, 0xffc0, optype_20 },    // 120
    { _bchg,    0x0840, 0xffc0, optype_20 },
    { _bclr,    0x0880, 0xffc0, optype_20 },
    { _tst,     0x4a00, 0xff00, optype_07 },
    { _clr,     0x4200, 0xff00, optype_15 },
    { _neg,     0x4400, 0xff00, optype_15 },
    { _negx,    0x4000, 0xff00, optype_15 },
    { _not,     0x4600, 0xff00, optype_15 },
    { _subi,    0x0400, 0xff00, optype_13 },
    { _eori,    0x0a00, 0xff00, optype_13 },
    { _cmpi,    0x0c00, 0xff00, optype_13 },    // 130
    { _andi,    0x0200, 0xff00, optype_13 },
    { _addi,    0x0600, 0xff00, optype_13 },
    { _ori,     0x0000, 0xff00, optype_13 },
    { _btst,    0x0100, 0xf1c0, optype_21 },
    { _bset,    0x01c0, 0xf1c0, optype_21 },
    { _bchg,    0x0140, 0xf1c0, optype_21 },
    { _bclr,    0x0180, 0xf1c0, optype_21 },
    { _subq,    0x5100, 0xf100, optype_06 },
    { _suba,    0x90c0, 0xf0c0, optype_03 },
    { _subx,    0x9100, 0xf130, optype_14 },    // 140
    { _sub,     0x9000, 0xf000, optype_03 },

    { _ill,     0x0000, 0x0000, optype_unknown }
  };

/*****************************************************************************/
/* opcodes : 68000 opcodes array for initialization                          */
/*****************************************************************************/

OpCode Dasm68000::opcodes[mnemo68000_count] =
  {
    { "???",     Data },                /* _ill                              */
    { "ABCD",    Data },                /* _abcd                             */
    { "ADDA",    Data },                /* _adda                             */
    { "ADDI",    Data },                /* _addi                             */
    { "ADDQ",    Data },                /* _addq                             */
    { "ADDX",    Data },                /* _addx                             */
    { "ADD",     Data },                /* _add                              */
    { "AND",     Data },                /* _and                              */
    { "ANDI",    Data },                /* _andi                             */
    { "ASL",     Data },                /* _asl                              */
    { "ASR",     Data },                /* _asr                              */
    { "BCC",     Code },                /* _bcc                              */
    { "BCHG",    Data },                /* _bchg                             */
    { "BCLR",    Data },                /* _bclr                             */
    { "BCS",     Code },                /* _bcs                              */
    { "BEQ",     Code },                /* _beq                              */
    { "BGE",     Code },                /* _bge                              */
    { "BGT",     Code },                /* _bgt                              */
    { "BHI",     Code },                /* _bhi                              */
    { "BLE",     Code },                /* _ble                              */
    { "BLS",     Code },                /* _bls                              */
    { "BLT",     Code },                /* _blt                              */
    { "BMI",     Code },                /* _bmi                              */
    { "BNE",     Code },                /* _bne                              */
    { "BPL",     Code },                /* _bpl                              */
    { "BRA",     Code },                /* _bra                              */
    { "BSET",    Data },                /* _bset                             */
    { "BSR",     Code },                /* _bsr                              */
    { "BTST",    Data },                /* _btst                             */
    { "BVC",     Code },                /* _bvc                              */
    { "BVS",     Code },                /* _bvs                              */
    { "CHK",     Data },                /* _chk                              */
    { "CLR",     Data },                /* _clr                              */
    { "CMP",     Data },                /* _cmp                              */
    { "CMPA",    Data },                /* _cmpa                             */
    { "CMPI",    Data },                /* _cmpi                             */
    { "CMPM",    Data },                /* _cmpm                             */
    { "DBCC",    Code },                /* _dbcc                             */
    { "DBCS",    Code },                /* _dbcs                             */
    { "DBEQ",    Code },                /* _dbeq                             */
    { "DBF",     Code },                /* _dbf                              */
    { "DBGE",    Code },                /* _dbge                             */
    { "DBGT",    Code },                /* _dbgt                             */
    { "DBHI",    Code },                /* _dbhi                             */
    { "DBLE",    Code },                /* _dble                             */
    { "DBLS",    Code },                /* _dbls                             */
    { "DBLT",    Code },                /* _dblt                             */
    { "DBMI",    Code },                /* _dbmi                             */
    { "DBNE",    Code },                /* _dbne                             */
    { "DBPL",    Code },                /* _dbpl                             */
    { "DBT",     Code },                /* _dbt                              */
    { "DBVC",    Code },                /* _dbvc                             */
    { "DBVS",    Code },                /* _dbvs                             */
    { "DIVS",    Data },                /* _divs                             */
    { "DIVU",    Data },                /* _divu                             */
    { "EOR",     Data },                /* _eor                              */
    { "EORI",    Data },                /* _eori                             */
    { "EXG",     Data },                /* _exg                              */
    { "EXT",     Data },                /* _ext                              */
    { "JMP",     Code },                /* _jmp                              */
    { "JSR",     Code },                /* _jsr                              */
    { "LEA",     Data },                /* _lea                              */
    { "LINK",    Data },                /* _link                             */
    { "LSL",     Data },                /* _lsl                              */
    { "LSR",     Data },                /* _lsr                              */
    { "MOVE",    Data },                /* _move                             */
    { "MOVE.B",  Data },                /* _move_b                           */
    { "MOVE.W",  Data },                /* _move_w                           */
    { "MOVE.L",  Data },                /* _move_l                           */
    { "MOVEM",   Data },                /* _movem                            */
    { "MOVEP.W", Data },                /* _movep_w                          */
    { "MOVEP.L", Data },                /* _movep_l                          */
    { "MOVEQ",   Data },                /* _moveq                            */
    { "MULS",    Data },                /* _muls                             */
    { "MULU",    Data },                /* _mulu                             */
    { "NBCD",    Data },                /* _nbcd                             */
    { "NEG",     Data },                /* _neg                              */
    { "NEGX",    Data },                /* _negx                             */
    { "NOP",     Data },                /* _nop                              */
    { "NOT",     Data },                /* _not                              */
    { "OR",      Data },                /* _or                               */
    { "ORI",     Data },                /* _ori                              */
    { "PEA",     Data },                /* _pea                              */
    { "RESET",   Data },                /* _reset                            */
    { "ROL",     Data },                /* _rol                              */
    { "ROR",     Data },                /* _ror                              */
    { "ROXL",    Data },                /* _roxl                             */
    { "ROXR",    Data },                /* _roxr                             */
    { "RTE",     Data },                /* _rte                              */
    { "RTR",     Data },                /* _rtr                              */
    { "RTS",     Data },                /* _rts                              */
    { "SBCD",    Data },                /* _sbcd                             */
    { "SCC",     Data },                /* _scc                              */
    { "SCS",     Data },                /* _scs                              */
    { "SEQ",     Data },                /* _seq                              */
    { "SF",      Data },                /* _sf                               */
    { "SGE",     Data },                /* _sge                              */
    { "SGT",     Data },                /* _sgt                              */
    { "SHI",     Data },                /* _shi                              */
    { "SLE",     Data },                /* _sle                              */
    { "SLS",     Data },                /* _sls                              */
    { "SLT",     Data },                /* _slt                              */
    { "SMI",     Data },                /* _smi                              */
    { "SNE",     Data },                /* _sne                              */
    { "SPL",     Data },                /* _spl                              */
    { "ST",      Data },                /* _st                               */
    { "STOP",    Data },                /* _stop                             */
    { "SUB",     Data },                /* _sub                              */
    { "SUBA",    Data },                /* _suba                             */
    { "SUBI",    Data },                /* _subi                             */
    { "SUBQ",    Data },                /* _subq                             */
    { "SUBX",    Data },                /* _subx                             */
    { "SVC",     Data },                /* _svc                              */
    { "SVS",     Data },                /* _svs                              */
    { "SWAP",    Data },                /* _swap                             */
    { "TAS",     Data },                /* _tas                              */
    { "TRAP",    Data },                /* _trap                             */
    { "TRAPV",   Data },                /* _trapv                            */
    { "TST",     Data },                /* _tst                              */
    { "UNLK",    Data },                /* _unlk                             */
                                        /* Pseudo-Ops:                       */
    { "DC",      Data },                /* _dc                               */
    { "RMB",     Data },                /* _rmb                              */
  };

/*****************************************************************************/
/* Dasm68000 : constructor                                                   */
/*****************************************************************************/

Dasm68000::Dasm68000(Application *pApp)
  : Disassembler(pApp)
{
#if 1
asmtype = "gen";
#else
gas = false;
#endif
useFCC = true;
closeCC = true;
// TEST TETS TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
showInstTypes = false;
commentStart = ";";

sExtByte  = ".B";
sExtShort = ".S";
sExtWord  = ".W";
sExtLong  = ".L";
sExtQuad  = ".Q";

hdrHex = "$";
hdrDec = "";
hdrOct = "@";
hdrBin = "%";

int i, j;
mnemo.resize(mnemo68000_count);         /* set up mnemonics table            */
for (i = 0; i < mnemo68000_count; i++)
  mnemo[i] = opcodes[i];
                                        /* set up instruction table          */
otIndex = new uint8_t[65536];
for (i = optblSize - 1; i > 0; i--)
  if (OpTable[i - 1].ot != optype_unknown)
    break;
memset(otIndex, i, 65536);
for (--i; i >= 0; i--)
  {
  int nStart = OpTable[i].op_code;
  int nEnd = (nStart | ~OpTable[i].mask) & 0xffff;
  for (j = nStart; j <= nEnd; j++)
    {
    if ((j & OpTable[i].mask) == OpTable[i].op_code)
      otIndex[j] = (uint8_t)i;
    }
  }

AddOption("asm", "{gas|bird|gen[eric]}\tCreate output for a specific assembler",
          static_cast<PSetter>(&Dasm68000::Set68000Option),
          static_cast<PGetter>(&Dasm68000::Get68000Option));
AddOption("closecc", "{off|on}\tadd closing delimiter to char constants",
          static_cast<PSetter>(&Dasm68000::Set68000Option),
          static_cast<PGetter>(&Dasm68000::Get68000Option));
AddOption("fcc", "{off|on}\tuse FCC to define data",
          static_cast<PSetter>(&Dasm68000::Set68000Option),
          static_cast<PGetter>(&Dasm68000::Get68000Option));
#ifdef _DEBUG
// this is only for debugging the disassembler!
AddOption("shit", "{off|on}\tshow instruction types",
          static_cast<PSetter>(&Dasm68000::Set68000Option),
          static_cast<PGetter>(&Dasm68000::Get68000Option));
#endif
}

/*****************************************************************************/
/* ~Dasm68000 : destructor                                                   */
/*****************************************************************************/

Dasm68000::~Dasm68000(void)
{
if (otIndex)
  delete[] otIndex;
}

/*****************************************************************************/
/* Set68000Option : sets a disassembler option                               */
/*****************************************************************************/

int Dasm68000::Set68000Option(string lname, string value)
{
bool bnValue = true;                    /* default to "on"                   */
bool bIsBool = ParseBool(value, bnValue);

if (lname == "asm")
  {
  string lvalue(lowercase(value));
  if (lvalue == "generic")
    lvalue = "gen";
  SetAsmType(lvalue);
  return 1;
  }
else if (lname == "closecc")
  {
  closeCC = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (lname == "fcc")
  {
  useFCC = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (lname == "shit")
  {
  showInstTypes = bnValue;
  return bIsBool ? 1 : 0;
  }

// should never happen ... but better safe than sorry
return 1;                               /* name and value consumed           */
}

/*****************************************************************************/
/* Get68000Option : retrieves a disassembler option                          */
/*****************************************************************************/

string Dasm68000::Get68000Option(string lname)
{
if (lname == "asm")
  return asmtype;
else if (lname == "closecc")
  return closeCC ? "on" : "off";
else if (lname == "fcc")
  return useFCC ? "on" : "off";
else if (lname == "shit")
  return showInstTypes ? "on" : "off";

return "";
}

/*****************************************************************************/
/* SetAsmType : set up to produce output for a specific assembler            */
/*****************************************************************************/

void Dasm68000::SetAsmType(string &newType)
{
if (newType == "gas" || newType == "bird" || newType == "gen")
  {
  asmtype = newType;
  if (asmtype == "bird")
    {
    hdrHex = "0x";
    hdrOct = "0";
    hdrBin = "0b";
    hdrDec = "0d";
    }
  else
    {
    hdrHex = "$";
    hdrOct = "@";
    hdrBin = "%";
    hdrDec = "";
    }
  }

#if 0
// let's see whether we need that ...
int i;
static struct
  {
  int mnemo;                            /* mnemonic                          */
  const char *asmtype;                  /* assembler output                  */
  const char *mne;                      /* mnemonic                          */
  } mnemomod[] =
  {
    { _adda, "gen",  "ADDA" },
    { _adda, "bird", "ADD"  },
    { _adda, "gen",  "ADDA" },
    { _addi, "gen",  "ADDI" },
    { _addi, "bird", "ADD"  },
    { _addi, "gen",  "ADDI" },
    { _addq, "gen",  "ADDQ" },
    { _addq, "bird", "ADD"  },
    { _addq, "gen",  "ADDQ" },
  };
for (i = 0; i < _countof(mnemomod); i++)
  if (asmtype == mnemomod[i].asmtype)
    opcodes[mnemomod[i].mnemo].mne = mnemomod[i].mne;
#endif

if (asmtype == "gas")
  {
  sExtShort = ".S";
  }
else if (asmtype == "bird")
  {
  sExtShort = ".W";  // can't do that
  }
else // "gen" or anything else
  {
  sExtShort = ".S";
  }
}

/*****************************************************************************/
/* String2Number : convert a string to a number in all known formats         */
/*****************************************************************************/

bool Dasm68000::String2Number(string s, adr_t &value)
{
/* Standard formats for known 680xx assemblers :
   - a character has a leading '
     and may be followed by a (n unchecked) closing '
   - a binary has a leading %
   - an octal constant has a leading @
   - a hex constant has a leading $
*/
s = ltrim(s);
if (s[0] == '$')
  return (sscanf(s.substr(1).c_str(), "%x", &value) == 1);
else if (s[0] == '@')
  return (sscanf(s.substr(1).c_str(), "%o", &value) == 1);
else if (s[0] == '\'' && s.size() > 1)
  {
  value = s[1];
  return true;
  }
else if (s[0] == '%')
  {
  for (string::size_type i = 1; i < s.size(); i++)
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

string Dasm68000::Number2String
    (
    adr_t value,
    int nDigits,
    adr_t addr,
    int bus
    )
{
string s;

/* Standard formats for known 680xx assemblers :
   - a character has a leading '
     and may be followed by a (n unchecked) closing '
   - a binary has a leading %
   - an octal constant has a leading @
   - a hex constant has a leading $
*/

MemAttribute::Type cellType = GetCellType(addr, bus);
MemAttribute::Display disp;
bool bSigned = false;
if (cellType == MemAttribute::CellUntyped)
  disp = MemAttribute::DefaultDisplay;
else
  {
  disp = GetDisplay(addr, bus);
  bSigned = IsSigned(addr, bus);
  }

if (disp == MemAttribute::DefaultDisplay)
  disp = defaultDisplay;

if (disp == MemAttribute::Char &&       /* if character output requested     */
    (nDigits == 2 || isprint(value)))   /* and printable or 2-digit          */
  {
  value &= 0xff;
  if (isprint(value))
    s = sformat("'%c%s", value, closeCC ? "'" : "");
  else
    s = sformat("%s%02x", hdrHex.c_str(), value);
  }
else if (disp == MemAttribute::Binary)  /* if a binary                       */
  {
  int nBit;

  nDigits *= 4;                         /* convert from digits to bits       */
  s = hdrBin;                           /* prepare a binary value            */
                                        /* now do for all bits               */
  for (nBit = nDigits - 1; nBit >= 0; nBit--) 
    s.push_back('0' + (!!(value & (1 << nBit))));
  }
else if (disp == MemAttribute::Hex)     /* if hex                            */
  {
  adr_t mask = 0;
  for (int i = 0; i < nDigits; i++)
    mask |= (0x0f << (i * 4));
  value &= mask;
#if 0
  s = sformat("$%0*X", nDigits, value); /* prepare a hex value               */
#else
  // prepare a hex value with as many places as necessary
  s = sformat("%0*X", nDigits, value);
  while (s.size() > 2 && s.substr(0, 2) == "00")
    s = s.substr(2);
  s = hdrHex + s;
#endif
  }
else if (disp == MemAttribute::Octal)   /* if octal display                  */
  s = sformat("%s%0*o", hdrOct.c_str(), (nDigits * 4) + 2 / 3, value);
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
    {
    adr_t mask = 0;
    for (int i = 0; i < nDigits; i++)
      mask |= (0x0f << (i * 4));
    value &= mask;
    s = sformat("%u", value);           /* prepare unsigned decimal value    */
    }
  }
return s;                               /* pass back generated string        */
}

/*****************************************************************************/
/* InitParse : initialize parsing                                            */
/*****************************************************************************/

bool Dasm68000::InitParse(int bus)
{
// TODO: initialize system vector names etc.

return Disassembler::InitParse(bus);
}

/*****************************************************************************/
/* ParseData : parse data at given memory address for labels                 */
/*****************************************************************************/

adr_t Dasm68000::ParseData
    (
    adr_t addr,
    int bus
    )
{
SetLabelUsed(addr, Const, bus);         /* mark DefLabels as used            */

// TODO: complete this!
int csz = GetCellSize(addr, bus);
if (!IsConst(addr, bus) && !IsBss(addr))
  {
  if (csz == 2)                         /* if WORD data                      */
    SetLabelUsed(GetUWord(addr), Code, bus, addr);
  else if (csz == 4)                    /* if DWORD data                     */
    SetLabelUsed(GetUDWord(addr), Code, bus, addr);
  }
return csz;
}

/*****************************************************************************/
/* Helper functionality, copied from dis.cpp - adapt ASAP!                   */
/*****************************************************************************/

#define BYTE_SIZE	0x0100
#define WORD_SIZE	0x0101
#define LONG_SIZE	0x0102

static int xlate_size(int &size, int type)
{
if (type == 0)	/*	main type	*/
  {
  switch (size)
    {
    case 0:
      return BYTE_SIZE;
    case 1:
      return WORD_SIZE;
    case 2:
      return LONG_SIZE;
    }
  }
else if (type == 1)	/*	for move op	*/
  {
  switch (size)
    {
    case 1:
      size = 0;
      return BYTE_SIZE;
    case 2:
      size = 2;
      return LONG_SIZE;
    case 3:
      size = 1;
      return WORD_SIZE;
    }
  }
return 0;
}

/*****************************************************************************/
/* ParseCode : parse instruction at given memory address for labels          */
/*****************************************************************************/

adr_t Dasm68000::ParseCode
    (
    adr_t addr,
    int bus
    )
{
uint16_t code = GetUWord(addr, bus);
int i = otIndex[code];
adr_t len = 2;                         /* default to 2 bytes length         */
switch (OpTable[i].ot)                  /* parse according to op type        */
  {
  case optype_01 :
    len = ParseOptype01(addr, addr + 2, code, i) - addr;
    break;
  case optype_02 :
    len = ParseOptype02(addr, addr + 2, code, i) - addr;
    break;
  case optype_03 :
    len = ParseOptype03(addr, addr + 2, code, i) - addr;
    break;
  case optype_04 :
    len = ParseOptype04(addr, addr + 2, code, i) - addr;
    break;
  case optype_05 :
    len = ParseOptype05(addr, addr + 2, code, i) - addr;
    break;
  case optype_06 :
    len = ParseOptype06(addr, addr + 2, code, i) - addr;
    break;
  case optype_07 :
    len = ParseOptype07(addr, addr + 2, code, i) - addr;
    break;
  case optype_08 :
    len = ParseOptype08(addr, addr + 2, code, i) - addr;
    break;
  case optype_09 :
    len = ParseOptype09(addr, addr + 2, code, i) - addr;
    break;
  case optype_10 :
    len = ParseOptype10(addr, addr + 2, code, i) - addr;
    break;
  case optype_11 :
    len = ParseOptype11(addr, addr + 2, code, i) - addr;
    break;
  case optype_12 :
    len = ParseOptype12(addr, addr + 2, code, i) - addr;
    break;
  case optype_13 :
    len = ParseOptype13(addr, addr + 2, code, i) - addr;
    break;
  case optype_14 :
    len = ParseOptype14(addr, addr + 2, code, i) - addr;
    break;
  case optype_15 :
    len = ParseOptype15(addr, addr + 2, code, i) - addr;
    break;
  case optype_16 :
    len = ParseOptype16(addr, addr + 2, code, i) - addr;
    break;
  case optype_17 :
    len = ParseOptype17(addr, addr + 2, code, i) - addr;
    break;
  case optype_18 :
    len = ParseOptype18(addr, addr + 2, code, i) - addr;
    break;
  case optype_19 :
    len = ParseOptype19(addr, addr + 2, code, i) - addr;
    break;
  case optype_20 :
    len = ParseOptype20(addr, addr + 2, code, i) - addr;
    break;
  case optype_21 :
    len = ParseOptype21(addr, addr + 2, code, i) - addr;
    break;
  case optype_22 :
    len = ParseOptype22(addr, addr + 2, code, i) - addr;
    break;
  case optype_23 :
    len = ParseOptype23(addr, addr + 2, code, i) - addr;
    break;
  case optype_24 :
    len = ParseOptype24(addr, addr + 2, code, i) - addr;
    break;
  case optype_25 :
    len = ParseOptype25(addr, addr + 2, code, i) - addr;
    break;
  case optype_26 :
    len = ParseOptype26(addr, addr + 2, code, i) - addr;
    break;
  case optype_27 :
    len = ParseOptype27(addr, addr + 2, code, i) - addr;
    break;
  case optype_28 :
    len = ParseOptype28(addr, addr + 2, code, i) - addr;
    break;
  case optype_29 :
    len = ParseOptype29(addr, addr + 2, code, i) - addr;
    break;
  default :                             /* not a valid instruction ?         */
    SetInvalidInstruction(addr, 2, bus);  /* mark it as such                 */
    break;
  }

return len;
}

/*****************************************************************************/
/* ParseEffectiveAddress : parse the EA part of an instruction               */
/*****************************************************************************/

adr_t Dasm68000::ParseEffectiveAddress
    (
    adr_t instaddr,                    /* address of instruction start      */
    adr_t addr,                        /* current address                   */
    uint16_t ea,
    int16_t index,                      /* index into opcode table           */
    int op_mode
    )
{
int32_t displacement;
int16_t short_adr;
int index_reg;
int index_reg_ind;
int index_size;
int32_t a1;
MemoryType mt = mnemo[OpTable[index].mnemo].memType;

int mode = (ea & 0x38) >> 3;
int reg = ea & 0x7;
switch(mode)
  {
  case 0:                               /* one of the data registers         */
  case 1:                               /* one of the address registers      */
  case 2:                               /* indirect address                  */
    // sprintf(s,"(%s)", GetAddrReg(reg));
  case 3:                               /* indirect address with increment   */
    // sprintf(s,"(%s)+", GetAddrReg(reg));
  case 4:                               /* indirect address with decrement   */
    // sprintf(s,"-(%s)", GetAddrReg(reg));
    break;
  case 5:                               /* address with displacement         */
    displacement = GetSWord(addr);
    addr += 2;
    // sprintf(s,"$%x(%s)", displacement, GetAddrReg(reg));
    break;
  case 6:                               /* indexed address with displacement */
    displacement = GetUWord(addr);
    addr += 2;
    index_reg = (displacement & 0x7000) >> 12;
    index_reg_ind = (displacement & 0x8000)>> 15;
    index_size = (displacement & 0x8000) >> 11;
    displacement = (signed char)(displacement & 0xff);
#if 0
    if (index_size)
      {
      s1 = sExtLong;
      }
    else
      {
      s1 = sExtWord;
      }
    if(index_reg_ind)
      {
      /*	address reg is index reg	*/
      sprintf(s,"$%x(%s,%s%s)",displacement, GetAddrReg(reg), GetAddrReg(index_reg]),s1);
      }
    else
      {
      /*	data reg is index reg	*/
      sprintf(s,"$%x(%s,%s%s)",displacement, GetAddrReg(reg), GetDataReg(index_reg),s1);
      }
#endif
    break;
  case 7:
    switch (reg)
      {
      case 0:		/*	absolute short	*/
        short_adr = GetSWord(addr);
        SetCellSize(addr, 2);
        // sprintf(s,"$%lx%s",(long)short_adr, sExtShort.c_str());
        if (!IsConst(addr))
          {
          SetLabelUsed((uint16_t)short_adr, mt, BusCode, instaddr);
          a1 = PhaseInner((uint16_t)short_adr, instaddr);
          AddRelativeLabel(a1, addr, mt, true, BusCode, instaddr);
          }
        addr += 2;
        break;
      case 1:		/*	absolute long	*/
        a1 = GetSDWord(addr);
        SetCellSize(addr, 4);
        if (!IsConst(addr))
          SetLabelUsed(a1, mt, BusCode, instaddr);
        a1 = PhaseInner(a1, instaddr);
        if ((adr_t)a1 <= GetHighestCodeAddr())
          AddRelativeLabel(a1, addr, mt, true, BusCode, instaddr);
        addr += 4;
        break;
      case 2:	/*	program counter with displacement	*/
        displacement = GetSWord(addr);
        SetCellSize(addr, 2);
        a1 = (int32_t)addr + displacement;
        // sprintf(s,"$%lx(PC)",a1);
        if (!IsConst(addr))
          SetLabelUsed(a1, mt, BusCode, instaddr);
        a1 = PhaseInner(a1, instaddr);
        if ((adr_t)a1 <= GetHighestCodeAddr())
          AddRelativeLabel(a1, addr, mt, true, BusCode, instaddr);
        addr += 2;
        break;
      case 3:
        displacement = GetUWord(addr);
        a1 = (int32_t)addr + (char)(displacement & 0xff);
        addr += 2;
        index_reg = (displacement >> 12) & 0x07;
#if 0
        if (displacement & 0x8000)
          /*	address reg is index reg	*/
          sprintf(s,"$%lx(PC,%s%s)",a1,GetAddrReg(index_reg), sExtLong);
        else
          /*	data reg is index reg	*/
          sprintf(s,"$%lx(PC,%s%s)",a1,GetDataReg(index_reg), sExtWord);
#endif
        break;
      case 4:  // Immediate Data
        // This one is tough
        if (op_mode == LONG_SIZE)
          {
          a1 = GetSDWord(addr);
          if (!IsConst(addr))
            SetLabelUsed(a1, mt, BusCode, instaddr);
          addr += 4;
#if 0
          // this MIGHT be a label, but more likely it's constant data,
          // so don't automatically assign a label
          a1 = PhaseInner(a1, addr - 6);
          AddLabel(a1, mt, "", true);
#endif
          }
        else if (op_mode == WORD_SIZE || op_mode == BYTE_SIZE)
          {
          displacement = GetSWord(addr);
          addr += 2;
#if 0
          if (op_mode == WORD_SIZE)
            sprintf(s,"#$%x%s",displacement, sExtWord.c_str());
          else if (op_mode == BYTE_SIZE)
            {
            displacement = (char)(displacement & 0xff);
            sprintf(s,"#$%x%s",displacement, sExtByte.c_str());
            }
#endif
          }
        break;
      default :                         /* anything else?                    */
        SetInvalidInstruction(instaddr, 2);
        break;
      }	/*	end of switch reg	*/
    break;
  default :                             /* anything else ?                   */
    SetInvalidInstruction(instaddr, 2);
    break;
  }	/*	end of switch mode	*/

(void)index_reg;  // these are unused ATM
(void)index_reg_ind;
(void)index_size;

return addr;
}

/*****************************************************************************/
/* ParseOptypeXX : parsers for the various 68000 op types                    */
/*****************************************************************************/

adr_t Dasm68000::ParseOptype01(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr;
(void)code;
(void)optable_index;

int16_t displacement = GetSWord(addr);  /* get displacement                 */
(void)displacement;  // unused ATM
addr += 2;
return addr;
}

adr_t Dasm68000::ParseOptype02(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
uint16_t dest_reg = (code >> 9) & 0x07; /* get destination reg               */
(void)dest_reg;  // unused ATM
uint16_t source = code & 0x3f;          /* this is an effective address      */
                                        /* calculate effective address       */
addr = ParseEffectiveAddress(instaddr, addr, source, optable_index, 0);
return addr;
}

adr_t Dasm68000::ParseOptype03(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
uint16_t dest_reg = (code >> 9) & 0x07;
uint16_t source = code & 0x3f;          /* this is an effective address      */
int16_t op_mode = (code >> 6) & 0x07;   /* get op mode                       */
int16_t dir;
int size;
if (op_mode == 3 || op_mode == 7)       /* adda type instructions            */
  {
  dir = 0;
  if (op_mode == 3)
    size = 1;
  else
    size = 2;
  }
else
  {
  size = op_mode & 0x03;
  dir = (op_mode >> 2) & 0x01;
  }
op_mode = xlate_size(size, 0);
addr = ParseEffectiveAddress(instaddr, addr, source, optable_index, op_mode);

(void)dir;  // unused ATM
(void)dest_reg;
return addr;
}

adr_t Dasm68000::ParseOptype04(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
int size = (code >> 12) & 0x03;
int16_t op_mode = xlate_size(size, 1);
int16_t source = code & 0x3f;
int16_t dest = (code >> 6) & 0x3f;
/*	on dest, mode and reg are in different order, so swap them	*/
dest = ((dest & 0x07) << 3) | (dest >> 3);
addr = ParseEffectiveAddress(instaddr, addr, source, optable_index, op_mode);
addr = ParseEffectiveAddress(instaddr, addr, dest, optable_index, 0);
return addr;
}

adr_t Dasm68000::ParseOptype05(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
// PEA type instructions
int16_t source = code & 0x3f;
addr = ParseEffectiveAddress(instaddr, addr, source, optable_index, 0);
return addr;
}

adr_t Dasm68000::ParseOptype06(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
// ADDQ type instructions

int16_t dest = code & 0x3f;
int size = (code >> 6) & 0x03;  // 0x03? sure? not 0x07? what's at bit 8? TODO: check
int16_t op_mode = xlate_size(size, 0);
int16_t data = (code >> 9) & 0x07;
if (data == 0)
  data = 8;
addr = ParseEffectiveAddress(instaddr, addr, dest, optable_index, 0);
(void)op_mode; // unused ATM
return addr;
}

adr_t Dasm68000::ParseOptype07(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
// TST type instructions

int16_t dest = code & 0x3f;
int size = (code >> 6) & 0x03;
int16_t op_mode = xlate_size(size, 0);
addr = ParseEffectiveAddress(instaddr, addr, dest, optable_index, 0);

(void)op_mode;  // unused ATM
return addr;
}

adr_t Dasm68000::ParseOptype08(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
adr_t startaddr = addr - 2;
// Branches
uint16_t displacement = code & 0xff;
adr_t dest;
bool bSetLabelUsed = !IsConst(startaddr);
if (displacement == 0)
  {
  SetCellSize(addr, 2);
  displacement = GetUWord(addr);
  dest = addr + (int16_t)displacement;
  bSetLabelUsed = !IsConst(addr);
  addr += 2;
  }
else
  {
  displacement = (uint16_t)( (int)( (char)displacement) );
  dest = addr + (int16_t)displacement;
  }

if (bSetLabelUsed)
  SetLabelUsed(dest, Code, BusCode, instaddr);

dest = PhaseInner(dest, startaddr);
AddLabel(dest, mnemo[OpTable[optable_index].mnemo].memType, "", true);
return addr;
}

adr_t Dasm68000::ParseOptype09(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr; (void)code; (void)optable_index;
// UNLINK type instructions
int16_t reg = code & 0x07;
(void)reg;  // unused ATM
return addr;
}

adr_t Dasm68000::ParseOptype10(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr; (void)code; (void)optable_index;
// RTS type instructions - no arguments
return addr;
}

adr_t Dasm68000::ParseOptype11(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
// LSR type instructions

int size = (code >> 6) & 0x03;
if (size < 3)  /* register shifts */
  {
  int16_t type = (code >> 5) & 0x01;
  int16_t dest = (code & 0x07);
  int16_t count = (code >> 9) & 0x07;
#if 1
  (void)type; (void)dest; (void)count;
#else
  smnemo += GetSizeSuffix(size);
  if (type)
    sparm = sformat("%s,%s", GetDataReg(count), GetDataReg(dest));
  else
    {
    if (count == 0)
      count = 8;
    sparm = sformat("#$%x,%s", count, GetDataReg(dest));
    }
#endif
  }
else  /* memory shifts */
  {
  int16_t dest = (code & 0x3f);
  addr = ParseEffectiveAddress(instaddr, addr, dest, optable_index, 0);
  // sparm = e_a;
  }

return addr;
}

adr_t Dasm68000::ParseOptype12(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr; (void)optable_index;
// EXT type instruction

int16_t op_mode = ((code >> 6) & 0x07) - 1;
int16_t reg = code & 0x07;
// smnemo += GetSizeSuffix(op_mode);
// sparm = GetDataReg(reg);
(void)op_mode; (void)reg;  // unused ATM
return addr;
}

adr_t Dasm68000::ParseOptype13(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
// ADDI type instruction

int32_t il_data;  /* imediate data */
int16_t dest = code & 0x3f;
int16_t size = (code >> 6) & 0x03;
if (size == 2)  /* long operation */
  {
  il_data = GetSDWord(addr);
  addr += 4;
  }
else
  {
  il_data = GetSWord(addr);
  addr += 2;
  }
addr = ParseEffectiveAddress(instaddr, addr, dest, optable_index, 0);

// smnemo += GetSizeSuffix(size);
// sparm = sformat("#$%x,%s", il_data, e_a);
(void)il_data;  // unused ATM
return addr;
}

adr_t Dasm68000::ParseOptype14(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr; (void)optable_index;
// ABCD type instruction

int16_t dest = (code >> 9) & 0x07;
int16_t source = code & 0x07;
int16_t dest_type = (code >> 3) & 0x01;
int size = (code >> 6) & 0x03;
/*
smnemo += GetSizeSuffix(size);
sparm = (dest_type) ?
    sformat("-(%s),-(%s)", GetAddrReg(source), GetAddrReg(dest)) :
    sformat("%s,%s", GetDataReg(source), GetDataReg(dest));
*/
(void)dest; (void)source; (void)dest_type; (void)size;

return addr;
}

adr_t Dasm68000::ParseOptype15(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
// CLR type instruction

int16_t dest = code & 0x3f;
int size = (code >> 6) & 0x03;
addr = ParseEffectiveAddress(instaddr, addr, dest, optable_index, 0);
// smnemo += GetSizeSuffix(size);
// sparm = e_a;
(void)size;
return addr;
}

adr_t Dasm68000::ParseOptype16(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
// CHK type instruction

int16_t dest = code & 0x3f;
int16_t reg = (code >> 9) & 0x07;
addr = ParseEffectiveAddress(instaddr, addr, dest, optable_index, WORD_SIZE);
// sparm = sformat("%s,%s", e_a, GetDataReg(reg));
(void)reg;
return addr;
}

adr_t Dasm68000::ParseOptype17(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
// DBcc type instructions

int16_t reg = code & 0x07;
int16_t displacement = GetSWord(addr);
int32_t dest = (int32_t)addr + displacement;
if (!IsConst(addr))
  SetLabelUsed(dest, Code, BusCode, instaddr);
dest = PhaseInner(dest, addr - 2);
AddRelativeLabel(dest, addr, mnemo[OpTable[optable_index].mnemo].memType, true, BusCode, instaddr);
addr += 2;

// sparm = sformat("%s,$%lx", GetDataReg(reg), dest);
(void)reg;
return addr;
}

adr_t Dasm68000::ParseOptype18(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr; (void)optable_index;

// EXG type instructions

int16_t op_mode = (code >> 3) & 0x1f;
if (op_mode != 0x08 &&
    op_mode != 0x09 &&
    op_mode != 0x11)
  SetInvalidInstruction(addr - 2, 2);
return addr;
}

adr_t Dasm68000::ParseOptype19(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr; (void)optable_index;

// CMPM type instructions

int16_t source = code & 0x07;
int16_t dest = (code >> 9) & 0x07;
int size = (code >> 6) & 0x03;
// smnemo += GetSizeSuffix(size);
// sparm = sformat("(%s)+,(%s)+", GetAddrReg(source), GetAddrReg(dest));
(void)source; (void)dest; (void)size;
return addr;
}

adr_t Dasm68000::ParseOptype20(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
// BTST # type instruction

int16_t dest = code & 0x3f;
int32_t il_data = GetSWord(addr);
addr += 2;
addr = ParseEffectiveAddress(instaddr, addr, dest, optable_index, 0);
// sparm = sformat("#$%x,%s", il_data, e_a);
(void)il_data;
return addr;
}

adr_t Dasm68000::ParseOptype21(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
int16_t dest_reg = (code >> 9) & 0x07;
int16_t source = code & 0x3f;           /* this is an effective address      */
addr = ParseEffectiveAddress(instaddr, addr, source, optable_index, 0);
// sparm = sformat("%s,%s", GetDataReg(dest_reg), e_a);
(void)dest_reg;
return addr;
}

adr_t Dasm68000::ParseOptype22(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr; (void)optable_index;

// moveq

int16_t data = (int16_t)((char)(code & 0xff));
uint16_t dest_reg = (code >> 9) & 0x07;
// sparm = sformat("#$%x,%s", data, GetDataReg(dest_reg));
(void)data; (void)dest_reg;
return addr;
}

adr_t Dasm68000::ParseOptype23(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
// movem

int16_t source = code & 0x3f;
int size = (code >> 6) & 0x01;
int direction = (code >> 10) & 0x01;
uint16_t reg_mask = GetUWord(addr);
addr += 2; // not sure about that - TODO: check!
addr = ParseEffectiveAddress(instaddr, addr, source, optable_index, 0);
// process register mask
int16_t mode = (source >> 3) & 0x07;
int16_t flag = 0;
uint16_t mask;
bool first = true;
if (mode == 4)  /* predecrement */
  mask = 0x080;
else
  mask = 0x0100;
int i;
for (i = 0; i < 8; i++)
  {
  if (mask & reg_mask)  /* is this bit set? */
    {
    if (first)
      {
      //strcpy(regs, GetAddrReg(i));
      first = false;
      flag++;
      }
    else if (flag == 0)
      {
      //sprintf(regs + strlen(regs), "/%s", GetAddrReg(i));
      ++flag;
      }
    else if (flag)
      {
      ++flag;
      }
    }
  else if (flag)
    {
    //if (flag > 2)
    //  sprintf(regs + strlen(regs), "-%s", GetAddrReg(i - 1));
    //else if (flag == 2)
    //  sprintf(regs + strlen(regs), "/%s", GetAddrReg(i - 1));
    flag = 0;
    }
  if (mode == 4)
    mask >>= 1;
  else
    mask <<= 1;
  }

//if (flag > 1)
//  sprintf(regs + strlen(regs), "-%s", GetAddrReg(i - 1));
flag = 0;
if (mode == 4)  /* predecrement */
  mask = 0x8000;
else
  mask = 0x01;
for (i = 0; i < 8; ++i)
  {
  if (mask & reg_mask)	/*	is this bit set?	*/
    {
    if (first)
      {
      // strcpy(regs, GetDataReg(i));
      first = false;
      flag++;
      }
    else if (flag == 0)
      {
      // sprintf(regs + strlen(regs), "/%s", GetDataReg(i));
      ++flag;
      }
    else if (flag)
      {
      ++flag;
      }
    }
  else if (flag)
    {
    //if (flag > 2)
    //  sprintf(regs + strlen(regs), "-%s", GetDataReg(i - 1));
    //else if (flag == 2)
    //  sprintf(regs + strlen(regs), "/%s", GetDataReg(i - 1));
    flag = 0;
    }
  if (mode == 4)
    mask >>= 1;
  else
    mask <<= 1;
  }
#if 0
if (flag > 1)
  sprintf(regs + strlen(regs), "-%s", GetDataReg(i - 1));
const char *f, *d;
if (!direction)
  {
  f = regs;
  d = e_a;
  }
else
  {
  f = e_a;
  d = regs;
  }
smnemo += GetSizeSuffix(size ? 2 : 1);
sparm = sformat("%s,%s", f, d);
#endif
(void)size; (void)direction;

return addr;
}

adr_t Dasm68000::ParseOptype24(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr; (void)optable_index;

// trap

int16_t vector = code & 0x0f;
// sparm = sformat("#$%x", vector);
(void)vector;
return addr;
}

adr_t Dasm68000::ParseOptype25(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr; (void)code; (void)optable_index;

// stop

uint16_t data = GetUWord(addr);
addr += 2;
// sparm = sformat("#$%x", data);
(void)data;
return addr;
}

adr_t Dasm68000::ParseOptype26(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
// move sr

int16_t source = code & 0x3f;
addr = ParseEffectiveAddress(instaddr, addr, source, optable_index, WORD_SIZE);
uint16_t mt = code & 0x0600;
//const char *reg = (mt == 0x0200 || mt == 0x0400) ? "CCR" : "SR";
//sparm = (code & 0x400) ?
//    sformat("%s,%s", e_a, reg) :
//    sformat("%s,%s", reg, e_a);
(void)mt;
return addr;
}

adr_t Dasm68000::ParseOptype27(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr; (void)code; (void)optable_index;

uint16_t data = GetUWord(addr);
addr += 2;
//sparm = sformat("#$%x,%s", data, (code & 0x40) ? "SR" : "CCR");
(void)data;
return addr;
}

adr_t Dasm68000::ParseOptype28(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr; (void)code; (void)optable_index;

// MOVE USP
// sparm = sformat((code & 0x08) ? "USP,%s" : "%s,USP", GetAddrReg(code & 0x7));
return addr;
}

adr_t Dasm68000::ParseOptype29(adr_t instaddr, adr_t addr, uint16_t code, int optable_index)
{
(void)instaddr; (void)optable_index;

// MOVEP - move peripheral data
int data_reg = (code & 0x0e00) >> 9;
int adr_reg =  code & 0x07;
unsigned direction = code & 0x080;
int16_t displacement = GetSWord(addr);
addr += 2;
//if (direction)
//  sparm = sformat("%s,%x(%s)", GetDataReg(data_reg),displacement, GetAddrReg(adr_reg));
//else
//  sparm = sformat("%x(%s),%s", displacement, GetAddrReg(adr_reg), GetDataReg(data_reg));
(void)data_reg; (void)adr_reg; (void)direction; (void)displacement;
return addr;
}

/*****************************************************************************/
/* DisassembleLabel : disassemble used external labels                       */
/*****************************************************************************/

bool Dasm68000::DisassembleLabel
    (
    Label *label,
    string &slabel,
    string &smnemo,
    string &sparm,
    int bus
    )
{
string lbltxt = label->GetText();
if (lbltxt.find_first_of("+-") == string::npos)
  {
  adr_t laddr = label->GetAddress();
  if (lbltxt.size() && !GetRelative(laddr, bus))
    slabel = lbltxt;
  else
    slabel = Label2String(laddr, GetBusWidth() / 4, true, laddr, bus);
  smnemo = "EQU";
  sparm = Address2String(laddr, bus);
  Disassembler::DisassembleLabel(label, slabel, smnemo, sparm, bus);
  return true;
  }
return false;
}

/*****************************************************************************/
/* DisassembleDefLabel : pass back mnemonic and parameters for a DefLabel    */
/*****************************************************************************/

bool Dasm68000::DisassembleDefLabel
    (
    DefLabel *label,
    string &slabel,
    string &smnemo,
    string &sparm,
    int bus
    )
{
(void)bus;
slabel = label->GetText();
smnemo = "EQU";
sparm = label->GetDefinition();
Disassembler::DisassembleDefLabel(label, slabel, smnemo, sparm, bus);
return true;
}

/*****************************************************************************/
/* DisassembleData : disassemble data area at given memory address           */
/*****************************************************************************/

adr_t Dasm68000::DisassembleData
    (
    adr_t addr,
    adr_t end,
    uint32_t flags,
    string &smnemo,
    string &sparm,
    int maxparmlen,
    int bus                             /* ignored for 6800 and derivates    */
    )
{
adr_t done;
if (FindLabel(addr, Const, bus))
  flags &= ~SHMF_TXT;

if (flags & SHMF_RMB)                   /* if reserved memory block          */
  {
  done = end;                           /* remember it's done completely     */
  smnemo = opcodes[_rmb].mne;
  sparm = Number2String(end - addr, 4, addr);
  }
else if (useFCC && (flags & SHMF_TXT))  /* if FCC (text) allowed             */
  {
  smnemo = opcodes[_dc].mne + sExtByte;
  sparm = '"';                          /* start the game                    */
  for (done = addr; done < end; done++) /* assemble as many as possible      */
    {                                   /* if this would become too long     */
    if (sparm.size() + 2 > (string::size_type)maxparmlen ||
        FindLabel(done, Const, bus))    /* or a DefLabel chimes in           */
      break;                            /* terminate the loop                */
    sparm += *getat(done);
    }
  sparm += '"';                         /* append delimiter                  */
  }
else if (flags & 0xff)                  /* if not byte-sized                 */
  {
  int dsz = (int)(flags & 0xff) + 1;

  // 68000 can do byte, word, long, and quadword
  smnemo = opcodes[_dc].mne +
                  ((dsz == 2) ? sExtWord :
                   (dsz == 4) ? sExtLong :
                   (dsz == 8) ? sExtQuad :
                   sformat(".?%d?", dsz));
                                        /* assemble as many as possible      */
  for (done = addr; done < end; done += dsz)
    {
    string s;
    switch (dsz)
      {
      case 2 :
        s = Label2String(GetUWord(done), 4, !IsConst(done), done);
        break;
      case 4 :
        s = Label2String(GetUDWord(done), 8, !IsConst(done), done);
        break;
      case 8 :
        {
        uint64_t qw = GetUQWord(done);
        // this ain't no label. Never!
        // s = Label2String(qw, 16, !IsConst(done), done);

        // display as hex value in a format that any 32/64bit compiler can do
        s = sformat("%s%x%08x", hdrHex.c_str(), (uint32_t)(qw >> 32), (uint32_t)qw);
        }
        break;
      }

    if (sparm.size())                   /* if already something there        */
      {                                 /* if this would become too long     */
      if (sparm.size() + s.size() + 1 > (string::size_type)maxparmlen)
        break;                          /* terminate the loop                */
      sparm += ',';                     /* add separator                     */
      }
    sparm += s;                         /* append the byte's representation  */
    }
  }
else                                    /* if DC (hex or binary)             */
  {
  smnemo = opcodes[_dc].mne + sExtByte;
                                        /* assemble as many as possible      */
  for (done = addr; done < end; done++)
    {
    Label *deflbl = FindLabel(done, Const, bus);
    string s;
    if (deflbl)
      s = deflbl->GetText();
    else
      s = Number2String(*getat(done), 2, done);
    if (sparm.size())                   /* if already something there        */
      {                                 /* if this would become too long     */
      if (sparm.size() + s.size() + 1 > (string::size_type)maxparmlen)
        break;                          /* terminate the loop                */
      sparm += ',';                     /* add separator                     */
      }
    sparm += s;                         /* append the byte's representation  */
    }
  }

return done - addr;
}

/*****************************************************************************/
/* DisassembleCode : disassemble code instruction at given memory address    */
/*****************************************************************************/

adr_t Dasm68000::DisassembleCode
    (
    adr_t addr,
    string &smnemo,
    string &sparm,
    int bus                             /* ignored for 6800 and derivates    */
    )
{
uint16_t code = GetUWord(addr, bus);
int i = otIndex[code];
adr_t len = 2;                          /* default to 2 bytes length         */
smnemo = mnemo[OpTable[i].mnemo].mne;   /* initialize mnemonic               */
switch (OpTable[i].ot)                  /* parse according to op type        */
  {
  case optype_01 :
    len = DisassembleOptype01(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_02 :
    len = DisassembleOptype02(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_03 :
    len = DisassembleOptype03(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_04 :
    len = DisassembleOptype04(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_05 :
    len = DisassembleOptype05(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_06 :
    len = DisassembleOptype06(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_07 :
    len = DisassembleOptype07(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_08 :
    len = DisassembleOptype08(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_09 :
    len = DisassembleOptype09(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_10 :
    len = DisassembleOptype10(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_11 :
    len = DisassembleOptype11(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_12 :
    len = DisassembleOptype12(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_13 :
    len = DisassembleOptype13(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_14 :
    len = DisassembleOptype14(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_15 :
    len = DisassembleOptype15(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_16 :
    len = DisassembleOptype16(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_17 :
    len = DisassembleOptype17(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_18 :
    len = DisassembleOptype18(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_19 :
    len = DisassembleOptype19(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_20 :
    len = DisassembleOptype20(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_21 :
    len = DisassembleOptype21(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_22 :
    len = DisassembleOptype22(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_23 :
    len = DisassembleOptype23(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_24 :
    len = DisassembleOptype24(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_25 :
    len = DisassembleOptype25(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_26 :
    len = DisassembleOptype26(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_27 :
    len = DisassembleOptype27(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_28 :
    len = DisassembleOptype28(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  case optype_29 :
    len = DisassembleOptype29(addr, addr + 2, code, i, smnemo, sparm) - addr;
    break;
  default :                             /* not a valid instruction ?         */
    break;
  }

// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST 
if (showInstTypes)
  {
  int blk = sparm.size();
  if (blk > 20)
    blk = 20;
  blk = 20 - blk + 1;
  sparm += sformat("%*s%sT%02d@%03d", blk, "", commentStart.c_str(), OpTable[i].ot, i);
  }

return len;
}

/*****************************************************************************/
/* GetAddrReg : returns the name of an address register                      */
/*****************************************************************************/

const char *Dasm68000::GetAddrReg(int i)
{
static const char *regs[] = { "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7" };
return regs[i & 7];
}

/*****************************************************************************/
/* GetDataReg : returns the name of a data register                          */
/*****************************************************************************/

const char *Dasm68000::GetDataReg(int i)
{
static const char *regs[] = { "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7" };
return regs[i & 7];
}

/*****************************************************************************/
/* GetSizeSuffix : returns the matching size suffix for a size code          */
/*****************************************************************************/

const char *Dasm68000::GetSizeSuffix(int sizeCode)
{
#pragma message("This needs refinement for the configurable exts!")
static const string sizes[] = { ".B", ".W", ".L",
                               ".?3?", ".?4?", ".?5?", ".?6?", ".?7?" };
return sizes[sizeCode & 7].c_str();
}

/*****************************************************************************/
/* DisassembleEffectiveAddress : disassemble the EA part of an instruction   */
/*****************************************************************************/

adr_t Dasm68000::DisassembleEffectiveAddress
    (
    adr_t instaddr,                     /* address of instruction start      */
    adr_t addr,                         /* current address                   */
    string &s,                          /* destination buffer                */
    uint16_t ea,
    int16_t index,                      /* index into opcode table           */
    int op_mode
    )
{
(void)instaddr;
(void)index;

int mode;
int reg;
int32_t displacement;
int16_t short_adr;
int index_reg;
int index_reg_ind;
int index_size;
const char *s1;
int32_t a1;
bool bGetLabel;
Label *pLbl;

mode = (ea & 0x38) >> 3;
reg = ea & 0x7;
switch(mode)
  {
  case 0:
    s = sformat("%s", GetDataReg(reg));
    break;
  case 1:
    s = sformat("%s", GetAddrReg(reg));
    break;
  case 2:	/*	address indirect	*/
    s = sformat("(%s)", GetAddrReg(reg));
    break;
  case 3:
    s = sformat("(%s)+", GetAddrReg(reg));
    break;
  case 4:
    s = sformat("-(%s)", GetAddrReg(reg));
    break;
  case 5:	/*	address with displacement	*/
    s = sformat("%s(%s)",
                Number2String(GetUWord(addr), 4, addr).c_str(),
                GetAddrReg(reg));
    addr += 2;
    break;
  case 6:	/*	indexed address with displacement	*/
    displacement = GetUWord(addr);
    index_reg = (displacement & 0x7000) >> 12;
    index_reg_ind = (displacement & 0x8000)>> 15;
    index_size = (displacement & 0x8000) >> 11;
    displacement = (signed char)(displacement & 0xff);
    if (index_size)
      {
      s1 = sExtLong.c_str();
      }
    else
      {
      s1 = sExtWord.c_str();
      }
    if (index_reg_ind)
      {
      /*	address reg is index reg	*/
      s = sformat("%s(%s,%s%s)",
                  Number2String(displacement, 2, addr + 1).c_str(),
                  GetAddrReg(reg), GetAddrReg(index_reg), s1);
      }
    else
      {
      /*	data reg is index reg	*/
      s = sformat("%s(%s,%s%s)",
                  Number2String(displacement, 2, addr + 1).c_str(),
                  GetAddrReg(reg), GetDataReg(index_reg), s1);
      }
    addr += 2;
    break;
  case 7:
    switch (reg)	/*	other modes	*/
      {
      case 0:		/*	absolute short	*/
        // TODO: unsure whether this is signed or not. Check! Might save a ton of special code
        short_adr = GetSWord(addr);
        bGetLabel = !IsConst(addr);
        pLbl = FindLabel((uint16_t)short_adr);
        if (pLbl)
          s = Label2String((uint16_t)short_adr, 4, bGetLabel, addr) + sExtShort;
        else
          s = Number2String(short_adr, 4, addr) +  sExtShort;
        addr += 2;
        break;
      case 1:		/*	absolute long	*/
        a1 = GetSDWord(addr);
        bGetLabel = !IsConst(addr);
        // bIsHex = IsHex(addr);
        s = Label2String(a1, GetBusWidth() / 4, bGetLabel, addr) + sExtLong;
        addr += 4;
        break;
      case 2:	/*	program counter with displacement	*/
        displacement = GetSWord(addr);
        bGetLabel = !IsConst(addr);
        a1 = (int32_t)addr + displacement;
        s = sformat("%s(PC)", Label2String(a1, GetBusWidth() / 4, bGetLabel, addr).c_str());
        addr += 2;
        break;
      case 3:
        displacement = GetUWord(addr);
        // bIsHex = IsHex(addr);
        a1 = (int32_t)addr + (int32_t)(char)(displacement & 0xff);
        bGetLabel = !IsConst(addr);
        index_reg = (displacement >> 12) & 0x07;
        if (displacement & 0x8000)
          s = sformat("%s(PC,%s%s)",    /* address reg is index reg          */
                  Label2String(a1, GetBusWidth() / 4, bGetLabel, addr).c_str(),
                  GetAddrReg(index_reg),
                  sExtLong.c_str());
        else
          s = sformat("%s(PC,%s%s)",    /* data reg is index reg             */
                  Label2String(a1, GetBusWidth() / 4, bGetLabel, addr).c_str(),
                  GetDataReg(index_reg),
                  sExtWord.c_str());
        addr += 2;
        break;
      case 4:	//Immediate Data
        /*
        ** This one is tough
        */
        if (op_mode == LONG_SIZE)
          {
          bGetLabel = !IsConst(addr);
          a1 = GetSDWord(addr);
          s = sformat("#%s%s",
                      Label2String(a1, 8, bGetLabel, addr).c_str(),
                      sExtLong.c_str());
          addr += 4;
          }
        else if (op_mode == WORD_SIZE || op_mode == BYTE_SIZE)
          {
          displacement = GetSWord(addr);
          if (op_mode == WORD_SIZE)
            s = sformat("#%s%s",
                        Number2String(displacement, 4, addr).c_str(),
                        sExtWord.c_str());
          else if (op_mode == BYTE_SIZE)
            {
            displacement = (char)(displacement & 0xff);
            s = sformat("#%s%s",
                        Number2String(displacement, 2, addr).c_str(),
                        sExtByte.c_str());
            }
          addr += 2;
          }
        break;
      default :
        s = sformat("??%d??", reg);
        break;
      }	/*	end of switch reg	*/
    break;
  }	/*	end of switch mode	*/
return addr;
}

/*****************************************************************************/
/* DisassembleOptypeXX : disassemblers for the various 68000 op types        */
/*****************************************************************************/

adr_t Dasm68000::DisassembleOptype01(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)code; (void)optable_index; (void)smnemo;

int reg = code & 0x07;                  /* get register number               */
int16_t displacement = GetSWord(addr);  /* get word displacement             */
sparm = sformat("%s,#%s", GetAddrReg(reg),
                Number2String(displacement, 4, addr).c_str());
addr += 2;
return addr;
}

adr_t Dasm68000::DisassembleOptype02(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)smnemo;
// LEA
string e_a;                             /* place to put effective address    */
uint16_t dest_reg = (code >> 9) & 0x07; /* get destination reg               */
uint16_t source = code & 0x3f;          /* this is an effective address      */
                                        /* calculate effective address       */
addr = DisassembleEffectiveAddress(instaddr, addr, e_a, source, optable_index, 0);
#if 1
// this may be a bit inefficient, but it works ...
if (e_a.size() > 2 && e_a.substr(e_a.size() - 2) == sExtLong)
  e_a = e_a.substr(0, e_a.size() - 2);
else if (e_a.size() > 2 && e_a.substr(e_a.size() - 2) == sExtShort)
  e_a = e_a.substr(0, e_a.size() - 2) + sExtWord;  // no sign here.
#endif

sparm = sformat("%s,%s", e_a.c_str(), GetAddrReg(dest_reg));

return addr;
}

adr_t Dasm68000::DisassembleOptype03(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
uint16_t dest_reg = (code >> 9) & 0x07;
uint16_t source = code & 0x3f;          /* this is an effective address      */
int16_t op_mode = (code >> 6) & 0x07;   /* get op mode                       */
const char *reg;
int16_t dir;
int size;
if (op_mode == 3 || op_mode == 7)       /* adda type instructions            */
  {
  dir = 0;
  reg = GetAddrReg(dest_reg);
  if (op_mode == 3)
    size = 1;
  else
    size = 2;
  }
else
  {
  reg = GetDataReg(dest_reg);
  size = op_mode & 0x03;
  dir = (op_mode >> 2) & 0x01;
  }
op_mode = xlate_size(size, 0);
string e_a;
addr = DisassembleEffectiveAddress(instaddr, addr, e_a, source, optable_index, op_mode);

smnemo += GetSizeSuffix(size);
// this may be a bit inefficient, but it works ...
if (e_a.size() > 2 && e_a.substr(e_a.size() - 2) == GetSizeSuffix(size))
  e_a = e_a.substr(0, e_a.size() - 2);

sparm = (!dir) ?
    sformat("%s,%s", e_a.c_str(), reg) :
    sformat("%s,%s", reg, e_a.c_str());

return addr;
}

adr_t Dasm68000::DisassembleOptype04(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
string e_a1, e_a2;
int size = (code >> 12) & 0x03;
int16_t op_mode = xlate_size(size, 1);
int16_t source = code & 0x3f;
int16_t dest = (code >> 6) & 0x3f;
/*	on dest, mode and reg are in different order, so swap them	*/
dest = ((dest & 0x07) << 3) | (dest >> 3);

// this may be a bit inefficient, but it works ...
#pragma message("Mnemo size extension extraction is insufficient!")
string smnemotype;
if (smnemo.size() > 2 && smnemo[smnemo.size() - 2] == '.')
  {
  smnemotype = smnemo.substr(smnemo.size() - 2);
  smnemo = smnemo.substr(0, smnemo.size() - 2);
  }

addr = DisassembleEffectiveAddress(instaddr, addr, e_a1, source, optable_index, op_mode);
addr = DisassembleEffectiveAddress(instaddr, addr, e_a2, dest, optable_index, 0);
// if (smnemotype.size() && e_a1.size() > 2 && smnemotype == e_a1.substr(e_a1.size() - 2))
if (smnemotype.size() && e_a1.size() > 2 && e_a1[e_a1.size() - 2] == '.')
  e_a1 = e_a1.substr(0, e_a1.size() - 2);
if (smnemotype.size() && e_a2.size() > 2 && e_a2[e_a2.size() - 2] == '.')
  e_a2 = e_a2.substr(0, e_a2.size() - 2);
sparm = sformat("%s,%s", e_a1.c_str(), e_a2.c_str());
smnemo += smnemotype;
return addr;
}

adr_t Dasm68000::DisassembleOptype05(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)smnemo;
// PEA type instructions
int16_t source = code & 0x3f;
addr = DisassembleEffectiveAddress(instaddr, addr, sparm, source, optable_index, 0);
if (sparm.size() > 2 && sparm.substr(sparm.size() - 2) == sExtShort)
  sparm = sparm.substr(0, sparm.size() - 2) + sExtWord;  // no sign here.

return addr;
}

adr_t Dasm68000::DisassembleOptype06(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
// ADDQ type instructions

int16_t dest = code & 0x3f;
int size = (code >> 6) & 0x03;  // 0x03? sure? not 0x07? what's at bit 8? TODO: check
int16_t op_mode = xlate_size(size, 0);
int16_t data = (code >> 9) & 0x07;
if (data == 0)
  data = 8;
string s_data = Number2String(data, 1, addr - 2);
string e_a;
addr = DisassembleEffectiveAddress(instaddr, addr, e_a, dest, optable_index, 0);
smnemo += GetSizeSuffix(size);
sparm = sformat("#%s,%s", s_data.c_str(), e_a.c_str());

(void)op_mode;  // unused ATM
return addr;
}

adr_t Dasm68000::DisassembleOptype07(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
// TST type instructions

int16_t dest = code & 0x3f;
int size = (code >> 6) & 0x03;
int16_t op_mode = xlate_size(size, 0);
addr = DisassembleEffectiveAddress(instaddr, addr, sparm, dest, optable_index, 0);
if (sparm.size() > 2 && sparm.substr(sparm.size() - 2) == sExtShort)
  sparm = sparm.substr(0, sparm.size() - 2) + sExtWord;  // no sign here.

smnemo += GetSizeSuffix(size);

(void)op_mode;  // unused ATM
return addr;
}

adr_t Dasm68000::DisassembleOptype08(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)optable_index;
// branches

adr_t startaddr = addr - 1;
int32_t displacement = code & 0xff;
adr_t dest;
bool bGetLabel = !IsConst(startaddr);
if (displacement == 0)
  {
  bGetLabel &= !IsConst(addr);
  displacement = GetSWord(addr);
  // append ".L" if the displacement would allow a short instruction
  if ((asmtype == "bird") ||  // bird needs that in any case
      (displacement >= -128 && displacement <= 127))
    smnemo += sExtLong;
  dest = addr + displacement;
  startaddr = addr;
  addr += 2;
  }
else
  {
#if 0
  // not necessary; any decent assembler should do it this way
  if (asmtype != "bird")
    smnemo +=  sExtShort;
#endif
  displacement = (int32_t)((char)displacement);
  dest = addr + displacement;
  startaddr = addr - 1;
  }
sparm = Label2String(dest, GetBusWidth() / 4, bGetLabel, startaddr);
return addr;
}

adr_t Dasm68000::DisassembleOptype09(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)optable_index; (void)smnemo;
// UNLINK type instructions

int16_t reg = code & 0x07;
sparm = GetAddrReg(reg);

return addr;
}

adr_t Dasm68000::DisassembleOptype10(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)code; (void)optable_index; (void)smnemo; (void)sparm;
// RTS type instructions - no arguments
return addr;
}

adr_t Dasm68000::DisassembleOptype11(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
// LSR type instructions

int size = (code >> 6) & 0x03;
if (size < 3)  /* register shifts */
  {
  int16_t type = (code >> 5) & 0x01;
  int16_t dest = (code & 0x07);
  int16_t count = (code >> 9) & 0x07;
  smnemo += GetSizeSuffix(size);
  if (type)
    sparm = sformat("%s,%s", GetDataReg(count), GetDataReg(dest));
  else
    {
    if (count == 0)
      count = 8;
    sparm = sformat("#%s,%s",
                    Number2String(count, 1, addr - 2).c_str(),
                    GetDataReg(dest));
    }
  }
else  /* memory shifts */
  {
  int16_t dest = (code & 0x3f);
  addr = DisassembleEffectiveAddress(instaddr, addr, sparm, dest, optable_index, 0);
  }

return addr;
}

adr_t Dasm68000::DisassembleOptype12(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)optable_index;
// EXT type instruction

int16_t op_mode = ((code >> 6) & 0x07) - 1;
int16_t reg = code & 0x07;
smnemo += GetSizeSuffix(op_mode);
sparm = GetDataReg(reg);

return addr;
}

adr_t Dasm68000::DisassembleOptype13(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
// ADDI type instruction

int16_t dest = code & 0x3f;
int16_t size = (code >> 6) & 0x03;
string s_data;
if (size == 2)  /* long operation */
  {
  s_data = Number2String(GetSDWord(addr), 8, addr);
  addr += 4;
  }
else
  {
  // might be Labe2String-worthy
  s_data = Number2String(GetSWord(addr), 4, addr);
  addr += 2;
  }
string e_a;
addr = DisassembleEffectiveAddress(instaddr, addr, e_a, dest, optable_index, 0);

smnemo += GetSizeSuffix(size);
sparm = sformat("#%s,%s", s_data.c_str(), e_a.c_str());

return addr;
}

adr_t Dasm68000::DisassembleOptype14(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)optable_index;
// ABCD type instruction

int16_t dest = (code >> 9) & 0x07;
int16_t source = code & 0x07;
int16_t dest_type = (code >> 3) & 0x01;
int size = (code >> 6) & 0x03;
smnemo += GetSizeSuffix(size);
sparm = (dest_type) ?
    sformat("-(%s),-(%s)", GetAddrReg(source), GetAddrReg(dest)) :
    sformat("%s,%s", GetDataReg(source), GetDataReg(dest));
return addr;
}

adr_t Dasm68000::DisassembleOptype15(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
// CLR type instruction

int16_t dest = code & 0x3f;
int size = (code >> 6) & 0x03;
addr = DisassembleEffectiveAddress(instaddr, addr, sparm, dest, optable_index, 0);
if (sparm.size() > 2 && sparm.substr(sparm.size() - 2) == sExtShort)
  sparm = sparm.substr(0, sparm.size() - 2) + sExtWord;  // no sign here.
smnemo += GetSizeSuffix(size);
return addr;
}

adr_t Dasm68000::DisassembleOptype16(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)smnemo;
// CHK type instruction

int16_t dest = code & 0x3f;
int16_t reg = (code >> 9) & 0x07;
string e_a;
addr = DisassembleEffectiveAddress(instaddr, addr, e_a, dest, optable_index, WORD_SIZE);
sparm = sformat("%s,%s", e_a.c_str(), GetDataReg(reg));
return addr;
}

adr_t Dasm68000::DisassembleOptype17(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)optable_index; (void)smnemo;
// DBcc type instructions

int16_t reg = code & 0x07;
int16_t displacement = GetSWord(addr);
int32_t dest = (int32_t)addr + displacement;
bool bGetLabel = !IsConst(addr);
// sparm = sformat("%s,$%lx", GetDataReg(reg), dest);
sparm = sformat("%s,%s",
                GetDataReg(reg),
                Label2String(dest, GetBusWidth() / 4, bGetLabel, addr).c_str());
addr += 2;
return addr;
}

adr_t Dasm68000::DisassembleOptype18(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)optable_index; (void)smnemo;
// EXG type instructions

int16_t dest = code & 0x07;
int16_t source = (code >> 9) & 0x07;
int16_t op_mode = (code >> 3) & 0x1f;
const char *rx = "???", *ry = "???";
if (op_mode == 0x08)
  {
  rx = GetDataReg(source);
  ry = GetDataReg(dest);
  }
else if (op_mode == 0x09)
  {
  rx = GetAddrReg(source);
  ry = GetAddrReg(dest);
  }
else if (op_mode == 0x11)
  {
  rx = GetDataReg(source);
  ry = GetAddrReg(dest);
  }
sparm = sformat("%s,%s", rx, ry);
return addr;
}

adr_t Dasm68000::DisassembleOptype19(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)optable_index;
// CMPM type instructions

int16_t source = code & 0x07;
int16_t dest = (code >> 9) & 0x07;
int size = (code >> 6) & 0x03;
smnemo += GetSizeSuffix(size);
sparm = sformat("(%s)+,(%s)+", GetAddrReg(source), GetAddrReg(dest));
return addr;
}

adr_t Dasm68000::DisassembleOptype20(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)smnemo;
// BTST # type instruction

int16_t dest = code & 0x3f;
string s_data = Number2String(GetSWord(addr), 4, addr);
addr += 2;
string e_a;
addr = DisassembleEffectiveAddress(instaddr, addr, e_a, dest, optable_index, 0);
if (e_a.size() > 2 && e_a.substr(e_a.size() - 2) == sExtShort)
  e_a = e_a.substr(0, e_a.size() - 2) + sExtWord;  // no sign here.

if (smnemo.size() > 2 && smnemo[smnemo.size() - 2] != '.')
  {
  bool gas = (asmtype == "gas");
  int critsz = 2 + gas;
  int regnam = 0 + gas;
  int regnum = 1 + gas;
  if ((int)e_a.size() == critsz &&
      (!gas || e_a[0] == '%') &&
      (toupper(e_a[regnam]) == 'A' || toupper(e_a[regnam]) == 'D') &&
      isdigit(e_a[regnum]))
    ; // smnemo += sExtLong;
  else
    smnemo += sExtByte;
  }

sparm = sformat("#%s,%s", s_data.c_str(), e_a.c_str());
return addr;
}

adr_t Dasm68000::DisassembleOptype21(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)smnemo;
int16_t dest_reg = (code >> 9) & 0x07;
int16_t source = code & 0x3f;           /* this is an effective address      */
string e_a;
addr = DisassembleEffectiveAddress(instaddr, addr, e_a, source, optable_index, 0);
sparm = sformat("%s,%s", GetDataReg(dest_reg), e_a.c_str());
return addr;
}

adr_t Dasm68000::DisassembleOptype22(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)optable_index; (void)smnemo;
// moveq

int16_t data = (int16_t)((char)(code & 0xff));
uint16_t dest_reg = (code >> 9) & 0x07;
sparm = sformat("#%s%x,%s", hdrHex.c_str(), data, GetDataReg(dest_reg));
return addr;
}

adr_t Dasm68000::DisassembleOptype23(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
// movem

int16_t source = code & 0x3f;
int size = (code >> 6) & 0x01;
int direction = (code >> 10) & 0x01;
uint16_t reg_mask = GetUWord(addr);
addr += 2; // not sure about that - TODO: check!
string e_a;
addr = DisassembleEffectiveAddress(instaddr, addr, e_a, source, optable_index, 0);
if (e_a.size() > 2 && e_a.substr(e_a.size() - 2) == sExtShort)
  e_a = e_a.substr(0, e_a.size() - 2) + sExtWord;  // no sign here.
// process register mask
int16_t mode = (source >> 3) & 0x07;
int16_t flag = 0;
uint16_t mask;
bool first = true;
char regs[30];
regs[0] = '\0';
if (mode == 4)  /* predecrement */
  mask = 0x080;
else
  mask = 0x0100;
int i;
for (i = 0; i < 8; i++)
  {
  if (mask & reg_mask)  /* is this bit set? */
    {
    if (first)
      {
      strcpy(regs, GetAddrReg(i));
      first = false;
      flag++;
      }
    else if (flag == 0)
      {
      sprintf(regs + strlen(regs), "/%s", GetAddrReg(i));
      ++flag;
      }
    else if (flag)
      {
      ++flag;
      }
    }
  else if (flag)
    {
    if (flag > 2)
      sprintf(regs + strlen(regs), "-%s", GetAddrReg(i - 1));
    else if (flag == 2)
      sprintf(regs + strlen(regs), "/%s", GetAddrReg(i - 1));
    flag = 0;
    }
  if (mode == 4)
    mask >>= 1;
  else
    mask <<= 1;
  }

if (flag > 1)
  sprintf(regs + strlen(regs), "-%s", GetAddrReg(i - 1));
flag = 0;
if (mode == 4)  /* predecrement */
  mask = 0x8000;
else
  mask = 0x01;
for (i = 0; i < 8; ++i)
  {
  if (mask & reg_mask)	/*	is this bit set?	*/
    {
    if (first)
      {
      strcpy(regs, GetDataReg(i));
      first = false;
      flag++;
      }
    else if (flag == 0)
      {
      sprintf(regs + strlen(regs), "/%s", GetDataReg(i));
      ++flag;
      }
    else if (flag)
      {
      ++flag;
      }
    }
  else if (flag)
    {
    if (flag > 2)
      sprintf(regs + strlen(regs), "-%s", GetDataReg(i - 1));
    else if (flag == 2)
      sprintf(regs + strlen(regs), "/%s", GetDataReg(i - 1));
    flag = 0;
    }
  if (mode == 4)
    mask >>= 1;
  else
    mask <<= 1;
  }
if (flag > 1)
  sprintf(regs + strlen(regs), "-%s", GetDataReg(i - 1));
const char *f, *d;
if (!direction)
  {
  f = regs;
  d = e_a.c_str();
  }
else
  {
  f = e_a.c_str();
  d = regs;
  }
smnemo += GetSizeSuffix(size ? 2 : 1);
sparm = sformat("%s,%s", f, d);

return addr;
}

adr_t Dasm68000::DisassembleOptype24(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)optable_index; (void)smnemo;
// trap

int16_t vector = code & 0x0f;
sparm = sformat("#%s", Number2String(vector, 1, addr - 2).c_str());
return addr;
}

adr_t Dasm68000::DisassembleOptype25(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)code; (void)optable_index; (void)smnemo;
// stop

uint16_t data = GetUWord(addr);
sparm = sformat("#%s", Number2String(data, 4, addr).c_str());
addr += 2;
return addr;
}

adr_t Dasm68000::DisassembleOptype26(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)smnemo;
// move sr

int16_t source = code & 0x3f;
string e_a;
addr = DisassembleEffectiveAddress(instaddr, addr, e_a, source, optable_index, WORD_SIZE);
if (e_a.size() > 2 && e_a[e_a.size() - 2] == '.')
  {
  smnemo += e_a.substr(e_a.size() - 2);
  e_a = e_a.substr(0, e_a.size() - 2);
  }

uint16_t mt = code & 0x0600;
const char *reg = (mt == 0x0200 || mt == 0x0400) ? "CCR" : "SR";
sparm = (code & 0x400) ?
    sformat("%s,%s", e_a.c_str(), reg) :
    sformat("%s,%s", reg, e_a.c_str());

return addr;
}

adr_t Dasm68000::DisassembleOptype27(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)optable_index; (void)smnemo;
uint16_t data = GetUWord(addr);
sparm = sformat("#%s,%s",
                Number2String(data, 4, addr).c_str(),
                (code & 0x40) ? "SR" : "CCR");
addr += 2;
return addr;
}

adr_t Dasm68000::DisassembleOptype28(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)optable_index; (void)smnemo;
// MOVE USP
sparm = sformat((code & 0x08) ? "USP,%s" : "%s,USP", GetAddrReg(code & 0x07));
return addr;
}

adr_t Dasm68000::DisassembleOptype29(adr_t instaddr, adr_t addr, uint16_t code, int optable_index, string &smnemo, string &sparm)
{
(void)instaddr; (void)optable_index; (void)smnemo;
// MOVEP - move peripheral data
int data_reg = (code & 0x0e00) >> 9;
int adr_reg =  code & 0x07;
unsigned direction = code & 0x080;
int16_t displacement = GetSWord(addr);
if (direction)
  sparm = sformat("%s,%s(%s)",
                  GetDataReg(data_reg),
                  Number2String(displacement, 4, addr).c_str(),
                  GetAddrReg(adr_reg));
else
  sparm = sformat("%s(%s),%s",
                  Number2String(displacement, 4, addr).c_str(),
                  GetAddrReg(adr_reg),
                  GetDataReg(data_reg));
addr += 2;
return addr;
}


/*****************************************************************************/
/* DisassembleChanges : report dasm-specific state changes before/after addr */
/*****************************************************************************/

bool Dasm68000::DisassembleChanges
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
    LineChange chg;
    changes.push_back(chg);             /* append empty line before END      */
    chg.oper = MnemoCase("END");
    if (load != NO_ADDRESS &&           /* if entry point address given      */
        bLoadLabel)                     /* and labelling wanted              */
      chg.opnds = Label2String(load, GetBusWidth() / 4, true, load);
    changes.push_back(chg);
    }
  }
else // no bus check necessary, there's only one
  {
  adr_t org = DephaseOuter(addr, addr);
  adr_t prevorg = DephaseOuter(prevaddr, prevaddr);
  if (addr != prevaddr + prevsz)
    {
    if (!bAfterLine)
      {
      TMemory<adr_t, adr_t> *curPhArea  = FindPhase(addr);
      TMemory<adr_t, adr_t> *prevPhArea = FindPhase(prevaddr);

      adr_t prevphase = prevPhArea ? prevPhArea->GetType() : NO_ADDRESS;
      adr_t prevphstart = prevPhArea ? prevPhArea->GetStart() : NO_ADDRESS;
      adr_t curphase = curPhArea ? curPhArea->GetType() : NO_ADDRESS;
      adr_t curphstart = curPhArea ? curPhArea->GetStart() : NO_ADDRESS;
      LineChange chg;
      changes.push_back(chg);
      if (prevphase != NO_ADDRESS && prevphstart != curphstart)
        {
        chg.oper = MnemoCase("DEPHASE");
        changes.push_back(chg);
        changes.push_back(LineChange());
        }
      if (addr != NO_ADDRESS)
        {
        // TODO: check how that interferes with PIC!
        chg.oper = MnemoCase("ORG");
        chg.opnds = Number2String(addr, 6, NO_ADDRESS);
        changes.push_back(chg);
        if (curphase != NO_ADDRESS &&
            prevphstart != curphstart
// uncomment this to remove superfluous PHASE statements
//          && curphase != addr
            )
          {
          chg.oper = MnemoCase("PHASE");
          chg.opnds = Number2String(curphase, 6, NO_ADDRESS);
          changes.push_back(chg);
          }
        changes.push_back(LineChange());
        }
      }
    }
  (void)org; (void)prevorg;  // unused ATM
  }

return Disassembler::DisassembleChanges(addr, prevaddr, prevsz, bAfterLine, changes, bus);
}


/*===========================================================================*/
/* Dasm68008 class members                                                   */
/*===========================================================================*/

/*****************************************************************************/
/* Dasm68008 : constructor                                                   */
/*****************************************************************************/

Dasm68008::Dasm68008(Application *pApp)
  : Dasm68000(pApp)
{
}

/*****************************************************************************/
/* ~Dasm68008 : destructor                                                   */
/*****************************************************************************/

Dasm68008::~Dasm68008(void)
{
}
