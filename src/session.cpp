#include "../include/gdbplz/session.hpp"

namespace gdbplz
{
	struct session::impl : wiertlo::pimpl_implementation_mixin<session::pimpl_handle_type, session::impl>
	{
		unsigned token_counter;
	};
	
	gdb_version session::version() const
	{
		auto& i = impl::get(pi);
		// TODO: implement
		return gdb_version(0, 0, 0, "");
	}
	
	const auto tied_version = [](const gdb_version& v)
	{
		return std::tie(v.major, v.minor, v.patch, v.others);
	};
	
	bool gdb_version::operator<(const gdb_version& other) const
	{
		return tied_version(*this) < tied_version(other);
	}
	
	bool gdb_version::operator==(const gdb_version& other) const
	{
		return tied_version(*this) == tied_version(other);
	}
	
// Remove the "helpful" macros defined by glibc.
// Friendly reminder: I'm using `-std=c++11 -Wall -Wextra -pedantic`, these shouldn't be there.
// I thought only <windows.h> was retarded as fuck.
#ifdef major
#undef major
#endif
#ifdef minor
#undef minor
#endif
	
	gdb_version::gdb_version(unsigned major, unsigned minor) :
		major(major),
		minor(minor),
		patch(0)
	{
		
	}
	
	gdb_version::gdb_version(unsigned major, unsigned minor, unsigned patch) :
		major(major),
		minor(minor),
		patch(patch)
	{
		
	}
	
	gdb_version::gdb_version(unsigned major, unsigned minor, unsigned patch, std::string others) :
		major(major),
		minor(minor),
		patch(patch),
		others(others)
	{
		
	}
}