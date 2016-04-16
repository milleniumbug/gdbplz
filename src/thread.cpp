#include "../include/gdbplz/thread.hpp"

namespace gdbplz
{
	struct thread::impl : wiertlo::pimpl_implementation_mixin<thread::pimpl_handle_type, thread::impl>
	{
		
	};
	
	thread::~thread() = default;
	thread::thread(thread&&) = default;
	thread& thread::operator=(thread&&) = default;
	
}