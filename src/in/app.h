/* app.h */

#pragma once

#include "util.h"
#include "particles.h"

bool init(const char *title);

void run();

void cleanup();

void update_title(float fps);

void draw_motion_blur();
