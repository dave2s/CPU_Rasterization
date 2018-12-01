#define SDL_MAIN_HANDLED

#include <iostream>
#include <iomanip>//to set FP precision in std::stream<< outputs
#include <string>
#include <chrono>
#include "SDL.h"
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include "Mesh.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "main.h"

#define WIDTH 640
#define HEIGHT 480
#define PROFILE
#define OUT

bool quit = false;
enum ERROR_CODES {E_OK, E_FAIL}; // ???

std::vector<Mesh*> mesh_list;
//std::vector<Light*> light_list;

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

void MovePolling(SDL_Event &event, Camera &camera) {
	//SDL_PollEvent(&event);
	//bool left_click;

	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_w:
			camera.camera_position[2] -= 0.1f;
			break;
		case SDLK_s:
			camera.camera_position[2] += 0.1f;
			break;
		case SDLK_a:
			camera.camera_position[0] -= 0.1f;
			break;
		case SDLK_d:
			camera.camera_position[0] += 0.1f;
			break;
		case SDLK_LEFT:
			//if (!light_list.empty())
				//((RT_PointLight*)light_list.at(0))->position[0] -= 1.0f;
			break;
		case SDLK_RIGHT:
			//if (!light_list.empty())
				//((RT_PointLight*)light_list.at(0))->position[0] += 1.0f;
			break;
		case SDLK_UP:
			//if (!light_list.empty())
				//((RT_PointLight*)light_list.at(0))->position[1] += 1.0f;
			break;
		case SDLK_DOWN:
			//if (!light_list.empty())
			//	((RT_PointLight*)light_list.at(0))->position[1] -= 1.0f;
			break;
		case SDLK_c:
			camera.camera_position[1] -= 0.1f;
			break;
		case SDLK_r:
			camera.camera_position = { 0.f ,0.f,0.0f };
			break;
		case SDLK_SPACE:
			camera.camera_position[1] += 0.1f;
			break;
		case SDLK_l:
			//((RT_PointLight*)light_list.at(0))->position = glm::vec3(0.f, 0.f, 0.f);
			//light_list[1]->position = calcRandPos(LO0x, LO0y, LO0z, HI0x, HI0y, HI0z);
			break;
		case SDLK_g:
		/*	if (global_light_on) {
				for (auto light = light_list.begin(); light != light_list.end();) {
					if ((*light)->getType() == RT_Light::distant) {
						light_list_off.push_back(*light);
						light_list.erase(light);
					}
					else light++;
				}
			}
			else {
				for (auto light = light_list_off.begin(); light != light_list_off.end();) {
					if ((*light)->getType() == RT_Light::distant) {
						light_list.push_back(*light);
						light_list_off.erase(light);
					}
					else light++;
				}
			}
			global_light_on ^= 1;
			updateSkyColor();*/
			break;
		}
		camera.Update(glm::vec3(0.f, 0.f, -1.f));

		//return;
	}

	if (event.type == SDL_QUIT)
		quit = 1;
	if (event.type == SDL_KEYUP) {
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
			quit = 1;
		}
	}

	/*float x1 ;float x2;
	float y1 ;float y2;
	//const Uint8* keystate = SDL_GetKeyboardState(NULL);
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		left_click = true;
		x1 = event.motion.x;
		y1 = event.motion.y;
	}
	if (event.type == SDL_MOUSEMOTION) {
		//if (event.key.keysym.sym==SDLK_LCTRL) {
		if (left_click) {
			x2 = event.motion.x;
			y2 = event.motion.y;
		}
			//std::cout << "moving mouse x: "<< event.motion.xrel<<" y: "<<event.motion.yrel <<std::endl
		//}
	}
	if (event.type == SDL_MOUSEBUTTONUP) {
		left_click = false;
		camera.camera_position[0] += 0.0001*((x1 - x2));	//	camera.camera_position[0] += 0.001*event.motion.xrel;
		camera.camera_position[1] += 0.0001*((y1 - y2));	//	camera.camera_position[1] += 0.001*event.motion.yrel;
		camera.Update(glm::vec3(0.f, 0.f, 0.1f));
	}*/
}

void createZBuffer(std::vector<float> &zbuffer)
{
	zbuffer = std::vector<float>(HEIGHT*WIDTH);
	for (uint32_t i = 0; i < HEIGHT*WIDTH; ++i) {
		zbuffer[i] = inf;
	}
}

void perspectiveDivide(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2)
{
	v0.x = (CAM_NEAR_PLANE * v0.x) / (-v0.z);
	v0.y = (CAM_NEAR_PLANE * v0.y) / (-v0.z);
	v0.z *= -1;

	v1.x = (CAM_NEAR_PLANE * v1.x) / (-v1.z);
	v1.y = (CAM_NEAR_PLANE * v1.y) / (-v1.z);
	v1.z *= -1;

	v2.x = (CAM_NEAR_PLANE * v2.x) / (-v2.z);
	v2.y = (CAM_NEAR_PLANE * v2.y) / (-v2.z);
	v2.z *= -1;
}
//
void convertToNDC(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2,Camera &cam)
{
	v0.x = v0.x * cam.scale * cam.aspect_ratio;
	v0.y = v0.y * cam.scale;

	v1.x = v1.x * cam.scale * cam.aspect_ratio;
	v1.y = v1.y * cam.scale;

	v2.x = v2.x * cam.scale * cam.aspect_ratio;
	v2.y = v2.y * cam.scale;
}
//(0 < v.x < width);(height > v.y > 0)
void convertToRasterSpace(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2) {
	v0.x = (v0.x + 1) / 2 * WIDTH;
	v0.y = (1 - v0.y) / 2 * HEIGHT;

	v1.x = (v1.x + 1) / 2 * WIDTH;
	v1.y = (1 - v1.y) / 2 * HEIGHT;

	v2.x = (v2.x + 1) / 2 * WIDTH;
	v2.y = (1 - v2.y) / 2 * HEIGHT;
}

int main(int argc, char* argv[]) {
	quit = false;
	SDL_Window* main_window; SDL_Renderer* renderer;
	SDL_Surface* frame_buffer; SDL_Texture* texture;
	SDL_Event event;

	std::vector<float> zbuffer;

	main_window = SDL_CreateWindow("Rasterizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (main_window == NULL) {
		std::cerr << "SDL2 Main window creation failed.";
		return E_FAIL;
	}
	renderer = SDL_CreateRenderer(main_window, -1, 0);
	frame_buffer = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, (Uint32)0xff000000, (Uint32)0x00ff0000, (Uint32)0x0000ff00, (Uint32)0x000000ff);
	texture = SDL_CreateTextureFromSurface(renderer, frame_buffer);
	
	Camera camera = Camera(glm::vec3(0.f, 1.f, 2.8f), glm::vec3(0.f, 0.f, -1.f), 30.f, (float)WIDTH / (float)HEIGHT);

	std::string model_path = "D:\\Users\\David\\Documents\\2MIT\\Graphics\\OpenGL\\OpenGLApps\\Raytracer\\Raytracer\\Models\\CornellBox\\CornellBox-Mirror.obj";
//	std::string model_path = "/home/kamil/CPU_Rasterization/example/CornellBox-Mirror.obj";

	ModelLoader::loadScene(model_path, mesh_list);

	uint16_t triangle_count = 0;
	glm::u8vec3 sky_color = glm::u8vec3(150, 150, 200);
	while (!quit)
	{

#ifdef PROFILE
		auto start = std::chrono::high_resolution_clock::now();
#endif
		createZBuffer(OUT zbuffer);

		for (auto mesh = mesh_list.begin(); mesh != mesh_list.end(); ++mesh) {

			triangle_count = (*mesh)->getTriangleCount();
			for (uint32_t i = 0; i < triangle_count; ++i) {

				Mesh::Vertex* triangle = (*mesh)->getTriangle(i);
				glm::vec3 v0 = triangle[0].position;
				glm::vec3 v1 = triangle[1].position;
				glm::vec3 v2 = triangle[2].position;
				delete triangle;

				//move vertices to camera space
				v0 = (camera.view_matrix) * glm::vec4(v0, 1.f);
				v1 = (camera.view_matrix) * glm::vec4(v1, 1.f);
				v2 = (camera.view_matrix) * glm::vec4(v2, 1.f);

				//move vertices to screen space
				perspectiveDivide(OUT v0, OUT v1, OUT v2);

				convertToNDC(OUT v0, OUT v1, OUT v2,camera);
				convertToRasterSpace(OUT v0, OUT v1, OUT v2);
				///v0-v2 je je treba prepocitat perspektivou a prevest na integer (horni 4 bity lze pouzit .x a.y na subpixel presnost)slo priradit vrcholy pixelum

				///todo - Je zavhodno prochazet pouze pixely v okoli trojuhelnika - je treba ho obalit do obdelnika rovnobezneho s osami (axis aligned 2d bounding box)
				float t; float u; float v;
				
				///predpocet konstant pro edgestep funkci - optimalizace opakovaneho volani edgefunkce 
				/*
				float edge0; float edge_y_step; float edge_x_step;
				edge0 = Mesh::edgeFunction(v0, v1, glm::vec2(0, 0));
				edge_y_step = (v1.x-v0.x);
				edge_x_step = (v1.y-v0.y);
*/
				for (int y = 0; y < HEIGHT; ++y) {
					//SDL_PollEvent(&event); //so the app does not stop responding while drawing
					while (SDL_PollEvent(&event)) {
						MovePolling(event, camera);
					}
					OUT float z; OUT bool is_pixel_in_triangle;
					for (int x = 0; x < WIDTH; ++x) {
						setRGBAPixel(x, y, frame_buffer, sky_color);

						Mesh::isPixelInTriangle(is_pixel_in_triangle,v0, v1, v2, glm::vec2(x, y), u, v, z);

						if (is_pixel_in_triangle)
						{
							if (z < zbuffer[x + y * HEIGHT]) {
								zbuffer[x+y*HEIGHT] = z;
								t = 1 - u - v; // barrycentric
								setRGBAPixel(x, y, frame_buffer, (*mesh)->material.ambient_color);
							}
						}
						//setRGBAPixel(x, y, frame_buffer, glm::u8vec3(150, 150, 200));

					}//end row loop
				}//end image loop/for each pixel loop
			}//end triangle loop for a given mesh
		}//end mesh loop

		///Draw
		SDL_LockSurface(frame_buffer);
		SDL_UpdateTexture(texture, NULL, frame_buffer->pixels, WIDTH * sizeof(Uint32));//
		SDL_UnlockSurface(frame_buffer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

#ifdef PROFILE
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		std::cout << "Frametime: " << std::fixed << std::setprecision(2) << (float)microseconds / 1000000.f << "s";
		std::cout << "; " << std::fixed << std::setprecision(2) << 1000000.f / (float)microseconds << "	fps" << std::endl;
		std::flush(std::cout);
#endif

	}

	return E_OK;

}
