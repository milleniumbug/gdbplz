#include "../include/gdbplz/breakpoint.hpp"

namespace gdbplz
{
	struct breakpoint::impl : wiertlo::pimpl_implementation_mixin<breakpoint::pimpl_handle_type, breakpoint::impl>
	{
		static impl_type& get_impl(pimpl_handle_type& handle) { return *get_handle(handle); }
	};
}