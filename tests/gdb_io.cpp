#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/variant.hpp>
#include <wiertlo/string.hpp>
#include <wiertlo/lambda_visitor.hpp>
#include <gdbplz/gdb_raw.hpp>
#include <gdbplz/internal/parsing.hpp>
#include "../Catch/include/catch.hpp"
#include "../rapidcheck/include/rapidcheck.h"

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
	REQUIRE(gdbplz::string_from_c_string_literal(R"("test\n")") == "test\n");
	REQUIRE(gdbplz::string_from_c_string_literal(R"("te\tst\nkol\\\\es")") == "te\tst\nkol\\\\es");
	REQUIRE(gdbplz::string_from_c_string_literal(R"("\112\113\114")") == "\112\113\114");
	REQUIRE(gdbplz::string_from_c_string_literal(R"***("\"\x53\x5A")***") == "\"\x53\x5A");
	REQUIRE(gdbplz::string_from_c_string_literal(R"***("\"\\\t\\\\\t\f\b\xFF")***") == "\"\\\t\\\\\t\f\b\xFF");
}

TEST_CASE("round-trip conversion", "[parse]")
{
	auto dec = [](boost::string_ref x){ return gdbplz::string_from_c_string_literal(x); };
	auto enc = [](boost::string_ref x){ return gdbplz::c_string_literal_from_string(x); };
	
	auto r1 = [&](boost::string_ref x){ return dec(x); };
	auto r3 = [&](boost::string_ref x){ return dec(enc(r1(x))); };
	auto testcases = {
		R"("test\n")",
		R"("te\tst\nkol\\\\es")",
		R"("\112\113\114")",
		R"***("\"\x53\x5A")***",
		R"***("\"\\\t\\\\\t\f\b\xFF")***"
	};
	for(auto t : testcases)
	{
		REQUIRE(r1(t) == r3(t));
	}
}

TEST_CASE("round-trip conversion rapidcheck", "[parse]")
{
	auto dec = [](boost::string_ref x){ return gdbplz::string_from_c_string_literal(x); };
	auto enc = [](boost::string_ref x){ return gdbplz::c_string_literal_from_string(x); };
	
	auto r1 = [&](boost::string_ref x){ return enc(x); };
	auto r3 = [&](boost::string_ref x){ return enc(dec(r1(x))); };
	rc::check([&](const std::string& s)
	{
		REQUIRE(r1(s) == r3(s));
	});
}

TEST_CASE("generating mi commands", "[generate]")
{
	gdbplz::mi_command cmd{ "42", "gdb-version" };
	REQUIRE(boost::lexical_cast<std::string>(cmd) == "42-gdb-version \n");
}

TEST_CASE("simple command log", "[parse]")
{
	boost::string_ref log =
R"!#%^&*****(=thread-group-added,id="i1"
~"GNU gdb (GDB) Fedora 7.10.1-30.fc23\n"
~"Copyright (C) 2015 Free Software Foundation, Inc.\n"
~"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.  Type \"show copying\"\nand \"show warranty\" for details.\n"
~"This GDB was configured as \"x86_64-redhat-linux-gnu\".\nType \"show configuration\" for configuration details."
~"\nFor bug reporting instructions, please see:\n"
~"<http://www.gnu.org/software/gdb/bugs/>.\n"
~"Find the GDB manual and other documentation resources online at:\n<http://www.gnu.org/software/gdb/documentation/>.\n"
~"For help, type \"help\".\n"
~"Type \"apropos word\" to search for commands related to \"word\"...\n"
~"Reading symbols from build/gdbplz_tests..."
~"(no debugging symbols found)...done.\n"
(gdb) 
&"Catch requires an event name.\n"
^error,msg="Catch requires an event name."
(gdb) 
~"Catchpoint 1 (throw)\n"
=breakpoint-created,bkpt={number="1",type="breakpoint",disp="keep",enabled="y",addr="0x0000000000603130",what="exception throw",catch-type="throw",thread-groups=["i1"],times="0"}
^done
(gdb) 
~"Starting program: /home/milleniumbug/dokumenty/PROJEKTY/InDevelopment/gdbplz/build/gdbplz_tests \n"
=thread-group-started,id="i1",pid="784"
=thread-created,id="1",group-id="i1"
=library-loaded,id="/lib64/ld-linux-x86-64.so.2",target-name="/lib64/ld-linux-x86-64.so.2",host-name="/lib64/ld-linux-x86-64.so.2",symbols-loaded="0",thread-group="i1"
^running
*running,thread-id="all"
(gdb) 
=library-loaded,id="/lib64/libboost_system.so.1.58.0",target-name="/lib64/libboost_system.so.1.58.0",host-name="/lib64/libboost_system.so.1.58.0",symbols-loaded="0",thread-group="i1"
=library-loaded,id="/lib64/libboost_filesystem.so.1.58.0",target-name="/lib64/libboost_filesystem.so.1.58.0",host-name="/lib64/libboost_filesystem.so.1.58.0",symbols-loaded="0",thread-group="i1"
=library-loaded,id="/lib64/libpthread.so.0",target-name="/lib64/libpthread.so.0",host-name="/lib64/libpthread.so.0",symbols-loaded="0",thread-group="i1"
=library-loaded,id="/lib64/libstdc++.so.6",target-name="/lib64/libstdc++.so.6",host-name="/lib64/libstdc++.so.6",symbols-loaded="0",thread-group="i1"
=library-loaded,id="/lib64/libm.so.6",target-name="/lib64/libm.so.6",host-name="/lib64/libm.so.6",symbols-loaded="0",thread-group="i1"
=library-loaded,id="/lib64/libgcc_s.so.1",target-name="/lib64/libgcc_s.so.1",host-name="/lib64/libgcc_s.so.1",symbols-loaded="0",thread-group="i1"
=library-loaded,id="/lib64/libc.so.6",target-name="/lib64/libc.so.6",host-name="/lib64/libc.so.6",symbols-loaded="0",thread-group="i1"
=library-loaded,id="/lib64/librt.so.1",target-name="/lib64/librt.so.1",host-name="/lib64/librt.so.1",symbols-loaded="0",thread-group="i1"
~"[Thread debugging using libthread_db enabled]\n"
~"Using host libthread_db library \"/lib64/libthread_db.so.1\".\n"
=breakpoint-modified,bkpt={number="1",type="breakpoint",disp="keep",enabled="y",addr="0x00007ffff74b1b3d",what="exception throw",catch-type="throw",thread-groups=["i1"],times="0"}
=breakpoint-modified,bkpt={number="1",type="breakpoint",disp="keep",enabled="y",addr="0x00007ffff74b1b3d",what="exception throw",catch-type="throw",thread-groups=["i1"],times="1"}
~"Catchpoint 1 (exception thrown), __cxxabiv1::__cxa_throw (obj=0x944e60, tinfo=0x6b9980 <typeinfo for gdbplz::invalid_token>, dest=0x61a776 <gdbplz::invalid_token::~invalid_token()>) at ../../../../libstdc++-v3/libsupc++/eh_throw.cc:63\n"
~"63\t  PROBE2 (throw, obj, tinfo);\n"
*stopped,reason="breakpoint-hit",disp="keep",bkptno="1",frame={addr="0x00007ffff74b1b3d",func="__cxxabiv1::__cxa_throw",args=[{name="obj",value="0x944e60"},{name="tinfo",value="0x6b9980 <typeinfo for gdbplz::invalid_token>"},{name="dest",value="0x61a776 <gdbplz::invalid_token::~invalid_token()>"}],file="../../../../libstdc++-v3/libsupc++/eh_throw.cc",fullname="/usr/src/debug/gcc-5.3.1-20151207/libstdc++-v3/libsupc++/eh_throw.cc",line="63"},thread-id="1",stopped-threads="all",core="1"
(gdb) 
~"Continuing.\n"
^running
*running,thread-id="all"
(gdb) 
=breakpoint-modified,bkpt={number="1",type="breakpoint",disp="keep",enabled="y",addr="0x00007ffff74b1b3d",what="exception throw",catch-type="throw",thread-groups=["i1"],times="2"}
~"Catchpoint 1 (exception thrown), __cxxabiv1::__cxa_throw (obj=0x945220, tinfo=0x6b99d0 <typeinfo for gdbplz::empty_container>, dest=0x61a7e8 <gdbplz::empty_container::~empty_container()>) at ../../../../libstdc++-v3/libsupc++/eh_throw.cc:63\n"
~"63\t  PROBE2 (throw, obj, tinfo);\n"
*stopped,reason="breakpoint-hit",disp="keep",bkptno="1",frame={addr="0x00007ffff74b1b3d",func="__cxxabiv1::__cxa_throw",args=[{name="obj",value="0x945220"},{name="tinfo",value="0x6b99d0 <typeinfo for gdbplz::empty_container>"},{name="dest",value="0x61a7e8 <gdbplz::empty_container::~empty_container()>"}],file="../../../../libstdc++-v3/libsupc++/eh_throw.cc",fullname="/usr/src/debug/gcc-5.3.1-20151207/libstdc++-v3/libsupc++/eh_throw.cc",line="63"},thread-id="1",stopped-threads="all",core="1"
(gdb) 
~"#0  __cxxabiv1::__cxa_throw (obj=0x945220, tinfo=0x6b99d0 <typeinfo for gdbplz::empty_container>, dest=0x61a7e8 <gdbplz::empty_container::~empty_container()>) at ../../../../libstdc++-v3/libsupc++/eh_throw.cc:63\n"
~"#1  0x000000000060f2c6 in void gdbplz::nonempty_verify::operator()<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) ()\n"
~"#2  0x000000000060e834 in wiertlo::strong_typedef<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, gdbplz::nonempty<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, gdbplz::nonempty_verify>::strong_typedef<char const (&) [6], (void*)0>(char const (&) [6]) ()\n"
~"#3  0x000000000060df33 in gdbplz::nonempty<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >::nonempty<char const (&) [6], (void*)0>(char const (&) [6]) ()\n"
~"#4  0x0000000000609de9 in ____C_A_T_C_H____T_E_S_T____28()::{lambda()#3}::operator()() const ()\n"
~"#5  0x000000000060a00d in ____C_A_T_C_H____T_E_S_T____28 ()\n"
~"#6  0x0000000000637efa in Catch::FreeFunctionTestCase::invoke() const ()\n"
~"#7  0x0000000000623dd5 in Catch::TestCase::invoke() const ()\n"
~"#8  0x0000000000636f8f in Catch::RunContext::invokeActiveTestCase() ()\n"
~"#9  0x0000000000636c56 in Catch::RunContext::runCurrentTest(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&) ()\n"
~"#10 0x000000000063562c in Catch::RunContext::runTest(Catch::TestCase const&) ()\n"
~"#11 0x0000000000621623 in Catch::runTests(Catch::Ptr<Catch::Config> const&) ()\n"
~"#12 0x000000000063780b in Catch::Session::run() ()\n"
~"#13 0x00000000006376e2 in Catch::Session::run(int, char const**) ()\n"
~"#14 0x0000000000637712 in Catch::Session::run(int, char**) ()\n"
~"#15 0x0000000000629a0c in main ()\n"
^done
(gdb) 
~"Continuing.\n"
^running
*running,thread-id="all"
(gdb) 
=breakpoint-modified,bkpt={number="1",type="breakpoint",disp="keep",enabled="y",addr="0x00007ffff74b1b3d",what="exception throw",catch-type="throw",thread-groups=["i1"],times="3"}
~"Catchpoint 1 (exception thrown), __cxxabiv1::__cxa_throw (obj=0x945220, tinfo=0x6b99d0 <typeinfo for gdbplz::empty_container>, dest=0x61a7e8 <gdbplz::empty_container::~empty_container()>) at ../../../../libstdc++-v3/libsupc++/eh_throw.cc:63\n"
~"63\t  PROBE2 (throw, obj, tinfo);\n"
*stopped,reason="breakpoint-hit",disp="keep",bkptno="1",frame={addr="0x00007ffff74b1b3d",func="__cxxabiv1::__cxa_throw",args=[{name="obj",value="0x945220"},{name="tinfo",value="0x6b99d0 <typeinfo for gdbplz::empty_container>"},{name="dest",value="0x61a7e8 <gdbplz::empty_container::~empty_container()>"}],file="../../../../libstdc++-v3/libsupc++/eh_throw.cc",fullname="/usr/src/debug/gcc-5.3.1-20151207/libstdc++-v3/libsupc++/eh_throw.cc",line="63"},thread-id="1",stopped-threads="all",core="1"
(gdb)
)!#%^&*****";
	boost::char_separator<char> sep("\r\n");
	boost::tokenizer<
		decltype(sep),
		decltype(log)::const_iterator,
		std::string> tokenizer(log, sep);
	for(auto line : tokenizer)
	{
		auto out = gdbplz::parse(line);
		//boost::apply_visitor(vis, out);
	}
}

TEST_CASE("gdb mi buggy async records", "[parse]")
{
	std::string input = R"**(*running,thread-id="all"
	)**";
	auto out = gdbplz::parse(input);
	gdbplz::logger_visitor vis(std::cout);
	boost::apply_visitor(vis, out);
}