#pragma once

///For getting working directory path
//COMMENT THIS LINE IF NOT WINDOWS
#define WINDOWS
#include <stdio.h>
#ifdef WINDOWS
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
#define U8vec2F32vec(uint_vec) glm::f32vec3(U2F(uint_vec[0]),U2F(uint_vec[1],U2F(uint_vec[2])

#define WIDTH 800
#define HEIGHT 600
#define PROFILE
#define OUT

#define AMBIENT_LIGHT 0.075f
#define BACKFACE_CULLING
#define SMOOTH_SHADING