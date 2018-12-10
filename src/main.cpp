#define SDL_MAIN_HANDLED
#include "Defines.h"
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

bool quit = false;
enum ERROR_CODES {E_OK, E_FAIL}; // ???

std::vector<Mesh*> mesh_list;
//std::vector<Light*> light_list;
std::vector<Mesh::Texture> loaded_textures;

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
			camera.position[2] -= 0.5f;
			break;
		case SDLK_s:
			camera.position[2] += 0.5f;
			break;
		case SDLK_a:
			camera.position[0] -= 0.5f;
			break;
		case SDLK_d:
			camera.position[0] += 0.5f;
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
			camera.position[1] -= 0.5f;
			break;
		case SDLK_r:
			camera.position = { 0.f ,0.f,0.0f };
			break;
		case SDLK_SPACE:
			camera.position[1] += 0.5f;
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

void resetZBuffer(std::vector<float> &zbuffer,float far_plane)
{
	for (uint32_t i = 0; i < HEIGHT*WIDTH; ++i) {
		zbuffer[i] = far_plane;
	}
}

void perspectiveDivide(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2)
{
	v0.x = (CAM_NEAR_PLANE * v0.x) / (-v0.z);
	v0.y = (CAM_NEAR_PLANE * v0.y) / (-v0.z);
	v0.z *= -1.f;

	v1.x = (CAM_NEAR_PLANE * v1.x) / (-v1.z);
	v1.y = (CAM_NEAR_PLANE * v1.y) / (-v1.z);
	v1.z *= -1.f;

	v2.x = (CAM_NEAR_PLANE * v2.x) / (-v2.z);
	v2.y = (CAM_NEAR_PLANE * v2.y) / (-v2.z);
	v2.z *= -1.f;
}
//(-1,1)
void convertToNDC(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2,Camera &cam)
{
	v0.x =2*v0.x/0.5 -0.5 / 1.f;
	v0.y = 2*v0.y/0.5;

	v1.x = 2*v1.x/0.5 - 0.5 / 1.f;
	v1.y = 2*v1.y/0.5;

	v2.x = 2*v2.x/0.5 - 0.5 / 13.f;
	v2.y = 2*v2.y/0.5;
}
//(0 < v.x < width);(height > v.y > 0)
void convertToRasterSpace(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2,Camera &cam) {
	v0.x = (((v0.x/(cam.aspect_ratio*cam.scale) + 1) / 2)* WIDTH);// / (cam.aspect_ratio*cam.scale);
	v0.y = (((1 - v0.y/cam.scale) / 2)* HEIGHT);// / cam.scale;

	v1.x = (((v1.x/(cam.aspect_ratio*cam.scale) + 1) /2)* WIDTH);// / (cam.aspect_ratio*cam.scale);
	v1.y = (((1 - v1.y/cam.scale) / 2)* HEIGHT);// / (cam.scale);

	v2.x = (((v2.x/(cam.aspect_ratio*cam.scale) + 1) / 2)* WIDTH);// / (cam.aspect_ratio*cam.scale);
	v2.y = (((1 - v2.y/cam.scale) / 2 )* HEIGHT);// / (cam.scale);
}

void clearFrameBuffer(SDL_Surface* frame_buffer) {
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			setRGBAPixel(x, y, frame_buffer, glm::u8vec3(0));
		}
	}
}
//Returns true if all projected vertices lay outside of view frustum
bool frustumCulling(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2,Camera &cam) {
	//left and right
	if (v0.x < 0 && v1.x < 0 && v2.x < 0) return true;
	if (v0.x >WIDTH && v1.x > WIDTH && v2.x > WIDTH) return true;
	//top and bottom
	if (v0.y < 0 && v1.y < 0 && v2.y < 0) return true;
	if (v0.y > HEIGHT && v1.y > HEIGHT && v2.y > HEIGHT) return true;
	//near and far
	if (v0.z < (CAM_NEAR_PLANE) && v1.z < (CAM_NEAR_PLANE) && v2.z < (CAM_NEAR_PLANE)) return true;
	if (v0.z > (CAM_FAR_PLANE) && v1.z > (CAM_FAR_PLANE) && v2.z > (CAM_FAR_PLANE)) return true;

	return false;
}

bool backfaceCulling(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
#ifndef SMOOTH_SHADING
	glm::f32vec3 N = calcTriangleUnNormal(v0, v1, v2);
#else
	//For backface culling we use NOT interpolated normal which is same for the whole triangle
	glm::f32vec3 N = Mesh::calcTriangleUnNormal(v0, v1, v2);
#endif
	if (glm::dot(N, (glm::vec3(v0) - glm::vec3(0))) > 0) {
		return true;
	}
	return false;
}


int main(int argc, char* argv[]) {
	quit = false;
	SDL_Window* main_window; SDL_Renderer* renderer;
	SDL_Surface* frame_buffer; SDL_Texture* texture;
	SDL_Event event;

	std::vector<float> zbuffer;
	zbuffer = std::vector<float>(HEIGHT*WIDTH);

	main_window = SDL_CreateWindow("Rasterizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (main_window == NULL) {
		std::cerr << "SDL2 Main window creation failed.";
		return E_FAIL;
	}
	renderer = SDL_CreateRenderer(main_window, -1, 0);
	frame_buffer = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, (Uint32)0xff000000, (Uint32)0x00ff0000, (Uint32)0x0000ff00, (Uint32)0x000000ff);
	texture = SDL_CreateTextureFromSurface(renderer, frame_buffer);
	
	Camera camera = Camera(glm::vec3(0.f, 1.f, 6.f), glm::vec3(0.f, 0.f, -1.f), 30.f, (float)WIDTH / (float)HEIGHT);

	char current_dir[FILENAME_MAX];
	GetCurrentDir(current_dir, FILENAME_MAX);
	//std::string model_path = "example/sponza/sponza.obj";
	//std::string model_path = std::string(current_dir).append("/example/CornellBox/CornellBox-Original.obj");
	std::string model_path = std::string(current_dir).append("/example/f16/f16.obj");
	//std::string model_path = std::string(current_dir).append("/example/suzanne/suzanne.obj");
	//std::string model_path = std::string(current_dir).append("/example/cruiser/cruiser.obj");
//	std::string model_path = "/home/kamil/CPU_Rasterization/example/CornellBox-Original.obj";

	ModelLoader::loadScene(model_path, mesh_list/*, loaded_textures*/);

	uint16_t triangle_count = 0;
	glm::u8vec3 sky_color = glm::u8vec3(150, 150, 200);
	while (!quit)
	{

#ifdef PROFILE
		auto start = std::chrono::high_resolution_clock::now();
#endif
		resetZBuffer(OUT zbuffer,CAM_FAR_PLANE);
		clearFrameBuffer(frame_buffer);
		while (SDL_PollEvent(&event)) {
			MovePolling(event, camera);
		}
		for (auto mesh = mesh_list.begin(); mesh != mesh_list.end(); ++mesh) {
			triangle_count = (*mesh)->getTriangleCount();
			for (uint32_t i = 0; i < triangle_count; ++i) {
				Mesh::Vertex* triangle = (*mesh)->getTriangle(i);
				Mesh::Vertex v0 = triangle[0];
				Mesh::Vertex v1 = triangle[1];
				Mesh::Vertex v2 = triangle[2];
				delete triangle;

				//move vertices to camera space
				v0.position = (camera.view_matrix) * glm::vec4(v0.position, 1.f);
				v1.position = (camera.view_matrix) * glm::vec4(v1.position, 1.f);
				v2.position = (camera.view_matrix) * glm::vec4(v2.position, 1.f);

				glm::vec3 v0cam = v0.position;
				glm::vec3 v1cam = v1.position;
				glm::vec3 v2cam = v2.position;

				//move vertices to screen space
				perspectiveDivide(OUT v0.position, OUT v1.position, OUT v2.position);

				//convertToNDC(OUT v0, OUT v1, OUT v2, camera);
				convertToRasterSpace(OUT v0.position, OUT v1.position, OUT v2.position, camera);
				///v0-v2 je je treba prepocitat perspektivou a prevest na integer (horni 4 bity lze pouzit .x a.y na subpixel presnost)slo priradit vrcholy pixelum

				///very naive and not robust
				if (frustumCulling(v0.position, v1.position, v2.position,camera)) continue;

#ifdef BACKFACE_CULLING
				if (backfaceCulling(v0cam, v1cam, v2cam)) {
					continue;
				}
#endif
				glm::uvec2 bounding_box[2];
				Mesh::computeTriangleBoundingBox(bounding_box,v0.position, v1.position, v2.position);
				
				///predpocet konstant pro edgestep funkci - optimalizace opakovaneho volani edgefunkce 
				//2*triangle area - to normalize barycentric later
				float parallelogram_area = Mesh::edgeFunction(v0.position, v1.position, v2.position);

				///Barycentric optimization - precompute these so we can just add steps within the pixel loop instead of cumputing these many times
				//precompute barycentric coords for bounding box corner - unnormalized by 2*triangle_area
				glm::vec2 pixel = glm::vec2(bounding_box[0].x+0.5f, bounding_box[0].y+0.5f);
				std::vector<float> tuv_row;
				tuv_row.push_back(Mesh::edgeFunction(v1.position,v2.position,pixel));
				tuv_row.push_back(Mesh::edgeFunction(v2.position,v0.position,pixel));
				tuv_row.push_back(Mesh::edgeFunction(v0.position,v1.position,pixel));
				//precompute edge deltas for y and x (zmena x a y mezi vrcholy)
				std::vector<float> edges_y; std::vector<float> edges_x;
				edges_y.push_back(v2.position.y - v1.position.y);
				edges_y.push_back(v0.position.y - v2.position.y);
				edges_y.push_back(v1.position.y - v0.position.y);

				edges_x.push_back(v1.position.x - v2.position.x);
				edges_x.push_back(v2.position.x - v0.position.x);
				edges_x.push_back(v0.position.x - v1.position.x);

				//barycentric coords (not normalized, for pixel overlap test)
				std::vector<float> tuv(3);
				//2. and 3. barycentric coord (normalized, for interpolation of vertex attributes)
				glm::vec2 uv;
				//pixel color sent to buffer
				glm::f32vec3 pixel_color;
				///PIXEL LOOP y, main scan-line loop
				for (uint16_t y = bounding_box[0].y; y <= bounding_box[1].y; ++y)
				{
					OUT float z; 
					//unnormalized barycentric
					
					tuv[0]=tuv_row[0];
					tuv[1]=tuv_row[1];
					tuv[2]=tuv_row[2];

					///PIXEL LOOP x
					for(uint16_t x = bounding_box[0].x;x <= bounding_box[1].x; ++x)
					{
						//sample center of the pixel...for antialiasing loop "pixel loop x" over more samples
						//pixel = glm::vec2(x + 0.5, y + 0.5);
						if(Mesh::isPixelInTriangle(tuv,v0.position,v1.position,v2.position))
						{
							uv.x = tuv[1] / parallelogram_area;
							uv.y = tuv[2] / parallelogram_area;
							//pixel depth in camera space
							z = 1 / ((1-uv.x-uv.y)/ v0.position.z + uv.x / v1.position.z + uv.y / v2.position.z);

							if (z < zbuffer[x + y*HEIGHT] /* && z > (CAM_NEAR_PLANE)*/) {
								zbuffer[x + y*HEIGHT] = z;
								pixel_color = 0.9f*(*mesh)->material.diffuse_color+AMBIENT_LIGHT*(*mesh)->material.ambient_color;
								glm::vec3 N;
								if (!(*mesh)->textures.empty()) {
									// Calculate shading and texturing
									// calcFragmentProperties
									 glm::vec2 tex_coords;
									Mesh::Texture texture;
									for (auto tex_itr = (*mesh)->textures.begin(); tex_itr != (*mesh)->textures.end();++tex_itr) {
										if ((*tex_itr).type == "texture_diffuse") {
											texture = (*tex_itr);
										}
									}
									Mesh::calcFragmentProperties(v0, v1, v2,v0cam,v1cam,v2cam, uv,z, texture.height, texture.width, OUT N, OUT tex_coords);

									float fragment_x = z * ((v0.position.x / -v0.position.z) * (1-uv.x-uv.y) + (v1.position.x / -v1.position.z) * uv.x + (v2.position.x / -v2.position.z) * uv.y);
									float fragment_y = z * ((v0.position.y / -v0.position.z) * (1-uv.x-uv.y) + (v1.position.y / -v1.position.z) * uv.x + (v2.position.y / -v2.position.z) * uv.y);

									glm::vec3 fragment_camera_space_position = glm::vec3(fragment_x, fragment_y,-z);
									glm::vec3 view_direction = glm::normalize(-fragment_camera_space_position);

									float angle_of_incidence = std::max(0.f, glm::dot(N,view_direction));

									//clamp texture coords..cant just subtract 1 because coords can be less than 1 and the result would be negative
									uint32_t texel_index = 3*((int)tex_coords.x + texture.width*(int)tex_coords.y);
									//if (texel_index <= 3*((texture.width)*(texture.height))) {
									glm::u8vec3 rgb = glm::u8vec3(	texture.data[0 + texel_index],
																	texture.data[1 + texel_index],
																	texture.data[2 + texel_index]
									);
									//  Let the final color be C = I_view*M + I_ambient*M, where 
									//	Lview is view (cosine) angle attenuation intensity and 
									//	Lambient is ambient light intensity
									//	M is material color
									pixel_color = glm::clamp(angle_of_incidence*(U8vec2F32vec(rgb)) + AMBIENT_LIGHT*(U8vec2F32vec(rgb)),0.f,1.f);
									setRGBAPixel(x, y, frame_buffer, F32vec2U8vec(pixel_color));
									/*}
									else {
										setRGBAPixel(x, y, frame_buffer, glm::u8vec3(180,120,180));
									}*/
								}
								else {
									Mesh::calcFragmentProperties(v0, v1, v2, v0cam, v1cam, v2cam, uv, z, OUT N);
									//interpolate point in camera space
									float fragment_x = z*((v0.position.x / -v0.position.z) * (1 - uv.x - uv.y) + (v1.position.x / -v1.position.z) * uv.x + (v2.position.x / -v2.position.z) * uv.y);
									float fragment_y = z*((v0.position.y / -v0.position.z) * (1 - uv.x - uv.y) + (v1.position.y / -v1.position.z) * uv.x + (v2.position.y / -v2.position.z) * uv.y);

									glm::vec3 fragment_camera_space_position = glm::vec3(fragment_x, fragment_y, -z);
									glm::vec3 view_direction = glm::normalize(-fragment_camera_space_position);

									float angle_of_incidence = std::max(0.f, glm::dot(N, view_direction));

									pixel_color = glm::clamp(angle_of_incidence*(*mesh)->material.diffuse_color + AMBIENT_LIGHT * (*mesh)->material.ambient_color,0.f,1.f);

									setRGBAPixel(x, y, frame_buffer, F32vec2U8vec(pixel_color));
								}
							}
						}
						//x step
						tuv[0] += edges_y[0];
						tuv[1] += edges_y[1];
						tuv[2] += edges_y[2];
					}//end PIXEL row loop
					//y step
					tuv_row[0] += edges_x[0];
					tuv_row[1] += edges_x[1];
					tuv_row[2] += edges_x[2];
				}//end line loop
			}//end triangle loop for a given mesh
		}//end mesh loop

		///Draw
		//SDL_LockSurface(frame_buffer);
		SDL_UpdateTexture(texture, NULL, frame_buffer->pixels, WIDTH * sizeof(Uint32));//
		//SDL_UnlockSurface(frame_buffer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

#ifdef PROFILE
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		std::cout << "Frametime: " << std::fixed << std::setprecision(2) << (float)microseconds / 1000000.f << "s";
		std::cout << "; " << std::fixed << std::setprecision(2) << 1000000.f / (float)microseconds << " fps" << std::endl;
		std::flush(std::cout);
#endif

	}

	return E_OK;

}