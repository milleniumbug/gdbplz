#ifndef GDBPLZ_EXPRESSION_HPP_F452FB1B00034F0B81FC533F15A5B280
#define GDBPLZ_EXPRESSION_HPP_F452FB1B00034F0B81FC533F15A5B280

#include <memory>

namespace gdbplz
{
	class expression
	{
		virtual ~expression() {}
		virtual std::unique_ptr<expression> freeze() = 0;
	};
}

#endif