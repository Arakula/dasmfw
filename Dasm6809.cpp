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
 * The 6809 disassembler engine is based on dasm09, last found at          *
 * http://koti.mbnet.fi/~atjs/mc6809/Disassembler/dasm09.TGZ , so          *
 *                    Parts Copyright (C) 2000  Arto Salmi                 *
 ***************************************************************************/

/*****************************************************************************/
/* Dasm6809.cpp : 6809 disassembler implementation                           */
/*****************************************************************************/

#include "Dasm6809.h"

/*****************************************************************************/
/* Create6809 : create an 6809 disassembler                                  */
/*****************************************************************************/

static Disassembler *Create6809()
{
Disassembler *pDasm = new Dasm6809;
if (pDasm) pDasm->Setup();
return pDasm;
}

/*****************************************************************************/
/* Auto-registration                                                         */
/*****************************************************************************/

static bool bRegistered = RegisterDisassembler("6809", Create6809);


/*****************************************************************************/
/* os9_codes : OS/9 entry points                                             */
/*****************************************************************************/

char *Dasm6809::os9_codes[0x100] =
  {
  "F$Link",      "F$Load",      "F$UnLink",    "F$Fork",        /* 00..03 */
  "F$Wait",      "F$Chain",     "F$Exit",      "F$Mem",         /* 04..07 */
  "F$Send",      "F$Icpt",      "F$Sleep",     "F$SSpd",        /* 08..0B */
  "F$ID",        "F$SPrior",    "F$SSWI",      "F$Perr",        /* 0C..0F */
  "F$PrsNam",    "F$CmpNam",    "F$SchBit",    "F$AllBit",      /* 10..13 */
  "F$DelBit",    "F$Time",      "F$STime",     "F$CRC",         /* 14..17 */
  "F$GPrDsc",    "F$GBlkMp",    "F$GModDr",    "F$CpyMem",      /* 18..1B */
  "F$SUser",     "F$UnLoad",    "F$Alarm",     "F$",            /* 1C..1F */
  "F$",          "F$NMLink",    "F$NMLoad",    "F$Ctime",       /* 20..23 */
  "F$Cstime",    "F$CTswi2",    "F$",          "F$VIRQ",        /* 24..27 */
  "F$SRqMem",    "F$SRtMem",    "F$IRQ",       "F$IOQu",        /* 28..2B */
  "F$AProc",     "F$NProc",     "F$VModul",    "F$Find64",      /* 2C..2F */
  "F$All64",     "F$Ret64",     "F$SSvc",      "F$IODel",       /* 30..33 */
  "F$SLink",     "F$Boot",      "F$BtMem",     "F$GProcP",      /* 34..37 */
  "F$Move",      "F$AllRAM",    "F$AllImg",    "F$DelImg",      /* 38..3B */
  "F$SetImg",    "F$FreeLB",    "F$FreeHB",    "F$AllTsk",      /* 3C..3F */
  "F$DelTsk",    "F$SetTsk",    "F$ResTsk",    "F$RelTsk",      /* 40..43 */
  "F$DATLog",    "F$DATTmp",    "F$LDAXY",     "F$LDAXYP",      /* 44..47 */
  "F$LDDDXY",    "F$LDABX",     "F$STABX",     "F$AllPrc",      /* 48..4B */
  "F$DelPrc",    "F$ELink",     "F$FModul",    "F$MapBlk",      /* 4C..4F */
  "F$ClrBlk",    "F$DelRAM",    "F$GCMDir",    "F$AlHRam",      /* 50..53 */
  "F$",          "F$",          "F$",          "F$",            /* 54..57 */
  "F$",          "F$",          "F$",          "F$",            /* 58..5B */
  "F$",          "F$",          "F$",          "F$",            /* 5C..5F */
  "F$",          "F$",          "F$",          "F$",            /* 60..63 */
  "F$",          "F$",          "F$",          "F$",            /* 64..67 */
  "F$",          "F$",          "F$",          "F$",            /* 68..6B */
  "F$",          "F$",          "F$",          "F$",            /* 6C..6F */
  "F$",          "F$",          "F$",          "F$",            /* 70..73 */
  "F$",          "F$",          "F$",          "F$",            /* 74..77 */
  "F$",          "F$",          "F$",          "F$",            /* 78..7B */
  "F$",          "F$",          "F$",          "F$",            /* 7C..7F */
  "I$Attach",    "I$Detach",    "I$Dup",       "I$Create",      /* 80..83 */
  "I$Open",      "I$MakDir",    "I$Chgdir",    "I$Delete",      /* 84..87 */
  "I$Seek",      "I$Read",      "I$Write",     "I$ReadLn",      /* 88..8B */
  "I$WritLn",    "I$GetStt",    "I$SetStt",    "I$Close",       /* 8C..8F */
  "I$DeletX",    "F$",          "F$",          "F$",            /* 90..93 */
  "F$",          "F$",          "F$",          "F$",            /* 94..97 */
  "F$",          "F$",          "F$",          "F$",            /* 98..9B */
  "F$",          "F$",          "F$",          "F$",            /* 9C..9F */
  "F$",          "F$",          "F$",          "F$",            /* A0..A3 */
  "F$",          "F$",          "F$",          "F$",            /* A4..A7 */
  "F$",          "F$",          "F$",          "F$",            /* A8..AB */
  "F$",          "F$",          "F$",          "F$",            /* AC..AF */
  "F$",          "F$",          "F$",          "F$",            /* B0..B3 */
  "F$",          "F$",          "F$",          "F$",            /* B4..B7 */
  "F$",          "F$",          "F$",          "F$",            /* B8..BB */
  "F$",          "F$",          "F$",          "F$",            /* BC..BF */
  "F$",          "F$",          "F$",          "F$",            /* C0..C3 */
  "F$",          "F$",          "F$",          "F$",            /* C4..C7 */
  "F$",          "F$",          "F$",          "F$",            /* C8..CB */
  "F$",          "F$",          "F$",          "F$",            /* CC..CF */
  "F$",          "F$",          "F$",          "F$",            /* D0..D3 */
  "F$",          "F$",          "F$",          "F$",            /* D4..D7 */
  "F$",          "F$",          "F$",          "F$",            /* D8..DB */
  "F$",          "F$",          "F$",          "F$",            /* DC..DF */
  "F$",          "F$",          "F$",          "F$",            /* E0..E3 */
  "F$",          "F$",          "F$",          "F$",            /* E4..E7 */
  "F$",          "F$",          "F$",          "F$",            /* E8..EB */
  "F$",          "F$",          "F$",          "F$",            /* EC..EF */
  "F$",          "F$",          "F$",          "F$",            /* F0..F3 */
  "F$",          "F$",          "F$",          "F$",            /* F4..F7 */
  "F$",          "F$",          "F$",          "F$",            /* F8..FB */
  "F$",          "F$",          "F$",          "F$"             /* FC..FF */
  };


/*===========================================================================*/
/* Dasm6809 class members                                                    */
/*===========================================================================*/

/*****************************************************************************/
/* m6809_codes : table of all 6809 instruction bytes and types               */
/*****************************************************************************/

uint8_t Dasm6809::m6809_codes[512] =
  {
  _neg  ,_dir,   _ill  ,_nom,   _ill  ,_nom,   _com  ,_dir,     /* 00..03 */
  _lsr  ,_dir,   _ill  ,_nom,   _ror  ,_dir,   _asr  ,_dir,     /* 04..07 */
  _asl  ,_dir,   _rol  ,_dir,   _dec  ,_dir,   _ill  ,_nom,     /* 08..0B */
  _inc  ,_dir,   _tst  ,_dir,   _jmp  ,_dir,   _clr  ,_dir,     /* 0C..0F */
  _ill  ,_nom,   _ill  ,_nom,   _nop  ,_imp,   _sync ,_imp,     /* 10..13 */
  _ill  ,_nom,   _ill  ,_nom,   _lbra ,_rew,   _lbsr ,_rew,     /* 14..17 */
  _ill  ,_nom,   _daa  ,_imp,   _orcc ,_imb,   _ill  ,_nom,     /* 18..1B */
  _andcc,_imb,   _sex  ,_imp,   _exg  ,_r1 ,   _tfr  ,_r1 ,     /* 1C..1F */
  _bra  ,_reb,   _brn  ,_reb,   _bhi  ,_reb,   _bls  ,_reb,     /* 20..23 */
  _bcc  ,_reb,   _bcs  ,_reb,   _bne  ,_reb,   _beq  ,_reb,     /* 24..27 */
  _bvc  ,_reb,   _bvs  ,_reb,   _bpl  ,_reb,   _bmi  ,_reb,     /* 28..2B */
  _bge  ,_reb,   _blt  ,_reb,   _bgt  ,_reb,   _ble  ,_reb,     /* 2C..2F */
  _leax ,_ind,   _leay ,_ind,   _leas ,_ind,   _leau ,_ind,     /* 30..33 */
  _pshs ,_r2 ,   _puls ,_r2 ,   _pshu ,_r3 ,   _pulu ,_r3 ,     /* 34..37 */
  _ill  ,_nom,   _rts  ,_imp,   _abx  ,_imp,   _rti  ,_imp,     /* 38..3B */
  _cwai ,_imb,   _mul  ,_imp,   _reset,_imp,   _swi  ,_imp,     /* 3C..3F */
  _nega ,_imp,   _ill  ,_nom,   _ill  ,_nom,   _coma ,_imp,     /* 40..43 */
  _lsra ,_imp,   _ill  ,_nom,   _rora ,_imp,   _asra ,_imp,     /* 44..47 */
  _asla ,_imp,   _rola ,_imp,   _deca ,_imp,   _ill  ,_nom,     /* 48..4B */
  _inca ,_imp,   _tsta ,_imp,   _ill  ,_nom,   _clra ,_imp,     /* 4C..4F */
  _negb ,_imp,   _ill  ,_nom,   _ill  ,_nom,   _comb ,_imp,     /* 50..53 */
  _lsrb ,_imp,   _ill  ,_nom,   _rorb ,_imp,   _asrb ,_imp,     /* 54..57 */
  _aslb ,_imp,   _rolb ,_imp,   _decb ,_imp,   _ill  ,_nom,     /* 58..5B */
  _incb ,_imp,   _tstb ,_imp,   _ill  ,_nom,   _clrb ,_imp,     /* 5C..5F */
  _neg  ,_ind,   _ill  ,_nom,   _ill  ,_nom,   _com  ,_ind,     /* 60..63 */
  _lsr  ,_ind,   _ill  ,_nom,   _ror  ,_ind,   _asr  ,_ind,     /* 64..67 */
  _asl  ,_ind,   _rol  ,_ind,   _dec  ,_ind,   _ill  ,_nom,     /* 68..6B */
  _inc  ,_ind,   _tst  ,_ind,   _jmp  ,_ind,   _clr  ,_ind,     /* 6C..6F */
  _neg  ,_ext,   _ill  ,_nom,   _ill  ,_nom,   _com  ,_ext,     /* 70..73 */
  _lsr  ,_ext,   _ill  ,_nom,   _ror  ,_ext,   _asr  ,_ext,     /* 74..77 */
  _asl  ,_ext,   _rol  ,_ext,   _dec  ,_ext,   _ill  ,_nom,     /* 78..7B */
  _inc  ,_ext,   _tst  ,_ext,   _jmp  ,_ext,   _clr  ,_ext,     /* 7C..7F */
  _suba ,_imb,   _cmpa ,_imb,   _sbca ,_imb,   _subd ,_imw,     /* 80..83 */
  _anda ,_imb,   _bita ,_imb,   _lda  ,_imb,   _ill  ,_nom,     /* 84..87 */
  _eora ,_imb,   _adca ,_imb,   _ora  ,_imb,   _adda ,_imb,     /* 88..8B */
  _cmpx ,_imw,   _bsr  ,_reb,   _ldx  ,_imw,   _ill  ,_nom,     /* 8C..8F */
  _suba ,_dir,   _cmpa ,_dir,   _sbca ,_dir,   _subd ,_dir,     /* 90..93 */
  _anda ,_dir,   _bita ,_dir,   _lda  ,_dir,   _sta  ,_dir,     /* 94..97 */
  _eora ,_dir,   _adca ,_dir,   _ora  ,_dir,   _adda ,_dir,     /* 98..9B */
  _cmpx ,_dir,   _jsr  ,_dir,   _ldx  ,_dir,   _stx  ,_dir,     /* 9C..9F */
  _suba ,_ind,   _cmpa ,_ind,   _sbca ,_ind,   _subd ,_ind,     /* A0..A3 */
  _anda ,_ind,   _bita ,_ind,   _lda  ,_ind,   _sta  ,_ind,     /* A4..A7 */
  _eora ,_ind,   _adca ,_ind,   _ora  ,_ind,   _adda ,_ind,     /* A8..AB */
  _cmpx ,_ind,   _jsr  ,_ind,   _ldx  ,_ind,   _stx  ,_ind,     /* AC..AF */
  _suba ,_ext,   _cmpa ,_ext,   _sbca ,_ext,   _subd ,_ext,     /* B0..B3 */
  _anda ,_ext,   _bita ,_ext,   _lda  ,_ext,   _sta  ,_ext,     /* B4..B7 */
  _eora ,_ext,   _adca ,_ext,   _ora  ,_ext,   _adda ,_ext,     /* B8..BB */
  _cmpx ,_ext,   _jsr  ,_ext,   _ldx  ,_ext,   _stx  ,_ext,     /* BC..BF */
  _subb ,_imb,   _cmpb ,_imb,   _sbcb ,_imb,   _addd ,_imw,     /* C0..C3 */
  _andb ,_imb,   _bitb ,_imb,   _ldb  ,_imb,   _ill  ,_nom,     /* C4..C7 */
  _eorb ,_imb,   _adcb ,_imb,   _orb  ,_imb,   _addb ,_imb,     /* C8..CB */
  _ldd  ,_imw,   _ill  ,_nom,   _ldu  ,_imw,   _ill  ,_nom,     /* CC..CF */
  _subb ,_dir,   _cmpb ,_dir,   _sbcb ,_dir,   _addd ,_dir,     /* D0..D3 */
  _andb ,_dir,   _bitb ,_dir,   _ldb  ,_dir,   _stb  ,_dir,     /* D4..D7 */
  _eorb ,_dir,   _adcb ,_dir,   _orb  ,_dir,   _addb ,_dir,     /* D8..DB */
  _ldd  ,_dir,   _std  ,_dir,   _ldu  ,_dir,   _stu  ,_dir,     /* DC..DF */
  _subb ,_ind,   _cmpb ,_ind,   _sbcb ,_ind,   _addd ,_ind,     /* E0..E3 */
  _andb ,_ind,   _bitb ,_ind,   _ldb  ,_ind,   _stb  ,_ind,     /* E4..E7 */
  _eorb ,_ind,   _adcb ,_ind,   _orb  ,_ind,   _addb ,_ind,     /* E8..EB */
  _ldd  ,_ind,   _std  ,_ind,   _ldu  ,_ind,   _stu  ,_ind,     /* EC..EF */
  _subb ,_ext,   _cmpb ,_ext,   _sbcb ,_ext,   _addd ,_ext,     /* F0..F3 */
  _andb ,_ext,   _bitb ,_ext,   _ldb  ,_ext,   _stb  ,_ext,     /* F4..F7 */
  _eorb ,_ext,   _adcb ,_ext,   _orb  ,_ext,   _addb ,_ext,     /* F8..FB */
  _ldd  ,_ext,   _std  ,_ext,   _ldu  ,_ext,   _stu  ,_ext,     /* FC..FF */
  };

/*****************************************************************************/
/* m6809_codes10 : $10 extended instruction 2nd byte                         */
/*****************************************************************************/

uint8_t Dasm6809::m6809_codes10[512] =
  {
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 00..03 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 04..07 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 08..0B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 0C..0F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 10..13 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 14..17 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 18..1B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 1C..1F */
  _ill  ,_nom,   _lbrn ,_rew,   _lbhi ,_rew,   _lbls ,_rew,     /* 20..23 */
  _lbcc ,_rew,   _lbcs ,_rew,   _lbne ,_rew,   _lbeq ,_rew,     /* 24..27 */
  _lbvc ,_rew,   _lbvs ,_rew,   _lbpl ,_rew,   _lbmi ,_rew,     /* 28..2B */
  _lbge ,_rew,   _lblt ,_rew,   _lbgt ,_rew,   _lble ,_rew,     /* 2C..2F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 30..33 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 34..37 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 38..3B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _swi2 ,_imp,     /* 3C..3F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 40..43 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 44..47 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 48..4B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 4C..4F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 50..53 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 54..57 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 58..5B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 5C..5F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 60..63 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 64..67 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 68..6B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 6C..6F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 70..73 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 74..77 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 78..7B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 7C..7F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _cmpd ,_imw,     /* 80..83 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 84..87 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 88..8B */
  _cmpy ,_imw,   _ill  ,_nom,   _ldy  ,_imw,   _ill  ,_nom,     /* 8C..8F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _cmpd ,_dir,     /* 90..93 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 94..97 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 98..9B */
  _cmpy ,_dir,   _ill  ,_nom,   _ldy  ,_dir,   _sty  ,_dir,     /* 9C..9F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _cmpd ,_ind,     /* A0..A3 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* A4..A7 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* A8..AB */
  _cmpy ,_ind,   _ill  ,_nom,   _ldy  ,_ind,   _sty  ,_ind,     /* AC..AF */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _cmpd ,_ext,     /* B0..B3 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* B4..B7 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* B8..BB */
  _cmpy ,_ext,   _ill  ,_nom,   _ldy  ,_ext,   _sty  ,_ext,     /* BC..BF */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* C0..C3 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* C4..C7 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* C8..CB */
  _ill  ,_nom,   _ill  ,_nom,   _lds  ,_imw,   _ill  ,_nom,     /* CC..CF */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* D0..D3 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* D4..D7 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* D8..DB */
  _ill  ,_nom,   _ill  ,_nom,   _lds  ,_dir,   _sts  ,_dir,     /* DC..DF */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* E0..E3 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* E4..E7 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* E8..EB */
  _ill  ,_nom,   _ill  ,_nom,   _lds  ,_ind,   _sts  ,_ind,     /* EC..EF */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* F0..F3 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* F4..F7 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* F8..FB */
  _ill  ,_nom,   _ill  ,_nom,   _lds  ,_ext,   _sts  ,_ext,     /* FC..FF */
  };

/*****************************************************************************/
/* m6809_codes11 : $11 extended instruction 2nd byte                         */
/*****************************************************************************/

uint8_t Dasm6809::m6809_codes11[512] =
  {
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 00..03 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 04..07 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 08..0B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 0C..0F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 10..13 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 14..17 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 18..1B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 1C..1F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 20..23 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 24..27 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 28..2B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 2C..2F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 30..33 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 34..37 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 38..3B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _swi3 ,_imp,     /* 3C..3F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 40..43 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 44..47 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 48..4B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 4C..4F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 50..53 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 54..57 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 58..5B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 5C..5F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 60..63 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 64..67 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 68..6B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 6C..6F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 70..73 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 74..77 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 78..7B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 7C..7F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _cmpu ,_imw,     /* 80..83 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 84..87 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 88..8B */
  _cmps ,_imw,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 8C..8F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _cmpu ,_dir,     /* 90..93 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 94..97 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 98..9B */
  _cmps ,_dir,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 9C..9F */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _cmpu ,_ind,     /* A0..A3 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* A4..A7 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* A8..AB */
  _cmps ,_ind,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* AC..AF */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _cmpu ,_ext,     /* B0..B3 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* B4..B7 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* B8..BB */
  _cmps ,_ext,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* BC..BF */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* C0..C3 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* C4..C7 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* C8..CB */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* CC..CF */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* D0..D3 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* D4..D7 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* D8..DB */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* DC..DF */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* E0..E3 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* E4..E7 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* E8..EB */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* EC..EF */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* F0..F3 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* F4..F7 */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* F8..FB */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* FC..FF */
  };


static char *m6809_exg_tfr[] =
  {
  "D", "X", "Y", "U", "S", "PC","??","??",
  "A", "B", "CC","DP","??","??","??","??"
  };

char Dasm6809::reg[] = { 'X', 'Y', 'U', 'S' };

/*****************************************************************************/
/* opcodes : additional opcodes over 6800                                    */
/*****************************************************************************/

OpCode Dasm6809::opcodes[mnemo6809_count - mnemo6800_count] =
  {
    { "ABX",   Data },                  /* _abx                              */
    { "ADDD",  Data },                  /* _addd                             */
    { "ANDCC", Data },                  /* _andcc                            */
    { "BRN",   Code },                  /* _brn                              */
    { "CMPD",  Data },                  /* _cmpd                             */
    { "CMPS",  Data },                  /* _cmps                             */
    { "CMPU",  Data },                  /* _cmpu                             */
    { "CMPX",  Data },                  /* _cmpx                             */
    { "CMPY",  Data },                  /* _cmpy                             */
    { "CWAI",  Data },                  /* _cwai                             */
    { "EXG",   Data },                  /* _exg                              */
    { "LBCC",  Code },                  /* _lbcc                             */
    { "LBCS",  Code },                  /* _lbcs                             */
    { "LBEQ",  Code },                  /* _lbeq                             */
    { "LBGE",  Code },                  /* _lbge                             */
    { "LBGT",  Code },                  /* _lbgt                             */
    { "LBHI",  Code },                  /* _lbhi                             */
    { "LBLE",  Code },                  /* _lble                             */
    { "LBLS",  Code },                  /* _lbls                             */
    { "LBLT",  Code },                  /* _lblt                             */
    { "LBMI",  Code },                  /* _lbmi                             */
    { "LBNE",  Code },                  /* _lbne                             */
    { "LBPL",  Code },                  /* _lbpl                             */
    { "LBRA",  Code },                  /* _lbra                             */
    { "LBRN",  Code },                  /* _lbrn                             */
    { "LBSR",  Code },                  /* _lbsr                             */
    { "LBVC",  Code },                  /* _lbvc                             */
    { "LBVS",  Code },                  /* _lbvs                             */
    { "LDD",   Data },                  /* _ldd                              */
    { "LDU",   Data },                  /* _ldu                              */
    { "LDY",   Data },                  /* _ldy                              */
    { "LEAS",  Data },                  /* _leas                             */
    { "LEAU",  Data },                  /* _leau                             */
    { "LEAX",  Data },                  /* _leax                             */
    { "LEAY",  Data },                  /* _leay                             */
    { "MUL",   Data },                  /* _mul                              */
    { "ORCC",  Data },                  /* _orcc                             */
    { "PSHS",  Data },                  /* _pshs                             */
    { "PSHU",  Data },                  /* _pshu                             */
    { "PULS",  Data },                  /* _puls                             */
    { "PULU",  Data },                  /* _pulu                             */
    { "RESET", Data },                  /* _reset                            */
    { "SEX",   Data },                  /* _sex                              */
    { "STD",   Data },                  /* _std                              */
    { "STU",   Data },                  /* _stu                              */
    { "STY",   Data },                  /* _sty                              */
    { "SUBD",  Data },                  /* _subd                             */
    { "SWI2",  Data },                  /* _swi2                             */
    { "SWI3",  Data },                  /* _swi3                             */
    { "SYNC",  Data },                  /* _sync                             */
    { "TFR",   Data },                  /* _tfr                              */
    // Convenience mnemonics
    { "CLF",   Data },                  /* _clf                              */
    { "CLIF",  Data },                  /* _clif                             */
    { "CLZ",   Data },                  /* _clz                              */
    { "DEU",   Data },                  /* _deu                              */
    { "DEY",   Data },                  /* _dey                              */
    { "INU",   Data },                  /* _inu                              */
    { "INY",   Data },                  /* _iny                              */
    { "SEF",   Data },                  /* _sef                              */
    { "SEIF",  Data },                  /* _seif                             */
    { "SEZ",   Data },                  /* _sez                              */

  };

/*****************************************************************************/
/* Dasm6809 : constructor                                                    */
/*****************************************************************************/

Dasm6809::Dasm6809(void)
{
codes = m6809_codes;
codes10 = m6809_codes10;
codes11 = m6809_codes11;
exg_tfr = m6809_exg_tfr;
os9Patch = false;
useFlex = false;
mnemo.resize(mnemo6809_count);          /* set up additional mnemonics       */
for (int i = 0; i < mnemo6809_count - mnemo6800_count; i++)
  mnemo[mnemo6800_count + i] = opcodes[i];
mnemo[_lda].mne   = "LDA";              /* adjust slight mnemo differences   */
mnemo[_ldb].mne   = "LDB";
mnemo[_sta].mne   = "STA";
mnemo[_stb].mne   = "STB";
mnemo[_ora].mne   = "ORA";
mnemo[_orb].mne   = "ORB";

// set up options table
// class uses one generic option setter/getter pair (not mandatory)
AddOption("flex", "{off|on}\tuse FLEX9 standard labels",
          static_cast<PSetter>(&Dasm6809::Set6809Option),
          static_cast<PGetter>(&Dasm6809::Get6809Option));
AddOption("os9", "{off|on}\tpatch swi2 (os9 call)",
          static_cast<PSetter>(&Dasm6809::Set6809Option),
          static_cast<PGetter>(&Dasm6809::Get6809Option));
}

/*****************************************************************************/
/* ~Dasm6809 : destructor                                                    */
/*****************************************************************************/

Dasm6809::~Dasm6809(void)
{
}

/*****************************************************************************/
/* Set6809Option : sets a disassembler option                                */
/*****************************************************************************/

int Dasm6809::Set6809Option(string lname, string value)
{
bool bnValue = true;                    /* default to "on"                   */
bool bIsBool = ParseBool(value, bnValue);

if (lname == "flex")
  {
  useFlex = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (lname == "os9")
  {
  os9Patch = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (lname == "dplabel")
  {
  useDPLabels = bnValue;
  return bIsBool ? 1 : 0;
  }
else
  return 0;                             /* only name consumed                */

// should never happen ... but better safe than sorry
return 1;                               /* name and value consumed           */
}

/*****************************************************************************/
/* Get6809Option : retrieves a disassembler option                           */
/*****************************************************************************/

string Dasm6809::Get6809Option(string lname)
{
if (lname == "flex")         return useFlex ? "on" : "off";
else if (lname == "os9")     return os9Patch ? "on" : "off";
else if (lname == "dplabel") return useDPLabels ? "on" : "off";
return "";
}

/*****************************************************************************/
/* InfoHelp : print disassembler-specific info file help                     */
/*****************************************************************************/

string Dasm6809::InfoHelp()
{
return
#if RB_VARIANT
        "\nAddressing control\n"
        "\tset DP value:       SETDP [addr[-addr]] DP-content\n"
        "\t                    (without addr, last one is used)\n"
#else
        "SETDP [addr[-addr]] DP-content (without addr, last definition is used)\n"
#endif
        ;
}

/*****************************************************************************/
/* ProcessInfo : process an info file line                                   */
/*****************************************************************************/

bool Dasm6809::ProcessInfo
    (
    string key,                         /* parsed key                        */
    string value,                       /* rest of the line                  */
    addr_t &from,                       /* from/to (if range given)          */
    addr_t &to,
    addr_t &step,                       /* step size                         */
    vector<TMemoryArray<addr_t>> &remaps ,  /* remaps, if necessary          */
    bool bProcInfo,                     /* flag whether processing           */
    int bus,                            /* target bus for command            */
    int tgtbus                          /* target bus for parameters (ign.)  */
    )
{
if (!bProcInfo || bus != BusCode)       /* only if processing code bus...    */
  return false;

enum InfoCmd
  {
  infoUnknown = -1,                     /* unknown info command              */
  infoSetDP,                            /* SETDP [addr[-addr]] dp            */
  infoUnsetDP,                          /* UNSETDP [addr[-addr]]             */
  };
static struct                           /* structure to convert key to type  */
  {
  const char *name;
  InfoCmd cmdType;
  } sKey[] =
  {
  { "SETDP",        infoSetDP },
  { "UNSETDP",      infoSetDP },
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
  case infoSetDP :                      /* SETDP [addr[-addr]] dp            */
    {
    string srange;
    string::size_type idx = value.find_first_of(" \t");
    if (idx == value.npos) idx = value.size();
    srange = value.substr(0, idx);
    addr_t dp = NO_ADDRESS;
    value = (idx == value.size()) ? "" : trim(value.substr(idx));
    if (value.size() &&                 /* if addr[-addr] dp                 */
        value[0] != '*')
      String2Number(value, dp);
    if (dp == NO_ADDRESS &&             /* if only dp                        */
        from != NO_ADDRESS &&
        to == from)
      {
      dp = from;
      from = to = NO_ADDRESS;
      }
    if (dp != NO_ADDRESS)               /* unless it's OFF                   */
      {
      if (dp & 0xff)                    /* get dp into page range $xx00      */
        dp <<= 8;
      if (dp < GetLowestDataAddr() || dp > GetHighestDataAddr() - 15)
        return false;
      }
    if (from == NO_ADDRESS)
      dirpage = dp;
    else for (addr_t scanned = from;
              scanned >= from && scanned <= to;
              scanned++)
      SetDirectPage(scanned, dp, bus);
    }
    break;
  case infoUnsetDP :                    /* UNSETDP [addr[-addr]]             */
    if (from == NO_ADDRESS)
      dirpage = 0;
    else for (addr_t scanned = from;
              scanned >= from && scanned <= to;
              scanned++)
      SetDirectPage(scanned, 0, bus);
    break;
  }
return true;
}

/*****************************************************************************/
/* InitParse : initialize parsing                                            */
/*****************************************************************************/

bool Dasm6809::InitParse(int bus)
{
if (bus == BusCode)
  {
  if (useFlex)
    AddFlexLabels();

  if (bSetSysVec)
    {
    // set up SWI3-FIRQ system vectors (rest done by 6800)
    static const char *vectbl[] =
      {
      "SWI3",                           /* fff2                              */
      "SWI2",                           /* fff4                              */
      "FIRQ",                           /* fff6                              */
      };
    for (addr_t addr = 0xfff2; addr < 0xfff8; addr += 2)
      {
      MemoryType memType = GetMemType(addr);
      if (memType != Untyped &&         /* if system vector loaded           */
          memType != Const &&           /* and not defined as constant       */
          !FindLabel(addr))             /* and no label set in info file     */
        {
        SetMemType(addr, Data);         /* that's a data word                */
        SetCellSize(addr, 2);
                                        /* look whether it points to loaded  */
        addr_t tgtaddr = GetUWord(addr);
        if (GetMemType(tgtaddr) != Untyped)
          {                             /* if so,                            */
          SetMemType(tgtaddr, Code);    /* that's code there                 */
          AddLabel(tgtaddr, Code,       /* and it got a predefined label     */
                   sformat("vec_%s", vectbl[(addr - 0xfff2) / 2]),
                   true);
          }
        }
      }
    }

  }
return Dasm6800::InitParse(bus);
}

/*****************************************************************************/
/* FetchInstructionDetails : fetch details about current instruction         */
/*****************************************************************************/

addr_t Dasm6809::FetchInstructionDetails
    (
    addr_t PC,
    uint8_t &O,
    uint8_t &T,
    uint8_t &M,
    uint16_t &W,
    int &MI,
    const char *&I,
    string *smnemo
    )
{
O = T = GetUByte(PC++);
if (T == 0x10)
  {
  T = GetUByte(PC++);
  W = T * 2;
  MI = T = codes10[W++];
  I = mnemo[T].mne;
  M = codes10[W];
  }
else if (T == 0x11)
  {
  T = GetUByte(PC++);
  W = T * 2;
  MI = T = codes11[W++];
  I = mnemo[T].mne;
  M = codes11[W];
  }
else
  {
  W = T * 2;
  MI = T = codes[W++];
  M = codes[W];
  I = mnemo[T].mne;
  }

if (smnemo)
  *smnemo = I;
return PC;
}

/*****************************************************************************/
/* IndexParse : parses index for labels                                      */
/*****************************************************************************/

addr_t Dasm6809::IndexParse(int MI, addr_t pc)
{
uint8_t T;
uint16_t W;
uint16_t Wrel;
char R;
addr_t PC = pc;
bool bSetLabel = true;
Label *lbl;

T = GetUByte(PC++);
R = reg[(T >> 5) & 0x03];

if (T & 0x80)
  {
  switch(T & 0x1F)
    {
    case 0x00:                          /* register operations               */
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x0B:
    case 0x11:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x1B:
      break;
    case 0x0C:
    case 0x1C:
      bSetLabel = !IsConst(PC);
      if (!bSetLabel)
        SetDefLabelUsed(PC);
      T = GetUByte(PC++);
      if (bSetLabel)
        AddLabel((uint16_t)((int)((char)T) + PC), mnemo[MI].memType, "", true);
      break;
    case 0x08:
      bSetLabel = !IsConst(PC);
      T = GetUByte(PC);
      SetCellType(PC, MemAttribute::SignedInt);
      Wrel = (uint16_t)GetRelative(PC);
      if (!bSetLabel && !Wrel)
        SetDefLabelUsed(PC);
      W = (uint16_t)((int)((char)T));
      Wrel += W;
      PC++;
      // no labels in indirect addressing!
      // ...except when they are explicitly given in the info file, of course
      if (bSetLabel &&
          (W != Wrel ||                 /* if it's relative, or label's there*/
            FindLabel(Wrel)))
        AddLabel(Wrel,                  /* mark it as used                   */
                 mnemo[MI].memType, "", true);
      break;
    case 0x18:
      SetDefLabelUsed(PC);
      Wrel = (uint16_t)GetRelative(PC);
      PC++;
      break;
    case 0x0D:
      bSetLabel = !IsConst(PC);
      SetDefLabelUsed(PC);
      SetCellSize(PC, 2);
      W = GetUWord(PC); PC += 2;
      if (bSetLabel)
        AddLabel((uint16_t)(W + PC), mnemo[MI].memType, "", true);
      break;
    case 0x09:
      bSetLabel = !IsConst(PC);
      Wrel = (uint16_t)GetRelative(PC);
      if (!bSetLabel && !Wrel)
        SetDefLabelUsed(PC);
      SetCellSize(PC, 2);
      W = GetUWord(PC);
      Wrel += W;
      PC += 2;
      // no labels in indirect addressing!
      // ...except when they are explicitly given in the info file, of course
      if (bSetLabel &&
          (W != Wrel ||                 /* if it's relative, or label's there*/
            FindLabel(Wrel)))
        AddLabel(Wrel,                  /* mark it as used                   */
                 mnemo[MI].memType, "", true);
      break;
    case 0x19:
    case 0x1D:
      bSetLabel = !IsConst(PC);
      if (!bSetLabel)
        SetDefLabelUsed(PC);
      SetCellSize(PC, 2);
      W = GetUWord(PC); PC += 2;
      if (bSetLabel)
        AddLabel((uint16_t)(W + PC), mnemo[MI].memType, "", true);
      break;

    default:
      if (T == 0x9F)
        {
        bSetLabel = !IsConst(PC);
        lbl = bSetLabel ? NULL : FindLabel(PC, Const);
        if (lbl)
          lbl->SetUsed();
        SetCellSize(PC, 2);
        W = GetUWord(PC); PC += 2;
        if (bSetLabel)
          AddLabel(W, mnemo[MI].memType, "", true);
        }
      break;
    }
  }
else
  {
  char c = T & 0x1F;
  if (c & 0x10)
    c |= 0xf0;
  Wrel = (uint16_t)GetRelative(PC - 1);
  if (Wrel)
    {
    bSetLabel = !IsConst(PC - 1);
    lbl = bSetLabel ? NULL : FindLabel(PC - 1, Const);
    W = (uint16_t)(c + Wrel);
    if (bSetLabel)
      AddLabel(W, mnemo[MI].memType, "", true);
    }
  else
    lbl = FindLabel(PC - 1, Const);
  if (lbl)
    lbl->SetUsed();
  }
return PC;
}

/*****************************************************************************/
/* IndexString : converts index to string                                    */
/*****************************************************************************/

string Dasm6809::IndexString(addr_t &pc)
{
uint8_t T;
uint16_t W, Wrel;
char R;
string buf;
addr_t PC = pc;
bool bGetLabel;
Label *lbl;

T = GetUByte(PC++);
R = reg[(T >> 5) & 0x03];

if (T & 0x80)
  {
  switch (T & 0x1F)
    {
    case 0x00:
      buf = sformat(",%c+", R);
      break;
    case 0x01:
      buf = sformat(",%c++", R);
      break;
    case 0x02:
      buf = sformat(",-%c", R);
      break;
    case 0x03:
      buf = sformat(",--%c", R);
      break;
    case 0x04:
      buf = sformat(",%c", R);
      if (GetRelative(PC - 1))
        {
        bGetLabel = !IsConst(PC - 1);
        buf += sformat("   %s (%s)",
                       commentStart.c_str(),
                       Label2String(0, 4, bGetLabel, PC - 1).c_str());
        }
      break;
    case 0x05:
      buf = sformat("B,%c", R);
      break;
    case 0x06:
      buf = sformat("A,%c", R);
      break;
    case 0x08:
      bGetLabel = !IsConst(PC);
      T = GetUByte(PC);
      W = (uint16_t)((int)((char)T));
      Wrel = W + (uint16_t)GetRelative(PC);
      // look whether there's a const label directly for the offset
      lbl = (bGetLabel || Wrel != W) ? NULL : FindLabel(PC, Const);
      if (bGetLabel && (Wrel != W || FindLabel(Wrel)))
        {
        string slbl = lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
        bool bFwd = IsForwardRef(W, bGetLabel, PC);
        buf = sformat(bFwd ? "<%s,%c" : "%s,%c", slbl.c_str(), R);
        }
      else
        {
        string slbl = lbl ? lbl->GetText() : SignedNumber2String((int)((char)T), 2, PC);
        // DefLabels are output before the code, so they ARE defined and need no <
        // buf = sformat((lbl && Wrel >= PC - 1) ? "<%s,%c" : "%s,%c", slbl.c_str(), R);
        buf = sformat((lbl && Wrel >= PC - 1) ? "%s,%c" : "%s,%c", slbl.c_str(), R);
        }
      PC++;
      break;
    case 0x09:
      bGetLabel = !IsConst(PC);
      W = GetUWord(PC);
      Wrel = W + (uint16_t)GetRelative(PC);
      lbl = (bGetLabel || Wrel) ? NULL : FindLabel(PC, Const);
      if ((Wrel != W) || FindLabel(Wrel))
        {
        string slbl = lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
        buf = sformat("%s,%c", slbl.c_str(), R);
        if (((W < 0x80) || (W >= 0xff80)) && forceExtendedAddr)
          buf = ">" + buf;
        }
      else
        {
        string slbl;
        if (((W < 0x80) || (W >= 0xff80)) && forceExtendedAddr)
          {
          slbl = lbl ? lbl->GetText() : SignedNumber2String((int)(short)W, 4, PC);
          buf = sformat(">%s,%c", slbl.c_str(), R);
          }
        else
          {
          slbl = lbl ? lbl->GetText() : Number2String((uint16_t)(int)(short)W, 4, PC);  /* RB: this was "signed_string" */
          buf = sformat("%s,%c", slbl.c_str(), R);
          }
        }
      PC += 2;
      break;
    case 0x0B:
      buf = sformat("D,%c", R);
      break;
    case 0x0C:
      T = GetUByte(PC);
      bGetLabel = !IsConst(PC);
#if 0
      sprintf(buf,"$%s,PC",signed_string((int)(char)T, 2, (word)PC));
#else
      if (bGetLabel)
        buf = Label2String((uint16_t)((int)((char)T) + PC + 1), 4,
                           bGetLabel, PC) + ",PCR";
      else
        {
        lbl = FindLabel(PC, Const);
        string slbl = lbl ? lbl->GetText() : Number2String((uint16_t)(int)(char)T, 2, PC);
        buf = slbl + ",PC";
        }
      if (((char)T > 0) && forceDirectAddr)
        buf = "<" + buf;
#endif
      PC++;
      break;
    case 0x0D:
      {
      bGetLabel = !IsConst(PC);
      lbl = bGetLabel ? NULL : FindLabel(PC, Const);
      W = GetUWord(PC);
      PC += 2;
      string slbl = lbl ? lbl->GetText() :
                          Label2String((uint16_t)(W + PC), 4, bGetLabel, PC - 2);
      if (((W < 0x80) || (W >= 0xff80)) && forceExtendedAddr)
        buf = sformat(">%s,PCR", slbl.c_str());
      else
        buf = sformat("%s,PCR", slbl.c_str());
      }
      break;
    case 0x11:
      buf = sformat("[,%c++]", R);
      break;
    case 0x13:
      buf = sformat("[,--%c]", R);
      break;
    case 0x14:
      buf = sformat("[,%c]", R);
      break;
    case 0x15:
      buf = sformat("[B,%c]", R);
      break;
    case 0x16:
      buf = sformat("[A,%c]", R);
      break;
    case 0x18:
      {
      lbl = FindLabel(PC, Const);
      T = GetUByte(PC);
      string slbl = lbl ? lbl->GetText() : Number2String(T, 2, PC);
      buf = sformat("[%s,%c]",
              slbl.c_str(),
              R);
      PC++;
      }
      break;
    case 0x19:
      {
      bGetLabel = !IsConst(PC);
      lbl = bGetLabel ? NULL : FindLabel(PC, Const);
      W = GetUWord(PC);
      string slbl = lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
      buf = sformat("[%s,%c]", slbl.c_str(), R);
      PC += 2;
      }
      break;
    case 0x1B:
      buf = sformat("[D,%c]", R);
      break;
    case 0x1C:
      {
#if 1
      T = GetUByte(PC);
      bGetLabel = !IsConst(PC);
      if (bGetLabel)
        buf = Label2String((uint16_t)((int)((char)T) + PC + 1), 4,
                           bGetLabel, PC) + ",PCR";
      else
        {
        lbl = FindLabel(PC, Const);
        string slbl = lbl ? lbl->GetText() : Number2String((uint16_t)(int)(char)T, 2, PC);
        buf = slbl + ",PC";
        }
      if (((char)T > 0) && forceDirectAddr)
        buf = "<" + buf;
      buf = "[" + buf + "]";
      PC++;
#else
      lbl = FindLabel(PC, Const);
      T = GetUByte(PC);
      string slbl = lbl ? lbl->GetText() : Number2String(T, 2, PC);
      buf = sformat("[%s,PC]", slbl.c_str());
      PC++;
#endif
      }
      break;
    case 0x1D:
      {
#if 1
      bGetLabel = !IsConst(PC);
      lbl = bGetLabel ? NULL : FindLabel(PC, Const);
      W = GetUWord(PC);
      PC += 2;
      string slbl = lbl ? lbl->GetText() :
                          Label2String((uint16_t)(W + PC), 4, bGetLabel, PC - 2);
      if (((W < 0x80) || (W >= 0xff80)) && forceExtendedAddr)
        buf = sformat("[>%s,PCR]", slbl.c_str());
      else
        buf = sformat("[%s,PCR]", slbl.c_str());
#else
      bGetLabel = !IsConst(PC);
      lbl = bGetLabel ? NULL : FindLabel(PC, Const);
      W = GetUWord(PC);
      string slbl = lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
      buf = sformat("[%s,PC]", slbl.c_str());
      PC += 2;
#endif
      }
      break;
    case 0x07:
    case 0x17:
    case 0x0A:
    case 0x1A:
    case 0x0E:
    case 0x1E:
      buf = "???";
      break;
             
    default:
      if (T == 0x9F)
        {
        bGetLabel = !IsConst(PC);
        lbl = bGetLabel ? NULL : FindLabel(PC, Const);
        W = GetUWord(PC);
        string slbl = lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
        buf = sformat("[%s]", slbl.c_str());
        PC += 2;
        }
      else
        buf = "???";
      break;
    }
    
  }
else
  {
  // lowest 5 bits are a signed offset in range [-16..+15]
  char c = T & 0x1F;
  if (c & 0x10)
    c |= 0xf0;
  bGetLabel = !IsConst(PC - 1);
  Wrel = (uint16_t)GetRelative(PC - 1);
  if (Wrel)
    {
    lbl = bGetLabel ? NULL : FindLabel(PC - 1, Const);
    string slbl = lbl ? lbl->GetText() : Label2String((uint16_t)c, 4, bGetLabel, PC - 1);
    buf = sformat("%s,%c", slbl.c_str(), R);
    }
  else
    {
    lbl = FindLabel(PC - 1, Const);
    string slbl = lbl ? lbl->GetText() : SignedNumber2String(c, 2, PC - 1);
    buf = sformat("%s,%c", slbl.c_str(), R);
    }
  }

pc = PC;
return buf;
}

/*****************************************************************************/
/* ParseCode : parse instruction at given memory address for labels          */
/*****************************************************************************/

addr_t Dasm6809::ParseCode
    (
    addr_t addr,
    int bus                             /* ignored for 6800 and derivates    */
    )
{
uint8_t O, T, M;
uint16_t W;
int MI;
const char *I;
bool bSetLabel;
Label *lbl;
addr_t PC = FetchInstructionDetails(addr, O, T, M, W, MI, I);

if ((T == _swi2) && os9Patch)
  return (PC + 1 - addr);

switch (M)                              /* which mode is this ?              */
  {
  case _ind:                            /* indexed                           */
    PC = IndexParse(MI,PC);
    break;
    
  case _rew:                            /* relative word                     */
    bSetLabel = !IsConst(PC);
    lbl = bSetLabel ? NULL : FindLabel(PC, Const, bus);
    if (lbl)
      lbl->SetUsed();
    SetCellSize(PC, 2);
    W = GetUWord(PC); PC += 2;
    W += (uint16_t)PC;
    W = (uint16_t)DephaseOuter(W, PC - 2);
    if (bSetLabel)
      AddLabel(W, mnemo[MI].memType, "", true);
    break;
    
  case _r1:                             /* tfr/exg mode                      */
    T = GetUByte(PC); PC++;
    break;

  case _r2:                             /* pul/psh system                    */
  case _r3:                             /* pul/psh user                      */
    T = GetUByte(PC); PC++;
    break;

  default :                             /* anything else is handled by base  */
    return Dasm6800::ParseCode(addr, bus);
  }
return PC - addr;                       /* pass back # processed bytes       */
}

/*****************************************************************************/
/* DisassembleCode : disassemble code instruction at given memory address    */
/*****************************************************************************/

addr_t Dasm6809::DisassembleCode
    (
    addr_t addr,
    string &smnemo,
    string &sparm,
    int bus                             /* ignored for 6800 and derivates    */
    )
{
uint8_t O, T, M;
uint16_t W;
int MI;
const char *I;
addr_t PC = addr;
bool bGetLabel;
Label *lbl;

PC = FetchInstructionDetails(PC, O, T, M, W, MI, I, &smnemo);
if ((T == _swi2) && os9Patch)
  {
  T = GetUByte(PC++);
  smnemo = "OS9";
  sparm = os9_codes[T];
  return PC - addr;
  }

switch (M)                              /* which mode is this?               */
  {
  case _imb:                            /* immediate byte                    */
    T = GetUByte(PC);
    if (useConvenience)
      W = (uint16_t)(O << 8) | T;
    else
      W = 0;
    switch (W)                          /* examine for special CC settings   */
      {
      case 0x1a01 :                     /* (6809) ORCC $01                   */
        smnemo = mnemo[_sec].mne;
        break;
      case 0x1a02 :                     /* (6809) ORCC $02                   */
        smnemo = mnemo[_sev].mne;
        break;
      case 0x1a04 :                     /* (6809) ORCC $04                   */
        smnemo = mnemo[_sez].mne;
        break;
      case 0x1a10 :                     /* (6809) ORCC $10                   */
        smnemo = mnemo[_sei].mne;
        break;
      case 0x1a40 :                     /* (6809) ORCC $40                   */
        smnemo = mnemo[_sef].mne;
        break;
      case 0x1a50 :                     /* (6809) ORCC $40+$10               */
        smnemo = mnemo[_seif].mne;
        break;
      case 0x1cfe :                     /* (6809) ANDCC ~$01                 */
        smnemo = mnemo[_clc].mne;
        break;
      case 0x1cfd :                     /* (6809) ANDCC ~$02                 */
        smnemo =  mnemo[_clv].mne;
        break;
      case 0x1cfb :                     /* (6809) ANDCC ~$04                 */
        smnemo = mnemo[_clz].mne;
        break;
      case 0x1cef :                     /* (6809) ANDCC ~$10                 */
        smnemo = mnemo[_cli].mne;
        break;
      case 0x1cbf :                     /* (6809) ANDCC ~$40                 */
        smnemo = mnemo[_clf].mne;
        break;
      case 0x1caf :                     /* (6809) ANDCC ~($40 + $10)         */
        smnemo = mnemo[_clif].mne;
        break;
      case 0x3cff :                     /* (6809) CWAI $FF                   */
        smnemo = mnemo[_wai].mne;
        break;
      default :
        lbl = FindLabel(PC, Const, bus);
        sparm = "#" + (lbl ? lbl->GetText() : Number2String(T, 2, PC));
      }
    PC++;
    break;

  case _ind:                            /* indexed                           */
    if (useConvenience)
      W = (uint16_t)(O << 8) | GetUByte(PC);
    else
      W = 0;
    switch (W)
      {
      case 0x3001 :                     /* (6809) LEAX +1                    */
        smnemo = mnemo[_inx].mne; PC++;
        break;
      case 0x301f :                     /* (6809) LEAX -1                    */
        smnemo = mnemo[_dex].mne; PC++;
        break;
      case 0x3121 :                     /* (6809) LEAY +1                    */
        smnemo = mnemo[_iny].mne; PC++;
        break;
      case 0x313f :                     /* (6809) LEAY -1                    */
        smnemo = mnemo[_dey].mne; PC++;
        break;
      case 0x3261 :                     /* (6809) LEAS +1                    */
        smnemo = mnemo[_ins].mne; PC++;
        break;
      case 0x327f :                     /* (6809) LEAS -1                    */
        smnemo = mnemo[_des].mne; PC++;
        break;
      case 0x3341 :                     /* (6809) LEAU +1                    */
        smnemo = mnemo[_inu].mne; PC++;
        break;
      case 0x335f :                     /* (6809) LEAU -1                    */
        smnemo = mnemo[_deu].mne; PC++;
        break;
      default :
        sparm = IndexString(PC);
        break;
      }
    break;
    
  case _rew:                            /* relative word                     */
    bGetLabel = !IsConst(PC);
    lbl = bGetLabel ? NULL : FindLabel(PC, Const, bus);
    W = GetUWord(PC);
    PC += 2;
    W += (uint16_t)PC;
    W = (uint16_t)DephaseOuter(W, PC - 2);
    sparm = lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC - 2);
    break;
    
  case _r1:                             /* tfr/exg mode                      */
    if (useConvenience)
      W = (uint16_t)(O << 8) | GetUByte(PC);
    else
      W = 0;
    switch (W)
      {
      case 0x1f14 :                     /* (6809) TFR X,S                    */
        smnemo = mnemo[_txs].mne; PC++;
        break;
      case 0x1f41 :                     /* (6809) TFR S,X                    */
        smnemo = mnemo[_tsx].mne; PC++;
        break;
/* hairy - some assemblers expand TAB to TAB + TSTA...
   but there's no guarantee.
      case 0x1f89 :
        smnemo = "TAB"; PC++;
        break; */
      case 0x1f8a :                     /* (6809) TFR A,CC                   */
        smnemo = mnemo[_tap].mne; PC++;
        break;
/* hairy - some assemblers expand TBA to TBA + TSTA...
   but there's no guarantee.
      case 0x1f98 :
        smnemo = "TBA"; PC++;
        break; */
      case 0x1fa8 :                     /* (6809) TFR CC,A                   */
        smnemo = mnemo[_tpa].mne; PC++;
        break;
      default :
        T = GetUByte(PC++);
        sparm = sformat("%s,%s", exg_tfr[T >> 4], exg_tfr[T & 0xF]);
        break;
      }
    break;

  case _r2:                             /* pul/psh system                    */
  case _r3:                             /* pul/psh user                      */
    if (useConvenience)
      W = (uint16_t)(O << 8) | GetUByte(PC);
    else
      W = 0;
    switch (W)
      {
      case 0x3404 :                     /* (6809) PSHS B                     */
        if (FindLabel(PC + 1))
          W = 0;
        else
          W = GetUWord(PC + 1);
        switch (W)
          {
          case 0xa0e0 :                 /* (6809) PSHS B / SUBA ,S++         */
            smnemo = mnemo[_sba].mne; PC += 3;
            break;
          case 0xa1e0 :                 /* (6809) PSHS B / CMPA ,S++         */
            smnemo = mnemo[_cba].mne; PC += 3;
            break;
          case 0xabe0 :                 /* (6809) PSHS B / ADDA ,S++         */
            smnemo = mnemo[_aba].mne; PC += 3;
            break;
          default:                      /* (6809) PSHS B / anything else     */
            smnemo = mnemo[_pshb].mne; PC++;
            break;
          }
        break;
      case 0x3402 :                     /* (6809) PSHS A                     */
        smnemo = mnemo[_psha].mne; PC++;
        break;
      case 0x3406 :                     /* (6809) PSHS D                     */
        smnemo = "PSHD"; PC++;
        break;
      case 0x3410 :                     /* (6809) PSHS X                     */
        smnemo = "PSHX"; PC++;
        break;
      case 0x3420 :                     /* (6809) PSHS Y                     */
        smnemo = "PSHY"; PC++;
        break;
      case 0x3502 :                     /* (6809) PULS A                     */
        smnemo = mnemo[_pula].mne; PC++;
        break;
      case 0x3504 :                     /* (6809) PULS B                     */
        smnemo = mnemo[_pulb].mne; PC++;
        break;
      case 0x3506 :                     /* (6809) PULS D                     */
        smnemo = "PULD"; PC++;
        break;
      case 0x3510 :                     /* (6809) PULS X                     */
        smnemo = "PULX"; PC++;
        break;
      case 0x3520 :                     /* (6809) PULS Y                     */
        smnemo = "PULY"; PC++;
        break;
      default:
        // buf[0] = '\0';
        T = GetUByte(PC++);
        if (T & 0x80)
          sparm += "PC,";
        if (T & 0x40)
          {
          if (M == _r2)
            sparm += "U,";
          if (M == _r3)
            sparm += "S,";
          }
        if (T&0x20)
          sparm += "Y,";
        if (T & 0x10)
          sparm += "X,";
        if (T & 0x08)
          sparm += "DP,";
        if ((T & 0x06) == 0x06)
          sparm += "D,";
        else
          {
          if (T & 0x04)
            sparm += "B,";
          if (T & 0x02)
            sparm += "A,";
          }
        if (T & 0x01)
          sparm += "CC,";
        if (sparm.size())
          sparm = sparm.substr(0, sparm.size() - 1);
      break;
      }
    break;

  default :                             /* anything else is handled by base  */
    return Dasm6800::DisassembleCode(addr, smnemo, sparm, bus);
  }
return PC - addr;                       /* pass back # processed bytes       */
}

/*****************************************************************************/
/* DisassembleChanges : report dasm-specific state changes before/after addr */
/*****************************************************************************/

bool Dasm6809::DisassembleChanges
    (
    addr_t addr,
    addr_t prevaddr,
    addr_t prevsz,
    bool bAfterLine,
    vector<LineChange> &changes,
    int bus
    )
{
// init / exit
if (addr == NO_ADDRESS && prevaddr == NO_ADDRESS)
  {
  // no specialties here
  return Dasm6800::DisassembleChanges(addr, prevaddr, prevsz, bAfterLine, changes, bus);
  }
if (!bAfterLine)                        /* if before the address             */
  {
  // report direct page changes
  addr_t dpold =
      (prevaddr == NO_ADDRESS) ? DEFAULT_ADDRESS : GetDirectPage(prevaddr, bus);
  addr_t dp = GetDirectPage(addr, bus);
  if (!dp && dpold == DEFAULT_ADDRESS) dpold = dp;
  if (dp != dpold)
    {
    LineChange chg;
    changes.push_back(chg);
    chg.oper = "SETDP";
    if (dp != NO_ADDRESS)
      chg.opnds = sformat("$%02X", dp >> 8);
    changes.push_back(chg);
    }
  }
return Dasm6800::DisassembleChanges(addr, prevaddr, prevsz, bAfterLine, changes, bus);
}

/*****************************************************************************/
/* AddFlexLabels : adds FLEX9 labels to disassembler                         */
/*****************************************************************************/

void Dasm6809::AddFlexLabels()
{
static struct
  {
  addr_t from;
  addr_t to;
  char const *txt;
  } FlexLbls[] =
  {
                                        /* FLEX9 DOS entries                 */
    { 0xCD00, 0xCD00, "COLDS" },
    { 0xCD03, 0xCD03, "WARMS" },
    { 0xCD06, 0xCD06, "RENTER" },
    { 0xCD09, 0xCD09, "INCH" },
    { 0xCD0C, 0xCD0C, "INCH2" },
    { 0xCD0F, 0xCD0F, "OUTCH" },
    { 0xCD12, 0xCD12, "OUTCH2" },
    { 0xCD15, 0xCD15, "GETCHR" },
    { 0xCD18, 0xCD18, "PUTCHR" },
    { 0xCD1B, 0xCD1B, "INBUFF" },
    { 0xCD1E, 0xCD1E, "PSTRNG" },
    { 0xCD21, 0xCD21, "CLASS" },
    { 0xCD24, 0xCD24, "PCRLF" },
    { 0xCD27, 0xCD27, "NXTCH" },
    { 0xCD2A, 0xCD2A, "RSTRIO" },
    { 0xCD2D, 0xCD2D, "GETFIL" },
    { 0xCD30, 0xCD30, "LOAD" },
    { 0xCD33, 0xCD33, "SETEXT" },
    { 0xCD36, 0xCD36, "ADDBX" },
    { 0xCD39, 0xCD39, "OUTDEC" },
    { 0xCD3C, 0xCD3C, "OUTHEX" },
    { 0xCD3F, 0xCD3F, "RPTERR" },
    { 0xCD42, 0xCD42, "GETHEX" },
    { 0xCD45, 0xCD45, "OUTADR" },
    { 0xCD48, 0xCD48, "INDEC" },
    { 0xCD4B, 0xCD4B, "DOCMND" },
    { 0xCD4E, 0xCD4E, "STAT" },
                                        /* FLEX FMS entries:                 */
    { 0xD400, 0xD400, "FMSINI" },       /* FMS init                          */
    { 0xD403, 0xD403, "FMSCLS" },       /* FMS close                         */
    { 0xD406, 0xD406, "FMS" },
    { 0xC840, 0xC97F, "FCB" },          /* standard system FCB               */
                                        /* miscellaneous:                    */
    { 0xD435, 0xD435, "VFYFLG" },       /* FMS verify flag                   */
    { 0xC080, 0xC0FF, "LINBUF" },       /* line buffer                       */
    { 0xCC00, 0xCC00, "TTYBS" },        /* TTYSET backspace character        */
    { 0xCC01, 0xCC01, "TTYDEL" },       /* TTYSET delete character           */
    { 0xCC02, 0xCC02, "TTYEOL" },       /* TTYSET EOL character              */
    { 0xCC03, 0xCC03, "TTYDPT" },       /* TTYSET depth count                */
    { 0xCC04, 0xCC04, "TTYWDT" },       /* TTYSET width count                */
    { 0xCC05, 0xCC05, "TTYNUL" },       /* TTYSETnull count                  */
    { 0xCC06, 0xCC06, "TTYTAB" },       /* TTYSET tab character              */
    { 0xCC07, 0xCC07, "TTYBSE" },       /* TTYSET backspace echo character   */
    { 0xCC08, 0xCC08, "TTYEJ" },        /* TTYSET eject count                */
    { 0xCC09, 0xCC09, "TTYPAU" },       /* TTYSET pause control              */
    { 0xCC0A, 0xCC0A, "TTYESC" },       /* TTYSET escape character           */
    { 0xCC0B, 0xCC0B, "SYSDRV" },       /* current system drive              */
    { 0xCC0C, 0xCC0C, "WRKDRV" },       /* current working drive             */
    { 0xCC0E, 0xCC10, "SYSDAT" },       /* System date month/day/year        */
    { 0xCC11, 0xCC11, "TTYTRM" },
    { 0xCC12, 0xCC12, "COMTBL" },       /* user command table                */
    { 0xCC14, 0xCC14, "LINBFP" },       /* line buffer pointer               */
    { 0xCC16, 0xCC16, "ESCRET" },       /* escape return register            */
    { 0xCC18, 0xCC18, "LINCHR" },       /* current char in linebuffer        */
    { 0xCC19, 0xCC19, "LINPCH" },       /* previous char in linebuffer       */
    { 0xCC1A, 0xCC1A, "LINENR" },       /* line nr of current page           */
    { 0xCC1B, 0xCC1B, "LODOFS" },       /* loader address offset             */
    { 0xCC1D, 0xCC1D, "TFRFLG" },       /* loader  transfer flag             */
    { 0xCC1E, 0xCC1E, "TFRADR" },       /* transfer address                  */
    { 0xCC20, 0xCC20, "FMSERR" },       /* FMS error type                    */
    { 0xCC21, 0xCC21, "IOFLG" },        /* special I/O flag                  */
    { 0xCC22, 0xCC22, "OUTSWT" },       /* output switch                     */
    { 0xCC23, 0xCC23, "INSWT" },        /* input switch                      */
    { 0xCC24, 0xCC24, "FOPADR" },       /* file output address               */
    { 0xCC26, 0xCC26, "FIPADR" },       /* file input address                */
    { 0xCC28, 0xCC28, "COMFLG" },       /* command flag                      */
    { 0xCC29, 0xCC29, "OUTCOL" },       /* current output column             */
    { 0xCC2A, 0xCC2A, "SCRATC" },       /* system scratch                    */
    { 0xCC2B, 0xCC2B, "MEMEND" },       /* memory end                        */
    { 0xCC2D, 0xCC2D, "ERRVEC" },       /* error name vector                 */
    { 0xCC2F, 0xCC2F, "INECHO" },       /* file input echo flag              */
                                        /* Printer support                   */
    { 0xCCC0, 0xCCC0, "PRTINI" },       /* printer initialize                */
    { 0xCCD8, 0xCCD8, "PRTCHK" },       /* printer check                     */
    { 0xCCE4, 0xCCE4, "PRTOUT" },       /* printer output                    */
                                        /* Console I/O Driver Table :        */
    { 0xd3e5, 0xd3e5, "CINCHNE" },      /* input character w/o echo          */
    { 0xd3e7, 0xd3e7, "CIHNDLR" },      /* IRQ interrupt handler             */
    { 0xd3e9, 0xd3e9, "CSWIVEC" },      /* SWI3 vector location              */
    { 0xd3eb, 0xd3eb, "CIRQVEC" },      /* IRQ vector location               */
    { 0xd3ed, 0xd3ed, "CTMOFF" },       /* timer off routine                 */
    { 0xd3ef, 0xd3ef, "CTMON" },        /* timer on routine                  */
    { 0xd3f1, 0xd3f1, "CTMINT" },       /* timer initialization              */
    { 0xd3f3, 0xd3f3, "CMONITR" },      /* monitor entry address             */
    { 0xd3f5, 0xd3f5, "CTINIT" },       /* terminal initialization           */
    { 0xd3f7, 0xd3f7, "CSTAT" },        /* check terminal status             */
    { 0xd3f9, 0xd3f9, "COUTCH" },       /* output character                  */
    { 0xd3fb, 0xd3fb, "CINCH" },        /* input character w/ echo           */
                                        /* Disk Driver Jump Table :          */
    { 0xde00, 0xde00, "DDJ_READ" },     /* read a single sector              */
    { 0xde03, 0xde03, "DDJ_WRITE" },    /* write a single sector             */
    { 0xde06, 0xde06, "DDJ_VERIFY" },   /* verify last sector written        */
    { 0xde09, 0xde09, "DDJ_RESTORE" },  /* restore head to track #0          */
    { 0xde0c, 0xde0c, "DDJ_DRIVE" },    /* select the specified drive        */
    { 0xde0f, 0xde0f, "DDJ_CHKRDY" },   /* Check for drive ready             */
    { 0xde12, 0xde12, "DDJ_QUICK" },    /* Quick check for drive ready       */
    { 0xde15, 0xde15, "DDJ_INIT" },     /* driver initialize (cold start)    */
    { 0xde18, 0xde18, "DDJ_WARM" },     /* driver initialize (warm start)    */
    { 0xde1b, 0xde1b, "DDJ_SEEK" },     /* seek to specified track           */
  };

for (int i = 0; i < _countof(FlexLbls); i++)
  {
  for (addr_t a = FlexLbls[i].from; a <= FlexLbls[i].to; a++)
    {
    string s(FlexLbls[i].txt);
    if (a != FlexLbls[i].from)
      s += sformat("+%s", Number2String(a - FlexLbls[i].from, 4, a).c_str());
    AddLabel(a, Untyped, s);
    }
  }
}
