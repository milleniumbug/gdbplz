#ifndef GDBPLZ_THREAD_HPP_E53E234D18C94C8AB705CCD03FE91E42
#define GDBPLZ_THREAD_HPP_E53E234D18C94C8AB705CCD03FE91E42

#include "./utility/pimpl_handle.hpp"
#include "./function_context.hpp"
#include <vector>

namespace gdbplz
{
	class inferior;
	
	class thread
	{
	private:
		struct impl;
		typedef wiertlo::pimpl_handle<impl> pimpl_handle_type;
		pimpl_handle_type pi;
		
		friend inferior;
	public:
		~thread();
		thread(thread&&);
		thread& operator=(thread&&);
		thread(const thread&) = delete;
		thread& operator=(const thread&) = delete;
		
		std::vector<function_context> backtrace();
	};
}

#endif