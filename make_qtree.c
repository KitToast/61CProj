/*
 * PROJ1-1: YOUR TASK B CODE HERE
 *
 * Feel free to define additional helper functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include "quadtree.h"
#include "make_qtree.h"
#include "utils.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

int homogenous(unsigned char *depth_map, int map_width, int x, int y, int section_width) {
    
    
    return 256;
}

qNode *depth_to_quad(unsigned char *depth_map, int map_width) {
    
    if (map_width == 1) { //case that the depth map is a pixel
        
    }
    struct qNode *ans = malloc(sizeof(quadtree));
    
    return ans;
    
}

//The next four functions take in a depth map to return the quarter of their respective directions

(unsigned char) *depth_NW(unsigned char *depth_map, int map_width) {
    unsigned char *NW_map = (unsigned char *)malloc(map_width * map_width / 4 * sizeof(unsigned char))
    for (int i = 0; i < map_width / 2; i++) {
        for(int j = 0; j < map_width / 2; j++) {
            *(NW_map + i * map_width / 2 + j) = *(depth_map + i * map_width + j);
        }
    }
    return *NW_map;
}

(unsigned char) *depth_NE(unsigned char *depth_map, int map_width) {
    unsigned char *NE_map = (unsigned char *)malloc(map_width * map_width / 4 * sizeof(unsigned char))
    for(int i = 0; i < map_width / 2; i++) {
        for(int j = map_width / 2; j < map_width; j++) {
            *(NE_map + i * map_width / 2 + j - map_width / 2) = *(depth_map + i * map_width + j);
        }
    }
    return *NE_map;
}

(unsigned char) *depth_SE(unsigned char *depth_map, int map_width) {
    unsigned char *SE_map = (unsigned char *)malloc(map_width * map_width / 4 * sizeof(unsigned char))
    for(int i = map_width / 2; i < map_width; i++) {
        for(int j = map_width / 2; j < map_width; j++) {
            *(SE_map + (i - map_width / 2) * map_width / 2 + j - map_width / 2) = *(depth_map + i * map_width + j);
        }
    }
    return *SE_map;
}

(unsigned char) *depth_SW(unsigned char *depth_map, int map_width) {
    unsigned char *SW_map = (unsigned char *)malloc(map_width * map_width / 4 * sizeof(unsigned char))
    for(int i = map_width / 2; i < map_width; i++) {
        for(int j = 0; j < map_width / 2; j++) {
            *(SW_map + (i - map_width / 2) * map_width / 2 + j) = *(depth_map + i * map_width + j);
        }
    }
    return *SW_map;
}

void free_qtree(qNode *qtree_node) {
    if(qtree_node) {
        if(!qtree_node->leaf){
            free_qtree(qtree_node->child_NW);
            free_qtree(qtree_node->child_NE);
            free_qtree(qtree_node->child_SE);
            free_qtree(qtree_node->child_SW);
        }
        free(qtree_node);
    }
}
