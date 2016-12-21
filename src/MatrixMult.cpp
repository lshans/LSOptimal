// multiply two mat
// D:		input 1st mat, H1 x K1
// f:		input 2st mat, K1 x W1
// temp:	otuput mat, H1 x W1
#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <iomanip>
#include <iostream>
#include "pre.h"
#include "common.h"

using namespace std; 
void Mult(double **mat1, double **mat2,double **resMat, int H1, int W1, int K1)	//DCTÕý±ä»»  H1 * K1 | K1 * W1
{
	double sum = 0;
	for(int i = 0; i < H1; i++)
	{
		for(int j = 0; j < W1; j++)
		{
			sum = 0;
			for(int k = 0;k < K1; k++)
				sum += mat1[i][k] * mat2[k][j];
			resMat[i][j] = sum;
			//printf("%f", resMat[i][j]); 
		}
		printf("\n"); 
	}
}