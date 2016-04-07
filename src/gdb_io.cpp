#include <iostream>
#include <type_traits>
#include <stdexcept>
#include "../include/gdbplz/utility/ascii.hpp"
#include "../include/gdbplz/utility/string.hpp"
#include "../include/gdbplz/gdb_io.hpp"

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
	
	output parse_next(boost::string_ref gdb_output)
	{
		// TODO: implement
		static_cast<void>(gdb_output);
		return output();
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
	
	std::string string_from_c_string_literal(boost::string_ref literal)
	{
		using wiertlo::trim;
		using wiertlo::slice;
		
		std::string result;
		literal = trim(literal);
		if(!literal.starts_with('\"') || !literal.ends_with('\"'))
			throw bad_parse("not a string literal (unquoted)");
		
		bool escape_char = false;
		std::string num;
		
		for(auto ch : slice(literal, 1, -1))
		{
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
					throw "NOT IMPLEMENTED";
				escape_char = false;
			}
			else if(ch != '\\')
				result += ch;
			else
				escape_char = true;
		}
		return result;
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
		os << " --";
		for(auto&& param : command.parameters)
		{
			os << " " << param.get();
		}
		return os;
	}
	
	std::ostream& operator<<(std::ostream& os, const cli_command& command)
	{
		os << command.token << wiertlo::trim(command.command) << "\n";
		return os;
	}
}