#pragma once

#include "../fuji.hpp"
#include <type_traits>

namespace fuji {
	bool validate_context_settings(const ContextSettings& settings);

	template<typename T>
	constexpr auto to_underlying_type(T enum_value) -> decltype(std::underlying_type_t<T>(enum_value)) 
	{
		return static_cast<std::underlying_type_t<T>>(enum_value);
	}
}