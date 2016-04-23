#ifndef GDBPLZ_GDB_AUX_HPP_4CF660BFB9BA4A3A93B42A2CF250373E
#define GDBPLZ_GDB_AUX_HPP_4CF660BFB9BA4A3A93B42A2CF250373E

#include <boost/filesystem.hpp>

namespace gdbplz
{
	struct source_location
	{
		boost::filesystem::path source;
		unsigned long long line;
		
		source_location(boost::filesystem::path source, unsigned long long line);
	};
}

#endif