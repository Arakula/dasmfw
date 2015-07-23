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
/* Dasm6800.cpp : 6800 disassembler implementation                           */
/*****************************************************************************/

#include "Dasm6800.h"

/*****************************************************************************/
/* Create6800 : create an 6800 processor handler                             */
/*****************************************************************************/

static Disassembler *Create6800()
{
Disassembler *pDasm = new Dasm6800;
if (pDasm) pDasm->Setup();
return pDasm;
}

/*****************************************************************************/
/* Auto-registration                                                         */
/*****************************************************************************/

static bool bRegistered[] =
  {
  RegisterDisassembler("6800", Create6800),
  RegisterDisassembler("6802", Create6800),
  RegisterDisassembler("6808", Create6800),
  };


/*===========================================================================*/
/* Dasm6800 class members                                                    */
/*===========================================================================*/

/*****************************************************************************/
/* m6800_codes : table of all 6800 instruction bytes and types               */
/*****************************************************************************/

uint8_t Dasm6800::m6800_codes[512] =
  {
  _ill  ,_nom,   _nop  ,_imp,   _ill  ,_nom,   _ill  ,_nom,     /* 00..03 */
  _ill  ,_nom,   _ill  ,_nom,   _tap  ,_imp,   _tpa  ,_imp,     /* 04..07 */
  _inx  ,_imp,   _dex  ,_imp,   _clv  ,_imp,   _sev  ,_imp,     /* 08..0B */
  _clc  ,_imp,   _sec  ,_imp,   _cli  ,_imp,   _sei  ,_imp,     /* 0C..0F */
  _sba  ,_imp,   _cba  ,_imp,   _ill  ,_nom,   _ill  ,_nom,     /* 10..13 */
  _ill  ,_nom,   _ill  ,_nom,   _tab  ,_imp,   _tba  ,_imp,     /* 14..17 */
  _ill  ,_nom,   _daa  ,_imp,   _ill  ,_nom,   _aba  ,_imp,     /* 18..1B */
  _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,   _ill  ,_nom,     /* 1C..1F */
  _bra  ,_reb,   _ill  ,_nom,   _bhi  ,_reb,   _bls  ,_reb,     /* 20..23 */
  _bcc  ,_reb,   _bcs  ,_reb,   _bne  ,_reb,   _beq  ,_reb,     /* 24..27 */
  _bvc  ,_reb,   _bvs  ,_reb,   _bpl  ,_reb,   _bmi  ,_reb,     /* 28..2B */
  _bge  ,_reb,   _blt  ,_reb,   _bgt  ,_reb,   _ble  ,_reb,     /* 2C..2F */
  _tsx  ,_imp,   _ins  ,_imp,   _pula ,_imp,   _pulb ,_imp,     /* 30..33 */
  _des  ,_imp,   _txs  ,_imp,   _psha ,_imp,   _pshb ,_imp,     /* 34..37 */
  _ill  ,_nom,   _rts  ,_imp,   _ill  ,_nom,   _rti  ,_imp,     /* 38..3B */
  _ill  ,_nom,   _ill  ,_nom,   _wai  ,_imp,   _swi  ,_imp,     /* 3C..3F */
  _nega ,_imp,   _ill  ,_nom,   _ill  ,_nom,   _coma ,_imp,     /* 40..43 */
  _lsra ,_imp,   _ill  ,_nom,   _rora ,_imp,   _asra ,_imp,     /* 44..47 */
  _asla ,_imp,   _rola ,_imp,   _deca ,_imp,   _ill  ,_nom,     /* 48..4B */
  _inca ,_imp,   _tsta ,_imp,   _ill  ,_nom,   _clra ,_imp,     /* 4C..4F */
  _negb ,_imp,   _ill  ,_nom,   _ill  ,_nom,   _comb ,_imp,     /* 50..53 */
  _lsrb ,_imp,   _ill  ,_nom,   _rorb ,_imp,   _asrb ,_imp,     /* 54..57 */
  _aslb ,_imp,   _rolb ,_imp,   _decb ,_imp,   _ill  ,_nom,     /* 58..5B */
  _incb ,_imp,   _tstb ,_imp,   _ill  ,_nom,   _clrb ,_imp,     /* 5C..5F */
  _neg  ,_ix8,   _ill  ,_nom,   _ill  ,_nom,   _com  ,_ix8,     /* 60..63 */
  _lsr  ,_ix8,   _ill  ,_nom,   _ror  ,_ix8,   _asr  ,_ix8,     /* 64..67 */
  _asl  ,_ix8,   _rol  ,_ix8,   _dec  ,_ix8,   _ill  ,_nom,     /* 68..6B */
  _inc  ,_ix8,   _tst  ,_ix8,   _jmp  ,_ix8,   _clr  ,_ix8,     /* 6C..6F */
  _neg  ,_ext,   _ill  ,_nom,   _ill  ,_nom,   _com  ,_ext,     /* 70..73 */
  _lsr  ,_ext,   _ill  ,_nom,   _ror  ,_ext,   _asr  ,_ext,     /* 74..77 */
  _asl  ,_ext,   _rol  ,_ext,   _dec  ,_ext,   _ill  ,_nom,     /* 78..7B */
  _inc  ,_ext,   _tst  ,_ext,   _jmp  ,_ext,   _clr  ,_ext,     /* 7C..7F */
  _suba ,_imb,   _cmpa ,_imb,   _sbca ,_imb,   _ill  ,_nom,     /* 80..83 */
  _anda ,_imb,   _bita ,_imb,   _lda  ,_imb,   _ill  ,_nom,     /* 84..87 */
  _eora ,_imb,   _adca ,_imb,   _ora  ,_imb,   _adda ,_imb,     /* 88..8B */
  _cpx  ,_imw,   _bsr  ,_reb,   _lds  ,_imw,   _ill  ,_nom,     /* 8C..8F */
  _suba ,_dir,   _cmpa ,_dir,   _sbca ,_dir,   _ill  ,_nom,     /* 90..93 */
  _anda ,_dir,   _bita ,_dir,   _lda  ,_dir,   _sta  ,_dir,     /* 94..97 */
  _eora ,_dir,   _adca ,_dir,   _ora  ,_dir,   _adda ,_dir,     /* 98..9B */
  _cpx  ,_dir,   _ill  ,_nom,   _lds  ,_dir,   _sts  ,_dir,     /* 9C..9F */
  _suba ,_ix8,   _cmpa ,_ix8,   _sbca ,_ix8,   _ill  ,_nom,     /* A0..A3 */
  _anda ,_ix8,   _bita ,_ix8,   _lda  ,_ix8,   _sta  ,_ix8,     /* A4..A7 */
  _eora ,_ix8,   _adca ,_ix8,   _ora  ,_ix8,   _adda ,_ix8,     /* A8..AB */
  _cpx  ,_ix8,   _jsr  ,_ix8,   _lds  ,_ix8,   _sts  ,_ix8,     /* AC..AF */
  _suba ,_ext,   _cmpa ,_ext,   _sbca ,_ext,   _ill  ,_nom,     /* B0..B3 */
  _anda ,_ext,   _bita ,_ext,   _lda  ,_ext,   _sta  ,_ext,     /* B4..B7 */
  _eora ,_ext,   _adca ,_ext,   _ora  ,_ext,   _adda ,_ext,     /* B8..BB */
  _cpx  ,_ext,   _jsr  ,_ext,   _lds  ,_ext,   _sts  ,_ext,     /* BC..BF */
  _subb ,_imb,   _cmpb ,_imb,   _sbcb ,_imb,   _ill  ,_nom,     /* C0..C3 */
  _andb ,_imb,   _bitb ,_imb,   _ldb  ,_imb,   _ill  ,_nom,     /* C4..C7 */
  _eorb ,_imb,   _adcb ,_imb,   _orb  ,_imb,   _addb ,_imb,     /* C8..CB */
  _ill  ,_nom,   _ill  ,_nom,   _ldx  ,_imw,   _ill  ,_nom,     /* CC..CF */
  _subb ,_dir,   _cmpb ,_dir,   _sbcb ,_dir,   _ill  ,_nom,     /* D0..D3 */
  _andb ,_dir,   _bitb ,_dir,   _ldb  ,_dir,   _stb  ,_dir,     /* D4..D7 */
  _eorb ,_dir,   _adcb ,_dir,   _orb  ,_dir,   _addb ,_dir,     /* D8..DB */
  _ill  ,_nom,   _ill  ,_nom,   _ldx  ,_dir,   _stx  ,_dir,     /* DC..DF */
  _subb ,_ix8,   _cmpb ,_ix8,   _sbcb ,_ix8,   _ill  ,_nom,     /* E0..E3 */
  _andb ,_ix8,   _bitb ,_ix8,   _ldb  ,_ix8,   _stb  ,_ix8,     /* E4..E7 */
  _eorb ,_ix8,   _adcb ,_ix8,   _orb  ,_ix8,   _addb ,_ix8,     /* E8..EB */
  _ill  ,_nom,   _ill  ,_nom,   _ldx  ,_ix8,   _stx  ,_ix8,     /* EC..EF */
  _subb ,_ext,   _cmpb ,_ext,   _sbcb ,_ext,   _ill  ,_nom,     /* F0..F3 */
  _andb ,_ext,   _bitb ,_ext,   _ldb  ,_ext,   _stb  ,_ext,     /* F4..F7 */
  _eorb ,_ext,   _adcb ,_ext,   _orb  ,_ext,   _addb ,_ext,     /* F8..FB */
  _ill  ,_nom,   _ill  ,_nom,   _ldx  ,_ext,   _stx  ,_ext,     /* FC..FF */
  };

char *Dasm6800::bit_r[] = {"CC","A","B","??"};

char *Dasm6800::block_r[] =
  {
  "D","X","Y","U","S","?","?","?","?","?","?","?","?","?","?","?"
  };

/*****************************************************************************/
/* opcodes : 6800 opcodes array for initialization                           */
/*****************************************************************************/

OpCode Dasm6800::opcodes[mnemo6800_count] =
  {
    { "???",   Data },                  /* _ill                              */
    { "ABA",   Data },                  /* _aba                              */
    { "ADCA",  Data },                  /* _adca                             */
    { "ADCB",  Data },                  /* _adcb                             */
    { "ADDA",  Data },                  /* _adda                             */
    { "ADDB",  Data },                  /* _addb                             */
    { "ANDA",  Data },                  /* _anda                             */
    { "ANDB",  Data },                  /* _andb                             */
    { "ASLA",  Data },                  /* _asla                             */
    { "ASLB",  Data },                  /* _aslb                             */
    { "ASL",   Data },                  /* _asl                              */
    { "ASRA",  Data },                  /* _asra                             */
    { "ASRB",  Data },                  /* _asrb                             */
    { "ASR",   Data },                  /* _asr                              */
    { "BCC",   Code },                  /* _bcc                              */
    { "BCS",   Code },                  /* _bcs                              */
    { "BEQ",   Code },                  /* _beq                              */
    { "BGE",   Code },                  /* _bge                              */
    { "BGT",   Code },                  /* _bgt                              */
    { "BHI",   Code },                  /* _bhi                              */
    { "BITA",  Data },                  /* _bita                             */
    { "BITB",  Data },                  /* _bitb                             */
    { "BLE",   Code },                  /* _ble                              */
    { "BLS",   Code },                  /* _bls                              */
    { "BLT",   Code },                  /* _blt                              */
    { "BMI",   Code },                  /* _bmi                              */
    { "BNE",   Code },                  /* _bne                              */
    { "BPL",   Code },                  /* _bpl                              */
    { "BRA",   Code },                  /* _bra                              */
    { "BSR",   Code },                  /* _bsr                              */
    { "BVC",   Code },                  /* _bvc                              */
    { "BVS",   Code },                  /* _bvs                              */
    { "CBA",   Data },                  /* _cba                              */
    { "CLI",   Data },                  /* _cli                              */
    { "CLRA",  Data },                  /* _clra                             */
    { "CLRB",  Data },                  /* _clrb                             */
    { "CLR",   Data },                  /* _clr                              */
    { "CLC",   Data },                  /* _clc                              */
    { "CLV",   Data },                  /* _clv                              */
    { "CMPA",  Data },                  /* _cmpa                             */
    { "CMPB",  Data },                  /* _cmpb                             */
    { "COMA",  Data },                  /* _coma                             */
    { "COMB",  Data },                  /* _comb                             */
    { "COM",   Data },                  /* _com                              */
    { "CPX",   Data },                  /* _cpx                              */
    { "DAA",   Data },                  /* _daa                              */
    { "DECA",  Data },                  /* _deca                             */
    { "DECB",  Data },                  /* _decb                             */
    { "DEC",   Data },                  /* _dec                              */
    { "DES",   Data },                  /* _des                              */
    { "DEX",   Data },                  /* _dex                              */
    { "EORA",  Data },                  /* _eora                             */
    { "EORB",  Data },                  /* _eorb                             */
    { "INCA",  Data },                  /* _inca                             */
    { "INCB",  Data },                  /* _incb                             */
    { "INC",   Data },                  /* _inc                              */
    { "INS",   Data },                  /* _ins                              */
    { "INX",   Data },                  /* _inx                              */
    { "JMP",   Code },                  /* _jmp                              */
    { "JSR",   Code },                  /* _jsr                              */
    { "LDAA",  Data },                  /* _lda                              */
    { "LDAB",  Data },                  /* _ldb                              */
    { "LDS",   Data },                  /* _lds                              */
    { "LDX",   Data },                  /* _ldx                              */
    { "LSRA",  Data },                  /* _lsra                             */
    { "LSRB",  Data },                  /* _lsrb                             */
    { "LSR",   Data },                  /* _lsr                              */
    { "NEGA",  Data },                  /* _nega                             */
    { "NEGB",  Data },                  /* _negb                             */
    { "NEG",   Data },                  /* _neg                              */
    { "NOP",   Data },                  /* _nop                              */
    { "ORAA",  Data },                  /* _ora                              */
    { "ORAB",  Data },                  /* _orb                              */
    { "PSHA",  Data },                  /* _psha                             */
    { "PSHB",  Data },                  /* _pshb                             */
    { "PULA",  Data },                  /* _pula                             */
    { "PULB",  Data },                  /* _pulb                             */
    { "ROLA",  Data },                  /* _rola                             */
    { "ROLB",  Data },                  /* _rolb                             */
    { "ROL",   Data },                  /* _rol                              */
    { "RORA",  Data },                  /* _rora                             */
    { "RORB",  Data },                  /* _rorb                             */
    { "ROR",   Data },                  /* _ror                              */
    { "RTI",   Data },                  /* _rti                              */
    { "RTS",   Data },                  /* _rts                              */
    { "SBA",   Data },                  /* _sba                              */
    { "SBCA",  Data },                  /* _sbca                             */
    { "SBCB",  Data },                  /* _sbcb                             */
    { "SEC",   Data },                  /* _sec                              */
    { "SEI",   Data },                  /* _sei                              */
    { "SEV",   Data },                  /* _sev                              */
    { "STAA",  Data },                  /* _sta                              */
    { "STAB",  Data },                  /* _stb                              */
    { "STS",   Data },                  /* _sts                              */
    { "STX",   Data },                  /* _stx                              */
    { "SUBA",  Data },                  /* _suba                             */
    { "SUBB",  Data },                  /* _subb                             */
    { "SWI",   Data },                  /* _swi                              */
    { "TAB",   Data },                  /* _tab                              */
    { "TAP",   Data },                  /* _tap                              */
    { "TBA",   Data },                  /* _tba                              */
    { "TPA",   Data },                  /* _tpa                              */
    { "TSTA",  Data },                  /* _tsta                             */
    { "TSTB",  Data },                  /* _tstb                             */
    { "TST",   Data },                  /* _tst                              */
    { "TSX",   Data },                  /* _tsx                              */
    { "TXS",   Data },                  /* _txs                              */
    { "WAI",   Data },                  /* _wai                              */
    // Convenience mnemonics
    { "ASLD",  Data },                  /* _asld                             */
    { "LSRD",  Data },                  /* _lsrd                             */
  };

/*****************************************************************************/
/* Dasm6800 : constructor                                                    */
/*****************************************************************************/

Dasm6800::Dasm6800(void)
{
codes = m6800_codes;
useConvenience = true;
showIndexedModeZeroOperand = false;
useFCC = true;
#if RB_VARIANT
forceExtendedAddr = false;
forceDirectAddr = false;
closeCC = true;
#else
forceExtendedAddr = true;
forceDirectAddr = true;
closeCC = false;
commentStart = "*";
#endif

mnemo.resize(mnemo6800_count);          /* set up mnemonics table            */
for (int i = 0; i < mnemo6800_count; i++)
  mnemo[i] = opcodes[i];

// set up options table
// class uses one generic option setter/getter pair (not mandatory)
AddOption("conv", "{off|on}\tUse convenience macros",
          static_cast<PSetter>(&Dasm6800::Set6800Option),
          static_cast<PGetter>(&Dasm6800::Get6800Option));
AddOption("showzero", "{off|on}\tdo not omit indexed-mode operands of $00",
          static_cast<PSetter>(&Dasm6800::Set6800Option),
          static_cast<PGetter>(&Dasm6800::Get6800Option));
AddOption("closecc", "{off|on}\tadd closing delimiter to char constants",
          static_cast<PSetter>(&Dasm6800::Set6800Option),
          static_cast<PGetter>(&Dasm6800::Get6800Option));
AddOption("fcc", "{off|on}\tuse FCC to define data",
          static_cast<PSetter>(&Dasm6800::Set6800Option),
          static_cast<PGetter>(&Dasm6800::Get6800Option));
}

/*****************************************************************************/
/* ~Dasm6800 : destructor                                                    */
/*****************************************************************************/

Dasm6800::~Dasm6800(void)
{
}

/*****************************************************************************/
/* Set6800Option : sets a disassembler option                                */
/*****************************************************************************/

int Dasm6800::Set6800Option(std::string lname, std::string value)
{
std::string lvalue(lowercase(value));
int bnvalue = (lvalue == "off") ? 0 : (lvalue == "on") ? 1 : atoi(value.c_str());

if (lname.substr(0, 2) == "no")         /* obviously a boolean negation      */
  {
  lname = lname.substr(2);              /* skip the "no"                     */
  bnvalue = !bnvalue;                   /* and invert the value              */
  }
if (lname == "conv")
  useConvenience = !!bnvalue;
else if (lname == "showzero")
  showIndexedModeZeroOperand = !!bnvalue;
else if (lname == "closecc")
  closeCC = !!bnvalue;
else if (lname == "fcc")
  useFCC = !!bnvalue;
else
  return 0;                             /* only name consumed                */
return 1;                               /* name and value consumed           */
}

/*****************************************************************************/
/* Get6800Option : retrieves a disassembler option                           */
/*****************************************************************************/

std::string Dasm6800::Get6800Option(std::string lname)
{
if (lname == "conv")
  return useConvenience ? "on" : "off";
else if (lname == "showzero")
  return showIndexedModeZeroOperand ? "on" : "off";
else if (lname == "closecc")
  return closeCC ? "on" : "off";
else if (lname == "fcc")
  return useFCC ? "on" : "off";

return "";
}

/*****************************************************************************/
/* SFlexRecord : a record in a binary FLEX(9) disk file                      */
/*****************************************************************************/

#pragma pack(1)
struct SFlexRecord
  {
  uint8_t bSOI;                         /* start of record indicator         */
  uint8_t bLoadAddress[2];              /* Hi/Lo byte of load address        */
  uint8_t bDataLen;                     /* length of data record             */
  uint8_t bData[255];                   /* data record                       */

  int IsTransferAddress() { return (bSOI == 0x16); }
  int IsRecord()          { return (bSOI == 0x02); }
  int GetSize()           { return bDataLen; }
  addr_t GetLoadAddress() { return (((addr_t)(bLoadAddress[0])) << 8) | bLoadAddress[1]; }
  uint8_t *GetData()      { return bData; }
  };
#pragma pack()

/*****************************************************************************/
/* ReadFlexRecord : read one record of a FLEX9 binary                        */
/*****************************************************************************/

static bool ReadFlexRecord(FILE *f, SFlexRecord *pRecord)
{
int nCurPos = ftell(f);
uint8_t bCur = 0;
int i;

while (!bCur)                           /* read 1st byte, skipping 0 bytes   */
  if (!fread(&bCur, 1, 1, f))
    return false;
switch (bCur)                           /* OK, so what's that?               */
  {
  case 0x02 :                           /* Start of Record Indicator ?       */
    {
    pRecord->bSOI = bCur;
    if ((!fread(pRecord->bLoadAddress, 2, 1, f)) ||
        (!fread(&pRecord->bDataLen, 1, 1, f)))
      return false;
    for (i = 0; i < pRecord->bDataLen; i++)
      if (!fread(&pRecord->bData[i], 1, 1, f))
        return false;
    }
    break;
  case 0x16 :                           /* Transfer Address ?                */
    pRecord->bSOI = bCur;
    if (!fread(pRecord->bLoadAddress, 2, 1, f))
      return false;
    break;
  default :
    fseek(f, nCurPos, SEEK_SET);        /* seek back                         */
    return false;
  }
return true;
}

/*****************************************************************************/
/* LoadFlex : loads a FLEX(9) binary executable                              */
/*****************************************************************************/

bool Dasm6800::LoadFlex(FILE *f, std::string &sLoadType)
{
struct SFlexRecord rec;
int nCurPos = ftell(f);
int nRecs = 0;
bool bExecutable = false;
addr_t fbegin = GetHighestCodeAddr(), fend = GetLowestCodeAddr();
addr_t i;

while (ReadFlexRecord(f, &rec))
  {
  addr_t nStart = rec.GetLoadAddress();
  addr_t nEnd = nStart + rec.GetSize() - 1;

  nRecs++;
  if (nStart < fbegin)
    fbegin = nStart;
  if (nEnd > fend)
    fend = nEnd;
  if (rec.IsRecord() && rec.GetSize())
    {
    AddMemory(nStart, rec.GetSize(), Code, rec.GetData());
    for (i = nStart; i <= nEnd; i++)    /* mark area as used                 */
      {
      SetCellUsed(i);
      SetDisplay(i, defaultDisplay);
      }
    }
  else if (rec.IsTransferAddress())
    {
    bExecutable = true;
    load = rec.GetLoadAddress();
    }
  }

if (fgetc(f) != EOF)                    /* if not read through the whole file*/
  {
  for (i = fbegin; i <= fend; i++)      /* mark area as UNused               */
    SetCellUsed(i, false);
  nRecs = 0;                            /* this ain't no valid FLEX file     */
  }

fseek(f, nCurPos, SEEK_SET);            /* reset position for next filetype  */
if (nRecs > 0)
  {
  if (fbegin < begin)
    begin = fbegin;
  if (fend > end)
    end = fend;
  sLoadType = "FLEX";
  }
return (nRecs > 0);
}

/*****************************************************************************/
/* LoadFile : loads an opened file                                           */
/*****************************************************************************/

bool Dasm6800::LoadFile(std::string filename, FILE *f, std::string &sLoadType, int interleave)
{
return LoadFlex(f, sLoadType) ||  // FLEX9 files need no interleave
       Disassembler::LoadFile(filename, f, sLoadType, interleave);
}

/*****************************************************************************/
/* String2Number : convert a string to a number in all known formats         */
/*****************************************************************************/

bool Dasm6800::String2Number(std::string s, addr_t &value)
{
/* Standard formats for known 680x assemblers :
   - a character has a leading '
     and may be followed by a (n unchecked) closing '
   - a binary has a leading %
   - an octal constant has a leading @
   - a hex constant has a leading $
*/
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
  for (std::string::size_type i = 1; i < s.size(); i++)
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

std::string Dasm6800::Number2String(addr_t value, int nDigits, addr_t addr, bool bDataBus)
{
std::string s;

/* Standard formats for known 680x assemblers :
   - a character has a leading '
     and may be followed by a (n unchecked) closing '
   - a binary has a leading %
   - an octal constant has a leading @
   - a hex constant has a leading $
*/

MemoryType memType = GetMemType(addr);
MemAttribute::Display disp;
bool bSigned = false;
if (memType == MemAttribute::CellUntyped)
  {
#pragma message("Define default type!")
  disp = MemAttribute::Hex;
  }
else
  {
  disp = GetDisplay(addr);
  bSigned = IsSigned(addr);
  }

if ((nDigits == 2) &&                   /* if 2-digit value                  */
    (disp == MemAttribute::Char))       /* and character output requested    */
  {
  if (isprint(value))
    s = sformat("'%c%s", value, closeCC ? "'" : "");
  else
    s = sformat("$%02x", value);
  }
else if (disp == MemAttribute::Binary)  /* if a binary                       */
  {
  int nBit;

  nDigits *= 4;                         /* convert from digits to bits       */
  s = '%';                              /* prepare a binary value            */
                                        /* now do for all bits               */
  for (nBit = nDigits - 1; nBit >= 0; nBit--) 
    s.push_back('0' + (!!(value & (1 << nBit))));
  }
else if (disp == MemAttribute::Hex)     /* if hex                            */
  s = sformat("$%0*X", nDigits, value); /* prepare a hex value               */
else if (disp == MemAttribute::Octal)   /* if octal display                  */
  s = sformat("@%0*o", (nDigits * 4) + 2 / 3, value);
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

bool Dasm6800::InitParse(bool bDataBus)
{
if (!bDataBus)
  {
  // set up IRQ-RST system vectors
  static const char *vectbl[] =
    {
    "IRQ",                              /* fff8                              */
    "SWI",                              /* fffa                              */
    "NMI",                              /* fffc                              */
    "RST"                               /* fffe                              */
    };
  for (addr_t addr = 0xfff8; addr <= GetHighestCodeAddr(); addr+= 2)
    {
    if (GetMemType(addr) != Untyped)    /* if system vector loaded           */
      {
      SetMemType(addr, Data);           /* that's a data word                */
      SetCellSize(addr, 2);
      addr_t tgtaddr = GetUWord(addr);  /* look whether it points to loaded  */
      if (GetMemType(tgtaddr) != Untyped)
        {                               /* if so,                            */
        SetMemType(tgtaddr, Code);      /* that's code there                 */
        AddLabel(tgtaddr, Code,         /* and it got a predefined label     */
                 sformat("vec_%s", vectbl[(addr - 0xfff8) / 2]),
                 true);
        }
      }
    }
  }
return Disassembler::InitParse(bDataBus);
}

/*****************************************************************************/
/* ParseData : parse data at given memory address for labels                 */
/*****************************************************************************/

addr_t Dasm6800::ParseData
    (
    addr_t addr,
    bool bDataBus                       /* ignored for 6800 and derivates    */
    )
{
int csz = GetCellSize(addr);
if (csz == 2)                           /* if WORD data                      */
  {
  if (!IsConst(addr))
    {
    addr_t wo = GetUWord(addr);
    Label *pLabel = FindLabel(wo);      /* if there's a label for the addr   */
    if (pLabel)
      pLabel->SetUsed();                /* mark it as used                   */
    }
  }
return csz;
}

/*****************************************************************************/
/* ParseCode : parse instruction at given memory address for labels          */
/*****************************************************************************/

addr_t Dasm6800::ParseCode
    (
    addr_t addr,
    bool bDataBus                       /* ignored for 6800 and derivates    */
    )
{
uint8_t O, T, M;
uint16_t W;
int MI;
const char *I;
addr_t PC = addr;
bool bSetLabel;

O = T = GetUByte(PC++);
W = (uint16_t)(T * 2);
MI = T = codes[W++];
I = mnemo[T].mne;
M = codes[W];

switch (M)                              /* which mode is this ?              */
  {
  case _nom:                            /* no mode                           */
    PC = addr + 1;
    break;

  case _imp:                            /* inherent/implied                  */
    break;

  case _imb:                            /* immediate byte                    */
    PC++;
    break;

  case _imw:                            /* immediate word                    */
    bSetLabel = !IsConst(PC);
    W = GetUWord(PC);
    if (bSetLabel)
      {
      W = (uint16_t)PhaseInner(W, PC);
      AddLabel(W, mnemo[MI].memType, "", true);
      }
    PC += 2;
    break;

  case _dir:                            /* direct                            */
    bSetLabel = !IsConst(PC);
    T = GetUByte(PC);
    if (bSetLabel)
      {
      W = T;  // on 6800, dp=0
      W = (uint16_t)PhaseInner(W, PC);
      AddLabel(W, mnemo[MI].memType, "", true);
      }
    PC++;
    break;

  case _ext:                            /* extended                          */
    bSetLabel = !IsConst(PC);
    if (bSetLabel)
      {
      uint16_t ow = GetUWord(PC);
      W = (uint16_t)PhaseInner(ow, PC);
      AddLabel(W, mnemo[MI].memType, "", true);
      }
    PC += 2;
    break;
    
  case _ix8:                            /* indexed for 6800 (unsigned)       */
    PC++;
    break;

  case _reb:                            /* relative byte                     */
    bSetLabel = !IsConst(PC);
    T = GetUByte(PC); PC++;
    W = (uint16_t)(PC + (signed char)T);
    if (bSetLabel)
      {
      W = (uint16_t)DephaseOuter(W, PC - 1);
      AddLabel(W, mnemo[MI].memType, "", true);
      }
    break;
    
  }

return PC - addr;                       /* pass back # processed bytes       */
}

/*****************************************************************************/
/* DisassembleLabel : disassemble used external labels                       */
/*****************************************************************************/

bool Dasm6800::DisassembleLabel
    (
    addr_t addr,
    std::string &smnemo,
    std::string &sparm,
    bool bDataBus
    )
{
Label *pLabel;
std::string s;

if (/* !IsCellUsed(addr) && */ // to be done externally!
    (pLabel = FindLabel(addr)) != NULL &&
    pLabel->IsUsed() &&
    pLabel->GetText().find_first_of("+-") == std::string::npos)
  {
  smnemo = "EQU";
  sparm = sformat("$%04X", addr);
  return true;
  }
return false;
}

/*****************************************************************************/
/* DisassembleData : disassemble data area at given memory address           */
/*****************************************************************************/

addr_t Dasm6800::DisassembleData
    (
    addr_t addr,
    addr_t end,
    uint32_t flags,
    std::string &smnemo,
    std::string &sparm,
    int maxparmlen,
    bool bDataBus                       /* ignored for 6800 and derivates    */
    )
{
std::string::size_type max = 24; //(nComment ? 24 : 52);
addr_t done;

if (flags & SHMF_RMB)                   /* if reserved memory block          */
  {
  done = end;                           /* remember it's done completely     */
  smnemo = "RMB";
  sparm = Number2String(end - addr, 4, addr);
  }
else if (useFCC && (flags & SHMF_TXT))  /* if FCC (text) allowed             */
  {
  smnemo = "FCC";
  sparm = '"';                          /* start the game                    */
  for (done = addr; done < end; done++) /* assemble as many as possible      */
    {                                   /* if this would become too long     */
    if (sparm.size() + 2 > (std::string::size_type)maxparmlen)
      break;                            /* terminate the loop                */
    sparm += *getat(done);
    }
  sparm += '"';                         /* append delimiter                  */
  }
else if (flags & 0xff)                  /* if not byte-sized                 */
  {
  // 680x can ony do byte and word, so assume word entities
  smnemo = "FDB";
                                        /* assemble as many as possible      */
  for (done = addr; done < end; done += 2)
    {
    std::string s = Label2String(GetUWord(done), !IsConst(done), done);
    if (sparm.size())                   /* if already something there        */
      {                                 /* if this would become too long     */
      if (sparm.size() + s.size() + 1 > (std::string::size_type)maxparmlen)
        break;                          /* terminate the loop                */
      sparm += ',';                     /* add separator                     */
      }
    sparm += s;                         /* append the byte's representation  */
    }
  }
else                                    /* if FCB (hex or binary)            */
  {
  smnemo = "FCB";
                                        /* assemble as many as possible      */
  for (done = addr; done < end; done++)
    {
    std::string s = Number2String(*getat(done), 2, done);
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
/* DisassembleCode : disassemble code instruction at given memory address    */
/*****************************************************************************/

addr_t Dasm6800::DisassembleCode
    (
    addr_t addr,
    std::string &smnemo,
    std::string &sparm,
    bool bDataBus                       /* ignored for 6800 and derivates    */
    )
{
uint8_t O, T, M;
uint16_t W;
addr_t Wrel;
const char *I;
addr_t PC = addr;
bool bGetLabel;

O = T = GetUByte(PC++);
W = (uint16_t)(T * 2);
T = codes[W++];
I = mnemo[T].mne;
M = codes[W];

smnemo = I;                             /* initialize mnemonic               */

switch (M)                              /* which mode is this?               */
  {
  case _nom:                            /* no mode                           */
    smnemo = "FCB";
    sparm = Number2String(O, 2, PC++);
    break;

  case _imp:                            /* inherent/implied                  */
    if (useConvenience &&
        !IsLabel(PC))                   /* not if 2nd byte has a label!      */
      {
      switch ((uint16_t)(O << 8) | GetUByte(PC))
        {
        case 0x4456 :                   /* LSRA + RORB -> LSRD               */
          smnemo = mnemo[_lsrd].mne; PC++;
          break;
        case 0x5849 :                   /* ASLB + ROLA -> ASLD               */
          smnemo = mnemo[_asld].mne; PC++;
          break;
        }
      }
    break;

  case _imb:                            /* immediate byte                    */
    T = GetUByte(PC++);
    sparm = "#" + Number2String(T, 2, PC - 1);
    break;

  case _imw:                            /* immediate word                    */
    bGetLabel = !IsConst(PC);
    W = GetUWord(PC);
    PC += 2;
    if (bGetLabel)
      W = (uint16_t)PhaseInner(W, PC - 2);
    sparm = "#" + Label2String(W, bGetLabel, PC - 2);
    break;

  case _dir:                            /* direct                            */
    bGetLabel = !IsConst(PC);
    T = GetUByte(PC++);
    W = (uint16_t)T;
    if (bGetLabel)
      W = (uint16_t)PhaseInner(W, PC - 1);
    sparm = Label2String(W, bGetLabel, PC - 1);
    break;

  case _ext:                            /* extended                          */
    bGetLabel = !IsConst(PC);
    W = (uint16_t)PhaseInner(GetUWord(PC), PC);
    PC += 2;
    if (forceExtendedAddr && (W & (uint16_t)0xff00) == 0)
      sparm = ">" + Label2String(W, bGetLabel, PC - 2);
    else
      sparm = Label2String(W, bGetLabel, PC - 2);
    break;
    
  case _ix8:                            /* indexed for 6800 (unsigned)       */
    bGetLabel = !IsConst(PC);
    T = GetUByte(PC++);
    Wrel = GetRelative(PC - 1);
    if (Wrel)
      {
      W = (int)((unsigned char)T) + (uint16_t)Wrel;
      sparm = Label2String((addr_t)((int)((unsigned char)T)), bGetLabel, PC - 1) + ",X";
      }
    else if (!T && !showIndexedModeZeroOperand)
      sparm = ",X";   /* omit '$00', unless the user has set the 'showzero' option */
    else
      sparm = Number2String(T, 2, PC - 1) + ",X";
    break;

  case _reb:                            /* relative byte                     */
    bGetLabel = !IsConst(PC);
    T = GetUByte(PC++);
    if (bGetLabel)
      {
      W = (uint16_t)(PC + (int8_t)T);
      W = (uint16_t)DephaseOuter(W, PC - 1);
      sparm = Label2String(W, bGetLabel, PC - 1);
      }
    else
      {
      int nDiff = (int)(int8_t)T;
      sparm = "*";
      if (nDiff >= 0)
        sparm += "+";
      sparm += SignedNumber2String(nDiff + 2, 2, PC - 1);
      }
    break;
  }
return PC - addr;                       /* pass back # processed bytes       */
}

/*****************************************************************************/
/* DisassembleChanges : report dasm-specific state changes before/after addr */
/*****************************************************************************/

bool Dasm6800::DisassembleChanges
    (
    addr_t addr,
    addr_t prevaddr,
    addr_t prevsz,
    bool bAfterLine,
    std::vector<LineChange> &changes,
    bool bDataBus
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
    chg.smnemo = "END";
    if (load != NO_ADDRESS &&           /* if entry point address given      */
        bLoadLabel)                     /* and labelling wanted              */
      chg.sparm = Label2String(load, true, load);
    changes.push_back(chg);
    }
  }
else if (!bDataBus)                     /* no Harvard architecture here.     */
  {
  addr_t org = DephaseOuter(addr, addr);
  addr_t prevorg = DephaseOuter(prevaddr, prevaddr);
  if (addr != prevaddr + prevsz)
    {
    if (!bAfterLine)
      {
      TMemory<addr_t, addr_t> *curPhArea  = FindPhase(addr);
      TMemory<addr_t, addr_t> *prevPhArea = FindPhase(prevaddr);

      addr_t prevphase = prevPhArea ? prevPhArea->GetType() : NO_ADDRESS;
      addr_t prevphstart = prevPhArea ? prevPhArea->GetStart() : NO_ADDRESS;
      addr_t curphase = curPhArea ? curPhArea->GetType() : NO_ADDRESS;
      addr_t curphstart = curPhArea ? curPhArea->GetStart() : NO_ADDRESS;
      LineChange chg;
      changes.push_back(chg);
      if (prevphase != NO_ADDRESS && prevphstart != curphstart)
        {
        chg.smnemo = "DEPHASE";
        changes.push_back(chg);
        changes.push_back(LineChange());
        }
      if (addr != NO_ADDRESS)
        {
        chg.smnemo = "ORG";
        chg.sparm = Number2String(addr, 4, NO_ADDRESS);
        changes.push_back(chg);
        if (curphase != NO_ADDRESS &&
            prevphstart != curphstart
// uncomment this to remove superfluous PHASE statements
//          && curphase != addr
            )
          {
          chg.smnemo = "PHASE";
          chg.sparm = Number2String(curphase, 4, NO_ADDRESS);
          changes.push_back(chg);
          }
        changes.push_back(LineChange());
        }
      }
    }
  }

return Disassembler::DisassembleChanges(addr, prevaddr, prevsz, bAfterLine, changes, bDataBus);
}
