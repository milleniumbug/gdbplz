#ifndef GDBPLZ_FUNCTION_CONTEXT_HPP_E066CFC4E7CD4BFCAE82237AEDB9CBD6
#define GDBPLZ_FUNCTION_CONTEXT_HPP_E066CFC4E7CD4BFCAE82237AEDB9CBD6

#include <functional>
#include <boost/utility/string_ref.hpp>
#include <wiertlo/pimpl_handle.hpp>
#include "./function_id.hpp"
#include "./expression.hpp"
#include "./source_location.hpp"

namespace gdbplz
{	
	class function_context
	{
	private:
		struct impl;
		typedef wiertlo::pimpl_handle<impl> pimpl_handle_type;
		pimpl_handle_type pi;
	public:
		function_id function() const;
		source_location source_loc() const;
		std::shared_ptr<expression> evaluate_expression();
		
	};
}

#endif