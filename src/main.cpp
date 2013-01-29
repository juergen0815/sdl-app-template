
#include "app.h"
#include "err.h"

#include <exception>
#include <iostream>


extern "C" int SDL_main( int argc, char* argv[] )
{
	try {
		App app;
		app.Init( argc, argv );
		return app.Run();
	}
	catch ( std::exception &ex ) {
		ShowError( ex.what(), "Application failed with Exception!" );
	}
	return -1;
}
