#include <stdlib.h>
#include <stdio.h>
#include "quadtree.h"
#include "make_qtree.h"
#include "utils.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

/*******************function declaration********************/
static qNode *child_to_quad(unsigned char *, int, int, int);
/***********************************************************/


//The next four functions take in a depth map to return the quarter of their respective directions

unsigned char *depth_NW(unsigned char *depth_map, int map_width) {
    unsigned char *NW_map = (unsigned char *)malloc(map_width * map_width * sizeof(unsigned char) / 4);
    for (int i = 0; i < map_width / 2; i++) {
        for(int j = 0; j < map_width / 2; j++) {
            *(NW_map + i * map_width / 2 + j) = *(depth_map + i * map_width + j);
        }
    }
    return NW_map;
}

unsigned char *depth_NE(unsigned char *depth_map, int map_width) {
    unsigned char *NE_map = (unsigned char *)malloc(map_width * map_width * sizeof(unsigned char) / 4);
    for (int i = 0; i < map_width / 2; i++) {
        for(int j = 0; j < map_width / 2; j++) {
            *(NE_map + i * map_width / 2 + j) = *(depth_map + i * map_width + (j + map_width / 2));
        }
    }
    return NE_map;
}

unsigned char *depth_SE(unsigned char *depth_map, int map_width) {
    unsigned char *SE_map = (unsigned char *)malloc(map_width * map_width * sizeof(unsigned char) / 4);
    for (int i = 0; i < map_width / 2; i++) {
        for(int j = 0; j < map_width / 2; j++) {
            *(SE_map + i * map_width / 2 + j) = *(depth_map + (i + map_width / 2) * map_width + (j + map_width / 2));
        }
    }
    return SE_map;
}

unsigned char *depth_SW(unsigned char *depth_map, int map_width) {
    unsigned char *SW_map = (unsigned char *)malloc(map_width * map_width * sizeof(unsigned char) / 4);
    for(int i = 0; i < map_width / 2; i++) {
        for(int j = 0; j < map_width / 2; j++) {
            *(SW_map + i * map_width / 2 + j) = *(depth_map + (i + map_width / 2) * map_width + j);
        }
    }
    return SW_map;
}



int homogenous(unsigned char *depth_map, int map_width, int x, int y, int section_width) {
    unsigned char color = *(depth_map + x + y * map_width);
    int i = x + y * map_width;
    while(*(depth_map + i) == color) {
        if(i == (x + section_width - 1) + (y + section_width - 1) * map_width) { //last tile to be checked then:
            return color;
        }
        if ((i % map_width) == (x + section_width - 1)) { //next tile when at the edge of square being checked
            i += map_width - section_width + 1; 
        } else { //"normal" case while staying within the square
            i++;
        }
    }
    return 256;
}

qNode *depth_to_quad(unsigned char *depth_map, int map_width) {

    qNode *root = (qNode *)malloc(sizeof(qNode));
    
    if (!root) { //if null then allocation failed
        allocation_failed();
    }
    
    int gray_value = homogenous(depth_map, map_width, 0, 0, map_width); //returns 256 if yes, the color if otherwise
    root->size = map_width; 
    root->x = 0; 
    root->y = 0;
    root->gray_value = gray_value; //gray_value will always be the result of homogeneous? whether leaf or not
    
    if (gray_value != 256) { //case when the whole thing is just a solid block, hence just a leaf
        root->leaf = 1;
    } else {
        root->leaf = 0;
        
        root->child_NW = (qNode *)malloc(sizeof(qNode)); //malloc the children so we can assign them
        root->child_NE = (qNode *)malloc(sizeof(qNode));
        root->child_SE = (qNode *)malloc(sizeof(qNode));
        root->child_SW = (qNode *)malloc(sizeof(qNode));
        
        unsigned char *NW_map = depth_NW(depth_map, map_width); //the depth maps for the children
        unsigned char *NE_map = depth_NE(depth_map, map_width);
        unsigned char *SE_map = depth_SE(depth_map, map_width);
        unsigned char *SW_map = depth_SW(depth_map, map_width);
        
        root->child_NW = child_to_quad(NW_map, map_width / 2, 0, 0); //setting the children
        root->child_NE = child_to_quad(NE_map, map_width / 2, map_width / 2, 0);
        root->child_SE = child_to_quad(SE_map, map_width / 2, map_width / 2, map_width / 2);
        root->child_SW = child_to_quad(SW_map, map_width / 2, 0, map_width / 2);
        
        /****POSSIBLY @FIXME****/
        if (!NW_map) { //free the maps
            free(NW_map);
        }
        if (!NE_map) {
            free(NE_map);
        }
        if (!SE_map) {
            free(SE_map);
        }
        if (!SW_map) {
            free(SW_map);
        }
    }
    return root;
}

qNode *child_to_quad(unsigned char *depth_map, int map_width, int x, int y) { //recursion for children (we need x and y)
    
    qNode *node = (qNode *)malloc(sizeof(qNode));
    
    if (!node) {
        allocation_failed();
    }
    
    int gray_value = homogenous(depth_map, map_width, 0, 0, map_width); //returns 256 if yes, the color if otherwise
    node->size = map_width;
    node->x = x;
    node->y = y;
    node->gray_value = gray_value; //gray_value will always be the result of homogeneous? whether leaf or not
    
    if (gray_value != 256) { 
        node->leaf = 1;
    } else {
        node->leaf = 0;
        
        node->child_NW = (qNode *)malloc(sizeof(qNode)); //malloc the children so we can assign them
        node->child_NE = (qNode *)malloc(sizeof(qNode));
        node->child_SE = (qNode *)malloc(sizeof(qNode));
        node->child_SW = (qNode *)malloc(sizeof(qNode));
        
        unsigned char *NW_map = depth_NW(depth_map, map_width); //the depth maps for the children
        unsigned char *NE_map = depth_NE(depth_map, map_width);
        unsigned char *SE_map = depth_SE(depth_map, map_width);
        unsigned char *SW_map = depth_SW(depth_map, map_width);
        
        node->child_NW = child_to_quad(NW_map, map_width / 2, x, y); //setting the children
        node->child_NE = child_to_quad(NE_map, map_width / 2, x + map_width / 2, y);
        node->child_SE = child_to_quad(SE_map, map_width / 2, x + map_width / 2, y + map_width / 2);
        node->child_SW = child_to_quad(SW_map, map_width / 2, x, y + map_width / 2);
        
        /****POSSIBLY @FIXME****/
        if (!NW_map) { //free the maps
            free(NW_map);
        }
        if (!NE_map) {
            free(NE_map);
        }
        if (!SE_map) {
            free(SE_map);
        }
        if (!SW_map) {
            free(SW_map);
        }
    }
    return node;
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
