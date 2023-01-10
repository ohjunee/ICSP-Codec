#pragma once
#include "main.h"

#define VIDEO_FRAME			90

#define WIDTH				352
#define HEIGHT				288
#define PSIZE				8

#define INPUT_FILENAME		"C:\\images\\football_cif(352X288)_90f_bitstream.raw"
#define OUTPUT_FILENAME		"C:\\images\\football_cif(352X288)_90f_recon_decoder.yuv"

#define OUTPUT_FILENAME_2	"C:\\images\\text_decoder.txt"

#define FILE_1				"C:\\images\\football_cif(352X288)_90f_recon_encoder.yuv"
#define FILE_2				"C:\\images\\football_cif(352X288)_90f_recon_decoder.yuv"

#define PADDING_VAL			pow(2, 8) /2	// PADDING_VAL = pow(2 , Pixel_range) / 2
#define log(x, base)		log(x)/log(base)


#define TEST_HUFFMAN		0	// 1 : ������ ��ȣȭ ���
								// 0 : ������� ��ȣȭ ���

#define READ_DCT_SKIP		1	// 1 : DCT skip ������ �д´�

#define PRED_MODE			4

#define READ_SYMBOL_VAL		1	// �ɺ� ������ ������ŭ �ɺ� �� �б�
#define READ_SYMBOL_VAL_	0	// �ɺ��� �б�

#define READ_SYMBOL_NUM		1	// �ɺ��� ���� �б�
#define READ_FREQ			0	// �ɺ��� �󵵼� �б�