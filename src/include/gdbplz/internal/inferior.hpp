#ifndef GDBPLZ_INTERNAL_INFERIOR_HPP_D0F7EF364B8F4C478BEF2AFAB76EE811
#define GDBPLZ_INTERNAL_INFERIOR_HPP_D0F7EF364B8F4C478BEF2AFAB76EE811

#include <future>
#include <gdbplz/inferior.hpp>
#include <gdbplz/session.hpp>
#include <gdbplz/function_id.hpp>

namespace gdbplz
{
	namespace internal
	{
		class inferior : public ::gdbplz::inferior
		{
			std::function<std::future<result_record>(const mi_command&)> send;
			
			
			void kill() override;
			std::vector<function_id> lookup_function_by_name(boost::string_ref) override;
			boost::optional<function_id> lookup_function_by_source_location(const source_location&) override;
		};
	}
}

#endif