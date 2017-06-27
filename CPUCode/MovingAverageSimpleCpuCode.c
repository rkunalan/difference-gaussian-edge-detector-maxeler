/**
 * MaxFile name: MovingAverageSimple
 * Using "Difference of Gaussian Edge Detector Algorithm" to an image to find the 
 * difference of gaussian edge detection of that image.
 * The input of the image is loaded and send it through DFE as a color image(cake.ppm)
 * and get the output of the image(final_cake.ppm) as a black and white.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"               // Includes .max files
#include <MaxSLiCInterface.h>	    // Simple Live CPU interface

// Load the specific image
void load_image(char *file_name, int **dest, int *width, int *height, int gray_scale) {
	// hack to make the buffer big enough
	char buffer[1000];
	FILE *file = fopen(file_name, "r");

	if (file == NULL) {
		printf("Error opening file %s.", file_name);
		exit(1);
	}
	fgets(buffer, 1000, file); // Type
	// assume no comment for now
	fscanf(file, "%d %d\n", width, height);
	fgets(buffer, 1000, file); // Max intensity

	*dest = malloc((*width) * (*height) * sizeof(int));

	int read_pixels = 0;
	int pixel = 0;
	int current_component = 0;

	for (int i = 0; i < ((*width) * (*height) * 3); i++) {
		int v;
		int got = fscanf(file, "%d", &v);
		if (got == 0) {
			printf("Unexpected end of file after reading %d color values", i);
			exit(1);
		}
		switch (current_component) {
		case 0:
			pixel = v;
			current_component++;
			break;
		case 1:
			pixel = (pixel << 8) | (v & 0xFF);
			current_component++;
			break;
		case 2:
			pixel = (pixel << 8) | (v & 0xFF);
			current_component = 0;
			if (gray_scale)
				pixel = ((pixel >> 16) & 0xff) * 3 / 10 + ((pixel >> 8) & 0xff)
						* 59 / 100 + ((pixel) & 0xff) * 11 / 100;
			(*dest)[read_pixels] = pixel;
			read_pixels++;
			break;
		}
	}
	fclose(file);
}

// Write the new image with specific algorithm
void write_image(char *file_name, int *data, int width, int height, int gray_scale) {
	FILE *file = fopen(file_name, "w");

	fprintf(file, "P3\n");
	fprintf(file, "%d %d\n", width, height);
	fprintf(file, "255\n");

	for (int i = 0; i < width * height; i++) {
		if (gray_scale) {
			data[i] = data[i] > 255 ? 255 : data[i];
			data[i] = data[i] < 0 ? 0 : data[i];
			for (int j = 0; j < 3; j++)
				fprintf(file, "%d\n", (int) data[i]);
		} else {
			fprintf(file, "%d\n", ((int) data[i]) >> 16);
			fprintf(file, "%d\n", (((int) data[i]) >> 8) & 0xff);
			fprintf(file, "%d\n", (((int) data[i])) & 0xff);
		}
	}
	fclose(file);
}

// main method
int main(void)
{
	printf("Loading given cake image(cake.ppm).\n");
	int32_t *input_image;
	int width = 0, height = 0;
	load_image("cake.ppm", &input_image, &width, &height, 0);

	int data_size = width * height * sizeof(int);
	// Allocate a buffer for the output image
	int32_t * output_image = malloc(data_size);

	printf("Running Difference of Gaussian Edge Detector Algorithm in Kernel.\n");
	MovingAverageSimple(width * height, input_image, output_image);

	printf("Saving new image using Difference of Gaussian Edge Detector algorithm.\n");
	write_image("final_cake.ppm", output_image, width, height, 0);

	printf("Created final_cake.ppm image in CPUCode directory based on the Difference of Gaussian Edge Detector algorithm.\nNow Exiting from the kernel.\n");
	return 0;
}

