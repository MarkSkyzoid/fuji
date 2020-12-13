
#define FUJI_IMPLEMENTATION
#include "../fuji/fuji.hpp"

#include <SDL.h>
#include <SDL_syswm.h>

int main(int argv, char** args)
{
	const uint32_t WIDTH = 1700;
	const uint32_t HEIGHT = 900;

	// We initialize SDL and create a window with it.
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
	SDL_Window* window = SDL_CreateWindow("fuji example", SDL_WINDOWPOS_UNDEFINED,
													  SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, window_flags);
	if (window == nullptr) {
		return 1;
	}

	SDL_SysWMinfo sys_wm_info;
	SDL_VERSION(&sys_wm_info.version); // Call this before calling SDL_GetWindowWMInfo
	SDL_GetWindowWMInfo(window, &sys_wm_info);

	fuji::ContextSettings ctx_settings {};
	ctx_settings.width = WIDTH;
	ctx_settings.height = HEIGHT;
	ctx_settings.platform_data.hinstance = sys_wm_info.info.win.hinstance;
	ctx_settings.platform_data.hwnd = sys_wm_info.info.win.window;
#if defined(NDEBUG)
	ctx_settings.enable_validation = false;
#else // NDEBUG
	ctx_settings.enable_validation = true;
#endif // NDEBUG

	fuji::Context fuji_context;
	if (!fuji::create_context(ctx_settings, fuji_context)) {
		return 1;
	}

	SDL_Event sdl_event;
	bool do_quit = false;
	// main loop
	while (!do_quit) {
		// Handle events on queue
		while (SDL_PollEvent(&sdl_event) != 0) {
			// close the window when user alt-f4s or clicks the X button
			if (sdl_event.type == SDL_QUIT)
				do_quit = true;
		}

		// #TODO
		// draw();
	}

	fuji::destroy_context(fuji_context);

	SDL_DestroyWindow(window);

	return 0;
}