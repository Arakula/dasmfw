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
 ***************************************************************************/

/*****************************************************************************/
/* Dasm650X.cpp : 650X disassembler implementation                           */
/*****************************************************************************/

#include "Dasm6500.h"

/*****************************************************************************/
/* Create6501 : create an 6501 processor handler                             */
/*****************************************************************************/

static Disassembler *Create6501(Application *pApp)
{
Disassembler *pDasm = new Dasm6501(pApp);
if (pDasm) pDasm->Setup();
return pDasm;
}

/*****************************************************************************/
/* Create6503 : create an 6503 processor handler                             */
/*****************************************************************************/

static Disassembler *Create6503(Application *pApp)
{
Disassembler *pDasm = new Dasm6503(pApp);
if (pDasm) pDasm->Setup();
return pDasm;
}

/*****************************************************************************/
/* Create6504 : create an 6504 processor handler                             */
/*****************************************************************************/

static Disassembler *Create6504(Application *pApp)
{
Disassembler *pDasm = new Dasm6504(pApp);
if (pDasm) pDasm->Setup();
return pDasm;
}

/*****************************************************************************/
/* Create65C02 : create an 65C02 processor handler                           */
/*****************************************************************************/

static Disassembler *Create65C02(Application *pApp)
{
Disassembler *pDasm = new Dasm65C02(pApp);
if (pDasm) pDasm->Setup();
return pDasm;
}

/*****************************************************************************/
/* Auto-registration                                                         */
/*****************************************************************************/

static bool bRegistered[] =
  {
  RegisterDisassembler("6501",  Create6501),
  RegisterDisassembler("7501",  Create6501),
  RegisterDisassembler("6502",  Create6501),
  RegisterDisassembler("6503",  Create6503),
  RegisterDisassembler("6504",  Create6504),
  RegisterDisassembler("6505",  Create6503),
  RegisterDisassembler("6506",  Create6503),
  RegisterDisassembler("6507",  Create6504),
  RegisterDisassembler("65c02", Create65C02),
  };


/*===========================================================================*/
/* Dasm650X class members                                                    */
/*===========================================================================*/

/*****************************************************************************/
/* m6500_codes : table of all 6500 instruction bytes and types               */
/*****************************************************************************/

uint8_t Dasm650X::m6500_codes[512] =
  {
  _brk  ,_imp,   _ora  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* 00..03 */
  _ill  ,_nom,   _ora  ,_zpg,   _asl  ,_zpg,   _ill  ,_nom,     /* 04..07 */
  _php  ,_imp,   _ora  ,_imm,   _asl  ,_acc,   _ill  ,_nom,     /* 08..0B */
  _ill  ,_nom,   _ora  ,_abs,   _asl  ,_abs,   _ill  ,_nom,     /* 0C..0F */
  _bpl  ,_rel,   _ora  ,_idy,   _ill  ,_nom,   _ill  ,_nom,     /* 10..13 */
  _ill  ,_nom,   _ora  ,_zpx,   _asl  ,_zpx,   _ill  ,_nom,     /* 14..17 */
  _clc  ,_imp,   _ora  ,_aby,   _ill  ,_nom,   _ill  ,_nom,     /* 18..1B */
  _ill  ,_nom,   _ora  ,_abx,   _asl  ,_abx,   _ill  ,_nom,     /* 1C..1F */
  _jsr  ,_abs,   _and  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* 20..23 */
  _bit  ,_zpg,   _and  ,_zpg,   _rol  ,_zpg,   _ill  ,_nom,     /* 24..27 */
  _plp  ,_imp,   _and  ,_imm,   _rol  ,_acc,   _ill  ,_nom,     /* 28..2B */
  _bit  ,_abs,   _and  ,_abs,   _rol  ,_abs,   _ill  ,_nom,     /* 2C..2F */
  _bmi  ,_rel,   _and  ,_idy,   _ill  ,_nom,   _ill  ,_nom,     /* 30..33 */
  _ill  ,_nom,   _and  ,_zpx,   _rol  ,_zpx,   _ill  ,_nom,     /* 34..37 */
  _sec  ,_imp,   _and  ,_aby,   _ill  ,_nom,   _ill  ,_nom,     /* 38..3B */
  _ill  ,_nom,   _and  ,_abx,   _rol  ,_abx,   _ill  ,_nom,     /* 3C..3F */
  _rti  ,_imp,   _eor  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* 40..43 */
  _ill  ,_nom,   _eor  ,_zpg,   _lsr  ,_zpg,   _ill  ,_nom,     /* 44..47 */
  _pha  ,_imp,   _eor  ,_imm,   _lsr  ,_acc,   _ill  ,_nom,     /* 48..4B */
  _jmp  ,_abs,   _eor  ,_abs,   _lsr  ,_abs,   _ill  ,_nom,     /* 4C..4F */
  _bvc  ,_rel,   _eor  ,_idy,   _ill  ,_nom,   _ill  ,_nom,     /* 50..53 */
  _ill  ,_nom,   _eor  ,_zpx,   _lsr  ,_zpx,   _ill  ,_nom,     /* 54..57 */
  _cli  ,_imp,   _eor  ,_aby,   _ill  ,_nom,   _ill  ,_nom,     /* 58..5B */
  _ill  ,_nom,   _eor  ,_abx,   _lsr  ,_abx,   _ill  ,_nom,     /* 5C..5F */
  _rts  ,_imp,   _adc  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* 60..63 */
  _ill  ,_nom,   _adc  ,_zpg,   _ror  ,_zpg,   _ill  ,_nom,     /* 64..67 */
  _pla  ,_imp,   _adc  ,_imm,   _ror  ,_acc,   _ill  ,_nom,     /* 68..6B */
  _jmp  ,_ind,   _adc  ,_abs,   _ror  ,_abs,   _ill  ,_nom,     /* 6C..6F */
  _bvs  ,_rel,   _adc  ,_idy,   _ill  ,_nom,   _ill  ,_nom,     /* 70..73 */
  _ill  ,_nom,   _adc  ,_zpx,   _ror  ,_zpx,   _ill  ,_nom,     /* 74..77 */
  _sei  ,_imp,   _adc  ,_aby,   _ill  ,_nom,   _ill  ,_nom,     /* 78..7B */
  _ill  ,_nom,   _adc  ,_abx,   _ror  ,_abx,   _ill  ,_nom,     /* 7C..7F */
  _ill  ,_nom,   _sta  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* 80..83 */
  _sty  ,_zpg,   _sta  ,_zpg,   _stx  ,_zpg,   _ill  ,_nom,     /* 84..87 */
  _dey  ,_imp,   _ill  ,_nom,   _txa  ,_imp,   _ill  ,_nom,     /* 88..8B */
  _sty  ,_abs,   _sta  ,_abs,   _stx  ,_abs,   _ill  ,_nom,     /* 8C..8F */
  _bcc  ,_rel,   _sta  ,_idy,   _ill  ,_nom,   _ill  ,_nom,     /* 90..93 */
  _sty  ,_zpx,   _sta  ,_zpx,   _stx  ,_zpy,   _ill  ,_nom,     /* 94..97 */
  _tya  ,_imp,   _sta  ,_aby,   _txs  ,_imp,   _ill  ,_nom,     /* 98..9B */
  _ill  ,_nom,   _sta  ,_abx,   _ill  ,_nom,   _ill  ,_nom,     /* 9C..9F */
  _ldy  ,_imm,   _lda  ,_idx,   _ldx  ,_imm,   _ill  ,_nom,     /* A0..A3 */
  _ldy  ,_zpg,   _lda  ,_zpg,   _ldx  ,_zpg,   _ill  ,_nom,     /* A4..A7 */
  _tay  ,_imp,   _lda  ,_imm,   _tax  ,_imp,   _ill  ,_nom,     /* A8..AB */
  _ldy  ,_abs,   _lda  ,_abs,   _ldx  ,_abs,   _ill  ,_nom,     /* AC..AF */
  _bcs  ,_rel,   _lda  ,_idy,   _ill  ,_nom,   _ill  ,_nom,     /* B0..B3 */
  _ldy  ,_zpx,   _lda  ,_zpx,   _ldx  ,_zpy,   _ill  ,_nom,     /* B4..B7 */
  _clv  ,_imp,   _lda  ,_aby,   _tsx  ,_imp,   _ill  ,_nom,     /* B8..BB */
  _ldy  ,_abx,   _lda  ,_abx,   _ldx  ,_aby,   _ill  ,_nom,     /* BC..BF */
  _cpy  ,_imm,   _cmp  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* C0..C3 */
  _cpy  ,_zpg,   _cmp  ,_zpg,   _dec  ,_zpg,   _ill  ,_nom,     /* C4..C7 */
  _iny  ,_imp,   _cmp  ,_imm,   _dex  ,_imp,   _ill  ,_nom,     /* C8..CB */
  _cpy  ,_abs,   _cmp  ,_abs,   _dec  ,_abs,   _ill  ,_nom,     /* CC..CF */
  _bne  ,_rel,   _cmp  ,_idy,   _ill  ,_nom,   _ill  ,_nom,     /* D0..D3 */
  _ill  ,_nom,   _cmp  ,_zpx,   _dec  ,_zpx,   _ill  ,_nom,     /* D4..D7 */
  _cld  ,_imp,   _cmp  ,_aby,   _ill  ,_nom,   _ill  ,_nom,     /* D8..DB */
  _ill  ,_nom,   _cmp  ,_abx,   _dec  ,_abx,   _ill  ,_nom,     /* DC..DF */
  _cpx  ,_imm,   _sbc  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* E0..E3 */
  _cpx  ,_zpg,   _sbc  ,_zpg,   _inc  ,_zpg,   _ill  ,_nom,     /* E4..E7 */
  _inx  ,_imp,   _sbc  ,_imm,   _nop  ,_imp,   _ill  ,_nom,     /* E8..EB */
  _cpx  ,_abs,   _sbc  ,_abs,   _inc  ,_abs,   _ill  ,_nom,     /* EC..EF */
  _beq  ,_rel,   _sbc  ,_idy,   _ill  ,_nom,   _ill  ,_nom,     /* F0..F3 */
  _ill  ,_nom,   _sbc  ,_zpx,   _inc  ,_zpx,   _ill  ,_nom,     /* F4..F7 */
  _sed  ,_imp,   _sbc  ,_aby,   _ill  ,_nom,   _ill  ,_nom,     /* F8..FB */
  _ill  ,_nom,   _sbc  ,_abx,   _inc  ,_abx,   _ill  ,_nom,     /* FC..FF */
  };

/*****************************************************************************/
/* opcodes : 6500 opcodes array for initialization                           */
/*****************************************************************************/

OpCode Dasm650X::opcodes[mnemo6500_count] =
  {
    { "???",   Data },                  /* _ill                              */

    { "ADC",   Data },                  /* _adc                              */
    { "AND",   Data },                  /* _and                              */
    { "ASL",   Data },                  /* _asl                              */
    { "BCC",   Code },                  /* _bcc                              */
    { "BCS",   Code },                  /* _bcs                              */
    { "BEQ",   Code },                  /* _beq                              */
    { "BIT",   Data },                  /* _bit                              */
    { "BMI",   Code },                  /* _bmi                              */
    { "BNE",   Code },                  /* _bne                              */
    { "BPL",   Code },                  /* _bpl                              */
    { "BRK",   Code },                  /* _brk                              */
    { "BVC",   Code },                  /* _bvc                              */
    { "BVS",   Code },                  /* _bvs                              */
    { "CLC",   Data },                  /* _clc                              */
    { "CLD",   Data },                  /* _cld                              */
    { "CLI",   Data },                  /* _cli                              */
    { "CLV",   Data },                  /* _clv                              */
    { "CMP",   Data },                  /* _cmp                              */
    { "CPX",   Data },                  /* _cpx                              */
    { "CPY",   Data },                  /* _cpy                              */
    { "DEC",   Data },                  /* _dec                              */
    { "DEX",   Data },                  /* _dex                              */
    { "DEY",   Data },                  /* _dey                              */
    { "EOR",   Data },                  /* _eor                              */
    { "INC",   Data },                  /* _inc                              */
    { "INX",   Data },                  /* _inx                              */
    { "INY",   Data },                  /* _iny                              */
    { "JMP",   Code },                  /* _jmp                              */
    { "JSR",   Code },                  /* _jsr                              */
    { "LDA",   Data },                  /* _lda                              */
    { "LDX",   Data },                  /* _ldx                              */
    { "LDY",   Data },                  /* _ldy                              */
    { "LSR",   Data },                  /* _lsr                              */
    { "NOP",   Data },                  /* _nop                              */
    { "ORA",   Data },                  /* _ora                              */
    { "PHA",   Data },                  /* _pha                              */
    { "PHP",   Data },                  /* _php                              */
    { "PLA",   Data },                  /* _pla                              */
    { "PLP",   Data },                  /* _plp                              */
    { "ROL",   Data },                  /* _rol                              */
    { "ROR",   Data },                  /* _ror                              */
    { "RTI",   Code },                  /* _rti                              */
    { "RTS",   Code },                  /* _rts                              */
    { "SBC",   Data },                  /* _sbc                              */
    { "SEC",   Data },                  /* _sec                              */
    { "SED",   Data },                  /* _sed                              */
    { "SEI",   Data },                  /* _sei                              */
    { "STA",   Data },                  /* _sta                              */
    { "STX",   Data },                  /* _stx                              */
    { "STY",   Data },                  /* _sty                              */
    { "TAX",   Data },                  /* _tax                              */
    { "TAY",   Data },                  /* _tay                              */
    { "TSX",   Data },                  /* _tsx                              */
    { "TXA",   Data },                  /* _txa                              */
    { "TXS",   Data },                  /* _txs                              */
    { "TYA",   Data },                  /* _tya                              */
  };

/*****************************************************************************/
/* Dasm650X : constructor                                                    */
/*****************************************************************************/

Dasm650X::Dasm650X(Application *pApp)
  : Disassembler(pApp)
{
codes = m6500_codes;
useConvenience = true;
dirpage = 0;  // ... and that's all it can be for 650X
useFCC = true;
forceExtendedAddr = true;
forceDirectAddr = true;
closeCC = false;
useDPLabels = false;

mnemo.resize(mnemo6500_count);          /* set up mnemonics table            */
for (int i = 0; i < mnemo6500_count; i++)
  mnemo[i] = opcodes[i];

// set up options table
// class uses one generic option setter/getter pair (not mandatory)
#if 0
// none yet
AddOption("conv", "{off|on}\tUse convenience macros",
          static_cast<PSetter>(&Dasm650X::Set6500Option),
          static_cast<PGetter>(&Dasm650X::Get6500Option));
#endif
AddOption("closecc", "{off|on}\tadd closing delimiter to char constants",
          static_cast<PSetter>(&Dasm650X::Set6500Option),
          static_cast<PGetter>(&Dasm650X::Get6500Option));
AddOption("fcc", "{off|on}\tuse FCC to define data",
          static_cast<PSetter>(&Dasm650X::Set6500Option),
          static_cast<PGetter>(&Dasm650X::Get6500Option));
AddOption("dplabel", "{off|on}\tinterpret single-byte data as direct page labels",
          static_cast<PSetter>(&Dasm650X::Set6500Option),
          static_cast<PGetter>(&Dasm650X::Get6500Option));
AddOption("forceaddr", "{off|on}\tuse forced addressing where necessary",
          static_cast<PSetter>(&Dasm650X::Set6500Option),
          static_cast<PGetter>(&Dasm650X::Get6500Option));
}

/*****************************************************************************/
/* ~Dasm650X : destructor                                                    */
/*****************************************************************************/

Dasm650X::~Dasm650X(void)
{
}

/*****************************************************************************/
/* Set6500Option : sets a disassembler option                                */
/*****************************************************************************/

int Dasm650X::Set6500Option(string lname, string value)
{
bool bnValue = true;                    /* default to "on"                   */
bool bIsBool = ParseBool(value, bnValue);

if (lname == "conv")
  {
  useConvenience = bnValue;
  return bIsBool ? 1 : 0;
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
else if (lname == "dplabel")
  {
  useDPLabels = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (lname == "forceaddr")
  {
  forceExtendedAddr = forceDirectAddr = bnValue;
  return bIsBool ? 1 : 0;
  }
else
  return 0;                             /* only name consumed                */

// should never happen ... but better safe than sorry
return 1;                               /* name and value consumed           */
}

/*****************************************************************************/
/* Get6500Option : retrieves a disassembler option                           */
/*****************************************************************************/

string Dasm650X::Get6500Option(string lname)
{
if (lname == "conv")
  return useConvenience ? "on" : "off";
else if (lname == "closecc")
  return closeCC ? "on" : "off";
else if (lname == "fcc")
  return useFCC ? "on" : "off";
else if (lname == "dplabel")
  return useDPLabels ? "on" : "off";
else if (lname == "forceaddr")
  return (forceExtendedAddr || forceDirectAddr) ? "on" : "off";

return "";
}

/*****************************************************************************/
/* String2Number : convert a string to a number in all known formats         */
/*****************************************************************************/

bool Dasm650X::String2Number(string s, adr_t &value)
{
/* Standard formats for known 680x assemblers :
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

string Dasm650X::Number2String
    (
    adr_t value,
    int nDigits,
    adr_t addr,
    int bus
    )
{
(void)bus;

string s;

/* Standard formats for known 650x assemblers :
   - a character has a leading '
     and may be followed by a (n unchecked) closing '
   - a binary has a leading %
   - an octal constant has a leading @
   - a hex constant has a leading $
*/

MemAttribute::Type cellType = GetCellType(addr);
MemAttribute::Display disp;
bool bSigned = false;
if (cellType == MemAttribute::CellUntyped)
  disp = MemAttribute::DefaultDisplay;
else
  {
  disp = GetDisplay(addr);
  bSigned = IsSigned(addr);
  }

if (disp == MemAttribute::DefaultDisplay)
  disp = defaultDisplay;

if ((nDigits == 2) &&                   /* if 2-digit value                  */
    (disp == MemAttribute::Char))       /* and character output requested    */
  {
  value &= 0xff;
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
  {
  adr_t mask = 0;
  for (int i = 0; i < nDigits; i++)
    mask |= (0x0f << (i * 4));
  value &= mask;
  s = sformat("$%0*X", nDigits, value); /* prepare a hex value               */
  }
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

bool Dasm650X::InitParse(int bus)
{
if (bus == BusCode)
  {
  if (bSetSysVec)
    {
    // set up IRQ-RST system vectors
    static const char *vectbl[] =
      {
      "NMI",                            /* fffa                              */
      "RST",                            /* fffc                              */
      "IRQ"                             /* fffe                              */
      };
    adr_t vecStart = GetHighestCodeAddr() - 5;
    for (adr_t addr = vecStart; addr <= GetHighestCodeAddr(); addr+= 2)
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
                   sformat("vec_%s", vectbl[(addr - vecStart) / 2]),
                   true);
          }
        }
      }
    }
  }
return Disassembler::InitParse(bus);
}

/*****************************************************************************/
/* ParseData : parse data at given memory address for labels                 */
/*****************************************************************************/

adr_t Dasm650X::ParseData
    (
    adr_t addr,
    int bus                             /* ignored for 6500 and derivates    */
    )
{
SetLabelUsed(addr, Const);              /* mark DefLabels as used            */

int csz = GetCellSize(addr);
if (!IsConst(addr) && !IsBss(addr))
  {
  if (csz == 2)                         /* if WORD data                      */
    SetLabelUsed(GetUWord(addr), Code, BusCode, addr);
  else if (csz == 1 && useDPLabels)     /* or BYTE data and using DP labels  */
    {
    adr_t dp = GetDirectPage(addr, bus);
    if (dp != NO_ADDRESS)
      SetLabelUsed(dp | GetUByte(addr), Code, BusCode, addr);
    }
  }

return csz;
}

/*****************************************************************************/
/* FetchInstructionDetails : fetch details about current instruction         */
/*****************************************************************************/
// direct copy of 6800 code - maybe not all are necessary
adr_t Dasm650X::FetchInstructionDetails
    (
    adr_t PC,
    uint8_t &instpg,
    uint8_t &instb,
    uint8_t &mode,
    int &MI,
    const char *&I,
    string *smnemo
    )
{
uint16_t W;
instpg = 0;  // 650X has no instruction page
instb = GetUByte(PC++);
W = (uint16_t)instb * 2;
MI = codes[W++];
mode = codes[W];
I = mnemo[MI].mne;
if (smnemo)
  *smnemo = I;
return PC;
}

/*****************************************************************************/
/* ParseCode : parse instruction at given memory address for labels          */
/*****************************************************************************/

adr_t Dasm650X::ParseCode
    (
    adr_t addr,
    int bus                             /* ignored for 6500 and derivates    */
    )
{
uint8_t instpg, instb, T, mode;
uint16_t W;
int MI;
const char *I;
bool bSetLabel;
adr_t PC = FetchInstructionDetails(addr, instpg, instb, mode, MI, I);

switch (mode)                           /* which mode is this ?              */
  {
  case _nom:                            /* no mode                           */
    SetDefLabelUsed(PC, bus);
    PC = addr + 1;
    break;

  case _imp:                            /* inherent/implied                  */
  case _acc:                            /* accumulator                       */
    break;

  case _imm:                            /* immediate                         */
    SetDefLabelUsed(PC, bus);
    PC++;
    break;

  case _zpg:                            /* zero-page                         */
  case _zpx:                            /* zero-page,X                       */
  case _zpy:                            /* zero-page,Y                       */
  case _ind:                            /* indirect                          */
  case _idx:                            /* (indirect,X)                      */
  case _idy:                            /* (indirect),Y                      */
    bSetLabel = !IsConst(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);
    if (bSetLabel)
      {
      // this isn't really necessary for 6500, but maybe for derived classes
      adr_t dp = GetDirectPage(PC);
      if (dp == DEFAULT_ADDRESS || dp == NO_ADDRESS)
        dp = GetDirectPage(addr);
      if (dp == DEFAULT_ADDRESS)
        dp = 0;
      if (dp != NO_ADDRESS)
        {
        T = GetUByte(PC);
        W = (uint16_t)dp | T;
        W = (uint16_t)PhaseInner(W, PC);
        AddRelativeLabel(W, PC, mnemo[MI].memType, true, BusCode, addr);
        }
      }
    PC++;
    break;

  case _abs:                            /* absolute                          */
  case _abx:                            /* absolute,X                        */
  case _aby:                            /* absolute,Y                        */
    bSetLabel = !IsConst(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);
    SetCellSize(PC, 2);
    if (bSetLabel)
      {
      W = GetUWord(PC);
      W = (uint16_t)PhaseInner(W, PC);
      AddRelativeLabel(W, PC, mnemo[MI].memType, true, BusCode, addr);
      }
    PC += 2;
    break;

  case _rel:                            /* relative byte                     */
    bSetLabel = !IsConst(PC);
    // lbl = bSetLabel ? NULL : FindLabel(PC, Const, bus);
    T = GetUByte(PC); PC++;
    W = (uint16_t)(PC + (signed char)T);
    if (bSetLabel)
      {
      W = (uint16_t)DephaseOuter(W, PC - 1);
      AddRelativeLabel(W, PC - 1, mnemo[MI].memType, true, BusCode, addr);
      }
    break;
    
  }

return PC - addr;                       /* pass back # processed bytes       */
}

/*****************************************************************************/
/* DisassembleLabel : disassemble used external labels                       */
/*****************************************************************************/

bool Dasm650X::DisassembleLabel
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
    slabel = Label2String(laddr, 4, true, laddr, bus);
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

bool Dasm650X::DisassembleDefLabel
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
/* GetDisassemblyFlags : get flags for disassembly of data areas             */
/*****************************************************************************/

uint32_t Dasm650X::GetDisassemblyFlags(adr_t addr, int bus)
{
uint32_t flags = Disassembler::GetDisassemblyFlags(addr, bus);
if (flags & SHMF_TXT)
  {
  if (FindLabel(addr, Const, bus))
    flags &= ~SHMF_TXT;
  if (useDPLabels)
    {
    adr_t dp = GetDirectPage(addr, bus);
    if (dp != NO_ADDRESS)
      {
#if 0
      // activate this to match ALL direct page labels
      // only makes sense if data DEFINES the label, too
      if (!IsConst(addr, bus))
#else
      // activate this to only match DEFINED labels
      adr_t t = dp | GetUByte(addr);
      if (!IsConst(addr, bus) &&
          FindLabel(t, Untyped, bus))
#endif
        flags &= ~SHMF_TXT;
      }
    }
  }

return flags;
}

/*****************************************************************************/
/* DisassembleData : disassemble data area at given memory address           */
/*****************************************************************************/

adr_t Dasm650X::DisassembleData
    (
    adr_t addr,
    adr_t end,
    uint32_t flags,
    string &smnemo,
    string &sparm,
    int maxparmlen,
    int bus                             /* ignored for 6500 and derivates    */
    )
{
adr_t done;

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
    if (sparm.size() + 2 > (string::size_type)maxparmlen ||
        FindLabel(done, Const, bus))    /* or a DefLabel chimes in           */
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
    string s = Label2String(GetUWord(done), 4, !IsConst(done), done);
    if (sparm.size())                   /* if already something there        */
      {                                 /* if this would become too long     */
      if (sparm.size() + s.size() + 1 > (string::size_type)maxparmlen)
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
    Label *deflbl = FindLabel(done, Const, bus);
    adr_t dp = GetDirectPage(done, bus);
    string s;
    if (deflbl)
      s = deflbl->GetText();
    else if (useDPLabels && dp != NO_ADDRESS)
      s = Label2String(dp | GetUByte(done), 2, !IsConst(done), done);
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

adr_t Dasm650X::DisassembleCode
    (
    adr_t addr,
    string &smnemo,
    string &sparm,
    int bus                             /* ignored for 6500 and derivates    */
    )
{
uint8_t instpg, instb, T, mode;
uint16_t W;
int MI;
const char *I;
adr_t PC = FetchInstructionDetails(addr, instpg, instb, mode, MI, I, &smnemo);
bool bGetLabel;
Label *lbl;

switch (mode)                           /* which mode is this?               */
  {
  case _nom:                            /* no mode                           */
    smnemo = "FCB";
    lbl = FindLabel(PC, Const, bus);
    sparm = lbl ? lbl->GetText() : Number2String(instb, 2, PC);
    PC = addr + 1;
    break;

  case _imp:                            /* inherent/implied                  */
    // no need to do anything
    break;
  case _acc:                            /* accumulator                       */
    sparm = "A";
    break;

  case _imm:                            /* immediate byte                    */
    lbl = FindLabel(PC, Const, bus);
    T = GetUByte(PC);
    sparm = "#" + (lbl ? lbl->GetText() : Number2String(T, 2, PC));
    PC++;
    break;

  case _zpg:                            /* zero page                         */
  case _zpx:                            /* zero page,X                       */
  case _zpy:                            /* zero page,Y                       */
  case _ind:                            /* indirect                          */
  case _idx:                            /* (indirect,X)                      */
  case _idy:                            /* (indirect),Y                      */
    {
    bGetLabel = !IsConst(PC);
    lbl = bGetLabel ? NULL : FindLabel(PC, Const, bus);
    T = GetUByte(PC);
    adr_t dp = GetDirectPage(PC);
    if (dp == DEFAULT_ADDRESS || dp == NO_ADDRESS)
      dp = GetDirectPage(addr);
    if (dp == DEFAULT_ADDRESS)
      dp = 0;
    if (dp != NO_ADDRESS)
      {
      W = (uint16_t)dp | T;
      if (bGetLabel)
        W = (uint16_t)PhaseInner(W, PC);
      sparm = GetForcedAddr(PC) ? "<" : "";
      sparm += lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
      }
    else // if no direct page, this can't be interpreted as a label
      sparm = (lbl ? lbl->GetText() : Number2String(T, 2, PC));
    if (mode == _zpx)
      sparm += ",X";
    else if (mode == _zpy)
      sparm += ",Y";
    else if (mode == _ind)
      sparm = "(" + sparm + ")";
    else if (mode == _idx)
      sparm = "(" + sparm + ",X)";
    else if (mode == _idy)
      sparm = "(" + sparm + "),Y";
    PC++;
    }
    break;

  case _abs:                            /* absolute                          */
  case _abx:                            /* absolute,X                        */
  case _aby:                            /* absolute,Y                        */
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
    if (forceExtendedAddr && (W & (uint16_t)0xff00) == (uint16_t)dp)
      sparm = ">" + slbl;
    else
      sparm = slbl;
    if (mode == _abx)
      sparm += ",X";
    else if (mode == _aby)
      sparm += ",Y";
    PC += 2;
    }
    break;
    
  case _rel:                            /* relative                          */
    bGetLabel = !IsConst(PC);
    lbl = bGetLabel ? NULL : FindLabel(PC, Const, bus);
    T = GetUByte(PC++);
    if (bGetLabel)
      {
      W = (uint16_t)(PC + (int8_t)T);
      W = (uint16_t)DephaseOuter(W, PC - 1);
      sparm = Label2String(W, 4, bGetLabel, PC - 1);
      }
    else
      {
      int nDiff = (int)(int8_t)T;
      sparm = "*";
      if (nDiff >= 0 || lbl)
        sparm += "+";
      sparm += lbl ? lbl->GetText() : SignedNumber2String(nDiff + 2, 2, PC - 1);
      }
    break;
  }
return PC - addr;                       /* pass back # processed bytes       */
}

/*****************************************************************************/
/* DisassembleChanges : report dasm-specific state changes before/after addr */
/*****************************************************************************/

bool Dasm650X::DisassembleChanges
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
      chg.opnds = Label2String(load, 4, true, load);
    changes.push_back(chg);
    }
  }
else // no bus check necessary, there's only one
  {
  adr_t org = DephaseOuter(addr, addr);
  adr_t prevorg = DephaseOuter(prevaddr, prevaddr);
  (void)org;(void)prevorg;  // unused ATM
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
        chg.opnds = Number2String(addr, 4, NO_ADDRESS);
        changes.push_back(chg);
        if (curphase != NO_ADDRESS &&
            prevphstart != curphstart
// uncomment this to remove superfluous PHASE statements
//          && curphase != addr
            )
          {
          chg.oper = MnemoCase("PHASE");
          chg.opnds = Number2String(curphase, 4, NO_ADDRESS);
          changes.push_back(chg);
          }
        changes.push_back(LineChange());
        }
      }
    }
  }

return Disassembler::DisassembleChanges(addr, prevaddr, prevsz, bAfterLine, changes, bus);
}


/*===========================================================================*/
/* Dasm6501 class members                                                    */
/*===========================================================================*/

/*****************************************************************************/
/* m6501_codes : table of all 6501 instruction bytes and types               */
/*****************************************************************************/
// taken from https://wiki.nesdev.com/w/index.php/CPU_unofficial_opcodes
uint8_t Dasm6501::m6501_codes[512] =
  {
  _brk  ,_imp,   _ora  ,_idx,   _ill  ,_nom,   _slo,_idx|_und,  /* 00..03 */
  _ill  ,_nom,   _ora  ,_zpg,   _asl  ,_zpg,   _slo,_zpg|_und,  /* 04..07 */
  _php  ,_imp,   _ora  ,_imm,   _asl  ,_acc,   _ill  ,_nom,     /* 08..0B */
  _ill  ,_nom,   _ora  ,_abs,   _asl  ,_abs,   _slo,_abs|_und,  /* 0C..0F */
  _bpl  ,_rel,   _ora  ,_idy,   _ill  ,_nom,   _slo,_idy|_und,  /* 10..13 */
  _ill  ,_nom,   _ora  ,_zpx,   _asl  ,_zpx,   _slo,_zpx|_und,  /* 14..17 */
  _clc  ,_imp,   _ora  ,_aby,   _ill  ,_nom,   _slo,_aby|_und,  /* 18..1B */
  _ill  ,_nom,   _ora  ,_abx,   _asl  ,_abx,   _slo,_abx|_und,  /* 1C..1F */
  _jsr  ,_abs,   _and  ,_idx,   _ill  ,_nom,   _rla,_idx|_und,  /* 20..23 */
  _bit  ,_zpg,   _and  ,_zpg,   _rol  ,_zpg,   _rla,_zpg|_und,  /* 24..27 */
  _plp  ,_imp,   _and  ,_imm,   _rol  ,_acc,   _anc,_imm|_und,  /* 28..2B */
  _bit  ,_abs,   _and  ,_abs,   _rol  ,_abs,   _rla,_abs|_und,  /* 2C..2F */
  _bmi  ,_rel,   _and  ,_idy,   _ill  ,_nom,   _rla,_idy|_und,  /* 30..33 */
  _ill  ,_nom,   _and  ,_zpx,   _rol  ,_zpx,   _rla,_zpx|_und,  /* 34..37 */
  _sec  ,_imp,   _and  ,_aby,   _ill  ,_nom,   _rla,_aby|_und,  /* 38..3B */
  _ill  ,_nom,   _and  ,_abx,   _rol  ,_abx,   _rla,_abx|_und,  /* 3C..3F */
  _rti  ,_imp,   _eor  ,_idx,   _ill  ,_nom,   _sre,_idx|_und,  /* 40..43 */
  _ill  ,_nom,   _eor  ,_zpg,   _lsr  ,_zpg,   _sre,_zpg|_und,  /* 44..47 */
  _pha  ,_imp,   _eor  ,_imm,   _lsr  ,_acc,   _alr,_imm|_und,  /* 48..4B */
  _jmp  ,_abs,   _eor  ,_abs,   _lsr  ,_abs,   _sre,_abs|_und,  /* 4C..4F */
  _bvc  ,_rel,   _eor  ,_idy,   _ill  ,_nom,   _sre,_idy|_und,  /* 50..53 */
  _ill  ,_nom,   _eor  ,_zpx,   _lsr  ,_zpx,   _sre,_zpx|_und,  /* 54..57 */
  _cli  ,_imp,   _eor  ,_aby,   _ill  ,_nom,   _sre,_aby|_und,  /* 58..5B */
  _ill  ,_nom,   _eor  ,_abx,   _lsr  ,_abx,   _sre,_abx|_und,  /* 5C..5F */
  _rts  ,_imp,   _adc  ,_idx,   _ill  ,_nom,   _rra,_idx|_und,  /* 60..63 */
  _ill  ,_nom,   _adc  ,_zpg,   _ror  ,_zpg,   _rra,_zpg|_und,  /* 64..67 */
  _pla  ,_imp,   _adc  ,_imm,   _ror  ,_acc,   _arr,_imm|_und,  /* 68..6B */
  _jmp  ,_ind,   _adc  ,_abs,   _ror  ,_abs,   _rra,_abs|_und,  /* 6C..6F */
  _bvs  ,_rel,   _adc  ,_idy,   _ill  ,_nom,   _rra,_idy|_und,  /* 70..73 */
  _ill  ,_nom,   _adc  ,_zpx,   _ror  ,_zpx,   _rra,_zpx|_und,  /* 74..77 */
  _sei  ,_imp,   _adc  ,_aby,   _ill  ,_nom,   _rra,_aby|_und,  /* 78..7B */
  _ill  ,_nom,   _adc  ,_abx,   _ror  ,_abx,   _rra,_abx|_und,  /* 7C..7F */
  _ill  ,_nom,   _sta  ,_idx,   _ill  ,_nom,   _sax,_idx|_und,  /* 80..83 */
  _sty  ,_zpg,   _sta  ,_zpg,   _stx  ,_zpg,   _sax,_zpg|_und,  /* 84..87 */
  _dey  ,_imp,   _ill  ,_nom,   _txa  ,_imp,   _xaa,_imm|_und,  /* 88..8B */
  _sty  ,_abs,   _sta  ,_abs,   _stx  ,_abs,   _sax,_abs|_und,  /* 8C..8F */
  _bcc  ,_rel,   _sta  ,_idy,   _ill  ,_nom,   _ahx,_idy|_und,  /* 90..93 */
  _sty  ,_zpx,   _sta  ,_zpx,   _stx  ,_zpy,   _sax,_zpy|_und,  /* 94..97 */
  _tya  ,_imp,   _sta  ,_aby,   _txs  ,_imp,   _tas,_aby|_und,  /* 98..9B */
  _shy,_abx|_und,_sta  ,_abx,   _shx,_aby|_und,_ahx,_aby|_und,  /* 9C..9F */
  _ldy  ,_imm,   _lda  ,_idx,   _ldx  ,_imm,   _lax,_idx|_und,  /* A0..A3 */
  _ldy  ,_zpg,   _lda  ,_zpg,   _ldx  ,_zpg,   _lax,_zpg|_und,  /* A4..A7 */
  _tay  ,_imp,   _lda  ,_imm,   _tax  ,_imp,   _lax,_imm|_und,  /* A8..AB */
  _ldy  ,_abs,   _lda  ,_abs,   _ldx  ,_abs,   _lax,_abs|_und,  /* AC..AF */
  _bcs  ,_rel,   _lda  ,_idy,   _ill  ,_nom,   _lax,_idy|_und,  /* B0..B3 */
  _ldy  ,_zpx,   _lda  ,_zpx,   _ldx  ,_zpy,   _lax,_zpy|_und,  /* B4..B7 */
  _clv  ,_imp,   _lda  ,_aby,   _tsx  ,_imp,   _las,_aby|_und,  /* B8..BB */
  _ldy  ,_abx,   _lda  ,_abx,   _ldx  ,_aby,   _lax,_aby|_und,  /* BC..BF */
  _cpy  ,_imm,   _cmp  ,_idx,   _ill  ,_nom,   _dcp,_idx|_und,  /* C0..C3 */
  _cpy  ,_zpg,   _cmp  ,_zpg,   _dec  ,_zpg,   _dcp,_zpg|_und,  /* C4..C7 */
  _iny  ,_imp,   _cmp  ,_imm,   _dex  ,_imp,   _axs,_imm|_und,  /* C8..CB */
  _cpy  ,_abs,   _cmp  ,_abs,   _dec  ,_abs,   _dcp,_abs|_und,  /* CC..CF */
  _bne  ,_rel,   _cmp  ,_idy,   _ill  ,_nom,   _dcp,_idy|_und,  /* D0..D3 */
  _ill  ,_nom,   _cmp  ,_zpx,   _dec  ,_zpx,   _dcp,_zpx|_und,  /* D4..D7 */
  _cld  ,_imp,   _cmp  ,_aby,   _ill  ,_nom,   _dcp,_aby|_und,  /* D8..DB */
  _ill  ,_nom,   _cmp  ,_abx,   _dec  ,_abx,   _dcp,_abx|_und,  /* DC..DF */
  _cpx  ,_imm,   _sbc  ,_idx,   _ill  ,_nom,   _isc,_idx|_und,  /* E0..E3 */
  _cpx  ,_zpg,   _sbc  ,_zpg,   _inc  ,_zpg,   _isc,_zpg|_und,  /* E4..E7 */
  _inx  ,_imp,   _sbc  ,_imm,   _nop  ,_imp,   _ill  ,_nom,     /* E8..EB */
  _cpx  ,_abs,   _sbc  ,_abs,   _inc  ,_abs,   _isc,_abs|_und,  /* EC..EF */
  _beq  ,_rel,   _sbc  ,_idy,   _ill  ,_nom,   _isc,_idy|_und,  /* F0..F3 */
  _ill  ,_nom,   _sbc  ,_zpx,   _inc  ,_zpx,   _isc,_zpx|_und,  /* F4..F7 */
  _sed  ,_imp,   _sbc  ,_aby,   _ill  ,_nom,   _isc,_aby|_und,  /* F8..FB */
  _ill  ,_nom,   _sbc  ,_abx,   _inc  ,_abx,   _isc,_abx|_und,  /* FC..FF */
  };

// yet to determine what to do:
// $0B is the same undefined instruction as $2B
// $EB is the same undefined instruction as $E9 (where it IS defined)
// VARIOUS instructions would map to NOP or HCF!
// ... and we can't have that, as it would be impossible to re-assemble

/*****************************************************************************/
/* opcodes : 6500 opcodes array for initialization                           */
/*****************************************************************************/

OpCode Dasm6501::m6501_opcodes[mnemo6501_count - mnemo6500_count] =
  {
    // undocumented opcodes:
    { "LAX",   Data },                  /* _lax                              */
    { "DCP",   Data },                  /* _dcp                              */
    { "ISC",   Data },                  /* _isc - asmx uses "ISB" mnemo      */
    { "RLA",   Data },                  /* _rla                              */
    { "RRA",   Data },                  /* _rra                              */
    { "SAX",   Data },                  /* _sax                              */
    { "SLO",   Data },                  /* _slo                              */
    { "SRE",   Data },                  /* _sre                              */
    { "ANC",   Data },                  /* _anc                              */
    { "ARR",   Data },                  /* _arr                              */
    { "ALR",   Data },                  /* _asr - asmb uses "ASR" mnemo      */
    { "AXS",   Data },                  /* _axs - asmb uses "SBX" mnemo      */
    { "XAA",   Data },                  /* _xaa - asmb doesn't have that!    */
    { "TAS",   Data },                  /* _tas - asmb doesn't have that!    */
    { "AHX",   Data },                  /* _ahx - asmb doesn't have that!    */
    { "LAS",   Data },                  /* _las - asmb doesn't have that!    */
    { "SHX",   Data },                  /* _shx - asmb doesn't have that!    */
    { "SHY",   Data },                  /* _shy - asmb doesn't have that!    */

  };

/*****************************************************************************/
/* Dasm6501 : constructor                                                    */
/*****************************************************************************/

Dasm6501::Dasm6501(Application *pApp) : Dasm650X(pApp)
{
codes = m6501_codes;
mnemo.resize(mnemo6501_count);          /* expand mnemonics table            */
for (int i = mnemo6500_count; i < mnemo6501_count; i++)
  mnemo[i] = m6501_opcodes[i - mnemo6500_count];
useUndefined = false;
AddOption("undef", "{off|on}\tdisassemble unique undefined opcodes",
          static_cast<PSetter>(&Dasm6501::Set6501Option),
          static_cast<PGetter>(&Dasm6501::Get6501Option));
}

/*****************************************************************************/
/* Set65001ption : sets a disassembler option                                */
/*****************************************************************************/

int Dasm6501::Set6501Option(string lname, string value)
{
bool bnValue = true;                    /* default to "on"                   */
bool bIsBool = ParseBool(value, bnValue);

if (lname == "undef")
  {
  useUndefined = bnValue;
  return bIsBool ? 1 : 0;
  }
else
  return 0;                             /* only name consumed                */

// should never happen ... but better safe than sorry
return 1;                               /* name and value consumed           */
}

/*****************************************************************************/
/* Get6501Option : retrieves a disassembler option                           */
/*****************************************************************************/

string Dasm6501::Get6501Option(string lname)
{
if (lname == "undef")
  return useUndefined ? "on" : "off";

return "";
}

/*****************************************************************************/
/* FetchInstructionDetails : fetch details about current instruction         */
/*****************************************************************************/

adr_t Dasm6501::FetchInstructionDetails
    (
    adr_t PC,
    uint8_t &instpg,
    uint8_t &instb,
    uint8_t &mode,
    int &MI,
    const char *&I,
    string *smnemo
    )
{
uint16_t W;

instpg = 0;  // 6501 has no instruction page
instb = GetUByte(PC++);
W = (uint16_t)instb * 2;
MI = codes[W++];
mode = codes[W];
bool und = !!(mode & _und);
mode &= ~_und;
if (und && !useUndefined)               /* if unwanted undefined opcode      */
  {
  MI = _ill;
  mode = _nom;
  }
I = mnemo[MI].mne;
if (smnemo)
  *smnemo = I;
return PC;
}



/*===========================================================================*/
/* Dasm6503 class members                                                    */
/*===========================================================================*/
// since that's just a 6501 with a smaller bus, not much to do

/*****************************************************************************/
/* Dasm6503 : constructor                                                    */
/*****************************************************************************/

Dasm6503::Dasm6503(Application *pApp) : Dasm6501(pApp)
{
}


/*===========================================================================*/
/* Dasm6504 class members                                                    */
/*===========================================================================*/
// since that's just a 6501 with a smaller bus, not much to do

/*****************************************************************************/
/* Dasm6504 : constructor                                                    */
/*****************************************************************************/

Dasm6504::Dasm6504(Application *pApp) : Dasm6501(pApp)
{
}


/*===========================================================================*/
/* Dasm65C02 class members                                                   */
/*===========================================================================*/

/*****************************************************************************/
/* m65c02_codes : table of all 65C02 instruction bytes and types             */
/*****************************************************************************/

uint8_t Dasm65C02::m65c02_codes[512] =
  {
  _brk  ,_imp,   _ora  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* 00..03 */
  _tsb  ,_zpg,   _ora  ,_zpg,   _asl  ,_zpg,   _rmb  ,_zpb,     /* 04..07 */
  _php  ,_imp,   _ora  ,_imm,   _asl  ,_acc,   _ill  ,_nom,     /* 08..0B */
  _tsb  ,_abs,   _ora  ,_abs,   _asl  ,_abs,   _bbr  ,_bbt,     /* 0C..0F */
  _bpl  ,_rel,   _ora  ,_idy,   _ora  ,_zpi,   _ill  ,_nom,     /* 10..13 */
  _trb  ,_zpg,   _ora  ,_zpx,   _asl  ,_zpx,   _rmb  ,_zpb,     /* 14..17 */
  _clc  ,_imp,   _ora  ,_aby,   _inc  ,_imp,   _ill  ,_nom,     /* 18..1B */
  _trb  ,_abs,   _ora  ,_abx,   _asl  ,_abx,   _bbr  ,_bbt,     /* 1C..1F */
  _jsr  ,_abs,   _and  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* 20..23 */
  _bit  ,_zpg,   _and  ,_zpg,   _rol  ,_zpg,   _rmb  ,_zpb,     /* 24..27 */
  _plp  ,_imp,   _and  ,_imm,   _rol  ,_acc,   _ill  ,_nom,     /* 28..2B */
  _bit  ,_abs,   _and  ,_abs,   _rol  ,_abs,   _bbr  ,_bbt,     /* 2C..2F */
  _bmi  ,_rel,   _and  ,_idy,   _and  ,_zpi,   _ill  ,_nom,     /* 30..33 */
  _bit  ,_zpx,   _and  ,_zpx,   _rol  ,_zpx,   _rmb  ,_zpb,     /* 34..37 */
  _sec  ,_imp,   _and  ,_aby,   _dec  ,_imp,   _ill  ,_nom,     /* 38..3B */
  _bit  ,_abx,   _and  ,_abx,   _rol  ,_abx,   _bbr  ,_bbt,     /* 3C..3F */
  _rti  ,_imp,   _eor  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* 40..43 */
  _ill  ,_nom,   _eor  ,_zpg,   _lsr  ,_zpg,   _rmb  ,_zpb,     /* 44..47 */
  _pha  ,_imp,   _eor  ,_imm,   _lsr  ,_acc,   _ill  ,_nom,     /* 48..4B */
  _jmp  ,_abs,   _eor  ,_abs,   _lsr  ,_abs,   _bbr  ,_bbt,     /* 4C..4F */
  _bvc  ,_rel,   _eor  ,_idy,   _eor  ,_zpi,   _ill  ,_nom,     /* 50..53 */
  _ill  ,_nom,   _eor  ,_zpx,   _lsr  ,_zpx,   _rmb  ,_zpb,     /* 54..57 */
  _cli  ,_imp,   _eor  ,_aby,   _phy  ,_imp,   _ill  ,_nom,     /* 58..5B */
  _ill  ,_nom,   _eor  ,_abx,   _lsr  ,_abx,   _bbr  ,_bbt,     /* 5C..5F */
  _rts  ,_imp,   _adc  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* 60..63 */
  _stz  ,_zpg,   _adc  ,_zpg,   _ror  ,_zpg,   _rmb  ,_zpb,     /* 64..67 */
  _pla  ,_imp,   _adc  ,_imm,   _ror  ,_acc,   _ill  ,_nom,     /* 68..6B */
  _jmp  ,_ind,   _adc  ,_abs,   _ror  ,_abs,   _bbr  ,_bbt,     /* 6C..6F */
  _bvs  ,_rel,   _adc  ,_idy,   _adc  ,_zpi,   _ill  ,_nom,     /* 70..73 */
  _stz  ,_zpx,   _adc  ,_zpx,   _ror  ,_zpx,   _rmb  ,_zpb,     /* 74..77 */
  _sei  ,_imp,   _adc  ,_aby,   _ply  ,_imp,   _ill  ,_nom,     /* 78..7B */
  _jmp  ,_abx,   _adc  ,_abx,   _ror  ,_abx,   _bbr  ,_bbt,     /* 7C..7F */
  _bra  ,_rel,   _sta  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* 80..83 */
  _sty  ,_zpg,   _sta  ,_zpg,   _stx  ,_zpg,   _smb  ,_zpb,     /* 84..87 */
  _dey  ,_imp,   _bit  ,_imm,   _txa  ,_imp,   _ill  ,_nom,     /* 88..8B */
  _sty  ,_abs,   _sta  ,_abs,   _stx  ,_abs,   _bbs  ,_bbt,     /* 8C..8F */
  _bcc  ,_rel,   _sta  ,_idy,   _sta  ,_zpi,   _ill  ,_nom,     /* 90..93 */
  _sty  ,_zpx,   _sta  ,_zpx,   _stx  ,_zpy,   _smb  ,_zpb,     /* 94..97 */
  _tya  ,_imp,   _sta  ,_aby,   _txs  ,_imp,   _ill  ,_nom,     /* 98..9B */
  _stz  ,_abs,   _sta  ,_abx,   _stz  ,_abx,   _bbs  ,_bbt,     /* 9C..9F */
  _ldy  ,_imm,   _lda  ,_idx,   _ldx  ,_imm,   _ill  ,_nom,     /* A0..A3 */
  _ldy  ,_zpg,   _lda  ,_zpg,   _ldx  ,_zpg,   _smb  ,_zpb,     /* A4..A7 */
  _tay  ,_imp,   _lda  ,_imm,   _tax  ,_imp,   _ill  ,_nom,     /* A8..AB */
  _ldy  ,_abs,   _lda  ,_abs,   _ldx  ,_abs,   _bbs  ,_bbt,     /* AC..AF */
  _bcs  ,_rel,   _lda  ,_idy,   _lda  ,_zpi,   _ill  ,_nom,     /* B0..B3 */
  _ldy  ,_zpx,   _lda  ,_zpx,   _ldx  ,_zpy,   _smb  ,_zpb,     /* B4..B7 */
  _clv  ,_imp,   _lda  ,_aby,   _tsx  ,_imp,   _ill  ,_nom,     /* B8..BB */
  _ldy  ,_abx,   _lda  ,_abx,   _ldx  ,_aby,   _bbs  ,_bbt,     /* BC..BF */
  _cpy  ,_imm,   _cmp  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* C0..C3 */
  _cpy  ,_zpg,   _cmp  ,_zpg,   _dec  ,_zpg,   _smb  ,_zpb,     /* C4..C7 */
  _iny  ,_imp,   _cmp  ,_imm,   _dex  ,_imp,   _wai  ,_imp,     /* C8..CB */
  _cpy  ,_abs,   _cmp  ,_abs,   _dec  ,_abs,   _bbs  ,_bbt,     /* CC..CF */
  _bne  ,_rel,   _cmp  ,_idy,   _cmp  ,_zpi,   _ill  ,_nom,     /* D0..D3 */
  _ill  ,_nom,   _cmp  ,_zpx,   _dec  ,_zpx,   _smb  ,_zpb,     /* D4..D7 */
  _cld  ,_imp,   _cmp  ,_aby,   _phx  ,_imp,   _stp  ,_imp,     /* D8..DB */
  _ill  ,_nom,   _cmp  ,_abx,   _dec  ,_abx,   _bbs  ,_bbt,     /* DC..DF */
  _cpx  ,_imm,   _sbc  ,_idx,   _ill  ,_nom,   _ill  ,_nom,     /* E0..E3 */
  _cpx  ,_zpg,   _sbc  ,_zpg,   _inc  ,_zpg,   _smb  ,_zpb,     /* E4..E7 */
  _inx  ,_imp,   _sbc  ,_imm,   _nop  ,_imp,   _ill  ,_nom,     /* E8..EB */
  _cpx  ,_abs,   _sbc  ,_abs,   _inc  ,_abs,   _bbs  ,_bbt,     /* EC..EF */
  _beq  ,_rel,   _sbc  ,_idy,   _sbc  ,_zpi,   _ill  ,_nom,     /* F0..F3 */
  _ill  ,_nom,   _sbc  ,_zpx,   _inc  ,_zpx,   _smb  ,_zpb,     /* F4..F7 */
  _sed  ,_imp,   _sbc  ,_aby,   _plx  ,_imp,   _ill  ,_nom,     /* F8..FB */
  _ill  ,_nom,   _sbc  ,_abx,   _inc  ,_abx,   _bbs  ,_bbt,     /* FC..FF */
  };

/*****************************************************************************/
/* opcodes : 65C02 opcodes array for initialization                          */
/*****************************************************************************/

OpCode Dasm65C02::m65c02_opcodes[mnemo65C02_count - mnemo6500_count] =
  {
    { "BRA",   Code },                  /* _bra                              */
    { "PHX",   Data },                  /* _phx                              */
    { "PHY",   Data },                  /* _phy                              */
    { "PLX",   Data },                  /* _plx                              */
    { "PLY",   Data },                  /* _ply                              */
    { "STZ",   Data },                  /* _stz                              */
    { "TRB",   Data },                  /* _trb                              */
    { "TSB",   Data },                  /* _tsb                              */
    { "BBR",   Data },                  /* _bbr                              */
    { "BBS",   Data },                  /* _bbs                              */
    { "RMB",   Data },                  /* _rmb                              */
    { "SMB",   Data },                  /* _smb                              */
    { "STP",   Data },                  /* _stp                              */
    { "WAI",   Data },                  /* _wai                              */
  };

/*****************************************************************************/
/* Dasm65C02 : constructor                                                   */
/*****************************************************************************/

Dasm65C02::Dasm65C02(Application *pApp) : Dasm650X(pApp)
{
codes = m65c02_codes;
mnemo.resize(mnemo65C02_count);         /* expand mnemonics table            */
for (int i = mnemo6500_count; i < mnemo65C02_count; i++)
  mnemo[i] = m65c02_opcodes[i - mnemo6500_count];
}

/*****************************************************************************/
/* ParseCode : parse instruction at given memory address for labels          */
/*****************************************************************************/

adr_t Dasm65C02::ParseCode
    (
    adr_t addr,
    int bus                             /* ignored for 6500 and derivates    */
    )
{
uint8_t instpg, instb, T, mode;
uint16_t W;
int MI;
const char *I;
bool bSetLabel;
adr_t PC = FetchInstructionDetails(addr, instpg, instb, mode, MI, I);

switch (mode)                           /* which mode is this ?              */
  {
  case _zpi:                            /* (zero-page)                       */
  case _zpb:                            /* zero-page bit                     */
    bSetLabel = !IsConst(PC);
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);
    if (bSetLabel)
      {
      // this isn't really necessary for 6500, but maybe for derived classes
      adr_t dp = GetDirectPage(PC);
      if (dp == DEFAULT_ADDRESS || dp == NO_ADDRESS)
        dp = GetDirectPage(addr);
      if (dp == DEFAULT_ADDRESS)
        dp = 0;
      if (dp != NO_ADDRESS)
        {
        T = GetUByte(PC);
        W = (uint16_t)dp | T;
        W = (uint16_t)PhaseInner(W, PC);
        AddRelativeLabel(W, PC, mnemo[MI].memType, true, BusCode, addr);
        }
      }
    PC++;
    break;
  case _bbt :                           /* branch on bit                     */
    bSetLabel = !IsConst(PC);           /* process the zero-page part        */
    if (!bSetLabel)
      SetDefLabelUsed(PC, bus);
    if (bSetLabel)
      {
      // this isn't really necessary for 6500, but maybe for derived classes
      adr_t dp = GetDirectPage(PC);
      if (dp == DEFAULT_ADDRESS || dp == NO_ADDRESS)
        dp = GetDirectPage(addr);
      if (dp == DEFAULT_ADDRESS)
        dp = 0;
      if (dp != NO_ADDRESS)
        {
        T = GetUByte(PC);
        W = (uint16_t)dp | T;
        W = (uint16_t)PhaseInner(W, PC);
        AddRelativeLabel(W, PC, mnemo[MI].memType, true, BusCode, addr);
        }
      }
    PC++;
                                        /* process the rel part              */
    bSetLabel = !IsConst(PC);
    // lbl = bSetLabel ? NULL : FindLabel(PC, Const, bus);
    T = GetUByte(PC); PC++;
    W = (uint16_t)(PC + (signed char)T);
    if (bSetLabel)
      {
      W = (uint16_t)DephaseOuter(W, PC - 1);
      AddRelativeLabel(W, PC - 1, mnemo[MI].memType, true, BusCode, addr);
      }
    break;

  default:
    return Dasm650X::ParseCode(addr, bus);
  }

return PC - addr;                       /* pass back # processed bytes       */
}

/*****************************************************************************/
/* DisassembleCode : disassemble code instruction at given memory address    */
/*****************************************************************************/

adr_t Dasm65C02::DisassembleCode
    (
    adr_t addr,
    string &smnemo,
    string &sparm,
    int bus                             /* ignored for 6500 and derivates    */
    )
{
uint8_t instpg, instb, T, mode;
uint16_t W;
int MI;
const char *I;
adr_t PC = FetchInstructionDetails(addr, instpg, instb, mode, MI, I, &smnemo);
bool bGetLabel;
Label *lbl;
char bitnum;

switch (mode)                           /* which mode is this?               */
  {
  case _zpi:                            /* (zero page)                       */
    {
    bGetLabel = !IsConst(PC);
    lbl = bGetLabel ? NULL : FindLabel(PC, Const, bus);
    T = GetUByte(PC);
    adr_t dp = GetDirectPage(PC);
    if (dp == DEFAULT_ADDRESS || dp == NO_ADDRESS)
      dp = GetDirectPage(addr);
    if (dp == DEFAULT_ADDRESS)
      dp = 0;
    if (dp != NO_ADDRESS)
      {
      W = (uint16_t)dp | T;
      if (bGetLabel)
        W = (uint16_t)PhaseInner(W, PC);
      sparm = GetForcedAddr(PC) ? "<" : "";
      sparm += lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
      }
    else // if no direct page, this can't be interpreted as a label
      sparm = (lbl ? lbl->GetText() : Number2String(T, 2, PC));
    sparm = "(" + sparm + ")";
    PC++;
    }
    break;
  case _bbt :                           /* branch on bit                     */
    {
    bitnum = (instb & 0x70) >> 4;       /* get number to add to mnemonic     */
    smnemo += '0' + bitnum;
                                        /* process the zpg part              */
    bGetLabel = !IsConst(PC);
    lbl = bGetLabel ? NULL : FindLabel(PC, Const, bus);
    T = GetUByte(PC);
    adr_t dp = GetDirectPage(PC);
    if (dp == DEFAULT_ADDRESS || dp == NO_ADDRESS)
      dp = GetDirectPage(addr);
    if (dp == DEFAULT_ADDRESS)
      dp = 0;
    if (dp != NO_ADDRESS)
      {
      W = (uint16_t)dp | T;
      if (bGetLabel)
        W = (uint16_t)PhaseInner(W, PC);
      sparm = GetForcedAddr(PC) ? "<" : "";
      sparm += lbl ? lbl->GetText() : Label2String(W, 4, bGetLabel, PC);
      }
    else // if no direct page, this can't be interpreted as a label
      sparm = (lbl ? lbl->GetText() : Number2String(T, 2, PC));
    sparm += ",";
    PC++;

    bGetLabel = !IsConst(PC);           /* process the rel part              */
    lbl = bGetLabel ? NULL : FindLabel(PC, Const, bus);
    T = GetUByte(PC++);
    if (bGetLabel)
      {
      W = (uint16_t)(PC + (int8_t)T);
      W = (uint16_t)DephaseOuter(W, PC - 1);
      sparm = Label2String(W, 4, bGetLabel, PC - 1);
      }
    else
      {
      int nDiff = (int)(int8_t)T;
      sparm += "*";
      if (nDiff >= 0 || lbl)
        sparm += "+";
      sparm += lbl ? lbl->GetText() : SignedNumber2String(nDiff + 2, 2, PC - 1);
      }
    }
    break;
  case _zpb:                            /* zero-page bit                     */
#if 0
    // not sure what I wanted to do here ...
    bitnum = (instb & 0x70) >> 4;       /* get number to add to mnemonic     */
    smnemo += '0' + bitnum;
    mode = _zpg;                        /* otherwise, it's zero-page         */
#endif
    return Dasm650X::DisassembleCode(addr, smnemo, sparm, bus);
  default :
    return Dasm650X::DisassembleCode(addr, smnemo, sparm, bus);
  }
return PC - addr;                       /* pass back # processed bytes       */
}

