//------------------------------------------------------------------  
//����: ���ò�����Ԫ�ĸ�˹��ȥ������A�������B  
//��ڲ���: ���뷽��������󣬷������  
//����ֵ: true or false  
//------------------------------------------------------------------- 
#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <iomanip>
#include <iostream>
#include "pre.h"
#include "common.h"

using namespace std; 
bool Gauss(double **A, double **B, int n)  
{  
	int i, j, k;  
	double max, temp;  
	double t[N][N];                //��ʱ����  
	//��A����������ʱ����t[n][n]��  
	for (i = 0; i < n; i++)  
	{  
		for (j = 0; j < n; j++)  
		{  
			t[i][j] = A[i][j];  
		}  
	}  
	//��ʼ��B����Ϊ��λ��  
	for (i = 0; i < n; i++)  
	{  
		for (j = 0; j < n; j++)  
		{  
			B[i][j] = (i == j) ? (double)1 : 0;  
		}  
	}  
	for (i = 0; i < n; i++)  
	{  
		//Ѱ����Ԫ  
		max = t[i][i];  
		k = i;  
		for (j = i + 1; j < n; j++)  
		{  
			if (fabs(t[j][i]) > fabs(max))  
			{  
				max = t[j][i];  
				k = j;  
			}  
		}  
		//�����Ԫ�����в��ǵ�i�У������н���  
		if (k != i)  
		{  
			for (j = 0; j < n; j++)  
			{  
				temp = t[i][j];  
				t[i][j] = t[k][j];  
				t[k][j] = temp;  
				//B���潻��  
				temp = B[i][j];  
				B[i][j] = B[k][j];  
				B[k][j] = temp;  
			}  
		}  
		//�ж���Ԫ�Ƿ�Ϊ0, ����, �����A�������Ⱦ���,�����������  
		if (t[i][i] == 0)  
		{  
			cout << "There is no inverse matrix!";  
			return false;  
		}  
		//��ȥA�ĵ�i�г�ȥi������ĸ���Ԫ��  
		temp = t[i][i];  
		for (j = 0; j < n; j++)  
		{  
			t[i][j] = t[i][j] / temp;        //���Խ����ϵ�Ԫ�ر�Ϊ1  
			B[i][j] = B[i][j] / temp;        //�������  
		}  
		for (j = 0; j < n; j++)        //��0��->��n��  
		{  
			if (j != i)                //���ǵ�i��  
			{  
				temp = t[j][i];  
				for (k = 0; k < n; k++)        //��j��Ԫ�� - i��Ԫ��*j��i��Ԫ��  
				{  
					t[j][k] = t[j][k] - t[i][k] * temp;  
					B[j][k] = B[j][k] - B[i][k] * temp;  
				}  
			}  
		}  
	}  
	return true;  
}