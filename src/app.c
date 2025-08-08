/* app.c */

#include "in/app.h"
#include "in/particles.h"
#include "in/util.h"
#include <GL/gl.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

static SDL_Window *window;
static SDL_GLContext glContext;

bool init (const char *title) {
	srand(time(NULL));

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Could not init SDL: %s\n", SDL_GetError());
		return false;
	}
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!window) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		SDL_Quit();
		return false;
	}

	glContext = SDL_GL_CreateContext(window);
	if (!glContext) {
		fprintf(stderr, "Could not create context: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-SCREEN_WIDTH*0.5f, SCREEN_WIDTH*0.5f, -SCREEN_HEIGHT*0.5f, SCREEN_HEIGHT*0.5f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4f(0, 0, 0, 0.12f);
	SDL_GL_SetSwapInterval(1);
	// glEnable(GL_DEPTH_TEST);

	return true;
}

void run() {
	i8 running = true;
	SDL_Event event;

	Uint32 last_ticks = SDL_GetTicks();
	Uint32 fps_timer = SDL_GetTicks();
	i32 frame_count = 0;
	float fps = 0.0f;

	allocate_particles();
	// load_model("model.txt");
	reset_particles();

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			} else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						running = false;
						break;
					case SDLK_r:
						reset_particles();
						break;
					case SDLK_SPACE:
						randomize_model();
						reset_particles();
						break;
					case SDLK_s:
						// save_model("model.txt");
						break;
					case SDLK_l:
						// load_model("model.txt");
						break;
					case SDLK_m:
						motionBlur = !motionBlur;
						break;
					case SDLK_b:
						bounded = !bounded;
						break;
					default:
						break;
				}
			} else if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					int width = event.window.data1, height = event.window.data2;
					glViewport(0, 0, width, height);
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					glOrtho(-width*0.5f, width*0.5f, -height*0.5f, height*0.5f, -1.0f, 1.0f);
					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();
				}
			}
		}

		Uint32 now = SDL_GetTicks();
		last_ticks = now;

		update_particles();

		if (motionBlur) {
			draw_motion_blur();
		} else {
			glColor4f(0, 0, 0, 0.12f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		render_particles();

		SDL_GL_SwapWindow(window);
		SDL_Delay(16);

		frame_count++;
		if (now - fps_timer >= 500) {
			fps = (frame_count*1000.0f) / (now - fps_timer); 
			fps_timer = now;
			frame_count = 0;
			update_title(fps);
		}
	}
}

void cleanup() {
	free_particles();
	if (glContext) {
		SDL_GL_DeleteContext(glContext);
	}
	if (window) {
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
}

void update_title(float fps) {
	char title[256];
	int total = 0;
	for (int i = 0; i < GROUP_TYPES; ++i) total += Groups[i].count;
	snprintf(title, sizeof(title), "Particle Life - Particles: %d  FPS: %.0f  Visc: %.3f  Gravity: %.3f  Bounded:%d  MB:%d",
	total, fps, viscosity, worldGravity, bounded, motionBlur);
	SDL_SetWindowTitle(window, title);
}

void draw_motion_blur() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0, 0, 0, 0.12f);
	glBegin(GL_QUADS);
		glVertex2f(-SCREEN_WIDTH*0.5f, -SCREEN_HEIGHT*0.5f);
		glVertex2f(SCREEN_WIDTH*0.5f, -SCREEN_HEIGHT*0.5f);
		glVertex2f(SCREEN_WIDTH*0.5f, SCREEN_HEIGHT*0.5f);
		glVertex2f(-SCREEN_WIDTH*0.5f, SCREEN_HEIGHT*0.5f);
	glEnd();
	glDisable(GL_BLEND);
}
