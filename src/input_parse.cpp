#include <type_traits>
#include <stdexcept>
#include "../include/gdbplz/input_parse.hpp"

namespace gdbplz
{
	struct bad_parse : std::invalid_argument
	{
		using std::invalid_argument::invalid_argument;
	};
	
	boost::string_ref slice(
		boost::string_ref in,
		std::make_signed<boost::string_ref::size_type>::type b,
		std::make_signed<boost::string_ref::size_type>::type e = -1)
	{
		auto to_index = [&](std::make_signed<boost::string_ref::size_type>::type i){ return i >= 0 ? i : in.size()+i; };
		b = to_index(b);
		e = to_index(e);
		return in.substr(b, e-b);
	}
	
	boost::string_ref trim(boost::string_ref in, boost::string_ref whitespace = " \r\n\t")
	{
		auto b = in.find_first_not_of(whitespace);
		auto e = in.find_last_not_of(whitespace);
		return slice(in, b, e+1);
	}
	
	std::string string_from_c_string_literal(boost::string_ref literal)
	{
		std::string result;
		literal = trim(literal);
		if(!literal.starts_with('\"') || !literal.ends_with('\"'))
			throw bad_parse("not a string literal (unquoted)");
		
		bool escape_char = false;
		std::string num;
		
		for(auto ch : slice(literal, 1, -1))
		{
			if(num.size() < 4)
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
				escape_char = false;
			}
			else if(ch != '\\')
				result += ch;
			else
				escape_char = true;
		}
		return result;
	}
	
	output parse_next(boost::string_ref gdb_output)
	{
		
	}
}