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

static Disassembler *Create6809(Application *pApp)
{
Disassembler *pDasm = new Dasm6809(pApp);
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

const char *Dasm6809::os9_codes[0x100] =
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

CMatrixEntry Dasm6809::m6809_codes[256] =
  {
  {_neg  ,_dir}, {_ill  ,_nom}, {_ill  ,_nom}, {_com  ,_dir},   /* 00..03 */
  {_lsr  ,_dir}, {_ill  ,_nom}, {_ror  ,_dir}, {_asr  ,_dir},   /* 04..07 */
  {_asl  ,_dir}, {_rol  ,_dir}, {_dec  ,_dir}, {_ill  ,_nom},   /* 08..0B */
  {_inc  ,_dir}, {_tst  ,_dir}, {_jmp  ,_dir}, {_clr  ,_dir},   /* 0C..0F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_nop  ,_imp}, {_sync ,_imp},   /* 10..13 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_lbra ,_rew}, {_lbsr ,_rew},   /* 14..17 */
  {_ill  ,_nom}, {_daa  ,_imp}, {_orcc ,_imb}, {_ill  ,_nom},   /* 18..1B */
  {_andcc,_imb}, {_sex  ,_imp}, {_exg  ,_r1 }, {_tfr  ,_r1 },   /* 1C..1F */
  {_bra  ,_reb}, {_brn  ,_reb}, {_bhi  ,_reb}, {_bls  ,_reb},   /* 20..23 */
  {_bcc  ,_reb}, {_bcs  ,_reb}, {_bne  ,_reb}, {_beq  ,_reb},   /* 24..27 */
  {_bvc  ,_reb}, {_bvs  ,_reb}, {_bpl  ,_reb}, {_bmi  ,_reb},   /* 28..2B */
  {_bge  ,_reb}, {_blt  ,_reb}, {_bgt  ,_reb}, {_ble  ,_reb},   /* 2C..2F */
  {_leax ,_ind}, {_leay ,_ind}, {_leas ,_ind}, {_leau ,_ind},   /* 30..33 */
  {_pshs ,_r2 }, {_puls ,_r2 }, {_pshu ,_r3 }, {_pulu ,_r3 },   /* 34..37 */
  {_ill  ,_nom}, {_rts  ,_imp}, {_abx  ,_imp}, {_rti  ,_imp},   /* 38..3B */
  {_cwai ,_imb}, {_mul  ,_imp}, {_reset,_imp}, {_swi  ,_imp},   /* 3C..3F */
  {_nega ,_imp}, {_ill  ,_nom}, {_ill  ,_nom}, {_coma ,_imp},   /* 40..43 */
  {_lsra ,_imp}, {_ill  ,_nom}, {_rora ,_imp}, {_asra ,_imp},   /* 44..47 */
  {_asla ,_imp}, {_rola ,_imp}, {_deca ,_imp}, {_ill  ,_nom},   /* 48..4B */
  {_inca ,_imp}, {_tsta ,_imp}, {_ill  ,_nom}, {_clra ,_imp},   /* 4C..4F */
  {_negb ,_imp}, {_ill  ,_nom}, {_ill  ,_nom}, {_comb ,_imp},   /* 50..53 */
  {_lsrb ,_imp}, {_ill  ,_nom}, {_rorb ,_imp}, {_asrb ,_imp},   /* 54..57 */
  {_aslb ,_imp}, {_rolb ,_imp}, {_decb ,_imp}, {_ill  ,_nom},   /* 58..5B */
  {_incb ,_imp}, {_tstb ,_imp}, {_ill  ,_nom}, {_clrb ,_imp},   /* 5C..5F */
  {_neg  ,_ind}, {_ill  ,_nom}, {_ill  ,_nom}, {_com  ,_ind},   /* 60..63 */
  {_lsr  ,_ind}, {_ill  ,_nom}, {_ror  ,_ind}, {_asr  ,_ind},   /* 64..67 */
  {_asl  ,_ind}, {_rol  ,_ind}, {_dec  ,_ind}, {_ill  ,_nom},   /* 68..6B */
  {_inc  ,_ind}, {_tst  ,_ind}, {_jmp  ,_ind}, {_clr  ,_ind},   /* 6C..6F */
  {_neg  ,_ext}, {_ill  ,_nom}, {_ill  ,_nom}, {_com  ,_ext},   /* 70..73 */
  {_lsr  ,_ext}, {_ill  ,_nom}, {_ror  ,_ext}, {_asr  ,_ext},   /* 74..77 */
  {_asl  ,_ext}, {_rol  ,_ext}, {_dec  ,_ext}, {_ill  ,_nom},   /* 78..7B */
  {_inc  ,_ext}, {_tst  ,_ext}, {_jmp  ,_ext}, {_clr  ,_ext},   /* 7C..7F */
  {_suba ,_imb}, {_cmpa ,_imb}, {_sbca ,_imb}, {_subd ,_imw},   /* 80..83 */
  {_anda ,_imb}, {_bita ,_imb}, {_lda  ,_imb}, {_ill  ,_nom},   /* 84..87 */
  {_eora ,_imb}, {_adca ,_imb}, {_ora  ,_imb}, {_adda ,_imb},   /* 88..8B */
  {_cmpx ,_imw}, {_bsr  ,_reb}, {_ldx  ,_imw}, {_ill  ,_nom},   /* 8C..8F */
  {_suba ,_dir}, {_cmpa ,_dir}, {_sbca ,_dir}, {_subd ,_dir},   /* 90..93 */
  {_anda ,_dir}, {_bita ,_dir}, {_lda  ,_dir}, {_sta  ,_dir},   /* 94..97 */
  {_eora ,_dir}, {_adca ,_dir}, {_ora  ,_dir}, {_adda ,_dir},   /* 98..9B */
  {_cmpx ,_dir}, {_jsr  ,_dir}, {_ldx  ,_dir}, {_stx  ,_dir},   /* 9C..9F */
  {_suba ,_ind}, {_cmpa ,_ind}, {_sbca ,_ind}, {_subd ,_ind},   /* A0..A3 */
  {_anda ,_ind}, {_bita ,_ind}, {_lda  ,_ind}, {_sta  ,_ind},   /* A4..A7 */
  {_eora ,_ind}, {_adca ,_ind}, {_ora  ,_ind}, {_adda ,_ind},   /* A8..AB */
  {_cmpx ,_ind}, {_jsr  ,_ind}, {_ldx  ,_ind}, {_stx  ,_ind},   /* AC..AF */
  {_suba ,_ext}, {_cmpa ,_ext}, {_sbca ,_ext}, {_subd ,_ext},   /* B0..B3 */
  {_anda ,_ext}, {_bita ,_ext}, {_lda  ,_ext}, {_sta  ,_ext},   /* B4..B7 */
  {_eora ,_ext}, {_adca ,_ext}, {_ora  ,_ext}, {_adda ,_ext},   /* B8..BB */
  {_cmpx ,_ext}, {_jsr  ,_ext}, {_ldx  ,_ext}, {_stx  ,_ext},   /* BC..BF */
  {_subb ,_imb}, {_cmpb ,_imb}, {_sbcb ,_imb}, {_addd ,_imw},   /* C0..C3 */
  {_andb ,_imb}, {_bitb ,_imb}, {_ldb  ,_imb}, {_ill  ,_nom},   /* C4..C7 */
  {_eorb ,_imb}, {_adcb ,_imb}, {_orb  ,_imb}, {_addb ,_imb},   /* C8..CB */
  {_ldd  ,_imw}, {_ill  ,_nom}, {_ldu  ,_imw}, {_ill  ,_nom},   /* CC..CF */
  {_subb ,_dir}, {_cmpb ,_dir}, {_sbcb ,_dir}, {_addd ,_dir},   /* D0..D3 */
  {_andb ,_dir}, {_bitb ,_dir}, {_ldb  ,_dir}, {_stb  ,_dir},   /* D4..D7 */
  {_eorb ,_dir}, {_adcb ,_dir}, {_orb  ,_dir}, {_addb ,_dir},   /* D8..DB */
  {_ldd  ,_dir}, {_std  ,_dir}, {_ldu  ,_dir}, {_stu  ,_dir},   /* DC..DF */
  {_subb ,_ind}, {_cmpb ,_ind}, {_sbcb ,_ind}, {_addd ,_ind},   /* E0..E3 */
  {_andb ,_ind}, {_bitb ,_ind}, {_ldb  ,_ind}, {_stb  ,_ind},   /* E4..E7 */
  {_eorb ,_ind}, {_adcb ,_ind}, {_orb  ,_ind}, {_addb ,_ind},   /* E8..EB */
  {_ldd  ,_ind}, {_std  ,_ind}, {_ldu  ,_ind}, {_stu  ,_ind},   /* EC..EF */
  {_subb ,_ext}, {_cmpb ,_ext}, {_sbcb ,_ext}, {_addd ,_ext},   /* F0..F3 */
  {_andb ,_ext}, {_bitb ,_ext}, {_ldb  ,_ext}, {_stb  ,_ext},   /* F4..F7 */
  {_eorb ,_ext}, {_adcb ,_ext}, {_orb  ,_ext}, {_addb ,_ext},   /* F8..FB */
  {_ldd  ,_ext}, {_std  ,_ext}, {_ldu  ,_ext}, {_stu  ,_ext},   /* FC..FF */
  };

/*****************************************************************************/
/* m6809_codes10 : $10 extended instruction 2nd byte                         */
/*****************************************************************************/

CMatrixEntry Dasm6809::m6809_codes10[256] =
  {
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 00..03 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 04..07 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 08..0B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 0C..0F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 10..13 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 14..17 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 18..1B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 1C..1F */
  {_ill  ,_nom}, {_lbrn ,_rew}, {_lbhi ,_rew}, {_lbls ,_rew},   /* 20..23 */
  {_lbcc ,_rew}, {_lbcs ,_rew}, {_lbne ,_rew}, {_lbeq ,_rew},   /* 24..27 */
  {_lbvc ,_rew}, {_lbvs ,_rew}, {_lbpl ,_rew}, {_lbmi ,_rew},   /* 28..2B */
  {_lbge ,_rew}, {_lblt ,_rew}, {_lbgt ,_rew}, {_lble ,_rew},   /* 2C..2F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 30..33 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 34..37 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 38..3B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_swi2 ,_imp},   /* 3C..3F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 40..43 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 44..47 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 48..4B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 4C..4F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 50..53 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 54..57 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 58..5B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 5C..5F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 60..63 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 64..67 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 68..6B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 6C..6F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 70..73 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 74..77 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 78..7B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 7C..7F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_cmpd ,_imw},   /* 80..83 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 84..87 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 88..8B */
  {_cmpy ,_imw}, {_ill  ,_nom}, {_ldy  ,_imw}, {_ill  ,_nom},   /* 8C..8F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_cmpd ,_dir},   /* 90..93 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 94..97 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 98..9B */
  {_cmpy ,_dir}, {_ill  ,_nom}, {_ldy  ,_dir}, {_sty  ,_dir},   /* 9C..9F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_cmpd ,_ind},   /* A0..A3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* A4..A7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* A8..AB */
  {_cmpy ,_ind}, {_ill  ,_nom}, {_ldy  ,_ind}, {_sty  ,_ind},   /* AC..AF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_cmpd ,_ext},   /* B0..B3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* B4..B7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* B8..BB */
  {_cmpy ,_ext}, {_ill  ,_nom}, {_ldy  ,_ext}, {_sty  ,_ext},   /* BC..BF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* C0..C3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* C4..C7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* C8..CB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_lds  ,_imw}, {_ill  ,_nom},   /* CC..CF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* D0..D3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* D4..D7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* D8..DB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_lds  ,_dir}, {_sts  ,_dir},   /* DC..DF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* E0..E3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* E4..E7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* E8..EB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_lds  ,_ind}, {_sts  ,_ind},   /* EC..EF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* F0..F3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* F4..F7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* F8..FB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_lds  ,_ext}, {_sts  ,_ext},   /* FC..FF */
  };

/*****************************************************************************/
/* m6809_codes11 : $11 extended instruction 2nd byte                         */
/*****************************************************************************/

CMatrixEntry Dasm6809::m6809_codes11[256] =
  {
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 00..03 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 04..07 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 08..0B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 0C..0F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 10..13 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 14..17 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 18..1B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 1C..1F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 20..23 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 24..27 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 28..2B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 2C..2F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 30..33 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 34..37 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 38..3B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_swi3 ,_imp},   /* 3C..3F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 40..43 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 44..47 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 48..4B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 4C..4F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 50..53 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 54..57 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 58..5B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 5C..5F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 60..63 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 64..67 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 68..6B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 6C..6F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 70..73 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 74..77 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 78..7B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 7C..7F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_cmpu ,_imw},   /* 80..83 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 84..87 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 88..8B */
  {_cmps ,_imw}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 8C..8F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_cmpu ,_dir},   /* 90..93 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 94..97 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 98..9B */
  {_cmps ,_dir}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 9C..9F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_cmpu ,_ind},   /* A0..A3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* A4..A7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* A8..AB */
  {_cmps ,_ind}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* AC..AF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_cmpu ,_ext},   /* B0..B3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* B4..B7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* B8..BB */
  {_cmps ,_ext}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* BC..BF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* C0..C3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* C4..C7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* C8..CB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* CC..CF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* D0..D3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* D4..D7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* D8..DB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* DC..DF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* E0..E3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* E4..E7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* E8..EB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* EC..EF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* F0..F3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* F4..F7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* F8..FB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* FC..FF */
  };


const int Dasm6809::m6809_exg_tfr[] =
  {
  _d,   _x,   _y,   _u,    _s,   _pc, _unkr, _unkr,
  _a,   _b,  _cc,  _dp, _unkr, _unkr, _unkr, _unkr
  };

const int Dasm6809::reg[] = { _x, _y, _u, _s };

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
    { "SETDP", Data },                  /* _setdp                            */
    { "OS9",   Data },                  /* _os9                              */
    // Convenience mnemonics
    { "CLF",   Data },                  /* _clf                              */
    { "CLIF",  Data },                  /* _clif                             */
    { "CLZ",   Data },                  /* _clz                              */
    { "DEU",   Data },                  /* _deu                              */
    { "DEY",   Data },                  /* _dey                              */
    { "INU",   Data },                  /* _inu                              */
    { "INY",   Data },                  /* _iny                              */
    { "PSHD",  Data },                  /* _pshd                             */
    { "PSHX",  Data },                  /* _pshx                             */
    { "PSHY",  Data },                  /* _pshy                             */
    { "PULD",  Data },                  /* _puld                             */
    { "PULX",  Data },                  /* _pulx                             */
    { "PULY",  Data },                  /* _puly                             */
    { "SEF",   Data },                  /* _sef                              */
    { "SEIF",  Data },                  /* _seif                             */
    { "SEZ",   Data },                  /* _sez                              */

  };

/*****************************************************************************/
/* regnames : additional register names over 6800                            */
/*****************************************************************************/

const char *Dasm6809::regnames[reg6809_count - reg6800_count] =
  {
  "D",                                  /* _d                                */
  "Y",                                  /* _y                                */
  "U",                                  /* _u                                */
  "DP",                                 /* _dp                               */
  "PCR",                                /* _pcr                              */
  };

/*****************************************************************************/
/* Dasm6809 : constructor                                                    */
/*****************************************************************************/

Dasm6809::Dasm6809(Application *pApp)
  : Dasm6800(pApp)
{
codes = m6809_codes;
codes10 = m6809_codes10;
codes11 = m6809_codes11;
exg_tfr = m6809_exg_tfr;
os9Patch = false;
os9Comments = false;
os9ModHeader = false;
useFlex = false;
int i;
mnemo.resize(mnemo6809_count);          /* set up additional mnemonics       */
for (i = 0; i < mnemo6809_count - mnemo6800_count; i++)
  mnemo[mnemo6800_count + i] = opcodes[i];
mnemo[_lda].mne   = "LDA";              /* adjust slight mnemo differences   */
mnemo[_ldb].mne   = "LDB";
mnemo[_sta].mne   = "STA";
mnemo[_stb].mne   = "STB";
mnemo[_ora].mne   = "ORA";
mnemo[_orb].mne   = "ORB";
regname.resize(reg6809_count);          /* set up additional register names  */
for (i = 0; i < reg6809_count - reg6800_count; i++)
  regname[reg6800_count + i] = regnames[i];

// set up options table
// class uses one generic option setter/getter pair (not mandatory)
AddOption("flex", "{off|on}\tuse FLEX9 standard labels",
          static_cast<PSetter>(&Dasm6809::Set6809Option),
          static_cast<PGetter>(&Dasm6809::Get6809Option));
AddOption("os9", "{off|on}\tpatch swi2 (os9 call)",
          static_cast<PSetter>(&Dasm6809::Set6809Option),
          static_cast<PGetter>(&Dasm6809::Get6809Option));
AddOption("os9c", "{off|on}\tauto-create os9 module comments",
          static_cast<PSetter>(&Dasm6809::Set6809Option),
          static_cast<PGetter>(&Dasm6809::Get6809Option));
AddOption("os9m", "{off|on}\tauto-create os9 module header and end",
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
else if (lname == "os9c")
  {
  os9Comments = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (lname == "os9m")
  {
  os9ModHeader = bnValue;
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
else if (lname == "os9c")    return os9Comments ? "on" : "off";
else if (lname == "os9m")    return os9ModHeader ? "on" : "off";
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
    adr_t &from,                        /* from/to (if range given)          */
    adr_t &to,
    adr_t &step,                        /* step size                         */
    vector<TMemoryArray<adr_t>> &remaps,  /* remaps, if necessary            */
    bool bProcInfo,                     /* flag whether processing           */
    int bus,                            /* target bus for command            */
    int tgtbus                          /* target bus for parameters (ign.)  */
    )
{
(void)remaps;  // unused ATM
(void)tgtbus;

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
  { "UNSETDP",      infoUnsetDP },
  };

InfoCmd cmdType = infoUnknown;
for (size_t i = 0; i < _countof(sKey); i++)
  if (key == sKey[i].name)
    {
    cmdType = sKey[i].cmdType;
    break;
    }
if (cmdType == infoUnknown)
  return false;  // I KNOW that the parent classes don't do anything else.

switch (cmdType)
  {
  case infoSetDP :                      /* SETDP [addr[-addr]] dp            */
    {
    string srange;
    string::size_type idx = value.find_first_of(" \t");
    if (idx == value.npos) idx = value.size();
    srange = value.substr(0, idx);
    adr_t dp = NO_ADDRESS;
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
    else for (adr_t scanned = from;
              scanned >= from && scanned <= to;
              scanned += step)
      SetDirectPage(scanned, dp, bus);
    }
    break;
  case infoUnsetDP :                    /* UNSETDP [addr[-addr]]             */
    if (from == NO_ADDRESS)
      dirpage = 0;
    else for (adr_t scanned = from;
              scanned >= from && scanned <= to;
              scanned += step)
      SetDirectPage(scanned, 0, bus);
    break;
  default :  // keep gcc happy
    break;
  }
return true;
}

/*****************************************************************************/
/* LoadRel : loads a relocatable module generated by RELASMB                 */
/*****************************************************************************/

bool Dasm6809::LoadRel(FILE *f, string &sLoadType)
{
int c = fgetc(f);                       /* fetch first byte to decide        */
if (c == EOF)                           /* whether it may be a rel module    */
  return false;
ungetc(c, f);
if (c != 0x03)
  return false;

int nCurPos = ftell(f);                 /* remember where we started         */

bool ok = true;
bool isCommon = true;
// read the capsules
int nCapStart = nCurPos;
while (ok)
  {
  ok &= LoadRelCapsule(f, isCommon);
  if (feof(f))
    break;
  nCapStart = ftell(f);
  }

if (ok)
  sLoadType = "REL";
else
  fseek(f, nCurPos, SEEK_SET);          /* seek back                         */
return ok;
}

/*****************************************************************************/
/* LoadRelCapsule : loads a relocatable module capsule                       */
/*****************************************************************************/

bool Dasm6809::LoadRelCapsule(FILE *f, bool &isCommon)
{
RelCapsuleHeader c;
if (fread(&c, 1, sizeof(c), f) != sizeof(c))
  return false;
if (c.signature != 0x03 ||
    (c.flags1 != 0x10 && c.flags1 != 0x12 && c.flags1 != 0x18))
  return false;
// module layout:
// [<common capsule>]* <code capsule>
int capsbytes = sizeof(c);

switch (c.flags1)                       /* what's this one, exactly?         */
  {
  case 0x10 :                           /* a relative code capsule?          */
    isCommon = false;
    break;
  case 0x12 :                           /* an absolute code capsule?         */
    isCommon = false;
    break;
  case 0x18 :                           /* a common capsule ?                */
    if (!isCommon)                      /* that must be before code          */
      return false;
    break;
  }

while (capsbytes % 0xfc)                /* capsule is filled up with zeroes  */
  {                                     /* to FLEX sector size (252)         */
  int x = fgetc(f);
  if (x)
    return false;
  capsbytes++;
  }
return true;
}

/*****************************************************************************/
/* LoadOS9 : loads an OS9 module                                             */
/*****************************************************************************/

bool Dasm6809::LoadOS9(FILE *f, string &sLoadType)
{
int c = fgetc(f);                       /* fetch first byte to decide        */
if (c == EOF)                           /* whether it may be a OS9 module    */
  return false;
ungetc(c, f);
if (c != 0x87)
  return false;

int nCurPos = ftell(f);                 /* remember where we started         */
union
  {
  OS9ModuleHeader h;
  OS9UserModuleHeader um;
  OS9DeviceDescriptorHeader dd;
  OS9ConfigModuleHeader cf;
  OS9DataModuleHeader dm;
  } mod;
// first get the normal header
if (fread(&mod, 1, sizeof(OS9ModuleHeader), f) != sizeof(OS9ModuleHeader))
  {
  fseek(f, nCurPos, SEEK_SET);          /* seek back                         */
  return false;
  }
uint16_t wModSize = ((uint16_t)mod.h.bModSize[0]) * 256 + mod.h.bModSize[1];
uint16_t wOffName = ((uint16_t)mod.h.bOffName[0]) * 256 + mod.h.bOffName[1];
uint8_t hpck = 0;  // calc hdr chksum
for (uint8_t *ph = (uint8_t *)&mod; ph != mod.um.bExecOff; ph++)
  hpck ^= *ph;
// hpck should be $FF now.
if (mod.h.bSync[1] != 0xcd ||
    !mod.h.bModType ||
    wModSize < sizeof(mod.h) + 1 ||
    wOffName < sizeof(mod.h) ||
    hpck != 0xff)
  {
  fseek(f, nCurPos, SEEK_SET);
  return false;
  }

long nHLPos = ftell(f);                 /* remember pos after header load    */
adr_t nHdrLen = (adr_t)(nHLPos - nCurPos);

// OK, looks legit. Load it into standard OS9 places.
// This should be position-independent code. Load it at $0400
adr_t const loadAddr = 0x0400;
adr_t fbegin = loadAddr, fend = fbegin + wModSize - 1, i;
// For now, including the header portion:
AddMemory(fbegin, wModSize, Code);
for (i = 0; i < nHdrLen; i++)
  SetUByte(fbegin + i, ((uint8_t *)&mod)[i]);
for (; i < wModSize; i++)
  {
  int c = fgetc(f);
  if (c == EOF)
    {
    fseek(f, nCurPos, SEEK_SET);
    return false;
    }
  if (i < sizeof(mod))                  /* fill up all possible headers      */
    ((uint8_t*)&mod)[i] = (uint8_t)c;
  SetUByte(fbegin + i, (uint8_t)c);
  }

// now calculate the checksum!
// this is a close copy of the OS9 assembler logic in CRCCHK.
uint8_t crc24[3] = { 0xff, 0xff, 0xff };
for (i = fbegin; i <= fend; i++)
  {
  uint8_t a = GetUByte(i) ^ crc24[0];
  crc24[0] = crc24[1];
  crc24[1] = crc24[2] ^ (a >> 2) ^ (a >> 7);
  crc24[2] = (a << 1) ^ (a << 6);
  a ^= (a << 1);
  a ^= (a << 2);
  a ^= (a << 4);
  if (a & 0x80)
    {
    crc24[0] ^= 0x80;
    crc24[2] ^= 0x21;
    }
  }

if (crc24[0] != 0x80 ||                 /* CRCCHK checks against $800FE3!    */
    crc24[1] != 0x0f ||
    crc24[2] != 0xe3)
  {
  fseek(f, nCurPos, SEEK_SET);          /* seek back                         */
  return false;
  }

// loaded & checked OK, so set up contents
#if 0
if (fbegin < begin)
  begin = fbegin;
if (fend > end)
  end = fend;
#endif

sLoadType = "OS9";
os9Patch = true;  // in this case, DO patch OS9 codes.
bPIC = true;      // and default to position-independent code
// NB: this requires an info file to turn the options off!

int modtype = mod.h.bModType >> 4;
int language = mod.h.bModType & 0x0f;
string modname;
adr_t endName = fbegin + wOffName;
for (; ; endName++)
  {
  c = GetUByte(endName);
  modname += (char)(c & 0x7f);
  if (c & 0x80)
    break;
  }
adr_t startContent = endName + 2;
string modnm = modname + "$Nm";
string modsiz = modname + "$Sz";
string modstrt = modname + "$St";
string datasize = modname + "$DS";

const char *modtypenames[] =
  {
  "",
  "PRGRM",
  "SBRTN",
  "MULTI",
  "DATA",
  "$50",
  "$60",
  "$70",
  "$80",
  "$90",
  "$A0",
  "$B0",
  "SYSTM",
  "FLMGR",
  "DRIVR",
  "DEVIC",
  };
const char *languagenames[] =
  {
  "",
  "OBJCT",
  "ICODE",
  "PCODE",
  "CCODE",
  "CBLCODE",
  "FRTNCODE",
  "7",
  "8",
  "9",
  "10",
  "11",
  "12",
  "13",
  "14",
  "15",
  };
const char *attributenames[] =
  {
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "REENT",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  };
string modtypename = modtypenames[modtype];
if (*languagenames[language])
  modtypename += string("+") + languagenames[mod.h.bModType & 0x0f];
string attributes = attributenames[mod.h.bAttRev >> 4];
if (attributes.size())
  attributes += "+";
attributes += (char)('0' + (mod.h.bAttRev & 0x0f));

uint16_t wExecOff = 0;
uint16_t wPSReq = 0;
if (modtype != 0x04 && modtype < 0x0a)
  {
  wExecOff = ((uint16_t)mod.um.bExecOff[0]) * 256 + mod.um.bExecOff[1];
  wPSReq = ((uint16_t)mod.um.bPSReq[0]) * 256 + mod.um.bPSReq[1];
  }

for (i = fbegin; i <= fend; i++)
  {
  if (i < startContent)
    SetMemType(i, Data);
  else
    SetMemType(i, Code);
  bool bUsed = ((begin == NO_ADDRESS || i >= begin) &&
                (end == NO_ADDRESS || i <= end));
  SetCellUsed(i, bUsed);
  SetDisplay(i, defaultDisplay);
  }
SetupOS9(fbegin, wModSize, mod.h);
if (modtype == 4)
  SetupOS9(fbegin, mod.dm);
else if (modtype < 0x0a)
  SetupOS9(fbegin, mod.um);
else if (modtype == 0x0c)
  SetupOS9(fbegin, mod.cf);
else if (modtype == 0x0f)
  SetupOS9(fbegin, mod.dd);

// this might be better in the SetupOS9 methods ... but for now, keep it here.
if (os9ModHeader)
  {
  if (wExecOff)
    AddLabel(loadAddr + wExecOff, Code, modstrt, true);

  pApp->AddComment(fbegin, false, sformat("%*s %s%*s 0", 7, "", mnemo[_if].mne, 5, ""), true, false);
  pApp->AddComment(startContent, false, "", true, false);
  pApp->AddComment(startContent, false, sformat("%*s %s", 7, "", mnemo[_endif].mne), true, false);
  pApp->AddComment(startContent, false, datasize + " " + mnemo[_equ].mne + " " + Number2String(wPSReq, 4, NO_ADDRESS), true, false);
  pApp->AddComment(startContent, false, modnm + " " + mnemo[_fcs].mne + " /" + modname + "/", true, false);
  string modcmd(sformat("%*s %s%*s", 7, "", "mod", 4, ""));
  modcmd += modsiz + "," + modnm + "," + modtypename + "," + attributes + ",";
  modcmd += modstrt + "," + datasize;
  pApp->AddComment(startContent, false, modcmd, true, false);
  pApp->AddComment(startContent, false, sformat("%*s %s", 7, "", mnemo[_else].mne), true, false);

  adr_t chksadr = fend - 2;
  pApp->AddComment(chksadr, false, sformat("%*s %s%*s 0", 7, "", mnemo[_if].mne, 5, ""), true, false);
  pApp->AddComment(chksadr, false, "", true, false);
  pApp->AddComment(chksadr, true, sformat("%*s %s", 7, "", mnemo[_endif].mne), true, false);
  pApp->AddComment(chksadr, true, modsiz + " " + mnemo[_equ].mne + " *", true, false);
  pApp->AddComment(chksadr, true, sformat("%*s %s", 7, "", "emod"), true, false);
  pApp->AddComment(chksadr, true, sformat("%*s %s", 7, "", mnemo[_else].mne), true, false);
  }
else if (wExecOff)
  AddLabel(fbegin + wExecOff, Code, "", true);

offset = fend + 1;

(void)wPSReq;  // keep gcc happy

return true;
}

/*****************************************************************************/
/* SetupOS9 : format according to the module type                            */
/*****************************************************************************/

bool Dasm6809::SetupOS9(adr_t loadAddr, uint16_t modsize, OS9ModuleHeader &h)
{
// header and checksum are const
adr_t i;
for (i = 0; i < sizeof(h); i++)
  SetMemType(loadAddr + i, Const);
adr_t chksadr = loadAddr + modsize - 3;
for (i = 0; i < 3; i++)
  SetMemType(chksadr + i, Const);
SetCellSize(loadAddr + 2, 2);
SetCellType(loadAddr + 2, MemAttribute::UnsignedInt);
SetCellSize(loadAddr + 4, 2);
SetCellType(loadAddr + 4, MemAttribute::UnsignedInt);
SetBreakBefore(loadAddr + 7);
SetBreakBefore(loadAddr + 8);
uint16_t wOffName = ((uint16_t)h.bOffName[0]) * 256 + h.bOffName[1];
if (os9Comments)
  {
  pApp->AddComment(loadAddr, false, "OS9 Module Header Data");
  pApp->AddLComment(loadAddr, "Sync bytes");
  pApp->AddLComment(loadAddr + 2, "Module size");
  pApp->AddLComment(loadAddr + 4, "Module name offset");
  pApp->AddLComment(loadAddr + 6, "Module type and language");
  pApp->AddLComment(loadAddr + 7, "Attributes and revision level");
  pApp->AddLComment(loadAddr + 8, "Header checksum");
  pApp->AddLComment(loadAddr + wOffName, "Module name");
  pApp->AddComment(chksadr, false, "OS9 Checksum Data");
  pApp->AddLComment(chksadr, "Checksum bytes");
  }
return true;
}

bool Dasm6809::SetupOS9(adr_t loadAddr, OS9DataModuleHeader &h)
{
(void)h; // maybe needed later

SetMemType(loadAddr + 9, Const);
SetCellSize(loadAddr + 9, 2);
SetCellType(loadAddr + 9, MemAttribute::UnsignedInt);
SetMemType(loadAddr + 11, Const);
SetCellSize(loadAddr + 11, 2);
SetCellType(loadAddr + 11, MemAttribute::UnsignedInt);

if (os9Comments)
  {
  pApp->AddLComment(loadAddr + 9, "Data offset");
  pApp->AddLComment(loadAddr + 11, "Data size");
  }

return true;
}

bool Dasm6809::SetupOS9(adr_t loadAddr, OS9UserModuleHeader &h)
{
(void)h; // maybe needed later

SetMemType(loadAddr + 9, Const);
SetCellSize(loadAddr + 9, 2);
SetCellType(loadAddr + 9, MemAttribute::UnsignedInt);
SetMemType(loadAddr + 11, Const);
SetCellSize(loadAddr + 11, 2);
SetCellType(loadAddr + 11, MemAttribute::UnsignedInt);
if (os9Comments)
  {
  pApp->AddLComment(loadAddr + 9, "Execution offset");
  pApp->AddLComment(loadAddr + 11, "Permament storage requirement");
  }

#if 0
// if os9ModHeader goes in here, this might become interesting
uint16_t wExecOff = ((uint16_t)h.bExecOff[0]) * 256 + h.bExecOff[1];
uint16_t wPSReq = ((uint16_t)h.bPSReq[0]) * 256 + h.bPSReq[1];
#endif

return true;
}

bool Dasm6809::SetupOS9(adr_t loadAddr, OS9DeviceDescriptorHeader &h)
{
(void)h; // maybe needed later

for (int8_t i = sizeof(OS9ModuleHeader);
     i < (int8_t)sizeof(OS9DeviceDescriptorHeader);
     i++)
  SetMemType(loadAddr + i, Const);
SetCellSize(loadAddr + 9, 2);
SetCellType(loadAddr + 9, MemAttribute::UnsignedInt);
SetCellSize(loadAddr + 11, 2);
SetCellType(loadAddr + 11, MemAttribute::UnsignedInt);
if (os9Comments)
  {
  pApp->AddLComment(loadAddr + 9, "File manager name offset");
  pApp->AddLComment(loadAddr + 11, "Device driver name offset");
  pApp->AddLComment(loadAddr + 12, "Mode byte");
  pApp->AddLComment(loadAddr + 13, "Device controller address (24 bit)");
  pApp->AddLComment(loadAddr + 16, "Table size");
  }
return true;
}

bool Dasm6809::SetupOS9(adr_t loadAddr, OS9ConfigModuleHeader &h)
{
(void)h; // maybe needed later

for (int8_t i = sizeof(OS9ModuleHeader);
     i < (int8_t)sizeof(OS9ConfigModuleHeader);
     i++)
  SetMemType(loadAddr + i, Const);
SetCellSize(loadAddr + 14, 2);
SetCellType(loadAddr + 14, MemAttribute::UnsignedInt);
SetCellSize(loadAddr + 16, 2);
SetCellType(loadAddr + 16, MemAttribute::UnsignedInt);
SetCellSize(loadAddr + 18, 2);
SetCellType(loadAddr + 18, MemAttribute::UnsignedInt);
if (os9Comments)
  {
  pApp->AddLComment(loadAddr + 9, "Top limit of free RAM (24 bit)");
  pApp->AddLComment(loadAddr + 12, "IRQ polling entries");
  pApp->AddLComment(loadAddr + 13, "Device entries");
  pApp->AddLComment(loadAddr + 14, "Offset to startup module name");
  pApp->AddLComment(loadAddr + 16, "Offset to default drive name");
  pApp->AddLComment(loadAddr + 18, "Offset to bootstrap module name");
  }

return true;
}

/*****************************************************************************/
/* LoadFile : loads an opened file                                           */
/*****************************************************************************/

bool Dasm6809::LoadFile(string filename, FILE *f, string &sLoadType, int interleave, int bus)
{
return LoadOS9(f, sLoadType) ||  // OS9 files need no interleave nor bus
       LoadRel(f, sLoadType) ||  // RELASMB output needs no interleave nor bus
       Dasm6800::LoadFile(filename, f, sLoadType, interleave, bus);
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
    for (adr_t addr = 0xfff2; addr < 0xfff8; addr += 2)
      {
      MemoryType memType = GetMemType(addr);
      if (memType != Untyped &&         /* if system vector loaded           */
          memType != Const &&           /* and not defined as constant       */
          !FindLabel(addr))             /* and no label set in info file     */
        {
        SetMemType(addr, Data);         /* that's a data word                */
        SetCellSize(addr, 2);
                                        /* look whether it points to loaded  */
        adr_t tgtaddr = GetUWord(addr);
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

adr_t Dasm6809::FetchInstructionDetails
    (
    adr_t PC,
    uint8_t &instpg,
    uint8_t &instb,
    uint8_t &mode,
    int &mnemoIndex
    )
{
instpg = instb = GetUByte(PC++);
if (instpg == 0x10)
  {
  instb = GetUByte(PC++);
  mnemoIndex = codes10[instb].mne;
  mode = codes10[instb].adrmode;
  }
else if (instpg == 0x11)
  {
  instb = GetUByte(PC++);
  mnemoIndex = codes11[instb].mne;
  mode = codes11[instb].adrmode;
  }
else
  {
  instpg = 0;
  mnemoIndex = codes[instb].mne;
  mode = codes[instb].adrmode;
  }

return PC;
}

/*****************************************************************************/
/* SetConvenience : set convenience mnemonics as appropriate                 */
/*****************************************************************************/

bool Dasm6809::SetConvenience(uint8_t instpg, uint16_t u2, string &smnemo, adr_t &PC)
{
uint16_t W;

switch (u2)
  {
  case 0x1a01 :                         /* ORCC $01 -> SEC                   */
    smnemo = mnemo[_sec].mne; PC++;
    return true;
  case 0x1a02 :                         /* ORCC $02 -> SEV                   */
    smnemo = mnemo[_sev].mne; PC++;
    return true;
  case 0x1a04 :                         /* ORCC $04 -> SEZ                   */
    smnemo = mnemo[_sez].mne; PC++;
    return true;
  case 0x1a10 :                         /* ORCC $10 -> SEI                   */
    smnemo = mnemo[_sei].mne; PC++;
    return true;
  case 0x1a40 :                         /* ORCC $40 -> SEF                   */
    smnemo = mnemo[_sef].mne; PC++;
    return true;
  case 0x1a50 :                         /* ORCC $40+$10 -> SEIF              */
    smnemo = mnemo[_seif].mne; PC++;
    return true;
  case 0x1cfe :                         /* ANDCC ~$01 -> CLC                 */
    smnemo = mnemo[_clc].mne; PC++;
    return true;
  case 0x1cfd :                         /* ANDCC ~$02 -> CLV                 */
    smnemo =  mnemo[_clv].mne; PC++;
    return true;
  case 0x1cfb :                         /* ANDCC ~$04 -> CLZ                 */
    smnemo = mnemo[_clz].mne; PC++;
    return true;
  case 0x1cef :                         /* ANDCC ~$10 -> CLI                 */
    smnemo = mnemo[_cli].mne; PC++;
    return true;
  case 0x1cbf :                         /* ANDCC ~$40 -> CLF                 */
    smnemo = mnemo[_clf].mne; PC++;
    return true;
  case 0x1caf :                         /* ANDCC ~($40 + $10) -> CLIF        */
    smnemo = mnemo[_clif].mne; PC++;
    return true;
  case 0x3cff :                         /* CWAI $FF -> WAI                   */
    smnemo = mnemo[_wai].mne; PC++;
    return true;
  case 0x3001 :                         /* LEAX +1 -> INX                    */
    smnemo = mnemo[_inx].mne; PC++;
    return true;
  case 0x301f :                         /* LEAX -1 -> DEX                    */
    smnemo = mnemo[_dex].mne; PC++;
    return true;
  case 0x3121 :                         /* LEAY +1 -> INY                    */
    smnemo = mnemo[_iny].mne; PC++;
    return true;
  case 0x313f :                         /* LEAY -1 -> DEY                    */
    smnemo = mnemo[_dey].mne; PC++;
    return true;
  case 0x3261 :                         /* LEAS +1 -> INS                    */
    smnemo = mnemo[_ins].mne; PC++;
    return true;
  case 0x327f :                         /* LEAS -1 -> DES                    */
    smnemo = mnemo[_des].mne; PC++;
    return true;
  case 0x3341 :                         /* LEAU +1 -> INU                    */
    smnemo = mnemo[_inu].mne; PC++;
    return true;
  case 0x335f :                         /* LEAU -1 -> DEU                    */
    smnemo = mnemo[_deu].mne; PC++;
    return true;
  case 0x1f14 :                         /* TFR X,S -> TXS                    */
    smnemo = mnemo[_txs].mne; PC++;
    return true;
  case 0x1f41 :                         /* TFR S,X -> TSX                    */
    smnemo = mnemo[_tsx].mne; PC++;
    return true;
/* hairy - some assemblers expand TAB to TAB + TSTA...
   but there's no guarantee.
  case 0x1f89 :
    smnemo = mnemo[_tab]; PC++;
    return true; */
  case 0x1f8a :                         /* TFR A,CC -> TAP                   */
    smnemo = mnemo[_tap].mne; PC++;
    return true;
/* hairy - some assemblers expand TBA to TBA + TSTA...
   but there's no guarantee.
  case 0x1f98 :
    smnemo = mnemo[_tba]; PC++;
    return true; */
  case 0x1fa8 :                         /* TFR CC,A -> TPA                   */
    smnemo = mnemo[_tpa].mne; PC++;
    return true;
  case 0x3404 :                         /* PSHS B + ...                      */
    W = IsCLabel(PC + 1) ? 0 : GetUWord(PC + 1);
    switch (W)
      {
      case 0xa0e0 :                     /* PSHS B / SUBA ,S++ -> SBA         */
        smnemo = mnemo[_sba].mne; PC += 3;
        return true;
      case 0xa1e0 :                     /* PSHS B / CMPA ,S++ -> CBA         */
        smnemo = mnemo[_cba].mne; PC += 3;
        return true;
      case 0xabe0 :                     /* PSHS B / ADDA ,S++ -> ABA         */
        smnemo = mnemo[_aba].mne; PC += 3;
        return true;
      default:                          /* PSHS B / anything else -> PSHB    */
        smnemo = mnemo[_pshb].mne; PC++;
        return true;
      }
    break;
  case 0x3402 :                         /* PSHS A -> PSHA                    */
    smnemo = mnemo[_psha].mne; PC++;
    return true;
  case 0x3406 :                         /* PSHS D -> PSHD                    */
    smnemo = mnemo[_pshd].mne; PC++;
    return true;
  case 0x3410 :                         /* PSHS X -> PSHX                    */
    smnemo = mnemo[_pshx].mne; PC++;
    return true;
  case 0x3420 :                         /* PSHS Y -> PSHY                    */
    smnemo = mnemo[_pshy].mne; PC++;
    return true;
  case 0x3502 :                         /* PULS A -> PULA                    */
    smnemo = mnemo[_pula].mne; PC++;
    return true;
  case 0x3504 :                         /* PULS B -> PULB                    */
    smnemo = mnemo[_pulb].mne; PC++;
    return true;
  case 0x3506 :                         /* PULS D -> PULD                    */
    smnemo = mnemo[_puld].mne; PC++;
    return true;
  case 0x3510 :                         /* PULS X -> PULX                    */
    smnemo = mnemo[_pulx].mne; PC++;
    return true;
  case 0x3520 :                         /* PULS Y -> PULY                    */
    smnemo = mnemo[_puly].mne; PC++;
    return true;
  case 0x8300 :                         /* SUBD #1 -> DECD                   */
    W = GetUWord(PC);
    if (W == 1)
      {
      smnemo = mnemo[_decd].mne; PC += 2;
      return true;
      }
    break;
  case 0xc300 :                         /* ADDD #1 -> INCD                   */
    W = GetUWord(PC);
    if (W == 1)
      {
      smnemo = mnemo[_incd].mne; PC += 2;
      return true;
      }
    break;
  case 0xc001 :                         /* 6800-style DECD collision         */
  case 0xcd01 :                         /* 6800-style INCD collision         */
    return false;
  }

return Dasm6800::SetConvenience(instpg, u2, smnemo, PC);
}

/*****************************************************************************/
/* IndexParse : parses index for labels                                      */
/*****************************************************************************/

#define HMHMHM 0
adr_t Dasm6809::IndexParse(int mnemoIndex, adr_t pc, adr_t instaddr)
{
uint8_t T;
uint16_t W;
uint16_t Wrel;
// char R;
adr_t PC = pc;
bool bSetLabel = true;
Label *lbl;

T = GetUByte(PC++);
// R = reg[(T >> 5) & 0x03];

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
#if HMHMHM
        AddRelativeLabel((uint16_t)((int)((char)T) + PC), PC - 1, mnemo[mnemoIndex].memType,
                         true, BusCode, instaddr);
#else
        AddLabel((uint16_t)((int)((char)T) + PC), mnemo[mnemoIndex].memType, "", true);
#endif
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
      // no labels in indirect addressing!
      // ...except when they are explicitly given in the info file, of course
      if (bSetLabel &&
          (W != Wrel ||                 /* if it's relative, or label's there*/
            FindLabel(Wrel)))
#if 1
        AddRelativeLabel(W, PC, mnemo[mnemoIndex].memType, true, BusCode, instaddr);
#else
        AddLabel(Wrel,                  /* mark it as used                   */
                 mnemo[mnemoIndex].memType, "", true);
#endif
      PC++;
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
#if HMHMHM
        AddRelativeLabel((uint16_t)(W + PC), PC - 2, mnemo[mnemoIndex].memType, true,
                         BusCode, instaddr);
#else
        AddLabel((uint16_t)(W + PC), mnemo[mnemoIndex].memType, "", true);
#endif
      break;
    case 0x09:
      bSetLabel = !IsConst(PC);
      Wrel = (uint16_t)GetRelative(PC);
      if (!bSetLabel && !Wrel)
        SetDefLabelUsed(PC);
      SetCellSize(PC, 2);
      W = GetUWord(PC);
      Wrel += W;
      // no labels in indirect addressing!
      // ...except when they are explicitly given in the info file, of course
      if (bSetLabel &&
          (W != Wrel ||                 /* if it's relative, or label's there*/
            FindLabel(Wrel)))
#if 1
        AddRelativeLabel(W, PC, mnemo[mnemoIndex].memType, true, BusCode, instaddr);
#else
        AddLabel(Wrel,                  /* mark it as used                   */
                 mnemo[mnemoIndex].memType, "", true);
#endif
      PC += 2;
      break;
    case 0x19:
    case 0x1D:
      bSetLabel = !IsConst(PC);
      if (!bSetLabel)
        SetDefLabelUsed(PC);
      SetCellSize(PC, 2);
      W = GetUWord(PC); PC += 2;
      if (bSetLabel)
#if HMHMHM
        AddRelativeLabel((uint16_t)(W + PC), PC - 2, mnemo[mnemoIndex].memType, true,
                         BusCode, instaddr);
#else
        AddLabel((uint16_t)(W + PC), mnemo[mnemoIndex].memType, "", true);
#endif
      break;

    default:
      if (T == 0x9F)
        {
        bSetLabel = !IsConst(PC);
        lbl = bSetLabel ? NULL : FindLabel(PC, Const);
        if (lbl)
          lbl->SetUsed();
        SetCellSize(PC, 2);
        W = GetUWord(PC);
        if (bSetLabel)
#if HMHMHM
          AddRelativeLabel(W, PC, mnemo[mnemoIndex].memType, true, BusCode, instaddr);
#else
          AddLabel(W, mnemo[mnemoIndex].memType, "", true);
#endif
        PC += 2;
        }
      break;
    }
  }
else
  {
  // lowest 5 bits are a signed offset in range [-16..+15]
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
#if HMHMHM
      AddRelativeLabel(W, PC - 1, mnemo[mnemoIndex].memType, true, BusCode, instaddr);
#else
      AddLabel(W, mnemo[mnemoIndex].memType, "", true);
#endif
    }
  else
    lbl = FindLabel(PC - 1, Const);
  if (lbl)
    lbl->SetUsed();
  }

// (void)R;  // unused ATM - keep gcc happy
return PC;
}

/*****************************************************************************/
/* IndexString : converts index to string                                    */
/*****************************************************************************/

string Dasm6809::IndexString(string &smnemo, adr_t &pc)
{
uint8_t T;
uint16_t W, Wrel;
char R;
string buf;
adr_t PC = pc;
bool bGetLabel;
Label *lbl;

T = GetUByte(PC++);
R = reg[(T >> 5) & 0x03];

if (T & 0x80)
  {
  switch (T & 0x1F)
    {
    case 0x00:
      buf = MnemoCase(sformat(",%s+", regname[R].c_str()));
      break;
    case 0x01:
      buf = MnemoCase(sformat(",%s++", regname[R].c_str()));
      break;
    case 0x02:
      buf = MnemoCase(sformat(",-%s", regname[R].c_str()));
      break;
    case 0x03:
      buf = MnemoCase(sformat(",--%s", regname[R].c_str()));
      break;
    case 0x04:
      buf = MnemoCase(sformat(",%s", regname[R].c_str()));
      if (GetRelative(PC - 1))
        {
        bGetLabel = !IsConst(PC - 1);
        buf += sformat("   %s (%s)",
                       commentStart.c_str(),
                       Label2String(0, 4, bGetLabel, PC - 1).c_str());
        }
      break;
    case 0x05:
      buf = MnemoCase(sformat("%s,%s",
                              regname[_b].c_str(), regname[R].c_str()));
      break;
    case 0x06:
      buf = MnemoCase(sformat("%s,%s",
                              regname[_a].c_str(), regname[R].c_str()));
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
        buf = sformat(bFwd ? "<%s,%s" : "%s,%s",
                      slbl.c_str(),
                      MnemoCase(regname[R]).c_str());
        }
      else
        {
        string slbl = lbl ? lbl->GetText() : SignedNumber2String((int)((char)T), 2, PC);
        // DefLabels are output before the code, so they ARE defined and need no <
        // buf = sformat((lbl && Wrel >= PC - 1) ? "<%s,%s" : "%s,%s", slbl.c_str(), MnemoCase(R)).c_str());
        buf = sformat((lbl && Wrel >= PC - 1) ? "%s,%s" : "%s,%s",
                      slbl.c_str(), MnemoCase(regname[R]).c_str());
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
        buf = sformat("%s,%s", slbl.c_str(), MnemoCase(regname[R]).c_str());
        if (((W < 0x80) || (W >= 0xff80)) && forceExtendedAddr)
          //buf = ">" + buf;
          AddForced(smnemo, buf, true);
        }
      else
        {
        string slbl;
        if (((W < 0x80) || (W >= 0xff80)) && forceExtendedAddr)
          {
#if 1
          slbl = lbl ? lbl->GetText() : SignedNumber2String((int)(short)W, 4, PC);
          AddForced(smnemo, slbl, true);
          buf = sformat("%s,%s", slbl.c_str(), MnemoCase(regname[R]).c_str());
#else
          buf = sformat(">%s,%s", slbl.c_str(), MnemoCase(regname[R]).c_str());
#endif
          }
        else
          {
          slbl = lbl ? lbl->GetText() : Number2String((uint16_t)(int)(short)W, 4, PC);  /* RB: this was "signed_string" */
          buf = sformat("%s,%s", slbl.c_str(), MnemoCase(regname[R]).c_str());
          }
        }
      PC += 2;
      break;
    case 0x0B:
      buf = MnemoCase(sformat("%s,%s", regname[_d].c_str(), regname[R].c_str()));
      break;
    case 0x0C:
      T = GetUByte(PC);
      bGetLabel = !IsConst(PC);
#if 0
      sprintf(buf,"$%s,%s",
              signed_string((int)(char)T, 2, (word)PC),
              regname[_pc].c_str());
#else
      if (bGetLabel)
        {
        string slbl = Label2String((uint16_t)((int)((char)T) + PC + 1), 4,
                                   bGetLabel, PC);
        if (((char)T > 0) && forceDirectAddr)
          AddForced(smnemo, slbl, false);
        buf = slbl + "," + MnemoCase(regname[_pcr]);
        }
      else
        {
        lbl = FindLabel(PC, Const);
        string slbl = lbl ? lbl->GetText() : Number2String((uint16_t)(int)(char)T, 2, PC);
        if (((char)T > 0) && forceDirectAddr)
          AddForced(smnemo, slbl, false);
        buf = slbl + "," + MnemoCase(regname[_pc]);
        }
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
#if 1
        AddForced(smnemo, slbl, true);
#else
        buf = sformat(">%s", slbl.c_str()) + "," + MnemoCase(regname[_pcr]);
      else
#endif
      buf = slbl + "," + MnemoCase(regname[_pcr]);
      }
      break;
    case 0x11:
      buf = MnemoCase(sformat("[,%s++]", regname[R].c_str()));
      break;
    case 0x13:
      buf = MnemoCase(sformat("[,--%s]", regname[R].c_str()));
      break;
    case 0x14:
      buf = MnemoCase(sformat("[,%s]", regname[R].c_str()));
      break;
    case 0x15:
      buf = MnemoCase(sformat("[%s,%s]", regname[_b].c_str(), regname[R].c_str()));
      break;
    case 0x16:
      buf = MnemoCase(sformat("[%s,%s]", regname[_a].c_str(), regname[R].c_str()));
      break;
    case 0x18:
      {
      lbl = FindLabel(PC, Const);
      T = GetUByte(PC);
      string slbl = lbl ? lbl->GetText() : Number2String(T, 2, PC);
      buf = sformat("[%s,%s]",
              slbl.c_str(),
              MnemoCase(regname[R]).c_str());
      PC++;
      }
      break;
    case 0x19:
      {
      bGetLabel = !IsConst(PC);
      lbl = bGetLabel ? NULL : FindLabel(PC, Const);
      W = GetUWord(PC);
      string slbl = lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
      buf = sformat("[%s,%s]", slbl.c_str(), MnemoCase(regname[R]).c_str());
      PC += 2;
      }
      break;
    case 0x1B:
      buf = MnemoCase(sformat("[%s,%s]",
                              regname[_d].c_str(), regname[R].c_str()));
      break;
    case 0x1C:
      {
#if 1
      T = GetUByte(PC);
      bGetLabel = !IsConst(PC);
      if (bGetLabel)
        {
        string slbl = Label2String((uint16_t)((int)((char)T) + PC + 1), 4,
                           bGetLabel, PC);
        if (((char)T > 0) && forceDirectAddr)
          AddForced(smnemo, slbl, false);
        buf = slbl + "," + MnemoCase(regname[_pcr]);
        }
      else
        {
        lbl = FindLabel(PC, Const);
        string slbl = lbl ? lbl->GetText() : Number2String((uint16_t)(int)(char)T, 2, PC);
        if (((char)T > 0) && forceDirectAddr)
          AddForced(smnemo, slbl, false);
        buf = slbl + "," + MnemoCase(regname[_pc].c_str());
        }
      buf = "[" + buf + "]";
      PC++;
#else
      lbl = FindLabel(PC, Const);
      T = GetUByte(PC);
      string slbl = lbl ? lbl->GetText() : Number2String(T, 2, PC);
      buf = sformat("[%s,%s]", slbl.c_str(), regname[_pc].c_str());
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
#if 1
        AddForced(smnemo, slbl, true);
#else
        buf = sformat("[>%s,%s]", slbl.c_str(), MnemoCase(regname[_pcr]).c_str());
      else
#endif
      buf = sformat("[%s,%s]", slbl.c_str(), MnemoCase(regname[_pcr]).c_str());
#else
      bGetLabel = !IsConst(PC);
      lbl = bGetLabel ? NULL : FindLabel(PC, Const);
      W = GetUWord(PC);
      string slbl = lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
      buf = sformat("[%s,%s]", slbl.c_str(), regname[_pc].c_str());
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
      buf = MnemoCase(regname[_unkr]);
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
        buf = MnemoCase(regname[_unkr]);
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
    buf = sformat("%s,%s", slbl.c_str(), MnemoCase(regname[R]).c_str());
    }
  else
    {
    lbl = FindLabel(PC - 1, Const);
    string slbl = lbl ? lbl->GetText() : SignedNumber2String(c, 2, PC - 1);
    buf = sformat("%s,%s", slbl.c_str(), MnemoCase(regname[R]).c_str());
    }
  }

pc = PC;
return buf;
}

/*****************************************************************************/
/* ParseCode : parse instruction at given memory address for labels          */
/*****************************************************************************/

adr_t Dasm6809::ParseCode
    (
    adr_t addr,
    int bus                             /* ignored for 6800 and derivates    */
    )
{
uint8_t instpg, instb, /* T, */ mode;
uint16_t W;
int mnemoIndex;
bool bSetLabel;
Label *lbl;
adr_t PC = FetchInstructionDetails(addr, instpg, instb, mode, mnemoIndex);

if ((mnemoIndex == _swi2) && os9Patch)
  return (PC + 1 - addr);

switch (mode)                           /* which mode is this ?              */
  {
  case _ind:                            /* indexed                           */
    PC = IndexParse(mnemoIndex, PC, addr);
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
      AddLabel(W, mnemo[mnemoIndex].memType, "", true);
    break;
    
  case _r1:                             /* tfr/exg mode                      */
    /* T = GetUByte(PC); */ PC++;
    break;

  case _r2:                             /* pul/psh system                    */
  case _r3:                             /* pul/psh user                      */
    /* T = GetUByte(PC); */ PC++;
    break;

  default :                             /* anything else is handled by base  */
    return Dasm6800::ParseCode(addr, bus);
  }
return PC - addr;                       /* pass back # processed bytes       */
}

/*****************************************************************************/
/* DisassembleCode : disassemble code instruction at given memory address    */
/*****************************************************************************/

adr_t Dasm6809::DisassembleCode
    (
    adr_t addr,
    string &smnemo,
    string &sparm,
    int bus                             /* ignored for 6800 and derivates    */
    )
{
uint8_t instpg, instb, T, mode;
uint16_t W;
int mnemoIndex;
adr_t PC = FetchInstructionDetails(addr, instpg, instb, mode, mnemoIndex);
bool bGetLabel;
Label *lbl;

smnemo = mnemo[mnemoIndex].mne;
if ((mnemoIndex == _swi2) && os9Patch)
  {
  T = GetUByte(PC++);
  smnemo = mnemo[_os9].mne;
  sparm = os9_codes[T];
  return PC - addr;
  }

switch (mode)                           /* which mode is this?               */
  {
  case _ext:                            /* extended                          */
    {
    bGetLabel = !IsConst(PC);
    lbl = bGetLabel ? NULL : FindLabel(PC, Const, bus);
    W = GetUWord(PC);
    if (bGetLabel)
      W = (uint16_t)PhaseInner(W, PC);
    string slbl = lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
    adr_t dp = GetDirectPage(PC);
    if (dp == DEFAULT_ADDRESS || dp == NO_ADDRESS)
      dp = GetDirectPage(addr);
    if (dp == DEFAULT_ADDRESS)
      dp = 0;
#if 1
    if ((forceExtendedAddr && (W & (uint16_t)0xff00) == (uint16_t)dp) ||
        GetForcedAddr(PC))
      AddForced(smnemo, slbl, true);
    sparm = slbl;
#else
    if (forceExtendedAddr && (W & (uint16_t)0xff00) == (uint16_t)dp)
      sparm = ">" + slbl;
    else
      {
      sparm = GetForcedAddr(PC) ? ">" : "";
      sparm += slbl;
      }
#endif
    PC += 2;
    }
    break;

  case _ind:                            /* indexed                           */
    if (!useConvenience || 
        !SetConvenience(instpg, (uint16_t)(instb << 8) | GetUByte(PC), smnemo, PC))
      sparm = IndexString(smnemo, PC);
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
    if (!useConvenience || 
        !SetConvenience(instpg, (uint16_t)(instb << 8) | GetUByte(PC), smnemo, PC))
      {
      T = GetUByte(PC++);
      sparm = sformat("%s,%s",
                      MnemoCase(regname[exg_tfr[T >> 4]]).c_str(),
                      MnemoCase(regname[exg_tfr[T & 0xF]]).c_str());
      }
    break;

  case _r2:                             /* pul/psh system                    */
  case _r3:                             /* pul/psh user                      */
    if (!useConvenience || 
        !SetConvenience(instpg, (uint16_t)(instb << 8) | GetUByte(PC), smnemo, PC))
      {
      // buf[0] = '\0';
      T = GetUByte(PC++);
      if (T & 0x80)
        sparm += MnemoCase(regname[_pc]) + ",";
      if (T & 0x40)
        {
        if (mode == _r2)
          sparm += MnemoCase(regname[_u]) + ",";
        else if (mode == _r3)
          sparm += MnemoCase(regname[_s]) + ",";
        }
      if (T&0x20)
        sparm += MnemoCase(regname[_y]) + ",";
      if (T & 0x10)
        sparm += MnemoCase(regname[_x]) + ",";
      if (T & 0x08)
        sparm += MnemoCase(regname[_dp]) + ",";
      if ((T & 0x06) == 0x06)
        sparm += MnemoCase(regname[_d]) + ",";
      else
        {
        if (T & 0x04)
          sparm += MnemoCase(regname[_b]) + ",";
        if (T & 0x02)
          sparm += MnemoCase(regname[_a]) + ",";
        }
      if (T & 0x01)
        sparm += MnemoCase(regname[_cc]) + ",";
      if (sparm.size())
        sparm = sparm.substr(0, sparm.size() - 1);
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
  // no specialties here
  return Dasm6800::DisassembleChanges(addr, prevaddr, prevsz, bAfterLine, changes, bus);
  }
if (!bAfterLine)                        /* if before the address             */
  {
  // report direct page changes
  adr_t dpold =
      (prevaddr == NO_ADDRESS) ? DEFAULT_ADDRESS : GetDirectPage(prevaddr, bus);
  adr_t dp = GetDirectPage(addr, bus);
  if (!dp && dpold == DEFAULT_ADDRESS) dpold = dp;
  if (dp != dpold)
    {
    LineChange chg;
    changes.push_back(chg);
    chg.oper = MnemoCase(mnemo[_setdp].mne);
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
  adr_t from;
  adr_t to;
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

for (size_t i = 0; i < _countof(FlexLbls); i++)
  {
  for (adr_t a = FlexLbls[i].from; a <= FlexLbls[i].to; a++)
    {
    string s(FlexLbls[i].txt);
    if (a != FlexLbls[i].from)
      s += sformat("+%s", Number2String(a - FlexLbls[i].from, 4, a).c_str());
    AddLabel(a, Untyped, s);
    }
  }
}
