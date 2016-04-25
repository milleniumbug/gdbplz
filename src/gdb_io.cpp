#include <map>
#include <sstream>
#include <iostream>
#include <type_traits>
#include <stdexcept>
#include <wiertlo/string.hpp>
#include <gdbplz/gdb_raw.hpp>
#include <gdbplz/internal/parsing.hpp>
#include <gdbplz/utility/ascii.hpp>

namespace gdbplz
{
	struct bad_parse : std::invalid_argument
	{
		using std::invalid_argument::invalid_argument;
	};
	
	empty_container::empty_container() :
		std::invalid_argument("empty container is not a valid value for this type")
	{
		
	}
	
	invalid_token::invalid_token(std::string passed_token) :
		std::invalid_argument("invalid token passed: \"" + passed_token + "\", it contains a non-digit"),
		passed_token(passed_token)
	{
		
	}
	
	invalid_operation::invalid_operation(std::string passed_operation) :
		std::invalid_argument("\"" + passed_operation + "\" is not a valid operation"),
		passed_operation(passed_operation)
	{
		
	}
	
	void token_verify::operator()(const std::string& tok)
	{
		if(!std::all_of(tok.begin(), tok.end(), [](char x){ return x >= '0' && x <= '9'; }))
		{
			throw invalid_token(tok);
		}
	}
	
	std::pair<value, boost::string_ref> parse_tuple_rest(boost::string_ref gdb_output)
	{
		if(gdb_output.empty() || gdb_output.front() != '{')
			throw bad_parse("not a tuple");
		
		gdb_output = wiertlo::slice(gdb_output, 1);
		std::unordered_map<std::string, value> tuple;
		while(true)
		{
			if(gdb_output.empty())
				throw bad_parse("unexpected end of output");
			if(gdb_output.front() == ',')
				gdb_output = wiertlo::slice(gdb_output, 1);
			if(gdb_output.front() == '}')
			{
				gdb_output = wiertlo::slice(gdb_output, 1);
				break;
			}
			auto value_rest_pair = parse_result_rest(gdb_output);
			tuple.insert(value_rest_pair.first);
			gdb_output = value_rest_pair.second;
		}
		return std::make_pair(tuple, gdb_output);
	}
	
	std::pair<value, boost::string_ref> parse_result_list_rest(boost::string_ref gdb_output)
	{
		if(gdb_output.empty() || gdb_output.front() != '[')
			throw bad_parse("not a list");
		
		gdb_output = wiertlo::slice(gdb_output, 1);
		std::vector<result> list;
		while(true)
		{
			if(gdb_output.empty())
				throw bad_parse("unexpected end of output");
			if(gdb_output.front() == ',')
				gdb_output = wiertlo::slice(gdb_output, 1);
			if(gdb_output.front() == ']')
			{
				gdb_output = wiertlo::slice(gdb_output, 1);
				break;
			}
			auto value_rest_pair = parse_result_rest(gdb_output);
			list.push_back(value_rest_pair.first);
			gdb_output = value_rest_pair.second;
		}
		return std::make_pair(list, gdb_output);
	}
	
	std::pair<value, boost::string_ref> parse_value_list_rest(boost::string_ref gdb_output)
	{
		if(gdb_output.empty() || gdb_output.front() != '[')
			throw bad_parse("not a list");
		
		gdb_output = wiertlo::slice(gdb_output, 1);
		std::vector<value> list;
		while(true)
		{
			if(gdb_output.empty())
				throw bad_parse("unexpected end of output");
			if(gdb_output.front() == ',')
				gdb_output = wiertlo::slice(gdb_output, 1);
			if(gdb_output.front() == ']')
			{
				gdb_output = wiertlo::slice(gdb_output, 1);
				break;
			}
			auto value_rest_pair = parse_value_rest(gdb_output);
			list.push_back(value_rest_pair.first);
			gdb_output = value_rest_pair.second;
		}
		return std::make_pair(list, gdb_output);
	}
	
	std::pair<value, boost::string_ref> parse_list_rest(boost::string_ref gdb_output)
	{
		try
		{
			return parse_value_list_rest(gdb_output);
		}
		catch(const bad_parse& ex)
		{
			return parse_result_list_rest(gdb_output);
		}
	}
	
	std::pair<value, boost::string_ref> parse_value_rest(boost::string_ref gdb_output)
	{
		if(gdb_output.empty())
			throw bad_parse("unexpected end of output");
		auto first = gdb_output.front();
		if(first == '\"')
		{
			return string_from_c_string_literal_rest(gdb_output);
		}
		else if(first == '{')
		{
			return parse_tuple_rest(gdb_output);
		}
		else if(first == '[')
		{
			return parse_list_rest(gdb_output);
		}
		throw bad_parse("???");
	}
	
	std::pair<result, boost::string_ref> parse_result_rest(boost::string_ref gdb_output)
	{
		auto eq_pos = gdb_output.find_first_of('=');
		if(eq_pos == boost::string_ref::npos)
			throw bad_parse("invalid result (no '=' character in sight)");
		
		auto name = wiertlo::slice(gdb_output, 0, eq_pos);
		auto value_rest_pair = parse_value_rest(wiertlo::slice(gdb_output, eq_pos+1));
		value val = value_rest_pair.first;
		return std::make_pair(result(std::string(name), val), value_rest_pair.second);
	}
	
	std::vector<result> parse_result_sequence(boost::string_ref gdb_output)
	{
		if(gdb_output.empty())
			throw bad_parse("unexpected end of output");
		
		std::vector<result> sequence;
		while(wiertlo::trim_left(gdb_output) != "")
		{
			if(gdb_output.front() == ',')
				gdb_output = wiertlo::slice(gdb_output, 1);
			auto value_rest_pair = parse_result_rest(gdb_output);
			sequence.push_back(value_rest_pair.first);
			gdb_output = value_rest_pair.second;
		}
		return sequence;
	}
	
	result_record parse_result_record(user_token token, boost::string_ref gdb_output)
	{
		const boost::string_ref::size_type end = gdb_output.find_first_of(',');
		result_class rc = [](boost::string_ref rescl)
		{
			if(rescl == "done")
				return result_class::done;
			else if(rescl == "running")
				return result_class::done;
			else if(rescl == "connected")
				return result_class::connected;
			else if(rescl == "error")
				return result_class::error;
			else if(rescl == "exit")
				return result_class::exit;
			throw bad_parse("invalid result-class");
		}(wiertlo::slice(gdb_output, 0, end));
		gdb_output = wiertlo::slice(gdb_output, end+1);
		std::vector<result> result_sequence;
		if(end != boost::string_ref::npos)
			result_sequence = parse_result_sequence(gdb_output);
		result_record rec = { token, rc, result_sequence };
		return rec;
	}
	
	async_output parse_async_record(user_token token, boost::string_ref gdb_output)
	{
		const boost::string_ref::size_type end = gdb_output.find_first_of(',');
		async_class rc = [](boost::string_ref rescl)
		{
			static const std::map<boost::string_ref, async_class> classes = {
				{ "stopped", async_class::stopped },
				{ "running", async_class::running },
				{ "thread-group-added", async_class::thread_group_added },
				{ "thread-group-removed", async_class::thread_group_removed },
				{ "thread-group-started", async_class::thread_group_started },
				{ "thread-group-exited", async_class::thread_group_exited },
				{ "thread-created", async_class::thread_created },
				{ "thread-exited", async_class::thread_exited },
				{ "thread-selected", async_class::thread_selected },
				{ "library-loaded", async_class::library_loaded },
				{ "library-unloaded", async_class::library_unloaded },
				{ "traceframe-changed", async_class::traceframe_changed },
				{ "tsv-created", async_class::tsv_created },
				{ "tsv-modified", async_class::tsv_modified },
				{ "tsv-deleted", async_class::tsv_deleted },
				{ "breakpoint-created", async_class::breakpoint_created },
				{ "breakpoint-modified", async_class::breakpoint_modified },
				{ "breakpoint-deleted", async_class::breakpoint_deleted },
				{ "record-started", async_class::record_started },
				{ "record-stopped", async_class::record_stopped },
				{ "cmd-param-changed", async_class::cmd_param_changed },
				{ "memory-changed", async_class::memory_changed },
			};
			auto it = classes.find(rescl);
			if(it != classes.end())
				return it->second;
			else
			{
				std::cerr << "Notify maintainer of a missing async class: " << rescl << "\n";
				return async_class::unknown;
			}
		}(wiertlo::slice(gdb_output, 0, end));
		gdb_output = wiertlo::slice(gdb_output, end+1);
		std::vector<result> result_sequence;
		if(!gdb_output.empty())
			result_sequence = parse_result_sequence(gdb_output);
		async_output rec = { token, rc, result_sequence };
		return rec;
	}
	
	output parse(boost::string_ref gdb_output)
	{
		if(wiertlo::trim(gdb_output) == "(gdb)")
			return end_of_output_tag();
		const boost::string_ref starting_characters = "^*+=~@&";
		const boost::string_ref::size_type end = gdb_output.find_first_of(starting_characters);
		const user_token token(wiertlo::slice(gdb_output, 0, end));
		const char recogniser_char = gdb_output[end];
		
		gdb_output = wiertlo::slice(gdb_output, end+1);
		if(recogniser_char == '^')
		{
			return parse_result_record(token, gdb_output);
		}
		else if(recogniser_char == '*')
		{
			return async_record(exec_async_output(parse_async_record(token, gdb_output)));
		}
		else if(recogniser_char == '+')
		{
			return async_record(status_async_output(parse_async_record(token, gdb_output)));
		}
		else if(recogniser_char == '=')
		{
			return async_record(notify_async_output(parse_async_record(token, gdb_output)));
		}
		else if(recogniser_char == '~')
		{
			return stream_record(console_stream_output(string_from_c_string_literal(gdb_output)));
		}
		else if(recogniser_char == '@')
		{
			return stream_record(target_stream_output(string_from_c_string_literal(gdb_output)));
		}
		else if(recogniser_char == '&')
		{
			return stream_record(log_stream_output(string_from_c_string_literal(gdb_output)));
		}
		throw bad_parse("unexpected input");
	}
	
	void mi_operation_verify::operator()(const std::string& tok)
	{
		static_cast<void>(tok);
	}
	
	void cli_operation_verify::operator()(const std::string& tok)
	{
		auto trimtok = wiertlo::trim(tok);
		if(trimtok.empty())
			throw invalid_operation(tok);
		if(trimtok.front() == '-')
			throw invalid_operation(tok);
	}
	
	std::string string_from_c_string_literal(boost::string_ref literal)
	{
		return string_from_c_string_literal_rest(literal).first;
	}
	
	std::string c_string_literal_from_string(boost::string_ref literal)
	{
		std::string result = "\"";
		static_assert(wiertlo::is_basic_source_character_set_ascii_compatible(), "");
		auto printable_as_is = [](unsigned char ch)
		{
			return
				ch >= 32 && ch < 127 && // printable
				ch != '\\' &&
				ch != '\"' &&
				ch != '\?';
		};
		for(const auto ch : literal)
		{
			const auto uch = static_cast<unsigned char>(ch);
			if(printable_as_is(uch))
			{
				result += ch;
			}
			else
			{
				auto hexchars = "0123456789ABCDEF";
				result += "\\x";
				result += hexchars[uch / 16];
				result += hexchars[uch % 16];
			}
		}
		result += "\"";
		return result;
	}
	
	std::pair<std::string, boost::string_ref> string_from_c_string_literal_rest(boost::string_ref literal)
	{
		using wiertlo::trim;
		using wiertlo::slice;
		
		std::string result;
		if(!literal.starts_with('\"'))
			throw bad_parse("not a string literal (doesn't start with a quote character)");
		
		bool escape_char = false;
		std::string num;
		literal = slice(literal, 1);
		for(boost::string_ref::size_type i = 0; i < literal.size(); ++i)
		{
			auto ch = literal[i];
			if(num.size() > 0 && num.size() < 4)
			{
				num += ch;
				if(num.size() == 4)
				{
					result += std::stoi(num, nullptr, 0);
					num.clear();
				}
			}
			else if(escape_char)
			{
				if(ch == 'a')
					result += '\a';
				if(ch == 'b')
					result += '\b';
				if(ch == 'f')
					result += '\f';
				if(ch == 'n')
					result += '\n';
				if(ch == 'r')
					result += '\r';
				if(ch == 't')
					result += '\t';
				if(ch == '\\')
					result += '\\';
				if(ch == '\'')
					result += '\'';
				if(ch == '\"')
					result += '\"';
				if(ch >= '0' && ch <= '9')
				{
					num = "0";
					num += ch;
				}
				if(ch == 'x')
					num = "0x";
				if(ch == 'u')
					throw bad_parse("unicode escape sequences not implemented");
				escape_char = false;
			}
			else if(ch == '\"')
				return std::make_pair(result, wiertlo::slice(literal, i+1));
			else if(ch == '\\')
				escape_char = true;
			else
				result += ch;
		}
		throw bad_parse("not a string literal (unterminated)");
	}
	
	std::ostream& operator<<(std::ostream& os, const user_token& token)
	{
		os << token.get();
		return os;
	}
	
	std::ostream& operator<<(std::ostream& os, const mi_operation& op)
	{
		os << op.get();
		return os;
	}
	
	std::ostream& operator<<(std::ostream& os, const cli_operation& op)
	{
		os << op.get();
		return os;
	}
	
	std::ostream& operator<<(std::ostream& os, const option& opt)
	{
		os << '-' << gdbplz::c_string_literal_from_string(opt.name.get());
		if(opt.value)
		{
			os << " " << opt.value->get();
		}
		return os;
	}
	
	std::ostream& operator<<(std::ostream& os, const mi_command& command)
	{
		os << command.token << "-" << command.operation;
		for(auto&& opt : command.options)
		{
			os << " " << opt;
		}
		os << " ";
		//os << " --";
		for(auto&& param : command.parameters)
		{
			os << " " << param.get();
		}
		os << "\n";
		return os;
	}
	
	std::ostream& operator<<(std::ostream& os, const cli_command& command)
	{
		os << command.token << wiertlo::trim(command.command) << "\n";
		return os;
	}
	
	std::string to_string(const value& val)
	{
		std::stringstream ss;
		struct stringifier_visitor : public boost::static_visitor<void>
		{
			std::ostream& os;
			
			void operator()(const std::string& s) const
			{
				os << c_string_literal_from_string(s);
			}
			
			void operator()(const std::unordered_map<std::string, value>& s) const
			{
				os << '{';
				bool first = true;
				for(auto& kvp : s)
				{
					if(!first)
						os << ',';
					first = false;
					os << kvp.first << '=';
					boost::apply_visitor(*this, kvp.second);
				}
				os << '}';
			}
			
			void operator()(const std::vector<value>& s) const
			{
				os << '[';
				bool first = true;
				for(auto& v : s)
				{
					if(!first)
						os << ',';
					first = false;
					boost::apply_visitor(*this, v);
				}
				os << ']';
			}
			
			void operator()(const std::vector<std::pair<std::string, value>>& s) const
			{
				os << '[';
				bool first = true;
				for(auto& kvp : s)
				{
					if(!first)
						os << ',';
					first = false;
					os << kvp.first << '=';
					boost::apply_visitor(*this, kvp.second);
				}
				os << ']';
			}
			
			stringifier_visitor(std::ostream& os) : os(os) {}
		} vis(ss);
		boost::apply_visitor(vis, val);
		return ss.str();
	}
	
	struct logger_visitor_async_output : boost::static_visitor<void>
	{
		std::ostream& os;
		logger_visitor_async_output(std::ostream& os) : os(os) {}
		
		template<typename T>
		void operator()(T&& async_output) const
		{
			auto s = async_output.get();
			os << "USERTOKEN: " << s.token << " ASYNC-CLASS: " << static_cast<int>(s.state) << " RESULTS: ";
			for(auto& x : s.results)
			{
				os << x.first << "=>" << gdbplz::to_string(x.second) << " ";
			}
			os << "\n";
		}
	};
	
	void logger_visitor::operator()(const gdbplz::end_of_output_tag&) const
	{
		os << "GET USER INPUT\n";
	}
	
	void logger_visitor::operator()(const gdbplz::result_record& s) const
	{
		os << "USERTOKEN: " << s.token << " RESULT-CLASS: " << static_cast<int>(s.state) << " RESULTS: ";
		for(auto& x : s.results)
		{
			os << x.first << "=>" << gdbplz::to_string(x.second) << " ";
		}
		os << "\n";
	}
	
	void logger_visitor::operator()(const gdbplz::async_record& s) const
	{
		logger_visitor_async_output vis(std::cout);
		boost::apply_visitor(vis, s);
	}
	
	void logger_visitor::operator()(const gdbplz::stream_record&) const
	{
		
	}
}