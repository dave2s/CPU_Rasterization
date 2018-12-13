#pragma once

///For getting working directory path
//COMMENT THIS LINE IF NOT WINDOWS
#include <stdio.h>
#ifdef _WIN32
#include<direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

// Preprocessor directive for converting colors from floats to unsigned and vice-versa.
#define F2U(float_x) (uint8_t)round(float_x * 255.0f)
#define F32vec2U8vec(float_vec) glm::u8vec3(F2U(float_vec[0]),F2U(float_vec[1]),F2U(float_vec[2]))
#define U2F(uint_x) (float)uint_x/255.0f
#define U8vec2F32vec(uint_vec) glm::f32vec3(U2F(uint_vec[0]),U2F(uint_vec[1]),U2F(uint_vec[2]))

#define WIDTH 600
#define HEIGHT 480
#define PROFILE
#define OUT

//Optimization of drawing - go only trough pixels sorounding the triangle
#define BOUNDING_BOX
//material color minimal intensity - always at least this visible
#define AMBIENT_LIGHT 0.175f
//discard triangles facing away from the camera
#define BACKFACE_CULLING
//triangle normals are interpolated from Vertex.normal using barycentric coordinates (assimp precomputes if missing)
#define SMOOTH_SHADING
//repeat textures if coordinates extrapolate out of the texture size
#define TEXTURE_REPEAT
