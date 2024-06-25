// 314CA Dobre Andrei-Teodor 2023-2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "commands.h"
#define NMAX 256

int main(void)
{
	// Allocation of the image
	int_image *img = malloc(sizeof(int_image));
	img->loaded = 0; // At first the image is not loaded

	// Allocation of the command, filename and parameter
	// After every allocation we perform defensive programming
	char *command = malloc(NMAX * sizeof(char));
	defensive_programming_char(img, command);
	char *filename = malloc(NMAX * sizeof(char));
	defensive_programming_char(img, filename);
	char *parameter = malloc(NMAX * sizeof(char));
	defensive_programming_char(img, parameter);

	// We use the variable ascii to know if we are working with a ASCII image
	char ascii[6];

	// We use the variables x1_aux, x2_aux, y1_aux, y2_aux to read the
	// coordinates from the command and not modify the original coordinates
	int x1_aux, x2_aux, y1_aux, y2_aux, angle;

	while (1) {
		fgets(command, NMAX, stdin);

		if (strncmp(command, "EXIT", 4) == 0) {
			if (img->loaded == 1) {
				exit_command(img, command, filename, parameter);
				break;
			}
			printf("No image loaded\n");
			break;
		} else if (sscanf(command, "LOAD %s", filename) == 1) {
			load_image(img, filename);
		} else if (sscanf(command, "SELECT %d %d %d %d",
		&x1_aux, &y1_aux, &x2_aux, &y2_aux) == 4) {
			select_command(img, x1_aux, y1_aux, x2_aux, y2_aux);
		} else if (strncmp(command, "SELECT ALL", 10) == 0) {
			select_all(img);
		} else if (strncmp(command, "HISTOGRAM", 9) == 0) {
			histogram(img, command);
		} else if (strncmp(command, "EQUALIZE", 8) == 0) {
			equalize(img);
		} else if (strncmp(command, "CROP", 4) == 0) {
			crop(img);
		} else if (sscanf(command, "APPLY %s", parameter) != 0) {
			apply(img, parameter);
		} else if (sscanf(command, "SAVE %s %s", filename, ascii) == 2) {
			save_image(img, filename, 1);
		} else if (sscanf(command, "SAVE %s %s", filename, ascii) == 1) {
			save_image(img, filename, 0);
		} else if (sscanf(command, "ROTATE %d", &angle) == 1) {
			rotate(img, angle);
		} else {
			printf("Invalid command\n");
		}
	}
	return 0;
}
