#include <iostream>
#include <iomanip>//to set FP precision in std::stream<< outputs
#include <string>
#include <chrono>
#include "SDL.h"
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "glm\glm.hpp"

#define WIDTH 640
#define HEIGHT 480
#define PROFILE

enum ERROR_CODES {E_OK, E_FAIL}; // ???

/*
*	int x,y - top left origin coords of the drawn image space
*/
void setRGBAPixel(int x, int y, SDL_Surface* rendered_image, glm::u8vec3 rgba) {
	//rendered_image->pixels[x + 640 * y] = 
	unsigned char* pixels = (unsigned char*)rendered_image->pixels;
	pixels[4 * (y*rendered_image->w + x) + 0] = rgba[2];//blue
	pixels[4 * (y*rendered_image->w + x) + 1] = rgba[1];//green
	pixels[4 * (y*rendered_image->w + x) + 2] = rgba[0];//red
	pixels[4 * (y*rendered_image->w + x) + 3] = glm::u8(255);//rgba[3];//alpha
}

int main(int argc, char* argv[]) {
	bool exit = false;
	SDL_Window* main_window; SDL_Renderer* renderer;
	SDL_Surface* frame_buffer; SDL_Texture* texture;
	SDL_Event event;

	main_window = SDL_CreateWindow("Rasterizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (main_window == NULL) {
		std::cerr << "SDL2 Main window creation failed.";
		return E_FAIL;
	}
	renderer = SDL_CreateRenderer(main_window, -1, 0);
	frame_buffer = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, (Uint32)0xff000000, (Uint32)0x00ff0000, (Uint32)0x0000ff00, (Uint32)0x000000ff);
	texture = SDL_CreateTextureFromSurface(renderer, frame_buffer);

	while (!exit)
	{

#ifdef PROFILE
		auto start = std::chrono::high_resolution_clock::now();
#endif

		for (int y = 0; y < HEIGHT; ++y) {
			SDL_PollEvent(&event); //so the app does not stop responding while drawing

			for (int x = 0; x < WIDTH; ++x) {
				setRGBAPixel(x, y, frame_buffer, glm::u8vec3(150, 150, 200));
			}
		}

		///Draw
		SDL_LockSurface(frame_buffer);
		SDL_UpdateTexture(texture, NULL, frame_buffer->pixels, WIDTH * sizeof(Uint32));//
		SDL_UnlockSurface(frame_buffer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

#ifdef PROFILE
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		std::cout << "Frametime: " << std::fixed << std::setprecision(2) << (float)microseconds / 1000000.f << "us";
		std::cout << "; " << std::fixed << std::setprecision(2) << 1000000.f / (float)microseconds << "	fps" << std::endl;
		std::flush(std::cout);
#endif

	}

	return E_OK;

}