#ifndef COMMON_H
#define COMMON_H

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
const int bias_top_row = 3;              //ÿ������չ������
const int bias_down_row = 3;             //ÿ������չ������

const int bias_left_col = 3;             //ÿ������չ������
const int bias_right_col = 3;             //ÿ������չ������

const int BLOCKWIDTH = 4;		// Сͼ�Ŀ��
const int BLOCKHEIGHT = 4;		// Сͼ�ĸ߶�

const int padded_imageblock_height = BLOCKHEIGHT + bias_top_row + bias_down_row; //��չ���ͼ���ĸ�
const int padded_imageblock_width = BLOCKWIDTH + bias_left_col + bias_right_col; //��չ���ͼ���Ŀ�

const int ROWS = 128;	// ��ͼ��ֳ�С�飬ÿһ�е�ͼ�����Ŀ
const int COLS = 128;	// ��ͼ��ֳ�С�飬ÿһ�е�ͼ�����Ŀ
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
	double    Similarity[3];  // Similarity �ڣ�0,1����Χ��
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
		for (int i = 0; i < 3; ++i)	// Similarity �ڣ�0,1����Χ��
			Similarity[i] = 0.0f;
	}
};

extern struct block block_label[128][128];

//extern short block_image[256][256][8][8];
//extern short smallimage[256][256][8][8];	    // ȫ�ֿ�
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