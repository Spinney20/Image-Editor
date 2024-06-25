// 314CA Dobre Andrei-Teodor 2023-2024

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#define KERNEL_SIZE 3

void copy_matrix(unsigned char **source_matrix, unsigned char **dest_matrix,
				 int start_i, int end_i, int start_j, int end_j);
void copy_rgb_matrix(RGB **source_matrix, RGB **dest_matrix,
					 int start_i, int end_i, int start_j, int end_j);
void ignore_comment(FILE *in, char *array);
void update_selection(int_image *img);
void load_another_image(int_image *img);
void load_image(int_image *img, const char *filename);
int load_error(int_image *img);
void save_image(int_image *img, const char *outputfile, int ascii);
void select_command(int_image *img, int x1_aux, int y1_aux,
					int x2_aux, int y2_aux);
void select_all(int_image *img);
void histogram(int_image *img, char *command);
void clamp(double *value);
void update_for_crop(int_image *img, int new_width, int new_height);
void crop(int_image *img);
void equalize(int_image *img);
void set_kernel(double **kernel,
				const double source_kernel[KERNEL_SIZE][KERNEL_SIZE]);
void create_kernel(int_image *img, char *parameter, double **kernel,
				   int *valid);
int verify_apply(int_image *img, char *parameter);
void apply(int_image *img, char *parameter);
void rotate_gray_selection(int_image *img, int angle);
void rotate_color_selection(int_image *img, int angle);
void update_rotated_img(int_image *img);
void rotate_gray_image(int_image *img, int angle);
void rotate_color_image(int_image *img, int angle);
void rotate(int_image *img, int angle);
void exit_command(int_image *img, char *command, char *filename,
				  char *parameter);
