#include "Buffer.h"
#include "input.h"

void Get_PredBlock(BUFFER* img, uint8* Pred_Block, uint8 p_size, uint8 mode, uint32 ul_recon)
{
	uint8 i, j;
	switch (mode) {

		/*���� ����*/
	case 0:

		//�����̹��� ������ ����ȭ�� ��ġ���� ����������� 1, ... , (p_size + 1)��ŭ �̵�
		for (j = 0; j < p_size; j++) {
			for (i = 0; i < p_size; i++)
			{
				Pred_Block[j + i * p_size] = img->recon_padding[ul_recon + j + 1];
			}
		}
		break;

		/*���� ����*/
	case 1:

		//�����̹��� ������ ����ȭ�� ��ġ���� ������������ 1, ... , (p_size + 1)��ŭ �̵�
		for (j = 0; j < p_size; j++) {
			for (i = 0; i < p_size; i++)
			{
				Pred_Block[j + i * p_size] = img->recon_padding[ul_recon + (i + 1) * (img->nWidth + 1)];
			}
		}
		break;

		/*DC ����*/
	case 2:
	{
		uint32 dc_row = 0, dc_col = 0;

		//�����̹��� ������ ����ȭ�� ��ġ���� ����/����������� 1, ... , (p_size + 1) ��ġ�� �ִ� ȭ�Ұ��� ��� ����
		for (int tmp = 0; tmp < p_size; tmp++)
		{
			dc_row += img->recon_padding[ul_recon + tmp + 1];
			dc_col += img->recon_padding[ul_recon + (tmp + 1) * (img->nWidth + 1)];
		}

		uint32 dc_tot = round(double(dc_row + dc_col) / (2 * p_size));

		for (i = 0; i < p_size*p_size; i++) { Pred_Block[i] = dc_tot; }

		break;
	}

	/*�밢�� ����*/
	case 3:
	{
		int d_index;
		uint8 refPx_i, u_pxVal, l_pxVal;

		for (j = 0; j < p_size; j++) {
			for (i = 0; i < p_size; i++)
			{
				d_index = i - j;
				refPx_i = abs(d_index);										// ȭ�Ұ��� ���̸� ����ȭ���� �ε���������
				u_pxVal = img->recon_padding[ul_recon + refPx_i];				// (i,j)�� ��ġ�� ȭ�ҿ� ���� i <= j�� ���̸� ���� �ڵ��ϰ��� �ϴ� ��� ����� ����ȭ�Ұ��� ����
				l_pxVal = img->recon_padding[ul_recon + (refPx_i)* (img->nWidth + 1)];	// �����̸� ��� ������ ����ȭ�� ���� ����

				Pred_Block[j + i * p_size] = (i <= j) ? u_pxVal : l_pxVal;
			}
		}
		break;
	}
	}
}