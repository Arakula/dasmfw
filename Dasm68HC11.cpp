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
/* Dasm68HC11.cpp : 68HC11 disassembler classes implementation               */
/*****************************************************************************/

#include "Dasm68HC11.h"

/*****************************************************************************/
/* Create68HC11 : create an 68HC11 disassembler                              */
/*****************************************************************************/

static Disassembler *Create68HC11(Application *pApp)
{
Disassembler *pDasm = new Dasm68HC11(pApp);
if (pDasm) pDasm->Setup();
return pDasm;
}

/*****************************************************************************/
/* Auto-registration                                                         */
/*****************************************************************************/

static bool bRegistered[] =
  {
  RegisterDisassembler("hc11", Create68HC11),
  };


/*===========================================================================*/
/* Dasm68HC11 class members                                                  */
/*===========================================================================*/

/*****************************************************************************/
/* m68hc11_codes : table of all 68HC11 instruction bytes and types           */
/*****************************************************************************/

CMatrixEntry Dasm68HC11::m68hc11_codes[256] =
  {
  {_test ,_imp}, {_nop  ,_imp}, {_idiv ,_imp}, {_fdiv ,_imp},   /* 00..03 */
  {_lsrd ,_imp}, {_asld ,_imp}, {_tap  ,_imp}, {_tpa  ,_imp},   /* 04..07 */
  {_inx  ,_imp}, {_dex  ,_imp}, {_clv  ,_imp}, {_sev  ,_imp},   /* 08..0B */
  {_clc  ,_imp}, {_sec  ,_imp}, {_cli  ,_imp}, {_sei  ,_imp},   /* 0C..0F */
  {_sba  ,_imp}, {_cba  ,_imp}, {_brset,_bbd}, {_brclr,_bbd},   /* 10..13 */
  {_bset ,_bd }, {_bclr ,_bd }, {_tab  ,_imp}, {_tba  ,_imp},   /* 14..17 */
  {_ill  ,_nom}, {_daa  ,_imp}, {_ill  ,_nom}, {_aba  ,_imp},   /* 18..1B */
  {_bset ,_bi }, {_bclr ,_bi }, {_brset,_bbi}, {_brclr,_bbi},   /* 1C..1F */
  {_bra  ,_reb}, {_brn  ,_reb}, {_bhi  ,_reb}, {_bls  ,_reb},   /* 20..23 */
  {_bcc  ,_reb}, {_bcs  ,_reb}, {_bne  ,_reb}, {_beq  ,_reb},   /* 24..27 */
  {_bvc  ,_reb}, {_bvs  ,_reb}, {_bpl  ,_reb}, {_bmi  ,_reb},   /* 28..2B */
  {_bge  ,_reb}, {_blt  ,_reb}, {_bgt  ,_reb}, {_ble  ,_reb},   /* 2C..2F */
  {_tsx  ,_imp}, {_ins  ,_imp}, {_pula ,_imp}, {_pulb ,_imp},   /* 30..33 */
  {_des  ,_imp}, {_txs  ,_imp}, {_psha ,_imp}, {_pshb ,_imp},   /* 34..37 */
  {_pulx ,_imp}, {_rts  ,_imp}, {_abx  ,_imp}, {_rti  ,_imp},   /* 38..3B */
  {_pshx ,_imp}, {_mul  ,_imp}, {_wai  ,_imp}, {_swi  ,_imp},   /* 3C..3F */
  {_nega ,_imp}, {_ill  ,_nom}, {_ill  ,_nom}, {_coma ,_imp},   /* 40..43 */
  {_lsra ,_imp}, {_ill  ,_nom}, {_rora ,_imp}, {_asra ,_imp},   /* 44..47 */
  {_asla ,_imp}, {_rola ,_imp}, {_deca ,_imp}, {_ill  ,_nom},   /* 48..4B */
  {_inca ,_imp}, {_tsta ,_imp}, {_ill  ,_nom}, {_clra ,_imp},   /* 4C..4F */
  {_negb ,_imp}, {_ill  ,_nom}, {_ill  ,_nom}, {_comb ,_imp},   /* 50..53 */
  {_lsrb ,_imp}, {_ill  ,_nom}, {_rorb ,_imp}, {_asrb ,_imp},   /* 54..57 */
  {_aslb ,_imp}, {_rolb ,_imp}, {_decb ,_imp}, {_ill  ,_nom},   /* 58..5B */
  {_incb ,_imp}, {_tstb ,_imp}, {_ill  ,_nom}, {_clrb ,_imp},   /* 5C..5F */
  {_neg  ,_ix8}, {_ill  ,_nom}, {_ill  ,_nom}, {_com  ,_ix8},   /* 60..63 */
  {_lsr  ,_ix8}, {_ill  ,_nom}, {_ror  ,_ix8}, {_asr  ,_ix8},   /* 64..67 */
  {_asl  ,_ix8}, {_rol  ,_ix8}, {_dec  ,_ix8}, {_ill  ,_nom},   /* 68..6B */
  {_inc  ,_ix8}, {_tst  ,_ix8}, {_jmp  ,_ix8}, {_clr  ,_ix8},   /* 6C..6F */
  {_neg  ,_ext}, {_ill  ,_nom}, {_ill  ,_nom}, {_com  ,_ext},   /* 70..73 */
  {_lsr  ,_ext}, {_ill  ,_nom}, {_ror  ,_ext}, {_asr  ,_ext},   /* 74..77 */
  {_asl  ,_ext}, {_rol  ,_ext}, {_dec  ,_ext}, {_ill  ,_nom},   /* 78..7B */
  {_inc  ,_ext}, {_tst  ,_ext}, {_jmp  ,_ext}, {_clr  ,_ext},   /* 7C..7F */
  {_suba ,_imb}, {_cmpa ,_imb}, {_sbca ,_imb}, {_subd ,_imw},   /* 80..83 */
  {_anda ,_imb}, {_bita ,_imb}, {_lda  ,_imb}, {_ill  ,_nom},   /* 84..87 */
  {_eora ,_imb}, {_adca ,_imb}, {_ora  ,_imb}, {_adda ,_imb},   /* 88..8B */
  {_cpx  ,_imw}, {_bsr  ,_reb}, {_lds  ,_imw}, {_xgdx ,_imp},   /* 8C..8F */
  {_suba ,_dir}, {_cmpa ,_dir}, {_sbca ,_dir}, {_subd ,_dir},   /* 90..93 */
  {_anda ,_dir}, {_bita ,_dir}, {_lda  ,_dir}, {_sta  ,_dir},   /* 94..97 */
  {_eora ,_dir}, {_adca ,_dir}, {_ora  ,_dir}, {_adda ,_dir},   /* 98..9B */
  {_cpx  ,_dir}, {_jsr  ,_dir}, {_lds  ,_dir}, {_sts  ,_dir},   /* 9C..9F */
  {_suba ,_ix8}, {_cmpa ,_ix8}, {_sbca ,_ix8}, {_subd ,_ix8},   /* A0..A3 */
  {_anda ,_ix8}, {_bita ,_ix8}, {_lda  ,_ix8}, {_sta  ,_ix8},   /* A4..A7 */
  {_eora ,_ix8}, {_adca ,_ix8}, {_ora  ,_ix8}, {_adda ,_ix8},   /* A8..AB */
  {_cpx  ,_ix8}, {_jsr  ,_ix8}, {_lds  ,_ix8}, {_sts  ,_ix8},   /* AC..AF */
  {_suba ,_ext}, {_cmpa ,_ext}, {_sbca ,_ext}, {_subd ,_ext},   /* B0..B3 */
  {_anda ,_ext}, {_bita ,_ext}, {_lda  ,_ext}, {_sta  ,_ext},   /* B4..B7 */
  {_eora ,_ext}, {_adca ,_ext}, {_ora  ,_ext}, {_adda ,_ext},   /* B8..BB */
  {_cpx  ,_ext}, {_jsr  ,_ext}, {_lds  ,_ext}, {_sts  ,_ext},   /* BC..BF */
  {_subb ,_imb}, {_cmpb ,_imb}, {_sbcb ,_imb}, {_addd ,_imw},   /* C0..C3 */
  {_andb ,_imb}, {_bitb ,_imb}, {_ldb  ,_imb}, {_ill  ,_nom},   /* C4..C7 */
  {_eorb ,_imb}, {_adcb ,_imb}, {_orb  ,_imb}, {_addb ,_imb},   /* C8..CB */
  {_ldd  ,_imw}, {_ill  ,_nom}, {_ldx  ,_imw}, {_stop ,_imp},   /* CC..CF */
  {_subb ,_dir}, {_cmpb ,_dir}, {_sbcb ,_dir}, {_addd ,_dir},   /* D0..D3 */
  {_andb ,_dir}, {_bitb ,_dir}, {_ldb  ,_dir}, {_stb  ,_dir},   /* D4..D7 */
  {_eorb ,_dir}, {_adcb ,_dir}, {_orb  ,_dir}, {_addb ,_dir},   /* D8..DB */
  {_ldd  ,_dir}, {_std  ,_dir}, {_ldx  ,_dir}, {_stx  ,_dir},   /* DC..DF */
  {_subb ,_ix8}, {_cmpb ,_ix8}, {_sbcb ,_ix8}, {_addd ,_ix8},   /* E0..E3 */
  {_andb ,_ix8}, {_bitb ,_ix8}, {_ldb  ,_ix8}, {_stb  ,_ix8},   /* E4..E7 */
  {_eorb ,_ix8}, {_adcb ,_ix8}, {_orb  ,_ix8}, {_addb ,_ix8},   /* E8..EB */
  {_ldd  ,_ix8}, {_std  ,_ix8}, {_ldx  ,_ix8}, {_stx  ,_ix8},   /* EC..EF */
  {_subb ,_ext}, {_cmpb ,_ext}, {_sbcb ,_ext}, {_addd ,_ext},   /* F0..F3 */
  {_andb ,_ext}, {_bitb ,_ext}, {_ldb  ,_ext}, {_stb  ,_ext},   /* F4..F7 */
  {_eorb ,_ext}, {_adcb ,_ext}, {_orb  ,_ext}, {_addb ,_ext},   /* F8..FB */
  {_ldd  ,_ext}, {_std  ,_ext}, {_ldx  ,_ext}, {_stx  ,_ext},   /* FC..FF */
  };

/*****************************************************************************/
/* m68hc11_codes18 : $18 extended instruction 2nd byte                       */
/*****************************************************************************/

CMatrixEntry Dasm68HC11::m68hc11_codes18[256] =
  {
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 00..03 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 04..07 */
  {_iny  ,_imp}, {_dey  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 08..0B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 0C..0F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 10..13 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 14..17 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 18..1B */
  {_bset ,_bi }, {_bclr ,_bi }, {_brset,_bbi}, {_brclr,_bbi},   /* 1C..1F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 20..23 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 24..27 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 28..2B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 2C..2F */
  {_tsy  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 30..33 */
  {_ill  ,_nom}, {_tys  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 34..37 */
  {_puly ,_imp}, {_ill  ,_nom}, {_aby  ,_imp}, {_ill  ,_nom},   /* 38..3B */
  {_pshy ,_imp}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 3C..3F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 40..43 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 44..47 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 48..4B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 4C..4F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 50..53 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 54..57 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 58..5B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 5C..5F */
  {_neg  ,_ix8}, {_ill  ,_nom}, {_ill  ,_nom}, {_com  ,_ix8},   /* 60..63 */
  {_lsr  ,_ix8}, {_ill  ,_nom}, {_ror  ,_ix8}, {_asr  ,_ix8},   /* 64..67 */
  {_asl  ,_ix8}, {_rol  ,_ix8}, {_dec  ,_ix8}, {_ill  ,_nom},   /* 68..6B */
  {_inc  ,_ix8}, {_tst  ,_ix8}, {_jmp  ,_ix8}, {_clr  ,_ix8},   /* 6C..6F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 70..73 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 74..77 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 78..7B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 7C..7F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 80..83 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 84..87 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 88..8B */
  {_cpy  ,_imw}, {_ill  ,_nom}, {_ill  ,_nom}, {_xgdy ,_imp},   /* 8C..8F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 90..93 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 94..97 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 98..9B */
  {_cpy  ,_dir}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 9C..9F */
  {_suba ,_ix8}, {_cmpa ,_ix8}, {_sbca ,_ix8}, {_subd ,_ix8},   /* A0..A3 */
  {_anda ,_ix8}, {_bita ,_ix8}, {_lda  ,_ix8}, {_sta  ,_ix8},   /* A4..A7 */
  {_eora ,_ix8}, {_adca ,_ix8}, {_ora  ,_ix8}, {_adda ,_ix8},   /* A8..AB */
  {_cpy  ,_ix8}, {_jsr  ,_ix8}, {_lds  ,_ix8}, {_sts  ,_ix8},   /* AC..AF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* B0..B3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* B4..B7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* B8..BB */
  {_cpy  ,_ext}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* BC..BF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* C0..C3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* C4..C7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* C8..CB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ldy  ,_imw}, {_ill  ,_nom},   /* CC..CF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* D0..D3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* D4..D7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* D8..DB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ldy  ,_dir}, {_sty  ,_dir},   /* DC..DF */
  {_subb ,_ix8}, {_cmpb ,_ix8}, {_sbcb ,_ix8}, {_addd ,_ix8},   /* E0..E3 */
  {_andb ,_ix8}, {_bitb ,_ix8}, {_ldb  ,_ix8}, {_stb  ,_ix8},   /* E4..E7 */
  {_eorb ,_ix8}, {_adcb ,_ix8}, {_orb  ,_ix8}, {_addb ,_ix8},   /* E8..EB */
  {_ldd  ,_ix8}, {_std  ,_ix8}, {_ldy  ,_ix8}, {_sty  ,_ix8},   /* EC..EF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* F0..F3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* F4..F7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* F8..FB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ldy  ,_ext}, {_sty  ,_ext},   /* FC..FF */
  };

/*****************************************************************************/
/* m68hc11_codes1a : $1A extended instruction 2nd byte                       */
/*****************************************************************************/

CMatrixEntry Dasm68HC11::m68hc11_codes1a[256] =
  {
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 00..03 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 04..07 */
  {_iny  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 08..0B */
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
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 3C..3F */
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
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_cpd  ,_imw},   /* 80..83 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 84..87 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 88..8B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 8C..8F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_cpd  ,_dir},   /* 90..93 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 94..97 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 98..9B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 9C..9F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_cpd  ,_ix8},   /* A0..A3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* A4..A7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* A8..AB */
  {_cpy  ,_ix8}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* AC..AF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_cpd  ,_ext},   /* B0..B3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* B4..B7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* B8..BB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* BC..BF */
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
  {_ill  ,_nom}, {_ill  ,_nom}, {_ldy  ,_ix8}, {_sty  ,_ix8},   /* EC..EF */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* F0..F3 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* F4..F7 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* F8..FB */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* FC..FF */
  };

/*****************************************************************************/
/* m68hc11_codescd : $CD extended instruction 2nd byte                       */
/*****************************************************************************/

CMatrixEntry Dasm68HC11::m68hc11_codescd[256] =
  {
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 00..03 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 04..07 */
  {_iny  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 08..0B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 0C..0F */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 10..13 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 14..17 */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 18..1B */
  {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom}, {_ill  ,_nom},   /* 1C..1F */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 20..23 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 24..27 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 28..2B */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 2C..2F */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 30..33 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 34..37 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 38..3B */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 3C..3F */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 40..43 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 44..47 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 48..4B */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 4C..4F */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 50..53 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 54..57 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 58..5B */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 5C..5F */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 60..63 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 64..67 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 68..6B */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 6C..6F */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 70..73 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 74..77 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 78..7B */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 7C..7F */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 80..83 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 84..87 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 88..8B */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 8C..8F */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 90..93 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 94..97 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 98..9B */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* 9C..9F */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_cpd  ,_ix8},   /* A0..A3 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* A4..A7 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* A8..AB */
  {_cpx  ,_ix8}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* AC..AF */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* B0..B3 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* B4..B7 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* B8..BB */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* BC..BF */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* C0..C3 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* C4..C7 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* C8..CB */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* CC..CF */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* D0..D3 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* D4..D7 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* D8..DB */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* DC..DF */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* E0..E3 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* E4..E7 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* E8..EB */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ldx  ,_ix8}, {_stx  ,_ix8},   /* EC..EF */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* F0..F3 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* F4..F7 */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* F8..FB */
  {_ill  ,_nom}, {_nop  ,_imp}, {_ill  ,_nom}, {_ill  ,_nom},   /* FC..FF */
  };

/*****************************************************************************/
/* opcodes : additional opcodes over 6800                                    */
/*****************************************************************************/

OpCode Dasm68HC11::opcodes[mnemo68HC11_count - mnemo6801_count] =
  {
    { "ABY",   Data },                  /* _aby                              */
    { "BCLR",  Code },                  /* _bclr                             */
    { "BRCLR", Code },                  /* _brclr                            */
    { "BRSET", Code },                  /* _brset                            */
    { "BSET",  Code },                  /* _bset                             */
    { "CPD",   Data },                  /* _cpd                              */
    { "CPY",   Data },                  /* _cpy                              */
    { "DEY",   Data },                  /* _dey                              */
    { "FDIV",  Data },                  /* _fdiv                             */
    { "IDIV",  Data },                  /* _idiv                             */
    { "INY",   Data },                  /* _iny                              */
    { "LDY",   Data },                  /* _ldy                              */
    { "PSHY",  Data },                  /* _pshy                             */
    { "PULY",  Data },                  /* _puly                             */
    { "STOP",  Data },                  /* _stop                             */
    { "STY",   Data },                  /* _sty                              */
    { "TEST",  Data },                  /* _test                             */
    { "TSY",   Data },                  /* _tsy                              */
    { "TYS",   Data },                  /* _tys                              */
    { "XGDX",  Data },                  /* _xgdx                             */
    { "XGDY",  Data },                  /* _xgdy                             */
  };

/*****************************************************************************/
/* regnames : additional register names over 6800                            */
/*****************************************************************************/

const char *Dasm68HC11::regnames[reg68HC11_count - reg6801_count] =
  {
  "Y",                                  /* _y                                */
  };


/*****************************************************************************/
/* Dasm68HC11 : constructor                                                  */
/*****************************************************************************/

Dasm68HC11::Dasm68HC11(Application *pApp)
  : Dasm6801(pApp)
{
codes = m68hc11_codes;
codes18 = m68hc11_codes18;
codes1a = m68hc11_codes1a;
codescd = m68hc11_codescd;
boppcom = false;  // A09 can't handle commas as bitop parameter delimiter yet
int i;
mnemo.resize(mnemo68HC11_count);        /* set up additional mnemonics       */
for (i = 0; i < mnemo68HC11_count - mnemo6801_count; i++)
  mnemo[mnemo6801_count + i] = opcodes[i];
regname.resize(reg6801_count);          /* set up additional register names  */
for (i = 0; i < reg68HC11_count - reg6801_count; i++)
  regname[reg6801_count + i] = regnames[i];
// set up options table
// class uses one generic option setter/getter pair (not mandatory)
AddOption("boppcom", "{off|on}\tbit operation parameter comma delimiter",
          static_cast<PSetter>(&Dasm68HC11::Set68HC11Option),
          static_cast<PGetter>(&Dasm68HC11::Get68HC11Option));
}

/*****************************************************************************/
/* ~Dasm68HC11 : destructor                                                  */
/*****************************************************************************/

Dasm68HC11::~Dasm68HC11(void)
{
}

/*****************************************************************************/
/* Set68HC11Option : sets a disassembler option                              */
/*****************************************************************************/

int Dasm68HC11::Set68HC11Option(string lname, string value)
{
bool bnValue = true;                    /* default to "on"                   */
bool bIsBool = ParseBool(value, bnValue);

if (lname == "boppcom")
  {
  boppcom = bnValue;
  return bIsBool ? 1 : 0;
  }
else
  return 0;                             /* only name consumed                */

// should never happen ... but better safe than sorry
return 1;                               /* name and value consumed           */
}

/*****************************************************************************/
/* Get68HC11Option : retrieves a disassembler option                         */
/*****************************************************************************/

string Dasm68HC11::Get68HC11Option(string lname)
{
if (lname == "boppcom")      return boppcom ? "on" : "off";
return "";
}


/*****************************************************************************/
/* InitParse : initialize parsing                                            */
/*****************************************************************************/

bool Dasm68HC11::InitParse(int bus)
{
// 68HC11 has a drastically different interrupt vector table layout,
// so DON'T initialize for 6800/6801!

if (bus == BusCode)
  {
  if (bSetSysVec)
    {
    static const char *vectbl[] =
      {
      "SCI",                            /* ffd6                              */
      "SPI",                            /* ffd8                              */
      "PAI",                            /* ffda                              */
      "PAO",                            /* ffdc                              */
      "TIO",                            /* ffde                              */
      "TI4O5",                          /* ffe0                              */
      "TO4",                            /* ffe2                              */
      "TO3",                            /* ffe4                              */
      "TO2",                            /* ffe6                              */
      "TO1",                            /* ffe8                              */
      "TI3",                            /* ffea                              */
      "TI2",                            /* ffec                              */
      "TI1",                            /* ffee                              */
      "RTI",                            /* fff0                              */
      "IRQ",                            /* fff2                              */
      "XIRQ",                           /* fff4                              */
      "SWI",                            /* fff6                              */
      "ILL",                            /* fff8                              */
      "COPF",                           /* fffa                              */
      "CMOF",                           /* fffc                              */
      "RST",                            /* fffe                              */
      };
    for (adr_t addr = 0xffd6; addr <= GetHighestCodeAddr(); addr += 2)
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
                   sformat("vec_%s", vectbl[(addr - 0xffd6) / 2]),
                   true);
          }
        }
      }
    }
  }

return Disassembler::InitParse(bus);
}

/*****************************************************************************/
/* FetchInstructionDetails : fetch details about current instruction         */
/*****************************************************************************/

adr_t Dasm68HC11::FetchInstructionDetails
    (
    adr_t PC,
    uint8_t &instpg,
    uint8_t &instb,
    uint8_t &mode,
    int &mnemoIndex
    )
{
instpg = instb = GetUByte(PC++);
if (instpg == 0x18)
  {
  instb = GetUByte(PC++);
  mnemoIndex = codes18[instb].mne;
  mode = codes18[instb].adrmode;
  }
else if (instpg == 0x1A)
  {
  instb = GetUByte(PC++);
  mnemoIndex = codes1a[instb].mne;
  mode = codes1a[instb].adrmode;
  }
else if (instpg == 0xCD)
  {
  instb = GetUByte(PC++);
  mnemoIndex = codescd[instb].mne;
  mode = codescd[instb].adrmode;
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
/* GetIx8IndexReg : return index register for indexed addressing             */
/*****************************************************************************/

string Dasm68HC11::GetIx8IndexReg(uint8_t instpg)
{
if (instpg == 0x18 || instpg == 0xCD)
  return "," + MnemoCase(regname[_y]);
return "," + MnemoCase(regname[_x]);
}

/*****************************************************************************/
/* ParseCode : parse instruction at given memory address for labels          */
/*****************************************************************************/

adr_t Dasm68HC11::ParseCode
    (
    adr_t addr,
    int bus                             /* ignored for 6800 and derivates    */
    )
{
uint8_t instpg, instb, T, mode;
uint16_t W;
int mnemoIndex;
bool bSetLabel;
adr_t PC = FetchInstructionDetails(addr, instpg, instb, mode, mnemoIndex);

switch (mode)                           /* which mode is this ?              */
  {
  case _bd :                            /* Bit Manipulation direct           */
    bSetLabel = !IsConst(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);
    else
      {
      W = GetUByte(PC);
      W = (uint16_t)PhaseInner(W, PC);
      AddRelativeLabel(W, PC, mnemo[mnemoIndex].memType, true, BusCode, addr);
      }
    PC++;
    bSetLabel = !IsConst(PC);
    T = GetUByte(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);         /* the bit part                      */
    PC++;
    break;
  case _bi :                            /* Bit Manipulation indexed          */
    bSetLabel = !IsConst(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);
    PC++;
    bSetLabel = !IsConst(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);         /* the bit part                      */
    PC++;
    break;
  case _bbd :                           /* branch bitmask direct             */
    bSetLabel = !IsConst(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);
    else
      {
      W = GetUByte(PC);
      W = (uint16_t)PhaseInner(W, PC);
      AddRelativeLabel(W, PC, Data, true, BusCode, addr);
      }
    PC++;
    bSetLabel = !IsConst(PC);
    T = GetUByte(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);         /* the bit part                      */
    PC++;
    bSetLabel = !IsConst(PC);
    T = GetUByte(PC);
    W = (uint16_t)(PC + 1 + (signed char)T);
    if (bSetLabel)
      {
      W = (uint16_t)DephaseOuter(W, PC);
      // AddLabel(W, mnemo[MI].memType, "", true);
      AddRelativeLabel(W, PC, mnemo[mnemoIndex].memType, true, BusCode, addr);
      }
    else
      SetDefLabelUsed(PC, bus);
    PC++;
    break;
  case _bbi :                           /* branch bitmask indexed            */
    bSetLabel = !IsConst(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);
    PC++;
    bSetLabel = !IsConst(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);         /* the bit part                      */
    PC++;
    bSetLabel = !IsConst(PC);
    T = GetUByte(PC);
    W = (uint16_t)(PC + 1 + (signed char)T);
    if (bSetLabel)
      {
      W = (uint16_t)DephaseOuter(W, PC);
      // AddLabel(W, mnemo[MI].memType, "", true);
      AddRelativeLabel(W, PC, mnemo[mnemoIndex].memType, true, BusCode, addr);
      }
    else
      SetDefLabelUsed(PC, bus);
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

adr_t Dasm68HC11::DisassembleCode
    (
    adr_t addr,
    string &smnemo,
    string &sparm,
    int bus                             /* ignored for 6800 and derivates    */
    )
{
uint8_t instpg, instb, T, mode;
uint16_t W;
adr_t Wrel;
int mnemoIndex;
bool bGetLabel;
Label *lbl;
adr_t PC = FetchInstructionDetails(addr, instpg, instb, mode, mnemoIndex);

smnemo = mnemo[mnemoIndex].mne;
switch (mode)                           /* which mode is this?               */
  {
  case _bd :                            /* Bit Manipulation direct           */
    {
    bGetLabel = !IsConst(PC);
    lbl = bGetLabel ? NULL : FindLabel(PC, Const, bus);
    W = GetUByte(PC);
    if (bGetLabel)
      W = (uint16_t)PhaseInner(W, PC);
    string slbl = lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
    PC++;
    lbl = FindLabel(PC, Const, bus);    /* the bit part                      */
    T = GetUByte(PC);
    string smask = lbl ? lbl->GetText() : Number2String(T, 2, PC);
    PC++;
    char cdelim = boppcom ? ',' : ' ';
    sparm = sformat("%s%c%s",
                    slbl.c_str(),
                    cdelim,
                    smask.c_str());
    }
    break;
  case _bi :                            /* Bit Manipulation indexed          */
    {
    bGetLabel = !IsConst(PC);
    T = GetUByte(PC);
    Wrel = GetRelative(PC);
    lbl = (bGetLabel || Wrel) ? NULL : FindLabel(PC, Const, bus);
    string slbl;
    if (Wrel)
      {
      W = (int)((unsigned char)T) + (uint16_t)Wrel;
      slbl = Label2String((adr_t)((int)((unsigned char)T)), 4,
                          bGetLabel, PC) + GetIx8IndexReg(instpg);
      }
    else if (lbl)
      slbl = lbl->GetText() + GetIx8IndexReg(instpg);
    else if (!T && !showIndexedModeZeroOperand)
      // omit '$00', unless the user has set the 'showzero' option
      slbl = GetIx8IndexReg(instpg);
    else
      slbl = Number2String(T, 2, PC) + GetIx8IndexReg(instpg);
    PC++;
    T = GetUByte(PC);
    lbl = FindLabel(PC, Const, bus);    /* the bit part                      */
    string smask = lbl ? lbl->GetText() : Number2String(T, 2, PC);
    PC++;
    char cdelim = boppcom ? ',' : ' ';
    sparm = sformat("%s%c%s",
                    slbl.c_str(),
                    cdelim,
                    smask.c_str());
    }
    break;
  case _bbd :                           /* branch bitmask direct             */
    {
    bGetLabel = !IsConst(PC);
    lbl = bGetLabel ? NULL : FindLabel(PC, Const, bus);
    W = GetUByte(PC);
    if (bGetLabel)
      W = (uint16_t)PhaseInner(W, PC);
    string slbl = lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
    PC++;
    lbl = FindLabel(PC, Const, bus);    /* the bit part                      */
    T = GetUByte(PC);
    string smask = lbl ? lbl->GetText() : Number2String(T, 2, PC);
    PC++;
    bGetLabel = !IsConst(PC);
    lbl = bGetLabel ? NULL : FindLabel(PC, Const, bus);
    T = GetUByte(PC++);
    string stgt;
    if (bGetLabel)
      {
      W = (uint16_t)(PC + (int8_t)T);
      W = (uint16_t)DephaseOuter(W, PC - 1);
      stgt = Label2String(W, 4, bGetLabel, PC - 1);
      }
    else
      {
      int nDiff = (int)(int8_t)T;
      nDiff += (PC - addr);
      stgt = "*";
      if (nDiff)
        {
        if (nDiff >= 0 || lbl)
          stgt += "+";
        stgt += lbl ? lbl->GetText() : SignedNumber2String(nDiff, 2, PC - 1);
        }
      }
    char cdelim = boppcom ? ',' : ' ';
    sparm = sformat("%s%c%s%c%s",
                    slbl.c_str(),
                    cdelim,
                    smask.c_str(),
                    cdelim,
                    stgt.c_str());
    }
    break;
  case _bbi :                           /* branch bitmask indexed            */
    {
    bGetLabel = !IsConst(PC);
    T = GetUByte(PC);
    Wrel = GetRelative(PC);
    lbl = (bGetLabel || Wrel) ? NULL : FindLabel(PC, Const, bus);
    string slbl;
    if (Wrel)
      {
      W = (int)((unsigned char)T) + (uint16_t)Wrel;
      slbl = Label2String((adr_t)((int)((unsigned char)T)), 4,
                          bGetLabel, PC) + GetIx8IndexReg(instpg);
      }
    else if (lbl)
      slbl = lbl->GetText() + GetIx8IndexReg(instpg);
    else if (!T && !showIndexedModeZeroOperand)
      slbl = GetIx8IndexReg(instpg);   /* omit '$00', unless the user has set the 'showzero' option */
    else
      slbl = Number2String(T, 2, PC) + GetIx8IndexReg(instpg);
    PC++;
    T = GetUByte(PC);
    lbl = FindLabel(PC, Const, bus);    /* the bit part                      */
    string smask = lbl ? lbl->GetText() : Number2String(T, 2, PC);
    PC++;
    bGetLabel = !IsConst(PC);
    lbl = bGetLabel ? NULL : FindLabel(PC, Const, bus);
    T = GetUByte(PC++);
    string stgt;
    if (bGetLabel)
      {
      W = (uint16_t)(PC + (int8_t)T);
      W = (uint16_t)DephaseOuter(W, PC - 1);
      stgt = Label2String(W, 4, bGetLabel, PC - 1);
      }
    else
      {
      int nDiff = (int)(int8_t)T;
      nDiff += (PC - addr);
      stgt = "*";
      if (nDiff)
        {
        if (nDiff >= 0 || lbl)
          stgt += "+";
        stgt += lbl ? lbl->GetText() : SignedNumber2String(nDiff, 2, PC - 1);
        }
      }
    char cdelim = boppcom ? ',' : ' ';
    sparm = sformat("%s%c%s%c%s",
                    slbl.c_str(),
                    cdelim,
                    smask.c_str(),
                    cdelim,
                    stgt.c_str());
    }
    break;
  default :                             /* anything else is handled by base  */
    return Dasm6801::DisassembleCode(addr, smnemo, sparm, bus);
  }
return PC - addr;                       /* pass back # processed bytes       */
}
