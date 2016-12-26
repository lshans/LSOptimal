#ifndef COMMON_H
#define COMMON_H

#include <map>
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
int print_short_matrix(short **mat, const char *filename, int rows, int cols, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
int print_1dimension_short_matrix_to_file(unsigned char *mat, const char *filename, int rows, int cols, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);

//   读写文件
void read_img(const char* filename, unsigned char* pImg, int img_sz, int precision);
void write_img(const char* outfile, unsigned char ** pImg, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);

// 分配图像存储内存
short **new_short_mat(int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
unsigned char **new_uchar_mat(int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);

// 释放图像内存
void delete_short_mat(short **pImg, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
void delete_uchar_mat(unsigned char **pImg, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);

int ReadFormatedFile(short** direction_block, const char* in_filepath, int rows, int cols);
int search_direction(std::map<int, int> const&direction_table, int wedge_direction_index);

// 大小端格式转换，将一维指针 img_in指向的内存区域中的数据进行大小端格式转换，并存储到二维指针img指向的内存区域中
void transformat(short **img, unsigned char *img_in,  int height, int width, int endian, int precision);

// 对图像进行边界扩展
void img_padding(short **pImgPadding, short **pImg, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);

// 创建查找表
std::map<int, int> build_direction_search_table();

extern struct block block_label[ROWS][COLS];

#endif