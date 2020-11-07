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
 * The 6301 disassembler engine is based on contributions to f9dasm        *
 * by Rainer Buchty, so                                                    *
 *                    Parts Copyright (C) 2015  Rainer Buchty              *
 ***************************************************************************/

/*****************************************************************************/
/* Dasm6301.cpp : 6301 disassembler classes implementation                   */
/*****************************************************************************/

#include "Dasm6301.h"

/*****************************************************************************/
/* Create6301 : create a 6301 disassembler                                   */
/*****************************************************************************/

static Disassembler *Create6301()
{
Disassembler *pDasm = new Dasm6301;
if (pDasm) pDasm->Setup();
return pDasm;
}

/*****************************************************************************/
/* Auto-registration                                                         */
/*****************************************************************************/

static bool bRegistered[] =
  {
  RegisterDisassembler("6301", Create6301),
  RegisterDisassembler("6303", Create6301),
  };


/*===========================================================================*/
/* Dasm6301 class members                                                    */
/*===========================================================================*/

/*****************************************************************************/
/* m6301_codes : table of all 6301 instruction bytes and types               */
/*****************************************************************************/

uint8_t Dasm6301::h6301_codes[512] =
  {
  _ill  ,_nom,   _nop  ,_imp,   _ill  ,_nom,   _ill  ,_nom,     /* 00..03 */
  _lsrd ,_imp,   _asld ,_imp,   _tap  ,_imp,   _tpa  ,_imp,     /* 04..07 */
  _inx  ,_imp,   _dex  ,_imp,   _clv  ,_imp,   _sev  ,_imp,     /* 08..0B */
  _clc  ,_imp,   _sec  ,_imp,   _cli  ,_imp,   _sei  ,_imp,     /* 0C..0F */
  _sba  ,_imp,   _cba  ,_imp,   _ill  ,_nom,   _ill  ,_nom,     /* 10..13 */
  _ill  ,_nom,   _ill  ,_nom,   _tab  ,_imp,   _tba  ,_imp,     /* 14..17 */
  _xgdx ,_imp,   _daa  ,_imp,   _slp  ,_imp,   _aba  ,_imp,     /* 18..1B: extra 0x18/xgdx, 0x1a/slp */  
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 1C..1F */
  _bra  ,_reb,   _brn  ,_reb,   _bhi  ,_reb,   _bls  ,_reb,     /* 20..23 */
  _bcc  ,_reb,   _bcs  ,_reb,   _bne  ,_reb,   _beq  ,_reb,     /* 24..27 */
  _bvc  ,_reb,   _bvs  ,_reb,   _bpl  ,_reb,   _bmi  ,_reb,     /* 28..2B */
  _bge  ,_reb,   _blt  ,_reb,   _bgt  ,_reb,   _ble  ,_reb,     /* 2C..2F */
  _tsx  ,_imp,   _ins  ,_imp,   _pula ,_imp,   _pulb ,_imp,     /* 30..33 */
  _des  ,_imp,   _txs  ,_imp,   _psha ,_imp,   _pshb ,_imp,     /* 34..37 */
  _pulx ,_imp,   _rts  ,_imp,   _abx  ,_imp,   _rti  ,_imp,     /* 38..3B */
  _pshx ,_imp,   _mul  ,_imp,   _wai  ,_imp,   _swi  ,_imp,     /* 3C..3F */
  _nega ,_imp,   _ill  ,_nom,   _ill  ,_nom,   _coma ,_imp,     /* 40..43 */
  _lsra ,_imp,   _ill  ,_nom,   _rora ,_imp,   _asra ,_imp,     /* 44..47 */
  _asla ,_imp,   _rola ,_imp,   _deca ,_imp,   _ill  ,_nom,     /* 48..4B */
  _inca ,_imp,   _tsta ,_imp,   _ill  ,_nom,   _clra ,_imp,     /* 4C..4F */
  _negb ,_imp,   _ill  ,_nom,   _ill  ,_nom,   _comb ,_imp,     /* 50..53 */
  _lsrb ,_imp,   _ill  ,_nom,   _rorb ,_imp,   _asrb ,_imp,     /* 54..57 */
  _aslb ,_imp,   _rolb ,_imp,   _decb ,_imp,   _ill  ,_nom,     /* 58..5B */
  _incb ,_imp,   _tstb ,_imp,   _ill  ,_nom,   _clrb ,_imp,     /* 5C..5F */
  _neg  ,_ix8,   _aim  ,_bi,    _oim  ,_bi,    _com  ,_ix8,     /* 60..63: extra 0x62/aim, 0x63/oim */
  _lsr  ,_ix8,   _eim  ,_bi,    _ror  ,_ix8,   _asr  ,_ix8,     /* 64..67: extra 0x65/eim */
  _asl  ,_ix8,   _rol  ,_ix8,   _dec  ,_ix8,   _tim  ,_bi,      /* 68..6B: extra 0x6b/tim */
  _inc  ,_ix8,   _tst  ,_ix8,   _jmp  ,_ix8,   _clr  ,_ix8,     /* 6C..6F */
  _neg  ,_ext,   _aim  ,_bd,    _oim  ,_bd,    _com  ,_ext,     /* 70..73: extra 0x72/aim, 0x73/oim */
  _lsr  ,_ext,   _eim  ,_bd,    _ror  ,_ext,   _asr  ,_ext,     /* 74..77: extra 0x75/eim */
  _asl  ,_ext,   _rol  ,_ext,   _dec  ,_ext,   _tim  ,_bd,      /* 78..7B: extra 0x7b/tim */
  _inc  ,_ext,   _tst  ,_ext,   _jmp  ,_ext,   _clr  ,_ext,     /* 7C..7F */
  _suba ,_imb,   _cmpa ,_imb,   _sbca ,_imb,   _subd ,_imw,     /* 80..83 */
  _anda ,_imb,   _bita ,_imb,   _lda  ,_imb,   _ill  ,_nom,     /* 84..87 */
  _eora ,_imb,   _adca ,_imb,   _ora  ,_imb,   _adda ,_imb,     /* 88..8B */
  _cpx  ,_imw,   _bsr  ,_reb,   _lds  ,_imw,   _ill  ,_nom,     /* 8C..8F */
  _suba ,_dir,   _cmpa ,_dir,   _sbca ,_dir,   _subd ,_dir,     /* 90..93 */
  _anda ,_dir,   _bita ,_dir,   _lda  ,_dir,   _sta  ,_dir,     /* 94..97 */
  _eora ,_dir,   _adca ,_dir,   _ora  ,_dir,   _adda ,_dir,     /* 98..9B */
  _cpx  ,_dir,   _jsr  ,_dir,   _lds  ,_dir,   _sts  ,_dir,     /* 9C..9F */
  _suba ,_ix8,   _cmpa ,_ix8,   _sbca ,_ix8,   _subd ,_ix8,     /* A0..A3 */
  _anda ,_ix8,   _bita ,_ix8,   _lda  ,_ix8,   _sta  ,_ix8,     /* A4..A7 */
  _eora ,_ix8,   _adca ,_ix8,   _ora  ,_ix8,   _adda ,_ix8,     /* A8..AB */
  _cpx  ,_ix8,   _jsr  ,_ix8,   _lds  ,_ix8,   _sts  ,_ix8,     /* AC..AF */
  _suba ,_ext,   _cmpa ,_ext,   _sbca ,_ext,   _subd ,_ext,     /* B0..B3 */
  _anda ,_ext,   _bita ,_ext,   _lda  ,_ext,   _sta  ,_ext,     /* B4..B7 */
  _eora ,_ext,   _adca ,_ext,   _ora  ,_ext,   _adda ,_ext,     /* B8..BB */
  _cpx  ,_ext,   _jsr  ,_ext,   _lds  ,_ext,   _sts  ,_ext,     /* BC..BF */
  _subb ,_imb,   _cmpb ,_imb,   _sbcb ,_imb,   _addd ,_imw,     /* C0..C3 */
  _andb ,_imb,   _bitb ,_imb,   _ldb  ,_imb,   _ill  ,_nom,     /* C4..C7 */
  _eorb ,_imb,   _adcb ,_imb,   _orb  ,_imb,   _addb ,_imb,     /* C8..CB */
  _ldd  ,_imw,   _ill  ,_nom,   _ldx  ,_imw,   _ill  ,_nom,     /* CC..CF */
  _subb ,_dir,   _cmpb ,_dir,   _sbcb ,_dir,   _addd ,_dir,     /* D0..D3 */
  _andb ,_dir,   _bitb ,_dir,   _ldb  ,_dir,   _stb  ,_dir,     /* D4..D7 */
  _eorb ,_dir,   _adcb ,_dir,   _orb  ,_dir,   _addb ,_dir,     /* D8..DB */
  _ldd  ,_dir,   _std  ,_dir,   _ldx  ,_dir,   _stx  ,_dir,     /* DC..DF */
  _subb ,_ix8,   _cmpb ,_ix8,   _sbcb ,_ix8,   _addd ,_ix8,     /* E0..E3 */
  _andb ,_ix8,   _bitb ,_ix8,   _ldb  ,_ix8,   _stb  ,_ix8,     /* E4..E7 */
  _eorb ,_ix8,   _adcb ,_ix8,   _orb  ,_ix8,   _addb ,_ix8,     /* E8..EB */
  _ldd  ,_ix8,   _std  ,_ix8,   _ldx  ,_ix8,   _stx  ,_ix8,     /* EC..EF */
  _subb ,_ext,   _cmpb ,_ext,   _sbcb ,_ext,   _addd ,_ext,     /* F0..F3 */
  _andb ,_ext,   _bitb ,_ext,   _ldb  ,_ext,   _stb  ,_ext,     /* F4..F7 */
  _eorb ,_ext,   _adcb ,_ext,   _orb  ,_ext,   _addb ,_ext,     /* F8..FB */
  _ldd  ,_ext,   _std  ,_ext,   _ldx  ,_ext,   _stx  ,_ext,     /* FC..FF */
  };

/*****************************************************************************/
/* opcodes : additional opcodes over 6800                                    */
/*****************************************************************************/

OpCode Dasm6301::opcodes[mnemo6301_count - mnemo6801_count] =
  {
    { "AIM",   Data },                  /* _aim                              */
    { "EIM",   Data },                  /* _eim                              */
    { "OIM",   Data },                  /* _oim                              */
    { "TIM",   Data },                  /* _tim                              */
    { "SLP",   Data },                  /* _slp                              */
    { "XGDX",  Data },                  /* _xgdx                             */
  };

/*****************************************************************************/
/* Dasm6301 : constructor                                                    */
/*****************************************************************************/

Dasm6301::Dasm6301(void)
{
codes = h6301_codes;
mnemo.resize(mnemo6301_count);          /* set up additional mnemonics       */
for (int i = 0; i < mnemo6301_count - mnemo6801_count; i++)
  mnemo[mnemo6801_count + i] = opcodes[i];
}

/*****************************************************************************/
/* ~Dasm6301 : destructor                                                    */
/*****************************************************************************/

Dasm6301::~Dasm6301(void)
{
}

/*****************************************************************************/
/* ParseCode : parse instruction at given memory address for labels          */
/*****************************************************************************/

addr_t Dasm6301::ParseCode
    (
    addr_t addr,
    int bus                             /* ignored for 6800 and derivates    */
    )
{
uint8_t O, T, M;
uint16_t W;
int MI;
const char *I;
addr_t PC = addr;
bool bSetLabel;

PC = FetchInstructionDetails(PC, O, T, M, W, MI, I);

switch (M)                              /* which mode is this ?              */
  {
  case _bi :                            /* Bit Manipulation indexed          */
    SetDefLabelUsed(PC, bus);           /* the bit part                      */
    PC++;
    bSetLabel = !IsConst(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);
    PC++;
    break;
  case _bd :                            /* Bit Manipulation direct           */
    T = GetUByte(PC);
    SetDefLabelUsed(PC, bus);           /* the bit part                      */
    PC++;
    bSetLabel = !IsConst(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);
    else
      {
      W = GetUByte(PC);
      W = (uint16_t)PhaseInner(W, PC);
      AddLabel(W, mnemo[MI].memType, "", true);
      }
    PC++;
    break;
  default :                             /* anything else is handled by base  */
    return Dasm6801::ParseCode(addr, bus);
  }
return PC - addr;                       /* pass back # processed bytes       */
}

/*****************************************************************************/
/* DisassembleCode : disassemble code instruction at given memory address    */
/*****************************************************************************/

addr_t Dasm6301::DisassembleCode
    (
    addr_t addr,
    string &smnemo,
    string &sparm,
    int bus                             /* ignored for 6800 and derivates    */
    )
{
uint8_t O, T, M;
uint16_t W;
addr_t Wrel;
int MI;
const char *I;
addr_t PC = addr;
bool bGetLabel;
Label *lbl;

PC = FetchInstructionDetails(PC, O, T, M, W, MI, I, &smnemo);

switch (M)                              /* which mode is this?               */
  {
  case _bi :                            /* Bit Manipulation indexed          */
    T = GetUByte(PC);
    lbl = FindLabel(PC, Const, bus);    /* the bit part                      */
    sparm = (lbl ? lbl->GetText() : Number2String(T, 2, PC)) + ",";
    PC++;
    bGetLabel = !IsConst(PC);
    T = GetUByte(PC);
    Wrel = GetRelative(PC);
    lbl = (bGetLabel || Wrel) ? NULL : FindLabel(PC, Const, bus);
    if (Wrel)
      {
      W = (int)((unsigned char)T) + (uint16_t)Wrel;
      sparm += Label2String((addr_t)((int)((unsigned char)T)), 4,
                            bGetLabel, PC) + ",X";
      }
    else if (lbl)
      sparm += lbl->GetText() + ",X";
    else if (!T && !showIndexedModeZeroOperand)
      sparm += ",X";   /* omit '$00', unless the user has set the 'showzero' option */
    else
      sparm += Number2String(T, 2, PC) + ",X";
    PC++;
    break;
  case _bd :                            /* Bit Manipulation direct           */
    {
    lbl = FindLabel(PC, Const, bus);    /* the bit part                      */
    M = GetUByte(PC);
    string snum = lbl ? lbl->GetText() : Number2String(M, 2, PC);
    PC++;
    bGetLabel = !IsConst(PC);
    lbl = bGetLabel ? NULL : FindLabel(PC, Const, bus);
    W = GetUByte(PC);
    if (bGetLabel)
      W = (uint16_t)PhaseInner(W, PC);
    string slbl = lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
    sparm = sformat("#%s,%s", 
                    snum.c_str(),
                    slbl.c_str());
    PC++;
    }
    break;
  default :                             /* anything else is handled by base  */
    return Dasm6801::DisassembleCode(addr, smnemo, sparm, bus);
  }
return PC - addr;                       /* pass back # processed bytes       */
}
