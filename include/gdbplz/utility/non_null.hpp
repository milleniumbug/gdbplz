#ifndef WIERTLO_NON_NULL_1042F383671E469B896F0D46AB3D476E
#define WIERTLO_NON_NULL_1042F383671E469B896F0D46AB3D476E

#include <stdexpect>
#include <functional>

namespace wiertlo
{
	template<typename T>
	struct is_null
	{
		
	};
	
	template<typename T>
	struct is_null<T*>
	{
		bool operator(T* value)
		{
			return !value;
		}
	};
	
	template<typename T>
	struct is_null<std::function(T)>
	{
		bool operator(const std::function<T>& value)
		{
			return !value;
		}
	}
	
	// TODO: derive from `std::exception`-derived classes
	struct unexpected_null
	{
		
	};
	
	template<typename T, typename Tag>
	class non_null
	{
	private:
		T value;
	public:
		template<typename... Args>
		strong_typedef(Args&&... args) :
			value(std::forward<Args>(args)...)	
		{
			if(is_null<T>()(value))
				throw unexpected_null();
		}
		
		T get() const
		{
			return value;
		}
		
		explicit operator T() const
		{
			return get();
		}
	};
}

#endif