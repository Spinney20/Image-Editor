// 314CA Dobre Andrei-Teodor 2023-2024

#include <stdio.h>
#include <stdlib.h>

// The structure for the RGB pixel
// We use this structure for the RGB data
typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} RGB;

// The structure for the image
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

// Function for freeing a unsigned char matrix
void free_unsigned_char_matrix(unsigned char **matrix, int height)
{
	for (int i = 0; i < height; i++)
		free(matrix[i]);
	free(matrix);
}

// Function for freeing a RGB matrix
void free_rgb_matrix(RGB **matrix, int height)
{
	for (int i = 0; i < height; i++)
		free(matrix[i]);
	free(matrix);
}

// Function for freeing the memory of the image
void free_image(int_image *img)
{
	if (img->type == 2 || img->type == 5)
		free_unsigned_char_matrix(img->data, img->height);
	else if (img->type == 3 || img->type == 6)
		free_rgb_matrix(img->rgb_data, img->height);
}

// Function for performing defensive programming on a RGB matrix
// We use the variable is_data to know if we are working with the data matrix
// or with the temporary matrix
void defensive_programming_rgb(int_image *img, RGB **matrix,
							   int height, int is_data)
{
	if (!matrix) {
		// freeing the given matrix
		free_rgb_matrix(matrix, height);

		// If we are working with the data matrix, its data was already freed
		// in the previous function
		// If we are working with the temporary matrix (is_data == 0), we need
		// to free the image data as well
		if (is_data == 0)
			free_image(img);
		exit(1);
	}
}

// Function for performing defensive programming on a unsigned char matrix
// We use the variable is_data to know if we are working with the data matrix
// or with the temporary matrix
void defensive_programming_gray(int_image *img, unsigned char **matrix,
								int height, int is_data)
{
	if (!matrix) {
		// freeing the given matrix
		free_unsigned_char_matrix(matrix, height);

		// If we are working with the data matrix, its data was already freed
		// in the previous function
		// If we are working with the temporary matrix (is_data == 0), we need
		// to free the image data as well
		if (is_data == 0)
			free_image(img);
		exit(1);
	}
}

// Function for performing defensive programming on an array
void defensive_programming_array(int_image *img, int *arr)
{
	if (!arr) {
		// Freeing the given array
		free(arr);

		//Freeing the image data
		free_image(img);
		exit(1);
	}
}

void defensive_programming_char(int_image *img, char *arr)
{
	if (!arr) {
		// Freeing the given array
		free(arr);
		//Freeing the image data
		//if its an image loaded
		if (img->loaded == 1)
			free_image(img);
		exit(1);
	}
}

// Function for freeing a double matrix
void free_double_matrix(double **matrix, int height)
{
	for (int i = 0; i < height; i++)
		free(matrix[i]);
	free(matrix);
}
