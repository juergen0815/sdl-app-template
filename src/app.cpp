/*
 * app.cpp
 *
 *  Created on: 2013-01-15
 *      Author: jurgens
 */

#include "app.h"

#include <SDL/SDL.h>

#include <exception>
#include <stdexcept>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#define THROW( ... ) \
{											\
	char buffer[ 256 ]; 					\
	snprintf( buffer, 256, __VA_ARGS__ ); 	\
	throw std::runtime_error( buffer ); 	\
}

App::App()
{
}


App::~App()
{
}


void App::Init(int argc, char* argv[])
{
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		THROW( "Failed to initialize SDL video system! SDL Error: %s\n", SDL_GetError() );
	}
	atexit( SDL_Quit );

    // TODO: read capabilities and render accordingly
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32);
    int stencilSize(32); // overkill - use smaller stencil - testing only
//    SDL_GL_GetAttribute( SDL_GL_STENCIL_SIZE, &stencilSize );
//    if ( stencilSize > 8 ) stencilSize = 8;
    if ( SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, stencilSize ) == -1 ) {
        THROW("Error setting stencil size to %d! SDL Error:  %s\n", stencilSize, SDL_GetError());
    }
    // enable multi sampling
    if ( SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 ) == - 1 ) {
    	THROW("Error enabling multi sampling! SDL Error: %s\n", SDL_GetError());
    }
    int numSampleBuffers(32); // test what's the max AA. test 32xMSAA
//    SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &numSampleBuffers );
#ifdef MSAA_4X
    if ( numSampleBuffers > 4 ) numSampleBuffers = 4;
#endif
    if ( SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, numSampleBuffers ) == - 1 ) { // 4x MSAA
    	THROW("Error setting number (%d) of AA buffer! SDL Error: %s\n", numSampleBuffers, SDL_GetError());
    }
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	int vsync = 1;  // 0 = novsync
	SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, vsync );

    SDL_WM_SetCaption( "SDLFW", NULL );
    SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL );

    //  SDL_WM_SetIcon( pei::SDL::SurfaceRef( pei::LogoBrush ), NULL );
    //	SDL_ShowCursor(SDL_DISABLE);
    //	SDL_EnableUNICODE(1);
}


int App::Run()
{
	int r(0);

	// somebody must attach a worker
	BOOST_ASSERT( m_Worker );

	SDL_Surface *screen = nullptr;
	screen = SDL_SetVideoMode(640, 480, 32, SDL_OPENGL );
	if ( screen == NULL ) {
		THROW("Unable to set 640x480 video! SDL Error: %s\n", SDL_GetError());
	}
	// Run our worker thread
	boost::thread worker( boost::bind( &Worker::Run, m_Worker ) );

	bool running(true);
    SDL_Event event;
    do {
    	int eventsPending(0);
    	SDL_WaitEvent(&event);
    	do {
			switch (event.type) {
				case SDL_KEYDOWN:
					printf("The %s key was pressed!\n",
						   SDL_GetKeyName(event.key.keysym.sym));
					break;
				case SDL_QUIT:
					running = false;
					break;
			}
			if ( running ) {
				eventsPending = SDL_PollEvent(&event);
			}
    	} while ( eventsPending > 0 && running);
    } while ( running );

	m_Worker->Terminate();
	worker.join();

	return r;
}


