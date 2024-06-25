// 314CA Dobre Andrei-Teodor 2023-2024

#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} RGB;

typedef struct {
	unsigned int type; // 2 - gray ASCII, 3 - color ASCII, 5 - gray binary,
					   // 6 - color binary
	int width;
	int height;
	int maximum_value;

	// For gray images we use the data matrix
	unsigned char **data;
	// For color images we use the rgb_data matrix
	RGB **rgb_data;
	int loaded;
	int x1, x2, y1, y2;
	int kernel_type;

} int_image;

void free_unsigned_char_matrix(unsigned char **matrix, int height);
void free_rgb_matrix(RGB **matrix, int height);
void free_image(int_image *img);
void defensive_programming_rgb(int_image *img, RGB **matrix,
							   int height, int is_data);
void defensive_programming_gray(int_image *img, unsigned char **matrix,
								int height, int is_data);
void defensive_programming_array(int_image *img, int *arr);
void free_double_matrix(double **matrix, int height);
void defensive_programming_char(int_image *img, char *arr);
