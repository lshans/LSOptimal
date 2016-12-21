#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <malloc.h>
#include <cassert>
#include <iostream>
#include "common.h"
#include "canny.h"
using namespace std;
const double PI = 3.14159265;
typedef long LONG;
struct SIZE{
	int cy; 
	int cx;
	SIZE(): cy(0), cx(0){ }
};


//  一维高斯分布函数，用于平滑函数中生成的高斯滤波系数
void CreatGauss(double sigma, double **pdKernel, int *pnWidowSize)

{
	LONG i;

	int nCenter;//数组中心点
	double dDis;//数组中一点到中心点距离

	//中间变量
	double dValue = 0.0;
	double dSum = 0.0;

	*pnWidowSize = 1+ 2 * ceil(3 * sigma);// [-3*sigma,3*sigma] 以内数据，会覆盖绝大部分滤波系数  ceil返回大于等于表达式的最小整数

	nCenter = (*pnWidowSize)/2;

	//生成高斯数据
	for(i=0;i<(*pnWidowSize);i++)
	{
		dDis = (double)(i - nCenter);
		dValue = exp(-(1.0 / 2) * dDis * dDis / (sigma * sigma)) / (sqrt(2 * 3.1415926) * sigma);//
		(*pdKernel)[i] = dValue;
		dSum += dValue;
		//printf("pdKernel is %10.9f\n", (*pdKernel)[i]);
	}
	//printf("dSum is %10.9f\n", dSum);

	//归一化
	for(i=0;i<(*pnWidowSize);i++)
	{
		(*pdKernel)[i]/=dSum;
		//printf("pdKernel is %10.9f\n", (*pdKernel)[i]);
	}
}



//用高斯滤波器平滑原图像
void GaussianSmooth(SIZE sz, LPBYTE pGray, LPBYTE pResult, double sigma)
{
	//LONG x = 0, y = 0;
	//LONG i = 0;

	int nWindowSize = 0;//高斯滤波器长度

	int nLen = 0;//窗口长度

	double dDotMul = 0.0;//高斯系数与图像数据的点乘

	double dWeightSum = 0.0;//滤波系数总和 

	double *pdTemp = new double[sz.cx * sz.cy]();

	nWindowSize = 1+ 2*ceil(3*sigma);// [-3*sigma,3*sigma] 以内数据，会覆盖绝大部分滤波系数
	double *pdKernel = new double[nWindowSize + 1]();//一维高斯滤波器
	//产生一维高斯数据
	CreatGauss(sigma, &pdKernel, &nWindowSize);

	nLen = nWindowSize/2;

	//x方向滤波
	for(int y=0;y<sz.cy;y++)
	{
		for(int x=0;x<sz.cx;x++)
		{
			dDotMul = 0;
			dWeightSum = 0;
			for(int i = (-nLen); i <= nLen; i++)
			{
				//判断是否在图像内部
				if((i + x) >= 0 && (i + x) < sz.cx)
				{
					dDotMul += (double)(pGray[y * sz.cx+(i + x)] * pdKernel[nLen + i]);
					dWeightSum += pdKernel[nLen+i];
				}
			}
			pdTemp[y * sz.cx + x] = dDotMul / dWeightSum;
		}
	}

	//y方向滤波
	for(int x=0; x<sz.cx;x++)
	{
		for(int y=0; y<sz.cy; y++)
		{
			dDotMul = 0;
			dWeightSum = 0;
			for(int i=(-nLen);i<=nLen;i++)
			{
				if((i+y)>=0 && (i+y)< sz.cy)
				{
					dDotMul += (double)pdTemp[(y+i)*sz.cx+x]*pdKernel[nLen+i];
					dWeightSum += pdKernel[nLen+i];
				}
			}
			pResult[y*sz.cx+x] = (short)dDotMul/dWeightSum;
			//std::cout << pResult[y*sz.cx+x] << " ";
		}
		//std::cout << std::endl;
	}
	delete []pdKernel; 
	pdKernel = NULL; 

	delete []pdTemp; 
	pdTemp = NULL; 
}



// 方向导数,求梯度
void Grad(SIZE sz, LPBYTE pGray,int *pGradX, int *pGradY, int *pMag)
{
	//注意：原图进行了边界扩展，所以计算时，索引坐标从1开始，sz.cy-1结束，同理对于列坐标
	//LONG y,x;

	//中间变量
	double dSqt1;
	double dSqt2;
	int x_sobel[3][3];
	for(int ii =0;ii<3;ii++)
		for(int jj=0;jj<3;jj++)
			x_sobel[ii][jj] = 0;
	x_sobel[0][0] = -1;
	x_sobel[0][2] = 1;
	x_sobel[1][0] = -2;
	x_sobel[1][2] = 2;
	x_sobel[2][0] = -1;
	x_sobel[2][2] = 1;
	//x方向的方向导数
	for(int y=1;y<sz.cy-1;y++)
	{
		for(int x=1;x<sz.cx-1;x++)
		{
			pGradX[y * sz.cx + x] = (int)(pGray[(y - 1) * sz.cx + x + 1] + 2 * pGray[y * sz.cx + x + 1] + pGray[(y + 1)*sz.cx + x + 1] - pGray[(y - 1) * sz.cx + x - 1] - 2 * pGray[y * sz.cx + x - 1] - pGray[(y + 1)*sz.cx + x - 1]);

		}
	}
	int y_sobel[3][3];
	for(int ii =0;ii<3;ii++)
		for(int jj=0;jj<3;jj++)
			y_sobel[ii][jj] = 0;
	y_sobel[0][0] = 1;
	y_sobel[0][1] = 2;
	y_sobel[0][2] = 1;
	y_sobel[2][0] = -1;
	y_sobel[2][1] = -2;
	y_sobel[2][2] = -1;
	//y方向方向导数
	for(int y = 1; y < sz.cy - 1; y++)
	{
		for(int x = 1; x < sz.cx - 1; x++)
		{
			pGradY[y * sz.cx + x] = (int)(pGray[(y - 1) * sz.cx + x - 1] + 2 * pGray[(y - 1) * sz.cx + x] + pGray[(y - 1)*sz.cx + x + 1] - pGray[(y + 1) * sz.cx + x - 1] - 2 * pGray[(y + 1) * sz.cx + x] - pGray[(y + 1)*sz.cx + x + 1]);
		}
	}

	//求梯度
	for(int y = 1; y < sz.cy - 1; y++)
	{
		for(int x = 1; x < sz.cx - 1; x++)
		{
			//二阶范数求梯度
			dSqt1 = pGradX[y * sz.cx + x] * pGradX[y * sz.cx + x];
			dSqt2 = pGradY[y * sz.cx + x] * pGradY[y * sz.cx + x];
			pMag[y * sz.cx + x] = (int)(sqrt(dSqt1 + dSqt2) + 0.5);
		}
	}

}

/*************************************************************************
*
* \函数名称：
*   Canny()
*
* \输入参数:
*   unsigned char *pGray- 图象数据
*         int nWidth               - 图象数据宽度
*         int nHeight              - 图象数据高度
*   double sigma             - 高斯滤波的标准方差
*         double dRatioLow         - 低阈值和高阈值之间的比例
*         double dRatioHigh        - 高阈值占图象象素总数的比例
*   unsigned char *pResult - canny算子计算后的分割图
*
* \返回值:
*   无
*
* \说明:
*   canny分割算子，计算的结果保存在pUnchEdge中，逻辑1(255)表示该点为
*   边界点，逻辑0(0)表示该点为非边界点。该函数的参数sigma，dRatioLow
*   dRatioHigh，是需要指定的。这些参数会影响分割后边界点数目的多少
*************************************************************************
*/

void Canny(LPBYTE pGray, double sigma, int block_height, int block_width, block &block_label)
{
	SIZE sz;
	sz.cy = block_height;
	sz.cx = block_width;

	//经过高斯滤波后的图像
	LPBYTE pGaussSmooth;
	pGaussSmooth = new short[sz.cx*sz.cy]();
	//x方向导数的指针
	int *pGradX = new int[sz.cx*sz.cy]();
	//y方向
	int *pGradY  = new int[sz.cx*sz.cy]();
	//梯度的幅度
	int *pGradMag = new int[sz.cx*sz.cy]();

	//对原图高斯滤波
	GaussianSmooth(sz, pGray, pGaussSmooth,sigma);

	//计算方向导数和梯度的幅度
	Grad(sz, pGaussSmooth, pGradX, pGradY, pGradMag);
	//注意：原图进行了边界扩展，所以赋值给块结构体时，索引坐标从1开始，sz.cy-1结束，同理对于列坐标
	for(int i = 1; i < sz.cy - 1; ++i)
	{
		for(int j = 1; j < sz.cx - 1; ++j)
		{
			block_label.pGradX[i - 1][j - 1] = pGradX[i * sz.cx + j];
			block_label.pGradY[i - 1][j - 1] = pGradY[i * sz.cx + j];
			block_label.pGrad_amplitude[i - 1][j - 1] = pGradMag[i * sz.cx + j];
		}
	}
	//将各像素的梯度角量化成0~15之间的值
	//注意：原图进行了边界扩展，所以计算时，索引坐标从1开始，sz.cy-1结束，同理对于列坐标
	int grad_angle_quantify[BLOCKWIDTH][BLOCKHEIGHT] = {0};
	const int amplitude_threshold = 10;
	double quantify_step = PI / 8;
	for(int i = 1; i < sz.cy - 1; ++i)
	{
		for(int j = 1; j < sz.cx - 1; ++j)
		{
			//std::cout << pGradY[i * sz.cx + j] << " " << i << " " << j << std::endl;
			//std::cout << pGradY[i * sz.cx + j] << " " << i << " " << j << std::endl;

			// 判断幅值
			const int default_value = 15;
			const int grad_x = pGradX[i * sz.cx + j];
			const int grad_y = pGradY[i * sz.cx + j];
			double amplitude = abs(grad_y) + abs(grad_x);
			if(amplitude < amplitude_threshold) {
				grad_angle_quantify[i - 1][j - 1] = default_value;
				continue;
			}
			// 判断角度
			double angle = 0.0f;
			if(grad_x == 0)
			{
				if(grad_y > 0)
					angle = PI / 2;
				else if(grad_y < 0)
					angle = -PI / 2;
				else
					angle = 0.0f;
			}
			else
				angle = atan2((double)grad_y, (double)grad_x);

			if (angle < 0)
				angle += 2 * PI;
			grad_angle_quantify[i - 1][j - 1] = (int)(angle / quantify_step);
		}
	}
	//并统计各种值的像素个数直方图
	for(int i = 0; i < BLOCKHEIGHT; ++i)
	{
		for(int j = 0; j < BLOCKWIDTH; ++j)
		{
			assert(grad_angle_quantify[i][j] >= 0 && grad_angle_quantify[i][j] < 16);
			++block_label.hist[grad_angle_quantify[i][j]];
		}
	}
	//判断图像块的主梯度方向
	int max_hist_num = block_label.hist[0];
	block_label.primer_grad_direction = 0;
	for(int k = 1; k < 16; ++k)
	{
		if(block_label.hist[k] > max_hist_num)
		{
			max_hist_num = block_label.hist[k];
			block_label.primer_grad_direction = k;
		}
	}

	//for(int i = 0; i < sz.cy; ++i)
	//{
	//	for(int j = 0; j < sz.cx; ++j)
	//	{
	//		if(pGradX[i * sz.cx + j] == 0)
	//			hist[0] += 1;            //pGradMag[i * sz.cx + j];
	//		//else if(pGradY[i * sz.cx + j] != 0 && pGradX[i * sz.cx + j] == 0)
	//			//hist[0] += pGradMag[i * sz.cx + j];
	//		else if(abs(pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) < 0.2364)
	//			hist[1] += 1;               //pGradMag[i * sz.cx + j];
	//		else if((pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) > 0.2364 && (pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) < 0.7212)
	//			hist[6] += 1;                //pGradMag[i * sz.cx + j];
	//		else if((pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) > 0.7212 && (pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) < 1.3865)
	//			hist[4] += 1;               //pGradMag[i * sz.cx + j];
	//		else if((pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) > 1.3865 && (pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) < 4.2303)
	//			hist[5] += 1;               //pGradMag[i * sz.cx + j];
	//		else if(abs(pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) > 4.2303 || (pGradY[i * sz.cx + j] != 0 && pGradX[i * sz.cx + j] == 0))
	//			hist[0] += 1;               //pGradMag[i * sz.cx + j];
	//		else if((pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) > -4.2303 && (pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) < -1.3865)
	//			hist[7] += 1;                //pGradMag[i * sz.cx + j];
	//		else if((pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) > -1.3865 && (pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) < -0.7212)
	//			hist[3] += 1;                //pGradMag[i * sz.cx + j];
	//		else if((pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) > -0.7212 && (pGradY[i * sz.cx + j] / pGradX[i * sz.cx + j]) < -0.2364)
	//			hist[8] += 1;             // pGradMag[i * sz.cx + j];
	//		printf("%d ",pGradMag[i * sz.cx + j]);
	//	}
	//	printf("\n");
	//}

	delete []pGaussSmooth; 
	pGaussSmooth = NULL;
	delete []pGradX; 
	pGradX = NULL;
	delete []pGradY; 
	pGradY = NULL;
	delete []pGradMag; 
	pGradMag = NULL;

}