#pragma once

#include "platform.h"
#include "gamedata.h"

#include <stdlib.h>
#include "hash.h"

namespace sys
{

#define HASH_SUCCESS	0
// a hash collision occured
#define HASH_EHASHINUSE	1
// a read/write mutex is locked
#define HASH_EBUSY		2
// the hash does not refer to a valid record
#define HASH_EBADHASH	3
// oom, no memory
#define HASH_ENOMEM		4
// an token input is invalid
#define HASH_EINVAL		5

#define DEFAULT_TABLE_POW	16
#define MAX_TABLE_POW		24

// resize factor p/q - when m_hashcount / m_hashsize > p/q the hash is resized
#define HASH_RFACTOR_P	3
#define HASH_RFACTOR_Q	4

// readsafe ensures that reading and writing is done only
// to "safe" gamedata entries
#define HASH_FLAG_READSAFE		1
#define HASH_FLAG_EVENT_RW		2
#define HASH_FLAG_EVENT_SAFE	4
#define HASH_FLAG_EVENT_CRTDST	8		// create/destroy
#define HASH_FLAG_EVENT_ALL		14

#define RECORD_SAFE		1

class data_engine
{
public:
	// tablepow - 2^(tablepow) length of table 
	static bool initialize(unsigned int flags, size_t tablepow);
	static void destroy();

	// thread-safe non-blocking read/write game data
	static int create_data(const char *name, gamedata *d);
	static int destroy_data(const char *name);

	// writes data from the region to gamedata
	static int write(const char *name);
	static int write2(const char *name, gamedata *d);

	// reads data from the region to gamedata
	static int read(const char *name);
	static int read2(const char *name, gamedata *d);

	// gets a “working copy” of the the game data at the given read_object 
	// read_object is the read token from gamedata::data, which is copied
	// into the data region by create_data.  outptr is allocated to the length
	// returned in outlen.  free() should be called on your outptr when your
	// have finished using it.
	static int get_working_copy(const token read_object, 
		unsigned char **outptr,
		size_t *outlen);

	// these functions (un)set the safe flag for a record
	static int safe(const char *name);
	static int unsafe(const char *name);

	static data_engine *instance() { return m_instance; }
protected:
	uint32_t hash(const char *name);

	explicit data_engine(unsigned int flags, size_t tablepow);
	virtual ~data_engine(void);

	static data_engine *m_instance;
protected:
	// a data ticket token
	struct data_record
	{
		token_id       m_readobj;
		gamedata      *m_dptr;
		unsigned int   m_flags;
		unsigned char *m_region;
		size_t         m_len;
		uint32_t       m_crc;
	};

	// gamedata hash element
	struct data_hash
	{
		unsigned int key;
		data_record *data;
	};

	static data_hash *m_hash;
private:
	int create_hash();

	// collides will return true if name
	//   a) is NULL
	//   b) hashes to a value with a non-NULL data pointer
	bool collides(const char *name, uint32_t *hash);

	// check_resize will return true if m_hash
	//   a) doesn't need resizing
	//   b) is resized successfully
	bool check_resize();

	// returns true if the hash at hashval is safe
	bool issafe(uint32_t hash);

	// writes final data to region and sets readobj
	void write_final(data_record *r, gamedata *d);

	void event_safe(const bool s, const token readobj);
	void event_rw(const bool r, const token readobj);
	void event_crtdst(const bool c, const token readobj);
private:
	unsigned int m_flags;
	mutex m_mutex;

	size_t m_hashsize;
	size_t m_hashpower;

	size_t m_hashcount;
};

}