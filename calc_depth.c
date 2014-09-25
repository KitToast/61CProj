#include "calc_depth.h"
#include "utils.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>


/* Implements the normalized displacement function */
static unsigned char normalized_displacement(int dx, int dy,
                                             int maximum_displacement) {
    
    double squared_displacement = dx * dx + dy * dy;
    double normalized_displacement = round(255 * sqrt(squared_displacement) / sqrt(2 * maximum_displacement * maximum_displacement));
    return (unsigned char) normalized_displacement;
    
}

static int coord_to_offset(int height, int width, int image_width);
static int check_within_image(int height, int width, int feature_patch_width, int feature_height, int image_width, int image_height);
static unsigned char *check_bounds(unsigned char *image, int height, int width, int feature_patch_width, int feature_height, int image_width, int image_height); 
static unsigned char *populate_feature_patch(int feature_patch_height, int feature_patch_width, int image_width, int offset, unsigned char* image);
static int squared_euclidean_distance(unsigned char *feature_patch_one, unsigned char *feature_patch_two, int feature_patch_width, int feature_height);
static unsigned char scan_right_image(unsigned char *image, unsigned char *left_feature, int height, int width, int feature_height, int feature_patch_height, int image_width, int image_height, int max_displacement);

int coord_to_offset(int height, int width, int image_width) { //calculates the offset from the beginning of the image
	return (height * image_width) + width;
}



void calc_depth(unsigned char *depth_map, unsigned char *left,
                unsigned char *right, int image_width, int image_height,
                int feature_width, int feature_height, int maximum_displacement) {
    
    int feature_patch_height = 2 * feature_height + 1; //Set feature_patch dimensions
    int feature_patch_width = 2 * feature_width + 1;
    
    unsigned char *left_feature_patch; //Will point to the relevant left feature patch during the iteration below
    
    int counter = 0; //Counter for the depth map.
    for(int i  = 0; i < image_height; i++) {
    	for(int j = 0; j < image_width; j++) {
            
            left_feature_patch = check_bounds(left, i, j, feature_patch_width, feature_patch_height, image_width, image_height); //Get feature patch for pixel at position width_offset
            
            *(depth_map + counter) = (!left_feature_patch) ? 0 : scan_right_image(right,           //Either return a depth map of 0.
                                                                                  left_feature_patch,
                                                                                  i,
                                                                                  j,
                                                                                  feature_patch_width,
                                                                                  feature_patch_height,
                                                                                  image_width,
                                                                                  image_height,
                                                                                  maximum_displacement); //If Null, this means the left image feature was not in the bounds and thus, should have a displacement of 0
            counter++;
        }
        if(!left_feature_patch)
            free(left_feature_patch); //Free left_feature for next iteration if we malloced a left feature.
    }
}


unsigned char *check_bounds(unsigned char *image,
                            int height,
                            int width,
                            int feature_patch_width,
                            int feature_patch_height,
                            int image_width,
                            int image_height) {
    

	int corner_offset = coord_to_offset(height - (feature_patch_height / 2), //Should be only time corner is used.
										width - (feature_patch_width / 2),
										image_width); 
    
	int within_image = check_within_image(height, width, feature_patch_width, feature_patch_height, image_width, image_height); //Checks if creating a feature centered around pixel at (height, width) work
	return (within_image) ? populate_feature_patch(feature_patch_height, feature_patch_width, image_width, corner_offset, image) : NULL; //Create the feature if the feature is within bounds.
 }

unsigned char scan_right_image(unsigned char *image,
                               unsigned char *left_feature,
                               int height,
                               int width,
                               int feature_patch_width,
                               int feature_patch_height,
                               int image_width,
                               int image_height,
                               int max_displacement) {
    

    int length_of_max_dis_box = 2 * max_displacement + 1;

    int corner_coord_y = height - length_of_max_dis_box / 2; //Use corner for the box created using 2*max_displacement to test.
    int corner_coord_x =  width - length_of_max_dis_box / 2;

    int current_coord_y = corner_coord_y;
    int current_coord_x = corner_coord_x;
    
	int most_similar_euclid_distance = INT_MAX, distance_to_examine;
	int most_similar_nordis = INT_MAX; //Keeps most similar distance at any given time.
    
	int dy, dx, nordis; //Fields to store most similar normal displacement and the fields required to calculate it.
	unsigned char *feature_to_examine; //Points to most similar feature at any given time and the feature to be examined.

	
	for(int i = 0; i < length_of_max_dis_box; i++, current_coord_y++) {
	  current_coord_x = corner_coord_x; //Reset x coordinate for next row 
	  for(int j = 0; j < length_of_max_dis_box; j++, current_coord_x++) {
	      
	   feature_to_examine = check_bounds(image, current_coord_y, current_coord_x, feature_patch_width, feature_patch_height, image_width, image_height);    
		    
		if(feature_to_examine) { //If not null
			if((distance_to_examine = squared_euclidean_distance(feature_to_examine,
									    left_feature,
									    feature_patch_width,
									    feature_patch_height)) <= most_similar_euclid_distance) { //Set distance_to_examine as the euclidean distance between left feature and the right feature and compare between the most_similar_feature.

			    dy = abs(current_coord_y - height);
			    dx = abs(current_coord_x - width);
			    
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
	  } //End of for loop 

     }
    
	return most_similar_nordis; //max_displacement passed in as args.
    
}


/*
 Checks if a feature whose upper left corner at starting_point_offset is within the bounds of the image.
 */

int check_within_image(int height, int width, int feature_patch_width, int feature_patch_height, int image_width, int image_height) {
    
    //Top
	if(((height - (feature_patch_height / 2))  < 0) || ((width - (feature_patch_width / 2)) < 0)) { 
	  return 0;
	}

	//Ends
	if((width + (feature_patch_width / 2) >= image_width) ||
		(height + (feature_patch_height / 2) >= image_height)) {
	  return 0;
	}
	return 1;
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



/*
 Allocates a block of memory to create a feature for comparisons.
 */
unsigned char *populate_feature_patch(int feature_patch_height,
                                      int feature_patch_width,
                                      int image_width,
                                      int offset,
                                      unsigned char *image) {
    
	unsigned char *starting_pos = image + offset; //Start at position give by args
	unsigned char *feature_patch = (unsigned char *)malloc(feature_patch_height * feature_patch_width * sizeof(unsigned char)); //Malloc feature patch
	
	if(!feature_patch)
		allocation_failed();
	
	int height_offset, width_offset;
    int counter = 0;

	for(int i = 0; i < feature_patch_height; i++) {
	    height_offset = image_width * i;
	    for(int j = 0; j < feature_patch_width; j++) {
	        width_offset = height_offset + j;
            *(feature_patch + counter) = *(starting_pos + width_offset); //Set values from image into new feature patch
            counter++;
	    }
	}
	return feature_patch;
}