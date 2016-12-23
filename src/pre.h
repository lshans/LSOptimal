#ifndef PRE_H
#define PRE_H
#define N 4


// format transformation for input img file to little endian
// img:		output
// img_in:	origin img
void transformat(short **img, unsigned char *img_in, int height, int width, int endian, int precision);

//typedef unsigned int uint32_t;
//typedef unsigned char uint8_t;
//const int bias_top_row = 3;              //每个块扩展的行数
//const int bias_down_row = 3;             //每个块扩展的行数
//
//const int bias_left_col = 3;             //每个块扩展的列数
//const int bias_right_col = 3;             //每个块扩展的列数
//
//const int BLOCKWIDTH = 4;		// 小图的宽度
//const int BLOCKHEIGHT = 4;		// 小图的高度
//
//const int padded_imageblock_height = BLOCKHEIGHT + bias_top_row + bias_down_row; //扩展后的图像块的高
//const int padded_imageblock_width = BLOCKWIDTH + bias_left_col + bias_right_col; //扩展后的图像块的宽
//
//const int ROWS = 128;	// 将图像分成小块，每一行的图像块数目
//const int COLS = 128;	// 将图像分成小块，每一列的图像块数目
static void predict_4x4_dc_128(uint8_t *src);
static void predict_4x4_dc_left(uint8_t *src);
static void predict_4x4_dc_top(uint8_t *src);
static void predict_4x4_dc(uint8_t *src);
static void predict_4x4_h(uint8_t *src);
static void predict_4x4_v(uint8_t *src);
static void predict_4x4_ddl(uint8_t *src);
static void predict_4x4_ddr(uint8_t *src);
static void predict_4x4_vr(uint8_t *src);
static void predict_4x4_hd(uint8_t *src);
static void predict_4x4_vl(uint8_t *src);
static void predict_4x4_hu(uint8_t *src);


//本函数对4*4块的每行像素赋同样的值,给第一行的4 个像素赋值,给第二行的4 个像素赋值,给第三行的4 个像素赋值,给第三行的4 个像素赋值
#define PREDICT_4x4_DC(v)\
{\
	*(uint32_t*)&src[0*FDEC_STRIDE] =\
	*(uint32_t*)&src[1*FDEC_STRIDE] =\
	*(uint32_t*)&src[2*FDEC_STRIDE] =\
	*(uint32_t*)&src[3*FDEC_STRIDE] = v;\
}

//该定义表示依次取出当前宏块左边的 4个像素,l0 是第一行左边像素I,l1 是第二行左边像素J,l2 是第三行左边像素K,l3 是第四行左边像素L
#define PREDICT_4x4_LOAD_LEFT \
	const int l0 = src[-1+0*FDEC_STRIDE];\
	const int l1 = src[-1+1*FDEC_STRIDE];\
	const int l2 = src[-1+2*FDEC_STRIDE];\
	const int l3 = src[-1+3*FDEC_STRIDE];

//该定义表示依次取出当前宏块上方的 4个像素,----t0 是第一列上方的像素A, t1 是第二列上方的像素B, t2 是第三列上方的像素C,t3 是第四列上方的像素D
#define PREDICT_4x4_LOAD_TOP \
	const int t0 = src[0-1*FDEC_STRIDE]; \
	const int t1 = src[1-1*FDEC_STRIDE]; \
	const int t2 = src[2-1*FDEC_STRIDE]; \
	const int t3 = src[3-1*FDEC_STRIDE]; 

//该定义表示依次取出当前宏块右上方的 4个像素,t4 是右上方第一个像素E,t5 是右上方第二个像素F,t6 是右上方第三个像素G,t7 是右上方第四个像素H
#define PREDICT_4x4_LOAD_TOP_RIGHT \
	const int t4 = src[4-1*FDEC_STRIDE];\
	const int t5 = src[5-1*FDEC_STRIDE];\
	const int t6 = src[6-1*FDEC_STRIDE];\
	const int t7 = src[7-1*FDEC_STRIDE]; 


// estimate prediction param
// img:		input img value
// return:	 estimated prediction param value
void AccumulateParaAB_one(double **paraA, double **paraB, short block_image[8][8], int i, int j, int mode);
void AccumulateParaAB_two(double **paraA, double **paraB, short block_image[8][8], int i, int j, int mode);
void AccumulateParaAB_three(double **paraA, double **paraB, short block_image[8][8], int i, int j, int mode);
void AccumulateParaAB_four(double **paraA, double **paraB, short block_image[8][8], int i, int j, int mode);
void estimate(short **image_construct, double **para, int height, int width, int mode);
void estimate_LeftTop_one(short **image_construct, double **para, int height, int width, int mode);
void estimate_LeftDown_one(short **image_construct, double **para, int height, int width, int mode);
void estimate_LeftTop_two(short **image_construct, double **para, int height, int width, int mode);
void estimate_LeftDown_two(short **image_construct, double **para, int height, int width, int mode);
void estimate_LeftTop_three(short **image_construct, double **para, int height, int width, int mode);
void estimate_LeftDown_three(short **image_construct, double **para, int height, int width, int mode);
void estimate_LeftTop_four(short **image_construct, double **para, int height, int width, int mode);
void estimate_LeftDown_four(short **image_construct, double **para, int height, int width, int mode);
//void estimate_one_para(short **image_construct, double **para, int height, int width, int mode, int paranum);
//void estimate_two_para(short **image_construct, double **para, int height, int width, int mode, int paranum);
//void estimate_three_para(short **image_construct, double **para, int height, int width, int mode, int paranum);
//void estimate_four_para(short **image_construct, double **para, int height, int width, int mode, int paranum);

//对图像块进行变换量化、逆变换逆量化
void DCT_Quanter(int16_t inputData[4][4], int16_t outputData[4][4]);

//#define FDEC_STRIDE 32
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
const int FDEC_STRIDE = 8 * 8;	// 8x8 block

bool Gauss(double **A, double **B, int n);
void Mult(double **mat1, double **mat2,double **resMat, int H1, int W1, int K1);
long long predict(short **image_construct, short **resi, short **predicted, double **para, int height, int width, int mode);
long long predict_LeftTop_one(short **image_construct, short **resi, short **predicted, double **para, int height, int width, int mode);
long long predict_LeftDown_one(short **image_construct, short **resi, short **predicted, double **para, int height, int width, int mode);
long long predict_LeftTop_two(short **image_construct, short **resi, short **predicted, double **para, int height, int width, int mode);
long long predict_LeftDown_two(short **image_construct, short **resi, short **predicted, double **para, int height, int width, int mode);
long long predict_LeftTop_three(short **image_construct, short **resi, short **predicted, double **para, int height, int width, int mode);
long long predict_LeftDown_three(short **image_construct, short **resi, short **predicted, double **para, int height, int width, int mode);
long long predict_LeftTop_four(short **image_construct, short **resi, short **predicted, double **para, int height, int width, int mode);
long long predict_LeftDown_four(short **image_construct, short **resi, short **predicted, double **para, int height, int width, int mode);
void predict_one_para(short **image_construct, short **resi, short **predicted, int height, int width, int mode);
void predict_two_para(short **image_construct, short **resi, short **predicted, int height, int width, int mode);
void predict_three_para(short **image_construct, short **resi, short **predicted, int height, int width, int mode);
void predict_four_para(short **image_construct, short **resi, short **predicted, int height, int width, int mode);

void DCTCore4x4(uint8_t block[4][4], uint8_t tblock[4][4]);
void IDCTCore4x4(uint8_t tblock[4][4], uint8_t block[4][4]);


#endif