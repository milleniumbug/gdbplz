#include <boost/utility/string_ref.hpp>
#include "../include/gdbplz/utility/string.hpp"
#include "../Catch/include/catch.hpp"

TEST_CASE("slice empty string", "[slice]")
{
	using wiertlo::slice;
	REQUIRE(slice(boost::string_ref(""), 0) == "");
	REQUIRE(slice(boost::string_ref(""), 0, 0) == "");
	REQUIRE(slice(boost::string_ref(""), 0, -1) == "");
	REQUIRE(slice(boost::string_ref(""), 0, 5) == "");
	REQUIRE(slice(boost::string_ref(""), 5) == "");
	REQUIRE(slice(boost::string_ref(""), 3) == "");
	REQUIRE(slice(boost::string_ref(""), 7, 3) == "");
	REQUIRE(slice(boost::string_ref(""), boost::string_ref::npos, boost::string_ref::npos) == "");
}

TEST_CASE("slice one-element string", "[slice]")
{
	using wiertlo::slice;
	REQUIRE(slice(boost::string_ref("A"), 0, 0) == "");
	REQUIRE(slice(boost::string_ref("A"), 0, -1) == "");
	REQUIRE(slice(boost::string_ref("A"), 1, 1) == "");
	REQUIRE(slice(boost::string_ref("A"), 1, 5) == "");
	REQUIRE(slice(boost::string_ref("A"), 2, 5) == "");
}

TEST_CASE("slice longer string", "[slice]")
{
	using wiertlo::slice;
	REQUIRE(slice(boost::string_ref("ABCDEF"), 0, 0) == "");
	REQUIRE(slice(boost::string_ref("ABCDEF"), 0, -1) == "ABCDE");
	REQUIRE(slice(boost::string_ref("ABCDEF"), 1, 1) == "");
	REQUIRE(slice(boost::string_ref("ABCDEF"), 1, -1) == "BCDE");
	REQUIRE(slice(boost::string_ref("ABCDEF"), 1, -3) == "BC");
	REQUIRE(slice(boost::string_ref("ABCDEF"), 3, -3) == "");
	REQUIRE(slice(boost::string_ref("ABCDEF"), 10, -1) == "");
}

TEST_CASE("trim empty", "[trim]")
{
	using wiertlo::trim;
	REQUIRE(trim(boost::string_ref("")) == "");
	REQUIRE(trim(boost::string_ref("     \n \t    \n")) == "");
}

TEST_CASE("trim simple", "[trim]")
{
	using wiertlo::trim;
	REQUIRE(trim(boost::string_ref("A")) == "A");
	REQUIRE(trim(boost::string_ref("A ")) == "A");
	REQUIRE(trim(boost::string_ref("A    ")) == "A");
	REQUIRE(trim(boost::string_ref(" A ")) == "A");
	REQUIRE(trim(boost::string_ref("        A ")) == "A");
	REQUIRE(trim(boost::string_ref("     \n AZSD\t    \n")) == "AZSD");
	REQUIRE(trim(boost::string_ref("     \n A\t    \n")) == "A");
}

TEST_CASE("trim with spaces inside", "[trim]")
{
	using wiertlo::trim;
	REQUIRE(trim(boost::string_ref("        A ")) == "A");
	REQUIRE(trim(boost::string_ref("     AZ\nSD    \n")) == "AZ\nSD");
	REQUIRE(trim(boost::string_ref("   C  \n A    \n")) == "C  \n A");
}