#ifndef GDBPLZ_CONNECTION_HPP_FA1DC553361142A5B279A51FF85FD663
#define GDBPLZ_CONNECTION_HPP_FA1DC553361142A5B279A51FF85FD663

#include <boost/utility/string_ref.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <string>
#include <stdexcept>
#include <wiertlo/pimpl_handle.hpp>
#include "./gdb_raw.hpp"

namespace gdbplz
{
	class connection_error : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};
	
	class gdb_not_found : public connection_error
	{
	public:
		boost::filesystem::path requested_gdb_instance;
		gdb_not_found(boost::filesystem::path requested_gdb_instance);
	};
	
	class not_a_valid_gdb_executable : public connection_error
	{
		std::string received_output;
	};
	
	boost::optional<boost::filesystem::path> guess_gdb_path();
	
	class abstract_connection
	{
	public:
		virtual ~abstract_connection() {}
		virtual void send(mi_command comm) = 0;
		virtual boost::optional<boost::variant<input, output>> wait() = 0;
		virtual boost::optional<boost::variant<input, output>> poll() = 0;
		virtual void restart() = 0;
	};
	
	class connection : public abstract_connection
	{
	private:
		struct impl;
		typedef wiertlo::pimpl_handle<impl> pimpl_handle_type;
		pimpl_handle_type pi;
		
	public:
		~connection();
		connection(connection&&);
		connection& operator=(connection&&);
		connection(const connection&) = delete;
		connection& operator=(const connection&) = delete;
		
		connection(boost::filesystem::path gdb_executable);
		
		void send(mi_command comm);
		boost::optional<boost::variant<input, output>> wait();
		boost::optional<boost::variant<input, output>> poll();
		
		void restart();
	};
}

#endif