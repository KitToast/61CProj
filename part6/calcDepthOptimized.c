// CS 61C Fall 2014 Project 3

// include SSE intrinsics


// include OpenMP
#if !defined(_MSC_VER)
#include <pthread.h>
#endif
#include <omp.h>


#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "utils.h"
#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement)
{

    int sseBounds = -featureWidth + ((2 * featureWidth + 1) / 4) * 4;
    int tail_case = (2*featureWidth + 1) % 4;
    int y_bound = imageHeight - featureHeight;
    int x_bound = imageWidth - featureWidth;

    memset(depth, 0, imageWidth * imageHeight * sizeof(float)); //Memset all of depth map to zero

    for (int y = featureHeight; y < y_bound; y++)
    {

        for (int x = featureWidth; x < x_bound; x++)
        {

            float minimumSquaredDifference = -1;
            int minimumDy = 0;
            int minimumDx = 0;

            for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
            {
                for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++)
                {
                    if (y + dy - featureHeight < 0 || y + dy + featureHeight >= imageHeight || x + dx - featureWidth < 0 || x + dx + featureWidth >= imageWidth)
                    {
                        continue;
                    }

                    float squaredDifference = 0; //Remember to coalesce this in the future
		    float tailsquaredDifference = 0;
                    __m128 msquared_difference = _mm_setzero_ps();
                    int leftY;
                    int rightY;
                    int leftX;
                    int rightX;

                    for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
                    {
                        leftY = y + boxY;
                        rightY = y + dy + boxY;
                        int leftProductY = leftY * imageWidth;
                        int rightProductY = rightY * imageWidth;
                        for (int boxX = -featureWidth; boxX < sseBounds; boxX += 4)
                        {
                            leftX = x + boxX;
                            rightX = x + dx + boxX;

                            __m128 left_image0 = _mm_loadu_ps(&(left[leftProductY + leftX]));
                            __m128 right_image0 = _mm_loadu_ps(&(right[rightProductY + rightX]));

                            __m128 difference0 = _mm_sub_ps(left_image0,right_image0);
                            msquared_difference  = _mm_add_ps(msquared_difference, _mm_mul_ps(difference0, difference0));
                        }


                        leftX = x + sseBounds;
                        rightX = x + dx + sseBounds;
                        //Tail Case
                        if(tail_case == 1)
                        {
                            float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
                            tailsquaredDifference += difference * difference;
                        }
                        else
                        {
                            float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
                            tailsquaredDifference += difference * difference;

                            difference = left[leftY * imageWidth + leftX + 1] - right[rightY * imageWidth + rightX + 1];
                            tailsquaredDifference += difference * difference;

                            difference = left[leftY * imageWidth + leftX + 2] - right[rightY * imageWidth + rightX + 2];
                            tailsquaredDifference += difference * difference;
                        }
                    }

                        float results[4];
                        msquared_difference = _mm_hadd_ps(msquared_difference,msquared_difference);
                        msquared_difference = _mm_hadd_ps(msquared_difference,msquared_difference);
                        _mm_storeu_ps(results,msquared_difference);
                        squaredDifference += results[0];
			squaredDifference += tailsquaredDifference;

                    if ((minimumSquaredDifference == -1) || 
                       ((minimumSquaredDifference == squaredDifference) && 
                       (displacementNaive(dx,dy) < displacementNaive(minimumDx, minimumDy))) || 
                       ((minimumSquaredDifference > squaredDifference) && 
                       (minimumSquaredDifference != squaredDifference)))
                    {
                        minimumSquaredDifference = squaredDifference;
                        minimumDx = dx;
                        minimumDy = dy;
                    }
                }
            }

            if (minimumSquaredDifference != -1)
            {
                if (maximumDisplacement != 0)
                {
                    depth[y * imageWidth + x] = displacementNaive(minimumDx, minimumDy);
                }
            }
        }
    }
}
