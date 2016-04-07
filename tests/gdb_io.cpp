#include "../include/gdbplz/gdb_io.hpp"
#include "../include/gdbplz/utility/string.hpp"
#include "../Catch/include/catch.hpp"

TEST_CASE("token creation", "[token]")
{
	REQUIRE_NOTHROW([]()
	{
		gdbplz::user_token t;
	}());
	REQUIRE_NOTHROW([]()
	{
		gdbplz::user_token t("");
	}());
	REQUIRE_NOTHROW([]()
	{
		gdbplz::user_token t("42");
	}());
	REQUIRE_THROWS_AS([]()
	{
		gdbplz::user_token t("abcdef");
	}(), gdbplz::invalid_token);
}

TEST_CASE("parse c string literal", "[parse]")
{
	REQUIRE(gdbplz::string_from_c_string_literal(R"(  "test\n"   )") == "test\n");
	REQUIRE(gdbplz::string_from_c_string_literal(R"( "te\tst\nkol\\\\es" )") == "te\tst\nkol\\\\es");
	REQUIRE(gdbplz::string_from_c_string_literal(R"( "\112\113\114" )") == "\112\113\114");
	REQUIRE(gdbplz::string_from_c_string_literal(R"( "\"\x53\x5A" )") == "\"\x53\x5A");
	REQUIRE(gdbplz::string_from_c_string_literal(R"( "\"\\\t\\\\\t\f\b\xFF" )") == "\"\\\t\\\\\t\f\b\xFF");
}

TEST_CASE("generate c string literal", "[parse]")
{
	REQUIRE(gdbplz::c_string_literal_from_string("test\n") == wiertlo::trim(R"(  "test\n"   )"));
	REQUIRE(gdbplz::c_string_literal_from_string(R"( "te\tst\nkol\\\\es" )") == "te\tst\nkol\\\\es");
	REQUIRE(gdbplz::c_string_literal_from_string(R"( "\112\113\114" )") == "\112\113\114");
	REQUIRE(gdbplz::c_string_literal_from_string(R"( "\"\x53\x5A" )") == "\"\x53\x5A");
	REQUIRE(gdbplz::c_string_literal_from_string(R"( "\"\\\t\\\\\t\f\b\xFF" )") == "\"\\\t\\\\\t\f\b\xFF");
}