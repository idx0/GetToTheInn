#pragma once

#ifdef    __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)

uint32_t hashfunc(const void *key, size_t length, const uint32_t initval);

#ifdef    __cplusplus
}
#endif