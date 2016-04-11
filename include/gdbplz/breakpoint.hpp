#ifndef GDBPLZ_BREAKPOINT_HPP_460C3356BF4D4FADB584CFBCCCDBD53D
#define GDBPLZ_BREAKPOINT_HPP_460C3356BF4D4FADB584CFBCCCDBD53D

#include "./utility/pimpl_handle.hpp"

namespace gdbplz
{
	class breakpoint
	{
	private:
		struct impl;
		typedef wiertlo::pimpl_handle<impl, void*, 16> pimpl_handle_type;
		pimpl_handle_type pi;
	public:
		void set_enabled(bool enable);
	};
}

#endif