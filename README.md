## Dobre Andrei-Teodor 314CA

# IMAGE EDITOR

I used 2 structures in this program: 'RGB' structure and 'int_image' structure.
- The RGB structure represents the pixel of a RGB image and it is used to
declare the data of the image if the loaded image is RGB (it stores the red,
green and blue components of a RGB pixel).
- The 'int_image' structure represents an image, storing information about its:
 * type (p2, p3, p5, p6)
 * dimmensions (height and width)
 * coordinates of the selected region (x1, y1, x2, y2)
 * if an image is actually loaded
 * kernel_type ( for the apply command)
 * its data as such :
  - For gray images, the data matrix of type unsigned char
  - For RGB images, the rgb_data matrix of type RGB, the structure that we
  specifically designed for this case

The project is divided into the following files:
- memory.h and memory.c :
these files handle memory allocation and deallocation for matrices and arrays
the structures 'int_image' and 'RGB' are also defined in memory.c
- commands.h and commands.c:
these files contains all the functions that are needed for the commands to work
- image_editor.c: this file includes the main function which manages the
interaction with the user and calling the function needed for the commands
to take place

The program is structured around the main function which manages input data
and image processing. This function manages a command line interface for users
to interact with the image editor. I utilize dynamic memory allocation to
handle image data and user commands .

Afterward I create an infinite loop which works as follows:

The user can interact with the program until the 'EXIT' command is read from
stdin, which implies exiting the while loop, after ,of course, freeing all the
allocated memory.

### Command 'LOAD <file>'
I call the 'load_image' command in which:

1. Error checking
I treated all the errors that could have came from incorrect user usage or
failing to open the file and also made sure to free the memory of the previous
image in the case which the user tries to upload a new image when one is
already loaded.

2. Reading metadata
The function reads the image type, dimensions, and maximum pixel value from
the file.
It uses the ignore_comment function to skip any comments before each line.
I used fseek to go back to the beginning of the image after ignoring comments.

3. Image loading
Depending on the image type, the function allocates memory and reads the img:
For gray images (type 2 and 5), memory is allocated for img->data, and pixel
values are read either in ASCII or binary format.
For RGB images (type 3 and 6), memory is allocated for img->rgb_data, and RGB
pixel values are read either in ASCII or binary format.
Also, after loading the image, the function updates the selected area to cover
the entire image.

### Command 'SELECT <x1> <y1> <x2> <y2>'
I call the select_command function which works as follows:

Because I used the structure to hold the coordinates of the selected region
this opreation becomes trivial:

1. Error Checking
Checking if an image is actually loaded
Checking if the selection coordinates are smaller than the height or width
of the image
Validates the given selection coordinates and ensures they are in the correct
order before updating them in the structure (for which i used aux variables)

2. Updating selection coordinates
After ensuring that the coordinates are valid i just update them in the
structure

### Command 'SELECT ALL'
Calling the select_all function which:

Verifies that an image is loaded then updates the selection coordinates
in the struct to the full image

### Command ' HISTOGRAM <x> <y> '

1. Error checking
Checking for various user input errors such as incorrect parameter count
or incorrect values
Checking if an image is actually loaded
Checking if the image is grayscale (cant perform histogram on a RGB image)

2. Implementation
I allocate an array histogram in which i store each pixel's frequency
I divide the pixel into bins
I calculate the total frequency in each bin
I determine the max freq - max stars
I show the histogram

### Command 'EQUALIZE'

1. Error checking

2. Implementation
I allocate the frequency vector in which i store the cumulative frequency
for each pixel in the image.
Then, I apply the equalization formula to each pixel in the image based on
the cumulative frequency and the maximum pixel value.
I also make sure that the calculated result is within the valid range
[0, img->maximum_value] by clamping it.

### Command 'ROTATE <angle>'
I have split the rotate command in 5 big functions:
rotate_gray_image, rotate_color_image, rotate_gray_selection
rotate_color_selection and rotate.

The command is structured around the 'rotate' function which is the main entry
point for rotating images. It checks if an image is loaded, if the rotation
angle is valid, and whether to rotate the entire image or just a selection.
It then calls the appropriate rotation function based on the image type and
what the user wants to rotate (selection or whole image).

All of the 4 rotation functions work on the same principle:

1. Checking if the rotation angle is supported
2. For 0, 360, -360 degrees the image stays the same
3. 90 degrees to the right = 270 to the left
4. 180 degrees to the right = 180 to the left
5. 270 degrees to the right = 90 to the left
For cases 3., 4. and 5. I :
- Allocate a temporary matrix in which I store the rotated image
based on some formulas and making sure the height and width are
updated accordingly
- Freeing the initial image and allocating a new one with the updated
width and heigth
- Copying the image data from the temporary matrix to the new allocated
image data
- Freeing the memory

### Command 'CROP'
Calling the 'crop' function which works as such:

I am checking the errors which can appear (not loaded an image, wrong input)

I have splitted the function into 2 cases:

- Cropping a gray image
- Cropping a RGB image

Both cases work on the same principle :
- Allocating a temporary image in which I store the values beetween the current
selection coordinates
- Freeing the old image
- Updating the height and width according to the current selection coordinatges
- Allocating the new image data with the new width and height
- Copying in the new allocated image data from the temporary matrix
- freeing the temporary matrix

### Command 'APPLY <parameter>'

- I define the kernel matrices for every operation
- The selected kernel is set and then I can continue applying the kernel
on the image (on each pixel) using convolution
- To avoid corrupting the image data during the application of the kernel,
a temporary matrix is allocated. The updated pixel values are computed and
stored in this temporary matrix before being copied back to the original image.
- The function traverses each pixel in the specified image region, computing a
weighted sum of neighboring pixels based on the kernel matrix. The result is
then used to update every pixel value.
- I also use kernel_type so I know when I should divide the sum (with 9 or 16)
respecting the formulas for the respective operations

### Command 'SAVE <filename> ascii'

- I treat the posible appearence of 'ascii' (which tells me if I should save
the image in ascii format) directly in the main function using sscanf

Then I call the save_image function accordingly (with ascii parameter == 0
if it is not present and with ascii parameter == 1 if it is present). The
function works as it follows:

1.File Opening:
The function attempts to open the specified file (outputfile) in write mode.
If the file opening fails, an appropriate error message is displayed.

2. File Type and Image Parameters:
The function correctly determines the image type (P2, P3, P5, or P6) based on
the image type (img->type). It adjusts the type representation for ASCII and
binary modes accordingly.

3. Printing the image metadata

4. Saving image data
- For ASCII mode (ascii is true), the function prints pixel values to the file.
For grayscale images (P2 or P5), it prints single-channel values, and for RGB
images (P3 or P6), it prints three channels (red, green, blue).

-  binary mode (ascii is false), the function uses fwrite to write pixel values
directly to the file. It handles both grayscale and RGB images, ensuring
correct representation and order of color channels.

5. File closure
After successfully writing the image data, the function closes the file.
