#include <iostream>
#include <SDL2/SDL.h>
#include <complex>
#include <mutex>
#include <vector>
#include <thread>
#include <math.h>

SDL_Window *Window = NULL;
SDL_Renderer *Renderer = NULL;
SDL_Surface *Surface = NULL;
SDL_Surface *HighResSurface = NULL;
const int ImageWidth = 1920;
const int ImageHeight = 1080;
const int WindowWidth = 1920;
const int WindowHeight = 1080;
const int MaxThreads = std::thread::hardware_concurrency();
std::vector<std::thread*> Threads;
std::mutex Lock;


void ErrorPrint( const char* _Message )
{
	std::cout << "[ERROR]: " << _Message << std::endl;
}

void CreateThreads()
{
	for( size_t i = 0; i < MaxThreads; ++i )
	{
		Threads.push_back( new std::thread );
	}
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


	HighResSurface = SDL_CreateRGBSurface(NULL, ImageWidth, ImageHeight,NULL,NULL,NULL,NULL,NULL);


	SDL_SetWindowPosition( Window, 0, 0);

	return 0;
}


int QuitSDL()
{

	SDL_DestroyWindow( Window );

	SDL_DestroyRenderer( Renderer );

	SDL_Quit();

}


void GeneratePart( int yFrom, int yTo, int xFrom, int xTo)
{
	std::vector<int> Colours;
	for( size_t y = yFrom; y < yTo; ++y )
	{
		for( size_t x = xFrom; x < xTo; ++x )
		{

			std::complex<double> c(1.0 * x/ImageHeight * 4.0 - 2.0, 1.0 * y / ImageHeight * 4.0 - 2.0);	
			std::complex<double> z(0.0, 0.0);

			int count = 0;
			while( count < 255)
			{
				z = z * z + c;
				if( abs(z) > 2.0)
					break;
				++count;
			}


			for(int i = 0; i < 3; ++i)
			{
				Colours.push_back(count);
			}
			Colours.push_back(255);

		}	
	}

	size_t ColoursSize = (yTo-yFrom)*(xTo-xFrom)*4;
	// Render
	Lock.lock();
	int xPos = xFrom;
	int yPos = yFrom;
	for(size_t i = 0; i < ColoursSize; i += 4)
	{
		SDL_SetRenderDrawColor( Renderer, Colours[i], Colours[i+1], Colours[i+2], Colours[i+3] );
		xPos += 1;
		if(xPos == xTo)
		{
			xPos = xFrom;
			yPos += 1;
		}
		SDL_RenderDrawPoint( Renderer, xPos, yPos);
	}
	Lock.unlock();
}


void GenerateImage()
{
	int PatchWidth = ceil(ImageWidth/(MaxThreads/2));
	int PatchHeight = ceil(ImageHeight/(MaxThreads/2));
	size_t PatchStartX = 0;
	size_t PatchStartY = 0;

	for(auto i : Threads)
	{
		i = new std::thread( &GeneratePart, PatchStartY, PatchStartY + PatchHeight, PatchStartX, PatchStartX + PatchWidth );
		PatchStartX += PatchWidth;
		if( PatchStartX > ImageWidth)
		{
			PatchStartX = 0;
			PatchStartY += PatchHeight;
			if( PatchStartY > ImageHeight )
			{
				break;
			}
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

	//SDL_Delay(10000);

	QuitSDL();

	return 0;

}
