#include "Buffer.h"
#include "input.h"

void Decoder(BUFFER* img, Data* file)
{
	fread(&file->output, sizeof(uint8), 1, file->fp_in);

	//������ ��ü frame ���ڵ�
	while (file->frameCount < VIDEO_FRAME)
	{
		// ������ 1 frame �ڵ� Y/Cb/Cr ������ ����� ��Ʈ��Ʈ���� ���ڵ�
		for (int component = 0; component < 1; component++)
		{
			component == 0 ? File_Decoding(img, file, HEIGHT, WIDTH) : File_Decoding(img, file, HEIGHT / 2, WIDTH / 2);
		}
		file->frameCount++;
	}
}

void File_Decoding(BUFFER* img, Data* file, uint32 height, uint32 width)
{
	Check_Padding_Image	(img, file, height, width, true);

	Combine_Partition	(img, file, img->nRow, img->nCol, PSIZE);
	Write_Recon_Image	(img, file, height, width);

	Check_Padding_Image(img, file, height, width, false);
}
