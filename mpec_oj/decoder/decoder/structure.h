#pragma once
#include "main.h"

typedef struct Node {

	uint8*	ref_img;
	Node*	next;

}Node;

typedef struct imageBuffer
{
	uint8* org;
	uint8* Recon;

	double* Recon_DCT_blk;
	double* DC_Recon;
	double* ZigZag_arr;

	double** PredErr_buf2D;

	uint32	nBlk_width, nBlk_height, total_blk, blk_cnt, row_order;

	bool	isFirstBlk_in_row, isLastBlk_in_row;

}BUFFER;

typedef struct FILE_BUFFER
{
	FILE* fp_in;				//���ڴ� �Է¿��� ����������
	FILE* fp_out;				//���ڴ� ��¿��� ����������

	FILE* fp;					//�ؽ�Ʈ ����

	FILE* fp_1;
	FILE* fp_2;

	double* buffer;

	uint8*	PM_Arr;
	uint8	MPM_flag;			//�ֺ�������κ��� ������ ����� ���� ���������� ��� 1
	uint8	PredMode;

	uint8*	symbol;
	uint8	n_symbol;
	
	int mv_x, mv_y;

	uint8	AC_flag;
	uint8	MVmodeflag;

	uint8	output;				//��Ʈ��Ʈ�� �ƿ�ǲ
	uint32	length;

	uint32  frameCount;

}DATA;