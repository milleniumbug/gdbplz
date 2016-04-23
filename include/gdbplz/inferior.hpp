#ifndef GDBPLZ_INFERIOR_HPP_186CC32C46944E66A71AA4803F4A674B
#define GDBPLZ_INFERIOR_HPP_186CC32C46944E66A71AA4803F4A674B

#include <boost/filesystem.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/optional.hpp>
#include <wiertlo/strong_typedef.hpp>

namespace gdbplz
{
	class function_id;
	class thread;
	class source_location;
	
	class inferior
	{
	public:
		virtual ~inferior() {}
		
		virtual std::vector<thread*> get_threads() = 0;
		virtual void kill() = 0;
		
		virtual std::vector<function_id> lookup_function_by_name(boost::string_ref function_name) = 0;
		virtual boost::optional<function_id> lookup_function_by_source_location(const source_location& function_name) = 0;
	};
}

#endif