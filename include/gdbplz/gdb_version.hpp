#ifndef GDBPLZ_GDB_VERSION_HPP_87147F33047942B582333D9FC419213C
#define GDBPLZ_GDB_VERSION_HPP_87147F33047942B582333D9FC419213C

#include <boost/operators.hpp>
#include <string>

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
}

#endif