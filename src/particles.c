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
			Groups[i].pos[j].x = randf(-SCREEN_WIDTH/2, SCREEN_WIDTH/2);
			Groups[i].pos[j].y = randf(-SCREEN_HEIGHT/2, SCREEN_HEIGHT/2);
			Groups[i].vel[j].x = randf(-0.5f, 0.5f);
			Groups[i].vel[j].y = randf(-0.5f, 0.5f);
		}
	}
}

void randomize_model() {
    for (int i = 0; i < GROUP_TYPES; i++) {
        counts[i] = (i8)roundf(randf(100.0f, MAX_GROUP_PARTICLES));
    }
    // forces [-100,100] scaled
    for (int i = 0; i < GROUP_TYPES; i++) for (int j = 0; j < GROUP_TYPES; j++) {
        Forces[i][j] = randf(-100.0f, 100.0f) * 0.18f;
        force_rads[i][j] = randf(10.0f, 200.0f) * 0.9f;
    }
    viscosity = randf(0.0f, 0.12f);
    worldGravity = randf(-0.2f, 0.2f);
    wallRepel = randf(0.0f, 80.0f);
}

void update_particles() {
	for (int i = 0; i < GROUP_TYPES; i++) for (int j = 0; j < GROUP_TYPES; j++) {
		apply_forces(&Groups[i], &Groups[j], Forces[i][j], force_rads[i][j], bounded);
	}
}

void apply_forces(ParticleGroup *A, ParticleGroup *B, float G, float R, int toggle_bounds) {
	
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
