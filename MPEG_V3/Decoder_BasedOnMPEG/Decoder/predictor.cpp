#include "Functions.h"

void getPredBlock(BUFFER* img, uint8* Pred_b, uint8& mode, uint32& upleft_px)
{
	uint8 i, j, idx;

	switch (mode) {

		/*���� ����*/
	case 0:
	{
		//ù��° �࿡ �����ϴ� ��ϵ��� ������� ������ ��ܿ� �����ȼ��� �����Ƿ� gray scale�� �ȼ������� �߰����� 128�� ����
		if (img->row_order == 0)
			memset(Pred_b, 128, sizeof(uint8) * img->p_size * img->p_size);		//			Pred[j + i * p_size] = 128;

		else
		{
			//�����̹��� ������ ����ȭ�� ��ġ���� ����������� 1, ... , (p_size + 1)��ŭ �̵�
			for (j = 0; j < img->p_size; j++) {
				for (i = 0; i < img->p_size; i++) {
					Pred_b[j + i * img->p_size] = img->Recon[upleft_px + j - img->width];
				}
			}
		}
		break;
	}

	/*���� ����*/
	case 1:
	{
		if (img->isFirstBlk_in_row_org)
			memset(Pred_b, 128, sizeof(uint8) * img->p_size * img->p_size);		//			Pred[j + i * p_size] = 128;

		else
		{
			//�����̹��� ������ ����ȭ�� ��ġ���� ������������ 0, ... , (p_size - 1)��ŭ �̵�
			for (j = 0; j < img->p_size; j++) {
				for (i = 0; i < img->p_size; i++) {
					Pred_b[j + i * img->p_size] = img->Recon[i * img->width + upleft_px - 1];
				}
			}
		}

		break;
	}
	/*DC ����*/
	case 2:
	{
		uint32 dc_row = 0, dc_col = 0;

		uint8 mean;

		// ù��° ����� ������ ù° ��
		if (img->row_order == 0)
		{
			for (idx = 0; idx < img->p_size; idx++)
				dc_row += img->Recon[upleft_px + idx * img->width - 1];

			mean = round((dc_row + dc_col) / img->p_size);
			memset(Pred_b, mean, sizeof(uint8) * img->p_size * img->p_size);
		}

		// ù��° ����� ������ ù° ��
		else if (img->row_order != 0 && img->isFirstBlk_in_row_org)
		{
			for (idx = 0; idx < img->p_size; idx++)
				dc_col += img->Recon[upleft_px - img->width + idx];

			mean = round((dc_row + dc_col) / img->p_size);
			memset(Pred_b, mean, sizeof(uint8) * img->p_size * img->p_size);
		}

		else {
			//�����̹��� ������ ����ȭ�� ��ġ���� ����/����������� 0, ... , (p_size -1) ��ġ�� �ִ� ȭ�Ұ��� ��� ����
			for (idx = 0; idx < img->p_size; idx++)
			{
				dc_row += img->Recon[upleft_px - 1 + idx * img->width];
				dc_col += img->Recon[upleft_px - img->width + idx];
			}

			mean = round((dc_row + dc_col) / (2 * img->p_size));
			memset(Pred_b, mean, sizeof(uint8) * img->p_size * img->p_size);
		}

		break;
	}

	/*�밢�� ����*/
	case 3:
	{
		uint8 diff;

		// ù��° ����� ������ ù° ��
		if (img->row_order == 0)
		{
			for (j = 0; j < img->p_size; j++) {
				for (i = 0; i < img->p_size; i++)
				{
					// ȭ�Ұ��� ��ǥ��(i,j)�� �̿��Ͽ� �밢 ���� ����
					diff = abs(i - j);
					Pred_b[j + i * img->p_size] = (i <= j) ? 128 : img->Recon[upleft_px - 1 + (diff - 1) * img->width];
				}
			}
		}

		// ù��° ����� ������ ù° ��
		else if (img->row_order != 0 && img->isFirstBlk_in_row_org)
		{
			for (j = 0; j < img->p_size; j++) {
				for (i = 0; i < img->p_size; i++)
				{
					// ȭ�Ұ��� ��ǥ��(i,j)�� �̿��Ͽ� �밢 ���� ����
					diff = abs(i - j);
					Pred_b[j + i * img->p_size] = (i < j) ? img->Recon[upleft_px - 1 - img->width + diff] : 128;
				}
			}
		}

		else
		{
			for (j = 0; j < img->p_size; j++) {
				for (i = 0; i < img->p_size; i++)
				{
					// ȭ�Ұ��� ��ǥ��(i,j)�� �̿��Ͽ� �밢 ���� ����
					diff = abs(i - j);
					Pred_b[j + i * img->p_size] = (i <= j) ? img->Recon[upleft_px - 1 - img->width + diff] : img->Recon[upleft_px - 1 + (diff - 1) * img->width];
				}
			}
		}

		break;
	}
	}
}

//ù��° ����� ������ ������ ��ϵ鿡 ���� MPM_flag ����
void Find_MPM(BUFFER* img, DATA* file)
{
	int ref_Pmode[3] = { 0 };	// 0: Left, 1:Upper, 2: Upper left

	//ù��° ��� ��
	if (img->row_order == 0)
		ref_Pmode[1] = file->PM_Arr[img->blk_cnt - 1];						// left

	//ù��° ��� ��
	else if (img->isFirstBlk_in_row_org)
		ref_Pmode[1] = file->PM_Arr[img->blk_cnt - img->nBlk_width];		// up

	//������
	else
	{
		ref_Pmode[0] = file->PM_Arr[img->blk_cnt - 1];						// left
		ref_Pmode[1] = file->PM_Arr[img->blk_cnt - img->nBlk_width];		// up
		ref_Pmode[2] = file->PM_Arr[img->blk_cnt - img->nBlk_width - 1];	// upleft

		QuickSort(ref_Pmode, 0, 2);
	}

	file->PredMode = ref_Pmode[1];
}