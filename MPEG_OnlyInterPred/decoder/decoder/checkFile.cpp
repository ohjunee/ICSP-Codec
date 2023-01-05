#include "Functions.h"
#pragma warning(disable: 6387)

void Open_File(DATA* file)
{
	file->fp_in		= fopen(INPUT_FILENAME, "rb");
	file->fp_out	= fopen(OUTPUT_FILENAME, "wb");

	file->frameCount = 0;
	file->output = 0;
	file->length = 0;

	file->n_symbol = 0;
	file->AC_flag = 0;
}

void Close_File(DATA* file)
{
	fclose(file->fp_in);
	fclose(file->fp_out);
}

void Check_File(DATA* file, bool Bool)
{
	(Bool) ? Open_File(file) : Close_File(file);
}

/* ���ڴ� ���ڴ� ���������� mismatch Ȯ��*/
void Check_Image(DATA* file)
{
	fopen_s(&file->fp_1, FILE_1, "rb");
	fopen_s(&file->fp_2, FILE_2, "rb");

	int a = 0;
	uint32 cnt = 0;

	while (feof(file->fp_1) == 0 && feof(file->fp_2) == 0)
	{
		uint8 file_1_pixel, file_2_pixel;

		fread(&file_1_pixel, sizeof(uint8), 1, file->fp_1);
		fread(&file_2_pixel, sizeof(uint8), 1, file->fp_2);

		cnt++;

		if (file_1_pixel != file_2_pixel)
		{
			printf("�� ������ �ٸ��ϴ�. cnt: %d\n\n", cnt);
			break;
		}
	}

	printf("�� ������ �����ϴ�.\n");
	scanf("%d", &a);

	fclose(file->fp_1);
	fclose(file->fp_2);
}