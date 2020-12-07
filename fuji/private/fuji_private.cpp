#include "fuji_private.hpp"

namespace fuji {
	bool validate_context_settings(const ContextSettings& settings)
	{
		if (settings.width == 0 || settings.height == 0) {
			return false;
		}

#ifdef FUJI_WIN32
		if (settings.platformData.hwnd == NULL) {
			return false;
		}
#endif // FUJI_WIN32

		return true;
	}
} // namespace fuji