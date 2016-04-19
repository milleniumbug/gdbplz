#include <functional>
#include <future>
#include <thread>
#include <map>
#include "../include/gdbplz/utility/blocking_queue.hpp"
#include <wiertlo/lambda_visitor.hpp>
#include "../include/gdbplz/gdb_io.hpp"
#include "../include/gdbplz/session.hpp"

namespace gdbplz
{
	template<typename T>
	class Counter
	{
	private:
		T c;
	public:
		Counter() { c = T(); }
		T operator()() { return ++c; }
	};
	
	struct session::impl : wiertlo::pimpl_implementation_mixin<session::pimpl_handle_type, session::impl>
	{
		Counter<unsigned> token_counter; // access from the class
		connection connection_; // (thread-safe)
		std::thread event_thread;
		
		impl(connection connection_) :
			token_counter(),
			connection_(std::move(connection_))
		{
			
		}
	};
	
	session::~session()
	{
		auto& i = impl::get(pi);
		i.event_thread.detach();
	}
	
	session::session(session&&) = default;
	session& session::operator=(session&&) = default;
	
	session::session(gdbplz::connection conn) :
		pi(impl::create_pimpl_handle(std::unique_ptr<impl>(new impl(std::move(conn)))))
	{
		auto& i = impl::get(pi);
		// set up thread now, when everything's initialized
		i.event_thread = std::thread([this]()
		{
			auto& i = impl::get(pi);
			bool quit = false;
			do
			{
				auto resopt = i.connection_.wait();
				if(!resopt)
					continue;
				boost::apply_visitor(wiertlo::make_lambda_visitor<void>(
					[&](const output& out)
					{
						boost::apply_visitor(wiertlo::make_lambda_visitor<void>(
						[&](const result_record& res)
						{
							
						},
						[&](const async_record& as)
						{
							
						},
						[&](const stream_record& str)
						{
							
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
	
	inferior session::launch_local_program(local_params params)
	{
		auto& i = impl::get(pi);
		//i.connection_.send(mi_command{ i.token_counter(), "exec-run"});
		//params.arguments;
		return inferior(*this, params);
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
}

namespace std
{
	std::size_t hash<::gdbplz::function_id>::operator()(const ::gdbplz::function_id& id) const
	{
		return std::hash<std::string>()(::gdbplz::function_id::impl::get_handle(id.pi));
	}
}