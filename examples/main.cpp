
#define FUJI_IMPLEMENTATION
#include "../fuji/fuji.hpp"

int main()
{
	using namespace fuji;

	ContextSettings ctx_settings;
	ctx_settings.swapchainImageCount = 2;
	Context ctx = create_context(ctx_settings);

	// #TODO

	destroy_context(ctx);

	return 0;
}