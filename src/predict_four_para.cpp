#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <memory>
#include "common.h"
#include "pre.h"

void predict_four_para(short **image_construct, short **resi, short **predicted, int height, int width, int mode)
{
	 
	double **para = NULL;
    para = (double **)calloc(10,sizeof(double *));
	for(int i = 0; i < 10; i++)
	{
		para[i] = (double *)calloc(1,sizeof(double));
	}
	printf("start estimate\n");
	

	switch (mode)
	{
	case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 9: 
		estimate_LeftTop_four(image_construct, para, width, height, mode);
		break;
	case 8: case 10:
		estimate_LeftDown_four(image_construct, para, width, height, mode);
		break;
	default:
		break;
	}
	printf("finally para result is\n");
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 1; ++j)
		{
			printf("%f", para[i][j]); 
		}
		printf("\n");
	}
	switch (mode)
	{
	case 0: case 2: case 3: case 4: case 5: case 6: case 7: case 9:
			predict_LeftTop_four(image_construct, resi, predicted, para, height, width, mode);
			break;
	case 1: case 8: case 10:
			predict_LeftDown_four(image_construct, resi, predicted, para, height, width, mode);
			break;
		default:
			break;
	}
	for(int i = 0; i < 10; ++i)
	{
		free(para[i]);
	}
	free(para);
}
void AccumulateParaAB_four(double **paraA, double **paraB, short block_image[padded_imageblock_height][padded_imageblock_height], int i, int j, int mode)
{   //需要调整多个块累加的程序。。。。??
	static short u[5] = {0};	// u[y][x] equals to u(x, y) y equals i, x equals j
	switch (mode)
	{
	case 0:
		u[1] = block_image[i - 1][j - 1];
		u[2] = block_image[i - 1][j];
		u[3] = block_image[i - 1][j + 1];
		u[4] = block_image[i - 1][j + 2];
		break;
	case 1:
		u[1] = block_image[i - 2][j - 1];
		u[2] = block_image[i - 1][j - 1];
		u[3] = block_image[i][j - 1];
		u[4] = block_image[i + 1][j - 1];
		break;
	case 2:
		u[1] = block_image[i][j - 1];
		u[2] = block_image[i - 1][j - 1];
		u[3] = block_image[i - 1][j];
		u[4] = block_image[i - 2][j];
		break;
	case 3:
		u[1] = block_image[i][j - 1];
		u[2] = block_image[i - 1][j];
		u[3] = block_image[i - 1][j + 1];
		u[4] = block_image[i - 1][j + 2];
		break;
	case 4:
		u[1] = block_image[i][j - 1];
		u[2] = block_image[i - 1][j - 1];
		u[3] = block_image[i - 1][j];
		u[4] = block_image[i - 2][j - 1];
		break;	
	case 5:
		u[1] = block_image[i][j - 1];
		u[2] = block_image[i - 1][j - 1];
		u[3] = block_image[i - 1][j];
		u[4] = (block_image[i - 2][j - 2] + block_image[i - 1][j - 2]) / 2;  //自己构造
		break;	
	case 6:
		u[1] = block_image[i][j - 1];
		u[2] = block_image[i - 1][j - 1];
		u[3] = block_image[i - 1][j];
		u[4] = block_image[i - 1][j - 2];  //可自己构造
		break;	
	case 7:
		u[1] = block_image[i][j - 1];
		u[2] = block_image[i - 1][j - 1];
		u[3] = block_image[i - 1][j];
		u[4] = block_image[i - 1][j + 1];  //可自己构造
		break;	
	case 8:
		u[1] = block_image[i - 1][j];
		u[2] = block_image[i - 1][j - 1];
		u[3] = block_image[i][j - 1];
		u[4] = block_image[i + 1][j - 1];  //可自己构造
		break;
	case 9:
		u[1] = block_image[i - 1][j];
		u[2] = block_image[i - 1][j - 1];
		u[3] = block_image[i][j - 1];
		u[4] = block_image[i + 1][j - 1];  //可自己构造
		break;
	case 10:
		break;
	default:
		break;
	}
	u[0] = block_image[i][j];

	paraA[0][0] += u[1] * u[1];
	paraA[0][1] += u[1] * u[2];
	paraA[0][2] += u[1] * u[3];
	paraA[0][3] += u[1] * u[4];
	paraA[1][0] += u[1] * u[2];
	paraA[1][1] += u[2] * u[2];
	paraA[1][2] += u[2] * u[3];
	paraA[1][3] += u[2] * u[4];
	paraA[2][0] += u[1] * u[3];
	paraA[2][1] += u[2] * u[3];
	paraA[2][2] += u[3] * u[3];
	paraA[2][3] += u[3] * u[4];
	paraA[3][0] += u[1] * u[4];
	paraA[3][1] += u[2] * u[4];
	paraA[3][2] += u[3] * u[4];
	paraA[3][3] += u[4] * u[4];

	paraB[0][0] += u[0] * u[1];
	paraB[1][0] += u[0] * u[2];
	paraB[2][0] += u[0] * u[3];
	paraB[3][0] += u[0] * u[4];
}

void estimate_LeftTop_four(short **image_construct, double **para, int height, int width, int mode)
{
	double **paraA = NULL;
	double **paraB = NULL;
	double **result = NULL;
	short pixAverage = 0;              //当前块像素的平均值
	short pixSum = 0;              //像素的总和
	short block_image[ROWS][COLS][padded_imageblock_height][padded_imageblock_width] = {0}; 
	short smallimage[ROWS][COLS][padded_imageblock_height][padded_imageblock_width] = {0};

	paraA  = (double **)calloc(10, sizeof(double *));
	for(int i = 0; i < 10; i++)
	{
		paraA[i] = (double *)calloc(10,sizeof(double));
	}

	result  = (double **)calloc(10, sizeof(double *));
	for(int i = 0; i < 10; i++)
	{
		result[i] = (double *)calloc(10,sizeof(double));
	}

	paraB  = (double **)calloc(10, sizeof(double *));
	for(int i = 0; i < 10; i++)
	{
		paraB[i] = (double *)calloc(1,sizeof(double));
	}

	// 扩充原始块，用128填充上下各四行，左右各四列
	//short **matrix_in = NULL;
	//matrix_in = (short **)calloc(height + 4,sizeof(short *));
	//for(int i = 0; i < height + 4; i++)
	//	matrix_in[i] = (short *)calloc(width + 4,sizeof(short));
	//memset(matrix_in, 128, sizeof(short));
	//short **matrix_in_offset = (short **)&matrix_in[2][2];
	//for (int i = 0; i < height; ++i)
	//	for (int j = 0; j < width; ++j)
	//		matrix_in_offset[i][j] = img[i][j];


	for (int i_row = 0; i_row < ROWS; ++i_row)
	{
		for (int i_col = 0; i_col < COLS; ++i_col)
		{
			for (int r = 0; r < BLOCKHEIGHT + bias_top_row + bias_down_row; ++r)
			{
				for (int c = 0; c < BLOCKWIDTH + bias_left_col + bias_right_col; ++c)
				{
					block_image[i_row][i_col][r][c] = image_construct[i_row * BLOCKHEIGHT + r][i_col * BLOCKWIDTH + c];
					//dc_left_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];   // 本函数用宏块左边四个像素（I、J、K、L）的均值对所有像素覆盖   
					//dc_top_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 本函数用宏块上方 4个像素（A、B、C、D）的均值对所有像素覆盖
					//dc_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 本函数用左边和上边共 8个像素（I、J、K、L 、A、B、C、D）的均值对所有像素覆盖
					//h_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 本函数用每行左边的像素对行进行覆盖（一行的所有像素值相同）
					//v_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 本函数用每列上方的像素对列进行覆盖（一列的所有像素值相同）
					//ddl_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 45°右上至左下覆盖预测
					//ddr_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 45°左上至右下覆盖预测
					//vr_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 与y夹角 26.6°左上至右下覆盖预测 （没用到 L）
					//hd_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 与x夹角 26.6°左上至右下覆盖预测（没用到D）
					//vl_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 与y夹角 26.6°右上至左下覆盖预测（没用到H）
					//hu_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 与x夹角 26.6°左下至右上覆盖预测
					//image_mode012[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式0、1、2的原始块数组
					//image_mode3[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式3的原始块数组
					//image_mode4[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式4的原始块数组
					//image_mode5[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式5的原始块数组
					//image_mode6[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式6的原始块数组
					//image_mode7[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式7的原始块数组
					//image_mode8[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式8的原始块数组
				}
			}
			//将图像块的右边两列和下边两行的值赋值成128
			for(int r = 0; r < BLOCKHEIGHT + bias_top_row + bias_down_row; r++)    
			{
				for(int c = 0; c < BLOCKWIDTH + bias_left_col + bias_right_col; c++)
				{
					//if(r > BLOCKHEIGHT + 1 || (r > 2 && c > BLOCKWIDTH + 1)) //两种情况 赋值128的位置不同
					if(r > BLOCKHEIGHT + bias_top_row - 1  || (r > bias_top_row && c > BLOCKWIDTH + bias_left_col - 1))
						block_image[i_row][i_col][r][c] = 128; 
				}
			}
			//0,1,2,3,4,5,6,7模式可以用下面公式计算
			for(int i = bias_top_row; i < BLOCKHEIGHT + bias_top_row; ++i)
				for(int j = bias_left_col; j < BLOCKWIDTH + bias_left_col; ++j)
					AccumulateParaAB_four(paraA, paraB, block_image[i_row][i_col], i, j, mode);
			printf("simple block (%d, %d) end \n", i_row, i_col);
		}
	}
	printf("para end \n");


	//for(int i = 0; i < 3; i++)
	//{
	//	for(int j = 0; j < 3; ++j)
	//	{
	//		paraA[0][0] = 1;
	//		paraA[0][1] = 1;
	//		paraA[0][2] = 2;
	//		paraA[1][0] = -1;
	//		paraA[1][1] = 2;
	//		paraA[1][2] = 0;
	//		paraA[2][0] = 1;
	//		paraA[2][1] = 1;
	//		paraA[2][2] = 3;
	//	}
	//}
	//for(int i = 0; i < 3; i++)
	//{
	//	for(int j = 0; j < 3; ++j)
	//	{
	//		paraB[0][0] = 1;
	//		paraB[1][0] = 0;
	//		paraB[2][0] = 0;
	//	}
	//}

	//矩阵求逆
	if (Gauss(paraA, result, 4))  
	{  
		printf("Matrix A inve is\n");
		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				printf("%.31f ", result[i][j]); 
			}
			printf("\n");
		}
	} 

	//矩阵result 与paraB相乘
	Mult(result, paraB, para, 4, 1, 4);


	//for (int i = 0; i < height + 4; ++i)
	//	free(matrix_in[i]);
	//free(matrix_in);

	for (int i = 0; i < 10; ++i)
	{
		free(result[i]);
		free(paraA[i]);
		free(paraB[i]);
	}
	free(result);
	free(paraA);
	free(paraB);
}
void estimate_LeftDown_four(short **image_construct, double **para, int height, int width, int mode)
{

	double **paraA = NULL;
	double **paraB = NULL;
	double **result = NULL;
	short pixAverage = 0;              //当前块像素的平均值
	short pixSum = 0;              //像素的总和
	short block_image[ROWS][COLS][padded_imageblock_height][padded_imageblock_width] = {0}; 
	short smallimage[ROWS][COLS][padded_imageblock_height][padded_imageblock_width] = {0};

	paraA  = (double **)calloc(10, sizeof(double *));
	for(int i = 0; i < 10; i++)
	{
		paraA[i] = (double *)calloc(10,sizeof(double));
	}

	result  = (double **)calloc(10, sizeof(double *));
	for(int i = 0; i < 10; i++)
	{
		result[i] = (double *)calloc(10,sizeof(double));
	}

	paraB  = (double **)calloc(10, sizeof(double *));
	for(int i = 0; i < 10; i++)
	{
		paraB[i] = (double *)calloc(1,sizeof(double));
	}

	// 扩充原始块，用128填充上下各四行，左右各四列
	//short **matrix_in = NULL;
	//matrix_in = (short **)calloc(height + 4,sizeof(short *));
	//for(int i = 0; i < height + 4; i++)
	//	matrix_in[i] = (short *)calloc(width + 4,sizeof(short));
	//memset(matrix_in, 128, sizeof(short));
	//short **matrix_in_offset = (short **)&matrix_in[2][2];
	//for (int i = 0; i < height; ++i)
	//	for (int j = 0; j < width; ++j)
	//		matrix_in_offset[i][j] = img[i][j];


	for (int i_col = 0; i_col < COLS; ++i_col)    //遍历顺序很重要！！！！！从上到下，从第一列到最后一列
	{
		for (int i_row = 0; i_row < ROWS; ++i_row)
		{
			for (int r = 0; r < BLOCKHEIGHT + bias_top_row + bias_down_row; ++r)
			{
				for (int c = 0; c < BLOCKWIDTH + bias_left_col + bias_right_col; ++c)
				{
					block_image[i_row][i_col][r][c] = image_construct[i_row * BLOCKHEIGHT + r][i_col * BLOCKWIDTH + c];
					//dc_left_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];   // 本函数用宏块左边四个像素（I、J、K、L）的均值对所有像素覆盖   
					//dc_top_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 本函数用宏块上方 4个像素（A、B、C、D）的均值对所有像素覆盖
					//dc_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 本函数用左边和上边共 8个像素（I、J、K、L 、A、B、C、D）的均值对所有像素覆盖
					//h_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 本函数用每行左边的像素对行进行覆盖（一行的所有像素值相同）
					//v_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 本函数用每列上方的像素对列进行覆盖（一列的所有像素值相同）
					//ddl_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 45°右上至左下覆盖预测
					//ddr_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 45°左上至右下覆盖预测
					//vr_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 与y夹角 26.6°左上至右下覆盖预测 （没用到 L）
					//hd_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 与x夹角 26.6°左上至右下覆盖预测（没用到D）
					//vl_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 与y夹角 26.6°右上至左下覆盖预测（没用到H）
					//hu_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 与x夹角 26.6°左下至右上覆盖预测
					//image_mode012[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式0、1、2的原始块数组
					//image_mode3[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式3的原始块数组
					//image_mode4[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式4的原始块数组
					//image_mode5[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式5的原始块数组
					//image_mode6[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式6的原始块数组
					//image_mode7[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式7的原始块数组
					//image_mode8[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];// 模式8的原始块数组
				}
			}
			//将图像块的右边两列和下边两行的值赋值成128
			for (int r = 0; r < BLOCKHEIGHT + bias_top_row + bias_down_row; ++r)
			{
				for (int c = 0; c < BLOCKWIDTH + bias_left_col + bias_right_col; ++c)
				{
					//if((r > BLOCKHEIGHT + 1 && c > 2) || c > BLOCKWIDTH + 1)
					if((r > BLOCKHEIGHT + bias_top_row - 1 && c > bias_left_col) || c > BLOCKWIDTH + bias_left_col - 1)
						block_image[i_row][i_col][r][c] = 128;
				}
			}
			//8模式可以用下面公式计算
			for(int i = bias_top_row; i < BLOCKHEIGHT + bias_top_row; ++i)
				for(int j = bias_left_col; j < BLOCKWIDTH + bias_left_col; ++j)
					//AccumulateParaAB(paraA, paraB, image_mode[i_row][i_col], j, i, mode);
					AccumulateParaAB_four(paraA, paraB, block_image[i_row][i_col], i, j, mode);
			printf("simple block (%d, %d) end \n", i_row, i_col);
		}
	}
	printf("para end \n");


	//for(int i = 0; i < 3; i++)
	//{
	//	for(int j = 0; j < 3; ++j)
	//	{
	//		paraA[0][0] = 1;
	//		paraA[0][1] = 1;
	//		paraA[0][2] = 2;
	//		paraA[1][0] = -1;
	//		paraA[1][1] = 2;
	//		paraA[1][2] = 0;
	//		paraA[2][0] = 1;
	//		paraA[2][1] = 1;
	//		paraA[2][2] = 3;
	//	}
	//}
	//for(int i = 0; i < 3; i++)
	//{
	//	for(int j = 0; j < 3; ++j)
	//	{
	//		paraB[0][0] = 1;
	//		paraB[1][0] = 0;
	//		paraB[2][0] = 0;
	//	}
	//}
	// 矩阵求逆l
	if (Gauss(paraA, result, 4))  
	{  
		printf("Matrix A inve is\n");
		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				printf("%.31f ", result[i][j]); 
			}
			printf("\n");
		}
	} 

	//if (Gauss(paraA, result, 3))  
	//{  
	//	printf("Matrix A inve is\n");
	//	for(int i = 0; i < 3; ++i)
	//	{
	//		for(int j = 0; j < 3; ++j)
	//		{
	//			printf("%.31f ", result[i][j]); 
	//		}
	//		printf("\n");
	//	}
	//} 


	//
	//{
	//	FILE *fout = fopen("res.txt", "w");
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		for (int j = 0; j < 3; ++j)
	//			fprintf(fout, "%6.2f, ", result[i][j]);
	//		fprintf(fout, "\n");
	//	}
	//	fclose(fout);
	//}


	//矩阵result 与paraB相乘
	Mult(result, paraB, para, 4, 1, 4);


	//for (int i = 0; i < height + 4; ++i)
	//	free(matrix_in[i]);
	//free(matrix_in);

	for (int i = 0; i < 10; ++i)
	{
		free(result[i]);
		free(paraA[i]);
		free(paraB[i]);
	}
	free(result);
	free(paraA);
	free(paraB);
}
long long predict_LeftTop_four(short **image_construct, short **resi, short **predicted, double **para, int height, int width, int mode)
{
	// 图像残差块、变换量化后的图像块、的内存空间分配
	short outdataDct_Quant[BLOCKWIDTH][BLOCKHEIGHT] = {0};		  //每一个小块的变换量化后的值
	short block_construct[BLOCKWIDTH][BLOCKHEIGHT] = {0};		 //每一个小块的重建值
	short block_pre[BLOCKWIDTH][BLOCKHEIGHT] = {0};				 //每一个小块的预测值
	long long resi_energy = 0;					      // 所有已处理块的累计残差能量
	int energy_temp = 0;                              //每一个小块的各像素的残差能量值
	short resi_temp[BLOCKWIDTH][BLOCKHEIGHT] = {0};                 //块残差数据                 
	short block_image[ROWS][COLS][padded_imageblock_height][padded_imageblock_width] = {0}; 
	short smallimage[ROWS][COLS][padded_imageblock_height][padded_imageblock_width] = {0};	    

	for (int i_row = 0; i_row < ROWS; ++i_row)
	{
		for (int i_col = 0; i_col < COLS; ++i_col)
		{
			// 将输入图像读成 8x8 的图像块（边界扩展后），赋值给用来存储每一种预测模式输入图像的数组
			// 
			for (int i = 0; i < BLOCKHEIGHT + bias_top_row + bias_down_row; ++i)
			{
				for (int j = 0; j < BLOCKWIDTH + bias_left_col + bias_right_col; ++j)
				{
					smallimage[i_row][i_col][i][j] = image_construct[i_row * BLOCKHEIGHT + i] [i_col * BLOCKWIDTH + j];
					//smallimage[i_row * 256 + i_col][i * 8 + j]
					block_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];
				}
			}
			//将图像块的右边两列和下边两行的值赋值成128
			for (int r = 0; r < BLOCKHEIGHT + bias_top_row + bias_down_row; ++r)
			{
				for (int c = 0; c < BLOCKWIDTH + bias_left_col + bias_right_col; ++c)
				{
					if(r > BLOCKHEIGHT + bias_top_row - 1  || (r > bias_top_row && c > BLOCKWIDTH + bias_left_col - 1))  //两种情况 赋值128的位置不同
						block_image[i_row][i_col][r][c] = 128;
						//image_mode3[r][c] = 128;
						//image_mode4[r][c] = 128;
						//image_mode5[r][c] = 128;
						//image_mode6[r][c] = 128;
						//image_mode7[r][c] = 128;
						//image_mode8[r][c] = 128;  
				}
			}
			//short North, NW, W;
			//switch (mode)
			//{
			//	case 0: case 1: case 2:
			//		//0,1,4,5,6模式可以用下面公式计算
			//		for(int i = 2; i < BLOCKHEIGHT + 2; ++i)
			//		{
			//			for(int j = 2; j < BLOCKWIDTH + 2; ++j)
			//			{
			//				North = image_mode[i_row][i_col][i - 1][j];
			//				NW = image_mode[i_row][i_col][i - 1][j - 1];
			//				W  = image_mode[i_row][i_col][i][j - 1];
			//				block_pre[i - 2][j - 2] = para[0][0] * W + para[1][0]* NW + para[2][0] * North;
			//				resi_temp[i - 2][j - 2] = image_mode[i_row][i_col][i][j] - block_pre[i - 2][j - 2];//预测残差
			//				energy_temp = resi_temp[i - 2][j - 2] * resi_temp[i - 2][j - 2];
			//				printf("energe [%4d][%4d]  %I64d\n", i_row, i_col, energy_temp);
			//				resi_energy += energy_temp;
			//			}
			//		}
			//	case 3:
			//		//3模式可以用下面公式计算
			//		for(int i = BLOCKHEIGHT + 1; i >= 2; --i)
			//		{
			//			for(int j = BLOCKWIDTH + 1; j >= 2; --j)
			//			{
			//				North = (image_mode[i_row][i_col][i - 1][j] + 2 * image_mode[i_row][i_col][i - 1][j + 1] + image_mode[i_row][i_col][i - 1][j + 2]) / 4;
			//				NW = image_mode[i_row][i_col][i - 1][j];
			//				W  = image_mode[i_row][i_col][i][j - 1];
			//				block_pre[i - 2][j - 2] = para[0][0] * W + para[1][0]* NW + para[2][0] * North;
			//				resi_temp[i - 2][j - 2] = image_mode[i_row][i_col][i][j] - block_pre[i - 2][j - 2];//预测残差
			//				energy_temp = resi_temp[i - 2][j - 2] * resi_temp[i - 2][j - 2];
			//				printf("energe [%4d][%4d]  %I64d\n", i_row, i_col, energy_temp);
			//				resi_energy += energy_temp;
			//			}
			//		}
			//		break;
			//	case 7:
			//		//7模式可以用下面公式计算
			//		for(int i = BLOCKHEIGHT + 1; i >= 2; --i)
			//		{
			//			for(int j = BLOCKWIDTH + 1; j >= 2; --j)
			//			{
			//				North = (image_mode[i_row][i_col][i - 1][j - 1] + image_mode[i_row][i_col][i - 1][j]) / 2;
			//				NW = (image_mode[i_row][i_col][i - 1][j] + image_mode[i_row][i_col][i - 1][j + 1]) / 2;
			//				W = image_mode[i_row][i_col][i][j - 1];
			//				block_pre[i - 2][j - 2] = para[0][0] * W + para[1][0]* NW + para[2][0] * North;
			//				resi_temp[i - 2][j - 2] = image_mode[i_row][i_col][i][j] - block_pre[i - 2][j - 2];//预测残差
			//				energy_temp = resi_temp[i - 2][j - 2] * resi_temp[i - 2][j - 2];
			//				printf("energe [%4d][%4d]  %I64d\n", i_row, i_col, energy_temp);
			//				resi_energy += energy_temp;
			//			}
			//		}
			//		break;
			//		break;
			//	case 4:
			//		//4模式可以用下面公式计算
			//		for(int i = 2; i < BLOCKHEIGHT + 2; ++i)
			//		{
			//			for(int j = 2; j < BLOCKWIDTH + 2; ++j)
			//			{
			//				North = image_mode[i_row][i_col][i - 1][j - 2];
			//				NW = (image_mode[i_row][i_col][i - 1][j - 2] + 2 * image_mode[i_row][i_col][i - 1][j - 1] + image_mode[i_row][i_col][i - 1][j]) / 4;
			//				W  = image_mode[i_row][i_col][i][j - 1];
			//				block_pre[i - 2][j - 2] = para[0][0] * W + para[1][0]* NW + para[2][0] * North;
			//				resi_temp[i - 2][j - 2] = image_mode[i_row][i_col][i][j] - block_pre[i - 2][j - 2];//预测残差
			//				energy_temp = resi_temp[i - 2][j - 2] * resi_temp[i - 2][j - 2];
			//				printf("energe [%4d][%4d]  %I64d\n", i_row, i_col, energy_temp);
			//				resi_energy += energy_temp;
			//			}
			//		}
			//		break;
			//	case 5:
			//		//5模式可以用下面公式计算
			//		for(int i = 2; i < BLOCKHEIGHT + 2; ++i)
			//		{
			//			for(int j = 2; j < BLOCKWIDTH + 2; ++j)
			//			{
			//				North = (image_mode[i_row][i_col][i - 1][j - 2] + image_mode[i_row][i_col][i - 1][j - 1]) / 2;
			//				NW = (image_mode[i_row][i_col][i - 1][j - 1] + image_mode[i_row][i_col][i - 1][j]) / 2;
			//				W  = image_mode[i_row][i_col][i][j - 1];
			//				block_pre[i - 2][j - 2] = para[0][0] * W + para[1][0]* NW + para[2][0] * North;
			//				resi_temp[i - 2][j - 2] = image_mode[i_row][i_col][i][j] - block_pre[i - 2][j - 2];//预测残差
			//				energy_temp = resi_temp[i - 2][j - 2] * resi_temp[i - 2][j - 2];
			//				printf("energe [%4d][%4d]  %I64d\n", i_row, i_col, energy_temp);
			//				resi_energy += energy_temp;
			//			}
			//		}
			//		break;
			//	case 6:
			//		//6模式可以用下面公式计算
			//		for(int i = 2; i < BLOCKHEIGHT + 2; ++i)
			//		{
			//			for(int j = 2; j < BLOCKWIDTH + 2; ++j)
			//			{
			//				North = (image_mode[i_row][i_col][i - 1][j - 1] + image_mode[i_row][i_col][i - 2][j - 1]) / 2;
			//				NW = image_mode[i_row][i_col][i - 1][j];
			//				W = (image_mode[i_row][i_col][i][j - 1] + image_mode[i_row][i_col][i - 1][j - 1]) / 2;
			//				block_pre[i - 2][j - 2] = para[0][0] * W + para[1][0]* NW + para[2][0] * North;
			//				resi_temp[i - 2][j - 2] = image_mode[i_row][i_col][i][j] - block_pre[i - 2][j - 2];//预测残差
			//				energy_temp = resi_temp[i - 2][j - 2] * resi_temp[i - 2][j - 2];
			//				printf("energe [%4d][%4d]  %I64d\n", i_row, i_col, energy_temp);
			//				resi_energy += energy_temp;
			//			}
			//		}
			//		break;
			//	default:
			//		break;
			//}
			int block_energy = 0;
			for(int i = bias_top_row; i < BLOCKHEIGHT + bias_top_row; ++i)
			{
				for(int j = bias_left_col; j < BLOCKWIDTH + bias_left_col; ++j)
				{
					static short u[5] = {0};	// u[y][x] equals to u(x, y) y equals i, x equals j
					switch (mode)
					{
					case 0:
						u[1] = block_image[i_row][i_col][i - 1][j - 1];
						u[2] = block_image[i_row][i_col][i - 1][j];
						u[3] = block_image[i_row][i_col][i - 1][j + 1];
						u[4] = block_image[i_row][i_col][i - 1][j + 2];
						break;
					case 2:
						u[1] = block_image[i_row][i_col][i][j - 1];
						u[2] = block_image[i_row][i_col][i - 1][j - 1];
						u[3] = block_image[i_row][i_col][i - 1][j];
						u[4] = block_image[i_row][i_col][i - 2][j];
						break;
					case 3:
						u[1] = block_image[i_row][i_col][i][j - 1];
						u[2] = block_image[i_row][i_col][i - 1][j];
						u[3] = block_image[i_row][i_col][i - 1][j + 1];
						u[4] = block_image[i_row][i_col][i - 1][j + 2];
						break;
					case 4:
						u[1] = block_image[i_row][i_col][i][j - 1];
						u[2] = block_image[i_row][i_col][i - 1][j - 1];
						u[3] = block_image[i_row][i_col][i - 1][j];
						u[4] = block_image[i_row][i_col][i - 2][j - 1];
						break;	
					case 5:
						u[1] = block_image[i_row][i_col][i][j - 1];
						u[2] = block_image[i_row][i_col][i - 1][j - 1];
						u[3] = block_image[i_row][i_col][i - 1][j];
						u[4] = (block_image[i_row][i_col][i - 2][j - 2] + block_image[i_row][i_col][i - 1][j - 2]) / 2;  //自己构造
						break;	
					case 6:
						u[1] = block_image[i_row][i_col][i][j - 1];
						u[2] = block_image[i_row][i_col][i - 1][j - 1];
						u[3] = block_image[i_row][i_col][i - 1][j];
						u[4] = block_image[i_row][i_col][i - 1][j - 2];  //可自己构造
						break;	
					case 7:
						u[1] = block_image[i_row][i_col][i][j - 1];
						u[2] = block_image[i_row][i_col][i - 1][j - 1];
						u[3] = block_image[i_row][i_col][i - 1][j];
						u[4] = block_image[i_row][i_col][i - 1][j + 1];  //可自己构造
						break;	
					case 9:
						u[1] = block_image[i_row][i_col][i - 1][j - 1];
						u[2] = block_image[i_row][i_col][i - 2][j];
						u[3] = block_image[i_row][i_col][i - 1][j];
						u[4] = block_image[i_row][i_col][i - 1][j + 1];  //可自己构造
						break;
					default:
						break;
					}
					block_pre[i - bias_top_row][j - bias_left_col] = (short)(para[0][0] * u[1] + para[1][0]* u[2] + para[2][0] * u[3] + para[3][0] * u[4]);
					resi_temp[i - bias_top_row][j - bias_left_col] = block_image[i_row][i_col][i][j] - block_pre[i - bias_top_row][j - bias_left_col];//预测残差
					energy_temp = resi_temp[i - bias_top_row][j - bias_left_col] * resi_temp[i - bias_top_row][j - bias_left_col];
					block_energy += energy_temp;
				}
			}
			resi_energy += block_energy;

			printf("energe [%4d][%4d]  %4d\n", i_row, i_col, block_energy);
			printf("Accumulated energe [%4d][%4d]  %I64d\n", i_row, i_col, resi_energy);
			FILE *fout = fopen("block_energy.txt", "a+");
			assert(fout);
			if(fout != NULL)
				fprintf(fout, "%4d\n", block_energy);
			fclose(fout);
			//将当前块的预测值保存到整幅图像的预测矩阵中
			for (int i = 0; i < BLOCKHEIGHT; ++i)
			{
				for (int j = 0; j < BLOCKWIDTH; ++j)
				{
					//  resi[i_row][i_col][i][j] = block_resi[i_row][i_col][i][j];	严重错误！！！！
					predicted[i_row * BLOCKHEIGHT + i][i_col * BLOCKWIDTH + j] = block_pre[i][j];
				}
			}
			//将当前块的残差值保存到整幅图像的残差矩阵中
			for (int i = 0; i < BLOCKHEIGHT; ++i)
			{
				for (int j = 0; j < BLOCKWIDTH; ++j)
				{
					//  resi[i_row][i_col][i][j] = block_resi[i_row][i_col][i][j];	严重错误！！！！
					resi[i_row * BLOCKHEIGHT + i][i_col * BLOCKWIDTH + j] = resi_temp[i][j];
				}
			}
			// 对得到的最优的残差块进行变换、量化、逆量化、逆变换，
			/**************************************************************
	         * 3. transform and quant the residual of prediction
	         *************************************************************/
	  
			DCT_Quanter(resi_temp, outdataDct_Quant);
			//DCT_Quanter后的结果与预测值进行相加得到重建块，重建块的值拷贝到image_construct中当前块对应位置处，覆盖掉原始图像，
			//为下一次循环读取下一个块的邻近重建像素值做准备
			for (int i = 0; i < BLOCKHEIGHT; ++i)
			{
				for (int j = 0; j < BLOCKWIDTH; ++j)
				{
					//block_construct[i_row][i_col][i][j] = block_resi[i_row][i_col][i][j] + outdataDct_Quant[i_row][i_col][i][j];
					//image_construct[i_row][i_col][i + 1][j + 1] = block_construct[i_row][i_col][i][j]; 严重错误！！中间状态不需要索引块坐标，只需要它的值
					block_construct[i][j] = block_pre[i][j] + outdataDct_Quant[i][j];
					image_construct[i_row * BLOCKHEIGHT + i][i_col * BLOCKWIDTH + j] = block_construct[i][j];
				}
			}
		}
	}
	return resi_energy;
}
long long predict_LeftDown_four(short **image_construct, short **resi, short **predicted, double **para, int height, int width, int mode)
{
	// 图像残差块、变换量化后的图像块、的内存空间分配
	short outdataDct_Quant[BLOCKWIDTH][BLOCKHEIGHT] = {0};		  //每一个小块的变换量化后的值
	short block_construct[BLOCKWIDTH][BLOCKHEIGHT] = {0};		 //每一个小块的重建值
	short block_pre[BLOCKWIDTH][BLOCKHEIGHT] = {0};				 //每一个小块的预测值
	long long resi_energy = 0;					      // 所有已处理块的累计残差能量
	int energy_temp = 0;                              //每一个小块的各像素的残差能量值
	short resi_temp[BLOCKWIDTH][BLOCKHEIGHT] = {0};                 //块残差数据                 
	short block_image[ROWS][COLS][padded_imageblock_height][padded_imageblock_width] = {0}; 
	short smallimage[ROWS][COLS][padded_imageblock_height][padded_imageblock_width] = {0};     

	for (int i_col = 0; i_col < COLS; ++i_col)    //遍历顺序很重要！！！！！从上到下，从第一列到最后一列
	{
		for (int i_row = 0; i_row < ROWS; ++i_row)
		{
			// 将输入图像读成 8x8 的图像块（边界扩展后），赋值给用来存储每一种预测模式输入图像的数组
			// 
			for (int i = 0; i < BLOCKHEIGHT + bias_top_row + bias_down_row; ++i)
			{
				for (int j = 0; j < BLOCKWIDTH + bias_left_col + bias_right_col; ++j)
				{
					smallimage[i_row][i_col][i][j] = image_construct[i_row * BLOCKHEIGHT + i] [i_col * BLOCKWIDTH + j];
					block_image[i_row][i_col][i][j] = smallimage[i_row][i_col][i][j];
				}
			}
			//将图像块的右边两列和下边两行的值赋值成128
			for (int r = 0; r < BLOCKHEIGHT + bias_top_row + bias_down_row; ++r)
			{
				for (int c = 0; c < BLOCKWIDTH + bias_left_col + bias_right_col; ++c)
				{
					//if((r > BLOCKHEIGHT + 1 && c > 2) || c > BLOCKWIDTH + 1)
					if((r > BLOCKHEIGHT + bias_top_row - 1 && c > bias_left_col) || c > BLOCKWIDTH + bias_left_col - 1)
						block_image[i_row][i_col][r][c] = 128;
						//image_mode3[r][c] = 128;
						//image_mode4[r][c] = 128;
						//image_mode5[r][c] = 128;
						//image_mode6[r][c] = 128;
						//image_mode7[r][c] = 128;
						//image_mode8[r][c] = 128;  
				}
			}
			int block_energy = 0;
			static short u[5] = {0};
			//8模式可以用下面公式计算
			for(int i = bias_top_row; i < BLOCKHEIGHT + bias_top_row; ++i)
			{
				for(int j = bias_left_col; j < BLOCKWIDTH + bias_left_col; ++j)
				{
					switch (mode)
					{
						case 1:
						u[1] = block_image[i_row][i_col][i - 2][j - 1];
						u[2] = block_image[i_row][i_col][i - 1][j - 1];
						u[3] = block_image[i_row][i_col][i][j - 1];
						u[4] = block_image[i_row][i_col][i + 1][j - 1];
						break;
						case 8:
							u[1] = block_image[i_row][i_col][i - 1][j];
							u[2] = block_image[i_row][i_col][i - 1][j - 1];
							u[3] = block_image[i_row][i_col][i][j - 1];
							u[4] = block_image[i_row][i_col][i + 1][j - 1];  //可自己构造
							break;
							//case 10:
							//	u[1] = block_image[i_row][i_col][i - 1][j];
							//	u[2] = block_image[i_row][i_col][i - 1][j - 1];
							//	u[3] = block_image[i_row][i_col][i][j - 1];
							//	u[4] = block_image[i_row][i_col][i + 1][j - 1];  //可自己构造
							//	break;
						default:
							break;
					}

					block_pre[i - bias_top_row][j - bias_left_col] = (short)(para[0][0] * u[1] + para[1][0]* u[2] + para[2][0] * u[3] + para[3][0] * u[4]);
					resi_temp[i - bias_top_row][j - bias_left_col] = block_image[i_row][i_col][i][j] - block_pre[i - bias_top_row][j - bias_left_col];//预测残差
					energy_temp = resi_temp[i - bias_top_row][j - bias_left_col] * resi_temp[i - bias_top_row][j - bias_left_col];
					block_energy += energy_temp;
				}
			}
			resi_energy += block_energy;

			printf("energe [%4d][%4d]  %4d\n", i_row, i_col, block_energy);
			printf("Accumulated energe [%4d][%4d]  %I64d\n", i_row, i_col, resi_energy);
			FILE *fout = fopen("block_energy.txt", "a+");
			assert(fout);
			if(fout != NULL)
				fprintf(fout, "%4d\n", block_energy);
			fclose(fout);
			//将当前块的预测值保存到整幅图像的预测矩阵中
			for (int i = 0; i < BLOCKHEIGHT; ++i)
			{
				for (int j = 0; j < BLOCKWIDTH; ++j)
				{
					//  resi[i_row][i_col][i][j] = block_resi[i_row][i_col][i][j];	严重错误！！！！
					predicted[i_row * BLOCKHEIGHT + i][i_col * BLOCKWIDTH + j] = block_pre[i][j];
				}
			}
			//将当前块的残差值保存到整幅图像的残差矩阵中
			for (int i = 0; i < BLOCKHEIGHT; ++i)
			{
				for (int j = 0; j < BLOCKWIDTH; ++j)
				{
					//  resi[i_row][i_col][i][j] = block_resi[i_row][i_col][i][j];	严重错误！！！！
					resi[i_row * BLOCKHEIGHT + i][i_col * BLOCKWIDTH + j] = resi_temp[i][j];
				}
			}
			// 对得到的最优的残差块进行变换、量化、逆量化、逆变换，
			/**************************************************************
	         * 3. transform and quant the residual of prediction
	         *************************************************************/
	  
			DCT_Quanter(resi_temp, outdataDct_Quant);
			//DCT_Quanter后的结果与预测值进行相加得到重建块，重建块的值拷贝到image_construct中当前块对应位置处，覆盖掉原始图像，
			//为下一次循环读取下一个块的邻近重建像素值做准备
			for (int i = 0; i < BLOCKHEIGHT; ++i)
			{
				for (int j = 0; j < BLOCKWIDTH; ++j)
				{
					//block_construct[i_row][i_col][i][j] = block_resi[i_row][i_col][i][j] + outdataDct_Quant[i_row][i_col][i][j];
					//image_construct[i_row][i_col][i + 1][j + 1] = block_construct[i_row][i_col][i][j]; 严重错误！！中间状态不需要索引块坐标，只需要它的值
					block_construct[i][j] = block_pre[i][j] + outdataDct_Quant[i][j];
					image_construct[i_row * BLOCKHEIGHT + i][i_col * BLOCKWIDTH + j] = block_construct[i][j];

				}
			}
		}
	}
	return resi_energy;
}
