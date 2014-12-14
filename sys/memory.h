#pragma once

#include "platform.h"

namespace sys {

	void* mem_alloc(const size_t size);
	void  mem_free(void* ptr);
	void* mem_alloc0(const size_t size);

	template<typename T>
	inline bool nonnull(T* obj)
	{
		return (obj != static_cast<T*>(0));
	}

	template<typename T>
	class pointer
	{
	public:
		pointer()
		{
			m_ref = (__ref*)mem_alloc(sizeof(__ref));

			m_ref->ptr = new T;
			m_ref->c = 1;
		}

		pointer(T* ptr)
		{
			m_ref = (__ref*)mem_alloc(sizeof(__ref));

			m_ref->ptr = ptr;
			m_ref->c = 1;
		}

		pointer(const pointer& rhs) : m_ref(NULL)
		{
			operator=(rhs);
		}

		~pointer()
		{
			reset();
		}

		T* ptr() const
		{
			if (m_ref) return m_ref->ptr;

			return NULL;
		}

		unsigned int count() const
		{
			if (m_ref) return m_ref->c;

			return 0;
		}

		void reset()
		{
			if (m_ref) {
				m_ref->c--;

				if (m_ref->c == 0) {
					delete m_ref->ptr;
					mem_free(m_ref);
				}
			}

			m_ref = NULL;
		}

		pointer& operator=(const pointer& rhs)
		{
			if (this != &rhs) {
				// reset our current reference
				reset();

				// copy
				m_ref = rhs.m_ref;

				// increment ref count
				if (m_ref) {
					m_ref->c++;
				}
			}

			return *this;
		}

		// here we can safely allow things to segfault (just as they normally would;
		// without loss of generality) if the user tries to derefernece a NULL pointer
		T& operator*() const
		{
			return *(m_ref->ptr);
		}

		// here we can be "safe", but even returning NULL will likely cause a segfault
		// in the caller scope
		T* operator->() const
		{
			if (m_ref) return m_ref->ptr;

			return NULL;
		}

	protected:

		struct __ref
		{
			T* ptr;
			unsigned int c;
		};

		__ref* m_ref;
	};


	// T must be POD type
	template<typename T>
	class array {
	public:
		array(array<T>& old);
		array(unsigned int n);
		~array();

		T& operator[](unsigned int i)
		{
			assert(i < m_count);
			return m_buffer[i];
		}

		const T& operator[](unsigned int i) const
		{
			assert(i < m_count);
			return m_buffer[i];
		}

		T* ptr() { return m_buffer; }
		const T* ptr() const { return m_buffer; }

		size_t size() const { return m_count * sizeof(T); }

		unsigned int length() const { return m_count; }

		void clear()
		{
			zero();
			m_length = 0;
		}

		bool empty() const
		{
			return (m_length == 0);
		}

		void zero()
		{
			memset(ptr(), 0, size());
		}

	protected:

		T* m_buffer;
		unsigned int m_count;
	};

	template<typename T>
	inline array<T>::array(array<T>& old)
	{
		this->m_count = old.m_count;
		this->m_buffer = old.m_buffer;

		old.m_buffer = NULL;
		old.m_count = 0;
	}

	template<typename T>
	inline array<T>::array(unsigned int n) : m_count(n)
	{
		m_buffer = (T*)mem_alloc(n * sizeof(T));
	}

	template<typename T>
	inline array<T>::~array()
	{
		mem_free(m_buffer);
	}

}
