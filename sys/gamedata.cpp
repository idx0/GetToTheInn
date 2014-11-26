#include "gamedata.h"

#include <stdlib.h>
#include <string.h>

namespace sys
{

gamedata::gamedata(void)
{
	m_object.setObjectId(TOKEN_DATA, INVALID_DATA_INDEX);
}


gamedata::~gamedata(void)
{
	// nothing
}

gamedata::data_ptr gamedata::pointer()
{
	return m_data;
}

void gamedata::copy_until(void *dest, const size_t destlen,
	unsigned char **src, size_t *srclen)
{
	size_t m = 0;

	if (srclen != NULL) {
		m = (destlen < *srclen ? destlen : *srclen);

		if ((src != NULL) && (dest != NULL)) {
			memcpy(dest, *src, m);

			*src += m;
			*srclen -= m;
		}
	}
}

void gamedata::copy_after(unsigned char **dest, size_t *destlen,
	const void *src, const size_t srclen)
{
	size_t m = 0;

	if (destlen != NULL) {
		m = (*destlen < srclen ? *destlen : srclen);

		if ((src != NULL) && (dest != NULL)) {
			memcpy(*dest, src, m);

			*dest += m;
			*destlen -= m;
		}
	}
}

void gamedata::pack_free(unsigned char *ptr)
{
	if (ptr != NULL) {
		free(ptr);
	}
}

}
