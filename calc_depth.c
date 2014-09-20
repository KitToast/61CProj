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

static int squared_euclidean_distance(unsigned char *, unsigned char *, int, int);
static int return_depth_map(unsigned char *);
static unsigned char *populate_feature_patch(int, int, int, unsigned char*);
static unsigned char *check_bounds(unsigned char *, int, int, int, int, int); //Checks if a features pixels will be in bounds 
static int return_feature_diagonal(int , int);
static unsigned char scan_right_image(unsigned char *, unsigned char *, int, int, int, int, int, int);

/* Implements the normalized displacement function */
static unsigned char normalized_displacement(int dx, int dy,
        int maximum_displacement) {

    double squared_displacement = dx * dx + dy * dy;
    double normalized_displacement = round(255 * sqrt(squared_displacement) / sqrt(2 * maximum_displacement * maximum_displacement));
    return (unsigned char) normalized_displacement;

}

void calc_depth(unsigned char *depth_map, unsigned char *left,
        unsigned char *right, int image_width, int image_height,
        int feature_width, int feature_height, int maximum_displacement) {

    int feature_patch_height = 2 * feature_height + 1; //Set feature_patch dimensions
    int feature_patch_width = 2 * feature_width + 1;

    int search_field_width = feature_patch_width + (2 * maximum_displacement); //Define search area dimensions
    int search_field_height = feature_patch_height + (2 * maximum_displacement); 

    int height_offset, width_offset;
    unsigned char *left_feature_patch; //Will point to the relevant left feature patch during the iteration below
        
    int counter = 0; //Counter for the depth map. 
    for(int index_height = 0; i < feature_height; index_height++) {
	height_offset = feature_width * index_height; //Since the picture is described as a contiguous memory block, we must offset to help it emulate a 2D array
    	for(int index_width = 0; j < feature_width; index_width++) {
	    width_offset = height_offset + index_width; //Can think of as array[height_offset][width_offset]
	    left_feature_patch = check_bounds(left, width_offset, feature_patch_width, feature_patch_height, image_width, image_height); //Get feature path for pixel at position (height_offset, width_offset)
	    *(depth_map + counter) = (!left_feature_patch) ? 0 : scan_right_image(right, 
										 left_feature_patch,
										 width_offset,
										 feature_patch_width,
										 feature_patch_height,
										 image_width,
										 image_height,
										 search_field_width,
									         search_filed_height,
										 maximum_displacement); //If Null, this means the left image feature was not in the bounds and thus, should have a displacement of 0   
	    counter++;	                  	
	}
	if(!left_feature_path) free(left_feature_patch); //Free left_feature for next iteration if we malloced a left feature.     
    }
}

static unsigned char scan_right_image(unsigned char *image,
		unsigned char *left_feature,
		int pixel_offset, 
		int feature_patch_width, 
		int feature_patch_height, 
		int image_width, 
		int image_height,
		int search_field_width,
		int search_field_height,
		int max_displacement) {
	unsigned char *pixel = image + pixel_offset;
	int search_diagonal_offset = return_feature_bounds(search_field_width, search_field_height); //Find offset of starting position of search area
	unsigned char *search_field_start = image + search_diagonal_offset;

	int most_similar_distance, height_offset, width_offset, distance_to_examine; //Keeps most similar distance at any given time.
	int most_similar_offset, current_offset; //Will contain the offset  
	unsigned char feature_to_examine; //Points to most similar feature at any given time and the feature to be examined.
	
	for(int i = 0; i < search_field_height; i++) {
	   height_offset = search_field_width * i;
	   for(int j = 0; j < search_field_width; j++) {
	       width_offset = height_offset + j;
	       current_offset = search_diagonal_offset + width_offset;
               feature_to_examine = check_bounds(image, 
						 current_offset,
						 feature_patch_width,
					         feature_patch_height,
						 image_width,
						 image_height);
	       if(feature_to_examine) { //If not null
	           if((distance_to_examine = squared_euclidean_distance(feature_to_examine, 
					         left_feature,
						 feature_patch_width, 
						 feature_patch_height)) < most_simiar_distance) {
		       most_similar_distance  = distance_to_examine;
		       most_similar_offset = current_offset;
		   } 
		       free(feature_to_examine); //No need for feature anymore
	       }  
	   }

	   int current_y = current_offset / image_width;
	   int current_x = current_offset - current_y * image_width;

	   int left_pixel_y = pixel_offset / image_width;
	   int left_pixel_x = pixel_offset  - left_pixel_y * image_width;

	   int dy = abs(current_y - left_pixel_y);
	   int dx = abs(current_x - left_pixel_x);

	  return normalized_displacement(dx, dy, max_displacement);
	
}

static unsigned char *check_bounds(unsigned char *image, 
		int pixel_offset, 
		int feature_patch_width, 
		int feature_patch_height,
		int image_width
		int image_height) {

	unsigned char *pixel_to_examine = image + pixel_offset,  //Find pixel in question first 
	unsigned char *corner_of_feature = pixel_to_examine; //This pointer will point to the corner of the feature we will draw for the pixel in question 
	int diagonal_offset = return_feature_bounds(feature_patch_width, feature_patch_height); //find how many places to decrease from position.
	
	if((pixel_offset - diagonal_offset) < 0) { //Change this
		return NULL:
	}

	if(( ((diagonal_offset + feature_patch_width - 1) < ((offset/width + 1)) * width) &&
	     ((diagonal_offset + feature_patch_height * feature_patch_width) < (image_width * image_height)))) {
		return populate_feature(feature_patch_height, feature_patch_width, diagonal_offset, image);		
	}
	return NULL;
}

static int return_feature_bounds(int feature_patch_width, int feature_patch_height) {
	return ((feature_patch_width / 2) + ((feature_patch_height / 2) * feature_patch_width)); //Find diagonal of future feature.
}

static unsigned char *populate_feature_patch(int feature_patch_height, 
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
		new_feature_pos++;	
	    }	    
	}
	return feature_patch;
}

static int squared_euclidean_distance(unsigned char *feature_patch_one, 
		unsigned char *feature_patch_two,
		int feature_patch_width,
		int feature_patch_height) {

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

