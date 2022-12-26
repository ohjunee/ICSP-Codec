#include "Buffer.h"
#include "input.h"

// padding_buffer(original)
void ImageLoad(BUFFER* img, Data* file, uint32 height, uint32 width)
{
	uint32 i = 0, cnt = 0;
	uint8 org_px = 0;

	while (cnt < height*width)
	{
		if (img->extra_col != 0)
		{
			fread(&img->org_b[i], sizeof(uint8), 1, file->fp_ori);
			i++;
			cnt++;

			if (cnt % width == 0)
			{ 
				i = i + img->extra_col;
			}
		}
		else
		{
			fread(&img->org_b[i], sizeof(uint8), 1, file->fp_ori);
			i++;
			cnt++;
		}
	}
}

// padding_buffer(recon)
void Write_ReconImageBuffer(BUFFER* img, uint8* Recon, uint8 p_size, uint32 ul_recon)
{
	uint8 i, j;
	uint32 init_position = ul_recon + (img->nWidth + 1) + 1; // ����ȭ�� ��ġ�κ��� ���ϴܹ���(�밢������ ��ĭ) ȭ�� ��ġ�� �̵�

	for (j = 0; j < p_size; j++) {
		for (i = 0; i < p_size; i++)
		{
			// ���� �̹��� ������ �̵��� ����ȭ����ġ���� ���������� (���� + ���� = ����) ȭ�Ұ��� ����
			img->recon_padding[init_position + i * (img->nWidth + 1) + j] = Recon[j + i * p_size];
		}
	}
}
