/* particles.h */

#pragma once

#include "util.h"

#define GROUP_TYPES 4

typedef struct {
	float x, y;
} Vec2D;

typedef struct {
	Vec2D *pos;
	Vec2D *vel;
	int count;
} ParticleGroup;

#define MAX_GROUP_PARTICLES 300

extern ParticleGroup Groups[GROUP_TYPES];
extern float Forces[GROUP_TYPES][GROUP_TYPES];
extern float force_rads[GROUP_TYPES][GROUP_TYPES];
extern i8 counts[GROUP_TYPES];
extern float colors[GROUP_TYPES][3];

void reset_particles();

void allocate_particles();

void free_particles();

void randomize_model();

void apply_forces(ParticleGroup *A, ParticleGroup *B, float G, float R, int toggle_bounds);

void update_particles();

void render_particles();

extern float viscosity;
extern float worldGravity; // gravity applied to vel.y after viscosity
extern float wallRepel;
extern int bounded;
extern int motionBlur;


void save_model(const char *fname);
void load_model(const char *fname);
