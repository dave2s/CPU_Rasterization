#define SDL_MAIN_HANDLED
#include <iostream>
#include <iomanip> //to set FP precision in std::stream<< outputs
#include <string>
#include <algorithm>
#include <chrono>
#include "SDL.h"
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "glm/glm.hpp"
#include "Mesh.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "Light.h"
#include "Defines.h"

const glm::f32vec3 CONST_SKY_COLOR = glm::f32vec3(U2F(160), U2F(217), U2F(255));
const float GLOBAL_LIGHT_INTENSITY = 2.f;

bool g_Quit = false;
std::vector<Mesh*> g_MeshList;
std::vector<Light*> g_LightList;
std::vector<Light*> g_LightListOff;
unsigned char g_DefBuffer [HEIGHT*WIDTH*4];
bool g_GlobalLightOn = true;

/*
*	int x,y - top left origin coords of the drawn image space
*/
inline void SetRGBAPixel(int x, int y, unsigned char* pixels, glm::u8vec3 rgba)
{
	pixels[4 * (y*WIDTH + x) + 0] = rgba[2];//blue
	pixels[4 * (y*WIDTH + x) + 1] = rgba[1];//green
	pixels[4 * (y*WIDTH + x) + 2] = rgba[0];//red
	pixels[4 * (y*WIDTH + x) + 3] = glm::u8(255);//alpha
}

void UpdateSkyColor()
{
	glm::f32vec3 sky = glm::f32vec3(0.f);
	for (auto light = g_LightList.begin(); light != g_LightList.end(); ++light)
	{
		if ((*light)->GetType() == LIGHT_DISTANT)
		{
			sky += glm::clamp(CONST_SKY_COLOR * AMBIENT_LIGHT * 2.f + CONST_SKY_COLOR * (*light)->GetIntensity() / 4.f, 0.f, 1.f);
		}
	}

	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			SetRGBAPixel(x, y, g_DefBuffer, F32vec2U8vec(sky));
		}
	}

}

void MovePolling(SDL_Event& event, Camera& camera)
{
	if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_w:
			camera.ChangePosition(glm::vec3(0.f, 0.f, -MOVSTEP));
			break;
		case SDLK_s:
			camera.ChangePosition(glm::vec3(0.f, 0.f, MOVSTEP));
			break;
		case SDLK_a:
			camera.ChangePosition(glm::vec3(-MOVSTEP, 0.f, 0.f));
			break;
		case SDLK_d:
			camera.ChangePosition(glm::vec3(MOVSTEP, 0.f, 0.f));
			break;
		case SDLK_j:
			camera.ChangeRotation(0, ROTSTEP);
			break;
		case SDLK_l:
			camera.ChangeRotation(0, -ROTSTEP);
			break;
		case SDLK_i:
			camera.ChangeRotation(ROTSTEP, 0);
			break;
		case SDLK_k:
			camera.ChangeRotation(-ROTSTEP, 0);
			break;
		case SDLK_LEFT:
			if (!g_LightList.empty())
			{
				((PointLight*)g_LightList.at(0))->GetPosition()[0] -= 1.0f;
			}
			break;
		case SDLK_RIGHT:
			if (!g_LightList.empty())
			{
				((PointLight*)g_LightList.at(0))->GetPosition()[0] += 1.0f;
			}
			break;
		case SDLK_UP:
			if (!g_LightList.empty())
			{
				((PointLight*)g_LightList.at(0))->GetPosition()[1] += 1.0f;
			}
			break;
		case SDLK_DOWN:
			if (!g_LightList.empty())
			{
				((PointLight*)g_LightList.at(0))->GetPosition()[1] -= 1.0f;
			}
			break;
		case SDLK_c:
			camera.ChangePosition(glm::vec3(0.f, -MOVSTEP, 0.f));
			break;
		case SDLK_r:
			camera.Reset();
			break;
		case SDLK_SPACE:
			camera.ChangePosition(glm::vec3(0.f, MOVSTEP, 0.f));
			break;
		case SDLK_g:
			if (g_GlobalLightOn)
			{
				for (auto light = g_LightList.begin(); light != g_LightList.end();)
				{
					if ((*light)->GetType() == LIGHT_DISTANT)
					{
						g_LightListOff.push_back(*light);
						g_LightList.erase(light);
					}
					else
					{
						light++;
					}
				}
			}
			else
			{
				for (auto light = g_LightListOff.begin(); light != g_LightListOff.end();)
				{
					if ((*light)->GetType() == LIGHT_DISTANT)
					{
						g_LightList.push_back(*light);
						g_LightListOff.erase(light);
					}
					else
					{
						light++;
					}
				}
			}
			g_GlobalLightOn ^= 1;
			UpdateSkyColor();
			break;
		}
	}

	if (event.type == SDL_QUIT)
	{
		g_Quit = 1;
	}
	if (event.type == SDL_KEYUP)
	{
		if (event.key.keysym.sym == SDLK_ESCAPE)
		{
			g_Quit = 1;
		}
	}
}

inline void CreatePointLight(glm::vec3 pos, float intensity, glm::vec3 color)
{
	g_LightList.push_back(new PointLight(pos, intensity, color));
}

inline void CreateGlobalLight(glm::vec3 direction, float intensity, glm::vec3 color)
{
	g_LightList.push_back(new DistantLight(direction, intensity, color));
}

inline void PerspectiveDivide(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2)
{
	v0.z *= -1.f;
	v0.x = (CAM_NEAR_PLANE * v0.x) / v0.z;
	v0.y = (CAM_NEAR_PLANE * v0.y) / v0.z;

	v1.z *= -1.f;
	v1.x = (CAM_NEAR_PLANE * v1.x) / (v1.z);
	v1.y = (CAM_NEAR_PLANE * v1.y) / (v1.z);

	v2.z *= -1.f;
	v2.x = (CAM_NEAR_PLANE * v2.x) / (v2.z);
	v2.y = (CAM_NEAR_PLANE * v2.y) / (v2.z);
}

inline void ConvertToRasterSpace(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, Camera& camera)
{
	v0.x = (((v0.x/(camera.GetAspectRatio()*camera.GetScale()) + 1) / 2)* WIDTH);
	v0.y = (((1 - v0.y/camera.GetScale()) / 2)* HEIGHT);

	v1.x = (((v1.x/(camera.GetAspectRatio()*camera.GetScale()) + 1) /2)* WIDTH);
	v1.y = (((1 - v1.y/camera.GetScale()) / 2)* HEIGHT);

	v2.x = (((v2.x/(camera.GetAspectRatio()*camera.GetScale()) + 1) / 2)* WIDTH);
	v2.y = (((1 - v2.y/camera.GetScale()) / 2 )* HEIGHT);
}

inline void ClearFrameBuffer(SDL_Surface* frameBuffer)
{
	memcpy(frameBuffer->pixels, g_DefBuffer, HEIGHT*WIDTH*4);
}

void ResetZBuffer(std::vector<float> &zbuffer, float far_plane)
{
	for (uint32_t i = 0; i < WIDTH; ++i)
	{
		zbuffer[i] = far_plane;
	}
	for (uint32_t i = 1; i < HEIGHT; ++i)
	{
		memcpy(zbuffer.data()+(WIDTH*i), zbuffer.data(), sizeof(float)*WIDTH);
	}
}

//Returns true if all projected vertices lay outside of view frustum
inline bool FrustumCulling(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, Camera& camera)
{
	return (
	     //left and right
	        (v0.x < 0 && v1.x < 0 && v2.x < 0)
	     || (v0.x > WIDTH && v1.x > WIDTH && v2.x > WIDTH)
     	     //top and bottom
	     || (v0.y < 0 && v1.y < 0 && v2.y < 0)
	     || (v0.y > HEIGHT && v1.y > HEIGHT && v2.y > HEIGHT)
	     //near and far
	     || (v0.z < (-camera.GetPosition().z + CAM_NEAR_PLANE) && v1.z < (-camera.GetPosition().z + CAM_NEAR_PLANE) && v2.z < (-camera.GetPosition().z + CAM_NEAR_PLANE))
	     || (v0.z > (-camera.GetPosition().z + CAM_FAR_PLANE) && v1.z > (-camera.GetPosition().z + CAM_FAR_PLANE) && v2.z > (-camera.GetPosition().z +CAM_FAR_PLANE))
	     );
}

inline bool BackfaceCulling(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
	return (glm::dot(CalcTriangleUnNormal(v0, v1, v2), (glm::vec3(v0) - glm::vec3(0))) > 0);
}

int main(int argc, char* argv[])
{
	SDL_Window* mainWindow;
	SDL_Renderer* renderer;
	SDL_Surface* frameBuffer;
	SDL_Texture* texture;
	SDL_Event event;

	std::vector<float> zbuffer = std::vector<float>(HEIGHT*WIDTH);
	uint32_t triangleCount;

	mainWindow = SDL_CreateWindow("Rasterizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (mainWindow == NULL)
	{
		std::cerr << "SDL2 Main window creation failed.";
		return -1;
	}

	renderer = SDL_CreateRenderer(mainWindow, -1, 0);
	frameBuffer = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, (Uint32)0xff000000, (Uint32)0x00ff0000, (Uint32)0x0000ff00, (Uint32)0x000000ff);
	texture = SDL_CreateTextureFromSurface(renderer, frameBuffer);

	Camera camera = Camera(glm::vec3(0.f, 1.f, 2.8f), 0.f,90.f, 30.f, (float)WIDTH / (float)HEIGHT);
	CreatePointLight(glm::vec3(0.f, 0.5f, 1.f), 400.f, glm::f32vec3(U2F(64), U2F(134), U2F(244)));
	CreatePointLight(glm::vec3(0.f, 0.5f, 1.f), 400.f, glm::f32vec3(U2F(244), U2F(174), U2F(66)));
	CreateGlobalLight(glm::vec3(0.f, 0.f, -1.f), GLOBAL_LIGHT_INTENSITY, glm::f32vec3(U2F(255), U2F(255), U2F(255)));

	char currentDir[FILENAME_MAX];
	GetCurrentDir(currentDir, FILENAME_MAX);
	std::string modelPath = std::string(currentDir).append("/").append(MODEL);

	LoadScene(modelPath, g_MeshList);

	UpdateSkyColor();

	///Main loop
	while (!g_Quit)
	{

#ifdef PROFILE
		auto start = std::chrono::high_resolution_clock::now();
#endif
		ResetZBuffer(zbuffer, CAM_FAR_PLANE);
		ClearFrameBuffer(frameBuffer);
		while (SDL_PollEvent(&event))
		{
			MovePolling(event, camera);
		}

		for (auto meshItr = g_MeshList.begin(); meshItr != g_MeshList.end(); ++meshItr)
		{
			Mesh* mesh = (*meshItr);
			triangleCount = mesh->GetTriangleCount();
			for (uint32_t i = 0; i < triangleCount; ++i)
			{
				Vertex v0 = mesh->GetTrianglePoint(i, 0);
				Vertex v1 = mesh->GetTrianglePoint(i, 1);
				Vertex v2 = mesh->GetTrianglePoint(i, 2);

				//move vertices to camera space
				v0.position = (camera.GetViewMatrix()) * glm::vec4(v0.position, 1.f);
				v1.position = (camera.GetViewMatrix()) * glm::vec4(v1.position, 1.f);
				v2.position = (camera.GetViewMatrix()) * glm::vec4(v2.position, 1.f);

				glm::vec3 v0cam = v0.position;
				glm::vec3 v1cam = v1.position;
				glm::vec3 v2cam = v2.position;

				//move vertices to screen space
				PerspectiveDivide(v0.position, v1.position, v2.position);

				ConvertToRasterSpace(v0.position, v1.position, v2.position, camera);
				///v0-v2 je je treba prepocitat perspektivou a prevest na integer (horni 4 bity lze pouzit .x a.y na subpixel presnost) slo priradit vrcholy pixelum

				/// very naive and not robust
				if (FrustumCulling(v0.position, v1.position, v2.position, camera))
				{
					continue;
				}

#ifdef BACKFACE_CULLING
				if (BackfaceCulling(v0cam, v1cam, v2cam))
				{
					continue;
				}
#endif

#ifdef BOUNDING_BOX
				glm::uvec2 boundingBox[2];
				ComputeTriangleBoundingBox(boundingBox, v0.position, v1.position, v2.position);
#endif			
				// predpocet konstant pro edgestep funkci - optimalizace opakovaneho volani edgefunkce 
				// 2*triangle area - to normalize barycentric later
				float parallelogramArea = EdgeFunction(v0.position, v1.position, glm::vec2(v2.position.x, v2.position.y));

				// barycentric optimization - precompute these so we can just add steps within the pixel loop instead of cumputing these many times
				// precompute barycentric coords for bounding box corner - unnormalized by 2*triangle_area
#ifdef BOUNDING_BOX
				glm::vec2 pixel = glm::vec2(boundingBox[0].x + 0.5f, boundingBox[0].y + 0.5f);
#else
				glm::vec2 pixel = glm::vec2(0 + 0.5f, 0 + 0.5f);
#endif
				std::vector<float> tuvRow;
				std::vector<float> edgesY;
				std::vector<float> edgesX;

				tuvRow.push_back(EdgeFunction(v1.position, v2.position, pixel));
				tuvRow.push_back(EdgeFunction(v2.position, v0.position, pixel));
				tuvRow.push_back(EdgeFunction(v0.position, v1.position, pixel));

				//precompute edge deltas for y and x (zmena x a y mezi vrcholy)
				edgesY.push_back(v2.position.y - v1.position.y);
				edgesY.push_back(v0.position.y - v2.position.y);
				edgesY.push_back(v1.position.y - v0.position.y);

				edgesX.push_back(v1.position.x - v2.position.x);
				edgesX.push_back(v2.position.x - v0.position.x);
				edgesX.push_back(v0.position.x - v1.position.x);

				//barycentric coords (not normalized, for pixel overlap test)
				std::vector<float> tuv(3);
				//2. and 3. barycentric coord (normalized, for interpolation of vertex attributes)
				glm::vec2 uv;
				//pixel color sent to buffer
				glm::f32vec3 pixelColor;
				float z;

				///PIXEL LOOP y, main scan-line loop
#ifdef BOUNDING_BOX
				for (uint32_t y = boundingBox[0].y+0.5f; y <= boundingBox[1].y+0.5f; ++y) {
#else
				for (uint32_t y = 0; y <= HEIGHT; ++y) {
#endif
					//unnormalized barycentric
					tuv[0] = tuvRow[0];
					tuv[1] = tuvRow[1];
					tuv[2] = tuvRow[2];
					///PIXEL LOOP x
#ifdef BOUNDING_BOX
					for (uint32_t x = boundingBox[0].x+0.5f; x <= boundingBox[1].x+0.5f; ++x)
					{
#else
					for (uint32_t x = 0; x <= WIDTH; ++x)
					{
#endif
						//sample center of the pixel...for antialiasing loop "pixel loop x" over more samples
						//pixel = glm::vec2(x + 0.5, y + 0.5);
						if (IsPixelInTriangle(tuv, v0.position, v1.position, v2.position))
						{
							uv.x = tuv[1] / parallelogramArea;
							uv.y = tuv[2] / parallelogramArea;
							//pixel depth in camera space
							z = 1 / ((1 - uv.x - uv.y) / v0.position.z + uv.x / v1.position.z + uv.y / v2.position.z);

							if (z < (zbuffer[x + WIDTH * y]))
							{
								zbuffer[x + WIDTH * y] = z;

								glm::vec3 N;
								glm::f32vec3 d = glm::f32vec3(0);
								glm::f32vec3 s = glm::f32vec3(0);

								if (!mesh->GetTextures().empty())
								{
									// Calculate shading and texturing
									// calcFragmentProperties
									glm::vec2 texCoords;
									Texture texture;
									for (auto texItr = mesh->GetTextures().begin(); texItr != mesh->GetTextures().end(); ++texItr)
									{
										// TODO - should be strcpy
										if ((*texItr).type == "texture_diffuse")
										{
											texture = (*texItr);
										}
									}

									CalcFragmentProperties(v0, v1, v2, v0cam, v1cam, v2cam, uv, z, texture.height, texture.width, N, texCoords);

									glm::f32vec2 fragment = z * ((v0cam / -v0.position.z) * (1 - uv.x - uv.y) + (v1cam / -v1.position.z) * uv.x + (v2cam / -v2.position.z) * uv.y);

									float angleOfIncidence = std::max(0.f, glm::dot(N, glm::normalize(-glm::vec3(fragment, -z))));

									//clamp texture coords..cant just subtract 1 because coords can be less than 1 and the result would be negative
									uint32_t texelIndex = 3 * (glm::clamp((int)texCoords.x, 0, texture.width - 1) + (texture.width)*glm::clamp((int)texCoords.y, 0, texture.height - 1));
									glm::u8vec3 rgb = glm::u8vec3(texture.data[0 + texelIndex],
										texture.data[1 + texelIndex],
										texture.data[2 + texelIndex]
									);
									//  Let the final color be C = I_view*M + I_ambient*M, where 
									//	Lview is view (cosine) angle attenuation intensity and 
									//	Lambient is ambient light intensity
									//	M is material color
									pixelColor = glm::clamp(angleOfIncidence*(U8vec2F32vec(rgb)) + AMBIENT_LIGHT * (U8vec2F32vec(rgb)), 0.f, 1.f);
									SetRGBAPixel(x, y, (unsigned char*)frameBuffer->pixels, F32vec2U8vec(pixelColor));
								}
								else
								{
									CalcFragmentProperties(v0, v1, v2, v0cam, v1cam, v2cam, uv, z, N);
									//interpolate point in camera space
									glm::f32vec2 fragment = z * ((v0cam / -v0.position.z) * (1 - uv.x - uv.y) + (v1cam / -v1.position.z) * uv.x + (v2cam / -v2.position.z) * uv.y);

									glm::vec3 fragmentCameraSpacePosition = glm::vec3(fragment, -z);
									glm::vec3 viewDirection = glm::normalize(-fragmentCameraSpacePosition);
									float angleOfIncidence = std::max(0.f, glm::dot(N, viewDirection));

									///PHONG
									for (auto light = g_LightList.begin(); light != g_LightList.end(); ++light)
									{
										glm::vec3 lightIntensity;
										glm::vec3 lightDirection;
									       	float lightDistance;

										(*light)->Shine(lightIntensity, lightDistance, lightDirection, fragmentCameraSpacePosition);

										glm::vec3 refDir = lightDirection;
										CalcReflectedDirection(N, refDir);

										d += mesh->GetAlbedo() * lightIntensity * glm::f32vec3(std::max(0.f, glm::dot(N, -lightDirection)));

										s += lightIntensity * std::pow(std::max(0.f, glm::dot(refDir, viewDirection)), mesh->GetMaterial().shininess);
									}

									pixelColor = glm::clamp((1.0f*mesh->GetMaterial().emissiveColor + d * mesh->GetMaterial().diffuseColor + s * mesh->GetMaterial().specluarColor + mesh->GetMaterial().ambientColor*AMBIENT_LIGHT), 0.f, 1.f);
									SetRGBAPixel(x, y, (unsigned char*)frameBuffer->pixels, F32vec2U8vec(pixelColor));
								}
							}
						}

						//x step
						tuv[0] += edgesY[0];
						tuv[1] += edgesY[1];
						tuv[2] += edgesY[2];
					}//end PIXEL row loop

					//y step
					tuvRow[0] += edgesX[0];
					tuvRow[1] += edgesX[1];
					tuvRow[2] += edgesX[2];
				}//end line loop

			}//end triangle loop for a given mesh

		}//end mesh loop

		//Draw
		SDL_UpdateTexture(texture, NULL, frameBuffer->pixels, WIDTH * sizeof(Uint32));
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

	return 0;
}
