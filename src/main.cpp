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
				if(endian == 0) //大端 
					img[i][j] = (short)(img_in[(i * width + j)*2] * 256 + img_in[(i * width + j) * 2 + 1]);
				else if(endian == 1) //小端
					img[i][j] = (short)(img_in[(i * width + j) * 2 + 1] * 256 + img_in[(i * width + j) * 2]); 
			}
	}
}
int main(int argc, char *argv[])
{
	const char* in_filepath = "./resources/wedge_shape_direction.txt";
	freopen("engery.txt", "w", stdout);
	
	int  height;						// 原图高
	int  width;							// 原图宽
	int  endian;						// 大小端
	int  precision;						// 原图比特精度，一般8bit
	
	int16_t LS_resi_energy = 0;			  //原始大图的预测残差能量
	short pixAverage = 0;				   //像素的平均值
	long long pixSum = 0;				   //像素的总和

	int   img_size = 0;						// 图像尺寸
	int   resi_size = 0;					// 残差矩阵尺寸

		
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

    /************************** 1. 读入命令行参数**************************/
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

    /********************** 2.  读入输入图像数据 argv[1]**************************/
	unsigned char *img_in = new unsigned char [img_size];		// 原始图像的输入
	//img_in = (unsigned char *)calloc(img_size, sizeof(short));//如果分配成功则返回指向被分配内存的指针，否则返回空指针NULL
	assert(img_in);
	//输入原始图像文件，将命令行的第一个参数，输入文件argv[1], 读入到一维指针 img_in指向的内存区域中
	ReadFile(argv[1], img_in, img_size, precision);

    /********************** 3.  图像数据大小端格式转换 **************************/	
	// 动态开辟空间， 存储原始图像进行大小端转换后的图像数据
	short **img = new short*[height];//如果分配成功则返回指向被分配内存的指针，否则返回空指针NULL
	memory_new(img, height, width);
	// 大小端格式转换，将一维指针 img_in指向的内存区域中的数据进行大小端格式转换，并存储到二维指针img指向的内存区域中
	transformat(img, img_in, height, width, endian, precision);
	printf("转换文件成功\n");

    /********************** 4. 对图像数据进行边界扩展**************************/
	short **imgLSConstructed = new short*[height + bias_top_row + bias_down_row];	 //利用LS方法得到原图相应块位置的重建值，存储利用重建值更新原图后的整幅图数据
	memory_new(imgLSConstructed, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);// 使用函数memory_calloc, 动态开辟内存空间存储数据
	//将输入图像值赋值给imgLSConstructed  重建图像数组，并进行bias_top_row, bias_down_row, bias_left_col, bias_right_col大小的上下左右边界扩展
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
	// 打印初始重建图矩阵进行调试
	print_short_matrix_to_file(imgLSConstructed, "initConstruct.txt", height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);
	// 打印原图矩阵与初始重建值矩阵进行比较进行调试
	print_short_matrix_to_file(img, "lena_origin.txt", height, width);

	/***********************5. 读入图像块方向信息文件并判断图像数据块的方向，进行块合并*******/
	short** direction_block = new short*[ROWS];
	memory_new(direction_block, ROWS, COLS);
	ReadFormatedFile(direction_block, in_filepath, ROWS, COLS);
	// TODO 由wrj来重构
	image_cut_merge(img, height, width);

	/********************** 6.  建立图像块纹理主方向查找表**************************///TODO!!!
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
	/********************** 7.  对不同的图像块类型建立马尔可夫预测模型进行预测**************************/
	short **preLS = new short*[height];		//原始大图的预测值
	memory_new(preLS, height, width);
	short **LS_resi = new short*[height];	//存储LS方法预测后得到的整幅图的残差数据
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
	printf("预测结束\n");

	// 打印残差矩阵进行调试
	print_short_matrix_to_file(LS_resi, "resi.txt", height, width);

	/********************** 8. 完成编码，打开输出码流文件进行回写**************************/
	unsigned char **pRes = new unsigned char*[height + bias_top_row + bias_down_row];//存储LS方法预测后得到的整幅图的残差数据
	memory_new(pRes, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);

	//转换成8bit
	for (int r = 0; r < height + bias_top_row + bias_down_row; ++r)
		for (int c = 0; c < width + bias_left_col + bias_right_col; ++c)
			pRes[r][c] = static_cast<unsigned char>(imgLSConstructed[r][c]);

	// 写到命令行输出文件参数中
	WriteFile(argv[2], pRes, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);	

	//打印最终的重建图像用于调试
	print_short_matrix_to_file(imgLSConstructed, "out_imgLSConstructed.txt", height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);
	printf("预测成功\n");

	/********************** 9. 动态分配的内存的释放**************************/
	memory_free(pRes, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);
	memory_free(imgLSConstructed, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);
	memory_free(LS_resi, height, width);
	memory_free(preLS, height, width);
	memory_free(img, height, width);
	memory_1dimension_free(img_in, height, width);
	
	return 0;
}
