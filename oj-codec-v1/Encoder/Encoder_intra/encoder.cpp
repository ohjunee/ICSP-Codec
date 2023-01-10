#include "Buffer.h"
#include "input.h"
#include "Node.h"

void encoder(BUFFER* img, Data* file)
{
	//������ ��ü frame �ڵ�
	while (file->frameCount < VIDEO_FRAME)
	{
		// ������ 1 frame �ڵ�(4:2:0 ���, Y/Cb/Cr ������ ����� ��Ʈ��Ʈ�� ����)
		for (int component = 0; component < 3; component++)
			component == 0 ? Encoding(img, file, HEIGHT, WIDTH) : Encoding(img, file, HEIGHT / 2, WIDTH / 2);

		// ���� ��Ʈ�� msb ������ �б�
		if (file->frameCount == VIDEO_FRAME - 1 && file->length % 8 != 0)
		{
			file->output = file->output << (8 - (file->length % 8));
			fwrite(&file->output, sizeof(uint8), 1, file->fp_out);
		}
		file->frameCount++;
	}
}

void Encoding(BUFFER* img, Data* file, uint32 height, uint32 width)
{
	if (height == HEIGHT / 2)
		Just_Read_CbCr(img, file, height, width);				// Y������ ������ Cb Cr������ �������������� ����

	else
	{
		sturct_memory_check(img, file, height, width);			// ���� ���� �޸� �Ҵ� �� �ʱ�ȭ
		ImageLoad(img, file, height, width);					// ���� �Է�
		Partitioning(img, file, img->nRow, img->nCol, PSIZE);	// ���� ����
		Write_Recon_Image(img, file, height, width);			// ���� ���� ����
		struct_memory_free(img, file);							// ���� ���� �޸� ��ȯ
	}
}

void Just_Read_CbCr(BUFFER* img, Data* file, uint32 height, uint32 width)
{
	uint8* CbCr_img = (uint8*)calloc(height*width, sizeof(uint8));

	fread(CbCr_img, sizeof(uint8), height*width, file->fp_ori);

	free(CbCr_img);
}