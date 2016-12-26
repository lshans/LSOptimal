#ifndef COMMON_H
#define COMMON_H

#include <map>
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

int print_double_matrix_to_file(double **mat, const char *filename, int rows, int cols, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
int print_short_matrix(short **mat, const char *filename, int rows, int cols, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
int print_1dimension_short_matrix_to_file(unsigned char *mat, const char *filename, int rows, int cols, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);

//   ��д�ļ�
void read_img(const char* filename, unsigned char* pImg, int img_sz, int precision);
void write_img(const char* outfile, unsigned char ** pImg, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);

// ����ͼ��洢�ڴ�
short **new_short_mat(int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
unsigned char **new_uchar_mat(int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);

// �ͷ�ͼ���ڴ�
void delete_short_mat(short **pImg, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);
void delete_uchar_mat(unsigned char **pImg, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);

int ReadFormatedFile(short** direction_block, const char* in_filepath, int rows, int cols);
int search_direction(std::map<int, int> const&direction_table, int wedge_direction_index);

// ��С�˸�ʽת������һάָ�� img_inָ����ڴ������е����ݽ��д�С�˸�ʽת�������洢����άָ��imgָ����ڴ�������
void transformat(short **img, unsigned char *img_in,  int height, int width, int endian, int precision);

// ��ͼ����б߽���չ
void img_padding(short **pImgPadding, short **pImg, int height, int width, int bias_top_row = 0, int bias_down_row = 0, int bias_left_col = 0, int bias_right_col = 0);

// �������ұ�
std::map<int, int> build_direction_search_table();

extern struct block block_label[ROWS][COLS];

#endif