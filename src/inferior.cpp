#include <gdbplz/internal/inferior.hpp>

namespace gdbplz
{
	namespace internal
	{
		void inferior::kill()
		{
			
		}
			
		std::vector<function_id> inferior::lookup_function_by_name(boost::string_ref)
		{
			return std::vector<function_id>();
		}
			
		boost::optional<function_id> inferior::lookup_function_by_source_location(const source_location&)
		{
			return boost::none;
		}
	}
}