#ifndef GDBPLZ_BREAKPOINT_HPP_460C3356BF4D4FADB584CFBCCCDBD53D
#define GDBPLZ_BREAKPOINT_HPP_460C3356BF4D4FADB584CFBCCCDBD53D

#include <wiertlo/pimpl_handle.hpp>
#include "./source_location.hpp"
#include "./gdb_raw.hpp"

namespace gdbplz
{
	class breakpoint
	{
	public:
		breakpoint(source_location location) :
			location(location)
		{
			
		}
		
		source_location location;
		unsigned long long activation_delay = 0;
		std::vector<cli_command> commands;
		std::string condition;
		bool pending = false;
		bool temporary = false;
	};
	
	class armed_breakpoint
	{
	public:
		virtual ~armed_breakpoint() {}
		
		void set_enabled(bool enable);
		breakpoint info();
	};
}

#endif