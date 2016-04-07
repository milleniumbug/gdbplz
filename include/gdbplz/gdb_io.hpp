#ifndef GDBPLZ_INPUT_PARSE_HPP_BAC8A7E229994263AE976E0FC163F31E
#define GDBPLZ_INPUT_PARSE_HPP_BAC8A7E229994263AE976E0FC163F31E

#include <boost/utility/string_ref.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <iosfwd>
#include <string>
#include <unordered_map>
#include <vector>
#include "./utility/strong_typedef.hpp"

namespace gdbplz
{
	class empty_container : std::invalid_argument
	{
	public:
		empty_container();
	};
	
	struct nonempty_verify
	{
		template<typename T>
		void operator()(const T& c)
		{
			if(c.empty())
				throw empty_container();
		}
	};
	
	template<typename T>
	struct nonempty : wiertlo::strong_typedef<T, nonempty<T>, nonempty_verify> { using wiertlo::strong_typedef<T, nonempty<T>, nonempty_verify>::strong_typedef; };
	
	class invalid_token : std::invalid_argument
	{
	public:
		std::string passed_token;
		
		invalid_token(std::string passed_token);
	};
	
	struct token_verify
	{
		void operator()(const std::string& tok);
	};
	
	WIERTLO_STRONG_TYPEDEF_WITH_PRECONDITION(user_token, token_verify, std::string);
	std::ostream& operator<<(std::ostream& os, const user_token& token);
	
	
	typedef boost::make_recursive_variant<
		std::string,
		std::unordered_map<std::string, boost::recursive_variant_>,
		std::vector<boost::recursive_variant_>,
		std::vector<std::pair<std::string, boost::recursive_variant_>>
	>::type value;
	
	typedef std::pair<std::string, value> result;
	
	enum class result_class { done, connected, error, exit };
	enum class async_class { stopped, running };
	
	struct result_record
	{
		user_token token;
		result_class state;
		std::vector<result> results;
	};
	
	struct async_output
	{
		user_token token;
		std::vector<result> results;
	};
	
	WIERTLO_STRONG_TYPEDEF(exec_async_output, async_output);
	WIERTLO_STRONG_TYPEDEF(status_async_output, async_output);
	WIERTLO_STRONG_TYPEDEF(notify_async_output, async_output);
	
	WIERTLO_STRONG_TYPEDEF(console_stream_output, std::string);
	WIERTLO_STRONG_TYPEDEF(target_stream_output, std::string);
	WIERTLO_STRONG_TYPEDEF(log_stream_output, std::string);
	
	typedef boost::variant<
		exec_async_output,
		status_async_output,
		notify_async_output
	> async_record;
	
	typedef boost::variant<
		console_stream_output,
		target_stream_output,
		log_stream_output
	> stream_record;
	
	typedef boost::variant<
		result_record,
		async_record,
		stream_record
	> output;
	
	struct option
	{
		nonempty<std::string> name;
		boost::optional<nonempty<std::string>> value;
	};
	std::ostream& operator<<(std::ostream& os, const option& opt);
	
	class invalid_operation : std::invalid_argument
	{
	public:
		std::string passed_operation;
		
		invalid_operation(std::string passed_operation);
	};
	
	struct mi_operation_verify
	{
		void operator()(const std::string& tok);
	};
	WIERTLO_STRONG_TYPEDEF_WITH_PRECONDITION(mi_operation, mi_operation_verify, std::string);
	std::ostream& operator<<(std::ostream& os, const mi_operation& command);
	
	struct cli_operation_verify
	{
		void operator()(const std::string& tok);
	};
	WIERTLO_STRONG_TYPEDEF_WITH_PRECONDITION(cli_operation, cli_operation_verify, std::string);
	std::ostream& operator<<(std::ostream& os, const cli_operation& command);
	
	struct mi_command
	{
		user_token token;
		mi_operation operation;
		std::vector<option> options;
		std::vector<nonempty<std::string>> parameters;
	};
	std::ostream& operator<<(std::ostream& os, const mi_command& command);
	
	struct cli_command
	{
		user_token token;
		std::string command;
	};
	std::ostream& operator<<(std::ostream& os, const cli_command& command);
	
	std::string c_string_literal_from_string(boost::string_ref literal);
	std::string string_from_c_string_literal(boost::string_ref literal);
	output parse_next(boost::string_ref gdb_output);
}

#endif