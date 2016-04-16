#ifndef WIERTLO_COMPILER_HINTS_HPP_B0672D8E18FC40DBB6C9BFD852851368
#define WIERTLO_COMPILER_HINTS_HPP_B0672D8E18FC40DBB6C9BFD852851368

#include <cassert>

#ifdef _MSC_VER
#define WIERTLO_ASSUME(...) do { if(!!(__VA_ARGS__)) { assert(false); __assume(0); } } while(0)
#else
#define WIERTLO_ASSUME(...) do { if(!!(__VA_ARGS__)) { assert(false); __builtin_unreachable(); } } while(0)
#endif

#ifdef _MSC_VER
#define WIERTLO_UNREACHABLE do { assert(false); __assume(0); } while(0)
#else
#define WIERTLO_UNREACHABLE do { assert(false); __builtin_unreachable(); } while(0)
#endif

#ifdef _MSC_VER
#define WIERTLO_LIKELY(...) __VA_ARGS__
#elif defined(__clang__) || defined(__GNUC__)
#define WIERTLO_LIKELY(...) __builtin_expect(!!(__VA_ARGS__), 1)
#else
#define WIERTLO_LIKELY(...) __VA_ARGS__
#endif

#ifdef _MSC_VER
#define WIERTLO_UNLIKELY(...) __VA_ARGS__
#elif defined(__clang__) || defined(__GNUC__)
#define WIERTLO_UNLIKELY(...) __builtin_expect(!!(__VA_ARGS__), 0)
#else
#define WIERTLO_UNLIKELY(...) __VA_ARGS__
#endif

#ifdef _MSC_VER
#define WIERTLO_FORCE_INLINE __force_inline
#elif defined(__clang__) || defined(__GNUC__)
#define WIERTLO_FORCE_INLINE __attribute__((always_inline))
#else
#define WIERTLO_FORCE_INLINE // TODO
#endif

#define WIERLO_UNUSED_ARGUMENT(x) static_cast<void>(x)

#ifdef _WIN32
#define WIERTLO_DLLEXPORT __declspec(dllexport)
#else
#define WIERTLO_DLLEXPORT 
#endif

#ifdef _WIN32
#define WIERTLO_DLLIMPORT __declspec(dllimport)
#else
#define WIERTLO_DLLIMPORT
#endif

#ifdef _MSC_VER
#define WIERTLO_DEBUG_BREAK __debugbreak()
#else
#define WIERTLO_DEBUG_BREAK // TODO
#endif

#ifdef _MSC_VER
#define WIERTLO_DEPRECATED(...) __declspec(deprecated) __VA_ARGS__
#elif defined(__clang__) || defined(__GNUC__)
#define WIERTLO_DEPRECATED(...) __VA_ARGS__ __attribute__ ((deprecated))
#endif

#endif