/* util.h */

#pragma once

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 1280

typedef unsigned char i8;
typedef unsigned short i16;
typedef unsigned int i32;
typedef unsigned long i64;

#define true 1
#define false 0
#define bool i8

static inline float randf(float min, float max) {
	return ((float)rand() / RAND_MAX) * (max - min) + min;
}
