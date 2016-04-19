#ifndef GDBPLZ_GDB_AUX_HPP_4CF660BFB9BA4A3A93B42A2CF250373E
#define GDBPLZ_GDB_AUX_HPP_4CF660BFB9BA4A3A93B42A2CF250373E

#include <boost/utility/string_ref.hpp>
#include <boost/filesystem.hpp>
#include <boost/operators.hpp>
#include <wiertlo/pimpl_handle.hpp>

namespace gdbplz
{
	struct gdb_version : boost::totally_ordered<gdb_version>
	{
		unsigned major;
		unsigned minor;
		unsigned patch;
		std::string others;
		
		bool operator<(const gdb_version& other) const;
		bool operator==(const gdb_version& other) const;
		gdb_version(unsigned major, unsigned minor);
		gdb_version(unsigned major, unsigned minor, unsigned patch);
		gdb_version(unsigned major, unsigned minor, unsigned patch, std::string others);
	};
	
	class function_id : boost::totally_ordered<function_id>
	{
	private:
		struct impl;
		typedef wiertlo::pimpl_handle<impl, void*, 8> pimpl_handle_type;
		pimpl_handle_type pi;
	public:
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
	
	struct source_location
	{
		boost::filesystem::path source;
		unsigned long long line;
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