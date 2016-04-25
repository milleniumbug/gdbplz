#include <functional>
#include <future>
#include <thread>
#include <map>
#include <wiertlo/lambda_visitor.hpp>
#include <gdbplz/utility/blocking_queue.hpp>
#include <gdbplz/utility/counter.hpp>
#include <gdbplz/gdb_raw.hpp>
#include <gdbplz/gdb_version.hpp>
#include <gdbplz/function_id.hpp>
#include <gdbplz/source_location.hpp>
#include <gdbplz/session.hpp>
#include <gdbplz/internal/parsing.hpp>
#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/signals2.hpp>

namespace gdbplz
{
	struct user_token_compare
	{
		bool operator()(const user_token& lhs, const user_token& rhs)
		{
			return lhs.get() < rhs.get();
		}
	};
	
	struct session::impl : wiertlo::pimpl_implementation_mixin<session::pimpl_handle_type, session::impl>
	{
		utility::Counter<unsigned> token_counter; // access from the class
		std::unique_ptr<abstract_connection> connection_; // (thread-safe)
		boost::signals2::signal<void(std::string)> signal_program_output;
		boost::signals2::signal<void(std::string)> signal_cli_responses;
		boost::signals2::signal<void(std::string)> signal_gdb_logs;
		std::thread event_thread; // access from the class
		wiertlo::blocking_queue<std::pair<user_token, std::promise<result_record>>> promises_queue;
		
		impl(std::unique_ptr<abstract_connection> connection_) :
			token_counter(),
			connection_(std::move(connection_))
		{
			
		}
		
		std::future<result_record> send(mi_command mi)
		{
			mi.token = std::to_string(token_counter());
			std::promise<result_record> promise;
			auto future = promise.get_future();
			promises_queue.push(std::make_pair(mi.token, std::move(promise)));
			connection_->send(mi);
			return future;
		}
	};
	
	std::future<result_record> session::send(mi_command mi)
	{
		auto& i = impl::get(pi);
		return i.send(mi);
	}
	
	session::~session()
	{
		auto& i = impl::get(pi);
		i.event_thread.detach();
	}
	
	session::session(session&&) = default;
	session& session::operator=(session&&) = default;
	
	boost::signals2::signal<void(std::string)>& session::signal_program_output()
	{
		auto& i = impl::get(pi);
		return i.signal_program_output;
	}
	
	boost::signals2::signal<void(std::string)>& session::signal_cli_responses()
	{
		auto& i = impl::get(pi);
		return i.signal_cli_responses;
	}
	
	boost::signals2::signal<void(std::string)>& session::signal_gdb_logs()
	{
		auto& i = impl::get(pi);
		return i.signal_gdb_logs;
	}
	
	session::session(std::unique_ptr<abstract_connection> conn) :
		pi(impl::create_pimpl_handle(std::unique_ptr<impl>(new impl(std::move(conn)))))
	{
		auto& i = impl::get(pi);
		// set up thread now, when everything's initialized
		i.event_thread = std::thread([&i]()
		{
			std::map<user_token, std::promise<result_record>, user_token_compare> promises;
			bool quit = false;
			do
			{
				auto resopt = i.connection_->wait();
				if(!resopt)
					throw "FUCK"; // TODO: suitable exception type
				boost::apply_visitor(wiertlo::make_lambda_visitor<void>(
					[&](const output& out)
					{
						boost::apply_visitor(wiertlo::make_lambda_visitor<void>(
						[&](const result_record& res)
						{
							auto it = promises.find(res.token);
							if(it != promises.end())
							{
								it->second.set_value(res);
							}
							promises.erase(it);
						},
						[&](const async_record& as)
						{
							
						},
						[&](const stream_record& str)
						{
							boost::apply_visitor(wiertlo::make_lambda_visitor<void>(
							[&](const console_stream_output& output)
							{
								i.signal_cli_responses(output.get());
							},
							[&](const target_stream_output& output)
							{
								std::string s = output.get();
								i.signal_program_output(s);
							},
							[&](const log_stream_output& output)
							{
								i.signal_gdb_logs(output.get());
							}), str);
						},
						[&](end_of_output_tag)
						{
							
						}), out);
					},
					[&](const input& in)
					{
						boost::apply_visitor(wiertlo::make_lambda_visitor<void>(
						[&](const mi_command& comm)
						{
							promises.insert(i.promises_queue.pop());
						},
						[&](const cli_command& comm)
						{
							// TODO:
						},
						[&](end_work)
						{
							quit = true;
						}), in);
					}), *resopt);
			} while(!quit);
		});
	}
	
	void fill_in_defaults(local_params& params)
	{
		if(!params.arguments)
			params.arguments = std::vector<boost::string_ref>();
		if(!params.symbol_file)
			params.symbol_file = params.debugged_executable;
		// TODO: handle in another way
		if(!params.working_directory)
			params.symbol_file = params.debugged_executable.parent_path();
	}
	
	std::shared_ptr<inferior> session::launch(local_params params)
	{
		auto& i = impl::get(pi);
		fill_in_defaults(params);
		
		std::future<result_record> result;
		std::vector<nonempty<std::string>> cmdline_args(params.arguments->begin(), params.arguments->end());
		result = i.send(mi_command{ "", "exec-arguments", {}, cmdline_args });
		if(result.get().state != result_class::done)
			throw "FUCK";
		result = i.send(mi_command{ "", "file-symbol-file", {}, { params.symbol_file->string() } });
		if(result.get().state != result_class::done)
			throw "FUCK";
		/*result = i.send(mi_command{ "", "environment-directory", {}, { params.working_directory->string() } });
		if(result.get().state != result_class::done)
			throw "FUCK";*/
		result = i.send(mi_command{ "", "file-exec-file", {}, { params.debugged_executable.string() } });
		if(result.get().state != result_class::done)
			throw "FUCK";
		result = i.send(mi_command{ "", "exec-run", {}, {} });
		if(result.get().state != result_class::done)
			throw "FUCK";
		//params.arguments;
		return nullptr;
	}
	
	gdb_version session::version() const
	{
		//auto& i = impl::get(pi);
		// TODO: implement
		return gdb_version(0, 0, 0, "");
	}
	
	const auto tied_version = [](const gdb_version& v)
	{
		return std::tie(v.major, v.minor, v.patch, v.others);
	};
	
	bool gdb_version::operator<(const gdb_version& other) const
	{
		return tied_version(*this) < tied_version(other);
	}
	
	bool gdb_version::operator==(const gdb_version& other) const
	{
		return tied_version(*this) == tied_version(other);
	}
	
// Remove the "helpful" macros defined by glibc.
// Friendly reminder: I'm using `-std=c++11 -Wall -Wextra -pedantic`, these shouldn't be there.
// I thought only <windows.h> was retarded as fuck.
#ifdef major
#undef major
#endif
#ifdef minor
#undef minor
#endif
	
	gdb_version::gdb_version(unsigned major, unsigned minor) :
		major(major),
		minor(minor),
		patch(0)
	{
		
	}
	
	gdb_version::gdb_version(unsigned major, unsigned minor, unsigned patch) :
		major(major),
		minor(minor),
		patch(patch)
	{
		
	}
	
	gdb_version::gdb_version(unsigned major, unsigned minor, unsigned patch, std::string others) :
		major(major),
		minor(minor),
		patch(patch),
		others(others)
	{
		
	}
	
	struct function_id::impl : wiertlo::pimpl_implementation_mixin<function_id::pimpl_handle_type, void*, std::string>
	{
		
	};
	
	function_id::function_id(const function_id&) = default;
	function_id& function_id::operator=(const function_id&) = default;
	function_id::function_id(function_id&&) = default;
	function_id& function_id::operator=(function_id&&) = default;
	function_id::~function_id() = default;
	
	bool function_id::operator<(const function_id& other) const
	{
		return impl::get_handle(this->pi) < impl::get_handle(other.pi);
	}
	
	bool function_id::operator==(const function_id& other) const
	{
		return impl::get_handle(this->pi) == impl::get_handle(other.pi);
	}
	
	std::string function_id::to_string() const
	{
		return impl::get_handle(this->pi);
	}
	
	source_location::source_location(boost::filesystem::path source, unsigned long long line) :
		source(source),
		line(line)
	{
		
	}
}

namespace std
{
	std::size_t hash<::gdbplz::function_id>::operator()(const ::gdbplz::function_id& id) const
	{
		return std::hash<std::string>()(::gdbplz::function_id::impl::get_handle(id.pi));
	}
}