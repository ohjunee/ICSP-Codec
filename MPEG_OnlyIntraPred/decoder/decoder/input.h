#pragma once
#include "main.h"

#define VIDEO_FRAME			90

#define WIDTH				352
#define HEIGHT				288
#define PSIZE				8
#define nPREDMODE			4

#define INPUT_FILENAME		"C:\\images\\encoder\\ver_1.1\\bitstream\\football_cif(352X288)_90f.raw"
#define OUTPUT_FILENAME		"C:\\images\\decoder\\ver_1.1\\recon\\football_cif(352X288)_90f.yuv"

#define FILE_1				"C:\\images\\encoder\\ver_1.1\\recon\\football_cif(352X288)_90f.yuv"
#define FILE_2				"C:\\images\\decoder\\ver_1.1\\recon\\football_cif(352X288)_90f.yuv"

#define OUTPUT_FILENAME_2	"C:\\images\\decoder\\text_file\\test.txt"	// �̹��� ������ �ؽ�Ʈ ���Ϸ� Ȯ��

#define log(x,base)			log(x)/log(base)

#define TEST				0

#define INTRA_PERIOD		10
#define MV_DISTANCE			8