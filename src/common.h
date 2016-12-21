#ifndef COMMON_H
#define COMMON_H

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

extern struct block block_label[128][128];

//extern short block_image[256][256][8][8];
//extern short smallimage[256][256][8][8];	    // 全局块
//extern short dc_left_image[256][256][8][8];
//extern short dc_top_image[256][256][8][8];
//extern short dc_image[256][256][8][8];
//extern short h_image[256][256][8][8];
//extern short v_image[256][256][8][8];
//extern short ddl_image[256][256][8][8];
//extern short ddr_image[256][256][8][8];
//extern short vr_image[256][256][8][8];
//extern short hd_image[256][256][8][8];
//extern short vl_image[256][256][8][8];
//extern short hu_image[256][256][8][8];
#endif