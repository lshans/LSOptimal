#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cstdint>
#include <iostream>

#include "pre.h"
#include "common.h"
// 读入文件错误
void help()
{
	printf("The cmd usage like below:\n");
	printf("xx.exe input.raw output.raw height width endian(0 is big end) precison(8, 10, 12)\n");
}

void ReadFile(const char* file, unsigned char* pImg, int img_size, int precision)
{
	FILE *filein = NULL;				// 文件句柄
	if((filein = fopen(file,"rb")) == NULL)
	{
		printf("ReadFile is error!\n");
		help();
		exit(0);
	}
	fread(pImg, sizeof(unsigned char), precision == 8 ? img_size : 2 * img_size, filein);
	fclose(filein);
	filein = NULL;
	printf("打开文件成功\n");
	return; 
}

void WriteFile(const char* outfile, unsigned char ** pImg, int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	FILE *fileout = NULL;				// 输出文件句柄
	fileout = fopen("Constructed.raw", "wb");
	if (fileout == NULL)
	{
		printf("openning error fileout\n");
		exit(0);
	}
	for (int r = bias_top_row; r < height + bias_top_row; ++r)
		fwrite(pImg[r] + bias_left_col, sizeof(unsigned char), width, fileout);
	fclose(fileout);
}


// print a matrix to a file for debugging
int print_double_matrix_to_file(double **mat, const char *filename, int rows, int cols, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	FILE *fout = fopen(filename, "w");
	assert(fout);
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
			fprintf(fout, "%6.2f, ", mat[i][j]);
		fprintf(fout, "\n");
	}
	fclose(fout);
	return 0;
}

int print_short_matrix_to_file(short **mat, const char *filename, int rows, int cols, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	//FILE *fout = fopen(filename, "w");
	//assert(fout);
	FILE *fout = NULL;
	if((fout = fopen(filename, "w")) == NULL)
	{
		printf("oppenning error output_text\n");
		exit(0);
	}
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
			fprintf(fout, "%4d", mat[i][j]);
		fprintf(fout, "\n");
	}
	fclose(fout);
	return 0;
}
int print_1dimension_short_matrix_to_file(unsigned char *mat, const char *filename, int rows, int cols, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	//FILE *fout = fopen(filename, "w");
	//assert(fout);
	FILE *fout = NULL;
	if((fout = fopen(filename, "w")) == NULL)
	{
		printf("oppenning error output_text\n");
		exit(0);
	}
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
			fprintf(fout, "%4d", mat[i*cols + j]);
		fprintf(fout, "\n");
	}
	fclose(fout);
	return 0;
}
// 动态开辟内存空间
int memory_new(short **img, int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{   	
	for(int i = 0; i < height + bias_top_row + bias_down_row; i++)
		img[i] = new short[width + bias_left_col + bias_right_col];
	return 0;
}
int memory_new(unsigned char **img, int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{   	
	for(int i = 0; i < height + bias_top_row + bias_down_row; i++)
		img[i] = new unsigned char[width + bias_left_col + bias_right_col];
	return 0;
}

// 释放动态开辟的内存
int memory_free(short **img, int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	for (int i = 0; i < height + bias_top_row + bias_down_row; ++i)
		delete [] img[i];
	delete [] img;
	img = NULL;
	return 0;
}

int memory_free(unsigned char **img, int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	for (int i = 0; i < height + bias_top_row + bias_down_row; ++i)
		delete [] img[i];
	delete [] img;
	img = NULL;
	return 0;
}

int memory_1dimension_free(unsigned char *img, int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	delete [] img;
	img = NULL;
	return 0;
}