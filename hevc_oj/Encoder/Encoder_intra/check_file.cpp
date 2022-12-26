#include "Buffer.h"
#include "input.h"

void Open_File(Data* file)
{
	file->fp_ori		= fopen(INPUT_FILENAME,		"rb"); // �Է� ����
	file->fp_out		= fopen(OUTPUT_FILENAME_1,	"wb"); // ��� ����
	file->fp_ori_recon	= fopen(OUTPUT_FILENAME_2,	"wb"); // ���ڴ� ���� ����

	file->frameCount = 0;
	file->output = 0;
	file->length = 0;
}

void Close_File(Data* file)
{
	fclose(file->fp_ori);
	fclose(file->fp_out);
	fclose(file->fp_ori_recon);
}

void checkFile(Data* file, bool Bool)
{
	(Bool) ? Open_File(file) : Close_File(file);
}