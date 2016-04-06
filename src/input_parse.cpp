#include <type_traits>
#include <stdexcept>
#include "../include/gdbplz/utility/string.hpp"
#include "../include/gdbplz/input_parse.hpp"

namespace gdbplz
{
	struct bad_parse : std::invalid_argument
	{
		using std::invalid_argument::invalid_argument;
	};
	
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