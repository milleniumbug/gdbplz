#ifndef GDBPLZ_SESSION_HPP_4FC8D6EA7D054C9CA02EEDFE868107A1
#define GDBPLZ_SESSION_HPP_4FC8D6EA7D054C9CA02EEDFE868107A1

#include <boost/utility/string_ref.hpp>
#include <boost/filesystem.hpp>
#include <boost/operators.hpp>
#include "./connection.hpp"
#include "./utility/pimpl_handle.hpp"
#include "./utility/strong_typedef.hpp"

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
	
	struct remote_params
	{
		
	};
	
	struct local_params
	{
		boost::filesystem::path debugged_executable;
		boost::filesystem::path symbol_file = debugged_executable;
		std::vector<boost::string_ref> arguments = std::vector<boost::string_ref>();
	};
	
	struct process_id : wiertlo::strong_typedef<int, process_id>
	{
		using wiertlo::strong_typedef<int, process_id>::strong_typedef;
	};
	
	class session
	{
	private:
		struct impl;
		typedef wiertlo::pimpl_handle<impl> pimpl_handle_type;
		pimpl_handle_type pi;
	public:
		session(gdbplz::connection conn, remote_params params);
		session(gdbplz::connection conn, local_params params);
		session(gdbplz::connection conn, process_id pid);
		
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