#ifndef GDBPLZ_THREAD_HPP_E53E234D18C94C8AB705CCD03FE91E42
#define GDBPLZ_THREAD_HPP_E53E234D18C94C8AB705CCD03FE91E42

#include <wiertlo/pimpl_handle.hpp>
#include "./function_context.hpp"
#include "./breakpoint.hpp"
#include <vector>

namespace gdbplz
{
	
	class thread
	{
	public:
		virtual ~thread() {}
		
		virtual void step() = 0;
		virtual void step_into() = 0;
		virtual void step_over() = 0;
		
		virtual std::shared_ptr<expression> arm_watch() = 0;
		virtual std::shared_ptr<armed_breakpoint> arm_breakpoint(const breakpoint& br) = 0;
		virtual std::vector<function_context> backtrace() = 0;
	};
}

#endif