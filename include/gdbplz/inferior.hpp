#ifndef GDBPLZ_INFERIOR_HPP_186CC32C46944E66A71AA4803F4A674B
#define GDBPLZ_INFERIOR_HPP_186CC32C46944E66A71AA4803F4A674B

#include <boost/filesystem.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/optional.hpp>
#include <wiertlo/pimpl_handle.hpp>
#include <wiertlo/strong_typedef.hpp>
#include "./gdb_aux.hpp"

namespace gdbplz
{
	class session;
	
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
	
	class inferior
	{
	private:
		struct impl;
		typedef wiertlo::pimpl_handle<impl> pimpl_handle_type;
		pimpl_handle_type pi;
		
		inferior(session& s, remote_params params);
		inferior(session& s, local_params params);
		inferior(session& s, process_id pid);
		friend session;
	public:
		~inferior();
		inferior(inferior&&);
		inferior& operator=(inferior&&);
		inferior(const inferior&) = delete;
		inferior& operator=(const inferior&) = delete;
		
		void step();
		void step_into();
		void step_over();
		
		std::vector<function_id> lookup_function_by_name(boost::string_ref function_name);
		boost::optional<function_id> lookup_function_by_source_location(const source_location& function_name);
	};
}

#endif