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


//  һά��˹�ֲ�����������ƽ�����������ɵĸ�˹�˲�ϵ��
void CreatGauss(double sigma, double **pdKernel, int *pnWidowSize)

{
	LONG i;

	int nCenter;//�������ĵ�
	double dDis;//������һ�㵽���ĵ����

	//�м����
	double dValue = 0.0;
	double dSum = 0.0;

	*pnWidowSize = 1+ 2 * ceil(3 * sigma);// [-3*sigma,3*sigma] �������ݣ��Ḳ�Ǿ��󲿷��˲�ϵ��  ceil���ش��ڵ��ڱ��ʽ����С����

	nCenter = (*pnWidowSize)/2;

	//���ɸ�˹����
	for(i=0;i<(*pnWidowSize);i++)
	{
		dDis = (double)(i - nCenter);
		dValue = exp(-(1.0 / 2) * dDis * dDis / (sigma * sigma)) / (sqrt(2 * 3.1415926) * sigma);//
		(*pdKernel)[i] = dValue;
		dSum += dValue;
		//printf("pdKernel is %10.9f\n", (*pdKernel)[i]);
	}
	//printf("dSum is %10.9f\n", dSum);

	//��һ��
	for(i=0;i<(*pnWidowSize);i++)
	{
		(*pdKernel)[i]/=dSum;
		//printf("pdKernel is %10.9f\n", (*pdKernel)[i]);
	}
}



//�ø�˹�˲���ƽ��ԭͼ��
void GaussianSmooth(SIZE sz, LPBYTE pGray, LPBYTE pResult, double sigma)
{
	//LONG x = 0, y = 0;
	//LONG i = 0;

	int nWindowSize = 0;//��˹�˲�������

	int nLen = 0;//���ڳ���

	double dDotMul = 0.0;//��˹ϵ����ͼ�����ݵĵ��

	double dWeightSum = 0.0;//�˲�ϵ���ܺ� 

	double *pdTemp = new double[sz.cx * sz.cy]();

	nWindowSize = 1+ 2*ceil(3*sigma);// [-3*sigma,3*sigma] �������ݣ��Ḳ�Ǿ��󲿷��˲�ϵ��
	double *pdKernel = new double[nWindowSize + 1]();//һά��˹�˲���
	//����һά��˹����
	CreatGauss(sigma, &pdKernel, &nWindowSize);

	nLen = nWindowSize/2;

	//x�����˲�
	for(int y=0;y<sz.cy;y++)
	{
		for(int x=0;x<sz.cx;x++)
		{
			dDotMul = 0;
			dWeightSum = 0;
			for(int i = (-nLen); i <= nLen; i++)
			{
				//�ж��Ƿ���ͼ���ڲ�
				if((i + x) >= 0 && (i + x) < sz.cx)
				{
					dDotMul += (double)(pGray[y * sz.cx+(i + x)] * pdKernel[nLen + i]);
					dWeightSum += pdKernel[nLen+i];
				}
			}
			pdTemp[y * sz.cx + x] = dDotMul / dWeightSum;
		}
	}

	//y�����˲�
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



// ������,���ݶ�
void Grad(SIZE sz, LPBYTE pGray,int *pGradX, int *pGradY, int *pMag)
{
	//ע�⣺ԭͼ�����˱߽���չ�����Լ���ʱ�����������1��ʼ��sz.cy-1������ͬ�����������
	//LONG y,x;

	//�м����
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
	//x����ķ�����
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
	//y��������
	for(int y = 1; y < sz.cy - 1; y++)
	{
		for(int x = 1; x < sz.cx - 1; x++)
		{
			pGradY[y * sz.cx + x] = (int)(pGray[(y - 1) * sz.cx + x - 1] + 2 * pGray[(y - 1) * sz.cx + x] + pGray[(y - 1)*sz.cx + x + 1] - pGray[(y + 1) * sz.cx + x - 1] - 2 * pGray[(y + 1) * sz.cx + x] - pGray[(y + 1)*sz.cx + x + 1]);
		}
	}

	//���ݶ�
	for(int y = 1; y < sz.cy - 1; y++)
	{
		for(int x = 1; x < sz.cx - 1; x++)
		{
			//���׷������ݶ�
			dSqt1 = pGradX[y * sz.cx + x] * pGradX[y * sz.cx + x];
			dSqt2 = pGradY[y * sz.cx + x] * pGradY[y * sz.cx + x];
			pMag[y * sz.cx + x] = (int)(sqrt(dSqt1 + dSqt2) + 0.5);
		}
	}

}

/*************************************************************************
*
* \�������ƣ�
*   Canny()
*
* \�������:
*   unsigned char *pGray- ͼ������
*         int nWidth               - ͼ�����ݿ��
*         int nHeight              - ͼ�����ݸ߶�
*   double sigma             - ��˹�˲��ı�׼����
*         double dRatioLow         - ����ֵ�͸���ֵ֮��ı���
*         double dRatioHigh        - ����ֵռͼ�����������ı���
*   unsigned char *pResult - canny���Ӽ����ķָ�ͼ
*
* \����ֵ:
*   ��
*
* \˵��:
*   canny�ָ����ӣ�����Ľ��������pUnchEdge�У��߼�1(255)��ʾ�õ�Ϊ
*   �߽�㣬�߼�0(0)��ʾ�õ�Ϊ�Ǳ߽�㡣�ú����Ĳ���sigma��dRatioLow
*   dRatioHigh������Ҫָ���ġ���Щ������Ӱ��ָ��߽����Ŀ�Ķ���
*************************************************************************
*/

void Canny(LPBYTE pGray, double sigma, int block_height, int block_width, block &block_label)
{
	SIZE sz;
	sz.cy = block_height;
	sz.cx = block_width;

	//������˹�˲����ͼ��
	LPBYTE pGaussSmooth;
	pGaussSmooth = new short[sz.cx*sz.cy]();
	//x��������ָ��
	int *pGradX = new int[sz.cx*sz.cy]();
	//y����
	int *pGradY  = new int[sz.cx*sz.cy]();
	//�ݶȵķ���
	int *pGradMag = new int[sz.cx*sz.cy]();

	//��ԭͼ��˹�˲�
	GaussianSmooth(sz, pGray, pGaussSmooth,sigma);

	//���㷽�������ݶȵķ���
	Grad(sz, pGaussSmooth, pGradX, pGradY, pGradMag);
	//ע�⣺ԭͼ�����˱߽���չ�����Ը�ֵ����ṹ��ʱ�����������1��ʼ��sz.cy-1������ͬ�����������
	for(int i = 1; i < sz.cy - 1; ++i)
	{
		for(int j = 1; j < sz.cx - 1; ++j)
		{
			block_label.pGradX[i - 1][j - 1] = pGradX[i * sz.cx + j];
			block_label.pGradY[i - 1][j - 1] = pGradY[i * sz.cx + j];
			block_label.pGrad_amplitude[i - 1][j - 1] = pGradMag[i * sz.cx + j];
		}
	}
	//�������ص��ݶȽ�������0~15֮���ֵ
	//ע�⣺ԭͼ�����˱߽���չ�����Լ���ʱ�����������1��ʼ��sz.cy-1������ͬ�����������
	int grad_angle_quantify[BLOCKWIDTH][BLOCKHEIGHT] = {0};
	const int amplitude_threshold = 10;
	double quantify_step = PI / 8;
	for(int i = 1; i < sz.cy - 1; ++i)
	{
		for(int j = 1; j < sz.cx - 1; ++j)
		{
			//std::cout << pGradY[i * sz.cx + j] << " " << i << " " << j << std::endl;
			//std::cout << pGradY[i * sz.cx + j] << " " << i << " " << j << std::endl;

			// �жϷ�ֵ
			const int default_value = 15;
			const int grad_x = pGradX[i * sz.cx + j];
			const int grad_y = pGradY[i * sz.cx + j];
			double amplitude = abs(grad_y) + abs(grad_x);
			if(amplitude < amplitude_threshold) {
				grad_angle_quantify[i - 1][j - 1] = default_value;
				continue;
			}
			// �жϽǶ�
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
	//��ͳ�Ƹ���ֵ�����ظ���ֱ��ͼ
	for(int i = 0; i < BLOCKHEIGHT; ++i)
	{
		for(int j = 0; j < BLOCKWIDTH; ++j)
		{
			assert(grad_angle_quantify[i][j] >= 0 && grad_angle_quantify[i][j] < 16);
			++block_label.hist[grad_angle_quantify[i][j]];
		}
	}
	//�ж�ͼ�������ݶȷ���
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