#ifndef GDBPLZ_UTILITY_COUNTER_HPP_AF632E88829A4141A7021C9C955D9FA7
#define GDBPLZ_UTILITY_COUNTER_HPP_AF632E88829A4141A7021C9C955D9FA7

namespace gdbplz
{
	namespace utility
	{
		template<typename T>
		class Counter
		{
		private:
			T c;
		public:
			Counter() { c = T(); }
			T operator()() { return ++c; }
		};
	}
}

#endif