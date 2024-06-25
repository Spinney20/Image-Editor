// 314CA Dobre Andrei-Teodor 2023-2024

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#define NMAX 256
#define KERNEL_SIZE 3

// Function for copying a matrix into another matrix
void copy_matrix(unsigned char **source_matrix, unsigned char **dest_matrix,
				 int start_i, int end_i, int start_j, int end_j)
{
	for (int i = start_i; i < end_i; i++)
		for (int j = start_j; j < end_j; j++)
			dest_matrix[i][j] = source_matrix[i][j];
}

// Function for copying a RGB matrix into another RGB matrix
void copy_rgb_matrix(RGB **source_matrix, RGB **dest_matrix,
					 int start_i, int end_i, int start_j, int end_j)
{
	for (int i = start_i; i < end_i; i++)
		for (int j = start_j; j < end_j; j++)
			dest_matrix[i][j] = source_matrix[i][j];
}

// A function that ignores every line that starts with '#'
// and returns the first line that it encounters that doesn't start wiht #
void ignore_comment(FILE *in, char *array)
{
	while (1) {
		fgets(array, NMAX, in);
		if (array[0] == '#')
			continue;
		break;
	}
}

// updating the selection to the entire image
void update_selection(int_image *img)
{
	img->x1 = 0;
	img->x2 = img->width;
	img->y1 = 0;
	img->y2 = img->height;
	img->loaded = 1; // for the load command
}

//Function designed for the "load_image" command
// It is used for treating the case when the user wants to load another image
// while another image is already loaded
void load_another_image(int_image *img)
{
	// Checking if another image is loaded
	if (img->loaded == 1) {
		// Freeing the memory of the previous image
		free_image(img);

		// Setting the loaded parameter to 0
		img->loaded = 0;
	}
}

// Function for loading the image
void load_image(int_image *img, const char *filename)
{
	FILE *in = fopen(filename, "r");// Opening the file
	load_another_image(img); // Freeing the already loaded img if its the case
	if (!in) { // Checking if the file was opened correctly
		printf("Failed to load %s\n", filename);
		return;
	}
	char input_line[NMAX], P;
	int dummy; //variable for avoiding naked sscanf warnings

	// Ignoring the eventual comment before the magic word
	ignore_comment(in, input_line);
	dummy = sscanf(input_line, "%c %d", &P, &img->type);

	// Ignoring the eventual comment before the width and height
	ignore_comment(in, input_line);
	dummy = sscanf(input_line, "%d%d", &img->width, &img->height);

	// Ignoring the eventual comment before the maximum value
	ignore_comment(in, input_line);
	dummy = sscanf(input_line, "%d", &img->maximum_value);

	// Ignoring the eventual comment before the image
	ignore_comment(in, input_line);

	// Going back to the beginning of the image because the ignore_comment
	// function reads the first line that doesn't start with #
	// and we would have missed the first line of the image
	fseek(in, -strlen(input_line), SEEK_CUR);

	if (img->type == 2) { //loading the gray image in ASCII
		img->data = malloc(img->height * sizeof(unsigned char *));
		for (int i = 0; i < img->height; i++)
			img->data[i] = malloc(img->width * sizeof(unsigned char));
		defensive_programming_gray(img, img->data, img->height, 1);

		for (int i = 0; i < img->height; i++) // Reading the image
			for (int j = 0; j < img->width; j++)
				fscanf(in, "%hhu", &img->data[i][j]);
	} else if (img->type == 5) { //loading the gray image in binary
		img->data = malloc(img->height * sizeof(unsigned char *));
		for (int i = 0; i < img->height; i++)
			img->data[i] = malloc(img->width * sizeof(unsigned char));
		defensive_programming_gray(img, img->data, img->height, 1);

		for (int i = 0; i < img->height; i++) // Reading the image
			for (int j = 0; j < img->width; j++)
				fread(&img->data[i][j], sizeof(unsigned char), 1, in);

	} else if (img->type == 3) { //loading the RGB image in ASCII
		img->rgb_data = malloc(img->height * sizeof(RGB *));
		for (int i = 0; i < img->height; i++) {
			img->rgb_data[i] = malloc(img->width * sizeof(RGB));
		defensive_programming_rgb(img, img->rgb_data, img->height, 1);
			for (int j = 0; j < img->width; j++) {
				RGB pixel; // Reading the image
				fscanf(in, "%hhu%hhu%hhu", &pixel.red,
					   &pixel.green, &pixel.blue);
				img->rgb_data[i][j] = pixel;
			}
		}
	} else if (img->type == 6) { //loading the RGB image in binary
		img->rgb_data = malloc(img->height * sizeof(RGB *));
		for (int i = 0; i < img->height; i++) {
			img->rgb_data[i] = malloc(img->width * sizeof(RGB));
		defensive_programming_rgb(img, img->rgb_data, img->height, 1);
			for (int j = 0; j < img->width; j++) {
				RGB pixel; // Reading the image
				fread(&pixel.red, sizeof(unsigned char), 1, in);
				fread(&pixel.green, sizeof(unsigned char), 1, in);
				fread(&pixel.blue, sizeof(unsigned char), 1, in);
				img->rgb_data[i][j] = pixel;
			}
		}
	}
	// After loading the image, the selected area is the entire image
	if (dummy) // Checking if the dummy variable was used (to avoid warnings)
		update_selection(img);
	fclose(in); // Closing the file
	printf("Loaded %s\n", filename);
}

// Function for the case when the user wants to do an operation but hasn't
// loaded an image yet
// Used in multiple functions along the code
int load_error(int_image *img)
{
	if (img->loaded == 0) {
		printf("No image loaded\n");
		return 1;
	}
	return 0;
}

// Function for saving the image
void save_image(int_image *img, const char *outputfile, int ascii)
{
	// Checking if there is an image loaded
	if (load_error(img))
		return;

	FILE *out = fopen(outputfile, "w"); // Opening the file
	// Checking if the file was opened correctly
	if (!out) {
		printf("Failed to save %s\n", outputfile);
		return;
	}
	// Checking how the image parameters are saved (the type - 2,3,5 or 6)
	if (!ascii) {
		if (img->type == 5 || img->type == 6)
			fprintf(out, "P%d\n", img->type);
		else
			fprintf(out, "P%d\n", img->type + 3);
	} else {
		if (img->type == 5 || img->type == 6)
			fprintf(out, "P%d\n", img->type - 3);
		else
			fprintf(out, "P%d\n", img->type);
	}
	// Printing width, height and maximum value
	fprintf(out, "%d %d\n", img->width, img->height);
	fprintf(out, "%d\n", img->maximum_value);

	// Checking how we should save the image for the ascii case
	if (ascii) {
		if (img->type == 2 || img->type == 5) {
			for (int i = 0; i < img->height; i++) {
				for (int j = 0; j < img->width; j++)
					fprintf(out, "%hhu ", img->data[i][j]);
				fprintf(out, "\n");
			}
		} else if (img->type == 3 || img->type == 6) {
			for (int i = 0; i < img->height; i++) {
				for (int j = 0; j < img->width; j++) {
					fprintf(out, "%hhu %hhu %hhu ", img->rgb_data[i][j].red,
							img->rgb_data[i][j].green,
							img->rgb_data[i][j].blue);
				}
				fprintf(out, "\n");
			}
		}
	} else { // Checking how we should save the image for the binary case
		if (img->type == 2 || img->type == 5) { // saving a gray image
			for (int i = 0; i < img->height; i++)
				for (int j = 0; j < img->width; j++)
					fwrite(&img->data[i][j], sizeof(unsigned char), 1, out);
		} else if (img->type == 3 || img->type == 6) { // saving an RGB image
			for (int i = 0; i < img->height; i++) {
				for (int j = 0; j < img->width; j++) {
					fwrite(&img->rgb_data[i][j].red, // red pixel
						   sizeof(unsigned char), 1, out);
					fwrite(&img->rgb_data[i][j].green, // green pixel
						   sizeof(unsigned char), 1, out);
					fwrite(&img->rgb_data[i][j].blue, // blue pixel
						   sizeof(unsigned char), 1, out);
				}
			}
		}
	}
	fclose(out);
	printf("Saved %s\n", outputfile);
}

// Function for the "SELECT" command
void select_command(int_image *img, int x1_aux, int y1_aux,
					int x2_aux, int y2_aux)
{
	if (load_error(img))
		return;

	// I used aux variables because I don't want to change the original
	// coordinates if the input is invalid
	if (x1_aux < 0 || x1_aux > img->width || x2_aux < 0 ||
		x2_aux > img->width || y1_aux < 0 || y1_aux > img->height ||
		y2_aux < 0 || y2_aux > img->height) {
		printf("Invalid set of coordinates\n");
		return;
	}

	if (x1_aux == x2_aux || y1_aux == y2_aux) {
		printf("Invalid set of coordinates\n");
		return;
	}

	// I want the coordinates to be in the order x1, x2, y1, y2
	if (x1_aux > x2_aux) {
		int temp = x1_aux;
		x1_aux = x2_aux;
		x2_aux = temp;
	}

	if (y1_aux > y2_aux) {
		int temp = y1_aux;
		y1_aux = y2_aux;
		y2_aux = temp;
	}

	// Now if the input is valid, I change the original coordinates
	img->x1 = x1_aux;
	img->x2 = x2_aux;
	img->y1 = y1_aux;
	img->y2 = y2_aux;

	printf("Selected %d %d %d %d\n", x1_aux, y1_aux, x2_aux, y2_aux);
}

// Function for the "SELECT" command - the case when all the image is selected
void select_all(int_image *img)
{
	if (load_error(img))
		return;

	// Selecting the entire image
	img->x1 = 0;
	img->x2 = img->width;
	img->y1 = 0;
	img->y2 = img->height;

	printf("Selected ALL\n");
}

// Function for the histogram command
void histogram(int_image *img, char *command)
{
	int stars, bin, extra;

	// Verifying if an image is loaded
	if (load_error(img))
		return;

	// Checking the case when the user inputs 3 or more parameters
	if (sscanf(command, "HISTOGRAM %d %d %d", &stars, &bin, &extra) == 3) {
		printf("Invalid command\n");
		return;

	// After, we check the favorable case in which the user inputs 2 parameters
	} else if (sscanf(command, "HISTOGRAM %d %d", &stars, &bin) == 2) {
		if (stars < 0 || bin < 0 || stars > 256 || bin > 256) {
			printf("Invalid command\n");
			return;
		}

	// After, the case when the user inputs only 1 parameter
	} else if (sscanf(command, "HISTOGRAM %d", &stars) == 1) {
		printf("Invalid command\n");
		return;

	// And finally, the case when the user inputs no parameters
	} else if (strcmp(command, "HISTOGRAM\n") == 0) {
		printf("Invalid command\n");
		return;
	}

	// If the image is RGB printing an error message
	if (img->type == 3 || img->type == 6) {
		printf("Black and white image needed\n");
		return;
	}

	// In the frequency array histogram, I will store the appearances of
	// each pixel from the image
	int *histogram = calloc(img->maximum_value + 1, sizeof(int));
	defensive_programming_array(img, histogram);

	// Calculating the frequency of each pixel
	for (int i = img->y1; i < img->y2; i++)
		for (int j = img->x1; j < img->x2; j++)
			histogram[img->data[i][j]]++;

	int max_stars = 0;
	int bin_dimmension = NMAX / bin;
	double result;

	// In the frequency array bin_histogram, I will store the appearances of
	// the pixels in each bin
	int *bin_histogram = calloc(bin, sizeof(int));
	defensive_programming_array(img, bin_histogram);

	for (int i = 0; i < bin; i++) {
		for (int j = i * bin_dimmension; j < (i + 1) * bin_dimmension; j++)
			bin_histogram[i] += histogram[j];
		// Determining the bin with the maximum number of stars
		// because we need this value in the formula
		if (bin_histogram[i] > max_stars)
			max_stars = bin_histogram[i];
	}

	for (int i = 0; i < bin; i++) {
		// Using the formula to calculate the number of stars shown for each bin
		result = (double)bin_histogram[i] / max_stars * stars;

		// Printing the histogram as required
		printf("%d\t|\t", (int)result);
		for (int j = 0; j < (int)result; j++)
			printf("*");
		printf("\n");
	}

	// Freeing the memory
	free(histogram);
	free(bin_histogram);
}

// Clamp function
void clamp(double *value)
{
	// If the value is smaller than 0, it becomes 0
	if (*value < 0)
		*value = 0;
	// If the value is bigger than 255, it becomes 255
	else if (*value > 255)
		*value = 255;
}

// Function designed for the crop command to update the image parameters
void update_for_crop(int_image *img, int new_width, int new_height)
{
		img->height = new_height;
		img->width = new_width;
		img->x1 = 0;
		img->x2 = new_width;
		img->y1 = 0;
		img->y2 = new_height;
}

// Function designed for the "CROP" command
void crop(int_image *img)
{
	//Checking if an image is loaded
	if (load_error(img))
		return;

	int new_width = img->x2 - img->x1; // the new width of the image
	int new_height = img->y2 - img->y1; // the new height of the image

	if (img->type == 2 || img->type == 5) {
		// Allocation of a temporary image
		unsigned char **tmp_image =
		malloc(new_height * sizeof(unsigned char *));
		for (int i = 0; i < new_height; i++)
			tmp_image[i] = malloc(new_width * sizeof(unsigned char));
		defensive_programming_gray(img, tmp_image, new_height, 0);

		// Copying the selected area into the temporary image
		for (int i = img->y1; i < img->y2; i++)
			for (int j = img->x1; j < img->x2; j++)
				tmp_image[i - img->y1][j - img->x1] = img->data[i][j];

		// Freeing the original image
		free_image(img);
		// Updating the image
		update_for_crop(img, new_width, new_height);

		// Allocating the new, cropped, image
		img->data = malloc(img->height * sizeof(unsigned char *));
		for (int i = 0; i < img->height; i++)
			img->data[i] = malloc(img->width * sizeof(unsigned char));
		defensive_programming_gray(img, img->data, img->height, 1);

		// Copying the temporary image into the desired image
		copy_matrix(tmp_image, img->data, 0, img->height, 0, img->width);

		// Freeing the temporary image
		free_unsigned_char_matrix(tmp_image, img->height);

	} else if (img->type == 3 || img->type == 6) {
		// Alocation of a temporary color image
		RGB **tmp_image = malloc(new_height * sizeof(RGB *));
		for (int i = 0; i < new_height; i++)
			tmp_image[i] = malloc(new_width * sizeof(RGB));
		defensive_programming_rgb(img, tmp_image, new_height, 0);

		// Copying the selected area into the temporary image
		for (int i = img->y1; i < img->y2; i++)
			for (int j = img->x1; j < img->x2; j++)
				tmp_image[i - img->y1][j - img->x1] = img->rgb_data[i][j];

		// Freeing the initial matrix
		free_image(img);
		// Updating the image properties
		update_for_crop(img, new_width, new_height);

		// Allocating the new, cropped, image
		img->rgb_data = malloc(img->height * sizeof(RGB *));
		for (int i = 0; i < img->height; i++)
			img->rgb_data[i] = malloc(img->width * sizeof(RGB));
		defensive_programming_rgb(img, img->rgb_data, img->height, 1);

		// Copying the temporary image into the desired image
		copy_rgb_matrix(tmp_image, img->rgb_data,
						0, img->height, 0, img->width);

		// Freeing the temporary image
		free_rgb_matrix(tmp_image, img->height);
	}
	printf("Image cropped\n");
}

// Function for the "EQUALIZE" command
void equalize(int_image *img)
{
	// Checking if an image is loaded
	if (load_error(img))
		return;

	// Checking if the image is RGB, we need a grayscale image
	if (img->type == 3 || img->type == 6) {
		printf("Black and white image needed\n");
		return;
	}

	// Allocating a frequency array
	int *frequency = calloc(NMAX, sizeof(int));
	defensive_programming_array(img, frequency);

	for (int i = 0; i < img->height; i++)
		for (int j = 0; j < img->width; j++)
			frequency[img->data[i][j]]++;

	int area = img->width * img->height; // area of the image
	double result;
	unsigned long long sum_of_frequencies = 0;
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			sum_of_frequencies = 0;
			for (int k = 0; k <= img->data[i][j]; k++)
				sum_of_frequencies += frequency[k]; // Calculating the sum

			// Calculating the result using the formula
			result = (double)sum_of_frequencies / area * img->maximum_value;
			clamp(&result);
			img->data[i][j] = (unsigned char)round(result);
		}
	}

	free(frequency); // Freeing the frequency array
	printf("Equalize done\n");
}

// Creating the kernels needed for the apply command
const double edge_kernel[KERNEL_SIZE][KERNEL_SIZE] = {
	{-1, -1, -1},
	{-1, 8, -1},
	{-1, -1, -1}
};

const double sharpen_kernel[KERNEL_SIZE][KERNEL_SIZE] = {
	{0, -1, 0},
	{-1, 5, -1},
	{0, -1, 0}
};

const double blur_kernel[KERNEL_SIZE][KERNEL_SIZE] = {
	{1, 1, 1},
	{1, 1, 1},
	{1, 1, 1}
};

const double gaussian_blur_kernel[KERNEL_SIZE][KERNEL_SIZE] = {
	{1, 2, 1},
	{2, 4, 2},
	{1, 2, 1}
};

// Function for setting the kernel to the one we need (from source_kernel)
void set_kernel(double **kernel,
				const double source_kernel[KERNEL_SIZE][KERNEL_SIZE])
{
	for (int i = 0; i < KERNEL_SIZE; i++)
		for (int j = 0; j < KERNEL_SIZE; j++)
			kernel[i][j] = source_kernel[i][j];
}

// Function for creating the kernel according to the parameter
// kernel_type tells us which kernel we have and helps us to compute the sum
// according to the formulas
void create_kernel(int_image *img, char *parameter, double **kernel,
				   int *valid)
{
	if (strcmp(parameter, "EDGE") == 0) {
		// Setting the kernel to the given one
		set_kernel(kernel, edge_kernel);

		// The parameter is ok
		*valid = 1;

		// Kernel type - 1 for edge kernel
		img->kernel_type = 1; // kernel type 1 indicates edge kernel
	} else if (strcmp(parameter, "SHARPEN") == 0) {
		set_kernel(kernel, sharpen_kernel);
		*valid = 1;
		img->kernel_type = 2; // kernel type 2 indicates sharpen kernel
	} else if (strcmp(parameter, "BLUR") == 0) {
		set_kernel(kernel, blur_kernel);
		*valid = 1;
		img->kernel_type = 3; // kernel type 3 indicates blur kernel
	} else if (strcmp(parameter, "GAUSSIAN_BLUR") == 0) {
		set_kernel(kernel, gaussian_blur_kernel);
		*valid = 1;
		img->kernel_type = 4; // kernel type 4 indicates gaussian blur kernel
	}
}

// Function for the errors that can appear in the apply command
int verify_apply(int_image *img, char *parameter)
{
	// Checking if an image is loaded
	if (load_error(img))
		return 1;

	// Checking if the parameter is valid
	if (!parameter || strlen(parameter) == 0) {
		printf("Invalid command\n");
		return 1;
	}

	// Checking if the image is RGB
	if (img->type == 2 || img->type == 5) {
		printf("Easy, Charlie Chaplin\n");
		return 1;
	}
	return 0;
}

// Function designed for the "APPLY" command
void apply(int_image *img, char *parameter)
{
	// If there are no errors we can continue
	if (verify_apply(img, parameter))
		return;

	// Allocating the kernel
	double **kernel = malloc(3 * sizeof(double *));
	for (int i = 0; i < 3; i++)
		kernel[i] = malloc(3 * sizeof(double));

	int valid = 0;
	create_kernel(img, parameter, kernel, &valid);
	// If the parameter is not a valid one, we free the memory and return
	if (valid == 0) {
		printf("APPLY parameter invalid\n");
		free_double_matrix(kernel, 3);
		return;
	}
	// Allocation of a temporary matrix
	RGB **tmp_matrix = malloc(img->height * sizeof(RGB *));
	for (int i = 0; i < img->height; i++)
		tmp_matrix[i] = malloc(img->width * sizeof(RGB));
	defensive_programming_rgb(img, tmp_matrix, img->height, 0);

	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			// If the pixel is on the edge, we copy it into the temporary matrix
			if (i < 1 || i > img->height - 2 || j < 1 || j > img->width - 2) {
				tmp_matrix[i][j] = img->rgb_data[i][j];
				continue;
			} else { // Computing the sum according to the kernel method
				double sum_red = 0, sum_green = 0, sum_blue = 0;
				for (int k = 0; k < 3; k++) {
					for (int l = 0; l < 3; l++) {
						sum_red += img->rgb_data[i + k - 1]
						[j + l - 1].red * kernel[k][l];
						sum_green += img->rgb_data[i + k - 1]
						[j + l - 1].green * kernel[k][l];
						sum_blue += img->rgb_data[i + k - 1]
						[j + l - 1].blue * kernel[k][l];
					}
				}
				if (img->kernel_type == 3) { // If the kernel is blur
					sum_red /= 9;
					sum_green /= 9;
					sum_blue /= 9;
				} else if (img->kernel_type == 4) { //If the kernel is gaussian
					sum_red /= 16;
					sum_green /= 16;
					sum_blue /= 16;
				}
				clamp(&sum_red);
				clamp(&sum_green);
				clamp(&sum_blue);
				// Copying the result into the temporary matrix
				tmp_matrix[i][j].red = (unsigned char)round(sum_red);
				tmp_matrix[i][j].green = (unsigned char)round(sum_green);
				tmp_matrix[i][j].blue = (unsigned char)round(sum_blue);
			}
		}
	}

	// Copying the temporary matrix into the image
	copy_rgb_matrix(tmp_matrix, img->rgb_data,
					img->y1, img->y2, img->x1, img->x2);

	free_double_matrix(kernel, 3); // Freeing the kernel
	free_rgb_matrix(tmp_matrix, img->height); // Freeing the temporary matrix

	printf("APPLY %s done\n", parameter);
}

// Function for rotating a grayscale selection
void rotate_gray_selection(int_image *img, int angle)
{
	// Verifying if the selection is squared
	if ((img->x2 - img->x1) != (img->y2 - img->y1)) {
		printf("The selection must be square\n");
		return;
	}

	int dimmension = img->x2 - img->x1;
	// If the rotation has the following angles, the img stays the same
	if (angle == 0 || angle == 360 || angle == -360) {
		printf("Rotated %d\n", angle);
		return;
	}

	// Allocation of a tmpmatrix
	unsigned char **tmpmatrix =
	malloc((img->y2 - img->y1) * sizeof(unsigned char *));
	for (int i = 0; i < (img->y2 - img->y1); i++)
		tmpmatrix[i] = malloc((img->x2 - img->x1) * sizeof(unsigned char));
	defensive_programming_gray(img, tmpmatrix, (img->y2 - img->y1), 0);

	// The rotations
	// A rotation to the right with 90 degrees is equal with a rotation to
	// the left with 270 degrees
	if (angle == 90 || angle == -270) {
		for (int i = 0; i < dimmension; i++) {
			for (int j = 0; j < dimmension; j++) {
				tmpmatrix[j][dimmension - 1 - i] =
				img->data[img->y1 + i][img->x1 + j];
			}
		}
		printf("Rotated %d\n", angle);
		// A rotation with 180 degrees is the same no matter the direction
	} else if (angle == 180 || angle == -180) {
		for (int i = 0; i < dimmension; i++)
			for (int j = 0; j < dimmension; j++)
				tmpmatrix[dimmension - 1 - i][dimmension - 1 - j] =
				img->data[img->y1 + i][img->x1 + j];
		printf("Rotated %d\n", angle);
		// The same as the first one but this time to the left
	} else if (angle == 270 || angle == -90) {
		for (int i = 0; i < (img->y2 - img->y1); i++)
			for (int j = 0; j < (img->x2 - img->x1); j++)
				tmpmatrix[dimmension - 1 - j][i] =
				img->data[img->y1 + i][img->x1 + j];
		printf("Rotated %d\n", angle);
	}

	// Copying the tmpmatrix into the image
	for (int i = 0; i < (img->y2 - img->y1); i++)
		for (int j = 0; j < (img->x2 - img->x1); j++)
			img->data[img->y1 + i][img->x1 + j] = tmpmatrix[i][j];

	// Freeing the tmpmatrix
	free_unsigned_char_matrix(tmpmatrix, (img->y2 - img->y1));
}

// Function for rotating a RGB selection
void rotate_color_selection(int_image *img, int angle)
{
	// Verifying if the selection is squared
	if ((img->x2 - img->x1) != (img->y2 - img->y1)) {
		printf("The selection must be square\n");
		return;
	}

	int dimmension = img->x2 - img->x1;

	// If the rotation has the following angles, the img stays the same
	if (angle == 0 || angle == 360 || angle == -360) {
		printf("Rotated %d\n", angle);
		return;
	}

	// Allocation of a tmpmatrix
	RGB **tmp_matrix = malloc(dimmension * sizeof(RGB *));
	for (int i = 0; i < dimmension; i++)
		tmp_matrix[i] = malloc(dimmension * sizeof(RGB));
	defensive_programming_rgb(img, tmp_matrix, dimmension, 0);

	// The same as for the grayscale selection but now its RGB
	if (angle == 90 || angle == -270) {
		for (int i = 0; i < dimmension; i++) {
			for (int j = 0; j < dimmension; j++) {
				tmp_matrix[j][dimmension - 1 - i] =
				img->rgb_data[img->y1 + i][img->x1 + j];
			}
		}
	} else if (angle == 180 || angle == -180) {
		for (int i = 0; i < dimmension; i++)
			for (int j = 0; j < dimmension; j++)
				tmp_matrix[dimmension - 1 - i][dimmension - 1 - j] =
				img->rgb_data[img->y1 + i][img->x1 + j];
	} else if (angle == 270 || angle == -90) {
		for (int i = 0; i < dimmension; i++)
			for (int j = 0; j < dimmension; j++)
				tmp_matrix[dimmension - 1 - j][i] =
				img->rgb_data[img->y1 + i][img->x1 + j];
	}

	for (int i = 0; i < dimmension; i++)
		for (int j = 0; j < dimmension; j++)
			img->rgb_data[img->y1 + i][img->x1 + j] = tmp_matrix[i][j];

	free_rgb_matrix(tmp_matrix, dimmension);

	printf("Rotated %d\n", angle);
}

// Function for updating the image parameters so the rotation operation
// is done correctly and
// the selection, height and width are updated according to the new rotated img
//THIS FUNCTION IS USED ONLY FOR THE ROTATION OF THE ENTIRE IMAGE
//THIS FUNCTION IS USED FOR THE ROTATION AT +-90 DEGREES OR +-270 DEGREES
//because for 0, 180 and 360 degrees the height and width remain the same
void update_rotated_img(int_image *img)
{
	// Swaping the height and width
	int new_width = img->height;
	int new_height = img->width;
	img->width = new_width;
	img->height = new_height;

	// Swaping the coordinates of the selection
	int new_x2 = img->y2;
	int new_x1 = img->y1;
	int new_y2 = img->x2;
	int new_y1 = img->x1;
	img->x1 = new_x1;
	img->x2 = new_x2;
	img->y1 = new_y1;
	img->y2 = new_y2;
}

// Function for rotating a grayscale image
void rotate_gray_image(int_image *img, int angle)
{
	// If we need a rotation to the following angles the img stays the same
	if (angle == 0 || angle == 360 || angle == -360) {
		printf("Rotated %d\n", angle);
		return;
	}

	// Rotating 90 degrees to the right
	if (angle == 90 || angle == -270) {
		// Allocating a tmp matrix
		unsigned char **tmp_matrix =
		malloc(img->width * sizeof(unsigned char *));
		for (int i = 0; i < img->width; i++)
			tmp_matrix[i] = malloc(img->height * sizeof(unsigned char));
		defensive_programming_gray(img, tmp_matrix, img->width, 0);

		// Copying the image into the tmp matrix
		for (int i = 0; i < img->height; i++)
			for (int j = 0; j < img->width; j++)
				tmp_matrix[j][img->height - 1 - i] = img->data[i][j];

		//Freeing the initial image
		free_image(img);
		//Updating the height, width and the selection according to the new img
		update_rotated_img(img);

		// Allocating the image again with the updated parameters
		img->data = malloc(img->height * sizeof(unsigned char *));
		for (int i = 0; i < img->height; i++)
			img->data[i] = malloc(img->width * sizeof(unsigned char));
		defensive_programming_gray(img, img->data, img->height, 1);

		//Copying the values from the tmp_matrix in the image
		copy_matrix(tmp_matrix, img->data, 0, img->height, 0, img->width);

		free_unsigned_char_matrix(tmp_matrix, img->height);

		// if the matrix is rotated at 180 degrees its height and width remain
		// the same
		// so the algorithm is exactly the same as for the selection
	} else if (angle == 180 || angle == -180) {
		unsigned char **tmp_matrix =
		malloc(img->height * sizeof(unsigned char *));
		for (int i = 0; i < img->height; i++)
			tmp_matrix[i] = malloc(img->width * sizeof(unsigned char));
		defensive_programming_gray(img, tmp_matrix, img->height, 0);

		for (int i = 0; i < img->height; i++)
			for (int j = 0; j < img->width; j++)
				tmp_matrix[img->height - 1 - i][img->width - 1 - j] =
				img->data[i][j];

		// Copying the values from the tmp_matrix in the image
		copy_matrix(tmp_matrix, img->data, 0, img->height, 0, img->width);

		free_unsigned_char_matrix(tmp_matrix, img->height);
	} else if (angle == 270 || angle == -90) {
		unsigned char **tmp_matrix =
		malloc(img->width * sizeof(unsigned char *));
		for (int i = 0; i < img->width; i++)
			tmp_matrix[i] = malloc(img->height * sizeof(unsigned char));
		defensive_programming_gray(img, tmp_matrix, img->width, 0);

		for (int i = 0; i < img->height; i++)
			for (int j = 0; j < img->width; j++)
				tmp_matrix[img->width - 1 - j][i] = img->data[i][j];

		free_image(img);
		update_rotated_img(img);

		img->data = malloc(img->height * sizeof(unsigned char *));
		for (int i = 0; i < img->height; i++)
			img->data[i] = malloc(img->width * sizeof(unsigned char));
		defensive_programming_gray(img, img->data, img->height, 1);

		copy_matrix(tmp_matrix, img->data, 0, img->height, 0, img->width);

		free_unsigned_char_matrix(tmp_matrix, img->height);
	}
	printf("Rotated %d\n", angle);
}

// Function for rotating a RGB image
void rotate_color_image(int_image *img, int angle)
{
	//if the rotation is 0, 360 or -360 degrees the img stays the same
	if (angle == 0 || angle == 360 || angle == -360) {
		printf("Rotated %d\n", angle);
		return;
	}
	//The rotation to the right with 90 == rotation to the left with 270
	if (angle == 90 || angle == -270) {
		RGB **tmp_matrix = malloc(img->width * sizeof(RGB *));
		for (int i = 0; i < img->width; i++)
			tmp_matrix[i] = malloc(img->height * sizeof(RGB));
		defensive_programming_rgb(img, tmp_matrix, img->width, 0);

		for (int i = 0; i < img->height; i++)
			for (int j = 0; j < img->width; j++)
				tmp_matrix[j][img->height - 1 - i] = img->rgb_data[i][j];

		free_image(img);
		update_rotated_img(img);

		img->rgb_data = malloc(img->height * sizeof(RGB *));
		for (int i = 0; i < img->height; i++)
			img->rgb_data[i] = malloc(img->width * sizeof(RGB));
		defensive_programming_rgb(img, img->rgb_data, img->height, 1);

		// Copying the values from the tmp_matrix in the image
		copy_rgb_matrix(tmp_matrix, img->rgb_data,
						0, img->height, 0, img->width);

		free_rgb_matrix(tmp_matrix, img->height);
	} else if (angle == 180 || angle == -180) {
		RGB **tmp_matrix = malloc(img->height * sizeof(RGB *));
		for (int i = 0; i < img->height; i++)
			tmp_matrix[i] = malloc(img->width * sizeof(RGB));
		defensive_programming_rgb(img, tmp_matrix, img->height, 0);

		for (int i = 0; i < img->height; i++)
			for (int j = 0; j < img->width; j++)
				tmp_matrix[img->height - 1 - i][img->width - 1 - j] =
				img->rgb_data[i][j];

		copy_rgb_matrix(tmp_matrix, img->rgb_data,
						0, img->height, 0, img->width);

		free_rgb_matrix(tmp_matrix, img->height);
	} else if (angle == 270 || angle == -90) {
		RGB **tmp_matrix = malloc(img->width * sizeof(RGB *));
		for (int i = 0; i < img->width; i++)
			tmp_matrix[i] = malloc(img->height * sizeof(RGB));
		defensive_programming_rgb(img, tmp_matrix, img->width, 0);

		for (int i = 0; i < img->height; i++)
			for (int j = 0; j < img->width; j++)
				tmp_matrix[img->width - 1 - j][i] = img->rgb_data[i][j];

		free_image(img);
		update_rotated_img(img);

		img->rgb_data = malloc(img->height * sizeof(RGB *));
		for (int i = 0; i < img->height; i++)
			img->rgb_data[i] = malloc(img->width * sizeof(RGB));
		defensive_programming_rgb(img, img->rgb_data, img->height, 1);

		copy_rgb_matrix(tmp_matrix, img->rgb_data,
						0, img->height, 0, img->width);

		free_rgb_matrix(tmp_matrix, img->height);
	}
	printf("Rotated %d\n", angle);
}

// Function for the "ROTATE" command
void rotate(int_image *img, int angle)
{
	// Checking if an image is loaded
	if (load_error(img))
		return;

	// Checking if the angle is valid
	if (angle % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}

	// Checking if we rotate the whole image or just a selection
	if (img->x1 == 0 && img->x2 == img->width &&
		img->y1 == 0 && img->y2 == img->height) { // If we rotate the whole img
		if (img->type == 3 || img->type == 6)
			rotate_color_image(img, angle); // Rotating a RGB image
		else if (img->type == 2 || img->type == 5)
			rotate_gray_image(img, angle); // Rotating a grayscale image
	} else { // If we rotate a selection
		if (img->type == 3 || img->type == 6)
			rotate_color_selection(img, angle); // Rotating a RGB selection
		else if (img->type == 2 || img->type == 5)
			rotate_gray_selection(img, angle); // Rotating a gray selection
	}
}

// Function for the exit command
void exit_command(int_image *img, char *command, char *filename,
				  char *parameter)
{
	// freeing the memory
	free(command);
	free(filename);
	free(parameter);

	// if an image is loaded freeing the image
	if (img->loaded == 1)
		free_image(img);
	free(img);
}
