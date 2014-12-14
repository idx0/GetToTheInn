#include "memory.h"

#pragma hdrstop

namespace sys {

	void* mem_alloc(const size_t size)
	{
		if (!size) return NULL;
		const int len = (size + 15) & ~15;

		return _aligned_malloc(len, 16);
	}

	void  mem_free(void* ptr)
	{
		if (ptr) {
			_aligned_free(ptr);
		}
	}

	void* mem_alloc0(const size_t size)
	{
		void *m = mem_alloc(size);
		memset(m, 0, size);

		return m;
	}

}