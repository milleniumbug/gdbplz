#ifndef GDBPLZ_SESSION_HPP_4FC8D6EA7D054C9CA02EEDFE868107A1
#define GDBPLZ_SESSION_HPP_4FC8D6EA7D054C9CA02EEDFE868107A1

#include <boost/utility/string_ref.hpp>
#include <boost/filesystem.hpp>
#include <boost/operators.hpp>
#include "./connection.hpp"
#include "./utility/pimpl_handle.hpp"

namespace gdbplz
{
	struct gdb_version : boost::less_than_comparable<gdb_version>
	{
		unsigned major;
		unsigned minor;
		unsigned patch;
		std::string others;
		
		bool operator<(const gdb_version& other) const;
		bool operator==(const gdb_version& other) const;
		gdb_version(unsigned major, unsigned minor);
		gdb_version(unsigned major, unsigned minor, unsigned patch);
		gdb_version(unsigned major, unsigned minor, unsigned patch, std::string others);
	};
	
	class session
	{
	private:
		struct impl;
		typedef wiertlo::pimpl_handle<impl> pimpl_handle_type;
		pimpl_handle_type pi;
	public:
		session(gdbplz::connection conn);
		
		void start();
		void continue_program();
		void kill();
		
		void step();
		void step_into();
		void step_over();
		
		void backtrace();
		
		void user_command(boost::string_ref command);
		
		gdb_version version() const;
	};
}

#endif