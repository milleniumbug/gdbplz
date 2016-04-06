#ifndef GDBPLZ_INFERIOR_HPP_186CC32C46944E66A71AA4803F4A674B
#define GDBPLZ_INFERIOR_HPP_186CC32C46944E66A71AA4803F4A674B

#include <boost/filesystem.hpp>
#include <boost/utility/string_ref.hpp>
#include "./utility/pimpl_handle.hpp"
#include "./utility/strong_typedef.hpp"

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
	
	struct process_id : wiertlo::strong_typedef<int, process_id>
	{
		using wiertlo::strong_typedef<int, process_id>::strong_typedef;
	};
	
	class inferior
	{
	private:
		struct impl;
		typedef wiertlo::pimpl_handle<impl> pimpl_handle_type;
		pimpl_handle_type pi;
	public:
		~inferior();
		inferior(inferior&&);
		inferior& operator=(inferior&&);
		inferior(const inferior&);
		inferior& operator=(const inferior&);
		
		inferior(remote_params params);
		inferior(local_params params);
		inferior(process_id pid);
	};
}

#endif