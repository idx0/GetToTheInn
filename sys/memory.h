#pragma once

#include "platform.h"

namespace sys {

	void* mem_alloc(const size_t size);
	void  mem_free(void* ptr);
	void* mem_alloc0(const size_t size);

	// T must be POD type
	template <typename T>
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
			memset(ptr(), 0, size());
			m_length = 0;
		}

		void zero()
		{
			memset(ptr(), 0, size());
		}

	protected:

		T* m_buffer;
		unsigned int m_count;
	};

	template <typename T>
	inline array<T>::array(array<T>& old)
	{
		this->m_count = old.m_count;
		this->m_buffer = old.m_buffer;

		old.m_buffer = NULL;
		old.m_count = 0;
	}

	template <typename T>
	inline array<T>::array(unsigned int n) : m_count(n)
	{
		m_buffer = (T*)mem_alloc(n * sizeof(T));
	}

	template <typename T>
	inline array<T>::~array()
	{
		mem_free(m_buffer);
	}

}
