#include "data_engine.hpp"
#include "eventqueue.hpp"
#include <stdlib.h>


data_engine::data_hash *data_engine::m_hash = NULL;
data_engine *data_engine::m_instance = NULL;

data_engine::data_engine(unsigned int flags, size_t tablepow)
	: m_flags(flags)
{
	// restrict initial table size to 2^DEFAULT_TABLE_POW
	if (tablepow > 16) { tablepow = DEFAULT_TABLE_POW; }
	if (tablepow == 0) { tablepow = DEFAULT_TABLE_POW; }

	m_hashpower = tablepow;
	m_hashsize = 1 << tablepow;
	m_hashcount = 0;

	mutex_init(&m_mutex);
}


data_engine::~data_engine(void)
{
	size_t i;

	if (m_hash) {
		for (i = 0; i < m_hashsize; i++) {
			if (m_hash[i].data) { free(m_hash[i].data); }
		}

		free(m_hash);
	}

	mutex_destroy(&m_mutex);
}

int data_engine::create_hash()
{
	int err = HASH_SUCCESS;
	size_t i;

	m_hash = (data_hash *)malloc(m_hashsize * sizeof(data_hash));

	if (!m_hash) {
		err = HASH_ENOMEM;
	} else {
		for (i = 0; i < m_hashsize; i++) {
			m_hash[i].key = i;
			m_hash[i].data = NULL;
		}
	}

	return err;
}

int data_engine::create_data(const char *name, gamedata *d)
{
	int err = HASH_SUCCESS;
	data_record *r;
	uint32_t hashval;

	if (mutex_trylock(&m_instance->m_mutex) != 0) {
		err = HASH_EBUSY;
	} else if ((m_hash == NULL) && (m_instance->create_hash())) {
		err = HASH_ENOMEM;
	} else if (m_instance->check_resize() == false) {
		err = HASH_ENOMEM;
	} else if (d == NULL) {
		err = HASH_EINVAL;
	} else {
		if (m_instance->collides(name, &hashval) == false) {
			// everything looks good, lets try to allocate the region
			r = (data_record *)malloc(sizeof(data_record) + d->size());

			if (r != NULL) {
				// success!
				r->m_region = ((unsigned char *)r) + sizeof(data_record);
				r->m_len = d->size();
				r->m_readobj = object(OBJECT_DATA, hashval).getObjectId();
				r->m_dptr = d;
				r->m_flags |= RECORD_SAFE;

				m_instance->write_final(r, d);

				// set the creating gamedata class's readobj pointer
				d->pointer()->m_read_object = r->m_readobj;

				m_hash[hashval].data = r;

				m_instance->m_hashcount++;
				m_instance->event_crtdst(true, object(r->m_readobj));
			} else {
				err = HASH_ENOMEM;
			}
		} else {
			err = HASH_EHASHINUSE;
		}
	}

	if (err != HASH_EBUSY) {
		mutex_unlock(&m_instance->m_mutex);
	}

	return err;
}

int data_engine::destroy_data(const char *name)
{
	uint32_t hashval;
	int err = HASH_SUCCESS;

	if (mutex_trylock(&m_instance->m_mutex) != 0) {
		err = HASH_EBUSY;
	} else if ((m_hash == NULL) || (m_instance->collides(name, &hashval) == false)) {
		err = HASH_EBADHASH;
	} else {
		free(m_hash[hashval].data);
		m_hash[hashval].data = NULL;

		m_instance->m_hashcount--;
		m_instance->event_crtdst(false, object(m_hash[hashval].data->m_readobj));
	}

	if (err != HASH_EBUSY) {
		mutex_unlock(&m_instance->m_mutex);
	}

	return err;
}

bool data_engine::check_resize()
{
	bool ret = true;
	size_t s, t;

	// if the hash is over p/q full
	
	if ((HASH_RFACTOR_Q * m_hashcount) > (HASH_RFACTOR_P * m_hashsize)) {
		if (m_hashpower < MAX_TABLE_POW) {
			t = (m_hashpower + 1);
			s = 1 << t;
		
			data_hash *r = (data_hash *)realloc(m_hash, m_hashsize);

			if (r) {
				m_hash = r;

				m_hashpower = t;
				m_hashsize = s;
			} else {
				// we tried to resize, but we failed.
				ret = false;
			}
		} else {
			// we need to resize but our hashtable is already as big as it
			// can get.
			ret = false;
		}
	}

	return ret;
}

bool data_engine::collides(const char *name, uint32_t *hashval)
{
	bool ret = true;

	if (name != NULL) {
		*hashval = hash(name);

		// is data null?
		if (!m_hash[*hashval].data) {
			ret = false;
		}
	}

	return ret;
}

int data_engine::write(const char *name)
{
	uint32_t hashval;
	int err = HASH_SUCCESS;
	data_record *r;
	gamedata *d;

	if (mutex_trylock(&m_instance->m_mutex) != 0) {
		err = HASH_EBUSY;
	} else if ((m_hash == NULL) || (m_instance->collides(name, &hashval) == false)) {
		err = HASH_EBADHASH;
	} else {
		// valid hashval
		d = m_hash[hashval].data->m_dptr;
		r = (data_record *)realloc(
			m_hash[hashval].data,
			sizeof(data_record) + d->size());

		if (r != NULL) {
			// success!
			r->m_region = ((unsigned char *)r) + sizeof(data_record);
			r->m_len = d->size();

			r->m_readobj = object(OBJECT_DATA, hashval).getObjectId();
			m_instance->write_final(r, d);

			m_hash[hashval].data = r;
			m_instance->event_rw(false, object(r->m_readobj));
		} else {
			err = HASH_ENOMEM;
		}
	}

	if (err != HASH_EBUSY) {
		mutex_unlock(&m_instance->m_mutex);
	}

	return err;
}


int data_engine::write2(const char *name, gamedata *d)
{
	uint32_t hashval;
	int err = HASH_SUCCESS;
	data_record *r;

	if (mutex_trylock(&m_instance->m_mutex) != 0) {
		err = HASH_EBUSY;
	} else if ((m_hash == NULL) || (m_instance->collides(name, &hashval) == false)) {
		err = HASH_EBADHASH;
	} else if (d == NULL) {
		err = HASH_EINVAL;
	} else {
		// valid hashval
		r = (data_record *)realloc(
			m_hash[hashval].data,
			sizeof(data_record) + d->size());

		if (r != NULL) {
			// success!
			r->m_region = ((unsigned char *)r) + sizeof(data_record);
			r->m_len = d->size();

			r->m_readobj = object(OBJECT_DATA, hashval).getObjectId();
			m_instance->write_final(r, d);

			m_hash[hashval].data = r;
			m_instance->event_rw(false, object(r->m_readobj));
		} else {
			err = HASH_ENOMEM;
		}
	}

	if (err != HASH_EBUSY) {
		mutex_unlock(&m_instance->m_mutex);
	}

	return err;
}


int data_engine::read(const char *name)
{
	uint32_t hashval;
	int err = HASH_SUCCESS;
	data_record *d;

	if (mutex_trylock(&m_instance->m_mutex) != 0) {
		err = HASH_EBUSY;
	} else if ((m_hash == NULL) || (m_instance->collides(name, &hashval) == false)) {
		err = HASH_EBADHASH;
	} else {
		// valid hashval
		d = m_hash[hashval].data;

		// check safety
		if ((m_instance->m_flags & HASH_FLAG_READSAFE) &&
			(m_instance->issafe(hashval) == false)) {
			err = HASH_EBUSY;
			mutex_unlock(&m_instance->m_mutex);
		} else if (d->m_dptr != NULL) {
			d->m_dptr->unpack(d->m_region, d->m_len);
			m_instance->event_rw(true, (object(d->m_readobj)));
		} else {
			err = HASH_EINVAL;
		}
	}

	if (err != HASH_EBUSY) {
		mutex_unlock(&m_instance->m_mutex);
	}

	return err;
}

int data_engine::read2(const char *name, gamedata *d)
{
	uint32_t hashval;
	int err = HASH_SUCCESS;
	data_record *r;

	if (mutex_trylock(&m_instance->m_mutex) != 0) {
		err = HASH_EBUSY;
	} else if ((m_hash == NULL) || (m_instance->collides(name, &hashval) == false)) {
		err = HASH_EBADHASH;
	} else if (d == NULL) {
		err = HASH_EINVAL;
	} else {
		// valid hashval
		r = m_hash[hashval].data;

		// check safety
		if ((m_instance->m_flags & HASH_FLAG_READSAFE) &&
			(m_instance->issafe(hashval) == false)) {
			err = HASH_EBUSY;
			mutex_unlock(&m_instance->m_mutex);
		} else if (d != NULL) {
			d->unpack(r->m_region,
				(r->m_len > d->size() ? d->size() : r->m_len));
			m_instance->event_rw(true, object(r->m_readobj));
		} else {
			err = HASH_EINVAL;
		}
	}

	if (err != HASH_EBUSY) {
		mutex_unlock(&m_instance->m_mutex);
	}

	return err;
}

void data_engine::write_final(data_record *r, gamedata *d)
{
	// copy the package
	unsigned char *ptr = d->pack_alloc();
	if (ptr) {
		memcpy(r->m_region, ptr, d->size());
		d->pack_free(ptr);
	}

	// set readobj
	gamedata::data *dt = (gamedata::data *)r->m_region;
	dt->m_read_object = r->m_readobj;
}

int data_engine::safe(const char *name)
{
	uint32_t hashval;
	int err = HASH_SUCCESS;
	data_record *r;

	if (mutex_trylock(&m_instance->m_mutex) != 0) {
		err = HASH_EBUSY;
	} else if ((m_hash == NULL) || (m_instance->collides(name, &hashval) == false)) {
		err = HASH_EBADHASH;
	} else {
		// valid hashval
		r = m_hash[hashval].data;

		r->m_flags |= RECORD_SAFE;
		m_instance->event_safe(true, object(r->m_readobj));
	}

	if (err != HASH_EBUSY) {
		mutex_unlock(&m_instance->m_mutex);
	}

	return err;
}

int data_engine::unsafe(const char *name)
{
	uint32_t hashval;
	int err = HASH_SUCCESS;
	data_record *r;

	if (mutex_trylock(&m_instance->m_mutex) != 0) {
		err = HASH_EBUSY;
	} else if ((m_hash == NULL) || (m_instance->collides(name, &hashval) == false)) {
		err = HASH_EBADHASH;
	} else {
		// valid hashval
		r = m_hash[hashval].data;

		r->m_flags &= ~RECORD_SAFE;
		m_instance->event_safe(false, object(r->m_readobj));
	}

	if (err != HASH_EBUSY) {
		mutex_unlock(&m_instance->m_mutex);
	}

	return err;
}

bool data_engine::issafe(uint32_t hash)
{
	bool ret = false;

	if (m_hash[hash].data != NULL) {
		ret = (m_hash[hash].data->m_flags & RECORD_SAFE);
	}

	return ret;
}

int data_engine::get_working_copy(const object read_object, 
	unsigned char **outptr,
	size_t *outlen)
{
	unsigned char *ptr;
	int err = HASH_SUCCESS;
	uint32_t hashval = read_object.getIndex();

	// defaults on failure
	if (mutex_trylock(&m_instance->m_mutex) != 0) {
		err = HASH_EBUSY;
	} else if ((outptr == NULL) || (outlen == NULL) || 
			   (hashval > m_instance->m_hashsize)) {
		err = HASH_EINVAL;
	} else if (!m_hash[hashval].data) {
		err = HASH_EBADHASH;
	} else {
		*outlen = m_hash[hashval].data->m_len;

		// allocate outptr
		ptr = (unsigned char *)malloc(m_hash[hashval].data->m_len);
		memcpy(ptr, m_hash[hashval].data->m_region, m_hash[hashval].data->m_len);

		*outptr = ptr;
	}

	if (err != HASH_EBUSY) {
		mutex_unlock(&m_instance->m_mutex);
	}

	return err;
}

uint32_t data_engine::hash(const char *name)
{
	size_t len = strlen(name);
	uint32_t h = hashfunc(name, len, 0);

	h = (h & hashmask(m_hashpower));

	return h;
}

bool data_engine::initialize(unsigned int flags, size_t tablepow)
{
	bool ret = false;

	if (m_instance == NULL) {
		m_instance = new data_engine(flags, tablepow);
		ret = true;
	}

	return ret;
}

void data_engine::destroy()
{
	if (m_instance != NULL) {
		delete m_instance;
	}
}

void data_engine::event_safe(const bool s, const object readobj)
{
	event e;
	event_type t;
	event_payload payload;

	if (m_flags & HASH_FLAG_EVENT_SAFE) {
		payload.object = readobj.getObjectId();

		if (s == true) {
			t = EVENT_DATA_SAFE;
		} else {
			t = EVENT_DATA_UNSAFE;
		}

		e.setType(t);
		e.setPayload(payload);

		eventqueue::push(e);
	}
}

void data_engine::event_rw(const bool r, const object readobj)
{
	event e;
	event_type t;
	event_payload payload;

	if (m_flags & HASH_FLAG_EVENT_RW) {
		payload.object = readobj.getObjectId();

		if (r == true) {
			t = EVENT_DATA_READ;
		} else {
			t = EVENT_DATA_WRITE;
		}

		e.setType(t);
		e.setPayload(payload);

		eventqueue::push(e);
	}
}

void data_engine::event_crtdst(const bool c, const object readobj)
{
	event e;
	event_type t;
	event_payload payload;

	if (m_flags & HASH_FLAG_EVENT_CRTDST) {
		payload.object = readobj.getObjectId();

		if (c == true) {
			t = EVENT_DATA_CREATE;
		} else {
			t = EVENT_DATA_DESTROY;
		}

		e.setType(t);
		e.setPayload(payload);

		eventqueue::push(e);
	}
}
