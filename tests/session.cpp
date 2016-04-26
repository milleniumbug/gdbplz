#include <iostream>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <gdbplz/session.hpp>
#include <gdbplz/internal/parsing.hpp>
#include "../Catch/include/catch.hpp"

TEST_CASE("gdb send", "[session]")
{
	boost::filesystem::path path = gdbplz::guess_gdb_path().value();
	auto conn = std::make_unique<gdbplz::connection>(path);
	gdbplz::session sess(std::move(conn));
	auto fut = sess.send(gdbplz::mi_command{ "0", "exec-arguments", {}, { "a", "b" } });
	auto s = fut.get();
	gdbplz::logger_visitor{std::cout}(s);
}

/*TEST_CASE("gdb launch", "[session]")
{
	try
	{
		boost::filesystem::path path = gdbplz::guess_gdb_path().value();
		std::stringstream ss;
		auto conn = std::make_unique<gdbplz::connection>(path);
		gdbplz::session sess(std::move(conn));
		sess.signal_program_output().connect([&](std::string output){
			ss << output;
		});
		auto inf = sess.launch([](){
			gdbplz::local_params p;
			p.working_directory = "/home/milleniumbug/dokumenty/PROJEKTY/InDevelopment/gdbplz/";
			p.debugged_executable = "/home/milleniumbug/dokumenty/PROJEKTY/InDevelopment/gdbplz/build/1";
			return p;
		}());
		std::this_thread::sleep_for(std::chrono::seconds(5));
		REQUIRE(!ss.str().empty());
	}
	catch(const std::exception& ex)
	{
		std::cerr << ex.what();
	}
}*/