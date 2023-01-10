#include "Functions.h"

void getPredBlock(BUFFER* img, uint8* Pred_b, uint8 mode, uint32 upleft_px)
{
	uint8 i, j, idx;

	switch (mode) {

		/*���� ����*/
	case 0:
	{
		//ù��° �࿡ �����ϴ� ��ϵ��� ������� ������ ��ܿ� �����ȼ��� �����Ƿ� gray scale�� �ȼ������� �߰����� 128�� ����
		if (img->row_order == 0)
			memset(Pred_b, 128, sizeof(uint8) * PSIZE * PSIZE);		//			Pred[j + i * PSIZE] = 128;

		else
		{
			//�����̹��� ������ ����ȭ�� ��ġ���� ����������� 1, ... , (PSIZE + 1)��ŭ �̵�
			for (j = 0; j < PSIZE; j++) {
				for (i = 0; i < PSIZE; i++) {
					Pred_b[j + i * PSIZE] = img->Recon[upleft_px + j - WIDTH];
				}
			}
		}
		break;
	}

	/*���� ����*/
	case 1:
	{
		if (img->isFirstBlk_in_row)
			memset(Pred_b, 128, sizeof(uint8) * PSIZE * PSIZE);		//			Pred[j + i * PSIZE] = 128;

		else
		{
			//�����̹��� ������ ����ȭ�� ��ġ���� ������������ 0, ... , (PSIZE - 1)��ŭ �̵�
			for (j = 0; j < PSIZE; j++) {
				for (i = 0; i < PSIZE; i++) {
					Pred_b[j + i * PSIZE] = img->Recon[i * WIDTH + upleft_px - 1];
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
			for (idx = 0; idx < PSIZE; idx++)
				dc_row += img->Recon[upleft_px + idx * WIDTH - 1];

			mean = round((dc_row + dc_col) / PSIZE);
			memset(Pred_b, mean, sizeof(uint8) * PSIZE * PSIZE);
		}

		// ù��° ����� ������ ù° ��
		else if (img->row_order != 0 && img->isFirstBlk_in_row)
		{
			for (idx = 0; idx < PSIZE; idx++)
				dc_col += img->Recon[upleft_px - WIDTH + idx];

			mean = round((dc_row + dc_col) / PSIZE);
			memset(Pred_b, mean, sizeof(uint8) * PSIZE * PSIZE);
		}

		else {
			//�����̹��� ������ ����ȭ�� ��ġ���� ����/����������� 0, ... , (PSIZE -1) ��ġ�� �ִ� ȭ�Ұ��� ��� ����
			for (idx = 0; idx < PSIZE; idx++)
			{
				dc_row += img->Recon[upleft_px - 1 + idx * WIDTH];
				dc_col += img->Recon[upleft_px - WIDTH + idx];
			}

			mean = round((dc_row + dc_col) / (2 * PSIZE));
			memset(Pred_b, mean, sizeof(uint8) * PSIZE * PSIZE);
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
			for (j = 0; j < PSIZE; j++) {
				for (i = 0; i < PSIZE; i++)
				{
					// ȭ�Ұ��� ��ǥ��(i,j)�� �̿��Ͽ� �밢 ���� ����
					diff = abs(i - j);
					Pred_b[j + i * PSIZE] = (i <= j) ? 128 : img->Recon[upleft_px - 1 + (diff - 1) * WIDTH];
				}
			}
		}

		// ù��° ����� ������ ù° ��
		else if (img->row_order != 0 && img->isFirstBlk_in_row)
		{
			for (j = 0; j < PSIZE; j++) {
				for (i = 0; i < PSIZE; i++)
				{
					// ȭ�Ұ��� ��ǥ��(i,j)�� �̿��Ͽ� �밢 ���� ����
					diff = abs(i - j);
					Pred_b[j + i * PSIZE] = (i < j) ? img->Recon[upleft_px - 1 - WIDTH + diff] : 128;
				}
			}
		}

		else
		{
			for (j = 0; j < PSIZE; j++) {
				for (i = 0; i < PSIZE; i++)
				{
					// ȭ�Ұ��� ��ǥ��(i,j)�� �̿��Ͽ� �밢 ���� ����
					diff = abs(i - j);
					Pred_b[j + i * PSIZE] = (i <= j) ? img->Recon[upleft_px - 1 - WIDTH + diff] : img->Recon[upleft_px - 1 + (diff - 1) * WIDTH];
				}
			}
		}

		break;
	}
	}
}

//ù��° ����� ������ ������ ��ϵ鿡 ���� MPM_flag ����
void Find_MPM(BUFFER* img, DATA* file, uint32 width)
{
	int ref_Pmode[3] = { 0 };	// 0: Left, 1:Upper, 2: Upper left

	//ù��° ��� ��
	if (img->row_order == 0)
		ref_Pmode[1] = file->PM_Arr[img->blk_cnt - 1];					// left

	//ù��° ��� ��
	else if (img->isFirstBlk_in_row)
		ref_Pmode[1] = file->PM_Arr[img->blk_cnt - width / PSIZE];		// up

	//������
	else
	{
		ref_Pmode[0] = file->PM_Arr[img->blk_cnt - 1];					// left
		ref_Pmode[1] = file->PM_Arr[img->blk_cnt - width / PSIZE];		// up
		ref_Pmode[2] = file->PM_Arr[img->blk_cnt - width / PSIZE - 1];	// upleft

		QuickSort(ref_Pmode, 0, 2);
	}

	file->PredMode = ref_Pmode[1];
}