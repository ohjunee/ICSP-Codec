#include "Functions.h"

void Read_PRED_INFO(DATA* file, uint8* code, uint8 level)
{
	uint8 buffer = 0;
	uint8 bitLen = round(log(level, 2));		//ǥ����ȣ�� ������ ���� ��Ʈ�� ���� ����

	*code = 0;

	for (int cnt = 0; cnt < bitLen; cnt++)
	{
		buffer = (file->output << (file->length % 8) & 128) ? 1 : 0;
		*code = *code << 1 | buffer;

		file->length++;

		if (file->length % 8 == 0)
		{
			fread(&file->output, sizeof(uint8), 1, file->fp_in);
			file->length = 0;
		}
	}
}


