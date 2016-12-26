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

int mode = 1;			// mode for predcition 0~8 in 4 x 4
const int paranum = 4;			// num for para 1~4 in 4 x 4
const double PI = 3.1415;

static void help()
{
	printf("The cmd usage like below:\n");
	printf("xx.exe input.raw output.raw height width endian(0 is big end) precison(8, 10, 12)\n");
}


int main(int argc, char *argv[])
{
	//const char* in_filepath = "./resources/wedge_shape_direction.txt";
	freopen("engery.txt", "w", stdout);
	
	int height;						// ԭͼ��
	int width;						// ԭͼ��
	int endian;						// ��С��
	int precision;					// ԭͼ���ؾ��ȣ�һ��8bit
	int img_sz = 0;					// ͼ��ߴ�
	
	int16_t LS_resi_energy = 0;		//ԭʼ��ͼ��Ԥ��в�����

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
	const std::string input_filename = argv[1];		// ����ԭʼͼ���ļ�
	const std::string output_filename = argv[2];	// ����ؽ�ͼ���ļ�
	height = (int)atoi(argv[3]);
	width  = (int)atoi(argv[4]);
	endian = (int)atoi(argv[5]);
	precision = (int)atoi(argv[6]);
	img_sz = height * width;


    /********************** 2.  ��������ͼ������ argv[1]**************************/
	unsigned char *pImg = new unsigned char [img_sz];		// ԭʼͼ�������
	read_img(input_filename.c_str(), pImg, img_sz, precision);


    /********************** 3.  ͼ�����ݴ�С�˸�ʽת�� **************************/	
	short **imgConverted = new_short_mat(height, width);					// ԭʼͼ����д�С��ת�����ͼ��

	// ��С�˸�ʽͳһת��
	transformat(imgConverted, pImg, height, width, endian, precision);
	printf("��С�˸�ʽת�����.\n");
	delete [] pImg;	// �ͷ�ԭͼ


    /********************** 4. ��ͼ�����ݽ��б߽���չ**************************/
	///����LS�����õ�ԭͼ��Ӧ��λ�õ��ؽ�ֵ���洢�����ؽ�ֵ����ԭͼ�������ͼ����
	short **imgLSConstructed = new_short_mat(height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);

	//������ͼ��ֵ���б߽���չ
	img_padding(imgLSConstructed, imgConverted, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);

	// ��ӡ��չǰ���ͼ��
	print_short_matrix(imgConverted, "lena_origin.txt", height, width);
	print_short_matrix(imgLSConstructed, "initConstruct.txt", height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);
	delete_short_mat(imgConverted, height, width);


	/***********************5. ����ͼ��鷽����Ϣ�ļ����ж�ͼ�����ݿ�ķ��򣬽��п�ϲ�*******/
	//short** direction_block = new short*[ROWS];
	//new_short_mat(direction_block, ROWS, COLS);
	//ReadFormatedFile(direction_block, in_filepath, ROWS, COLS);
	//// TODO ��wrj���ع�
	//image_cut_merge(img, height, width);


	/********************** 6.  ����ͼ���������������ұ�**************************///TODO!!!
	std::map<int, int> direction_table = build_direction_search_table();

	int wedge_direction_index = 3;	// ʹ�õ����������Ш���˲�������
	mode = search_direction(direction_table, wedge_direction_index);
	assert(mode != -1);


	/********************** 7.  �Բ�ͬ��ͼ������ͽ�������ɷ�Ԥ��ģ�ͽ���Ԥ��**************************/
	short **preLS = new_short_mat(height, width);		//ԭʼ��ͼ��Ԥ��ֵ
	short **LS_resi = new_short_mat(height, width);		//�洢LS����Ԥ���õ�������ͼ�Ĳв�����

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
	printf("Ԥ�����.\n");
	delete_short_mat(preLS, height, width);

	// ��ӡ�в������е���
	print_short_matrix(LS_resi, "resi.txt", height, width);
	delete_short_mat(LS_resi, height, width);


	/********************** 8. ��ɱ��룬����������ļ����л�д**************************/
	unsigned char **pRes = new_uchar_mat(height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);	//�洢LS����Ԥ���õ�������ͼ�Ĳв�����

	// ��ͼ���ؽ�ֵת����8bitͼ����ʱ������8bit����ͼ��
	for (int r = 0; r < height + bias_top_row + bias_down_row; ++r)
		for (int c = 0; c < width + bias_left_col + bias_right_col; ++c)
			pRes[r][c] = static_cast<unsigned char>(imgLSConstructed[r][c]);

	// д������������ļ�������
	write_img(output_filename.c_str(), pRes, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);	
	delete_uchar_mat(pRes, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);


	//��ӡ���յ��ؽ�ͼ��
	print_short_matrix(imgLSConstructed, "out_imgLSConstructed.txt", height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);
	printf("�ü���Ч�������.\n");
	delete_short_mat(imgLSConstructed, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);

	return 0;
}
