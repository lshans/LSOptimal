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


void read_img(const char* filename, unsigned char* pImg, int img_sz, int precision)
{
	FILE *fin = NULL;
	if((fin = fopen(filename,"rb")) == NULL)
	{
		fprintf(stderr, "Openning file %s failed.\n", filename);
		exit(-1);
	}
	fread(pImg, sizeof(unsigned char), precision == 8 ? img_sz : 2 * img_sz, fin);
	fclose(fin);
	return; 
}

void write_img(const char* filename, unsigned char ** pImg, int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	FILE *fout = fopen(filename, "wb");
	if (fout == NULL)
	{
		fprintf(stderr, "Openning file %s failed.\n", filename);
		exit(-1);
	}
	for (int r = 0; r < height; ++r)	// ֻ�Է��������д�ļ���ƫ����Ϊbias_left_col, bias_top_row
		fwrite(pImg[bias_top_row + r] + bias_left_col, sizeof(unsigned char), width, fout);

	fclose(fout);
}

int ReadFormatedFile(short** direction_block, const char* in_filepath, int rows, int cols)
{
	FILE *filein = NULL;
	if((filein = fopen(in_filepath, "r")) == NULL)
	{
		printf("open direction_block file error\n");
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

int print_short_matrix(short **mat, const char *filename, int rows, int cols, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	FILE *fout = NULL;
	if((fout = fopen(filename, "w")) == NULL)
	{
		fprintf(stderr, "Openning %s failed.\n", filename);
		exit(-1);
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

// ���ݿ�߷����ά����
short **new_short_mat(int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{   	
	short **pImg = new short* [height + bias_top_row + bias_down_row];
	for(int i = 0; i < height + bias_top_row + bias_down_row; i++)
		pImg[i] = new short[width + bias_left_col + bias_right_col];
	return pImg;
}

unsigned char **new_uchar_mat(int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{   	
	unsigned char **pImg = new unsigned char *[height + bias_top_row + bias_down_row];
	for(int i = 0; i < height + bias_top_row + bias_down_row; i++)
		pImg[i] = new unsigned char[width + bias_left_col + bias_right_col];
	return pImg;
}

// �ͷŶ�̬���ٵ��ڴ�
void delete_short_mat(short **pImg, int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	for (int r = 0; r < height + bias_top_row + bias_down_row; ++r)
		delete [] pImg[r];
	delete [] pImg;

}

void delete_uchar_mat(unsigned char **pImg, int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	for (int r = 0; r < height + bias_top_row + bias_down_row; ++r)
		delete [] pImg[r];
	delete [] pImg;
}

// ���ݲ��ұ���ҷ����������ʧ�ܷ���-1
int search_direction(const std::map<int, int> &direction_table, int wedge_direction_index)
{
	if (direction_table.find(wedge_direction_index) != direction_table.end())
		return direction_table.find(wedge_direction_index)->second;
	else
		return -1;
}

// ��С�˸�ʽת������һάָ�� img_inָ����ڴ������е����ݽ��д�С�˸�ʽת�������洢����άָ��imgָ����ڴ�������
void transformat(short **img, unsigned char *img_in,  int height, int width, int endian, int precision)
{
	if(precision <= 8)
	{
		for(int i = 0; i < height; i++)
			for(int j = 0; j < width; j++)
				img[i][j] = img_in[i* width + j];
	}
	else if((precision >= 8) && (precision <= 16))
	{
		for(int i = 0; i < height; i++)
			for(int j = 0; j < width; j++)
			{
				if(endian == 0) //��� 
					img[i][j] = (short)(img_in[(i * width + j)*2] * 256 + img_in[(i * width + j) * 2 + 1]);
				else if(endian == 1) //С��
					img[i][j] = (short)(img_in[(i * width + j) * 2 + 1] * 256 + img_in[(i * width + j) * 2]); 
			}
	}
}

// ��ͼ����б߽���չ
void img_padding(short **pImgPadding, short **pImg, int height, int width, int bias_top_row, int bias_down_row, int bias_left_col, int bias_right_col)
{
	for(int r = 0; r < height + bias_top_row + bias_down_row; r++)
	{
		for(int c = 0; c < width + bias_left_col + bias_right_col; c++) {

			if(r < bias_top_row || r > height + bias_top_row - 1 || c < bias_left_col || c > bias_right_col - 1 + width)	// ���߽�
				pImgPadding[r][c] = 128;
			else																											// ���ԭʼֵ
				pImgPadding[r][c] = pImg[r - bias_top_row][c - bias_left_col];
		}
	}
}

// �������ұ�
std::map<int, int> build_direction_search_table()
{
	std::map<int, int> direction_table;
	direction_table.insert(std::pair<int, int>(1, 1));
	direction_table.insert(std::pair<int, int>(2, 8));
	direction_table.insert(std::pair<int, int>(3, 3));
	direction_table.insert(std::pair<int, int>(4, 7));
	direction_table.insert(std::pair<int, int>(5, 0));
	direction_table.insert(std::pair<int, int>(6, 5));
	direction_table.insert(std::pair<int, int>(7, 4));
	direction_table.insert(std::pair<int, int>(8, 6));
	return direction_table;
}