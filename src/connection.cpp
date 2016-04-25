#include <cassert>
#include <memory>
#include <thread>
#include <chrono>
#include <future>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <boost/tokenizer.hpp>
#include <map>
#include "process.hpp"
#include <gdbplz/connection.hpp>
#include <gdbplz/gdb_raw.hpp>
#include <gdbplz/internal/parsing.hpp>
#include <gdbplz/utility/blocking_queue.hpp>

namespace gdbplz
{
	struct connection::impl : wiertlo::pimpl_implementation_mixin<connection::pimpl_handle_type, connection::impl>
	{
		wiertlo::blocking_queue<
			boost::variant<
				input,
				output>> input_queue;
		boost::filesystem::path gdb_executable;
		std::unique_ptr<Process> gdb_process;
		std::ofstream log;
		
		void send(boost::string_ref c)
		{
			log << "< " << c << "\n";
			gdb_process->write(c.data(), c.size());
		}
		
		void exit()
		{
			if(gdb_process)
			{
				input_queue.push(input(end_work()));
				gdb_process->write("-gdb-exit\n");
				std::this_thread::sleep_for(std::chrono::milliseconds(400));
				gdb_process->kill();
			}
		}
	};
	
	connection::~connection()
	{
		if(impl::get_handle(pi))
		{
			auto& i = impl::get(pi);
			i.exit();
		}
	}
	
	connection::connection(connection&&) = default;
	connection& connection::operator=(connection&&) = default;
	
	connection::connection(boost::filesystem::path gdb_executable) :
		pi(impl::create_pimpl_handle(std::unique_ptr<impl>(new impl())))
	{
		auto& i = impl::get(pi);
		i.gdb_executable = gdb_executable;
		i.log.open("out");
		restart();
	}
	
	void connection::restart()
	{
		auto& i = impl::get(pi);
		i.exit();
		const std::string extra_params = "";
		i.input_queue.clear();
		i.gdb_process.reset(new Process(
			std::string("'") + i.gdb_executable.string() + "' " + extra_params + " --interpreter=mi2  -- ",
			"",
			[&i](const char* buffer, std::size_t size) {
				boost::string_ref output(buffer, size);
				boost::char_separator<char> sep("\r\n");
				boost::tokenizer<
					decltype(sep),
					decltype(output)::const_iterator,
					std::string> tokenizer(output, sep);
				for(auto line : tokenizer)
				{
					i.log << "> " << line << "\n";
					i.input_queue.push(gdbplz::parse(line));
				}
			}, // stdin output function
			[](const char*, std::size_t) {
				// just ignore it
			}, // stderr output function
			true // open stdin
		));
		/*int exitcode;
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		if(i.gdb_process->try_get_exit_status(exitcode))
		{
			i.gdb_process->kill();
			throw gdb_not_found(i.gdb_executable);
		}*/
		// TODO: throw if you can't launch gdb
	}
	
	void connection::send(mi_command comm)
	{
		auto& i = impl::get(pi);
		i.send(boost::lexical_cast<std::string>(comm));
		i.input_queue.push(input(std::move(comm)));
	}
	
	boost::optional<boost::variant<input, output>> connection::wait()
	{
		auto& i = impl::get(pi);
		return i.input_queue.pop();
	}
	
	boost::optional<boost::variant<input, output>> connection::poll()
	{
		auto& i = impl::get(pi);
		return i.input_queue.try_pop();
	}
	
	gdb_not_found::gdb_not_found(boost::filesystem::path requested_gdb_instance) :
		connection_error("Invalid or missing gdb " + requested_gdb_instance.string()),
		requested_gdb_instance(requested_gdb_instance)
	{
		
	}
	
	boost::optional<boost::filesystem::path> guess_gdb_path()
	{
		std::string out;
		Process which_gdb(
			"which gdb",
			"",
			[&out](const char* buffer, std::size_t size) {
				out.append(buffer, size);
			}
		);
		if(which_gdb.get_exit_status() == 0)
		{
			auto remove_trailing_newline = [&](){ out.pop_back(); };
			remove_trailing_newline();
			return boost::filesystem::path(out);
		}
		else
		{
			return boost::none;
		}
	}
}