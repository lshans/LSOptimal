#ifndef COMMON_H
#define COMMON_H

#include <map>
using namespace std;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
const int bias_top_row = 3;              //每个块扩展的行数
const int bias_down_row = 3;             //每个块扩展的行数

const int bias_left_col = 3;             //每个块扩展的列数
const int bias_right_col = 3;             //每个块扩展的列数

const int BLOCKWIDTH = 4;		// 小图的宽度
const int BLOCKHEIGHT = 4;		// 小图的高度

const int padded_imageblock_height = BLOCKHEIGHT + bias_top_row + bias_down_row; //扩展后的图像块的高
const int padded_imageblock_width = BLOCKWIDTH + bias_left_col + bias_right_col; //扩展后的图像块的宽

const int ROWS = 128;	// 将图像分成小块，每一行的图像块数目
const int COLS = 128;	// 将图像分成小块，每一列的图像块数目
struct block{
	int       primer_grad_direction;
	int       visited;
	short     min_resi[4][4];
	short     min_pre[4][4];
	int       hist[16];
	int       block_size;
	int		  pGradX[4][4];
	int		  pGradY[4][4];
	int		  pGrad_amplitude[4][4];
	double    Similarity[3];  // Similarity 在（0,1）范围内
public:
	block() {
		primer_grad_direction = 0;
		visited = 0;
		memset(min_resi, 0, sizeof(min_resi));
		memset(min_pre, 0, sizeof(min_pre));
		memset(hist, 0, sizeof(hist));
		block_size = 0;
		memset(pGradX, 0, sizeof(pGradX));
		memset(pGradY, 0, sizeof(pGradY));
		memset(pGrad_amplitude, 0, sizeof(pGrad_amplitude));
		for (int i = 0; i < 3; ++i)	// Similarity 在（0,1）范围内
			Similarity[i] = 0.0f;
	}
};

int print_double_matrix_to_file(double **mat, const char *filename, int rows, int cols, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
int print_short_matrix_to_file(short **mat, const char *filename, int rows, int cols, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
int print_1dimension_short_matrix_to_file(unsigned char *mat, const char *filename, int rows, int cols, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
// 读入文件出现错误时提供帮助
void help();
//   读文件
void ReadFile(const char* infile, unsigned char* pImg, int img_size, int precision);
//   写文件
void WriteFile(const char* outfile, unsigned char ** pImg, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
 //  动态开辟内存空间
int memory_new(short **img, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
int memory_new(unsigned char **img, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
//   // 释放动态开辟的内存
int memory_free(unsigned char **img, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
int memory_free(short **img, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
int memory_1dimension_free(unsigned char *img, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
int ReadFormatedFile(short** direction_block, const char* in_filepath, int rows, int cols);
int direction_search(int wedge_direction_index, map<int, int> const &direction_table, int mode);

extern struct block block_label[ROWS][COLS];

#endif