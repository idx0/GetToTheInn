#pragma once

#include "thread.h"
#include "token.h"

namespace sys {

	class workable
	{
	public:
		workable()  {}
		virtual ~workable() {}

		virtual void work() = 0;
	};

	class worker : public thread
	{
	public:
		worker();

		// creates the thread
		void work(workable* w);

		// returns true if the worker mutex is unlocked
		bool finished();
		
	protected:

		void thread_func();

		workable *m_currentWork;

	};

}
