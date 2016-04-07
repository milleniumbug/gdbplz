#include <boost/lexical_cast.hpp>
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

TEST_CASE("non-empty string test", "[nonempty]")
{
	REQUIRE_NOTHROW([]()
	{
		gdbplz::nonempty<std::string> t("test");
	}());
	REQUIRE_NOTHROW([]()
	{
		gdbplz::nonempty<std::string> t("1");
	}());
	REQUIRE_THROWS_AS([]()
	{
		gdbplz::nonempty<std::string> t("\0test");
	}(), gdbplz::empty_container);
	REQUIRE_THROWS_AS([]()
	{
		gdbplz::nonempty<std::string> t("");
	}(), gdbplz::empty_container);
}

TEST_CASE("parse c string literal", "[parse]")
{
	REQUIRE(gdbplz::string_from_c_string_literal(R"(  "test\n"   )") == "test\n");
	REQUIRE(gdbplz::string_from_c_string_literal(R"( "te\tst\nkol\\\\es" )") == "te\tst\nkol\\\\es");
	REQUIRE(gdbplz::string_from_c_string_literal(R"( "\112\113\114" )") == "\112\113\114");
	REQUIRE(gdbplz::string_from_c_string_literal(R"***( "\"\x53\x5A" )***") == "\"\x53\x5A");
	REQUIRE(gdbplz::string_from_c_string_literal(R"***( "\"\\\t\\\\\t\f\b\xFF" )***") == "\"\\\t\\\\\t\f\b\xFF");
}

TEST_CASE("round-trip conversion", "[parse]")
{
	auto dec = [](boost::string_ref x){ return gdbplz::string_from_c_string_literal(x); };
	auto enc = [](boost::string_ref x){ return gdbplz::c_string_literal_from_string(x); };
	
	auto r1 = [&](boost::string_ref x){ return dec(x); };
	auto r3 = [&](boost::string_ref x){ return dec(enc(r1(x))); };
	auto testcases = {
		R"(  "test\n"   )",
		R"( "te\tst\nkol\\\\es" )",
		R"( "\112\113\114" )",
		R"***( "\"\x53\x5A" )***",
		R"***( "\"\\\t\\\\\t\f\b\xFF" )***"
	};
	for(auto t : testcases)
	{
		REQUIRE(r1(t) == r3(t));
	}
}

TEST_CASE("generating mi commands", "[generate]")
{
	gdbplz::mi_command cmd{ "42", "gdb-version" };
	REQUIRE(boost::lexical_cast<std::string>(cmd) == "42-gdb-version --");
}