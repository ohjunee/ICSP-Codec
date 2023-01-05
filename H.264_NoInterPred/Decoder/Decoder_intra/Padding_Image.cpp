#include "Buffer.h"
#include "input.h"

void Check_Padding_Image(BUFFER* img, Data* file, uint32 height, uint32 width, bool Bool)
{
	if (Bool)
	{
		/* �е��� ȭ�Ұ��� �ʱ�ȭ */
		img->extra_row = height % PSIZE ? PSIZE - (height % PSIZE) : 0;
		img->extra_col = width  % PSIZE ? PSIZE - (width % PSIZE) : 0;

		img->nRow = ((height + img->extra_row) / PSIZE);
		img->nCol = ((width + img->extra_col) / PSIZE);

		// �е� �����̹��� ������ ���α���
		img->nWidth = PSIZE * img->nCol;					

		//�е� �̹��� ���� �޸� �Ҵ�, 128�� �ʱ�ȭ
		img->recon_padding = (uint8*)malloc((height + img->extra_row + 1) * (width + img->extra_col + 1) * sizeof(uint8));

		memset(img->recon_padding, PADDING_VAL, sizeof(uint8)*(height + img->extra_row + 1)	* (width + img->extra_col + 1));
	}
	else
	{
		//�е� �̹��� ���� �޸� ��ȯ
		free(img->recon_padding);
	}
}