#include "../include/gdbplz/inferior.hpp"

namespace gdbplz
{
	struct inferior::impl : wiertlo::pimpl_implementation_mixin<inferior::pimpl_handle_type, inferior::impl>
	{
		
	};
	
	inferior::~inferior() = default;
	inferior::inferior(inferior&&) = default;
	inferior& inferior::operator=(inferior&&) = default;
	
}