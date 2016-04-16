#ifndef GDBPLZ_BREAKPOINT_HPP_460C3356BF4D4FADB584CFBCCCDBD53D
#define GDBPLZ_BREAKPOINT_HPP_460C3356BF4D4FADB584CFBCCCDBD53D

#include "./gdb_io.hpp"
#include "./gdb_aux.hpp"
#include "./utility/pimpl_handle.hpp"

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
	private:
		struct impl;
		typedef wiertlo::pimpl_handle<impl, void*, 16> pimpl_handle_type;
		pimpl_handle_type pi;
	public:
		~armed_breakpoint();
		armed_breakpoint(armed_breakpoint&& other);
		armed_breakpoint& operator=(armed_breakpoint&& other);
		armed_breakpoint(const armed_breakpoint& other) = delete;
		armed_breakpoint& operator=(const armed_breakpoint& other) = delete;
		
		void set_enabled(bool enable);
		breakpoint info();
	};
}

#endif