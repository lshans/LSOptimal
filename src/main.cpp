/********************************************************************************************
 * date: 2016.3 by lss
 * description: prediction, transformation and encoding for single frame picture.
 * 1. estimate prediction param of 9 modes for block of 4 x 4 and 4 modes for block of 16 x 16
 *    by Markov prediction method
 * 2. use estimated prediction param to predict
 * 3. transform the residual of prediction
 * 4. quantization and entropy encoding

 * date: 2016.5.11 by lss
 * description: change mode 8 to four parameters from three parameters,process successfully,but airport.raw resi is large 349910805.
 *********************************************************************************************/

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
const int mode = 1;			// mode for predcition 0~8 in 4 x 4
const int paranum = 4;			// num for para 1~4 in 4 x 4
const double PI = 3.1415;

// format transformation for input img file to little endian
// img:		output
// img_in:	origin img
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
int main(int argc, char *argv[])
{
	const char* in_filepath = "./resources/wedge_shape_direction.txt";
	freopen("engery.txt", "w", stdout);
	
	int  height;						// ԭͼ��
	int  width;							// ԭͼ��
	int  endian;						// ��С��
	int  precision;						// ԭͼ���ؾ��ȣ�һ��8bit
	
	int16_t LS_resi_energy = 0;			  //ԭʼ��ͼ��Ԥ��в�����
	short pixAverage = 0;				   //���ص�ƽ��ֵ
	long long pixSum = 0;				   //���ص��ܺ�

	int   img_size = 0;						// ͼ��ߴ�
	int   resi_size = 0;					// �в����ߴ�

		
	//double paramter_4x4_ML[4][9] = {
	//{0.42, 0.88, 0.57, 0.31, 0.25, 0.08, 0.71, 0.24, 0.33},
	//{-0.38, -0.42, -0.36, -0.22, 0.50, 0.23, 0.11, 0.27, -0.35},
	//{0.94, 0.51, 0.70, 0.24, 0.36, 0.74, 0.27, 0.74, 0.48},
	//{0, 0, 0, 0.60, 0, 0, 0,0.24, 0.46}
	//};
	//double paramter_4x4_MSE[4][9] = {
	//{0.51, 0.96, 0.65, 0.26, 0.20, -0.09, 0.64, 0.30, 0.31},
	//{-0.49, -0.40, -0.41, -0.10, 0.58, 0.46, 0.33, -0.26, -0.25},
	//{0.97, 0.44, 0.63, 0.05, 0.25, 0.63, 0.07, 0.66, 0.38},
	//{0, 0, 0, 0.76, 0, 0, 0, 0.29, 0.53}
	//};

    /************************** 1. ���������в���**************************/
	if(argc != 7)
	{
		help();
		return 0;
	}
	height = (int)atoi(argv[3]);
	width  = (int)atoi(argv[4]);
	endian = (int)atoi(argv[5]);
	precision = (int)atoi(argv[6]);
	img_size = height * width;
	resi_size = height * width;

    /********************** 2.  ��������ͼ������ argv[1]**************************/
	unsigned char *img_in = new unsigned char [img_size];		// ԭʼͼ�������
	//img_in = (unsigned char *)calloc(img_size, sizeof(short));//�������ɹ��򷵻�ָ�򱻷����ڴ��ָ�룬���򷵻ؿ�ָ��NULL
	assert(img_in);
	//����ԭʼͼ���ļ����������еĵ�һ�������������ļ�argv[1], ���뵽һάָ�� img_inָ����ڴ�������
	ReadFile(argv[1], img_in, img_size, precision);

    /********************** 3.  ͼ�����ݴ�С�˸�ʽת�� **************************/	
	// ��̬���ٿռ䣬 �洢ԭʼͼ����д�С��ת�����ͼ������
	short **img = new short*[height];//�������ɹ��򷵻�ָ�򱻷����ڴ��ָ�룬���򷵻ؿ�ָ��NULL
	memory_new(img, height, width);
	// ��С�˸�ʽת������һάָ�� img_inָ����ڴ������е����ݽ��д�С�˸�ʽת�������洢����άָ��imgָ����ڴ�������
	transformat(img, img_in, height, width, endian, precision);
	printf("ת���ļ��ɹ�\n");

    /********************** 4. ��ͼ�����ݽ��б߽���չ**************************/
	short **imgLSConstructed = new short*[height + bias_top_row + bias_down_row];	 //����LS�����õ�ԭͼ��Ӧ��λ�õ��ؽ�ֵ���洢�����ؽ�ֵ����ԭͼ�������ͼ����
	memory_new(imgLSConstructed, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);// ʹ�ú���memory_calloc, ��̬�����ڴ�ռ�洢����
	//������ͼ��ֵ��ֵ��imgLSConstructed  �ؽ�ͼ�����飬������bias_top_row, bias_down_row, bias_left_col, bias_right_col��С���������ұ߽���չ
	for(int r = 0; r < height + bias_top_row + bias_down_row; r++)    
	{
		for(int c = 0; c < width + bias_left_col + bias_right_col; c++)
		{
			if(r < bias_top_row || r > height + bias_top_row - 1 || c < bias_left_col || c > bias_right_col - 1 + width)
				imgLSConstructed[r][c] = 128;
			else
				imgLSConstructed[r][c] = img[r - bias_top_row][c - bias_left_col];
		}
	}
	// ��ӡ��ʼ�ؽ�ͼ������е���
	print_short_matrix_to_file(imgLSConstructed, "initConstruct.txt", height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);
	// ��ӡԭͼ�������ʼ�ؽ�ֵ������бȽϽ��е���
	print_short_matrix_to_file(img, "lena_origin.txt", height, width);

	/***********************5. ����ͼ��鷽����Ϣ�ļ����ж�ͼ�����ݿ�ķ��򣬽��п�ϲ�*******/
	short** direction_block = new short*[ROWS];
	memory_new(direction_block, ROWS, COLS);
	ReadFormatedFile(direction_block, in_filepath, ROWS, COLS);
	// TODO ��wrj���ع�
	image_cut_merge(img, height, width);

	/********************** 6.  ����ͼ���������������ұ�**************************///TODO!!!
	int wedge_direction_index = 3;
	map<int, int> direction_table;
	direction_table.insert(pair<int, int>(1, 1));
	direction_table.insert(pair<int, int>(2, 8));
	direction_table.insert(pair<int, int>(3, 3));
	direction_table.insert(pair<int, int>(4, 7));
	direction_table.insert(pair<int, int>(5, 0));
	direction_table.insert(pair<int, int>(6, 5));
	direction_table.insert(pair<int, int>(7, 4));
	direction_table.insert(pair<int, int>(8, 6));
	direction_search(wedge_direction_index, direction_table, mode);
	/********************** 7.  �Բ�ͬ��ͼ������ͽ�������ɷ�Ԥ��ģ�ͽ���Ԥ��**************************/
	short **preLS = new short*[height];		//ԭʼ��ͼ��Ԥ��ֵ
	memory_new(preLS, height, width);
	short **LS_resi = new short*[height];	//�洢LS����Ԥ���õ�������ͼ�Ĳв�����
	memory_new(LS_resi, height, width);
	switch(paranum)
	{
		case 1:
			predict_one_para(imgLSConstructed, LS_resi, preLS, height, width, mode);
			break;
		case 2:
			predict_two_para(imgLSConstructed, LS_resi, preLS, height, width, mode);
			break;
		case 3:
			predict_three_para(imgLSConstructed, LS_resi, preLS, height, width, mode);
			break;
		case 4:
			predict_four_para(imgLSConstructed, LS_resi, preLS, height, width, mode);
			break;
		default:
			break;
	}
	printf("Ԥ�����\n");

	// ��ӡ�в������е���
	print_short_matrix_to_file(LS_resi, "resi.txt", height, width);

	/********************** 8. ��ɱ��룬����������ļ����л�д**************************/
	unsigned char **pRes = new unsigned char*[height + bias_top_row + bias_down_row];//�洢LS����Ԥ���õ�������ͼ�Ĳв�����
	memory_new(pRes, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);

	//ת����8bit
	for (int r = 0; r < height + bias_top_row + bias_down_row; ++r)
		for (int c = 0; c < width + bias_left_col + bias_right_col; ++c)
			pRes[r][c] = static_cast<unsigned char>(imgLSConstructed[r][c]);

	// д������������ļ�������
	WriteFile(argv[2], pRes, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);	

	//��ӡ���յ��ؽ�ͼ�����ڵ���
	print_short_matrix_to_file(imgLSConstructed, "out_imgLSConstructed.txt", height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);
	printf("Ԥ��ɹ�\n");

	/********************** 9. ��̬������ڴ���ͷ�**************************/
	memory_free(pRes, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);
	memory_free(imgLSConstructed, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);
	memory_free(LS_resi, height, width);
	memory_free(preLS, height, width);
	memory_free(img, height, width);
	memory_1dimension_free(img_in, height, width);
	
	return 0;
}
