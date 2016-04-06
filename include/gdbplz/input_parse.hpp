#ifndef GDBPLZ_INPUT_PARSE_HPP_BAC8A7E229994263AE976E0FC163F31E
#define GDBPLZ_INPUT_PARSE_HPP_BAC8A7E229994263AE976E0FC163F31E

#include <boost/utility/string_ref.hpp>
#include <boost/variant.hpp>
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
	
	enum class result_class { done, connected, error, exit };
	enum class async_class { stopped, running };
	
	struct result_record
	{
		std::string token;
		result_class state;
		std::vector<result> results;
	};
	
	struct async_output
	{
		std::string token;
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
	
	output parse_next(boost::string_ref gdb_output);
}

#endif