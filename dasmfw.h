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
/* dasmfw.h : global definitions for the DisASseMbler FrameWork              */
/*****************************************************************************/

#ifndef __dasmfw_h_defined__
#define __dasmfw_h_defined__


// Specials for Visual C++
#ifdef _MSC_VER
#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0510
#endif
#if _MSC_VER < 1600
  #include "backport_stdint.h"
#else
  #include <stdint.h>
#endif
#else
// gcc and the like should have this anyway
#include <stdint.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <memory>

#ifndef _countof
#define _countof(a) (sizeof(a) / sizeof(a[0]))
#endif

/*****************************************************************************/
/* Global definitions                                                        */
/*****************************************************************************/

#define DASMFW_VERSION  "0.4"

// set these to int64_t once 64bit processors become part of the framework
typedef uint32_t caddr_t;               /* container for maximal code address*/
typedef uint32_t daddr_t;               /* container for maximal data address*/
typedef uint32_t addr_t;                /* bigger of the 2 above             */
#define ADDR_T_SIZE 4                   /* sizeof(addr_t)                    */

#define NO_ADDRESS ((addr_t)-1)
#define DEFAULT_ADDRESS ((addr_t)-2)

#include "Memory.h"

/*****************************************************************************/
/* Global functions                                                          */
/*****************************************************************************/

std::string lowercase(std::string s);
std::string uppercase(std::string s);
std::string trim(std::string s);
std::string sformat(const std::string fmt_str, ...);

/*****************************************************************************/
/* Automatic Disassembler Registration                                       */
/*****************************************************************************/

class Disassembler;
bool RegisterDisassembler(std::string name, Disassembler * (*CreateDisassembler)());

/*****************************************************************************/
/* Comment : definition of a comment or insert line                          */
/*****************************************************************************/

class Comment : public AddrText
  {
  public:
    Comment(addr_t addr = 0, std::string sline = "")
      : AddrText(addr, Data, sline)
      { }
    virtual ~Comment() { }

    bool operator<(const Comment &other)
      { return AddrText::operator<((const AddrText &)other); }
    bool operator<=(const Comment &other)
      { return AddrText::operator<=((const AddrText &)other); }
    bool operator==(const Comment &other)
      { return AddrText::operator==((const AddrText &)other); }
    bool operator!=(const Comment &other) { return !operator==(other); }
    bool operator>=(const Comment &other) { return !operator<(other); }
    bool operator>(const Comment &other) { return !operator<=(other); }
  };

/*****************************************************************************/
/* Application : main application class                                      */
/*****************************************************************************/

class Application
{
public:
  Application(int argc, char* argv[]);
  ~Application();

  int Run();

  bool LoadInfo(std::string fileName, bool bProcInfo = true)
    {
    std::vector<std::string> loadStack;
    return LoadInfo(fileName, loadStack, bProcInfo);
    }

protected:
  bool LoadFiles();
  bool LoadInfoFiles();
  bool Parse(int nPass, bool bDataBus = false);
  bool ResolveRelativeLabels(bool bDataBus = false);
  bool DisassembleChanges(addr_t addr, addr_t prevaddr, addr_t prevsz, bool bAfterLine, bool bDataBus = false);
  bool DisassembleLabels(std::string sComDel, std::string sComHdr, bool bDataBus = false);
  addr_t DisassembleLine(addr_t addr, std::string sComDel, std::string sComHdr, std::string labelDelim, bool bDataBus = false);
  bool PrintLine(std::string sLabel = "", std::string smnemo = "", std::string sparm = "", std::string scomment = "", int labelLen = -1);
  bool LoadInfo(std::string fileName, std::vector<std::string> &loadStack, bool bProcInfo = true);
  int ParseInfoRange(std::string value, addr_t &from, addr_t &to);
  int ParseOption
    (
    std::string option,                 /* option name                       */
    std::string value,                  /* new option value                  */
    bool bSetDasm = false,              /* flag whether set disassembler     */
    bool bProcInfo = true               /* flag whether to fully process info*/
    );
  void ParseOptions
    (
    int argc,
    char* argv[],
    bool bSetDasm = false               /* flag whether set disassembler     */
    );
  void ListOptions();
  int InfoHelp(bool bQuit = false);
  int Help(bool bQuit = false);
#ifdef _DEBUG
  void DumpMem(bool bDataBus);
#endif

  // Comment / Text line handling
  bool AddComment(addr_t addr, bool bAfter = false, std::string sComment = "", bool bPrepend = false, bool bDataBus = false)
    {
    comments[bDataBus][bAfter].insert(new Comment(addr, sComment), !bPrepend);
    return true;
    }
  Comment *GetFirstComment(addr_t addr, AddrTextArray::iterator &it, bool bAfter = false, bool bDataBus = false)
    {
    it = comments[bDataBus][bAfter].find(addr, Data);
    return it != comments[bDataBus][bAfter].end() ? (Comment *)(*it) : NULL;
    }
  Comment *GetNextComment(addr_t addr, AddrTextArray::iterator &it, bool bAfter = false, bool bDataBus = false)
    {
    it++;
    return (it != comments[bDataBus][bAfter].end() && (*it)->GetAddress() == addr) ? (Comment *)(*it) : NULL;
    }

  int GetCommentCount(bool bAfter, bool bDataBus = false) { return comments[bDataBus][bAfter].size(); }
  Comment *CommentAt(bool bAfter, int index, bool bDataBus = false) { return (Comment *)comments[bDataBus][bAfter].at(index); }
  void RemoveCommentAt(bool bAfter, int index, bool bDataBus = false) { comments[bDataBus][bAfter].erase(comments[bDataBus][bAfter].begin() + index); }

  // Line Comment handling
  bool AddLComment(addr_t addr, std::string sComment = "", bool bPrepend = false, bool bDataBus = false)
    {
    lcomments[bDataBus].insert(new Comment(addr, sComment), !bPrepend);
    return true;
    }
  Comment *GetFirstLComment(addr_t addr, AddrTextArray::iterator &it, bool bDataBus = false)
    {
    it = lcomments[bDataBus].find(addr, Data);
    return it != lcomments[bDataBus].end() ? (Comment *)(*it) : NULL;
    }
  Comment *GetNextLComment(addr_t addr, AddrTextArray::iterator &it, bool bDataBus = false)
    {
    it++;
    return (it != lcomments[bDataBus].end() && (*it)->GetAddress() == addr) ? (Comment *)(*it) : NULL;
    }
  int GetLCommentCount(bool bDataBus = false) { return lcomments[bDataBus].size(); }
  Comment *LCommentAt(int index, bool bDataBus = false) { return (Comment *)lcomments[bDataBus].at(index); }
  void RemoveLCommentAt(int index, bool bDataBus = false) { lcomments[bDataBus].erase(lcomments[bDataBus].begin() + index); }


protected:
  int argc;
  char **argv;
  std::string sDasmName;                /* program name                      */
  Disassembler *pDasm;                  /* selected disassembler             */
  int iDasm;                            /* index of selected disassembler    */
  std::vector<std::string> saFNames;    /* array of files to load            */
  std::vector<std::string> saINames;    /* array of info files to load       */
  std::vector<std::string> saPINames;   /* array of processed info files     */
  std::string outname;                  /* output file name                  */
  FILE *out;                            /* output file                       */

  bool abortHelp;                       /* abort after help has been given   */
  bool bInfoBus;                        /* current bus selection             */
  bool showHex;                         /* flag for hex data display         */
  bool showAddr;                        /* flag for address display          */
  bool showAsc;                         /* flag for ASCII content display    */
  bool showUnused;                      /* flag for showing unused labels    */
  int labelLen;                         /* minimum label display length      */
  int lLabelLen;                        /* minimum label len for EQUs        */
  int mnemoLen;                         /* minimum mnemonics display length  */
  int cparmLen;                         /* min parm len with lcomment        */
  int uparmLen;                         /* max parm len without lcomment     */
  int dbCount;                          /* min bytes for hex/asc dump        */

  TMemoryArray<addr_t> remaps[2];       /* remap arrays                      */
  AddrTextArray comments[2][2];         /* comment / line text arrays        */
  AddrTextArray lcomments[2];           /* line comment arrays               */
};

#endif // __dasmfw_h_defined__
