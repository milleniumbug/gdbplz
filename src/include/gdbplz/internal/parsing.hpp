#ifndef GDBPLZ_INTERNAL_PARSING_HPP_4736E09E91A64B378FC4EB7365B299D3
#define GDBPLZ_INTERNAL_PARSING_HPP_4736E09E91A64B378FC4EB7365B299D3

#include <utility>
#include <gdbplz/gdb_raw.hpp>

namespace gdbplz
{
	std::pair<value, boost::string_ref> parse_tuple_rest(boost::string_ref gdb_output);
	std::pair<value, boost::string_ref> parse_list_rest(boost::string_ref gdb_output);
	std::pair<value, boost::string_ref> parse_value_rest(boost::string_ref gdb_output);
	std::pair<result, boost::string_ref> parse_result_rest(boost::string_ref gdb_output);
	std::vector<result> parse_result_sequence(boost::string_ref gdb_output);
	result_record parse_result_record(user_token token, boost::string_ref gdb_output);
	async_output parse_async_record(user_token token, boost::string_ref gdb_output);
	
	std::string to_string(const value& val);
	
	std::pair<std::string, boost::string_ref> string_from_c_string_literal_rest(boost::string_ref literal);
	std::string c_string_literal_from_string(boost::string_ref literal);
	std::string string_from_c_string_literal(boost::string_ref literal);
	output parse(boost::string_ref gdb_output);
	
	struct logger_visitor : public boost::static_visitor<void>
	{
		std::ostream& os;
		void operator()(const gdbplz::end_of_output_tag&) const;
		void operator()(const gdbplz::result_record& s) const;
		void operator()(const gdbplz::async_record& s) const;
		void operator()(const gdbplz::stream_record&) const;
		
		logger_visitor(std::ostream& os) : os(os) {}
	};
}

#endif