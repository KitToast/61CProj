// CS 61C Fall 2014 Project 3

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

#include <stdbool.h>
#include <math.h>

#include "utils.h"
#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement)
{
	/*
	size_t *floatOps = NULL;

	if (floatOps != NULL)
	{
		*floatOps = 0;
	}
	*/

	int sseBounds = -featureWidth + ((2* featureWidth + 1) / 4) * 4;
	int tail_case = (2 * featureWidth + 1) % 4;

	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < imageWidth; x++)
		{
			if ((y < featureHeight) || (y >= imageHeight - featureHeight) || (x < featureWidth) || (x >= imageWidth - featureWidth))
			{
				depth[y * imageWidth + x] = 0;
				continue;
			}

			float minimumSquaredDifference = -1;
			int minimumDy = 0;
			int minimumDx = 0;

			for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++)
			{
				for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
				{
					if (y + dy - featureHeight < 0 || y + dy + featureHeight >= imageHeight || x + dx - featureWidth < 0 || x + dx + featureWidth >= imageWidth)
					{
						continue;
					}

					float squaredDifference = 0; //Remember to coalesce this in the future
					int leftY;
					int rightY;
					int leftX;
					int rightX;  

					for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
					{
						__m128 msquared_difference = _mm_setzero_ps();
						leftY = y + boxY;
						rightY = y + dy + boxY;
						for (int boxX = -featureWidth; boxX < sseBounds; boxX += 4)
						{
							leftX = x + boxX;
							rightX = x + dx + boxX;

							__m128 left_image = _mm_loadu_ps(&(left[leftY * imageWidth + leftX]));
							__m128 right_image = _mm_loadu_ps(&(right[rightY * imageWidth + rightX]));

							__m128 difference = _mm_sub_ps(left_image,right_image);
							msquared_difference  = _mm_add_ps(msquared_difference, _mm_mul_ps(difference, difference));
						}

						float results[4];
						msquared_difference = _mm_hadd_ps(msquared_difference,msquared_difference);
						msquared_difference = _mm_hadd_ps(msquared_difference,msquared_difference);
						_mm_storeu_ps(results,msquared_difference);
						squaredDifference += results[0];

						leftX = x + sseBounds;
						rightX = x + dx + sseBounds;
						//Tail Case
						if(tail_case == 1) 
						{
							float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
							squaredDifference += difference * difference;
						} 
						else 
						{
							float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
							squaredDifference += difference * difference;

							difference = left[leftY * imageWidth + leftX + 1] - right[rightY * imageWidth + rightX + 1];
							squaredDifference += difference * difference;				

							difference = left[leftY * imageWidth + leftX + 2] - right[rightY * imageWidth + rightX + 2];
							squaredDifference += difference * difference;
						}
					}
		
					if ((minimumSquaredDifference == -1) || (minimumSquaredDifference == squaredDifference) && (displacementNaive(dx, dy) < displacementNaive(minimumDx, minimumDy))) || ((minimumSquaredDifference > squaredDifference) && (minimumSquaredDifference != squaredDifference))
					{
						minimumSquaredDifference = squaredDifference;
						minimumDx = dx;
						minimumDy = dy;
					}
				}
			}

			if (minimumSquaredDifference != -1)
			{
				if (maximumDisplacement == 0)
				{
					depth[y * imageWidth + x] = 0;
				}
				else
				{
					depth[y * imageWidth + x] = displacementNaive(minimumDx, minimumDy);
				}
			}
			else
			{
				depth[y * imageWidth + x] = 0;
			}
		}
	}
}
