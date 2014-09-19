/*
 * PROJ1-1: YOUR TASK A CODE HERE
 *
 * Feel free to define additional helper functions.
 */

#include "calc_depth.h"
#include "utils.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int squared_euclidean_distance(unsigned char *, unsigned char *, int, int);
int return_depth_map(unsigned char *);
unsigned char *populate_feature_patch(int, int, int, unsigned char*);
int check_bounds(unsigned char *, int, int, int); //Checks if a features pixels will be in bounds 

/* Implements the normalized displacement function */
unsigned char normalized_displacement(int dx, int dy,
        int maximum_displacement) {

    double squared_displacement = dx * dx + dy * dy;
    double normalized_displacement = round(255 * sqrt(squared_displacement) / sqrt(2 * maximum_displacement * maximum_displacement));
    return (unsigned char) normalized_displacement;

}

void calc_depth(unsigned char *depth_map, unsigned char *left,
        unsigned char *right, int image_width, int image_height,
        int feature_width, int feature_height, int maximum_displacement) {

    int feature_patch_height = 2 * feature_height + 1; //Set feature_patch dimensions
    int feature_patch_width = 2* feature_width + 1;


    int height_offset, width_offset;
    unsigned char *left_feature_patch //Will point to the relevant left feature patch during the iteration below

    for(int index_height = 0; i < feature_height; index_height++) {
	height_offset = feature_width * index_height; //Since the picture is described as a contiguous memory block, we must offset to help it emulate a 2D array
    	for(int index_width = 0; j < feature_width; index_width++) {
	    width_offset = height_offset + index_width; //Can think of as array[height_offset][width_offset]
	    left_feature_path = populate_feature_patch(feature_patch_height, feature_patch_width, width_offset, left); //Get feature path for pixel at position (height_offset, width_offset)
	                  	
	}
	free(left_feature_patch); //Free left_feature for next iteration 
    }

}

int check_bounds(unsigned char *image, 
		int pixel_offset, 
		int feature_patch_width, 
		int feature feature_patch_height) {

	unsigned char *pixel_to_examine = image + pixel_offset, 
	unsigned char *corner_of_feature = pixel_to_examine; //This pointer will point to the corner of the feature we will draw for the pixel in question 
	corner_of_feature -= ((feature_patch_width / 2) + ((feature_patch_height / 2) * feature_patch_width))); //Find diagonal of future feature.

}

unsigned char *populate_feature_patch(int feature_patch_height, 
		int feature_patch_width, 
		int offset,
		unsigned char *image) {

	unsigned char *starting_pos = image + offset; //Start at position give by args
	unsigned char *feature_patch = (unsigned char *)malloc(feature_patch_height * feature_patch_width * sizeof(unsigned char)); //Malloc feature patch
	int height_offset, width_offset;

	int new_feature_pos = 0; //Keeps index of new feature to copy in during iteration
	for(int i = 0; i < feature_patch_height; i++) {
	    height_offset = feature_patch_width * i;
	    for(int j = 0; j < feature_patch_width; j++) {
	        width_offset = height_offset + j;
		*(feature_patch + new_feature_pos) = *(starting_pos + width_offset); //Set values from image into new feature patch
		counter++;	
	    }	    
	}
	return feature_patch;
}

int squared_euclidean_distance(unsigned char *feature_patch_one, 
		unsigned char *feature_patch_two,
		int feature_patch_height,
		int feature_patch_width) {

    int distance_squared = 0;
    int height_offset, width_offset;
  	
    for(int i = 0; i < feature_patch_height; i++) {
	height_offset = feature_patch_width * i;
	for(int j = 0; j < feature_patch_height; j++) {
	    width_offset = height_offset + j;
	    distance_squared += pow((*(feature_patch_one + width_offset) - *(feature_patch_two + width_offset)), 2);	
	}
    }
	return distance_squared;
}

