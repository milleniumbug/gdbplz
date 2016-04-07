#ifndef GDBPLZ_INPUT_PARSE_HPP_BAC8A7E229994263AE976E0FC163F31E
#define GDBPLZ_INPUT_PARSE_HPP_BAC8A7E229994263AE976E0FC163F31E

#include <boost/utility/string_ref.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include "./utility/strong_typedef.hpp"

namespace gdbplz
{
	typedef boost::make_recursive_variant<
		std::string,
		std::unordered_map<std::string, boost::recursive_variant_>,
		std::vector<boost::recursive_variant_>,
		std::vector<std::pair<std::string, boost::recursive_variant_>>
	>::type value;
	
	typedef std::pair<std::string, value> result;
	
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
	
	class option
	{
		std::string name;
		boost::optional<std::string> value;
	};
	
	class mi_command
	{
		std::string operation;
		std::vector<option> options;
		std::vector<std::string> parameters;
	};
	
	class cli_command
	{
		user_token token;
		std::string command;
	};
	
	std::string c_string_literal_from_string(boost::string_ref literal);
	std::string string_from_c_string_literal(boost::string_ref literal);
	output parse_next(boost::string_ref gdb_output);
}

#endif