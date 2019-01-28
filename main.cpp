#include <iostream>
#include <SDL2/SDL.h>
#include <complex>

SDL_Window *Window = NULL;
SDL_Renderer *Renderer = NULL;
SDL_Surface *Surface = NULL;
const int ImageWidth = 1000;
const int ImageHeight = 1000;
const int WindowWidth = 1920;
const int WindowHeight = 1080;


void ErrorPrint( const char* _Message )
{
	std::cout << "[ERROR]: " << _Message << std::endl;
}



int SetupSDL()
{

	if ( SDL_Init( SDL_INIT_VIDEO ) != 0)
	{
		ErrorPrint( "Failed to initialise SDL" );
		return 1;
	}

	SDL_CreateWindowAndRenderer( WindowWidth, WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &Window, &Renderer);

	if ( Window == NULL || Renderer == NULL )
	{
		ErrorPrint( "Failed to create an SDL window or renderer" );
		return 1;
	}

	Surface = SDL_GetWindowSurface( Window );

	if ( Surface == NULL )
	{
		ErrorPrint( "Failed to get the windows surface" );
		return 1;
	}


	SDL_SetWindowPosition( Window, 0, 0);

	return 0;
}


int QuitSDL()
{

	SDL_DestroyWindow( Window );

	SDL_DestroyRenderer( Renderer );

	SDL_Quit();

}


void GenerateImage()
{
	for( size_t y = 0; y < WindowHeight; ++y )
	{
		for( size_t x = 0; x < WindowWidth; ++x )
		{

			std::complex<double> c(1.0 * x/WindowHeight * 4.0 - 2.0, 1.0 * y / WindowHeight * 4.0 - 2.0);	
			std::complex<double> z(0.0, 0.0);

			int count = 0;
			while( count < 255)
			{
				z = z * z + c;
				if( abs(z) > 2.0)
					break;
				++count;
			}

			SDL_SetRenderDrawColor( Renderer, count, count, count, 0xff);
			SDL_RenderDrawPoint( Renderer, x, y);

		}	
	}

	SDL_RenderPresent( Renderer );
}


int SaveImage()
{
	const Uint32 format = SDL_PIXELFORMAT_ARGB8888;

	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, ImageWidth, ImageHeight, 32, format);
    SDL_RenderReadPixels(Renderer, NULL, format, surface->pixels, surface->pitch);
    SDL_SaveBMP(surface, "screenshot.bmp");
    SDL_FreeSurface(surface);

	return 0;
}


int main()
{

	if ( SetupSDL() )
	{
		ErrorPrint( "Failed to setup SDL, Exiting" );
		QuitSDL();
		return 1;
	}

	GenerateImage();	
	SaveImage();

	SDL_Delay(10000);

	QuitSDL();

	return 0;

}
