/* main.c */

#include "src/in/app.h"

int main(void) {
	if(!init("LifeSim")) {
		return 1;
	}

	run();

	cleanup();

	printf("Exiting sim...\n");
	return 0;
}
