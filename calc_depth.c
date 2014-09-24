#include "calc_depth.h"
#include "utils.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static int squared_euclidean_distance(unsigned char *, unsigned char *, int, int);
static unsigned char *populate_feature_patch(int, int, int, unsigned char*);
static unsigned char *check_bounds(unsigned char *, int, int, int, int, int); //Checks if a feature's pixels will be in bounds
static int check_within_image(int, int, int, int, int);
static int return_feature_bounds(int, int, int);
static unsigned char scan_right_image(unsigned char *, unsigned char *, int, int, int, int, int, int, int, int);

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
    for(int i  = 0; i < image_height; i++) {
        height_offset = image_width * i; //Since the picture is described as a contiguous memory block, we must offset to help it emulate a 2D array
    	for(int j = 0; j < image_width; j++) {
            width_offset = height_offset + j;
            
            left_feature_patch = check_bounds(left, width_offset, feature_patch_width, feature_patch_height, image_width, image_height); //Get feature patch for pixel at position width_offset
            
            *(depth_map + counter) = (!left_feature_patch) ? 0 : scan_right_image(right,           //Either return a depth map of 0.
                                                                                  left_feature_patch,
                                                                                  width_offset,
                                                                                  feature_patch_width,
                                                                                  feature_patch_height,
                                                                                  image_width,
                                                                                  image_height,
                                                                                  search_field_width,
                                                                                  search_field_height,
                                                                                  maximum_displacement); //If Null, this means the left image feature was not in the bounds and thus, should have a displacement of 0
            counter++;
        }
        if(!left_feature_patch)
            free(left_feature_patch); //Free left_feature for next iteration if we malloced a left feature.
    }
}

unsigned char scan_right_image(unsigned char *image,
                               unsigned char *left_feature,
                               int pixel_offset,
                               int feature_patch_width,
                               int feature_patch_height,
                               int image_width,
                               int image_height,
                               int search_field_width,
                               int search_field_height,
                               int max_displacement) {
    
	int search_area_offset = (pixel_offset - return_feature_bounds(search_field_width,
                                                                   search_field_height,
                                                                   image_width)); //Find offset of starting position of search area. This is create a search field around the pixel.
    
	int most_similar_euclid_distance = INT_MAX, distance_to_examine;
	int most_similar_nordis = INT_MAX; //Keeps most similar distance at any given time.
	int current_offset, height_offset; //Will contain the offset
    
	int left_pixel_y = pixel_offset / image_width;
	int left_pixel_x = pixel_offset  - (left_pixel_y * image_width);
    
	int current_y, current_x, dy, dx, nordis; //Fields to store most similar normal displacement and the fields required to calculate it.
	unsigned char *feature_to_examine; //Points to most similar feature at any given time and the feature to be examined.

	
	for(int i = 0; i < search_field_height; i++) {
	
        height_offset = search_area_offset + image_width * i;
	current_offset = height_offset; 
        for(int j = 0; j < search_field_width; j++, current_offset++) {
            
	     if(!(current_offset >= image_width * ((height_offset / image_width + i + 1)) || 
	       (current_offset < (pixel_offset - ((search_field_height / 2) * image_width))))) { //Account for feature corner going off of right end and left
         
            int within_image = check_within_image(current_offset, feature_patch_width, feature_patch_height, image_width, image_height);
            feature_to_examine = (within_image) ? populate_feature_patch(feature_patch_height, feature_patch_width, current_offset, image) : NULL;
            
            if(feature_to_examine) { //If not null
                if((distance_to_examine = squared_euclidean_distance(feature_to_examine,
                                                                     left_feature,
                                                                     feature_patch_width,
                                                                     feature_patch_height)) <= most_similar_euclid_distance) { //Set distance_to_examine as the euclidean distance between left feature and the right feature and compare between the most_similar_feature.
                    
                    current_y = current_offset / image_width; //Calculate the x difference and y difference
                    current_x = current_offset - (current_y * image_width);
                    
                    dy = abs(current_y - left_pixel_y);
                    dx = abs(current_x - left_pixel_x);
                    
                    nordis = normalized_displacement(dx,dy, max_displacement);
                    
                    if((distance_to_examine == most_similar_euclid_distance) && (nordis < most_similar_nordis)) {
                        most_similar_nordis = nordis;
                    } else if (distance_to_examine != most_similar_euclid_distance) {
                        most_similar_euclid_distance = distance_to_examine;
                        most_similar_nordis = nordis;
                    }
                }
                free(feature_to_examine); //No need for feature anymore
            }
	  }
        } //End of for loop 

     }
    
	return most_similar_nordis; //max_displacement passed in as args.
    
}

unsigned char *check_bounds(unsigned char *image,
                            int pixel_offset,
                            int feature_patch_width,
                            int feature_patch_height,
                            int image_width,
                            int image_height) {
    
	int corner_offset = pixel_offset - return_feature_bounds(feature_patch_width, feature_patch_height, image_width); //Distance from image pointer (first pixel) to corner of the feature.
    
	int within_image = check_within_image(corner_offset, feature_patch_width, feature_patch_height, image_width, image_height);
	return (within_image) ? populate_feature_patch(feature_patch_height, feature_patch_width, corner_offset, image) : NULL; //Create the feature if the feature is within bounds.
    
}

/*
 Checks if a feature whose upper left corner at starting_point_offset is within the bounds of the image.
 */
int check_within_image(int starting_point_offset, int feature_patch_width, int feature_patch_height, int image_width, int image_height) {
    
	if(starting_point_offset < 0) { //If the corner itself is out of bounds, the entire feature will be out of bounds.
		return 0;
	}
    
	if(( ((starting_point_offset + feature_patch_width - 1) < ((starting_point_offset / image_width + 1)) * image_width) && //Essentially checking if adding the feature width will make it overlap to the next row. If so, this feature is out of bounds
        ((starting_point_offset + (feature_patch_width * feature_patch_height - 1) ) < (image_width * image_height )))) { //Essentially cheecking if the feature height is within bounds. If is within bounds. This means the @FIXME
        	return 1;
	}
	return 0;
}

int return_feature_bounds(int feature_patch_width, int feature_patch_height, int image_width) {
	return ((feature_patch_width / 2) + ((feature_patch_height / 2) * image_width));
}

/*
 Allocates a block of memory to create a feature for comparisons.
 */
unsigned char *populate_feature_patch(int feature_patch_height,
                                      int feature_patch_width,
                                      int offset,
                                      unsigned char *image) {
    
	unsigned char *starting_pos = image + offset; //Start at position give by args
	unsigned char *feature_patch = (unsigned char *)malloc(feature_patch_height * feature_patch_width * sizeof(unsigned char)); //Malloc feature patch
	
	if(!feature_patch)
		allocation_failed();
	
	int height_offset, width_offset;
    
	for(int i = 0; i < feature_patch_height; i++) {
	    height_offset = feature_patch_width * i;
	    for(int j = 0; j < feature_patch_width; j++) {
	        width_offset = height_offset + j;
            *(feature_patch + width_offset) = *(starting_pos + width_offset); //Set values from image into new feature patch
	    }
	}
	return feature_patch;
}

/*
 Returns the Squared Euclidean Distance for any two feature patches.
 */

int squared_euclidean_distance(unsigned char *feature_patch_one,
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
