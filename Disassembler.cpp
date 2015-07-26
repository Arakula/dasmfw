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
/* Disassembler.cpp : disassembler base class functionality                  */
/*****************************************************************************/

#include "Disassembler.h"

/*****************************************************************************/
/* GetHex : retrieves a hex value in given length from file                  */
/*****************************************************************************/

addr_t GetHex(FILE *f, int nChars, uint8_t *pchks)
{
addr_t out = 0;

for (int i = 0; i < nChars; i++)
  {
  int c = fgetc(f);
  if (c == EOF)
    return NO_ADDRESS;
  if ((c >= 'a') && (c <= 'f'))
    c -= ('a' - 'A');
  if ((!((c >= '0') && (c <= '9'))) &&
      (!((c >= 'A') && (c <= 'F'))))
    return NO_ADDRESS;
  c -= '0';
  if (c > 9)
    c -= 7;
  if (pchks)
    *pchks += (i & 1) ? c : (c << 4);
  out = out * 16 + c;
  }

return out;
}

/*****************************************************************************/
/* GetBasicDisassemblyFlags : returns basic disassembly flag set             */
/*****************************************************************************/

uint32_t GetBasicDisassemblyFlags(MemAttribute *pAttr, uint8_t mem, Label *plbl)
{
// should be sufficient for most, if not all, disassemblers
uint32_t wf = 0;
if (!pAttr ||
    pAttr->GetMemType() == Code ||
    !pAttr->IsUsed())
  return 0;

MemAttribute::Display display = pAttr->GetDisplay();
wf = pAttr->cellSize | SHMF_DATA;       /* assemble flags for data byte      */

if (pAttr->GetMemType() == Bss)
  wf |= SHMF_RMB;
else if (pAttr->GetSize() == 1
#if RB_VARIANT
         && display == MemAttribute::Char
#else
         && display != MemAttribute::Binary
#endif
         )
  {
  // NB: this is totally ASCII-centric, but as long as we don't
  // have to deal with other encodings, it should be OK.
  if ((mem >= 0x20) && (mem <= 0x7e) &&
      (mem != '&') && (mem != '\"') &&
      (pAttr->GetMemType() != Const))
    wf |= SHMF_TXT;
  }
if (pAttr->GetBreakBefore())
    wf |= SHMF_BREAK;

if (plbl && plbl->IsUsed())
  {
  // labels containing '+' or '-' are no real labels; they are
  // references to another label (e.g., "OLBL+4")
  std::string &s = plbl->GetText();
  if (s.find('-') == s.npos && s.find('+') == s.npos)
    wf |= SHMF_BREAK;
  }

#if 0
// Should be done outside
if ((commentlines[pc]) || (lcomments[pc]))
    wf |= SHMF_COMMENT;
#endif

return wf;
}


/*===========================================================================*/
/* Disassembler class members                                                */
/*===========================================================================*/

// determine the endianness of the machine the disassembler runs on
static const uint16_t chkEnd = (((uint16_t)Disassembler::BigEndian) << 8) |
                               (uint16_t)Disassembler::LittleEndian;
const Disassembler::Endian Disassembler::prgEndian = (Disassembler::Endian)*((uint8_t *)&chkEnd);

/*****************************************************************************/
/* Disassembler : constructor                                                */
/*****************************************************************************/

Disassembler::Disassembler()
{
begin = load = NO_ADDRESS;
end = offset = 0;
memattr[0] = memattr[1] = NULL;
commentStart = ";";
#if RB_VARIANT
labelDelim = ":";
#else
labelDelim = "";
#endif
defaultDisplay = MemAttribute::Hex;
pbase = 16;
bLoadLabel = true;
bSetSysVec = true;

// set up options table
// base class uses one generic option setter/getter pair (not mandatory)
AddOption("pbase", "{number}\tdefault base for parsing numbers",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("begin", "{addr}\tstart disassembly address",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("end", "{addr}\tend disassembly address",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("offset", "{addr}\taddress to load program",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("cchar", "{char}\tcomment start character",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("ldchar", "{char}\tlabel delimiter character",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("loadlabel", "{off|on}\tflag whether to use entry point label",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("sysvec", "{off|on}\tflag whether to set system vector labels",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
}

/*****************************************************************************/
/* ~Disassembler : destructor                                                */
/*****************************************************************************/

Disassembler::~Disassembler()
{
for (int i = 0; i < GetOptionCount(); i++)
  delete options[i];
}

/*****************************************************************************/
/* AddOption : add an option and its handler to the options array            */
/*****************************************************************************/

bool Disassembler::AddOption
    (
    std::string name,
    std::string help,
    PSetter setter,
    PGetter getter
    )
{
// allow overriding of base class option handlers
std::string lname(lowercase(name));
for (int i = 0; i < GetOptionCount(); i++)
  if (GetOptionName(i) == lname)
    {
    options[i]->help = help;
    options[i]->setter = setter;
    options[i]->getter = getter;
    return true;
    }
// append new option
try
  {
  OptionHandler *p = new OptionHandler;
  if (p)
    {
    p->name = lname;
    p->help = help;
    p->setter = setter;
    p->getter = getter;
    options.push_back(p);
    }
  else return false; // for very old compilers
  }
catch(...)
  {
  return false;
  }
return true;
}

/*****************************************************************************/
/* FindOption : returns index for an option or -1                            */
/*****************************************************************************/

int Disassembler::FindOption(std::string name)
{
std::string lname(lowercase(name));
for (int i = 0; i < GetOptionCount(); i++)
  if (name == options[i]->name)
    return i;
return -1;
}

/*****************************************************************************/
/* SetOption : sets a disassembler option                                    */
/*****************************************************************************/

int Disassembler::SetOption(std::string name, std::string value)
{
std::string lname(lowercase(name));

for (int i = 0; i < GetOptionCount(); i++)
  if (name == options[i]->name)
    return ((*this).*(options[i]->setter))(name, value);

return 0;
}

/*****************************************************************************/
/* GetOption : retrieves a disassembler option                               */
/*****************************************************************************/

std::string Disassembler::GetOption(std::string name)
{
std::string lname(lowercase(name));

for (int i = 0; i < GetOptionCount(); i++)
  if (name == options[i]->name)
    return ((*this).*(options[i]->getter))(name);

return "";
}

/*****************************************************************************/
/* DisassemblerSetOption : disassembler base class option handler            */
/*****************************************************************************/

int Disassembler::DisassemblerSetOption(std::string lname, std::string value)
{
// this could be expanded ... "0x" or "$" header come to mind...
std::string lvalue(lowercase(value));
addr_t ivalue = (addr_t)strtoul(value.c_str(), NULL, 10);
addr_t avalue;
String2Number(value, avalue);
int bnvalue = (lvalue == "off") ? 0 : (lvalue == "on") ? 1 : atoi(value.c_str());

if (lname == "pbase" &&
         ivalue >= 2 && ivalue <= 16)
  pbase = ivalue;
else if (lname == "begin" &&
    avalue >= GetLowestCodeAddr() &&
    avalue <= GetHighestCodeAddr())
  begin = avalue;
else if (lname == "end" &&
    avalue >= GetLowestCodeAddr() &&
    avalue <= GetHighestCodeAddr())
  end = avalue;
else if (lname == "offset" &&
    avalue >= GetLowestCodeAddr() &&
    avalue <= GetHighestCodeAddr())
  offset = avalue;
else if (lname == "cchar")
  commentStart = value;
else if (lname == "ldchar")
  labelDelim = value;
else if (lname == "loadlabel")
  bLoadLabel = !!bnvalue;
else if (lname == "sysvec")
  bSetSysVec = !!bnvalue;
else
  return 0;                             /* only option consumed              */

return 1;                               /* option + value consumed           */
}

/*****************************************************************************/
/* DisassemblerGetOption : disassembler base class option retrieval          */
/*****************************************************************************/

std::string Disassembler::DisassemblerGetOption(std::string lname)
{
std::string oval;
if (lname == "pbase") oval = sformat("%ld", pbase);
else if (lname == "begin") oval = (begin == NO_ADDRESS) ? "-1" : sformat("0x%lx", begin);
else if (lname == "end") oval = (end == NO_ADDRESS) ? "-1" : sformat("0x%lx", end);
else if (lname == "offset") oval = (offset == NO_ADDRESS) ? "-1" : sformat("0x%lx", offset);
else if (lname == "cchar") oval = commentStart;
else if (lname == "ldchar") oval = labelDelim;
else if (lname == "loadlabel") oval = bLoadLabel ? "on" : "off";
else if (lname == "sysvec") oval = bSetSysVec ? "on" : "off";
return oval;
}

/*****************************************************************************/
/* Label2String : converts a value to a (label) string                       */
/*****************************************************************************/

std::string Disassembler::Label2String
    (
    addr_t value,
    bool bUseLabel,
    addr_t addr,
    bool bDataBus
    )
{
std::string sOut;
addr_t relative = GetRelative(addr, bDataBus);
addr_t Wrel = (value + relative) &
              (bDataBus ? GetHighestDataAddr() : GetHighestCodeAddr());
std::string sLabel;
                                        /* get label name                    */
sLabel = (bUseLabel) ? GetLabel(Wrel, Untyped, bDataBus) : "";
                                        /* if there and absolute             */
if (Wrel == value && sLabel.size())
  return sLabel;                        /* return it                         */

if (sLabel.size())
  sOut = sLabel;
else if (bUseLabel && IsCLabel(Wrel, bDataBus))
  sOut = UnnamedLabel(Wrel, true, bDataBus);
else if (bUseLabel && IsDLabel(Wrel, bDataBus))
  sOut = UnnamedLabel(Wrel, false, bDataBus);
else
  sOut = Number2String(Wrel, 4, addr, bDataBus).c_str();

if (relative)                           /* if it's relative addressing       */
  {
  std::string sAdd("-");
  bool bInvert = true;
  int32_t nDiff = Wrel - value;         /* get difference                    */

                                        /* get base name                     */
  sLabel = (bUseLabel) ? GetLabel(relative, Untyped, bDataBus) : "";
  if (sLabel.size())
    sAdd += sLabel;
  else if (bUseLabel && IsCLabel(relative, bDataBus))
    sOut = UnnamedLabel(relative, true, bDataBus);
  else if (bUseLabel && IsDLabel(relative, bDataBus))
    sOut = UnnamedLabel(relative, false, bDataBus);
  else
    {
    if (nDiff < 0)                      /* if negative displacement          */
      {
      sAdd = "+";                       /* negative*negative is positive...  */
      bInvert = false;                  /* so invert the sign                */
                                        /* and make the number positive      */
      relative = (addr_t) (-((int32_t)relative));
      }
    sAdd += Number2String(relative, 4, addr, bDataBus);
    }

  if (bInvert)                          /* if inverting necessary,           */
    {
    std::string::iterator i = sAdd.begin();
    i++;
    while (i != sAdd.end())             /* invert eventual signs!            */
      {
      if (*i == '+')
        *i = '-';
      else if (*i == '-')
        *i = '+';
      i++;
      }
    }
  sOut += sAdd;
  }

return sOut;
}

/*****************************************************************************/
/* LoadIntelHex : tries to load as an Intel HEX file                         */
/*****************************************************************************/

bool Disassembler::LoadIntelHex
    (
    std::string filename,
    FILE *f,
    std::string &sLoadType,
    int interleave
    )
{
int nCurPos = ftell(f);
int c = 0, rectype;
bool done = false;
int nBytes = 0;
addr_t fbegin = GetHighestCodeAddr();
addr_t fend = GetLowestCodeAddr();
int segment = 0;                        /* segment address                   */

if ((c = fgetc(f)) == EOF)              /* look whether starting with ':'    */
  return false;
if (c != ':')
  {
  ungetc(c, f);
  return false;
  }

while (!done && (nBytes >= 0))          /* while there are lines             */
  {
  addr_t nAddr;
  int nBytesOnLine, i;
  uint8_t chks = 0;
  if (c != ':')
    break;
  nBytesOnLine = GetHex(f, 2, &chks);   /* retrieve # bytes on line          */
  if (nBytesOnLine < 0)                 /* if error                          */
    { nBytes = -1; break; }             /* return with error                 */
  else if (nBytesOnLine == 0)           /* if end of file                    */
    break;                              /* just break;                       */
  nAddr = GetHex(f,4, &chks) + segment; /* get address for bytes             */
  nAddr = (nAddr * interleave) + offset;
  int nSpreadBytes = (nBytesOnLine * interleave) - (interleave - 1);
  if ((nAddr < GetLowestCodeAddr()) ||  /* if illegal address                */
      (nAddr + nSpreadBytes > GetHighestCodeAddr()))
    { nBytes = -1; break; }             /* return with error                 */
  if (nAddr < fbegin)                   /* adjust start and end values       */
    {
    fbegin = nAddr;
    AddMemory(nAddr, nSpreadBytes);
    }
  if (nAddr + nSpreadBytes - 1 > fend)
    {
    AddMemory(nAddr, nSpreadBytes);
    fend = nAddr + nSpreadBytes - 1;
    }
  nBytes += nBytesOnLine;
  rectype = GetHex(f, 2, &chks);        /* fetch record type character       */
  switch (rectype)                      /* which type of record is this?     */
    {
    case 0 :                            /* data record                       */
      for (i = 0; i<nBytesOnLine; i++)  /* now get the bytes                 */
        {
        addr_t tgtaddr = nAddr + (i * interleave);
        c = GetHex(f, 2, &chks);        /* retrieve a byte                   */
        if ((c < 0) || (c > 0xff))      /* if illegal byte                   */
          { nBytes = -1; break; }       /* return with error                 */
        setat(tgtaddr, (uint8_t)c);     /* otherwise add memory byte         */
        SetCellUsed(tgtaddr);           /* mark as used byte                 */
        SetDisplay(tgtaddr, defaultDisplay);
        }
      break;
    case 1 :                            /* End Of File record                */
      done = true;
      break;
    case 2 :                            /* Extended Segment Address          */
      segment = GetHex(f, 4, &chks);    /* get segment value to use          */
      segment <<= 4;                    /* convert to linear addition value  */
      break;
    case 3 :                            /* Start Segment Address             */
      segment = GetHex(f, 4, &chks);    /* get segment value to use          */
      segment <<= 4;                    /* convert to linear addition value  */
      nAddr = GetHex(f, 4, &chks) +     /* get start instruction pointer     */
              segment;
    SetLoadAddr:
      nAddr = (nAddr * interleave) + offset;
      if ((nAddr < GetLowestCodeAddr()) || /* if illegal address           */
           (nAddr > GetHighestCodeAddr()))
        nBytes = -1;                    /* return with error                 */
      else
        load = nAddr;
      break;
    case 4 :                            /* Extended Linear Address           */
      segment = GetHex(f, 4, &chks);    /* get segment value to use          */
      segment <<= 16;                   /* convert to linear addition value  */
      break;
    case 5 :                            /* Start Linear Address              */
      nAddr = GetHex(f, 8, &chks);      /* get start instruction pointer     */
      goto SetLoadAddr;
    default :                           /* anything else?                    */
      nBytes = -1;                      /* unknown format. stop processing   */
      break;
    }

  /* ignore checksum byte; its calculation would be:
     add up all decoded bytes after the ':',
     take 2's complement of lowest byte */
  GetHex(f, 2, &chks);
  // chks should be 0!

  while (((c = fgetc(f)) != EOF) &&     /* skip to newline                   */
         (c != '\r') && (c != '\n'))
    ;

  while (((c = fgetc(f)) != EOF) &&     /* skip newline itself               */
         ((c == '\r') || (c == '\n')))
    ;
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
  sLoadType = "Intel HEX";
return (nBytes > 0);                    /* pass back #bytes interpreted      */
}

/*****************************************************************************/
/* LoadMotorolaHex : tries to load as a Motorola HEX file                    */
/*****************************************************************************/

bool Disassembler::LoadMotorolaHex
    (
    std::string filename,
    FILE *f,
    std::string &sLoadType,
    int interleave
    )
{
int nCurPos = ftell(f);
int c = 0;
bool done = false;
int nBytes = 0;
addr_t fbegin = GetHighestCodeAddr();
addr_t fend = GetLowestCodeAddr();

if ((c = fgetc(f)) == EOF)              /* look whether starting with 'S'    */
  return false;
if (c != 'S')
  {
  ungetc(c, f);
  return false;
  }

while ((!done) && (nBytes >= 0))        /* while there are lines             */
  {
  int nLineType = 0, nBytesOnLine, i;
  addr_t nAddr;
  uint8_t chks = 0;
  if (c != 'S')
    break;
  fread(&nLineType, 1, 1, f);           /* retrieve line type                */
  nBytesOnLine = GetHex(f, 2, &chks);   /* retrieve # bytes on line          */
  if (nBytesOnLine < 0)                 /* if error                          */
    { nBytes = -1; break; }             /* return with error                 */
  else if (nBytesOnLine == 0)           /* if end of file                    */
    break;                              /* just break;                       */
  int nSpreadBytes = (nBytesOnLine * interleave) - (interleave - 1);
  switch (nLineType)                    /* now examine line type             */
    {
    case '0' :
#if 0                                   /* simply ignore the rest of the line*/
      nBytesOnLine--;
      while (nBytesOnLine--)
        GetHex(f, 2);
#endif
      break;
    case '1' :                          /* record with 16bit address         */
      nBytesOnLine -= 3;
      nAddr = GetHex(f,4, &chks);       /* get address for bytes             */
    data16bit:
      nAddr = (nAddr * interleave) + offset;
      if ((nAddr < GetLowestCodeAddr()) || /* if illegal address             */
           (nAddr + nSpreadBytes > GetHighestCodeAddr()))
        { nBytes = -1; break; }         /* return with error                 */
      if (nAddr < fbegin)               /* adjust start and end values       */
        {
        fbegin = nAddr;
        AddMemory(nAddr, nSpreadBytes);
        }
      if (nAddr + nSpreadBytes - 1 > fend)
        {
        fend = nAddr + nSpreadBytes - 1;
        AddMemory(nAddr, nSpreadBytes);
        }
      nBytes += nBytesOnLine;
                                        /* now get the bytes                 */
      for (i = 0; i < nBytesOnLine; i++)
        {
        addr_t tgtaddr = nAddr + (i * interleave);
        c = GetHex(f, 2, &chks);        /* retrieve a byte                   */
        if ((c < 0) || (c > 0xff))      /* if illegal byte                   */
          { nBytes = -1; break; }       /* return with error                 */
        setat(tgtaddr, (uint8_t)c);     /* otherwise add memory byte         */
        SetCellUsed(tgtaddr);           /* mark as used byte                 */
        SetDisplay(tgtaddr, defaultDisplay);
        }
      break;
    case '2' :                          /* record with 24bit address         */
      nBytesOnLine -= 4;
      nAddr = GetHex(f, 6, &chks);      /* get address for bytes             */
      goto data16bit;
    case '3' :                          /* record with 32bit address         */
      nBytesOnLine -= 5;
      nAddr = GetHex(f, 8, &chks);      /* get address for bytes             */
      goto data16bit;
    /* S5/S6 records ignored; don't think they make any sense here           */
    case '5' :
    case '6' :
      break;
    case '7' :                          /* 32-bit entry point                */
      nAddr = GetHex(f, 8, &chks);      /* get address to jump to            */
      goto entry16bit;
    case '8' :                          /* 24-bit entry point                */
      nAddr = GetHex(f, 6, &chks);      /* get address to jump to            */
      goto entry16bit;
    case '9' :
      nAddr = GetHex(f, 4, &chks);      /* get address to jump to            */
    entry16bit:
      nAddr = (nAddr * interleave) + offset;
      if ((nAddr < GetLowestCodeAddr()) ||  /* if illegal address            */
          (nAddr > GetHighestCodeAddr()))
        { nBytes = -1; break; }         /* return with error                 */
      /* the documentation says "if address isn't needed, use 0".
       * bad idea IMO (they should have allowed to pass NO address instead),
       * but, well ... 0 MIGHT be a valid start address, so we need to live
       * with the ambiguity. */
      load = nAddr;
      done = true;
      break;
    default :
      done = true;
      break;
    }

  /* ignore checksum byte; its calculation would be:
     add up all decoded bytes after the record type,
     take 1's complement of lowest byte */
  GetHex(f, 2, &chks);
  // chks should be 0xff!

  while (((c = fgetc(f)) != EOF) &&     /* skip to newline                   */
         (c != '\r') && (c != '\n'))
    ;

  while (((c = fgetc(f)) != EOF) &&     /* skip newline itself               */
         ((c == '\r') || (c == '\n')))
    ;
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
  sLoadType = "Motorola S";
return (nBytes > 0);                    /* pass back #bytes interpreted      */
}

/*****************************************************************************/
/* LoadBinary : loads opened file as a binary                                */
/*****************************************************************************/

bool Disassembler::LoadBinary
    (
    std::string filename,
    FILE *f,
    std::string &sLoadType,
    int interleave
    )
{
int nCurPos = ftell(f);
// this logic is required, since reads for other file types might have ended
// with an ungetc() call, which could be ruined by the fseek()/ftell() below
// if the file is unseekable
int c = fgetc(f);                       /* read first byte from the file     */
addr_t i, off;
fseek(f, 0, SEEK_END);                  /* get file length                   */
off = ftell(f) - nCurPos;
fseek(f, nCurPos + 1, SEEK_SET);

if (off > 0 &&                          /* restrict to maximum code size     */ 
    offset + (off * interleave) > GetHighestCodeAddr() + 1)
  off = ((GetHighestCodeAddr() + 1) / interleave) - offset;

if (begin < 0 || offset < begin)        /* set begin if not specified        */
  begin = offset;

if (off > 0)                            /* set end if not specified          */
  {
  if (end < offset + (off * interleave) - 1)
    end = offset + (off * interleave) -1;
  AddMemory(begin,                      /* make sure memory is there         */
            off * interleave);
  }

for (i = 0; off < 0 || i < off; i++)    /* mark area as used                 */
  {
  if (c == EOF)                         /* if error, abort reading           */
    {
    if (off > 0)
      fseek(f, nCurPos, SEEK_SET);
    return off < 0;
    }
  addr_t tgtaddr = offset + (i * interleave);
  if (off < 0)                          /* if reading an unseekable stream   */
    AddMemory(tgtaddr, interleave);     /* assure memory                     */
  setat(tgtaddr, (uint8_t)c);           /* otherwise add memory byte         */
  SetCellUsed(tgtaddr);                 /* mark as used byte                 */
  SetDisplay(tgtaddr, defaultDisplay);
  c = fgetc(f);                         /* read next byte from the file      */
  }

sLoadType = "binary";
return true;
}

/*****************************************************************************/
/* LoadFile : loads an opened file                                           */
/*****************************************************************************/

bool Disassembler::LoadFile
    (
    std::string filename,
    FILE *f,
    std::string &sLoadType,
    int interleave
    )
{
return LoadIntelHex(filename, f, sLoadType) ||
       LoadMotorolaHex(filename, f, sLoadType) ||
       LoadBinary(filename, f, sLoadType);
}

/*****************************************************************************/
/* Load : loads a code file into the disassembler                            */
/*****************************************************************************/

bool Disassembler::Load
    (
    std::string filename,
    std::string &sLoadType,
    int interleave
    )
{
sLoadType.clear();
FILE *pFile = (filename == "-") ? stdin : fopen(filename.c_str(), "rb");
if (pFile == NULL)
  return false;
bool bOK = LoadFile(filename, pFile, sLoadType, interleave);
if (pFile != stdin)
  fclose(pFile);
if (bOK)                                /* if loading done,                  */
  offset = end + 1;                     /* prepare for next file             */
return bOK;
}
