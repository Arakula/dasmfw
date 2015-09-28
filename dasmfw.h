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
using namespace std;

#ifndef _countof
#define _countof(a) (sizeof(a) / sizeof(a[0]))
#endif

/*****************************************************************************/
/* Global definitions                                                        */
/*****************************************************************************/

#define DASMFW_VERSION  "0.9"

// set these to int64_t once 64bit processors become part of the framework
typedef uint32_t caddr_t;               /* container for maximal code address*/
typedef uint32_t daddr_t;               /* container for maximal data address*/
typedef uint32_t addr_t;                /* bigger of the 2 above             */
#define ADDR_T_SIZE 4                   /* sizeof(addr_t)                    */
typedef int32_t saddr_t;                /* should be same size as addr_t     */

#define NO_ADDRESS ((addr_t)-1)
#define DEFAULT_ADDRESS ((addr_t)-2)

/*****************************************************************************/
/* Default bus type enumeration                                              */
/*****************************************************************************/

enum
  {
  BusCode,                              /* Code bus (always there)           */

  BaseBusTypes
  };

/*****************************************************************************/
/* Global functions                                                          */
/*****************************************************************************/

string lowercase(string s);
string uppercase(string s);
string ltrim(string s);
string trim(string s);
string sformat(const string fmt_str, ...);
string triminfo
    (
    string s,
    bool bCutComment = true,
    bool bUnescape = true,
    bool bDotStart = false
    );


/*****************************************************************************/
/* Automatic Disassembler Registration                                       */
/*****************************************************************************/

class Disassembler;
bool RegisterDisassembler(string name, Disassembler * (*CreateDisassembler)());

// This one relies on the global functions above
#include "Memory.h"

/*****************************************************************************/
/* Comment : definition of a comment or insert line                          */
/*****************************************************************************/

class Comment : public AddrText
  {
  public:
    Comment(addr_t addr = 0, string sline = "", bool bIsComment = true)
      : AddrText(addr, Data, sline), bIsComment(bIsComment)
      { }
    virtual ~Comment() { }

    void SetComment(bool bOn = true) { bIsComment = bOn; }
    bool IsComment() { return bIsComment; }

#if 0
    // not necessary for comments - these are always multi-arrays
    void CopyUnset(const Comment &other)
      {
      AddrText::CopyUnset((const AddrText &)other);
      bIsComment = other.bIsComment;
      }
#endif

  protected:
    bool bIsComment;
  };

/*****************************************************************************/
/* CommentArray : an array of comment/insert lines                           */
/*****************************************************************************/

class CommentArray : public TAddrTypeArray<Comment>
  {
  public:
    CommentArray() : TAddrTypeArray<Comment>(true) { }
    Comment *GetFirst(addr_t addr, CommentArray::iterator &it)
      {
      it = find(addr, Data);
      return it != end() ? (Comment *)(*it) : NULL;
      }
    Comment *GetNext(addr_t addr, CommentArray::iterator &it)
      {
      it++;
      return (it != end() && (*it)->GetAddress() == addr) ? (Comment *)(*it) : NULL;
      }
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

  bool LoadInfo(string fileName, bool bProcInfo = true, bool bSetDasm = false)
    {
    vector<string> loadStack;
    return LoadInfo(fileName, loadStack, bProcInfo, bSetDasm);
    }

protected:
  bool LoadFiles();
  bool LoadInfoFiles();
  bool Parse(int nPass, int bus = BusCode);
  bool DisassembleComments(addr_t addr, bool bAfterLine, string sComDel, int bus = BusCode);
  bool DisassembleChanges(addr_t addr, addr_t prevaddr, addr_t prevsz, bool bAfterLine, int bus = BusCode);
  bool DisassembleLabels(string sComDel, string sComHdr, int bus = BusCode);
  bool DisassembleDefLabels(string sComDel, string sComHdr, int bus = BusCode);
  addr_t DisassembleLine(addr_t addr, string sComDel, string sComHdr, string labelDelim, int bus = BusCode);
  bool PrintLine(string sLabel = "", string smnemo = "", string sparm = "", string scomment = "", int labelLen = -1);
  bool LoadInfo(string fileName, vector<string> &loadStack, bool bProcInfo = true, bool bSetDasm = false);
  int ParseInfoRange(string value, addr_t &from, addr_t &to);
  int ParseOption
    (
    string option,                 /* option name                       */
    string value,                  /* new option value                  */
    bool bSetDasm = false,              /* flag whether set disassembler     */
    bool bProcInfo = true               /* flag whether to fully process info*/
    );
  void ParseOptions
    (
    int argc,
    char* argv[],
    bool bSetDasm = false               /* flag whether set disassembler     */
    );
  void ListOptions(bool bAllOptions = false);
  int InfoHelp(bool bQuit = false);
  int Help(bool bQuit = false, bool bAllOptions = false);
#ifdef _DEBUG
  void DumpMem(int bus);
#endif

  // Comment / Text line handling
  bool AddComment(addr_t addr, bool bAfter = false, string sComment = "", bool bPrepend = false, bool bIsComment = true, int bus = BusCode)
    {
    comments[bAfter][bus].insert(new Comment(addr, sComment, bIsComment), !bPrepend);
    return true;
    }
  Comment *GetFirstComment(addr_t addr, CommentArray::iterator &it, bool bAfter = false, int bus = BusCode)
    { return comments[bAfter][bus].GetFirst(addr, it); }
  Comment *GetNextComment(addr_t addr, CommentArray::iterator &it, bool bAfter = false, int bus = BusCode)
    { return comments[bAfter][bus].GetNext(addr, it); }

  int GetCommentCount(bool bAfter, int bus = BusCode) { return comments[bAfter][bus].size(); }
  Comment *CommentAt(bool bAfter, int index, int bus = BusCode) { return comments[bAfter][bus].at(index); }
  void RemoveCommentAt(bool bAfter, int index, int bus = BusCode) { comments[bAfter][bus].erase(comments[bAfter][bus].begin() + index); }

  // Line Comment handling
  bool AddLComment(addr_t addr, string sComment = "", bool bPrepend = false, int bus = BusCode)
    {
    lcomments[bus].insert(new Comment(addr, sComment), !bPrepend);
    return true;
    }
  Comment *GetFirstLComment(addr_t addr, CommentArray::iterator &it, int bus = BusCode)
    { return lcomments[bus].GetFirst(addr, it); }
  Comment *GetNextLComment(addr_t addr, CommentArray::iterator &it, int bus = BusCode)
    { return lcomments[bus].GetNext(addr, it); }
  int GetLCommentCount(int bus = BusCode) { return lcomments[bus].size(); }
  Comment *LCommentAt(int index, int bus = BusCode) { return lcomments[bus].at(index); }
  void RemoveLCommentAt(int index, int bus = BusCode) { lcomments[bus].erase(lcomments[bus].begin() + index); }


protected:
  int argc;
  char **argv;
  string sDasmName;                /* program name                      */
  Disassembler *pDasm;                  /* selected disassembler             */
  int iDasm;                            /* index of selected disassembler    */
  vector<string> saFNames;    /* array of files to load            */
  vector<string> saINames;    /* array of info files to load       */
  vector<string> saPINames;   /* array of processed info files     */
  string outname;                  /* output file name                  */
  FILE *out;                            /* output file                       */

  bool abortHelp;                       /* abort after help has been given   */
  int infoBus;                          /* current info file bus selection   */
  bool showHex;                         /* flag for hex data display         */
  bool showAddr;                        /* flag for address display          */
  bool showAsc;                         /* flag for ASCII content display    */
  bool showUnused;                      /* flag for showing unused labels    */
  bool showComments;                    /* flag for showing comments         */
  bool f9dasmComp;                      /* flag for f9dasm compatibility     */
  int labelLen;                         /* minimum label display length      */
  int lLabelLen;                        /* minimum label len for EQUs        */
  int mnemoLen;                         /* minimum mnemonics display length  */
  int cparmLen;                         /* min parm len with lcomment        */
  int uparmLen;                         /* max parm len without lcomment     */
  int dbCount;                          /* min bytes for hex/asc dump        */

  // remap arrays
  vector<TMemoryArray<addr_t>> remaps;
  // comment / line text arrays
  vector<CommentArray> comments[2];
  // line comment arrays
  vector<CommentArray> lcomments;
};

#endif // __dasmfw_h_defined__
