#ifndef GDBPLZ_SESSION_HPP_4FC8D6EA7D054C9CA02EEDFE868107A1
#define GDBPLZ_SESSION_HPP_4FC8D6EA7D054C9CA02EEDFE868107A1

#include <functional>
#include <future>
#include <boost/utility/string_ref.hpp>
#include <boost/signals2.hpp>
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
		boost::optional<boost::filesystem::path> working_directory;
		boost::optional<boost::filesystem::path> symbol_file;
		boost::optional<std::vector<boost::string_ref>> arguments;
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
		
		session(std::unique_ptr<gdbplz::abstract_connection> conn);
		
		boost::signals2::signal<void(std::string)>& signal_program_output();
		boost::signals2::signal<void(std::string)>& signal_cli_responses();
		boost::signals2::signal<void(std::string)>& signal_gdb_logs();
		
		std::shared_ptr<inferior> launch(local_params params);
		
		std::future<result_record> send(mi_command mi);
		gdb_version version() const;
	};
}

#endif