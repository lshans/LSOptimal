#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <cassert>
#include <cstdint>
#include "pre.h"
#include "common.h"
#include "canny.h"
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



const int mode = 1;			// mode for predcition 0~8 in 4 x 4
const int paranum = 4;			// num for para 1~4 in 4 x 4
const double PI = 3.1415;
//short dc_left_image[256][256][8][8];
//short dc_top_image[256][256][8][8];
//short dc_image[256][256][8][8];
//short h_image[256][256][8][8];
//short v_image[256][256][8][8];
//short ddl_image[256][256][8][8];
//short ddr_image[256][256][8][8];
//short vr_image[256][256][8][8];
//short hd_image[256][256][8][8];
//short vl_image[256][256][8][8];
//short hu_image[256][256][8][8];
//void imagecut(int argc, char *argv[]);

// print a matrix to a file for debugging
int print_matrix_to_file(double **mat, int rows, int cols, const char *filename)
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
int print_matrix_to_file(short **mat, int rows, int cols, const char *filename)
{
	FILE *fout = fopen(filename, "w");
	assert(fout);
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
			fprintf(fout, "%4d", mat[i][j]);
		fprintf(fout, "\n");
	}
	fclose(fout);
	return 0;
}

// format transformation for input img file to little endian
// img:		output
// img_in:	origin img
void transformat(short **img, unsigned char *img_in, int height, int width, int endian, int precision);

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

//ת�ú���
// TDCT: ת�ú�ľ���
// DCT��ԭʼ����
// H1��ԭʼ����ĸ�
// W1��ԭʼ����Ŀ�

// transpose DCT to IDCT
// DCT:		input
// IDCT:	output

//void Fast(double ** TDCT, double ** DCT, int rows, int cols)
//{
//	for(int i=0; i < rows; i++)
//		for(int j = 0; j < cols; j++)
//			TDCT[j][i] = DCT[i][j];
//}

int main(int argc, char *argv[])
{
	freopen("engery.txt", "w", stdout);
	FILE *filein = NULL;				// ����ԭͼ
	FILE *fileout = NULL;				// ���������ͼ��
	int  height;						// ԭͼ��
	int  width;							// ԭͼ��
	int  endian;						// ��С��
	int  precision;						// ԭͼ���ؾ��ȣ�һ��8bit
	unsigned char *img_in = NULL;		// ԭʼͼ�������

	//short img264Constructed[1024 + 1][1024 + 5] = {0}; //�ؽ�ͼ�����飬����264�����õ�ԭͼ��Ӧ��λ�õ��ؽ�ֵ���洢�����ؽ�ֵ����ԭͼ�������ͼ����
	//short resi264[1024][1024] = {0};                   //�洢264Ԥ�������ͼ�Ĳв�����
	//short pre264[1024][1024] = {0};                    //�洢264Ԥ����Ԥ������
	//int16_t H264resi_energy = 0;                       //ԭʼ��ͼ��Ԥ��в�����

	short **imgLSConstructed = NULL; //����LS�����õ�ԭͼ��Ӧ��λ�õ��ؽ�ֵ���洢�����ؽ�ֵ����ԭͼ�������ͼ����
	short **LS_resi = NULL;         //�洢LS����Ԥ���õ�������ͼ�Ĳв�����
	short **preLS = NULL;           //ԭʼ��ͼ��Ԥ��ֵ
	int16_t LS_resi_energy = 0;       //ԭʼ��ͼ��Ԥ��в�����
    short pixAverage = 0;              //���ص�ƽ��ֵ
    long long pixSum = 0;              //���ص��ܺ�
	
	int   img_size = 0;					// ͼ��ߴ�
	int   resi_size = 0;				// �в����ߴ�
	//double Ck;							// 

	/*double   **DCT = NULL;				// DCTϵ������
	double   **ODST = NULL;				// ODSTϵ������
	double   **TDCT = NULL;				// DCTϵ�������ת��
	double   **f  = NULL;				// �任��ľ��󣿣�����
	double   **F  = NULL;				// �任��ľ��󣿣�����
	double   **temp = NULL;				// �任�м����*/


	
	//	double Q[16][16];//   double q[16][16];
	//double q[1][16] =  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//{-x1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	//{0,-x1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	//{0,0,-x1,1,0,0,0,0,0,0,0,0,0,0,0,0},
	//{0,0,0,-x1,1,0,0,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,-x1,1,0,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,0,-x1,1,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,0,-x1,1,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,0,-x1,1,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,0,-x1,1,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,0,-x1,1,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,-x1,1,0,0,0,0,0,0,0,0,0,0},
	//q[0][0] = 1;
	//for(i = 0,j = 0; j <= 15; j++)
	//{
	//q[i][j] = 0;
	//}
	//
	//
	//for(i = 2; i < 16; i++)
	//{
	//for(j = 2; j < 16; j++)
	//{
	//q[i][i] = 1;
	//q[i][j-1] = -0.42;
	//}
	//}
	
		
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


	

// 1. �������
	if(argc != 7)
	{
		printf("please input :infile outfile height width endian precision\n");
		return 0;
	}
	height = (int)atoi(argv[3]);
	width  = (int)atoi(argv[4]);
	endian = (int)atoi(argv[5]);
	precision = (int)atoi(argv[6]);
	//smallheight = (int)atoi(argv[7]);
	//smallwidth = (int)atoi(argv[8]);
	img_size = height * width;
	resi_size = height * width;
	img_in = (unsigned char *)calloc(img_size, sizeof(short));//�������ɹ��򷵻�ָ�򱻷����ڴ��ָ�룬���򷵻ؿ�ָ��NULL
	assert(img_in);
	//malloc ��ϵͳ�������ָ��size���ֽڵ��ڴ�ռ䡣���������� void* ���͡�void* ��ʾδȷ�����͵�ָ�롣
	//C,C++�涨��void* ���Ϳ���ǿ��ת��Ϊ�κ��������͵�ָ�롣���غ�ǿ��ת��Ϊʵ�����͵�ָ�롣


	// ����̬�������ռ䲢�����ļ�
	short **img = NULL;
	img = (short **)calloc(height, sizeof(char *));
	for(int i = 0; i < height; i++)
		img[i] = (short *)calloc(width,sizeof(short));
	//short *img = NULL;					// ԭʼͼ���С��ת���������
	//img = (short *)calloc(img_size, sizeof(short));//�������ɹ��򷵻�ָ�򱻷����ڴ��ָ�룬���򷵻ؿ�ָ��NULL
	
	imgLSConstructed = (short **)calloc(height + bias_top_row + bias_down_row, sizeof(short *));
	for(int i = 0; i < height + bias_top_row + bias_down_row; i++)
		imgLSConstructed[i] = (short *)calloc(width + bias_left_col + bias_right_col,sizeof(short));

	LS_resi = (short **)calloc(height, sizeof(short *));
	for(int i = 0; i < height; i++)
		LS_resi[i] = (short *)calloc(width,sizeof(short));

	preLS = (short **)calloc(height, sizeof(short *));
	for(int i = 0; i < height; i++)
		preLS[i] = (short *)calloc(width,sizeof(short));

	// ����ԭʼͼ���ļ�
	if((filein = fopen(argv[1],"rb")) == NULL)
	{
		printf("the file can not open\n");
		exit(0);
	}
	fread(img_in,sizeof(unsigned char), precision == 8 ? img_size : 2 * img_size, filein);
	fclose(filein);
	filein = NULL;
	printf("���ļ��ɹ�\n");
	

	
	//image cut 
	//char cutpara[9][100] = {"", "03.raw", "1024", "1024", "8", "256", "256", "4", "4"};
	
	//imagecut(9, (char **)cutpara);

	// ��С�˸�ʽת��
	transformat(img, img_in, height, width, endian, precision);
	printf("ת���ļ��ɹ�\n");
	//long hist[10] = {0};    //�洢������������ݶȸ�ֵ
	//Canny(img, 1, height, width, hist);
	//printf("�������ݶ�ǿ��\n");
	//for(int i = 0; i < 10; ++i)
	//	printf("mode %d %ld\n", i, hist[i]);
	//������ͼ��ֵ��ֵ��imgLSConstructed  �ؽ�ͼ������
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
	FILE *fout = fopen("initConstruct.txt", "w");
	assert(fout);
	for (int i = 0; i < height + bias_top_row + bias_down_row; ++i)
	{
		for (int j = 0; j < width + bias_left_col + bias_right_col; ++j)
		{
			fprintf(fout, "%4d", imgLSConstructed[i][j]);
		}
		fprintf(fout, "\n");
	}
	fclose(fout);
	// ��ӡԭͼ�������ʼ�ؽ�ֵ������бȽϽ��е���
	if((fout = fopen("lena1024.txt", "w")) == NULL)
	{
		printf("oppenning error output_text\n");
		exit(0);
	}
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			fprintf(fout, "%4d", img[i][j]);
		}
		fprintf(fout, "\n");
	}
	fclose(fout);


	//for(int r = 0; r < height + 4; r++)
	//{
	//	for(int c = 0; c < width + 4; ++c)
	//	{
	//		pixSum += imgLSConstructed[r][c];	
	//	}
	//}
	//pixAverage = (short)(pixSum / (1028 * 1028));
	//for(int r = 0; r < height + 4; r++)
	//{
	//	for(int c = 0; c < width + 4; ++c)
	//	{
	//		imgLSConstructed[r][c] -= pixAverage;	

	//	}
	//}

	//if((fout = fopen("AverageLSConstructed.txt", "w")) == NULL)
	//{
	//	printf("oppenning error output_text\n");
	//	exit(0);
	//}
	//for (int i = 0; i < height; ++i)
	//{
	//	for (int j = 0; j < width; ++j)
	//	{
	//		fprintf(fout, "%4d", imgLSConstructed[i][j]);
	//	}
	//	fprintf(fout, "\n");
	//}
	//fclose(fout);
	image_cut_merge(img, height, width);
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
	//printf("%4d\n", LS_resi_energy);
	//LS_resi_energy = predict_LS(imgLSConstructed[1028][1028],LS_resi[1024][1024],  );
	printf("Ԥ�����\n");

	// ��ӡ�в������е���
	
	if((fout = fopen("resi.txt", "w")) == NULL)
	{
		printf("oppenning error output_text\n");
		exit(0);
	}
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			fprintf(fout, "%4d", LS_resi[i][j]);
		}
		fprintf(fout, "\n");
	}
	fclose(fout);

	// ��ɱ��룬����������ļ����л�д
	if((fileout = fopen(argv[2],"wb")) == NULL)
	{
		printf("oppenning error fileout\n");
		exit(0);
	}
	fwrite(imgLSConstructed, sizeof(short), (height + bias_top_row + bias_down_row) * (width + bias_left_col + bias_right_col), fileout);
	fclose(fileout);

	//// ת����8bit
	unsigned char **pRes = new unsigned char*[height + bias_top_row + bias_down_row];
	for (int i = 0; i < height + bias_top_row + bias_down_row; ++i)
		pRes[i] = new unsigned char[width + bias_left_col + bias_right_col];

	for (int r = 0; r < height + bias_top_row + bias_down_row; ++r)
		for (int c = 0; c < width + bias_left_col + bias_right_col; ++c)
			pRes[r][c] = static_cast<unsigned char>(imgLSConstructed[r][c]);
	fileout = fopen("img264Constructed_8bit.raw", "wb");
	if (fileout == NULL)
	{
		printf("openning error fileout\n");
		exit(0);
	}
	for (int r = bias_top_row; r < height + bias_top_row; ++r)
		fwrite(pRes[r] + bias_left_col, sizeof(unsigned char), width, fileout);
	fclose(fileout);
	for (int i = 0; i < height + bias_top_row + bias_down_row; ++i)
		delete [] pRes[i];
	delete [] pRes;
	pRes = NULL;
	//��ӡ���յ��ؽ�ͼ��
	if((fileout = fopen("out_imgLSConstructed.txt", "w")) == NULL)
	{
		printf("oppenning error out_imgLSConstructed\n");
		exit(0);
	}
	for(int r = bias_top_row; r < height + bias_top_row; ++r)
	{
		for(int c = bias_left_col; c < width + bias_left_col; ++c)
			fprintf(fileout, "%4d", imgLSConstructed[r][c]);
		fprintf(fileout, "\n");
	}
	fclose(fileout);
	

	printf("Ԥ��ɹ�\n");


	

	// ��̬������ڴ���ͷ�
	for(int i = 0; i < height + bias_top_row + bias_down_row; i++)  
		free(imgLSConstructed[i]);  
	free(imgLSConstructed);

	for(int i = 0; i < height; i++)  
		free(LS_resi[i]);  
	free(LS_resi);

	for(int i = 0; i < height; i++)  
		free(preLS[i]);  
	free(preLS);

	
	//for(int i = 0; i < height; i++)  
	//	free(img[i]);  
	free(img);
	img = NULL;
	free(img_in);
	img_in = NULL;

	return 0;
}
