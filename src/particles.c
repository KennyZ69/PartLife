/* particles.c */

#include "in/particles.h"
#include "in/util.h"
#include <GL/gl.h>
#include <stdlib.h>

ParticleGroup Groups[GROUP_TYPES];
i8 counts[GROUP_TYPES] = {200, 200, 200, 200};
float Forces[GROUP_TYPES][GROUP_TYPES];
float force_rads[GROUP_TYPES][GROUP_TYPES];
float colors[GROUP_TYPES][3] = {
	{0.2f, 1.0f, 0.2f},
	{1.0f, 0.2f,  0.2f},
	{1.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 0.2f}
};

float viscosity = 0.04f;
float worldGravity = 0.0f;
float wallRepel = 20.0f;
int bounded = 1;
int motionBlur = 0;

// void init_defaults() {
//     float def_force[GROUP_TYPES][GROUP_TYPES] = {
//         {  18.0f, -12.0f,   8.0f,  -6.0f },
//         { -10.0f,  22.0f,  -8.0f,  10.0f },
//         {   6.0f,  -8.0f,  14.0f,  -6.0f },
//         {  -6.0f,  10.0f,  -8.0f,  20.0f }
//     };
//     float def_radius[GROUP_TYPES][GROUP_TYPES] = {
//         { 60, 80, 70, 60 },
//         { 80, 60, 70, 75 },
//         { 70, 70, 60, 70 },
//         { 60, 75, 70, 65 }
//     };
//     for (int i=0;i<GROUP_TYPES;i++) for (int j=0;j<GROUP_TYPES;j++) {
//         Forces[i][j] = def_force[i][j];
//         force_rads[i][j] = def_radius[i][j];
//     }
// }

void allocate_particles() {
	for (int i = 0; i < GROUP_TYPES; i++) {
		Groups[i].pos = (Vec2D*)malloc(sizeof(Vec2D) * MAX_GROUP_PARTICLES);
		Groups[i].vel = (Vec2D*)malloc(sizeof(Vec2D) * MAX_GROUP_PARTICLES);
		Groups[i].count = 0;
	}
}

void free_particles() {
	for (int i = 0; i < GROUP_TYPES; i++) {
		free(Groups[i].pos);
		free(Groups[i].vel);
		Groups[i].pos = Groups[i].vel = NULL;
		Groups[i].count = 0;
	}
}

void reset_particles() {
	for (int i = 0; i < GROUP_TYPES; i++) {
		Groups[i].count = counts[i];
		for (int j = 0; j < Groups[i].count; j++) {
			Groups[i].pos[j].x = randf(-SCREEN_WIDTH*0.5f, SCREEN_WIDTH*0.5f);
			Groups[i].pos[j].y = randf(-SCREEN_HEIGHT*0.5f, SCREEN_HEIGHT*0.5f);
			Groups[i].vel[j].x = randf(-0.5f, 0.5f);
			Groups[i].vel[j].y = randf(-0.5f, 0.5f);
		}
	}
}

void randomize_model() {
    for (int i = 0; i < GROUP_TYPES; i++) {
        counts[i] = (i8)roundf(randf(MIN_GROUP_PARTICLES, MAX_GROUP_PARTICLES));
    }
    // forces [-100,100] scaled
    for (int i = 0; i < GROUP_TYPES; i++) for (int j = 0; j < GROUP_TYPES; j++) {
        Forces[i][j] = randf(-100.0f, 100.0f);
        force_rads[i][j] = randf(10.0f, 200.0f);
    }
    viscosity = randf(0.0f, 0.12f);
    worldGravity = randf(-0.05f, 0.05f);
    wallRepel = randf(0.0f, 60.0f);
}

void update_particles() {
	for (int i = 0; i < GROUP_TYPES; i++) for (int j = 0; j < GROUP_TYPES; j++) {
		apply_forces(&Groups[i], &Groups[j], Forces[i][j], force_rads[i][j], bounded);
	}
}

void apply_forces(ParticleGroup *A, ParticleGroup *B, float G, float R, int toggle_bounds) {
	float g = G / -100.0f; // attraction coefficient
	int countA = A->count;
	int countB = B->count;
	float r_sq = R*R;

	for (int i = 0; i < countA; i++) {
		float fx = 0.0f, fy = 0.0f;
		float ax = A->pos[i].x, ay = A->pos[i].y;
		for (int j = 0; j < countB; j++) {
			if (A == B && i == j) continue;

			float bx = B->pos[j].x, by = B->pos[j].y;
			float dx = ax - bx;
			float dy = ay - by;
			float dist_sq = dx*dx + dy*dy;
			if (dist_sq < INTERACTION_EPS) continue;
			if (dist_sq < r_sq) { // when it is in range for force
				float f = 1.0f / sqrtf(dist_sq);
				fx += dx * f;
				fy += dy * f;
			}
		}

		// wall repel
		if (wallRepel > 0.0f) {
			if (A->pos[i].x < - (SCREEN_WIDTH*0.5f) + wallRepel) A->vel[i].x += ((- (SCREEN_WIDTH*0.5f) + wallRepel) - A->pos[i].x) * 0.1f;
			if (A->pos[i].y < - (SCREEN_HEIGHT*0.5f) + wallRepel) A->vel[i].y += ((- (SCREEN_HEIGHT*0.5f) + wallRepel) - A->pos[i].y) * 0.1f;
			if (A->pos[i].x >   (SCREEN_WIDTH*0.5f) - wallRepel) A->vel[i].x += (((SCREEN_WIDTH*0.5f) - wallRepel) - A->pos[i].x) * 0.1f;
			if (A->pos[i].y >   (SCREEN_HEIGHT*0.5f) - wallRepel) A->vel[i].y += (((SCREEN_HEIGHT*0.5f) - wallRepel) - A->pos[i].y) * 0.1f;
		}

		// updating position based on the calculated sim gravity and viscosity
		A->vel[i].x = (A->vel[i].x + (fx * g)) * (1.0f - viscosity);
		A->vel[i].y = (A->vel[i].y + (fy * g)) * (1.0f - viscosity) + worldGravity;

		A->pos[i].x += A->vel[i].x;
		A->pos[i].y += A->vel[i].y;
	}

	if (toggle_bounds) {
	for (int i=0;i<countA;i++) {
            if (A->pos[i].x < -SCREEN_WIDTH*0.5f) A->pos[i].x = -SCREEN_WIDTH*0.5f;
            if (A->pos[i].x >  SCREEN_WIDTH*0.5f) A->pos[i].x = SCREEN_WIDTH*0.5f;
            if (A->pos[i].y < -SCREEN_HEIGHT*0.5f) A->pos[i].y = -SCREEN_HEIGHT*0.5f;
            if (A->pos[i].y >  SCREEN_HEIGHT*0.5f) A->pos[i].y = SCREEN_HEIGHT*0.5f;
        }
	}
}

void render_particles() {
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < GROUP_TYPES; i++) {
		glColor3f(colors[i][0], colors[i][1], colors[i][2]);
		for (int j = 0; j < Groups[i].count; j++) {
			glVertex2f(Groups[i].pos[j].x, Groups[i].pos[j].y);
		}
	}
	glEnd();
}
