#include <functional>
#include "../include/gdbplz/gdb_io.hpp"
#include "../include/gdbplz/session.hpp"

namespace gdbplz
{
	struct session::impl : wiertlo::pimpl_implementation_mixin<session::pimpl_handle_type, session::impl>
	{
		unsigned token_counter;
		connection connection_;
		
		result_record send_mi_command(const mi_command& comm)
		{
			// TODO: implement
			return result_record{ comm.token, result_class::done, std::vector<result>() };
		};
	};
	
	gdb_version session::version() const
	{
		//auto& i = impl::get(pi);
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
	
	struct function_id::impl : wiertlo::pimpl_implementation_mixin<function_id::pimpl_handle_type, void*, std::string>
	{
		
	};
	
	function_id::function_id(const function_id&) = default;
	function_id& function_id::operator=(const function_id&) = default;
	function_id::function_id(function_id&&) = default;
	function_id& function_id::operator=(function_id&&) = default;
	function_id::~function_id() = default;
	
	bool function_id::operator<(const function_id& other) const
	{
		return impl::get_handle(this->pi) < impl::get_handle(other.pi);
	}
	
	bool function_id::operator==(const function_id& other) const
	{
		return impl::get_handle(this->pi) == impl::get_handle(other.pi);
	}
	
	std::string function_id::to_string() const
	{
		return impl::get_handle(this->pi);
	}
}

namespace std
{
	std::size_t hash<::gdbplz::function_id>::operator()(const ::gdbplz::function_id& id) const
	{
		return std::hash<std::string>()(::gdbplz::function_id::impl::get_handle(id.pi));
	}
}