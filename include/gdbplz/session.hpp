#ifndef GDBPLZ_SESSION_HPP_4FC8D6EA7D054C9CA02EEDFE868107A1
#define GDBPLZ_SESSION_HPP_4FC8D6EA7D054C9CA02EEDFE868107A1

#include <functional>
#include <boost/utility/string_ref.hpp>
#include "./connection.hpp"
#include "./inferior.hpp"
#include "./gdb_version.hpp"
#include <wiertlo/pimpl_handle.hpp>
#include <wiertlo/strong_typedef.hpp>

namespace gdbplz
{
	struct remote_params
	{
		
	};
	
	struct local_params
	{
		boost::filesystem::path debugged_executable;
		boost::filesystem::path symbol_file = debugged_executable;
		std::vector<boost::string_ref> arguments = std::vector<boost::string_ref>();
	};
	
	WIERTLO_STRONG_TYPEDEF(process_id, int);	
	
	class session
	{
	private:
		struct impl;
		typedef wiertlo::pimpl_handle<impl> pimpl_handle_type;
		pimpl_handle_type pi;
	public:
		~session();
		session(session&&);
		session& operator=(session&&);
		session(const session&) = delete;
		session& operator=(const session&) = delete;
		
		session(gdbplz::connection conn);
		
		std::shared_ptr<inferior> launch_local_program(local_params params);
		
		gdb_version version() const;
	};
}

#endif