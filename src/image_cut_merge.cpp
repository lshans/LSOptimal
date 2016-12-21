#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <memory>
#include "common.h"
#include "canny.h"
struct block block_label[ROWS][COLS];
int min(int a, int b)
{
	return a < b ? a : b;
}
void image_cut_merge(short **image, int height, int width)
{
	//��ý��߽���չ�����һ������
	short **img_padding = NULL; //�洢��չ�����������������е�����ͼ����
	img_padding = (short **)calloc(height + 2, sizeof(short *));
	for(int i = 0; i < height + 2; i++)
		img_padding[i] = (short *)calloc(width + 2,sizeof(short));
	//for(int r = 0; r < height + 2; r++)    
	//{
	//	for(int c = 0; c < width + 2; c++)
	//	{
	//		if(r == 0)
	//		{
	//			if(c > 0 || c < width + 1)
	//				img_padding[r][c] = image[r][c - 1];
	//			else if(c == 0)
	//				img_padding[r][c] = image[r][c];
	//			else if(c == width + 1)
	//				img_padding[r][c] = image[r][c - 2];
	//		}
	//		else if(r > 0 || r < height + 1)
	//		{
	//			if(c == 0)
	//				img_padding[r][c] = image[r - 1][c];
	//			else if(c == width + 1)
	//				img_padding[r][c] = image[r - 1][c - 2];
	//			else
	//				img_padding[r][c] = image[r - 1][c - 1];
	//		}
	//		else if(r == height + 1)
	//		{
	//			if(c == 0)
	//				img_padding[r][c] = image[r - 2][c];
	//			else if(c == width + 1)
	//				img_padding[r][c] = image[r - 2][c - 2];
	//			else
	//				img_padding[r][c] = image[r - 2][c - 2];
	//		}
	//	}
	//}
	/// ��ԭͼ���¸���չһ�У����Ҹ���չһ��
	// ����ԭͼ
	for (int r = 0; r < height; ++r) {
		for (int c = 0; c < width; ++c) {
			img_padding[r + 1][c + 1] = image[r][c];
		}
	}
	// ��չ�߽�
	for (int r = 0; r < height + 2; ++r) {
		for (int c = 0; c < width + 2; ++c) {
			if (r == 0) {
				img_padding[r][c] = img_padding[r + 1][c];
			} else if (c == 0) {
				img_padding[r][c] = img_padding[r][c + 1];
			} else if (r == height + 2 - 1) {
				img_padding[r][c] = img_padding[r - 1][c];
			} else if (c == width + 2 - 1) {
				img_padding[r][c] = img_padding[r][c - 1];
			} else
				continue;
		}
	}
	// ����ĸ���
	img_padding[0][0] = image[0][0];
	img_padding[height + 2 - 1][0] = image[height - 1][0];
	img_padding[0][width + 2 - 1] = image[0][width - 1];
	img_padding[height + 2 - 1][width + 2 - 1] = image[height - 1][width - 1];
	//////////////////////////////////////////////////////////////////////////
	short block_image[ROWS][COLS][(BLOCKHEIGHT + 2)* (BLOCKWIDTH + 2)] = {0}; 
	short smallimage[ROWS][COLS][BLOCKHEIGHT + 2][BLOCKWIDTH + 2] = {0};
	for (int i_row = 0; i_row < ROWS; ++i_row)
	{
		for (int i_col = 0; i_col < COLS; ++i_col)
		{

			int start_r = i_row * BLOCKWIDTH + 1 - 1; //������֮��������ʼλ��
			int start_c = i_col * BLOCKWIDTH + 1 - 1; //������֮��������ʼλ��
			for (int r = 0; r < BLOCKHEIGHT + 2; ++r)
			{
				for (int c = 0; c < BLOCKWIDTH + 2; ++c)
				{

					smallimage[i_row][i_col][r][c] = img_padding[start_r + r][start_c + c];
					block_image[i_row][i_col][r * (BLOCKWIDTH + 2) + c] = smallimage[i_row][i_col][r][c];
				}
			}
			Canny(block_image[i_row][i_col], 1, BLOCKHEIGHT + 2, BLOCKWIDTH + 2, block_label[i_row][i_col]);
			//��ӡ����ͼ�����ݶ�ֱ��ͼ
			FILE *fout = fopen("block_label_hist.txt", "a+");
			assert(fout);
			if(fout != NULL)
			{
				fprintf(fout, "(%d,%d) block grad_direction hist is:\n", i_row, i_col);
				for(int k = 0; k < 16; ++k)
					fprintf(fout, "%4d", block_label[i_row][i_col].hist[k]);
				fprintf(fout, "\n");
				fprintf(fout, "primer_grad_direction is: %2d\n", block_label[i_row][i_col].primer_grad_direction);
			}
			fclose(fout);
			block_label[i_row][i_col].block_size = 4;
		}
	}
	//��ӡһ��ͼ�������ص��ݶ�ֵ
	FILE *fout_pGradY_pGradX = fopen("pGradY_pGradX.txt", "a+");
	assert(fout_pGradY_pGradX);
	for(int i_row = 0; i_row < ROWS; ++i_row)
	{
		for(int i_col = 0; i_col < COLS; ++i_col)
		{
			fprintf(fout_pGradY_pGradX, "[%4d%4d]**********************\n", i_row, i_col);
			for(int i = 0; i < BLOCKHEIGHT; ++i)
			{
				for(int j = 0; j < BLOCKWIDTH; ++j)
					fprintf(fout_pGradY_pGradX, "%4d\t%4d\t%4d\t|", block_label[i_row][i_col].pGradX[i][j], block_label[i_row][i_col].pGradY[i][j], block_label[i_row][i_col].pGrad_amplitude[i][j]);
				fprintf(fout_pGradY_pGradX, "\n");
			}
		}
	}
	fclose(fout_pGradY_pGradX);
	//��ӡ����ͼ�������ݶȷ���
	FILE *fout = fopen("primer_grad_direction.txt", "a+");
	assert(fout);
	for(int i_row = 0; i_row < ROWS; ++i_row)
	{
		for(int i_col = 0; i_col < COLS; ++i_col)
			fprintf(fout, "[%3d %3d]:%4d ", i_row, i_col, block_label[i_row][i_col].primer_grad_direction);
		fprintf(fout, "\n");
	}
	fclose(fout);
	// �������ͼ�������Χͼ�������ƶ���Ϣֵ��Similarity �ڣ�0,1����Χ��
	for (int i_row = 0; i_row < ROWS - 1; ++i_row)
	{
		for (int i_col = 0; i_col < COLS - 1; ++i_col)
		{
			if(block_label[i_row][i_col].visited != 0)
				continue;
			for(int k = 0; k < 16; ++k)
			{
				block_label[i_row][i_col].Similarity[0] += min(block_label[i_row][i_col].hist[k], block_label[i_row][i_col + 1].hist[k]);
				block_label[i_row][i_col].Similarity[1] += min(block_label[i_row][i_col].hist[k], block_label[i_row + 1][i_col].hist[k]);
				block_label[i_row][i_col].Similarity[2] += min(block_label[i_row][i_col].hist[k], block_label[i_row + 1][i_col + 1].hist[k]);
			}
			block_label[i_row][i_col].Similarity[0] /= (BLOCKHEIGHT * BLOCKWIDTH * 1.0);
			block_label[i_row][i_col].Similarity[1] /= (BLOCKHEIGHT * BLOCKWIDTH * 1.0);
			block_label[i_row][i_col].Similarity[2] /= (BLOCKHEIGHT * BLOCKWIDTH * 1.0);
			//�ϲ���8x8�Ŀ�
			if(block_label[i_row][i_col].Similarity[0] > 0.6500 && block_label[i_row][i_col].Similarity[1] > 0.6500 && block_label[i_row][i_col].Similarity[2] > 0.6500)
			{
				block_label[i_row][i_col].visited = 1;
				block_label[i_row][i_col + 1].visited = 1;
				block_label[i_row + 1][i_col].visited = 1;
				block_label[i_row][i_col].block_size = 4;
				block_label[i_row][i_col + 1].block_size = 4;
				block_label[i_row + 1][i_col].block_size = 4;
			}
		}
	}
	//�ϲ���16x16�Ŀ�
	for (int i_row = 0; i_row < ROWS - 4; ++i_row)
	{
		for (int i_col = 0; i_col < COLS - 4; ++i_col)
		{
			if(block_label[i_row][i_col].visited == 1 && block_label[i_row][i_col + 2].visited == 1 && block_label[i_row + 2][i_col].visited == 1)
			{
				const int row_visited = i_row + 4;
				const int col_visited = i_col + 4;
				for(int i = i_row; i < row_visited; ++i)
				{
					for(int j = i_col; j < col_visited; ++j)
					{
						block_label[i_row][i_col].visited = 2;
						block_label[i_row][i_col].block_size = 16;
					}
				}
			}
		}
	}
	//��ӡ����ͼ�������Χͼ�������ƶ���Ϣֵ
	FILE *fout_block_Similarity = fopen("block_Similarity.txt", "a+");
	assert(fout_block_Similarity);
	for (int i_row = 0; i_row < ROWS; ++i_row)
	{
		for (int i_col = 0; i_col < COLS; ++i_col)
		{
			fprintf(fout_block_Similarity, "[%4d%4d]:(%5.4f\t%5.4f\t%5.4f)\t", i_row, i_col, block_label[i_row][i_col].Similarity[0], block_label[i_row][i_col].Similarity[1], block_label[i_row][i_col].Similarity[2]);
		}
		fprintf(fout_block_Similarity, "\n");
	}
	fclose(fout_block_Similarity);

}