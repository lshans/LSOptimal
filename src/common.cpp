#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <map>

#include "pre.h"
#include "common.h"
#include "canny.h"
using namespace std;
// �����ļ�����
void help()
{
	printf("The cmd usage like below:\n");
	printf("xx.exe input.raw output.raw height width endian(0 is big end) precison(8, 10, 12)\n");
}

void ReadFile(const char* file, unsigned char* pImg, int img_size, int precision)
{
	FILE *filein = NULL;				// �ļ����
	if((filein = fopen(file,"rb")) == NULL)
	{
		printf("ReadFile is error!\n");
		help();
		exit(0);
	}
	fread(pImg, sizeof(unsigned char), precision == 8 ? img_size : 2 * img_size, filein);
	fclose(filein);
	filein = NULL;
	printf("���ļ��ɹ�\n");
	return; 
}

void WriteFile(const char* outfile, unsigned char ** pImg, int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	FILE *fileout = NULL;				// ����ļ����
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

int ReadFormatedFile(short** direction_block, const char* in_filepath, int rows, int cols)
{
	FILE *filein = NULL;
	if((filein = fopen(in_filepath, "r")) == NULL)
	{
		printf("open direction_block file error\n");
		help();
		exit(0);
	}
	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			fscanf(filein, "%hd", &direction_block[i][j]);
		}
	}
	fclose(filein);
	filein = NULL;
	printf("��ͼ��鷽����Ϣ�ļ��ɹ�\n");
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
// ��̬�����ڴ�ռ�
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

// �ͷŶ�̬���ٵ��ڴ�
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


int direction_search(int wedge_direction_index, map<int, int> const&direction_table, int mode)
{
	map<int, int>::const_iterator iter;
	iter = direction_table.find(1);
	mode = iter->second;
	return 0;
}