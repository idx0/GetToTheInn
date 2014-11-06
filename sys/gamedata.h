#pragma once

#include "token.h"

namespace sys
{

#define INVALID_DATA_INDEX TOKEN_INDEX_MASK

class gamedata
{
public:
	explicit gamedata(void);
	virtual ~gamedata(void);

public:
	// this structure holds all the data which will be packed
	// by the pack function.  You should derive a similar struct from
	// this one in your overriding class. i.e.
	//   struct mydata: public data {
	//     ...
	//   }
	//   m_data = new mydata;
	struct data
	{
		token_id m_read_object;	// “read only” - set by data_engine
	};

	typedef struct data *data_ptr;

	// size of packed data
	virtual unsigned int size() = 0;

	// packing function, returns a pointer to a contiguous region of
	// your packed data
	virtual unsigned char *pack_alloc() = 0;

	// frees memory returned by pack_alloc()
	virtual void pack_free(unsigned char *ptr);

	// unpacking function, input into this function is a contiguous region
	// of packed bytes of data of length nbytes
	virtual void unpack(unsigned char *packdata, size_t nbytes) = 0;

	// this function returns a pointer to your struct data (or derived
	// equivalent).
	virtual data_ptr pointer();
	
protected:
	// this function copy's min(destlen, *srclen) bytes of data into dest
	// from *src.  afterwards, it increments *src by the amount copied, and
	// decrements *srclen by the same amount.  if for some reason the data
	// cannot be copied successfully, then the dest pointers are returned
	// unchanged.
	void copy_until(void *dest, const size_t destlen,
		unsigned char **src, size_t *srclen);

	// this function copy's min(*destlen, srclen) bytes of data into *dest
	// from src.  afterwards, it increments *dest by the amount copied, and
	// decrements *destlen by the same amount.  if for some reason the data
	// cannot be copied successfully, then the dest pointers are returned
	// unchanged.
	void copy_after(unsigned char **dest, size_t *destlen,
		const void *src, const size_t srclen);

	struct data *m_data;

	token m_object;
};

}