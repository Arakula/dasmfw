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

adr_t GetHex(FILE *f, int nChars, uint8_t *pchks)
{
adr_t out = 0;

for (int i = 0; i < nChars; i++)
  {
  int c = fgetc(f);
  if (c == EOF)
    return NO_ADDRESS;
  if ((c >= 'a') && (c <= 'f'))
    c -= ('a' - 'A');
  if ((!((c >= '0') && (c <= '9'))) &&
      (!((c >= 'A') && (c <= 'F'))))
    {
    ungetc(c, f);
    return NO_ADDRESS;
    }
  // ASCII-based. Needs rework for other encodings.
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
MemoryType mt = pAttr ? pAttr->GetMemType() : Untyped;
if (!pAttr ||
    mt == Code ||
    !pAttr->IsUsed())
  return 0;

MemAttribute::Display display = pAttr->GetDisplay();
wf = pAttr->cellSize | SHMF_DATA;       /* assemble flags for data byte      */

if (pAttr->GetSize() != 1 ||
#if RB_VARIANT
     (display != MemAttribute::Char ||
#else
     (display == MemAttribute::Binary ||
#endif
      mt == Const))
  wf |= SHMF_NOTXT;

if (mt == Bss)
  wf |= SHMF_RMB;
else if (!(wf & SHMF_NOTXT))
  {
  // NB: this is totally ASCII-centric, but as long as we don't
  // have to deal with other encodings, it should be OK.
  if ((mem >= 0x20) && (mem <= 0x7e) &&
      (mem != '&') && (mem != '\"'))
    wf |= SHMF_TXT;
  }
if (pAttr->GetBreakBefore())
    wf |= SHMF_BREAK;

if (plbl && plbl->IsUsed())
  {
  // labels containing '+' or '-' are no real labels; they are
  // references to another label (e.g., "OLBL+4")
  string const &s = plbl->GetText();
  if (s.find('-') == s.npos && s.find('+') == s.npos)
    wf |= SHMF_BREAK;
  }

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

Disassembler::Disassembler(Application *pApp)
  : pApp(pApp)
{
begin = load = NO_ADDRESS;
end = offset = 0;
commentStart = ";";
#if RB_VARIANT
labelDelim = ":";
#else
labelDelim = "";
#endif
defaultDisplay = MemAttribute::Hex;
pbase = 16;
disassemblyFlagMask = (uint32_t)-1;
bLoadLabel = true;
bSetSysVec = true;
bMultiLabel = false;
bAutoLabel = false;
bPIC = false;
nMnemoUpper = -1;  // don't care

// set up options table
// base class uses one generic option setter/getter pair (not mandatory)
AddOption("pbase", "{number}\tdefault base for parsing numbers",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("defdisp", "{bin|char|oct|dec|hex}\tdefault number output format",
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
AddOption("loadlabel", "{off|on}\tuse entry point label",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("multilabel", "{off|on}\tallow multiple labels for an address",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("autolabel", "{off|on}\tset labels based on previous text label",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("sysvec", "{off|on}\tset system vector labels",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("pic", "{off|on}\tdiasassemble as position-indpendent code",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
AddOption("upmnemo", "{off|on}\tdiasassemble mnemonics in UPPERCASE format",
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
/* Setup : Setup disassembler after construction                             */
/*****************************************************************************/

bool Disassembler::Setup()
{
bool bOK = true;

busnames.resize(GetBusCount());
memory.resize(GetBusCount());
memattr.resize(GetBusCount());
Labels.resize(GetBusCount());
DefLabels.resize(GetBusCount());
Relatives.resize(GetBusCount());
Phases.resize(GetBusCount());
busorder.resize(GetBusCount());
busbits.resize(GetBusCount());

busnames[BusCode] = "Code";

// default to basic memory attribute handlers
for (int bus = 0; bus < GetBusCount(); bus++)
  {
  memattr[bus] = CreateAttributeHandler();
  busorder[bus] = bus;
  bOK &= !!memattr[bus];
  RecalcBusBits(bus);
  }
return bOK;
}

/*****************************************************************************/
/* GetBusID : return a bus ID based on its name                              */
/*****************************************************************************/

int Disassembler::GetBusID(string busname)
{
bool numeric = !!busname.size();
for (string::size_type s = 0; s < busname.size(); s++)
  if (busname[s] < '0' || busname[s] > '9')
    {
    numeric = false;
    break;
    }
if (numeric)
  {
  int i = atoi(busname.c_str());
  if (i >= 0 && i < GetBusCount())
    return i;
  }
else
  {
  busname = lowercase(busname);
  for (int i = GetBusCount() - 1; i >= 0; i--)
    if (busname == lowercase(GetBusName(i)))
      return i;
  }

return -1;
}

/*****************************************************************************/
/* AddOption : add an option and its handler to the options array            */
/*****************************************************************************/

bool Disassembler::AddOption
    (
    string name,
    string help,
    PSetter setter,
    PGetter getter
    )
{
// allow overriding of base class option handlers
string lname(lowercase(name));
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

int Disassembler::FindOption(string name)
{
string lname(lowercase(name));
for (int i = 0; i < GetOptionCount(); i++)
  if (name == options[i]->name)
    return i;
return -1;
}

/*****************************************************************************/
/* RemoveOption : removes an option if it's there                            */
/*****************************************************************************/

bool Disassembler::RemoveOption(string name)
{
int i = FindOption(name);
if (i < 0)
  return false;

options.erase(options.begin() + i);
return true;
}

/*****************************************************************************/
/* SetOption : sets a disassembler option                                    */
/*****************************************************************************/

int Disassembler::SetOption(string name, string value)
{
string lname(lowercase(name));

for (int i = 0; i < GetOptionCount(); i++)
  if (name == options[i]->name)
    return ((*this).*(options[i]->setter))(name, value);

return 0;
}

/*****************************************************************************/
/* GetOption : retrieves a disassembler option                               */
/*****************************************************************************/

string Disassembler::GetOption(string name)
{
string lname(lowercase(name));

for (int i = 0; i < GetOptionCount(); i++)
  if (name == options[i]->name)
    return ((*this).*(options[i]->getter))(name);

return "";
}

/*****************************************************************************/
/* DisassemblerSetOption : disassembler base class option handler            */
/*****************************************************************************/

int Disassembler::DisassemblerSetOption(string lname, string value)
{
// this could be expanded ... "0x" or "$" header come to mind...
string lvalue(lowercase(value));
adr_t ivalue = (adr_t)strtoul(value.c_str(), NULL, 10);
adr_t avalue;
String2Number(value, avalue);
bool bnValue = true;                    /* default to "on"                   */
bool bIsBool = ParseBool(value, bnValue);

if (lname == "pbase" &&
         ivalue >= 2 && ivalue <= 16)
  pbase = ivalue;
else if (lname == "defdisp")
  {
  MemAttribute::Display disp = MemAttribute::DefaultDisplay;
  if (lvalue == "bin")
    disp = MemAttribute::Binary;
  else if (lvalue == "char")
    disp = MemAttribute::Char;
  else if (lvalue == "oct")
    disp = MemAttribute::Octal;
  else if (lvalue == "dec")
    disp = MemAttribute::Decimal;
  else if (lvalue == "hex")
    disp = MemAttribute::Hex;
  if (disp != MemAttribute::DefaultDisplay)
    SetDisplay(disp);
  }
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
  {
  bLoadLabel = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (lname == "multilabel")
  {
  bMultiLabel = bnValue;
  for (int i = 0; i < GetBusCount(); i++)
    Labels[i].SetMultipleDefs(bMultiLabel);
  return bIsBool ? 1 : 0;
  }
else if (lname == "autolabel")
  {
  bAutoLabel = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (lname == "sysvec")
  {
  bSetSysVec = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (lname == "pic")
  {
  bPIC = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (lname == "upmnemo")
  {
  if (lvalue == "default")
    nMnemoUpper = -1;
  else
    nMnemoUpper = bnValue;
  return bIsBool ? 1 : 0;
  }
else
  return 0;                             /* only option consumed              */

return 1;                               /* option + value consumed           */
}

/*****************************************************************************/
/* DisassemblerGetOption : disassembler base class option retrieval          */
/*****************************************************************************/

string Disassembler::DisassemblerGetOption(string lname)
{
string oval;
if (lname == "pbase") oval = sformat("%ld", pbase);
else if (lname == "defdisp")
  {
#if 0
AddOption("defdisp", "{bin|char|dec|oct|hex|}\tdefault number output format",
          &Disassembler::DisassemblerSetOption,
          &Disassembler::DisassemblerGetOption);
#endif
#ifdef _DEBUG
  MemAttribute::Display disp = GetDisplay();
#endif
  switch (GetDisplay())
    {
    case MemAttribute::Binary :  oval = "bin"; break;
    case MemAttribute::Char :    oval = "char"; break;
    case MemAttribute::Octal :   oval = "oct"; break;
    case MemAttribute::Decimal : oval = "dec"; break;
    case MemAttribute::Hex :     oval = "hex"; break;
    default : break;  // keep gcc happy
    }
  }
else if (lname == "begin") oval = (begin == NO_ADDRESS) ? "-1" : sformat("0x%lx", begin);
else if (lname == "end") oval = (end == NO_ADDRESS) ? "-1" : sformat("0x%lx", end);
else if (lname == "offset") oval = (offset == NO_ADDRESS) ? "-1" : sformat("0x%lx", offset);
else if (lname == "cchar") oval = commentStart;
else if (lname == "ldchar") oval = labelDelim;
else if (lname == "loadlabel") oval = bLoadLabel ? "on" : "off";
else if (lname == "multilabel") oval = bMultiLabel ? "on" : "off";
else if (lname == "autolabel") oval = bAutoLabel ? "on" : "off";
else if (lname == "sysvec") oval = bSetSysVec ? "on" : "off";
else if (lname == "pic") oval = bPIC ? "on" : "off";
else if (lname == "upmnemo") oval =
    (nMnemoUpper < 0) ? "default" :
    (nMnemoUpper > 0) ? "on" :
    "off";
return oval;
}

/*****************************************************************************/
/* IsForwardRef : returns whether label at given address is a forward ref    */
/*****************************************************************************/

bool Disassembler::IsForwardRef
    (
    adr_t value,
    bool bUseLabel,
    adr_t addr,
    int bus
    )
{
// if not using labels, it can be a deflabel at best, and these are always
// output before the code 
if (!bUseLabel)
  return false;

bool bForwardRef = false;
adr_t relative = GetRelative(addr, bus);
Label *pLbl;
MemoryType lblType;
if (relative)
  {
  pLbl = FindLabel(relative, Untyped, bus);
  lblType = pLbl ? pLbl->GetType() : Untyped;
  bForwardRef |= 
    ((lblType == Code || lblType == Data || lblType == Bss) &&
     relative >= addr);
  }
adr_t Wrel = (value + relative);
adr_t hiaddr = GetHighestBusAddr(bus) + 1;
adr_t WrelMod = (hiaddr) ? Wrel % hiaddr : Wrel;
pLbl = FindLabel(Wrel, Untyped, bus);
lblType = pLbl ? pLbl->GetType() : Untyped;
bForwardRef |= 
  ((lblType == Code || lblType == Data || lblType == Bss) &&
   Wrel >= addr);
if (!pLbl && Wrel != WrelMod)
  {
  pLbl = FindLabel(WrelMod, Untyped, bus);
  lblType = pLbl ? pLbl->GetType() : Untyped;
  bForwardRef |= 
    ((lblType == Code || lblType == Data || lblType == Bss) &&
     WrelMod >= addr);
  }

return bForwardRef;
}

/*****************************************************************************/
/* Label2String : converts a value to a (label) string                       */
/*****************************************************************************/

string Disassembler::Label2String
    (
    adr_t value,
    int nDigits,
    bool bUseLabel,
    adr_t addr,
    int bus
    )
{
string sOut;
adr_t relative = GetRelative(addr, bus);
adr_t Wrel = (value + relative);
adr_t hiaddr = GetHighestBusAddr(bus) + 1;
adr_t WrelMod = (hiaddr) ? Wrel % hiaddr : Wrel;
// % might not be the best choice under all circumstances, but works for me...

// NB: this always uses the LAST found label for this address.
// There's no way to find out which should be used for multiples.
Label *pLbl = NULL;
if (bUseLabel)
  {
  pLbl = FindLabel(Wrel, Untyped, bus);
  if (!pLbl && Wrel != WrelMod)
    pLbl = FindLabel(WrelMod, Untyped, bus);
  }
// DefLabel is independent of bUseLabel and is used if no normal label is there
#if 1
if (!pLbl)
  pLbl = FindLabel(WrelMod, Const, bus);
#endif

string sLabel;
if (pLbl)                               /* get label name                    */
  sLabel = pLbl->GetText();
MemoryType memType = pLbl ? pLbl->GetType() : Untyped;
if (pLbl && memType == Untyped)
  memType = GetMemType(WrelMod, bus);
if (memType == Const || memType == Bss)
  memType = Data;
                                        /* if there and absolute             */
if (Wrel == value && sLabel.size())
  return sLabel;                        /* return it                         */

if (sLabel.size())
  sOut = sLabel;
else if (bUseLabel && memType == Code)
  sOut = UnnamedLabel(WrelMod, true, bus);
else if (bUseLabel && memType == Data)
  sOut = UnnamedLabel(WrelMod, false, bus);
else  // no label - use ORIGINAL value
  sOut = Number2String(relative ? Wrel : value, nDigits, addr, bus);

if (relative)                           /* if it's relative addressing       */
  {
  string sAdd("-");
  bool bInvert = true;
  int32_t nDiff = Wrel - value;         /* get difference                    */

                                        /* get base name                     */
  pLbl = (bUseLabel) ? FindLabel(relative, Untyped, bus) : NULL;
  // DefLabel overrides normal labels and is independent of bUseLabel
  if (!pLbl)
    pLbl = FindLabel(relative, Const, bus);
  sLabel = (pLbl) ? pLbl->GetText() : "";
  if (sLabel.size())
    sAdd += sLabel;
  else if (bUseLabel && IsCLabel(relative, bus))
    sAdd += UnnamedLabel(relative, true, bus);
  else if (bUseLabel && IsDLabel(relative, bus))
    sAdd += UnnamedLabel(relative, false, bus);
  else
    {
    if (nDiff < 0)                      /* if negative displacement          */
      {
      sAdd = "+";                       /* negative*negative is positive...  */
      bInvert = false;                  /* so invert the sign                */
                                        /* and make the number positive      */
      relative = (adr_t) (-((int32_t)relative));
      }
    sAdd += Number2String(relative, nDigits, addr, bus);
    }

  if (bInvert)                          /* if inverting necessary,           */
    {
    string::iterator i = sAdd.begin();
    i++;
    while (i != sAdd.end())             /* invert signs!                     */
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
/* AddLabel : adds (or replaces) a label                                     */
/*****************************************************************************/

bool Disassembler::AddLabel
    (
    adr_t addr,
    MemoryType memType,
    string sLabel,
    bool bUsed,
    int bus
    )
{
#if 1
LabelArray::iterator it;
Label *pLbl = GetFirstLabel(addr, it, Untyped, bus);

if (pLbl)
  {
  bool bHasTxt = !!sLabel.size();
#if 0
  bool bMulti = Labels[bus].HasMultipleDefs();
#endif
  bool bInsert = true /*!bMulti*/;
  do
    {
    MemoryType oType = pLbl->GetType();
#if 0
    if (bMulti)
      {
#endif
      bool oHasTxt = pLbl->HasText();
      if ((bHasTxt && sLabel == pLbl->GetText()) ||
          ((!bHasTxt || !oHasTxt) && oType != Const))
        {
        if (!oHasTxt && bHasTxt)
          pLbl->SetText(sLabel);
        // This should deal with "used xxxx / label xxxx txt"
        if (!bHasTxt || !pLbl->IsUsed())
          pLbl->SetUsed(bUsed);         /* just set new used state           */
        if (oType == Untyped &&         /* and new type if none set yet      */
            memType != Const &&
            oType != memType)
          pLbl->SetType(memType);
        bInsert = false;
        }
#if 0
      }
    else
      {
      // In theory, this would also work in Label::CopyUnset(), like this:
      //   if (other.memType != Untyped) memType = other.memType;
      // but since it's more LabelArray- than Label-specific, I'll leave it here
      // override incoming type if already set
      if (oType != Untyped /* && oType != memType */)
        memType = oType;
      }
#endif
    pLbl = GetNextLabel(addr, it, Untyped, bus);
    } while (pLbl);

  if (!bInsert)
    return true;
  }
#endif

Labels[bus].insert(new Label(addr, memType, sLabel, bUsed),
                   true,
                   (memType == Const));
return true;
}

/*****************************************************************************/
/* AddRelativeLabel : add label for addr OR the relative addresses           */
/*****************************************************************************/

// TODO: is that enough? Or does it need 2 busses (for addr and for at)?
bool Disassembler::AddRelativeLabel
    (
    adr_t addr,
    adr_t at,
    MemoryType memType,
    bool bUsed,
    int bus,
    adr_t craddr,                       /* cref address                      */
    int crbus                           /* cref bus                          */
    )
{
(void)bUsed;
adr_t relative = GetRelative(at, bus);
adr_t addrRel = addr + relative;
adr_t hiaddr = GetHighestBusAddr(bus) + 1;
adr_t addrRelMod = (hiaddr) ? addrRel % hiaddr : addrRel;
bool rc;
if (addrRelMod != addr)
  {
#if 0
  // add relative if it is not defined.
  if (AddLabel(relative, memType, "", true, bus) &&
#else
  // don't add relative if it is not defined. If not, it's a constant.
  if (FindLabel(relative, Untyped, bus) &&
#endif
      craddr != NO_ADDRESS)
    SetLabelUsed(relative, memType, bus, craddr, crbus);
#if 0
  // add label in any case
  rc = AddLabel(addrRelMod, memType, "", true, bus);
#else
  // don't add relative if it is not defined. If not, it's a constant.
  // If rel is set but neither label is there, outputs a funny addition...
  rc = !!FindLabel(addrRelMod, Untyped, bus);
#endif
  if (rc && craddr != NO_ADDRESS)
    SetLabelUsed(addrRelMod, memType, bus, craddr, crbus);
  }
else
  {
  rc = AddLabel(addr, memType, "", true, bus);
  if (rc && craddr != NO_ADDRESS)
    SetLabelUsed(addr, memType, bus, craddr, crbus);
  }
return rc;
}

/*****************************************************************************/
/* FindLabel : find label of a given address and type                        */
/*****************************************************************************/

Label *Disassembler::FindLabel
    (
    adr_t addr,
    MemoryType memType,
    int bus
    )
{
LabelArray::iterator it;

// "Untyped" really means "anything but Const" -
// Const is reserved for DefLabels, which have to be searched with that type

// search rules:
// .) if searching for Untyped, return last non-Const
// .) if searching for Const, return last Const
// .) if searching for Code/Data, return last matching or Untyped

Label *found = NULL;
Label *lbl = Labels[bus].GetFirst(addr, it, memType);
while (lbl)
  {
  switch (memType)
    {
    case Const :
      if (lbl->IsConst())
#if 1
        // return LAST matching label (single-def-compatibe)
        found = lbl;
#else
        // return FIRST matching label
        return lbl;
#endif
      break;
    default :
      if (!lbl->IsConst())
#if 1
        // return LAST matching label (single-def-compatibe)
        found = lbl;
#else
        // return FIRST matching label
        return lbl;
#endif
      break;
    }

  lbl = Labels[bus].GetNext(addr, it, memType);
  }
return found;
}

/*****************************************************************************/
/* FindPrevNamedLabel : find previous named label for address and type       */
/*****************************************************************************/

Label *Disassembler::FindPrevNamedLabel
    (
    adr_t addr,
    MemoryType memType,
    int bus
    )
{
LabelArray::iterator it;

// "Untyped" really means "anything but Const" -
// Const is reserved for DefLabels, which have to be searched with that type

// search rules:
// .) if searching for Untyped, return last non-Const
// .) if searching for Const, return last Const
// .) if searching for Code/Data, return last matching or Untyped

Label *found = NULL;
Label *lbl = Labels[bus].GetPrevNamed(addr, it, memType);
while (lbl)
  {
  switch (memType)
    {
    case Const :
      if (lbl->IsConst())
#if 1
        // return LAST matching label (single-def-compatibe)
        found = lbl;
#else
        // return FIRST matching label
        return lbl;
#endif
      break;
    // case Untyped :
    default :
      if (!lbl->IsConst())
#if 1
        // return LAST matching label (single-def-compatibe)
        found = lbl;
#else
        // return FIRST matching label
        return lbl;
#endif
      break;
    }

  lbl = Labels[bus].GetNext(addr, it, memType);
  if (found && lbl && (lbl->GetAddress() != found->GetAddress()))
    break;
  }
return found;
}

/*****************************************************************************/
/* ResolveLabels : resolve all XXXXXXXX+/-nnnn and definition labels         */
/*****************************************************************************/

bool Disassembler::ResolveLabels(int bus)
{
for (int i = GetLabelCount(bus) - 1; i >= 0; i--)
  {
  Label *pLbl = LabelAt(i, bus);
  string s = pLbl->GetText();
  string::size_type p = s.find_first_of("+-");
  if (!pLbl->IsUsed())
    continue;
  // first, check for +/-nnnn offset
  adr_t offs;
  if (p != s.npos &&
//    String2Number(s.substr(p + 1), offs))
      sscanf(s.substr(p + 1).c_str(), "%d", &offs) == 1)
    {
    if (s[p] == '+') offs = (adr_t)(-(int32_t)offs);
    pLbl->SetUsed(false);
    adr_t addaddr = pLbl->GetAddress() + offs;
    AddLabel(addaddr, pLbl->GetType(),
             s.substr(0, p), true, bus);
    // copy all references to the base label
    Label *pBaseLbl = FindLabel(addaddr, pLbl->GetType(), bus);
    if (pBaseLbl)
      {
      for (size_t refi = 0; refi < pLbl->RefCount(); refi++)
        {
        AddrBus &ref = pLbl->GetRef(refi);
        pBaseLbl->SetUsed(true, ref.addr, ref.bus);
        }
      }

    // this might have caused an insertion, so restart here
    i++;
    continue;
    }
  // then, resolve DefLabels without value
  if (pLbl->IsConst())
    {
    adr_t addr = pLbl->GetAddress();
    MemoryType memType = GetMemType(addr, bus);
    int sz = GetCellSize(addr, bus);
    if (memType != Untyped && memType != Bss && !IsFloat(addr, bus))
      {
      if (s.size() > 0)
        {
        adr_t value = GetTypedInt(addr, bus);
        // insert with last found value
        AddDefLabel(addr,
                    s,
                    Number2String(value, sz * 2, addr, bus),
                    Const, bus);
        }
      }
    }
  }

return true;
}

/*****************************************************************************/
/* DefLabel2String : DefLabel or numeric constant to string                  */
/*****************************************************************************/

string Disassembler::DefLabel2String
    (
    adr_t value,
    int nDigits,
    adr_t addr,
    int bus
    )
{
string svalue = Number2String(value, nDigits, addr, bus);
// NB: this always uses the FIRST found label for this address.
// There's no way to find out which should be used for multiples.
Label *pLabel = FindLabel(addr, Const, bus);
if (pLabel)
  {
  string dname = pLabel->GetText();
  if (DefLabels[bus].Find(dname))
    svalue = dname;
  }
return svalue;
}

/*****************************************************************************/
/* AutoLabel2String : automatic label generation based on previous label     */
/*****************************************************************************/

string Disassembler::AutoLabel2String(adr_t addr, bool bCode, int bus)
{
(void)bCode;  // unused ATM
Label *pLabel = FindPrevNamedLabel(addr, Untyped /* Const */, bus);
if (pLabel)
  {
  adr_t off = addr - pLabel->GetAddress();
  return sformat("O%x_%s", off, pLabel->GetText().c_str());
  }
return "";
}

/*****************************************************************************/
/* GetTypedInt : get memory from given address defined by cell type/size     */
/*****************************************************************************/

adr_t Disassembler::GetTypedInt(adr_t addr, int bus)
{
MemoryType memType = GetMemType(addr, bus);
int sz = GetCellSize(addr, bus);
if (memType == Untyped || IsFloat(addr, bus)) return 0;
bool bSigned = IsSigned(addr, bus);
switch (sz)
  {
  case 1 : return bSigned ? GetSByte(addr, bus) : GetUByte(addr, bus);
  case 2 : return bSigned ? GetSWord(addr, bus) : GetUWord(addr, bus);
  case 4 : return bSigned ? GetSDWord(addr, bus) : GetUDWord(addr, bus);
#if (ADR_T_SIZE >= 8)
  case 8 : return bSigned ? GetSQWord(addr, bus) : GetUQWord(addr, bus);
#endif
  default :
    return 0;
  }
}

/*****************************************************************************/
/* GetTypedInt : set memory at given address defined by cell type/size       */
/*****************************************************************************/

void Disassembler::SetTypedInt(adr_t value, adr_t addr, int bus)
{
MemoryType memType = GetMemType(addr, bus);
int sz = GetCellSize(addr, bus);
bool bSigned = IsSigned(addr, bus);
if (memType == Untyped || IsFloat(addr, bus)) return;

switch (sz)
  {
  case 1 :
    if (bSigned)
      SetSByte(addr, (int8_t)value, bus);
    else
      SetUByte(addr, (uint8_t)value, bus);
    break;
  case 2 :
    if (bSigned)
      SetSWord(addr, (int16_t)value, bus);
    else
      SetUWord(addr, (uint16_t)value, bus);
    break;
  case 4 :
    if (bSigned)
      SetSDWord(addr, (int32_t)value, bus);
    else
      SetUDWord(addr, (uint32_t)value, bus);
    break;
#if (ADR_T_SIZE >= 8)
  case 8 :
    if (bSigned)
      SetSQWord(addr, (int64_t)value, bus);
    else
      SetUQWord(addr, (uint64_t)value, bus);
    break;
#endif
  }
}

/*****************************************************************************/
/* LoadIntelHex : tries to load as an Intel HEX file                         */
/*****************************************************************************/

bool Disassembler::LoadIntelHex
    (
    string filename,
    FILE *f,
    string &sLoadType,
    int interleave,
    int bus
    )
{
(void)filename;
int nCurPos = ftell(f);
int c = 0;

if ((c = fgetc(f)) == EOF)              /* look whether starting with ':'    */
  return false;
if (c != ':')
  {
  ungetc(c, f);
  return false;
  }

int rectype;
bool done = false;
int nBytes = 0;
adr_t fbegin = GetHighestBusAddr(bus);
adr_t fend = GetLowestBusAddr(bus);
int segment = 0;                        /* segment address                   */
MemoryType memType = GetDefaultMemoryType(bus);

while (!done && (nBytes >= 0))          /* while there are lines             */
  {
  adr_t nAddr;
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
                                        /* if illegal address                */
  if ((nAddr < GetLowestBusAddr(bus)) ||
      (nAddr + nSpreadBytes > GetHighestBusAddr(bus)))
    { nBytes = -1; break; }             /* return with error                 */
  if (nAddr < fbegin)                   /* adjust start and end values       */
    {
    fbegin = nAddr;
    AddMemory(nAddr, nSpreadBytes, memType, 0, bus);
    }
  if (nAddr + nSpreadBytes - 1 > fend)
    {
    AddMemory(nAddr, nSpreadBytes, memType, 0, bus);
    fend = nAddr + nSpreadBytes - 1;
    }
  nBytes += nBytesOnLine;
  rectype = GetHex(f, 2, &chks);        /* fetch record type character       */
  switch (rectype)                      /* which type of record is this?     */
    {
    case 0 :                            /* data record                       */
      for (i = 0; i<nBytesOnLine; i++)  /* now get the bytes                 */
        {
        adr_t tgtaddr = nAddr + (i * interleave);
        c = GetHex(f, 2, &chks);        /* retrieve a byte                   */
        if ((c < 0) || (c > 0xff))      /* if illegal byte                   */
          { nBytes = -1; break; }       /* return with error                 */
                                        /* otherwise add memory byte         */
        setat(tgtaddr, (uint8_t)c, bus);
        SetCellUsed(tgtaddr, true, bus);
        SetDisplay(tgtaddr, defaultDisplay, bus);
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
                                        /* if illegal address                */
      if ((nAddr < GetLowestBusAddr(bus)) ||
           (nAddr > GetHighestBusAddr(bus)))
        nBytes = -1;                    /* return with error                 */
      else if (bus == BusCode)
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
    string filename,
    FILE *f,
    string &sLoadType,
    int interleave,
    int bus
    )
{
(void)filename;
int nCurPos = ftell(f);
int c = 0;

if ((c = fgetc(f)) == EOF)              /* look whether starting with 'S'    */
  return false;
if (c != 'S')
  {
  ungetc(c, f);
  return false;
  }

bool done = false;
int nBytes = 0;
adr_t fbegin = GetHighestBusAddr(bus);
adr_t fend = GetLowestBusAddr(bus);
MemoryType memType = GetDefaultMemoryType(bus);

while ((!done) && (nBytes >= 0))        /* while there are lines             */
  {
  int nLineType = 0, nBytesOnLine, i;
  adr_t nAddr;
  uint8_t chks = 0;
  if (c != 'S')
    break;
  if (fread(&nLineType, 1, 1, f) != 1)  /* retrieve line type                */
    { nBytes = -1; break; }             /* return with error                 */
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
                                        /* if illegal address                */
      if ((nAddr < GetLowestBusAddr(bus)) ||
           (nAddr + nSpreadBytes > GetHighestBusAddr(bus)))
        { nBytes = -1; break; }         /* return with error                 */
      if (nAddr < fbegin)               /* adjust start and end values       */
        {
        fbegin = nAddr;
        AddMemory(nAddr, nSpreadBytes, memType, 0, bus);
        }
      if (nAddr + nSpreadBytes - 1 > fend)
        {
        fend = nAddr + nSpreadBytes - 1;
        AddMemory(nAddr, nSpreadBytes, memType, 0, bus);
        }
      nBytes += nBytesOnLine;
                                        /* now get the bytes                 */
      for (i = 0; i < nBytesOnLine; i++)
        {
        adr_t tgtaddr = nAddr + (i * interleave);
        c = GetHex(f, 2, &chks);        /* retrieve a byte                   */
        if ((c < 0) || (c > 0xff))      /* if illegal byte                   */
          { nBytes = -1; break; }       /* return with error                 */
                                        /* otherwise add memory byte         */
        setat(tgtaddr, (uint8_t)c, bus);
        SetCellUsed(tgtaddr, true, bus);
        SetDisplay(tgtaddr, defaultDisplay, bus);
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
                                        /* if illegal address                */
      if ((nAddr < GetLowestBusAddr(bus)) ||
          (nAddr > GetHighestBusAddr(bus)))
        { nBytes = -1; break; }         /* return with error                 */
      /* the documentation says "if address isn't needed, use 0".
       * bad idea IMO (they should have allowed to pass NO address instead),
       * but, well ... 0 MIGHT be a valid start address, so we need to live
       * with the ambiguity. */
      if (bus == BusCode)
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
    string filename,
    FILE *f,
    string &sLoadType,
    int interleave,
    int bus
    )
{
(void)filename;
int nCurPos = ftell(f);
// this logic is required, since reads for other file types might have ended
// with an ungetc() call, which could be ruined by the fseek()/ftell() below
// if the file is unseekable
int c = fgetc(f);                       /* read first byte from the file     */
adr_t i, off;
MemoryType memType = GetDefaultMemoryType(bus);

fseek(f, 0, SEEK_END);                  /* get file length                   */
off = ftell(f) - nCurPos;
fseek(f, nCurPos + 1, SEEK_SET);

if ((sadr_t)off > 0 &&                  /* restrict to maximum code size     */ 
    offset + (off * interleave) > GetHighestBusAddr(bus) + 1)
  off = ((GetHighestBusAddr(bus) + 1) / interleave) - offset;

if (begin == NO_ADDRESS ||              /* set begin if not specified        */
    begin < offset)
// if (begin < 0 || offset < begin)  // we allow begin > offset !
  begin = offset;

if ((sadr_t)off > 0)                    /* if file size can be determined    */
  {
  if (end == 0 || end < begin)          /* set end if not specified          */
//  if (end < offset + (off * interleave) - 1)
    end = offset + (off * interleave) - 1;
  if (end > offset && end < offset + (off * interleave) -1)
    off = (end + 1 - offset + interleave - 1) / interleave;
  // account for begin > offset scenarios
  adr_t memoff = begin > offset ? begin - offset : 0;
  AddMemory(begin,                      /* make sure memory is there         */
            (off - 1) * interleave + 1 - memoff, memType, 0, bus);
  }
                                        /* mark area as used                 */
for (i = 0; (sadr_t)off < 0 || i < off; i++)
  {
  if (c == EOF)                         /* if error, abort reading           */
    {
    if (off > 0)
      fseek(f, nCurPos, SEEK_SET);
    return (sadr_t)off < 0;
    }
  adr_t tgtaddr = offset + (i * interleave);
  if (tgtaddr >= begin &&
      tgtaddr <= end)
    {
    if ((sadr_t)off < 0)              /* if reading an unseekable stream   */
      AddMemory(tgtaddr, interleave,  /* assure memory                     */
                memType, 0, bus);
    setat(tgtaddr, (uint8_t)c, bus);  /* put byte                          */
    SetCellUsed(tgtaddr, true, bus);  /* mark as used byte                 */
    SetDisplay(tgtaddr, defaultDisplay, bus);
    }
  c = fgetc(f);                       /* read next byte from the file      */
  }

sLoadType = "binary";
return true;
}

/*****************************************************************************/
/* LoadFile : loads an opened file                                           */
/*****************************************************************************/

bool Disassembler::LoadFile
    (
    string filename,
    FILE *f,
    string &sLoadType,
    int interleave,
    int bus
    )
{
return LoadIntelHex(filename, f, sLoadType, interleave, bus) ||
       LoadMotorolaHex(filename, f, sLoadType, interleave, bus) ||
       LoadBinary(filename, f, sLoadType, interleave, bus);
}

/*****************************************************************************/
/* Load : loads a code file into the disassembler                            */
/*****************************************************************************/

bool Disassembler::Load
    (
    string filename,
    string &sLoadType,
    int interleave,
    int bus
    )
{
sLoadType.clear();
FILE *pFile = (filename == "-") ? stdin : fopen(filename.c_str(), "rb");
if (pFile == NULL)
  return false;
bool bOK = LoadFile(filename, pFile, sLoadType, interleave, bus);
if (pFile != stdin)
  fclose(pFile);
if (bOK)                                /* if loading done,                  */
  offset = end + 1;                     /* prepare for next file             */
return bOK;
}

/*****************************************************************************/
/* GetConsecutiveData : calculate consecutive data range                     */
/*                      (i.e., same type for all)                            */
/*****************************************************************************/

adr_t Disassembler::GetConsecutiveData
    (
    adr_t addr,
    uint32_t &flags,
    int maxparmlen,
    int bus
    )
{
adr_t end;
adr_t maxaddr = GetHighestBusAddr(bus);
                                        /* get flags for current byte        */
flags = GetDisassemblyFlags(addr, bus) &
        (~(SHMF_BREAK |                 /* without break flag                */
           SHMF_NOTXT));                /* and without NoText flag           */
      flags &= disassemblyFlagMask;
// safety fuse - process no more than maxparmlen at a time unless it's
// RMB. This may still be too much, but should not be too far off.
if (!(flags & SHMF_RMB) &&
    addr + (adr_t)maxparmlen > addr &&
    addr + (adr_t)maxparmlen <= maxaddr)
  maxaddr = addr + (adr_t)maxparmlen - 1;

for (end = addr + 1;                    /* find end of block                 */
     end > addr && end <= maxaddr;
     end++)
  {
  uint32_t fEnd = GetDisassemblyFlags(end, bus);
  fEnd &= ~SHMF_NOTXT;
  fEnd &= disassemblyFlagMask;
  if (fEnd != flags)
    break;
  }
if (flags & 0xff)                       /* if not 1-sized,                   */
  {
  int dsz = (int)(flags & 0xff) + 1;
  adr_t rest = (end - addr) % dsz;
  if (rest)                             /* don't use incomplete last item    */
    end -= rest;
  if (end == addr)                      /* if there's nothing left           */
    end = addr + dsz;                   /* use at least 1 item               */
  // NB: defining n-byte fields starting at less than (n-1) bytes
  // before the end of the highest memory address will produce garbage,
  // but... well... GIGO. Should be caught by info file parser anyway.
  }
return end;                             /* pass back last + 1                */
}
