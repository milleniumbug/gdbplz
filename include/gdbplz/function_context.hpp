#ifndef GDBPLZ_FUNCTION_CONTEXT_HPP_E066CFC4E7CD4BFCAE82237AEDB9CBD6
#define GDBPLZ_FUNCTION_CONTEXT_HPP_E066CFC4E7CD4BFCAE82237AEDB9CBD6

#include <functional>
#include <boost/utility/string_ref.hpp>
#include "./connection.hpp"
#include "./inferior.hpp"
#include "./gdb_aux.hpp"
#include "./utility/pimpl_handle.hpp"

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
		void evaluate_expression();
		
	};
}

#endif