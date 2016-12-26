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
	
	int height;						// 原图高
	int width;						// 原图宽
	int endian;						// 大小端
	int precision;					// 原图比特精度，一般8bit
	int img_sz = 0;					// 图像尺寸
	
	int16_t LS_resi_energy = 0;		//原始大图的预测残差能量

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
	const std::string input_filename = argv[1];		// 输入原始图像文件
	const std::string output_filename = argv[2];	// 输出重建图像文件
	height = (int)atoi(argv[3]);
	width  = (int)atoi(argv[4]);
	endian = (int)atoi(argv[5]);
	precision = (int)atoi(argv[6]);
	img_sz = height * width;


    /********************** 2.  读入输入图像数据 argv[1]**************************/
	unsigned char *pImg = new unsigned char [img_sz];		// 原始图像的输入
	read_img(input_filename.c_str(), pImg, img_sz, precision);


    /********************** 3.  图像数据大小端格式转换 **************************/	
	short **imgConverted = new_short_mat(height, width);					// 原始图像进行大小端转换后的图像

	// 大小端格式统一转换
	transformat(imgConverted, pImg, height, width, endian, precision);
	printf("大小端格式转换完成.\n");
	delete [] pImg;	// 释放原图


    /********************** 4. 对图像数据进行边界扩展**************************/
	///利用LS方法得到原图相应块位置的重建值，存储利用重建值更新原图后的整幅图数据
	short **imgLSConstructed = new_short_mat(height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);

	//将输入图像值进行边界扩展
	img_padding(imgLSConstructed, imgConverted, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);

	// 打印扩展前后的图像
	print_short_matrix(imgConverted, "lena_origin.txt", height, width);
	print_short_matrix(imgLSConstructed, "initConstruct.txt", height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);
	delete_short_mat(imgConverted, height, width);


	/***********************5. 读入图像块方向信息文件并判断图像数据块的方向，进行块合并*******/
	//short** direction_block = new short*[ROWS];
	//new_short_mat(direction_block, ROWS, COLS);
	//ReadFormatedFile(direction_block, in_filepath, ROWS, COLS);
	//// TODO 由wrj来重构
	//image_cut_merge(img, height, width);


	/********************** 6.  建立图像块纹理主方向查找表**************************///TODO!!!
	std::map<int, int> direction_table = build_direction_search_table();

	int wedge_direction_index = 3;	// 使用第三个方向的楔形滤波器测试
	mode = search_direction(direction_table, wedge_direction_index);
	assert(mode != -1);


	/********************** 7.  对不同的图像块类型建立马尔可夫预测模型进行预测**************************/
	short **preLS = new_short_mat(height, width);		//原始大图的预测值
	short **LS_resi = new_short_mat(height, width);		//存储LS方法预测后得到的整幅图的残差数据

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
	printf("预测完成.\n");
	delete_short_mat(preLS, height, width);

	// 打印残差矩阵进行调试
	print_short_matrix(LS_resi, "resi.txt", height, width);
	delete_short_mat(LS_resi, height, width);


	/********************** 8. 完成编码，打开输出码流文件进行回写**************************/
	unsigned char **pRes = new_uchar_mat(height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);	//存储LS方法预测后得到的整幅图的残差数据

	// 将图像重建值转换成8bit图像，暂时不处理8bit以上图像
	for (int r = 0; r < height + bias_top_row + bias_down_row; ++r)
		for (int c = 0; c < width + bias_left_col + bias_right_col; ++c)
			pRes[r][c] = static_cast<unsigned char>(imgLSConstructed[r][c]);

	// 写到命令行输出文件参数中
	write_img(output_filename.c_str(), pRes, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);	
	delete_uchar_mat(pRes, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);


	//打印最终的重建图像
	print_short_matrix(imgLSConstructed, "out_imgLSConstructed.txt", height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);
	printf("裁剪有效区域完成.\n");
	delete_short_mat(imgLSConstructed, height, width, bias_top_row, bias_down_row, bias_left_col, bias_right_col);

	return 0;
}
