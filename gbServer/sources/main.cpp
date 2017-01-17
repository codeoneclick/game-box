#if defined(__WIN32__)

#include "application_delegate_win32.h"
#include "game_loop.h"

int main(int argc, const char* argv[])
{
	ns::application_delegate_win32* application_delegate = new ns::application_delegate_win32();
	gb::execute_runloop();
	delete application_delegate;
	return 0;
}

#endif