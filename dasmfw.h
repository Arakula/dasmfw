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

#define DASMFW_VERSION  "0.7"

// set these to int64_t once 64bit processors become part of the framework
typedef uint32_t caddr_t;               /* container for maximal code address*/
typedef uint32_t daddr_t;               /* container for maximal data address*/
typedef uint32_t addr_t;                /* bigger of the 2 above             */
#define ADDR_T_SIZE 4                   /* sizeof(addr_t)                    */
typedef int32_t saddr_t;                /* should be same size as addr_t     */

#define NO_ADDRESS ((addr_t)-1)
#define DEFAULT_ADDRESS ((addr_t)-2)

/*****************************************************************************/
/* BusType : bus type enumeration                                            */
/*****************************************************************************/

enum BusType
  {
  BusCode,                              /* Code bus (normal)                 */
  BusData,                              /* Data bus (Hardvard architecture)  */
  BusIO,                                /* I/O Bus (if separate I/O bus)     */

  BusTypes
  };

/*****************************************************************************/
/* Global functions                                                          */
/*****************************************************************************/

std::string lowercase(std::string s);
std::string uppercase(std::string s);
std::string ltrim(std::string s);
std::string trim(std::string s);
std::string sformat(const std::string fmt_str, ...);
std::string triminfo
    (
    std::string s,
    bool bCutComment = true,
    bool bUnescape = true,
    bool bDotStart = false
    );


/*****************************************************************************/
/* Automatic Disassembler Registration                                       */
/*****************************************************************************/

class Disassembler;
bool RegisterDisassembler(std::string name, Disassembler * (*CreateDisassembler)());

// This one relies on the global functions above
#include "Memory.h"

/*****************************************************************************/
/* Comment : definition of a comment or insert line                          */
/*****************************************************************************/

class Comment : public AddrText
  {
  public:
    Comment(addr_t addr = 0, std::string sline = "", bool bIsComment = true)
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

  bool LoadInfo(std::string fileName, bool bProcInfo = true, bool bSetDasm = false)
    {
    std::vector<std::string> loadStack;
    return LoadInfo(fileName, loadStack, bProcInfo, bSetDasm);
    }

protected:
  bool LoadFiles();
  bool LoadInfoFiles();
  bool Parse(int nPass, BusType bus = BusCode);
  bool DisassembleComments(addr_t addr, bool bAfterLine, std::string sComDel, BusType bus = BusCode);
  bool DisassembleChanges(addr_t addr, addr_t prevaddr, addr_t prevsz, bool bAfterLine, BusType bus = BusCode);
  bool DisassembleLabels(std::string sComDel, std::string sComHdr, BusType bus = BusCode);
  bool DisassembleDefLabels(std::string sComDel, std::string sComHdr, BusType bus = BusCode);
  addr_t DisassembleLine(addr_t addr, std::string sComDel, std::string sComHdr, std::string labelDelim, BusType bus = BusCode);
  bool PrintLine(std::string sLabel = "", std::string smnemo = "", std::string sparm = "", std::string scomment = "", int labelLen = -1);
  bool LoadInfo(std::string fileName, std::vector<std::string> &loadStack, bool bProcInfo = true, bool bSetDasm = false);
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
  void DumpMem(BusType bus);
#endif

  // Comment / Text line handling
  bool AddComment(addr_t addr, bool bAfter = false, std::string sComment = "", bool bPrepend = false, bool bIsComment = true, BusType bus = BusCode)
    {
    comments[bus][bAfter].insert(new Comment(addr, sComment, bIsComment), !bPrepend);
    return true;
    }
  Comment *GetFirstComment(addr_t addr, CommentArray::iterator &it, bool bAfter = false, BusType bus = BusCode)
    { return comments[bus][bAfter].GetFirst(addr, it); }
  Comment *GetNextComment(addr_t addr, CommentArray::iterator &it, bool bAfter = false, BusType bus = BusCode)
    { return comments[bus][bAfter].GetNext(addr, it); }

  int GetCommentCount(bool bAfter, BusType bus = BusCode) { return comments[bus][bAfter].size(); }
  Comment *CommentAt(bool bAfter, int index, BusType bus = BusCode) { return comments[bus][bAfter].at(index); }
  void RemoveCommentAt(bool bAfter, int index, BusType bus = BusCode) { comments[bus][bAfter].erase(comments[bus][bAfter].begin() + index); }

  // Line Comment handling
  bool AddLComment(addr_t addr, std::string sComment = "", bool bPrepend = false, BusType bus = BusCode)
    {
    lcomments[bus].insert(new Comment(addr, sComment), !bPrepend);
    return true;
    }
  Comment *GetFirstLComment(addr_t addr, CommentArray::iterator &it, BusType bus = BusCode)
    { return lcomments[bus].GetFirst(addr, it); }
  Comment *GetNextLComment(addr_t addr, CommentArray::iterator &it, BusType bus = BusCode)
    { return lcomments[bus].GetNext(addr, it); }
  int GetLCommentCount(BusType bus = BusCode) { return lcomments[bus].size(); }
  Comment *LCommentAt(int index, BusType bus = BusCode) { return lcomments[bus].at(index); }
  void RemoveLCommentAt(int index, BusType bus = BusCode) { lcomments[bus].erase(lcomments[bus].begin() + index); }


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
  BusType infoBus;                      /* current bus selection             */
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

  TMemoryArray<addr_t> remaps[BusTypes]; /* remap arrays                     */
  CommentArray comments[BusTypes][2];   /* comment / line text arrays        */
  CommentArray lcomments[BusTypes];     /* line comment arrays               */
};

#endif // __dasmfw_h_defined__
