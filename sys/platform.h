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
#  define NOMINMAX
#  include <windows.h>
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
#endif
