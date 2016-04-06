#include <cassert>
#include <memory>
#include <thread>
#include <chrono>
#include <boost/tokenizer.hpp>
#include "../../../NotMine/jucipp/tiny-process-library/process.hpp"
#include "../include/gdbplz/connection.hpp"
#include "../include/gdbplz/utility/blocking_queue.hpp"

namespace gdbplz
{
	struct connection::impl : wiertlo::pimpl_implementation_mixin<connection::pimpl_handle_type, connection::impl>
	{	
		utility::blocking_queue<std::string> input_queue;
		boost::filesystem::path gdb_executable;
		std::unique_ptr<Process> gdb_process;
	};
	
	connection::~connection() = default;
	connection::connection(connection&&) = default;
	connection& connection::operator=(connection&&) = default;
	
	connection::connection(boost::filesystem::path gdb_executable) :
		pi(impl::create_pimpl_handle(std::unique_ptr<impl>(new impl())))
	{
		auto& i = impl::get(pi);
		i.gdb_executable = gdb_executable;
		restart();
	}
	
	void connection::send(boost::string_ref c)
	{
		auto& i = impl::get(pi);
		i.gdb_process->write(c.data(), c.size());
	}
	
	boost::optional<std::string> connection::poll()
	{
		auto& i = impl::get(pi);
		return i.input_queue.try_pop();
	}
	
	boost::optional<std::string> connection::wait()
	{
		auto& i = impl::get(pi);
		return i.input_queue.pop();
	}
	
	void connection::restart()
	{
		auto& i = impl::get(pi);
		if(i.gdb_process)
		{
			i.gdb_process->write("-gdb-exit\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			i.gdb_process->kill();
		}
		const std::string extra_params = "";
		i.input_queue.clear();
		i.gdb_process.reset(new Process(
			std::string("'") + i.gdb_executable.string() + "' " + extra_params + " --interpreter=mi2  -- ",
			"",
			[this](const char* buffer, std::size_t size) {
				auto& i = impl::get(pi);
				boost::string_ref output(buffer, size);
				boost::char_separator<char> sep("\r\n");
				boost::tokenizer<
					decltype(sep),
					decltype(output)::const_iterator,
					std::string> tokenizer(output, sep);
				for(auto line : tokenizer)
				{
					i.input_queue.push(line);
				}
			}
		));
		// TODO: throw if you can't launch gdb
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