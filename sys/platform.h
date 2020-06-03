/******************************************************************************
 * Copyright (2013) Gorilla Software
 * 
 * NOTICE:  All information contained herein is, and remains the property of
 * Gorilla Software and its suppliers, if any.  The intellectual and technical
 * concepts contained herein are proprietary to Gorilla Software and its
 * suppliers and may be covered by U.S. and Foreign Patents, patents in
 * process, and are protected by trade secret or copyright law.  Dissemination
 * of this information or reproduction of this material is strictly forbidden
 * unless prior written permission is obtained from Gorilla Software.
 *****************************************************************************/

#pragma once

#if (_MSC_VER > 0 && (defined(_WIN32) || defined(__MINGW32__)))
#  define __PLATFORM_WIN32__
#  define WIN32_LEAN_AND_MEAN
#  define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#  define NOVIRTUALKEYCODES // VK_*
#  define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
#  define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#  define NOSYSMETRICS      // SM_*
#  define NOMENUS           // MF_*
#  define NOICONS           // IDI_*
#  define NOKEYSTATES       // MK_*
#  define NOSYSCOMMANDS     // SC_*
#  define NORASTEROPS       // Binary and Tertiary raster ops
#  define NOSHOWWINDOW      // SW_*
#  define OEMRESOURCE       // OEM Resource values
#  define NOATOM            // Atom Manager routines
#  define NOCLIPBOARD       // Clipboard routines
#  define NOCOLOR           // Screen colors
#  define NOCTLMGR          // Control and Dialog routines
#  define NODRAWTEXT        // DrawText() and DT_*
#  define NOGDI             // All GDI defines and routines
#  define NOKERNEL          // All KERNEL defines and routines
#  define NOUSER            // All USER defines and routines
/*#  define NONLS             // All NLS defines and routines*/
#  define NOMB              // MB_* and MessageBox()
#  define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
#  define NOMETAFILE        // typedef METAFILEPICT
#  define NOMINMAX          // Macros min(a,b) and max(a,b)
#  define NOMSG             // typedef MSG and associated routines
#  define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#  define NOSCROLL          // SB_* and scrolling routines
#  define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
#  define NOSOUND           // Sound driver routines
#  define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
#  define NOWH              // SetWindowsHook and WH_*
#  define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
#  define NOCOMM            // COMM driver routines
#  define NOKANJI           // Kanji support stuff.
#  define NOHELP            // Help engine interface.
#  define NOPROFILER        // Profiler interface.
#  define NODEFERWINDOWPOS  // DeferWindowPos routines
#  define NOMCX             // Modem Configuration Extensions
#  define MMNOSOUND

/* Type required before windows.h inclusion  */
typedef struct tagMSG *LPMSG;

#  include <windows.h>

/* Type required by some unused function...  */
typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

#  include <objbase.h>
#  include <mmreg.h>
#  include <mmsystem.h>
#  if defined(_MSC_VER) || defined(__TINYC__)
#    include "propidl.h"
#  endif
#  include <intrin.h>
#  include <process.h>
#  include <malloc.h>
#else
#  define __PLATFORM_LINUX_
#  include <unistd.h>
#  include <pthread.h>
#endif

#include <errno.h>
#include <stdlib.h>

#undef NDEBUG
#include <assert.h>

#ifdef __PLATFORM_WIN32__
#define strncasecmp(x, y, z)	_strnicmp(x, y, z)
#define strcasecmp(x, y)		_stricmp(x, y)
#  ifdef __cplusplus
extern "C" {
#  endif
char *_strptime(const char *buf, const char *fmt, struct tm *tm);
#define strptime(x, y, z)	_strptime(x, y, z)
#  ifdef __cplusplus
}
#  endif
#else
#include <strings.h>
#define _aligned_alloc(x, y)	aligned_alloc(y, x)
#define _aligned_free(x)		free(x)
#define sscanf_s(x, y, z)		sscanf(x, y, z)
#endif
