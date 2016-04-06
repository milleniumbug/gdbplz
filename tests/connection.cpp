#include <boost/filesystem.hpp>
#include "../include/gdbplz/connection.hpp"
#include "../Catch/include/catch.hpp"

TEST_CASE("guessing gdb path", "[connection]")
{
	boost::optional<boost::filesystem::path> path = gdbplz::guess_gdb_path();
	REQUIRE((!path || boost::filesystem::exists(path.value())) == true);
}

TEST_CASE("launching with non-existing gdb", "[connection]")
{
	REQUIRE_THROWS_AS([](){
		gdbplz::connection conn("/sfsfsdfwdsfwerfwef/xcvxvfsdfsdfwerwefrsgfdfgdfgergtergdfg");
	}(), gdbplz::gdb_not_found);
}