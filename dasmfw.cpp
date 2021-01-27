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
/* dasmfw.cpp : main program for the DisASseMbler FrameWork                  */
/*****************************************************************************/

#include "dasmfw.h"
#include "Disassembler.h"
#include "Label.h"
#include "Memory.h"

/*****************************************************************************/
/* Global Data                                                               */
/*****************************************************************************/

static struct DisassemblerCreators      /* structure for dasm creators:      */
  {
  string name;                          /* code name of the disassembler     */
  Disassembler *(*Factory)();           /* factory to create it              */
  } *Disassemblers = NULL;
static int nRegDisassemblers = 0;       /* # registered disassemblers        */
static int nAllocDisassemblers = 0;     /* # allocated disassembler structs  */

/*****************************************************************************/
/* RegisterDisassembler : registers a disassembler                           */
/*****************************************************************************/

bool RegisterDisassembler(string name, Disassembler *(*CreateDisassembler)())
{
if (nRegDisassemblers + 1 > nAllocDisassemblers)
  {
  DisassemblerCreators *pNew;
  try
    {
    pNew = new DisassemblerCreators[nRegDisassemblers + 10];
    if (!pNew)  // deal with very old style allocator
      return false;
    for (int i = nRegDisassemblers; i < nRegDisassemblers + 10; i++)
      pNew[i].Factory = NULL;;
    nAllocDisassemblers = nRegDisassemblers + 10;
    }
  catch(...)
    {
    return false;
    }
  for (int i = 0; i < nRegDisassemblers; i++)
    {
    pNew[i].name = Disassemblers[i].name;
    pNew[i].Factory = Disassemblers[i].Factory;
    }
  if (Disassemblers) delete[] Disassemblers;
  Disassemblers = pNew;
  }

// simple insertion sort will do
for (int i = 0; i < nRegDisassemblers; i++)
  {
  if (Disassemblers[i].name > name)
    {
    for (int j = nRegDisassemblers; j > i; j--)
      {
      Disassemblers[j].name = Disassemblers[j - 1].name;
      Disassemblers[j].Factory = Disassemblers[j - 1].Factory;
      }
    nRegDisassemblers++;
    Disassemblers[i].name = name;
    Disassemblers[i].Factory = CreateDisassembler;
    return true;
    }
  }
Disassemblers[nRegDisassemblers].name = name;
Disassemblers[nRegDisassemblers++].Factory = CreateDisassembler;
return true;
}

/*****************************************************************************/
/* CreateDisassembler : creates a disassembler with a given code name        */
/*****************************************************************************/

Disassembler *CreateDisassembler(string name, int *pidx = NULL)
{
for (int da = 0; da < nRegDisassemblers; da++)
  if (Disassemblers[da].name == name)
    {
    if (pidx) *pidx = da;
    return Disassemblers[da].Factory();
    }
return NULL;
}

/*****************************************************************************/
/* ListDisassemblers : lists out all registered disassemblers                */
/*****************************************************************************/

static void ListDisassemblers(int indent = 0)
{
printf("%*sRegistered disassemblers:\n"
       "%*s%-8s %-16s %s\n"
       "%*s--------------------------------------------------------------\n",
       indent, "",
       indent, "", "Code", "Instruction Set", "Attributes",
       indent, "");
for (int da = 0; da < nRegDisassemblers; da++)
  {
  Disassembler *pDasm = Disassemblers[da].Factory();
  printf("%*s%-8s %-16s (%d/%d bits data/code, %s-endian",
         indent, "",
         Disassemblers[da].name.c_str(),
         pDasm->GetName().c_str(),
         pDasm->GetDataBits(),
         pDasm->GetCodeBits(),
         pDasm->GetEndianness() == Disassembler::BigEndian ? "big" : "little");
  if (pDasm->GetArchitecture() == Disassembler::Harvard)
    printf(", separate data bus");
  printf(")\n");
  delete pDasm;
  }
}

/*****************************************************************************/
/* sformat : sprintf for string                                              */
/*****************************************************************************/

string sformat(const string fmt_str, ...)
{
int final_n, n = ((int)fmt_str.size()) * 2;
vector<char> formatted;
va_list ap;
while (1)
  {
  formatted.resize(n);
  va_start(ap, fmt_str);
  final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
  va_end(ap);
  if (final_n < 0 || final_n >= n)
    n += abs(final_n - n + 1);
  else
    break;
  }
return string(&formatted[0]);
}

/*****************************************************************************/
/* lowercase : create lowercase copy of a string                             */
/*****************************************************************************/

string lowercase(string s)
{
string sout;
// primitive ASCII implementation
for (string::size_type i = 0; i < s.size(); i++)
  sout += tolower(s[i]);
return sout;
}

/*****************************************************************************/
/* uppercase : create uppercase copy of a string                             */
/*****************************************************************************/

string uppercase(string s)
{
string sout;
// primitive ASCII implementation
for (string::size_type i = 0; i < s.size(); i++)
  sout += toupper(s[i]);
return sout;
}

/*****************************************************************************/
/* ltrim : remove leading blanks from string                                 */
/*****************************************************************************/

string ltrim(string s)
{
if (s.empty()) return s;
string::size_type from = s.find_first_not_of(" ");
if (from == s.npos)
  return "";
return from ? s.substr(from) : s;
}

/*****************************************************************************/
/* trim : remove leading and trailing blanks from string                     */
/*****************************************************************************/

string trim(string s)
{
if (s.empty()) return s;
string::size_type from = s.find_first_not_of(" ");
if (from == s.npos)
  return "";
return s.substr(from, s.find_last_not_of(" ") - from + 1);
}

/*****************************************************************************/
/* main : main program function                                              */
/*****************************************************************************/

int main(int argc, char* argv[])
{
// let's go object-oriented, shall we? :-)
return Application(argc, argv).Run();
}



/*===========================================================================*/
/* Application class members                                                 */
/*===========================================================================*/

/*****************************************************************************/
/* Application : constructor                                                 */
/*****************************************************************************/

Application::Application(int argc, char* argv[])
  : argc(argc), argv(argv)

{
pDasm = NULL;                           /* selected disassembler             */
iDasm = -1;                             /* index of selected disassembler    */
abortHelp = false;                      /* abort after help has been given   */
infoBus = BusCode;                      /* start with code bus               */
showLogo = true;                        /* show logo in output file          */
showHex = true;                         /* flag for hex data display         */
showAddr = true;                        /* flag for address display          */
showComments = true;                    /* flag for comment display          */
showCref = false;                       /* flag for cross-reference display  */
f9dasmComp = false;                     /* flag for f9dasm compatibility     */
labelLen = 8;                           /* minimum label length              */
lLabelLen = 8;                          /* minimum EQU label length          */
mnemoLen = 8;                           /* minimum mnemonics length          */
cparmLen = 24;                          /* min parm len with lcomment        */
uparmLen = 52;                          /* max parm len without lcomment     */
dbCount = 5;                            /* min code bytes for hex/asc dump   */
#if RB_VARIANT
showAsc = false;                        /* flag for ASCII content display    */
#else
showAsc = true;                         /* flag for ASCII content display    */
#endif
showUnused = false;                     /* don't show unused labels          */
out = stdout;

// pre-load matching disassembler, if possible
sDasmName = argv[0];
string::size_type idx = sDasmName.find_last_of("\\/");
if (idx != sDasmName.npos)
  sDasmName = sDasmName.substr(idx + 1);
idx = sDasmName.rfind('.');
if (idx != sDasmName.npos)
  sDasmName = sDasmName.substr(0, idx);
// if the name might be "dasm"+code, try to use this disassembler
if (lowercase(sDasmName.substr(0, 4)) == "dasm")
  pDasm = CreateDisassembler(sDasmName.substr(4), &iDasm);
}

/*****************************************************************************/
/* ~Application : destructor                                                 */
/*****************************************************************************/

Application::~Application()
{
}

/*****************************************************************************/
/* Run : the application's main functionality                                */
/*****************************************************************************/

int Application::Run()
{
if (argc < 2)                           /* if no arguments given, give help  */
  {
  printf("%s: Disassembler Framework V%s\n", sDasmName.c_str(), DASMFW_VERSION);
  return Help();                        /* and exit                          */
  }

string defnfo[2];                       /* global / user default info files  */
#ifdef _MSC_VER
// Microsoft C - guaranteed Windows environment
char const *pHomePath = getenv("USERPROFILE");
defnfo[0] = pHomePath ?
    string(pHomePath) + "\\.dasmfw\\" + sDasmName + ".nfo" :
    "";
#else
char const *pHomePath = getenv("HOME");
defnfo[0] = pHomePath ?
    string(pHomePath) + "/.dasmfw/" + sDasmName + ".nfo" :
    "";
#endif
defnfo[1] = sDasmName + ".nfo";

int i;
int bus;
                                        /* first, just search for "dasm"     */
for (i = 0; i < _countof(defnfo); i++)
  if (!defnfo[i].empty())
    ParseOption("info", defnfo[i], true, false);
ParseOptions(argc, argv, true);
if (!pDasm)                             /* disassembler must be loaded now   */
  {
  printf("%s: Disassembler Framework V%s\n", sDasmName.c_str(), DASMFW_VERSION);
  return Help();                        /* if not, display help & exit       */
  }

remaps.resize(pDasm->GetBusCount());    /* set up all arrays                 */
comments[0].resize(pDasm->GetBusCount());
comments[1].resize(pDasm->GetBusCount());
lcomments.resize(pDasm->GetBusCount());

                                        /* then parse all other options      */
for (i = 0; i < _countof(defnfo); i++)  /* for data & info files to load     */
  if (!defnfo[i].empty())
    ParseOption("info", defnfo[i], false, false);
ParseOptions(argc, argv, false);

if (showLogo)
  printf("%s: Disassembler Framework V%s\n", sDasmName.c_str(), DASMFW_VERSION);

if (abortHelp)                          /* help has been given, so end it    */
  return 1;

LoadFiles();                            /* load all data files               */

LoadInfoFiles();                        /* load all info files               */

// parse labels in 2 passes
for (i = 0; i < pDasm->GetBusCount(); i++)
  {
  bus = pDasm->GetBus(i);
  if (pDasm->GetMemoryArrayCount(bus)) Parse(0, bus);
  }
for (i = 0; i < pDasm->GetBusCount(); i++)
  {
  bus = pDasm->GetBus(i);
  if (pDasm->GetMemoryArrayCount(bus)) Parse(1, bus);
  }

// resolve all XXXXXXXX+/-nnnn labels and DefLabels
for (i = 0; i < pDasm->GetBusCount(); i++)
  {
  bus = pDasm->GetBus(i);
  pDasm->ResolveLabels(bus);
  }

// set up often used texts and flags
string labelDelim = pDasm->GetOption("ldchar");
string sComDel = pDasm->GetOption("cchar");
string sComBlk = sComDel + " ";
string sComHdr(sComDel);
while (sComHdr.size() < 53)
  sComHdr += '*';

// create output file
out = outname.size() ? fopen(outname.c_str(), "w") : stdout;
if (out == NULL)
  out = stdout;
if (out != stdout && showLogo)          /* if output goes to file            */
  {                                     /* write header details              */
  PrintLine(sComDel +
            sformat(" %s: Disassembler Framework V" DASMFW_VERSION,
                    sDasmName.c_str()));
  for (i = 0;                           /* print loaded files                */
       i < (int)saFNames.size();
       i++)
    if (saFNames[i].substr(0, 1) != "-")
      PrintLine(sComDel + " " + saFNames[i]);
  for (i = 0;                           /* print loaded info files           */
       i < (int)saINames.size();
       i++)
  PrintLine(sComDel + " " + saINames[i]);
  PrintLine();
  }

//#ifdef _DEBUG
#if 0
PrintLine();
PrintLine(sComBlk + "After Parsing:");
PrintLine(sComBlk +
          sformat("Loaded code memory areas: begin=%s, end=%s",
                  pDasm->GetOption("begin").c_str(),
                  pDasm->GetOption("end").c_str()));
for (i = 0; i < pDasm->GetBusCount(); i++)
  DumpMem(i);
#endif
                                        /* show comments that go in front    */
for (i = 0; i < pDasm->GetBusCount(); i++)
  {
  bus = pDasm->GetBus(i);
  DisassembleComments(NO_ADDRESS, false, sComDel, bus);
  }

// disassembler-specific initialization
DisassembleChanges(NO_ADDRESS, NO_ADDRESS, 0, false, 0);

// output of (def)labels without data
for (i = 0; i < pDasm->GetBusCount(); i++)
  {
  bus = pDasm->GetBus(i);
  DisassembleLabels(sComDel, sComHdr, bus);
  DisassembleDefLabels(sComDel, sComHdr, bus);
  }

// disassemble all memory areas for the busses
for (i = 0; i < pDasm->GetBusCount(); i++)
  {
  bus = pDasm->GetBus(i);
  if (!pDasm->GetMemoryArrayCount(bus))
    continue;

  addr_t prevaddr = NO_ADDRESS, prevsz = 0;
  addr_t addr = pDasm->GetMemoryArray(0, bus).GetStart();
  if (!pDasm->IsCellUsed(addr, bus))
    addr = pDasm->GetNextAddr(addr, bus);
  bool bBusHdrOut = false;
  while (addr != NO_ADDRESS)
    {
    if (!bBusHdrOut)
      {
      PrintLine();
      PrintLine(sComHdr);
      PrintLine(sComBlk +
                sformat("Program's %s Areas",
                        pDasm->GetBusName().c_str()));
      PrintLine(sComHdr);
      PrintLine();

      bBusHdrOut = true;
      }

    DisassembleChanges(addr, prevaddr, prevsz, false, bus);

    addr_t sz = DisassembleLine(addr, sComDel, sComHdr, labelDelim, bus);

    DisassembleChanges(addr, prevaddr, prevsz, true, bus);

    prevaddr = addr;
    prevsz = sz;
    addr = pDasm->GetNextAddr(addr + sz - 1, bus);
    }

  // disassembler-specific end of area handling
  DisassembleChanges(addr, prevaddr, prevsz, false, bus);
  }

// disassembler-specific closing
DisassembleChanges(NO_ADDRESS, NO_ADDRESS, 0, true, pDasm->GetBusCount());

if (out != stdout)
  fclose(out);
if (pDasm) delete pDasm;
return 0;
}

/*****************************************************************************/
/* LoadFiles : load all data files referenced on command line or from info   */
/*****************************************************************************/

bool Application::LoadFiles()
{
bool bAllOK = true;

infoBus = BusCode;                      /* start with code bus               */

int nInterleave = 1;
for (int i = 0;                         /* load file(s) given on commandline */
     i < (int)saFNames.size();          /* and parsed from info files        */
     i++)
  {                                     /* and load-relevant option settings */
  if (saFNames[i].substr(0, 7) == "-begin:")
    ParseOption("begin", saFNames[i].substr(7));
  else if (saFNames[i].substr(0, 5) == "-end:")
    ParseOption("end", saFNames[i].substr(5));
  else if (saFNames[i].substr(0, 8) == "-offset:")
    ParseOption("offset", saFNames[i].substr(8));
  else if (saFNames[i].substr(0, 12) == "-interleave:")
    nInterleave = atoi(saFNames[i].substr(12).c_str());
  else if (saFNames[i].substr(0, 5) == "-bus:")
    ParseOption("bus", saFNames[i].substr(5));
  else
    {
    string sLoadType;
    bool bOK = pDasm->Load(saFNames[i], sLoadType, nInterleave, infoBus);
    saFNames[i] = sformat("%soaded: %s file \"%s\"",
                          bOK ? "L" : "NOT l",
                          sLoadType.c_str(),
                          saFNames[i].c_str());
    if (nInterleave != 1)
      saFNames[i] += sformat(" (interleave=%d)", nInterleave);
#ifndef _DEBUG
    if (showLogo)
      printf("%s\n", saFNames[i].c_str());
#endif
    bAllOK &= bOK;
    }
#ifdef _DEBUG
  if (showLogo)
    printf("%s\n", saFNames[i].c_str());
#endif
  }

return bAllOK;
}

/*****************************************************************************/
/* LoadInfoFiles : fully process all info files                              */
/*****************************************************************************/

bool Application::LoadInfoFiles()
{
bool bAllOK = true;

infoBus = BusCode;                      /* start with code bus               */

for (int i = 0;
     i < (int)saINames.size();
     i++)
  {
  bool bOK = LoadInfo(saINames[i]);
  saINames[i] = sformat("%soaded: Info file \"%s\"",
                         bOK ? "L" : "NOT l",
                         saINames[i].c_str());
  if (showLogo)
    printf("%s\n", saINames[i].c_str());
  bAllOK &= bOK;
  }

return bAllOK;
}

/*****************************************************************************/
/* Parse : go through the loaded memory areas and parse all labels           */
/*****************************************************************************/

bool Application::Parse(int nPass, int bus)
{
if (!nPass &&
    !pDasm->InitParse(bus))
  return false;

if (!pDasm->GetMemoryArrayCount(bus))
  return true;

addr_t prevaddr = NO_ADDRESS;
addr_t addr = pDasm->GetMemoryArray(0, bus).GetStart();
if (!pDasm->IsCellUsed(addr, bus))
  addr = pDasm->GetNextAddr(addr, bus);

while (addr != NO_ADDRESS)
  {
  MemAttribute::Type oct = pDasm->GetCellType(addr, bus);
  addr_t sz = pDasm->Parse(addr, bus);
  MemAttribute::Type nct = pDasm->GetCellType(addr, bus);
  // cell type has been changed by parser?
  if (oct != nct)
    {
    // code changed to untyped data ==> illegal instruction
    if (nct == MemAttribute::CellUntyped)
      {
      if (nPass == 0)
        AddLComment(addr, "Illegal instruction!", false, bus);
      }
    }
  prevaddr = addr;
  addr = pDasm->GetNextAddr(addr + sz - 1, bus);
  }

return true;
}

/*****************************************************************************/
/* DisassembleComments : show all comments for this line                     */
/*****************************************************************************/

bool Application::DisassembleComments
    (
    addr_t addr,
    bool bAfterLine,
    string sComDel,
    int bus
    )
{
CommentArray::iterator it;
sComDel += " ";
Comment *pComment = GetFirstComment(addr, it, bAfterLine, bus);
while (pComment)
  {
  string sTxt(pComment->GetText());
  string sHdr((pComment->IsComment() && sTxt.size()) ? sComDel : "");
  if (showComments || !pComment->IsComment())
    PrintLine(sHdr + sTxt);
  pComment = GetNextComment(addr, it, bAfterLine, bus);
  }
return true;
}

/*****************************************************************************/
/* DisassembleCref : show all cross-references for this line                 */
/*****************************************************************************/

bool Application::DisassembleCref(Label *pLabel, string sComDel, int bus)
{
if (!showCref || !pLabel || !pLabel->RefCount())
  return false;
pLabel->SortRefs();
string sLine(sComDel + " Xref: ");
AddrBus addrp(NO_ADDRESS, BusCode);
size_t i, j;
for (i = j = 0; i < pLabel->RefCount(); i++)
  {
  AddrBus &ref = pLabel->GetRef(i);
  if (ref != addrp)
    {
    int nDigits = (pDasm->BusAddressBits(ref.bus) + 3) / 4;
    string sLblText = pDasm->Label2String(ref.addr, nDigits, true, NO_ADDRESS, ref.bus);
    if (ref.bus != bus)
      sLblText += "(" + pDasm->GetBusName(ref.bus) + ")";
    if (j && sLine.size() + 2 + sLblText.size() > 79)
      {
      PrintLine(sLine);
      sLine = sComDel + "       ";
      j = 0;
      }
    else if (j)
      sLine += ", ";
    sLine += sLblText;
    j++;
    }
  addrp = ref;
  }
PrintLine(sLine);
return true;
}

/*****************************************************************************/
/* DisassembleChanges : parses pre-/post-changes for a disassembly line      */
/*****************************************************************************/

bool Application::DisassembleChanges
    (
    addr_t addr,
    addr_t prevaddr,
    addr_t prevsz,
    bool bAfterLine,
    int bus
    )
{
vector<Disassembler::LineChange> changes;
bool bRC = pDasm->DisassembleChanges(addr,
                                     prevaddr, prevsz,
                                     bAfterLine,
                                     changes,
                                     bus);

for (vector<Disassembler::LineChange>::size_type i = 0;
     i < changes.size();
     i++)
  {
  PrintLine(changes[i].label, changes[i].oper, changes[i].opnds);
  }
return bRC;
}

/*****************************************************************************/
/* DisassembleLabels : disassemble all labels without memory area            */
/*****************************************************************************/

bool Application::DisassembleLabels
    (
    string sComDel,
    string sComHdr,
    int bus
    )
{
string sComBlk(sComDel + " ");
CommentArray::iterator it;
Comment *pComment;
static bool bULHOut = false;
addr_t paddr = NO_ADDRESS;
for (int l = 0; l < pDasm->GetLabelCount(bus); l++)
  {
  Label *pLbl = pDasm->LabelAt(l, bus);
  addr_t laddr = pLbl->GetAddress();
  bool bShow = showUnused || pLbl->IsUsed();
  // if (!pLbl->IsConst())
  MemoryType memType = pDasm->GetMemType(laddr, bus);
  bShow &= (memType == Untyped);
  if (bShow)
    {
    string slabel, smnemo, sparm;
    if (pDasm->DisassembleLabel(pLbl, slabel, smnemo, sparm, bus))
      {
      // header, if not yet done
      if (!bULHOut)
        {
        PrintLine();
        PrintLine(sComHdr);
        PrintLine(sComBlk + "Used Labels");
        PrintLine(sComHdr);
        PrintLine();
        bULHOut = true;
        }

      // comments before line
      if (laddr != paddr)
        DisassembleComments(laddr, false, sComDel, bus);

      // Cross-references before label line
      DisassembleCref(pLbl, sComDel, bus);

      // the line itself
      pComment = (showComments && laddr != paddr) ?
                     GetFirstLComment(laddr, it, bus) : NULL;
      string scomment = pComment ? pComment->GetText() : "";
      if (scomment.size()) scomment = sComBlk + scomment;
      PrintLine(slabel, smnemo, sparm, scomment, lLabelLen);
      if (pComment)
        {
        while ((pComment = GetNextLComment(laddr, it, bus)) != NULL)
          PrintLine("", "", "", sComBlk + pComment->GetText());
        }

      // comments after line
      if (laddr != paddr)
        DisassembleComments(laddr, true, sComDel, bus);

      paddr = laddr;
      }
    }
  }

return true;
}

/*****************************************************************************/
/* DisassembleDefLabels : disassemble all used DefLabels                     */
/*****************************************************************************/

bool Application::DisassembleDefLabels
    (
    string sComDel,
    string sComHdr,
    int bus
    )
{
string sComBlk(sComDel + " ");

static bool bULHOut = false;
for (int l = 0; l < pDasm->GetDefLabelCount(bus); l++)
  {
  DefLabel *pLbl = pDasm->DefLabelAt(l, bus);
  string slabel, smnemo, sparm;
  // DefLabels got no comments ATM
  if (pLbl &&
      pDasm->DisassembleDefLabel(pLbl, slabel, smnemo, sparm, bus))
    {
    // header, if not yet done
    if (!bULHOut)
      {
      PrintLine();
      PrintLine(sComHdr);
      PrintLine(sComBlk + "Used Definitions");
      PrintLine(sComHdr);
      PrintLine();
      bULHOut = true;
      }
    // and no line comments
    PrintLine(slabel, smnemo, sparm, "", lLabelLen);
    }
  }

return true;
}

/*****************************************************************************/
/* DisassembleLine : create a line of disassembler output (+comments)        */
/*****************************************************************************/

addr_t Application::DisassembleLine
    (
    addr_t addr,
    string sComDel,
    string sComHdr,
    string labelDelim,
    int bus
    )
{
string sLabel, sMnemo, sParms, sComBlk(sComDel + " ");
CommentArray::iterator it;
Comment *pComment;
bool bWithComments = showHex || showAsc || showAddr;

// comments before line
DisassembleComments(addr, false, sComDel, bus);

// the line itself
// in case of multiple labels, it's always the last one that's used
// for the "real" code/data line, so prepend all others
LabelArray::iterator lit;
Label *p = NULL, *pNext = pDasm->GetFirstLabel(addr, lit, Untyped, bus);
while (pNext)
  {
  if (pNext->IsUsed() && !pNext->IsConst())
    p = pNext;
  pNext = pDasm->GetNextLabel(addr, lit, Untyped, bus);
  // multiple labels get their own lines
  if (p != pNext && pNext && pNext->IsUsed() && !pNext->IsConst())
    {
    // Cross-references before label line
    DisassembleCref(pNext, sComDel, bus);
    string s = p->GetText();
    if (s.size())
      PrintLine(s + labelDelim);
    }
  }
// Cross-references before label line
DisassembleCref(p, sComDel, bus);
if (p && p->IsUsed() && !p->IsConst())
  sLabel = pDasm->Label2String(addr, 4, true, addr, bus) +
           labelDelim;
pComment = showComments ? GetFirstLComment(addr, it, bus) : NULL;
int maxparmlen = (bWithComments || pComment) ? (cparmLen - 1) : uparmLen;
addr_t sz = pDasm->Disassemble(addr, sMnemo, sParms, maxparmlen, bus);

string scomment;
if (showAddr)
  scomment += sformat("%0*X%s",
                      (pDasm->BusAddressBits(bus) + 3) / 4, addr,
                      (showHex || showAsc) ? ": " : " ");
if ((showHex || showAsc) && !pDasm->IsBss(addr, bus))
  {
  string sHex, sAsc;
  if (showHex || showAsc)
    {
    pDasm->DisassembleHexAsc(addr, sz, (addr_t)dbCount, sHex, sAsc, bus);
    if (!pComment)
      sAsc = trim(sAsc);
#if 0
    for (addr_t i = sz; i < (addr_t)dbCount; i++)
      {
      if (showHex && showAsc)
        sHex += "   ";
      if (showAsc && pComment)
        sAsc += ' ';
      }
#endif
    }

  if (showHex)
    scomment += sHex;
  if (showAsc)
    scomment += sAsc;
  }
if (pComment && pComment->GetText().size())
  {
  if (scomment.size()) scomment += " ";
  scomment += pComment->GetText();
  }
if (scomment.size()) scomment = sComBlk + scomment;
PrintLine(sLabel, sMnemo, sParms, scomment);
if (pComment)
  {
// following line comments
  while ((pComment = GetNextLComment(addr, it, bus)) != NULL)
    PrintLine("", "", "", sComBlk + pComment->GetText());
  }

// comments after line
DisassembleComments(addr, true, sComDel, bus);

return sz;
}

/*****************************************************************************/
/* PrintLine : prints a formatted line to the output                         */
/*****************************************************************************/

bool Application::PrintLine
    (
    string sLabel,
    string smnemo,
    string sparm,
    string scomment,
    int labelLen
    )
{
if (labelLen < 0) labelLen = this->labelLen;
int nLen = 0;
int nMinLen = labelLen;
bool bLastBlank = false;
if (sLabel.size())
  {
  nLen += fprintf(out, "%s", sLabel.c_str());
  if (nLen > labelLen &&  // skip to next line on very long labels
      smnemo.size())
    nLen = fprintf(out, "\n%*s", labelLen - 1, "") - 1;
  }

if (smnemo.size())
  {
  if (nLen > 0) nLen += fprintf(out, " ");
  if (nLen < nMinLen) nLen += fprintf(out, "%*s", nMinLen - nLen, "");
  nLen += fprintf(out, "%s", smnemo.c_str());
  }
nMinLen += mnemoLen;
if (sparm.size())
  {
  if (nLen > 0) nLen += fprintf(out, " ");
  if (nLen < nMinLen) nLen += fprintf(out, "%*s", nMinLen - nLen, "");
  nLen += fprintf(out, "%s", sparm.c_str());
  }
nMinLen += cparmLen;
if (scomment.size())
  {
  if (nLen > 0) nLen += fprintf(out, " ");
  if (nLen < nMinLen) nLen += fprintf(out, "%*s", nMinLen - nLen, "");
  nLen += fprintf(out, "%s", scomment.c_str());
  }
nLen += fprintf(out, "\n");
return nLen > 0;
}

/*****************************************************************************/
/* ParseInfoRange : parses a range definition from an info file              */
/*****************************************************************************/

int Application::ParseInfoRange
    (
    string value,
    addr_t &from,
    addr_t &to,
    addr_t &step,
    bool remapped
    )
{
int n = pDasm ? pDasm->String2Range(value, from, to, step) : 0;
if (n < 1)
  from = NO_ADDRESS;
if (from != NO_ADDRESS)
  {
  addr_t *pmap = remaps[infoBus].getat(from);
  if (pmap && remapped) from += *pmap;
  if (from < pDasm->GetLowestBusAddr(infoBus) ||
      from > pDasm->GetHighestBusAddr(infoBus))
    {
    from = NO_ADDRESS;
    n = 0;
    }
  }
if (n < 2)
  to = from;
else if (to != NO_ADDRESS)
  {
  addr_t *pmap = remaps[infoBus].getat(to);
  if (pmap && remapped) to += *pmap;
  if (to < from ||
      to < pDasm->GetLowestBusAddr(infoBus) ||
      to > pDasm->GetHighestBusAddr(infoBus))
    {
    to = NO_ADDRESS;
    n = 0;
    }
  }
if (n < 3)
  step = 1;
return n;
}

/*****************************************************************************/
/* triminfo : trims an info line's value, cutting at comment character       */
/*****************************************************************************/

string triminfo
    (
    string s,
    bool bCutComment,
    bool bUnescape,
    bool bDotStart
    )
{
// copied from trim()
if (s.empty()) return s;
string::size_type from = s.find_first_not_of(" \t");
if (from == s.npos)
  return "";
// up to here. The rest is info-specific
if (bDotStart && s[from] == '.')        /* '.' can allow leading blanks      */
  from++;                               /* (redundant f9dasm behavior)       */
string sout;
for (; from < s.size(); from++)         /* copy the rest, with unescaping    */
  {
  if (bUnescape && s[from] == '\\' && (from + 1) < s.size())
    sout += s[++from];
  else if (bCutComment && (s[from] == '*' /*|| s[from] == ';'*/))
    break;
  else
    sout += s[from];
  }

if (sout.empty()) return sout;
return sout.substr(0, sout.find_last_not_of(" ") + 1);
}

/*****************************************************************************/
/* ParseBool : parses a boolean option value & returns whether it is one     */
/*****************************************************************************/

bool ParseBool(string value, bool &bResult)
{
string lvalue(lowercase(value));
if (value == "false" || value == "off" || value == "0")
  { bResult = false; return true; }
if (value == "true" || value == "on" || value == "1")
  { bResult = true; return true; }
return false;
}

/*****************************************************************************/
/* LoadInfo : loads an information file                                      */
/*****************************************************************************/

bool Application::LoadInfo
    (
    string fileName,
    vector<string> &loadStack,
    bool bProcInfo,
    bool bSetDasm
    )
{
if (!pDasm && bProcInfo)                /* no disassembler, no work.         */
  return false;
                                        /* inhibit recursion                 */
for (vector<string>::const_iterator lsi = loadStack.begin();
     lsi != loadStack.end();
     lsi++)
  if (*lsi == fileName)
    return false;

if (bProcInfo)                          /* if in processing mode             */
  {                                     /* assure this is only done once     */
  for (vector<string>::const_iterator lsi = saPINames.begin();
       lsi != saPINames.end();
       lsi++)
     if (*lsi == fileName)
       return false;
  saPINames.push_back(fileName);
  }

enum InfoCmd
  {
  infoUnknown = -1,                     /* unknown info command              */
  // bus selection
  infoBusSel,                           /* BUS {code|data}                   */
  // memory types
  infoCode,                             /* CODE addr[-addr]                  */
  infoData,                             /* DATA addr[-addr]                  */
  infoConstant,                         /* CONST addr[-addr]                 */
  infoCVector,                          /* CVEC[TOR] addr[-addr]             */
  infoDVector,                          /* DVEC[TOR] addr[-addr]             */
  infoRMB,                              /* RMB addr[-addr]                   */
  infoUnused,                           /* UNUSED addr[-addr]                */
  infoUsed,                             /* FORCE[USED] addr[-addr]           */
  // cell sizes
  infoByte,                             /* BYTE addr[-addr]                  */
  infoWord,                             /* WORD addr[-addr]                  */
  infoDWord,                            /* DWORD addr[-addr]                 */
  infoQWord,                            /* QWORD addr[-addr]                 */
  // cell display types
  infoBinary,                           /* BINARY addr[-addr]                */
  infoChar,                             /* CHAR addr[-addr]                  */
  infoOct,                              /* OCT addr[-addr]                   */
  infoDec,                              /* DEC addr[-addr]                   */
  infoHex,                              /* HEX addr[-addr]                   */
  infoSigned,                           /* SIGNED addr[-addr]                */
  infoUnsigned,                         /* UNSIGNED addr[-addr]              */
  infoFloat,                            /* FLOAT addr[-addr]                 */
  infoDouble,                           /* DOUBLE addr[-addr]                */
  infoTenBytes,                         /* TENBYTES addr[-addr]              */
  // break before cell
  infoBreak,                            /* BREAK addr[-addr]                 */
  infoUnBreak,                          /* UNBREAK addr[-addr]               */
  // forced addressing
  infoForceAddr,                        /* FORCEADDR addr[-addr]             */
  infoUnForceAddr,                      /* UNFORCEADDR addr[-addr]           */
  // relative address
  infoRelative,                         /* RELATIVE addr[-addr] rel          */
  infoUnRelative,                       /* UNRELATIVE addr[-addr]            */
  // label handling
  infoLabel,                            /* LABEL addr[-addr] label           */
  infoDefLabel,                         /* DEFLABEL addr[-addr] label        */
  infoUsedLabel,                        /* USEDLABEL addr[-addr] [label]     */
  infoUnlabel,                          /* UNLABEL addr[-addr]               */
  // phasing support
  infoPhase,                            /* PHASE addr[-addr] [+|-]phase      */
  infoUnphase,                          /* UNPHASE addr[-addr]               */

  // handled outside disassembler engine:
  infoInclude,                          /* INCLUDE infofilename              */
  infoOption,                           /* OPTION name data                  */
  infoFile,                             /* FILE filename [offset]            */
  infoRemap,                            /* REMAP addr[-addr] offs            */
  infoUnRemap,                          /* UNREMAP addr[-addr] offs          */
  // comment handling
  infoComment,                          /* COMMENT addr[-addr] comment       */
  infoPrepComm,                         /* PREPCOMM [addr[-addr]] comment    */
  infoInsert,                           /* INSERT addr[-addr] text           */
  infoPrepend,                          /* PREPEND [addr[-addr]] line        */
  infoLComment,                         /* LCOMMENT addr[-addr] [.]lcomment  */
  infoPrepLComm,                        /* PREPLCOMM addr[-addr] [.]lcomment */
  infoUncomment,                        /* UNCOMMENT addr[-addr]             */
  infoUnLComment,                       /* UNLCOMMENT addr[-addr]            */
  // patching support
  infoPatch,                            /* PATCH addr [byte]*                */
  infoPatchWord,                        /* PATCHW addr [word]*               */
  infoPatchDWord,                       /* PATCHDW addr [dword]*             */
  infoPatchFloat,                       /* PATCHF addr [float]*              */

  // end info file processing
  infoEnd,                              /* END (processing this file)        */
  };
static struct                           /* structure to convert key to type  */
  {
  const char *name;
  InfoCmd cmdType;
  } sKey[] =
  {
  // bus selection
  { "BUS",          infoBusSel },
  // memory types
  { "CODE",         infoCode },
  { "DATA",         infoData },
  { "CONSTANT",     infoConstant },
  { "CONST",        infoConstant },
  { "CVECTOR",      infoCVector },
  { "CVEC",         infoCVector },
  { "DVECTOR",      infoDVector },
  { "DVEC",         infoDVector },
  { "RMB",          infoRMB },
  { "BSS",          infoRMB },
  { "UNUSED",       infoUnused },
  { "FORCE",        infoUsed },
  { "FORCEUSED",    infoUsed },
  // cell sizes
  { "BYTE",         infoByte },
  { "WORD",         infoWord },
  { "DWORD",        infoDWord },
  { "QWORD",        infoQWord },
  // cell display types
  { "BIN",          infoBinary },
  { "BINARY",       infoBinary },
  { "CHAR",         infoChar },
  { "CHARACTER",    infoChar },
  { "OCTAL",        infoOct },
  { "OCT",          infoOct },
  { "DECIMAL",      infoDec },
  { "DEC",          infoDec },
  { "HEXADECIMAL",  infoHex },
  { "SEDECIMAL",    infoHex },
  { "HEX",          infoHex },
  { "SIGNED",       infoSigned },
  { "UNSIGNED",     infoUnsigned },
  { "FLOAT",        infoFloat },
  { "DOUBLE",       infoDouble },
  { "TENBYTES",     infoTenBytes },
  // break before cell
  { "BREAK",        infoBreak },
  { "UNBREAK",      infoUnBreak },
  // forced addressing
  { "FORCEADDR",    infoForceAddr },
  { "UNFORCEADDR",  infoUnForceAddr },
  // relative address
  { "RELATIVE",     infoRelative },
  { "REL",          infoRelative },
  { "UNRELATIVE",   infoUnRelative },
  { "UNREL",        infoUnRelative },
  // label handling
  { "LABEL",        infoLabel },
  { "DEFLABEL",     infoDefLabel },
  { "USED",         infoUsedLabel },
  { "USEDLABEL",    infoUsedLabel },
  { "UNLABEL",      infoUnlabel },
  // phasing support
  { "PHASE",        infoPhase },
  { "UNPHASE",      infoUnphase },

  // handled outside disassembler engine:
  { "INCLUDE",      infoInclude },
  { "OPTION",       infoOption },
  { "FILE",         infoFile },
  { "REMAP",        infoRemap },
  { "UNREMAP",      infoUnRemap },
  // comment handling
  { "COMMENT",      infoComment },
  { "COMM",         infoComment },
  { "PREPCOMMENT",  infoPrepComm },
  { "PREPCOMM",     infoPrepComm },
  { "INSERT",       infoInsert },
  { "PREPEND",      infoPrepend },
  { "LCOMMENT",     infoLComment },
  { "LCOMM",        infoLComment },
  { "PREPLCOMMENT", infoPrepLComm },
  { "PREPLCOMM",    infoPrepLComm },
  { "UNCOMMENT",    infoUncomment },
  { "UNCOMM",       infoUncomment },
  { "UNLCOMMENT",   infoUnLComment },
  { "UNLCOMM",      infoUnLComment },
  // patching support
  { "PATCH",        infoPatch },
  { "PATCHW",       infoPatchWord },
  { "PATCHDW",      infoPatchDWord },
  { "PATCHF",       infoPatchFloat },

  // end info file processing
  { "END",          infoEnd },
  };

                                        /* that's definitely a text file     */
FILE *fp = fopen(fileName.c_str(), "r");
if (!fp)
  return false;

int fc;
string line;
bool bMod = false, bEnd = false;
do
  {
  fc = fgetc(fp);
  // skip leading whitespace
  if ((fc == ' ' || fc == '\t') && !line.size())
    continue;
  if (fc == '+' && !line.size())        /* f9dasm compatibility              */
    {                                   /* (not needed in dasmfw)            */
    bMod = true;
    continue;
    }
  if (fc != '\r' && fc != '\n' && fc != EOF)
    {
    line += (char)fc;
    continue;
    }
  // newline or end of file encountered
  if (line.size() &&                    /* if line has contents              */
      line[0] != '*' && line[0] != ';')
    {
    string key, value;
    string::size_type idx = line.find_first_of(" \t");
    if (idx == line.npos) idx = line.size();
    key = uppercase(line.substr(0, idx));
    value = trim(line.substr(idx));

    InfoCmd cmdType = infoUnknown;
    for (int i = 0; i < _countof(sKey); i++)
      if (key == sKey[i].name)
        {
        cmdType = sKey[i].cmdType;
        break;
        }

    int tgtBus = infoBus;               /* target bus identification         */
    idx = value.find_first_of(" \t");
    if (idx != value.npos &&
        lowercase(value.substr(0, idx)) == "bus")
      {
      string bval(trim(value.substr(idx)));
      idx = bval.find_first_of(" \t");
      if (idx != bval.npos)
        {
        string tgtbus = lowercase(bval.substr(0, idx));
        bval = trim(bval.substr(idx));
        int bus = pDasm ? pDasm->GetBusID(tgtbus) : -1;
        if (bus >= 0)
          {
          tgtBus = (int)bus;
          value = bval;
          }
        }
      }

    if (pDasm && !bSetDasm)             /* let disassembler have a go at it  */
      {
      addr_t from, to, step;            /* address range has to be first!    */
      ParseInfoRange(value, from, to, step);
      if (pDasm->ProcessInfo(key, value,
                             from, to, step,
                             remaps, bProcInfo,
                             infoBus, tgtBus))
        cmdType = infoUnknown;
      }

    if (!bProcInfo)                     /* if just loading includes and files*/
      {
      if (cmdType != infoInclude &&
          cmdType != infoOption &&
          cmdType != infoFile)
        cmdType = infoUnknown;          /* ignore all unwanted command types */
      if (cmdType == infoFile && bSetDasm)
        cmdType = infoUnknown;
      }
    else                                /* if processing complete file       */
      {
      if (cmdType == infoFile)
        cmdType = infoUnknown;          /* ignore all unwanted command types */
      }

    switch (cmdType)
      {
      case infoBusSel :                 /* BUS {code|data}                   */
        {
        int bus = pDasm ? pDasm->GetBusID(value) : -1;
        if (bus >= 0)
          infoBus = (int)bus;
        }
        break;
      case infoInclude :                /* INCLUDE filename                  */
        {
        char delim1 = ' ', delim2 = '\t';
        string fn;
        string::size_type i = 0;
        if (value[i] == '\"' || value[i] == '\'')
          {
          delim1 = value[i++];
          delim2 = '\0';
          }
        for (; i < value.size() && value[i] != delim1 && value[i] != delim2; i++)
          fn += value[i];
        loadStack.push_back(fileName);
        LoadInfo(fn, loadStack, bProcInfo, bSetDasm);
        loadStack.pop_back();
        }
        break;
      case infoFile :                   /* FILE filename [offset]            */
        {
        char delim1 = ' ', delim2 = '\t';
        string fn;
        string::size_type i = 0;
        if (value[i] == '\"' || value[i] == '\'')
          {
          delim1 = value[i++];
          delim2 = '\0';
          }
        for (; i < value.size() && value[i] != delim1 && value[i] != delim2; i++)
          fn += value[i];
        if (i < value.size())
          value = trim(value.substr(i + 1));
        else
          value.clear();
        addr_t offs, ign;
        ParseInfoRange(value, offs, ign, ign);
        if (offs != NO_ADDRESS)
          saFNames.push_back(sformat("-offset:0x%x", offs));
        saFNames.push_back(fn);
        }
        break;
      case infoOption :                 /* OPTION name value                 */
        {
        string option;
        idx = value.find_first_of(" \t");
        if (idx == value.npos) idx = value.size();
        option = value.substr(0, idx);
        value = trim(value.substr(idx));
        ParseOption(option, value, !bProcInfo);
        }
        break;
      case infoRemap :                  /* REMAP addr[-addr] offset          */
      case infoUnRemap :                /* UNREMAP addr[-addr]               */
        {
        string range;
        idx = value.find_first_of(" \t");
        if (idx == value.npos) idx = value.size();
        range = value.substr(0, idx);
        value = trim(value.substr(idx));
        addr_t from, to, step, off;
        // allow remapped remaps - but don't remap the remap range :-)
        if (ParseInfoRange(range, from, to, step, false) >= 1 &&
            (cmdType == infoUnRemap || pDasm->String2Number(value, off)))
          {
          remaps[infoBus].AddMemory(from, to + 1 - from);
          for (addr_t scanned = from;
               scanned >= from && scanned <= to;
               scanned += step)
            {
            if (cmdType == infoRemap)
              *remaps[infoBus].getat(scanned) += off;
            else
              *remaps[infoBus].getat(scanned) = 0;
            }
          }
        }
        break;
      // memory types
      case infoCode :                   /* CODE addr[-addr]                  */
      case infoData :                   /* DATA addr[-addr]                  */
      case infoConstant :               /* CONST addr[-addr]                 */
      case infoCVector :                /* CVEC[TOR] addr[-addr]             */
      case infoDVector :                /* DVEC[TOR] addr[-addr]             */
      case infoRMB :                    /* RMB addr[-addr]                   */
      case infoUnused :                 /* UNUSED addr[-addr]                */
      case infoUsed :                   /* FORCE[USED] addr[-addr]           */
      // cell sizes
      case infoByte :                   /* BYTE addr[-addr]                  */
      case infoWord :                   /* WORD addr[-addr]                  */
      case infoDWord :                  /* DWORD addr[-addr]                 */
      case infoQWord :                  /* QWORD addr[-addr]                 */
      // cell display types
      case infoBinary :                 /* BINARY addr[-addr]                */
      case infoChar :                   /* CHAR addr[-addr]                  */
      case infoOct :                    /* OCT addr[-addr]                   */
      case infoDec :                    /* DEC addr[-addr]                   */
      case infoHex :                    /* HEX addr[-addr]                   */
      // cell types
      case infoSigned :                 /* SIGNED addr[-addr]                */
      case infoUnsigned :               /* UNSIGNED addr[-addr]              */
      // combined size + type
      case infoFloat :                  /* FLOAT addr[-addr]                 */
      case infoDouble :                 /* DOUBLE addr[-addr]                */
      case infoTenBytes :               /* TENBYTES addr[-addr]              */
      // break before
      case infoBreak :                  /* BREAK addr[-addr]                 */
      case infoUnBreak:                 /* UNBREAK addr[-addr]               */
      // forced addressing
      case infoForceAddr :              /* FORCEADDR addr[-addr]             */
      case infoUnForceAddr :            /* UNFORCEADDR addr[-addr]           */
        {
        addr_t from, to, step, tgtaddr;
        if (ParseInfoRange(value, from, to, step) >= 1)
          {
          // if forcing an area into use, allocate untyped memory for it
          if (cmdType == infoUsed)
            pDasm->AddMemory(from, to - from + 1, Untyped, NULL, infoBus);
          for (addr_t scanned = from;
               scanned >= from && scanned <= to;
               scanned += step)
            {
            MemoryType ty = pDasm->GetMemType(scanned, infoBus);
            int sz;
            if (ty != Untyped || cmdType == infoUsed)
              {
              switch (cmdType)
                {
                case infoCode :
                  pDasm->SetMemType(scanned, Code, infoBus);
                  break;
                case infoData :
#if 0
                  // remove all code attributes
                  if (ty == Code)
                    {
                    pDasm->SetCellSize(scanned, 1, infoBus);
                    pDasm->SetDisplay(scanned, MemAttribute::Char, infoBus);
                    }
#endif
                  pDasm->SetMemType(scanned, Data, infoBus);
                  break;
                case infoConstant :
#if 0
                  // remove all code attributes
                  if (ty == Code)
                    {
                    pDasm->SetCellSize(scanned, 1, infoBus);
                    pDasm->SetDisplay(scanned, MemAttribute::Char, infoBus);
                    }
#endif
                  pDasm->SetMemType(scanned, Const, infoBus);
                  break;
                case infoRMB :
#if 0
                  // remove all code attributes
                  if (ty == Code)
                    {
                    pDasm->SetCellSize(scanned, 1, infoBus);
                    pDasm->SetDisplay(scanned, MemAttribute::Char, infoBus);
                    }
#endif
                  pDasm->SetMemType(scanned, Bss, infoBus);
                  break;
                case infoUsed :
                  pDasm->SetCellUsed(scanned, true, infoBus);
                  break;
                case infoUnused :
                  {
                  pDasm->SetMemType(scanned, Untyped, infoBus);
                  pDasm->SetCellUsed(scanned, false, infoBus);
                  }
                case infoByte :
                  pDasm->SetCellSize(scanned, 1, infoBus);
                  break;
                case infoWord :
                  // for compatibility reasons to f9dasm, set type to Data
                  if (ty != Const)
                    pDasm->SetMemType(scanned, Data, infoBus);
                  pDasm->SetCellSize(scanned, 2, infoBus);
                  break;
                case infoDWord :
#if 0
                  // for compatibility reasons to f9dasm, set type to Data
                  if (ty != Const)
                    pDasm->SetMemType(scanned, Data, infoBus);
#endif
                  pDasm->SetCellSize(scanned, 4, infoBus);
                  break;
                case infoQWord :
                  pDasm->SetCellSize(scanned, 8, infoBus);
                  break;
                case infoBinary :
                  pDasm->SetDisplay(scanned, MemAttribute::Binary, infoBus);
                  break;
                case infoChar :
                  pDasm->SetDisplay(scanned, MemAttribute::Char, infoBus);
                  break;
                case infoOct :
                  pDasm->SetDisplay(scanned, MemAttribute::Octal, infoBus);
                  break;
                case infoDec :
                  pDasm->SetDisplay(scanned, MemAttribute::Decimal, infoBus);
                  break;
                case infoHex :
                  pDasm->SetDisplay(scanned, MemAttribute::Hex, infoBus);
                  break;
                case infoSigned :
                  pDasm->SetCellType(scanned, MemAttribute::SignedInt, infoBus);
                  break;
                case infoUnsigned :
                  pDasm->SetCellType(scanned, MemAttribute::UnsignedInt, infoBus);
                  break;
                case infoFloat :
                  pDasm->SetCellSize(scanned, 4, infoBus);
                  pDasm->SetCellType(scanned, MemAttribute::Float, infoBus);
                  scanned += 3;
                  break;
                case infoDouble :
                  pDasm->SetCellSize(scanned, 8, infoBus);
                  pDasm->SetCellType(scanned, MemAttribute::Float, infoBus);
                  scanned += 7;
                  break;
                case infoTenBytes :
                  pDasm->SetCellSize(scanned, 10, infoBus);
                  pDasm->SetCellType(scanned, MemAttribute::Float, infoBus);
                  scanned += 9;
                  break;
                case infoBreak :
                  pDasm->SetBreakBefore(scanned, true, infoBus);
                  break;
                case infoUnBreak:
                  pDasm->SetBreakBefore(scanned, false, infoBus);
                  break;
                case infoForceAddr :
                  pDasm->SetForcedAddr(scanned, true, infoBus);
                  break;
                case infoUnForceAddr :
                  pDasm->SetForcedAddr(scanned, false, infoBus);
                  break;
                case infoCVector :
                  // a code vector defines a table of code pointers
                  if (ty != Data)
                    {
                    pDasm->SetMemType(scanned, Data, infoBus);
                    pDasm->SetDisplay(scanned, MemAttribute::DefaultDisplay, infoBus);
                    }
                  sz = pDasm->GetCodePtrSize();
                  pDasm->SetCellSize(scanned, sz, infoBus);
                  tgtaddr = pDasm->GetTypedInt(scanned, infoBus);
                  pDasm->AddLabel(tgtaddr, Code,
                                  sformat("Z%0*Xvia%0*X",
                                          sz*2, tgtaddr,
                                          sz*2, scanned),
                                  true, tgtBus);
                  scanned += sz - 1;
                  break;
                case infoDVector :
                  // a data vector defines a table of data pointers
                  if (ty != Data)
                    {
                    pDasm->SetMemType(scanned, Data, infoBus);
                    pDasm->SetDisplay(scanned, MemAttribute::DefaultDisplay, infoBus);
                    }
                  sz = pDasm->GetDataPtrSize();
                  pDasm->SetCellSize(scanned, sz, infoBus);
                  tgtaddr = pDasm->GetTypedInt(scanned, infoBus);
                  pDasm->AddLabel(tgtaddr, Code,
                                  sformat("M%0*Xvia%0*X",
                                          sz*2, tgtaddr,
                                          sz*2, scanned),
                                  true, tgtBus);
                  scanned += sz - 1;
                  break;
                }
              }
            }
          }
        }
        break;
      case infoRelative :               /* RELATIVE addr[-addr] rel          */
        {
        string range;
        idx = value.find_first_of(" \t");
        if (idx == value.npos) idx = value.size();
        range = value.substr(0, idx);
        value = trim(value.substr(idx));
        addr_t from, to, step, rel;
        if (ParseInfoRange(range, from, to, step) >= 1 &&
            pDasm->String2Number(value, rel))
          {
          pDasm->AddRelative(from, to - from + 1, NULL, infoBus);
          for (addr_t scanned = from;
               scanned >= from && scanned <= to;
               scanned += step)
           pDasm->SetRelative(scanned, rel, infoBus);
          }
        }
        break;
      case infoUnRelative :             /* UNRELATIVE addr[-addr]            */
        {
        addr_t from, to, step;
        if (ParseInfoRange(value, from, to, step) >= 1)
          {
          for (addr_t scanned = from;
               scanned >= from && scanned <= to;
               scanned += step)
           pDasm->SetRelative(scanned, 0, infoBus);
          }
        }
        break;
      case infoLabel :                  /* LABEL addr[-addr] label           */
      case infoDefLabel :               /* DEFLABEL addr[-addr] label        */
      case infoUsedLabel :              /* USEDLABEL addr[-addr] [label]     */
        {
        addr_t from, to, step;
        string range;
        idx = value.find_first_of(" \t");
        if (idx == value.npos) idx = value.size();
        range = value.substr(0, idx);
        value = (idx < value.size()) ? triminfo(value.substr(idx), true, false) : "";
        idx = value.find_first_of(" \t");
        if (idx != value.npos)
          value = trim(value.substr(0, idx));
        bool bTextOk = (cmdType == infoUsedLabel) || value.size();
        MemoryType memType =
            (cmdType == infoDefLabel) ? Const :
            Untyped;
        bool bUsed = (/* cmdType == infoDefLabel || */ cmdType == infoUsedLabel);
        if (ParseInfoRange(range, from, to, step) >= 1 && bTextOk)
          {
          bool bNoDelta = cmdType == infoUsedLabel && value.empty();
          for (addr_t scanned = from;
               scanned >= from && scanned <= to;
               scanned += step)
            {
            pDasm->AddLabel(
                scanned, memType, 
                (scanned == from || bNoDelta) ?
                     value :
                     sformat("%s+%d", value.c_str(), scanned - from),
                bUsed,
                infoBus);
            }
          }
        }
        break;
      case infoUnlabel :                /* UNLABEL addr[-addr]               */
        {
        addr_t from, to, ign;
        if (ParseInfoRange(value, from, to, ign) >= 1)
          {
          for (int i = pDasm->GetLabelCount(infoBus) - 1; i >= 0; i--)
            {
            addr_t laddr = pDasm->LabelAt(i, infoBus)->GetAddress();
            if (laddr < from) break;
            if (laddr >= from && laddr <= to)
              pDasm->RemoveLabelAt(i, infoBus);
            }
          }
        }
        break;
      case infoPhase :                  /* PHASE addr[-addr] [+|-]phase      */
        {
        string range;
        idx = value.find_first_of(" \t");
        if (idx == value.npos) idx = value.size();
        range = value.substr(0, idx);
        value = trim(value.substr(idx));
        char sign = value.size() ? value[0] : 0;
        bool bSigned = (sign == '+' || sign == '-');
        addr_t from, to, ign, phase;
        if (ParseInfoRange(range, from, to, ign) >= 1 &&
            pDasm->String2Number(value, phase))
          {
          // This might cause garbage if overlapping or disjunct areas of
          // different phase are specified. Well, so be it. Don't do it.
          // The alternative would be to reallocate the phase area for each
          // and every byte.
          TMemory<addr_t, addr_t> *pFrom = pDasm->FindPhase(from, infoBus);
          TMemory<addr_t, addr_t> *pTo = pDasm->FindPhase(to, infoBus);
          if (!pFrom || !pTo)
            {
            // presumably, it's the same phase as others, so make sure
            // to create a unique area
            pDasm->AddPhase(from, to - from + 1, NO_ADDRESS, infoBus);
            pFrom = pDasm->FindPhase(from, infoBus);
            pFrom->SetType(phase);
            }
          for (addr_t scanned = from;
               scanned >= from && scanned <= to;
               scanned++)
            {
            TMemory<addr_t, addr_t> *pArea = pDasm->FindPhase(scanned, infoBus);
            if (pArea)
              {
              if (pArea->GetType() == phase)
                pDasm->SetPhase(scanned, NO_ADDRESS, infoBus);
              else
                pDasm->SetPhase(scanned,
                                bSigned ? pArea->GetType() - phase : phase,
                                infoBus);
              }
            }
          }
        }
        break;
      case infoUnphase :                /* UNPHASE addr[-addr]               */
        {
        addr_t from, to, ign;
        if (ParseInfoRange(value, from, to, ign) >= 1)
          {
          for (addr_t scanned = from;
               scanned >= from && scanned <= to;
               scanned++)
            pDasm->SetPhase(scanned, DEFAULT_ADDRESS, infoBus);
          }
        }
        break;
      case infoComment :                /* COMMENT [addr[-addr]] comment     */
      case infoPrepComm :               /* PREPCOMM [addr[-addr]] comment    */
      case infoInsert :                 /* INSERT [addr[-addr]] text         */
      case infoPrepend :                /* PREPEND [addr[-addr]] line        */
      case infoLComment :               /* LCOMMENT addr[-addr] [.]lcomment  */
      case infoPrepLComm :              /* PREPLCOMM addr[-addr] [.]lcomment */
        {
        // special: "AFTER" as 1st string (line comment ignores it)
        bool bAfter = false;
        bool bPrepend = cmdType == infoPrepComm ||
                         cmdType == infoPrepend ||
                         cmdType == infoPrepLComm;
        bool bIsComm = cmdType == infoComment ||
                       cmdType == infoPrepComm;
        idx = value.find_first_of(" \t");
        if (idx == value.npos) idx = value.size();
        string after = lowercase(value.substr(0, idx));
        if (after == "after")
          {
          bAfter = true;
          value = (idx < value.size()) ? trim(value.substr(idx)) : "";
          }
        addr_t from, to, step;
        string range;
        idx = value.find_first_of(" \t");
        if (idx == value.npos) idx = value.size();
        range = value.substr(0, idx);
        // comments/inserts can have NO address, in which case they
        // are output before anything else
        if (ParseInfoRange(range, from, to, step) < 1)
          idx = 0;
        value = (idx >= value.size()) ? "" :
          triminfo(value.substr(idx), true, true, true);
        for (addr_t scanned = from;
             scanned >= from && scanned <= to;
             scanned += step)
          {
                                        /* make sure comment breaks output   */
          pDasm->SetBreakBefore(scanned, true, infoBus);
          switch (cmdType)
            {
            case infoComment :          /* COMMENT addr[-addr] comment       */
            case infoPrepComm :         /* PREPCOMM [addr[-addr]] comment    */
            case infoInsert :           /* INSERT addr[-addr] text           */
            case infoPrepend :          /* PREPEND [addr[-addr]] line        */
              AddComment(scanned, bAfter, value, bPrepend, bIsComm, infoBus);
              break;
            case infoLComment :         /* LCOMMENT addr[-addr] [.]lcomment  */
            case infoPrepLComm :        /* PREPLCOMM addr[-addr] [.]lcomment */
              if (scanned != NO_ADDRESS)
                AddLComment(scanned, value, bPrepend, infoBus);
              break;
            }
          }
        }
        break;
      case infoUncomment :              /* UNCOMMENT [AFTER] addr[-addr]     */
      case infoUnLComment :             /* UNLCOMMENT addr[-addr]            */
        {
        // special: "AFTER" as 1st string (line comment ignores it)
        bool bAfter = false;
        idx = value.find_first_of(" \t");
        if (idx == value.npos) idx = value.size();
        string after = lowercase(value.substr(0, idx));
        if (after == "after")
          {
          bAfter = true;
          value = (idx < value.size()) ? trim(value.substr(idx)) : "";
          }
        addr_t from, to, ign;
        if (ParseInfoRange(value, from, to, ign) >= 1)
          {
          int i;
          if (cmdType == infoUncomment)
            {
            for (i = GetCommentCount(bAfter, infoBus) - 1; i >= 0; i--)
              {
              AddrText *c = CommentAt(bAfter, i, infoBus);
              addr_t caddr = c->GetAddress();
              if (caddr < from) break;
              if (caddr >= from && caddr <= to)
                RemoveCommentAt(bAfter, i, infoBus);
              }
            }
          else
            {
            for (i = GetLCommentCount(infoBus) - 1; i >= 0; i--)
              {
              AddrText *c = LCommentAt(i, infoBus);
              addr_t caddr = c->GetAddress();
              if (caddr < from) break;
              if (caddr >= from && caddr <= to)
                RemoveLCommentAt(i, infoBus);
              }
            }
          }
        }
        break;
      case infoPatch :                  /* PATCH addr [byte]*                */
      case infoPatchWord :              /* PATCHW addr [word]*               */
      case infoPatchDWord :             /* PATCHDW addr [dword]*             */
      case infoPatchFloat :             /* PATCHF addr [float]*              */
        {
        addr_t from, to, step;
        string range;
        idx = value.find_first_of(" \t");
        if (idx == value.npos) idx = value.size();
        range = value.substr(0, idx);
        value = (idx >= value.size()) ? "" :
          triminfo(value.substr(idx), true, false);
        if (ParseInfoRange(range, from, to, step) >= 1)
          {
          // "to" is not interesting here.
          do
            {
            if (from < pDasm->GetLowestCodeAddr() ||
                from > pDasm->GetHighestCodeAddr())
              break;
            idx = value.find_first_of(" \t");
            if (idx == value.npos) idx = value.size();
            string item = value.substr(0, idx);
            value = (idx >= value.size()) ? "" : trim(value.substr(idx));
            to = 0;
            MemoryType memtype = (infoBus == BusCode) ? Code : Data;
            switch (cmdType)
              {
              case infoPatch :          /* PATCH addr [byte]*                */
                if (sscanf(item.c_str(), "%x", &to) == 1)
                  {
                  if (pDasm->GetMemIndex(from, infoBus) == NO_ADDRESS)
                    pDasm->AddMemory(from, 1, memtype, NULL, infoBus);
                  pDasm->SetUByte(from, (uint8_t)to, infoBus);
                  from += step;
                  }
                else
                  from = NO_ADDRESS;
                break;
              case infoPatchWord :      /* PATCHW addr [word]*               */
                if (sscanf(item.c_str(), "%x", &to) == 1)
                  {
                  if (pDasm->GetMemIndex(from, infoBus) == NO_ADDRESS)
                    pDasm->AddMemory(from, 2, memtype, NULL, infoBus);
                  pDasm->SetUWord(from, (uint16_t)to, infoBus);
                  from += (step > 2) ? step : 2;
                  }
                else
                  from = NO_ADDRESS;
                break;
              case infoPatchDWord :     /* PATCHDW addr [dword]*             */
                if (sscanf(item.c_str(), "%x", &to) == 1)
                  {
                  if (pDasm->GetMemIndex(from, infoBus) == NO_ADDRESS)
                    pDasm->AddMemory(from, 4, memtype, NULL, infoBus);
                  pDasm->SetUDWord(from, (uint32_t)to, infoBus);
                  from += (step > 4) ? step : 4;
                  }
                else
                  from = NO_ADDRESS;
                break;
              case infoPatchFloat :     /* PATCHF addr [float]*              */
                {
                double d;
                if (sscanf(item.c_str(), "%lf", &d) == 1)
                  {
                  int sz;
                  if (pDasm->GetMemIndex(from, infoBus) == NO_ADDRESS)
                    {
                    pDasm->AddMemory(from, 4, memtype, NULL, infoBus);
                    sz = 4;
                    }
                  else
                    sz = pDasm->GetCellSize(from, infoBus);
                  if (sz == 4)
                    pDasm->SetFloat(from, (float)d, infoBus);
                  else if (sz == 8)
                    pDasm->SetDouble(from, d, infoBus);
                  // Tenbytes ... no, sorry. Not yet. Maybe never.
                  else
                    from = NO_ADDRESS;
                  if (from != NO_ADDRESS)
                    from += (step > (addr_t)sz) ? step : (addr_t)sz;
                  }
                else
                  from = NO_ADDRESS;
                }
                break;
              }

            } while (value.size() && from != NO_ADDRESS);
          }
        }
        break;
      case infoEnd :
        bEnd = true;
        break;
      }
    }
  line.clear();
  bMod = false;
  } while (fc != EOF && !bEnd);

fclose(fp);
return true;
}

/*****************************************************************************/
/* ParseOption : parses an option and associated value                       */
/*****************************************************************************/

int Application::ParseOption
    (
    string option,                      /* option name                       */
    string value,                       /* new option value                  */
    bool bSetDasm,                      /* flag whether set disassembler     */
    bool bProcInfo                      /* flag whether to fully process info*/
    )
{
string lvalue(lowercase(value));
int iValue = atoi(value.c_str());
bool bnValue = true;                    /* default to "on"                   */
bool bIsBool = ParseBool(value, bnValue);

if (bSetDasm)                           /* 1st pass: find only dasm          */
  {
  if (option == "dasm")
    {
    // argument must be one word; ignore anything beyond that
    string::size_type idx = lvalue.find_first_of(" \t*");
    if (idx != string::npos)
      lvalue = lvalue.substr(0, idx);
    if (pDasm) delete pDasm;            /* use the last one                  */
    pDasm = CreateDisassembler(lvalue, &iDasm);
    if (!pDasm)
      printf("Unknown disassembler \"%s\"\n", value.c_str());
    return 1;
    }
  else if (option != "info")            /* recurse through infofiles         */
    return 0;                           /* and ignore anything else          */
  }
else if (option == "dasm")              /* 2nd pass: ignore dasm             */
  return 1;

if (option == "?" || option == "help")
  {
  if (lvalue == "info")
    {
    InfoHelp(true);
    return 1;
    }
  else
    {
    Help(true, lvalue == "options");
#if 0
    // Help quits the application. No need for the return statement
    return (lvalue == "options") ? 1 : 0;
#endif
    }
  }
else if (option == "out")
  {
  outname = (value == "console") ? "" : value;
  return 1;
  }
else if (option == "info")
  {
  bool bOK = LoadInfo(value, bProcInfo, bSetDasm);
  if (bOK && !bSetDasm && !bProcInfo)
    saINames.push_back(value);
  return !!bOK;
  }
else if (option == "bus")
  {
  int newBus = pDasm ? pDasm->GetBusID(value) : -1;
  if (newBus >= 0)
    infoBus = newBus;
  }
else if (option == "logo")
  {
  showLogo = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (option == "nologo")
  showLogo = false;
else if (option == "addr")
  {
  showAddr = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (option == "noaddr")
  showAddr = false;
else if (option == "hex")
  {
  showHex = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (option == "nohex")
  showHex = false;
else if (option == "asc")
  {
  showAsc = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (option == "noasc")
  showAsc = false;
else if (option == "comment")
  {
  showComments = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (option == "nocomment")
  showComments = false;
else if (option == "cref")
  {
  showCref = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (option == "nocref")
  showCref = false;
else if (option == "unused")
  {
  showUnused = bnValue;
  return bIsBool ? 1 : 0;
  }
else if (option == "nounused")
  showUnused = false;
else if (option == "labellen" && iValue > 0 && iValue < 256)
  labelLen = iValue;
else if (option == "eqlbllen" && iValue > 0 && iValue < 256)
  lLabelLen = iValue;
else if (option == "opcodelen" && iValue > 0 && iValue < 256)
  mnemoLen = iValue;
else if (option == "copndlen" && iValue > 0 && iValue < 256)
  cparmLen = iValue;
else if (option == "uopndlen" && iValue > 0 && iValue < 256)
  uparmLen = iValue;
else if (option == "dbcount" && iValue > 0 && iValue < 256)
  dbCount = iValue;
else if (pDasm)
  {
  // little special for f9dasm compatibility: "-no{option}" is interpreted
  // as "-option=" (which should default to off)
  if (option.substr(0, 2) == "no" &&
      pDasm->FindOption(option.substr(2)) >= 0)
    pDasm->SetOption(option.substr(2), "off");
  else
    return pDasm->SetOption(option, value);
  }

return 0;
}

/*****************************************************************************/
/* ParseOptions : parse options                                              */
/*****************************************************************************/

void Application::ParseOptions
    (
    int argc,
    char *argv[],
    bool bSetDasm
    )
{
string curBegin, curEnd, curOff, curIlv("1");
int curBus = BusCode;

// parse command line
for (int arg = 1; arg < argc; arg++)
  {
  if (argv[arg][0] == '-' && argv[arg][1])
    {
    // allow -option:value or -option=value syntax, too
    string s(argv[arg] + 1);
    string::size_type x = s.find_first_of("=:");
    if (x != string::npos)
      ParseOption(s.substr(0, x), s.substr(x + 1),
                  bSetDasm, false);
    else
      arg += ParseOption(s, arg < argc - 1 ? argv[arg + 1] : "",
                         bSetDasm, false);
    }
  else if (!bSetDasm)
    {
    string name(argv[arg]);
    string silv("1");
    if (name.size() > 2)
      {
      // name can be name:interleave
      string::size_type idx = name.rfind(':');
      if (idx != string::npos)
        {
        silv = name.substr(idx + 1);
        if (silv.size() == 1 &&
            silv[0] >= '1' && silv[0] <= '8')
          name = name.substr(0, idx);
        else
          silv = "1";
        }
      }
    // in any case, the file relies on the current begin/end/offset settings
    // so their current setting must be preserved.
    string newBegin(pDasm->GetOption("begin"));
    if (newBegin != curBegin)
      {
      saFNames.push_back("-begin:"+newBegin);
      curBegin = newBegin;
      }
    string newEnd(pDasm->GetOption("end"));
    if (newEnd != curEnd)
      {
      saFNames.push_back("-end:"+newEnd);
      curEnd = newEnd;
      }
    string newOff(pDasm->GetOption("offset"));
    if (newOff != curOff)
      {
      saFNames.push_back("-offset:"+newOff);
      curOff = newOff;
      }
    if (silv != curIlv)
      {
      saFNames.push_back("-interleave:"+silv);
      curIlv = silv;
      }
    int newBus = infoBus;
    if (newBus != curBus)
      {
      saFNames.push_back("-bus:"+pDasm->GetBusName(newBus));
      curBus = newBus;
      }
    saFNames.push_back(name);
    }
  }
}

/*****************************************************************************/
/* ListOptions : lists the available options for the currently loaded dasm   */
/*****************************************************************************/

static void inline ListOptionLine
    (
    string name,
    string text,
    string current = ""
    )
{
printf("  %-13s ", name.c_str());
string::size_type idx = text.find('\t');
string optxt;
if (idx != string::npos)
  {
  optxt = text.substr(0, idx);
  text = text.substr(idx + 1);
  }
if (optxt.size())
  printf("%-10s ", optxt.c_str());
printf("%s", text.c_str());
if (current.size())
  printf("  (%s)", current.c_str());
printf("\n");
}

void Application::ListOptions(bool bAllOptions)
{
printf("\nAvailable options");
#if 0
if (pDasm)
  printf(" for %s (%s)",
         Disassemblers[iDasm].name.c_str(),
         pDasm->GetName().c_str());
#endif
printf(":\n");
ListOptionLine("?|help", "[options|info]\tHelp for options or info file");
ListOptionLine("dasm", "{code}\tDisassembler to use",
               pDasm ? Disassemblers[iDasm].name : string(""));

if (!pDasm)
  {
  printf("\n");
  ListDisassemblers();
  printf("\nTo show the available options for a disassembler, select one first\n"
         "and append -? to the command line\n");
  }
else
  {
  if (bAllOptions)
    ListDisassemblers(4);
  ListOptionLine("out", "Output File name", outname.size() ? outname : "console");
  ListOptionLine("info", "Info File name");
  if (pDasm->GetBusCount() > 1)
    {
    string busses = pDasm->GetBusNames();
    busses = sformat("{%s}\tBus for next file", busses.c_str());
    ListOptionLine("bus", busses, pDasm->GetBusName(BusCode));
    }
  printf("  Output formatting options:\n");
  ListOptionLine("logo", "{off|on}\toutput logo in file", showLogo ? "on" : "off");
  ListOptionLine("addr", "{off|on}\toutput address dump", showAddr ? "on" : "off");
  ListOptionLine("hex", "{off|on}\toutput hex dump", showHex ? "on" : "off");
  ListOptionLine("asc", "{off|on}\toutput ASCII rendering of code/data", showAsc ? "on" : "off");
  ListOptionLine("comment", "{off|on}\toutput comments", showComments ? "on" : "off");
  ListOptionLine("cref", "{off|on}\toutput cross-reference for used labels", showCref ? "on" : "off");
  ListOptionLine("unused", "{off|on}\toutput unused defined labels", showUnused ? "on" : "off");
  ListOptionLine("labellen", "{length}\tspace reserved for labels", sformat("%d", labelLen));
  ListOptionLine("eqlbllen", "{length}\tspace reserved for equ labels", sformat("%d", lLabelLen));
  ListOptionLine("opcodelen", "{length}\tspace reserved for opcodes", sformat("%d", mnemoLen));
  ListOptionLine("copndlen", "{length}\tspace reserved for commented operands", sformat("%d", cparmLen));
  ListOptionLine("uopndlen", "{length}\tspace reserved for uncommented operands", sformat("%d", uparmLen));
  ListOptionLine("dbcount", "{num}\tspace reserved for hex/ascii dump bytes",  sformat("%d", dbCount));
  printf("  %s-specific options:\n", Disassemblers[iDasm].name.c_str());
  for (int i = 0; i < pDasm->GetOptionCount(); i++)
    ListOptionLine(pDasm->GetOptionName(i),
                   pDasm->GetOptionHelp(i),
                   pDasm->GetOption(i));
  }
printf("\nOptions can be given in the following formats:\n"
       "  -option value\n"
       "  -option=value\n"
       "  -option:value\n"
       "  -NOoption     (only for off|on options; sets to OFF)\n");
}


/*****************************************************************************/
/* Help : give help about the application                                    */
/*****************************************************************************/

int Application::Help(bool bQuit, bool bAllOptions)
{
printf("Usage: %s [-option]* [filename]*\n", sDasmName.c_str());

ListOptions(bAllOptions);               /* list options for selected dasm    */

if (bQuit) abortHelp = true;
return 1;
}

/*****************************************************************************/
/* InfoHelp : pass back help for info files                                  */
/*****************************************************************************/

int Application::InfoHelp(bool bQuit)
{
bool multibus = false;
string busses;
if (!pDasm)
  busses = "code";
else
  {
  multibus = pDasm->GetBusCount() > 1;
  busses = pDasm->GetBusNames();
  }
#if RB_VARIANT
printf("Info file contents:\n"
       "\nLabel file comments\n"
        "\t* comment line\n"
        "\t; comment line\n"
       "\nOutput control\n"
        "\tPREPEND [AFTER] [addr[-addr]] text to be prepended to the output\n"
        "\tPREPCOMM [AFTER] [addr[-addr]] comment text to be prepended to the output\n"
       "\nMemory content definitions\n");
if (multibus)
  printf("\tBus definition:     BUS {%s}\n", busses.c_str());
printf("\tunused area:        UNUSED from-to\n"
       "\tforce used area:    FORCE[USED] from[-to]\n"
       "\treserved area:      RMB from-to\n"
       "\tcode area:          CODE from[-to]\n"
       "\tdata area:          DATA from[-to] (default: hex byte data)\n"
       "\tbinary data area:   BIN[ARY] from[-to]\n"
       "\tdecimal data area:  DEC[IMAL] from[-to]\n"
       "\thex data area:      HEX[ADECIMAL] from[-to]\n"
       "\tsigned data area:   SIGNED from[-to]\n"
       "\tunsigned data area: UNSIGNED from[-to]\n"
       "\tconstants in memory:CONST from[-to] (like hex)\n"
       "\tchar data area:     CHAR from[-to] (like hex, but ASCII if possible)\n"
       "\tword data area:     WORD from[-to] (like hex, but 16 bit)\n");
if (multibus)
  printf("\tcode vector area:   CVEC[TOR] [BUS {%s}] from[-to]\n", busses.c_str());
else
  printf("\tcode vector area:   CVEC[TOR] from[-to]\n");
printf("\t                    (like WORD, but adds target labels if necessary)\n"
if (multibus)
  printf("\tdata vector area:   DVEC[TOR] [BUS {%s}] from[-to]\n", busses.c_str());
else
  printf("\tdata vector area:   DVEC[TOR] from[-to]\n");
printf("\t                    (like WORD, but adds target labels if necessary)\n"
       "\nAddressing control\n"
        "\tforce addressing relative to base:\n"
        "\t\t\t    REL[ATIVE] addr[-addr] baseaddr\n"
        "\tunset relative addressing:\n"
        "\t\t\t    UNREL[ATIVE] addr[-addr]\n"
        "\tmap memory addresses to different base:\n"
        "\t\t\t    REMAP addr[-addr] offset\n"
        "\tmap file contents to different base:\n"
        "\t\t\t    PHASE addr[-addr] phase\n"
       "\nLabel control\n"
        "\tdefine label:       LABEL addr name\n"
        "\tremove label:       UNLABEL addr[-addr]\n"
        "\tdon't apply label name to constant but treat it as a number\n"
        "\t\t\t    CONST from[-to]\n"
        "\tmark auto-generated label as used\n"
        "\t\t\t    USED[LABEL] addr\n"
       "\nCommenting\n"
        "\tcomment:            COMM[ENT] [AFTER] addr[-addr] text\n"
        "\tsuppress comments:  UNCOMM[ENT] [AFTER] addr[-addr] text\n"
        "\tappended comments:  LCOMM[ENT] addr[-addr] text\n"
        "\tprepended comments: PREPLCOMM[ENT] addr[-addr] text\n"
        "\tsuppress lcomments: UNLCOMM[ENT] addr[-addr]\n"
       "\nMisc control\n"
        "\tinsert byte data:   PATCH addr[-addr] data[...]\n"
        "\tinsert word data:   PATCHW addr[-addr] data[...]\n"
        "\tinsert dword data:  PATCHDW addr[-addr] data[...]\n"
        "\tinsert float data:  PATCHF addr[-addr] data[...]\n"
        "\tinsert text:        INSERT [AFTER] addr[-addr] embedded line\n"
        "\tinclude label file: INCLUDE filename\n"
        "\tload binary file:   FILE filename [baseaddr]\n"
        "\tstop parsing:       END\n"
       );
#else
printf("Info file contents:\n"
       "Consists of text records of one of the following formats:\n"
       "* comment line\n");
if (multibus)
  printf("BUS {%s}\n", busses.c_str());

printf("CODE addr[-addr]\n"
       "DATA addr[-addr]\n"
       "CONST from[-to]\n");
if (multibus)
  printf("CVEC[TOR] [BUS {%s}] addr[-addr] (forced code vectors)\n"
         "DVEC[TOR] [BUS {%s}] addr[-addr] (forced data vectors)\n",
         busses.c_str(), busses.c_str());
else
  printf("CVEC[TOR] addr[-addr] (forced code vectors)\n"
         "DVEC[TOR] addr[-addr] (forced data vectors)\n");
printf("RMB addr[-addr]\n"
       "UNUSED addr[-addr]\n"
       "FORCE[USED] addr[-addr]\n"

       "BYTE addr[-addr] (forced byte data)\n"
       "WORD addr[-addr] (forced word data)\n"
       "DWORD addr[-addr] (forced doubleword data)\n"

       "BIN[ARY] addr[-addr] (forced binary data)\n"
       "CHAR addr[-addr] (forced character data, only if no WORD area)\n"
       "OCT[AL] addr[-addr] (forced octal data)\n"
       "DEC[IMAL] addr[-addr] (forced decimal data)\n"
       "HEX[ADECIMAL] addr[-addr] (forced hex data)\n"
       "SIGNED addr[-addr] (data is signed integer)\n"
       "UNSIGNED addr[-addr] (data is unsigned integer)\n"
       "FLOAT addr[-addr] (data is 32bit floating point)\n"
       "DOUBLE addr[-addr] (data is 64bit floating point)\n"
       "TENBYTES addr[-addr] (data is 80bit floating point)\n"

       "BREAK from[-to]\n"
       "UNBREAK from[-to]\n"

       "FORCEADDR from[-to]\n"
       "UNFORCEADDR from[-to]\n"

       "REL[ATIVE] addr[-addr] baseaddr\n"
       "UNREL[ATIVE] addr[-addr]\n"

       "LABEL addr[-addr] name\n"
       "DEFLABEL addr[-addr] name\n"
       "USED[LABEL] addr[-addr] [name]  (forces label to \"Used\")\n"
       "UNLABEL addr[-addr]\n"

       "PHASE addr[-addr] [+|-]phase\n"
       "UNPHASE addr[-addr]\n"

       "INCLUDE filename\n"
       "OPTION option value (like command line option, without leading -)\n"
       "FILE filename [baseaddr]\n"
       "REMAP addr[-addr] offset\n"
       "UNREMAP addr[-addr]\n"

       "COMMENT [AFTER] addr[-addr] embedded comment line\n"
       "PREPCOMM [AFTER] [addr[-addr]] comment text to be prepended to the output\n"
       "INSERT [AFTER] addr[-addr] embedded line\n"
       "PREPEND [AFTER] [addr[-addr]] text to be prepended to the output\n"
       "LCOMMENT addr[-addr] appended line comment\n"
       "PREPLCOMM[ENT] addr[-addr] prepended line comment\n"
       "UNCOMMENT [AFTER] addr[-addr]\n"
       "UNLCOMMENT addr[-addr]\n"

       "PATCH addr [byte]*\n"
       "PATCHW addr [word]*\n"
       "PATCHDW addr [word]*\n"
       "PATCHF addr [float]*\n"

       "END\n"
       );
#endif

if (pDasm)
  {
  string sDasmInfo = pDasm->InfoHelp();
  if (sDasmInfo.size())
    printf("\nDisassembler-specific info commands:\n%s", sDasmInfo.c_str());
  }

if (bQuit) abortHelp = true;
return 1;
}

/*****************************************************************************/
/* DumpMem : dump disassembler's memory areas                                */
/*****************************************************************************/

#ifdef _DEBUG
void Application::DumpMem(int bus)
{
if (!pDasm) return;

string sComBlk = pDasm->GetOption("cchar") + " ";

int mac = pDasm->GetMemoryArrayCount(bus);
int maac = pDasm->GetMemAttrArrayCount(bus);
PrintLine(sComBlk +
          sformat("%s Bus: %d memory areas, %d memory attribute areas",
                  pDasm->GetBusName(bus).c_str(),
                  mac, maac));
int i;
for (i = 0; i < mac; i++)
  {
  TMemory<uint8_t, MemoryType> &area = pDasm->GetMemoryArray(i, bus);
  PrintLine(sComBlk +
          sformat("Memory area %2d: start=%x size=%x", i,
                  area.GetStart(),
                  area.size()));
  }
MemAttributeHandler *pmah = pDasm->GetMemoryAttributeHandler(bus);
for (i = 0; i < maac; i++)
  {
  TMemory<uint8_t, MemoryType> &area = pDasm->GetMemoryArray(i, bus);
  PrintLine(sComBlk +
            sformat("Memory attribute area %2d: start=%x size=%x", i,
                    pmah->GetStart(i),
                    pmah->size(i)));
  }

int nLabels = pDasm->GetLabelCount(bus);
for (i = 0; i < nLabels; i++)
  {
  Label *pLabel = pDasm->LabelAt(i, bus);
  PrintLine(sComBlk +
          sformat("Label %4d: addr=%x type=%d \"%s\" %sused",
                  i, pLabel->GetAddress(), pLabel->GetType(),
                  pLabel->GetText().c_str(),
                  pLabel->IsUsed() ? "" : "un"));
  }

int nComments = GetCommentCount(false, bus);
for (i = 0; i < nComments; i++)
  {
  Comment *pComment = CommentAt(false, i, bus);
  PrintLine(sComBlk +
          sformat("%s %4d: before addr=%x \"%s\"",
                  pComment->IsComment() ? "Comment" : "Insert ",
                  i, pComment->GetAddress(),
                  pComment->GetText().c_str()));
  }
nComments = GetCommentCount(true, bus);
for (i = 0; i < nComments; i++)
  {
  Comment *pComment = CommentAt(true, i, bus);
  PrintLine(sComBlk +
          sformat("%s %4d: after  addr=%x \"%s\"",
                  pComment->IsComment() ? "Comment" : "Insert ",
                  i, pComment->GetAddress(),
                  pComment->GetText().c_str()));
  }
}
#endif
