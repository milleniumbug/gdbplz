#ifndef GDBPLZ_SESSION_HPP_4FC8D6EA7D054C9CA02EEDFE868107A1
#define GDBPLZ_SESSION_HPP_4FC8D6EA7D054C9CA02EEDFE868107A1

#include <functional>
#include <boost/utility/string_ref.hpp>
#include "./connection.hpp"
#include "./inferior.hpp"
#include "./gdb_aux.hpp"
#include "./utility/pimpl_handle.hpp"

namespace gdbplz
{	
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
		
		
		void user_command(boost::string_ref command);
		gdb_version version() const;
	};
}

#endif