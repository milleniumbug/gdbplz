#include "../include/gdbplz/utility/string.hpp"
#include "../Catch/include/catch.hpp"

TEST_CASE("slice empty string", "[slice]")
{
	using wiertlo::slice;
	REQUIRE(slice("", 0) == "");
	REQUIRE(slice("", 0, 0) == "");
	REQUIRE(slice("", 0, -1) == "");
	REQUIRE(slice("", 0, 5) == "");
	REQUIRE(slice("", 5) == "");
	REQUIRE(slice("", 3) == "");
	REQUIRE(slice("", 7, 3) == "");
	REQUIRE(slice("", boost::string_ref::npos, boost::string_ref::npos) == "");
}

TEST_CASE("slice one-element string", "[slice]")
{
	using wiertlo::slice;
	REQUIRE(slice("A", 0, 0) == "");
	REQUIRE(slice("A", 0, -1) == "");
	REQUIRE(slice("A", 1, 1) == "");
	REQUIRE(slice("A", 1, 5) == "");
	REQUIRE(slice("A", 2, 5) == "");
}

TEST_CASE("slice longer string", "[slice]")
{
	using wiertlo::slice;
	REQUIRE(slice("ABCDEF", 0, 0) == "");
	REQUIRE(slice("ABCDEF", 0, -1) == "ABCDE");
	REQUIRE(slice("ABCDEF", 1, 1) == "");
	REQUIRE(slice("ABCDEF", 1, -1) == "BCDE");
	REQUIRE(slice("ABCDEF", 1, -3) == "BC");
	REQUIRE(slice("ABCDEF", 3, -3) == "");
	REQUIRE(slice("ABCDEF", 10, -1) == "");
}

TEST_CASE("trim empty", "[trim]")
{
	using wiertlo::trim;
	REQUIRE(trim("") == "");
	REQUIRE(trim("     \n \t    \n") == "");
}

TEST_CASE("trim simple", "[trim]")
{
	using wiertlo::trim;
	REQUIRE(trim("A") == "A");
	REQUIRE(trim("A ") == "A");
	REQUIRE(trim("A    ") == "A");
	REQUIRE(trim(" A ") == "A");
	REQUIRE(trim("        A ") == "A");
	REQUIRE(trim("     \n AZSD\t    \n") == "AZSD");
	REQUIRE(trim("     \n A\t    \n") == "A");
}

TEST_CASE("trim with spaces inside", "[trim]")
{
	using wiertlo::trim;
	REQUIRE(trim("        A ") == "A");
	REQUIRE(trim("     AZ\nSD    \n") == "AZ\nSD");
	REQUIRE(trim("   C  \n A    \n") == "C  \n A");
}