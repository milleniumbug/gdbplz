#ifndef GDBPLZ_FUNCTION_ID_HPP_E333F544048347969A16EC6D16507FA2
#define GDBPLZ_FUNCTION_ID_HPP_E333F544048347969A16EC6D16507FA2

#include <boost/operators.hpp>
#include <wiertlo/pimpl_handle.hpp>
#include <string>

namespace gdbplz
{
	class function_id : boost::totally_ordered<function_id>
	{
	private:
		struct impl;
		typedef wiertlo::pimpl_handle<impl, void*, 8> pimpl_handle_type;
		pimpl_handle_type pi;
	public:
		function_id(std::string name);
		function_id(function_id&&);
		function_id& operator=(function_id&&);
		function_id(const function_id&);
		function_id& operator=(const function_id&);
		~function_id();
		
		// ordering relation
		// don't count on it being reproducible across machines or program runs
		// it's only provided to enable usage as key in associative containers
		bool operator<(const function_id& other) const;
		// equality comparison
		// two function_id are the same when they refer to the same function
		bool operator==(const function_id& other) const;
		// get user-readable string representation of the function
		std::string to_string() const;
		friend std::hash<function_id>;
	};
}

namespace std
{
	template<>
	struct hash<::gdbplz::function_id>
	{
		std::size_t operator()(const ::gdbplz::function_id& id) const;
	};
}

#endif