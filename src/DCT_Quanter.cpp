#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
void DCTCore4x4(int16_t block[4][4], int16_t tblock[4][4])
{
	int16_t i, ii;
	int16_t tmp[16];
	int16_t *pTmp = tmp, *pblock;
	int16_t p0,p1,p2,p3;
	int16_t t0,t1,t2,t3;

	// Horizontal
	for (i=0; i < 0 + 4; i++)
	{
		pblock = &block[i][0];
		p0 = *(pblock++);
		p1 = *(pblock++);
		p2 = *(pblock++);
		p3 = *(pblock  );

		t0 = p0 + p3;
		t1 = p1 + p2;
		t2 = p1 - p2;
		t3 = p0 - p3;

		*(pTmp++) =  t0 + t1;
		*(pTmp++) = (t3 << 1) + t2;
		*(pTmp++) =  t0 - t1;    
		*(pTmp++) =  t3 - (t2 << 1);
	}

	// Vertical 
	for (i=0; i < 4; i++)
	{
		pTmp = tmp + i;
		p0 = *pTmp;
		p1 = *(pTmp += 4);
		p2 = *(pTmp += 4);
		p3 = *(pTmp += 4);

		t0 = p0 + p3;
		t1 = p1 + p2;
		t2 = p1 - p2;
		t3 = p0 - p3;

		ii = 0 + i;
		tblock[0    ][ii] = t0 +  t1;
		tblock[0 + 1][ii] = t2 + (t3 << 1);
		tblock[0 + 2][ii] = t0 -  t1;
		tblock[0 + 3][ii] = t3 - (t2 << 1);
	}
}

void IDCTCore4x4(int16_t tblock[4][4], int16_t block[4][4])
{
	int16_t i, ii;
	int16_t tmp[16];
	int16_t *pTmp = tmp, *pblock;
	int16_t p0,p1,p2,p3;
	int16_t t0,t1,t2,t3;

	// Horizontal
	for (i = 0; i < 0 + 4; i++)
	{
		pblock = &tblock[i][0];
		t0 = *(pblock++);
		t1 = *(pblock++);
		t2 = *(pblock++);
		t3 = *(pblock  );

		p0 =  t0 + t2;
		p1 =  t0 - t2;
		p2 = (t1 >> 1) - t3;
		p3 =  t1 + (t3 >> 1);

		*(pTmp++) = p0 + p3;
		*(pTmp++) = p1 + p2;
		*(pTmp++) = p1 - p2;
		*(pTmp++) = p0 - p3;
	}

	//  Vertical 
	for (i = 0; i < 4; i++)
	{
		pTmp = tmp + i;
		t0 = *pTmp;
		t1 = *(pTmp += 4);
		t2 = *(pTmp += 4);
		t3 = *(pTmp += 4);

		p0 = t0 + t2;
		p1 = t0 - t2;
		p2 =(t1 >> 1) - t3;
		p3 = t1 + (t3 >> 1);

		ii = i + 0;
		block[0    ][ii] = p0 + p3;
		block[0 + 1][ii] = p1 + p2;
		block[0 + 2][ii] = p1 - p2;
		block[0 + 3][ii] = p0 - p3;
	}
}

static  int def_quant4_mf[6][4][4] =
{
	{ { 13107, 8066, 13107, 8066 }, { 8066, 5243, 8066, 5243 },
	{ 13107, 8066, 13107, 8066 }, { 8066, 5243, 8066, 5243 } },
	{ { 11916, 7490, 11916, 7490 }, { 7490, 4660, 7490, 4660 },
	{ 11916, 7490, 11916, 7490 }, { 7490, 4660, 7490, 4660 } },
	{ { 10082, 6554, 10082, 6554 }, { 6554, 4194, 6554, 4194 },
	{ 10082, 6554, 10082, 6554 }, { 6554, 4194, 6554, 4194 } },
	{ {  9362, 5825,  9362, 5825 }, { 5825, 3647, 5825, 3647 },
	{  9362, 5825,  9362, 5825 }, { 5825, 3647, 5825, 3647 } },
	{ {  8192, 5243,  8192, 5243 }, { 5243, 3355, 5243, 3355 },
	{  8192, 5243,  8192, 5243 }, { 5243, 3355, 5243, 3355 } },
	{ {  7282, 4559,  7282, 4559 }, { 4559, 2893, 4559, 2893 },
	{  7282, 4559,  7282, 4559 }, { 4559, 2893, 4559, 2893 } }
};
static int dequant_mf[6][4][4] =
{
	{ {10, 13, 10, 13}, {13, 16, 13, 16}, {10, 13, 10, 13}, {13, 16, 13, 16} },
	{ {11, 14, 11, 14}, {14, 18, 14, 18}, {11, 14, 11, 14}, {14, 18, 14, 18} },
	{ {13, 16, 13, 16}, {16, 20, 16, 20}, {13, 16, 13, 16}, {16, 20, 16, 20} },
	{ {14, 18, 14, 18}, {18, 23, 18, 23}, {14, 18, 14, 18}, {18, 23, 18, 23} },
	{ {16, 20, 16, 20}, {20, 25, 20, 25}, {16, 20, 16, 20}, {20, 25, 20, 25} },
	{ {18, 23, 18, 23}, {23, 29, 23, 29}, {18, 23, 18, 23}, {23, 29, 23, 29} }
};
#define QUANT_ONE( coef, mf ) \
{ \
	if( (coef) > 0 ) \
	(coef) = ( f + (coef) * (mf) ) >> i_qbits; \
	else \
	(coef) = - ( ( f - (coef) * (mf) ) >> i_qbits ); \
}
static void quant_4x4_core( int16_t dct[4][4], int quant_mf[4][4], int i_qbits, int f )
{
	int i;
	for( i = 0; i < 16; i++ )
		QUANT_ONE( dct[0][i], quant_mf[0][i] );
}
static void quant_4x4(int16_t dct[4][4], int quant_mf[6][4][4], int i_qscale, int b_intra )
{
	const int i_qbits = 15 + i_qscale / 6;
	const int i_mf = i_qscale % 6;
	const int f = ( 1 << i_qbits ) / ( b_intra ? 3 : 6 );
	quant_4x4_core( dct, quant_mf[i_mf], i_qbits, f );
}
#define DEQUANT_SHL( x ) \
	dct[y][x] = ( dct[y][x] * dequant_mf[i_mf][y][x] ) << i_qbits

#define DEQUANT_SHR( x ) \
	dct[y][x] = ( dct[y][x] * dequant_mf[i_mf][y][x] + f ) >> (-i_qbits)

static void dequant_4x4( int16_t dct[4][4], int dequant_mf[6][4][4], int i_qp )
{
	const int i_mf = i_qp%6;
	const int i_qbits = i_qp/6;
	//const int i_qbits = i_qp/6 - 4;
	int y;

	if( i_qbits >= 0 )
	{
		for( y = 0; y < 4; y++ )
		{
			DEQUANT_SHL( 0 );
			DEQUANT_SHL( 1 );
			DEQUANT_SHL( 2 );
			DEQUANT_SHL( 3 );
		}
	}
	else
	{
		const int f = 1 << (-i_qbits-1);
		for( y = 0; y < 4; y++ )
		{
			DEQUANT_SHR( 0 );
			DEQUANT_SHR( 1 );
			DEQUANT_SHR( 2 );
			DEQUANT_SHR( 3 );
		}
	}
}

#define ZIG(i,y,x) level[i] = dct[y][x];
static  void scan_zigzag_4x4full( int16_t level[16], int16_t dct[4][4])
{
	ZIG( 0,0,0) ZIG( 1,0,1) ZIG( 2,1,0) ZIG( 3,2,0)
		ZIG( 4,1,1) ZIG( 5,0,2) ZIG( 6,0,3) ZIG( 7,1,2)
		ZIG( 8,2,1) ZIG( 9,3,0) ZIG(10,3,1) ZIG(11,2,2)
		ZIG(12,1,3) ZIG(13,2,3) ZIG(14,3,2) ZIG(15,3,3)
}
static const int scan_zigzag_x[16]={0, 1, 0, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 3, 2, 3};
static const int scan_zigzag_y[16]={0, 0, 1, 2, 1, 0, 0, 1, 2, 3, 3, 2, 1, 2, 3, 3};

static  void unscan_zigzag_4x4full( int16_t dct[4][4], int16_t level[16] )
{
	int i;

	for( i = 0; i < 16; i++ )
	{
		dct[scan_zigzag_y[i]][scan_zigzag_x[i]] = level[i];
	}
}

void  forward4x4(int16_t InData[4][4],int16_t OutData[16])
{
  int16_t tmp[4][4];
  DCTCore4x4(InData,tmp);
  quant_4x4(tmp,def_quant4_mf,10,1);
  scan_zigzag_4x4full(OutData,tmp);
}
void inverse4x4(int16_t InData[16],int16_t OutData[4][4])
{
  int16_t tmp[4][4];
  unscan_zigzag_4x4full(tmp,InData);
  dequant_4x4(tmp,dequant_mf,10);
  IDCTCore4x4(tmp,OutData);
}
void DCT_Quanter(int16_t inputData[4][4], int16_t outputData[4][4])
{
	//int16_t d1[4][4]={5,11,8,10,9,8,4,12,1,10,11,4,19,6,15,7}; /*PDF文档中最后一个例子中的输入数据 */
  int16_t d1[4][4]={0};
  for (int r = 0; r < 4; ++r)
	  for (int c = 0; c < 4; ++c)
		  d1[r][c] = inputData[r][c];

  int16_t OutData[16];
  int16_t d2[4][4], *tmp;
  int16_t i;
  forward4x4(d1, OutData);/*输出结果与PDF文档中完全相同 */
  /*对输出结果进行其它处理...... */
  inverse4x4(OutData,d2);
  tmp=(int16_t*)d2;
  for(i=0;i<16;i++)
  {
    *tmp=(*tmp+(1<<5))>>6;
    tmp++;
  }
  /*逆转换的结果也与文档相同。 */

  for (int r = 0; r < 4; ++r)
	  for (int c = 0; c < 4; ++c)
		  outputData[r][c] = d2[r][c];
  
}