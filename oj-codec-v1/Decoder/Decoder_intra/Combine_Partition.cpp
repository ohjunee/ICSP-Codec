 #include "Buffer.h"
#include "input.h"

void Combine_Partition(BUFFER* img, Data* file, uint32 nRow, uint32 nCol, uint8 p_size)
{
	uint32 row_order = 0;
	uint32 UpLeftPx_recon = 0;;
	uint32 block_cnt = 0;

	while (block_cnt < nRow * nCol)
	{
		if (block_cnt != 0 && block_cnt % nCol == 0)
		{
			row_order++;
			UpLeftPx_recon = row_order * (img->nWidth + 1) * p_size;
		}

		Select_SplitFlag(img, file, p_size, UpLeftPx_recon);

		UpLeftPx_recon += p_size;
		block_cnt++;
	}
}

void Combine_4x4(BUFFER* img, Data* file, uint8 p_size, uint32 ul_recon)
{
	uint32 row_order = 0;
	uint32 UpLeft_px_recon = ul_recon;

	uint8* Pred_Block	= (uint8*)	calloc(p_size*p_size, sizeof(uint8));
	file->Recon_DCT_blk	= (double*)	calloc(p_size*p_size, sizeof(double));	//������ �����������

	//������� �ް�, ������� ����->8*8������Ͽ� ����, ����ȭ�� ��ȯ��� i_q, idct �� 8*8������Ͽ� ���� 
	for (int i = 0; i < 4; i++)
	{
		if (i != 0 && i % 2 == 0)
		{
			row_order++;
			UpLeft_px_recon = ul_recon + row_order * (img->nWidth + 1) * p_size;
		}
		// A0. ���� ��� �б�
		Read_PRED_INFO(file, &file->PredMode, PRED_MODE);

#if READ_DCT_SKIP == 1
		//A1.DCT skip �б�
		Read_PRED_INFO(file, &file->DCT_skip, 2);
#else
		file->DCT_skip = 1;
#endif 
		/* B. ���� ��忡 ���� ���� ��� ���� */
		Get_PredBlock(img, Pred_Block, p_size, file->PredMode, UpLeft_px_recon); //nWidth + 1

		/* ����ȭ�� ��ȯ����� �а�, ����ȯ */
		Read_PredErr(file, p_size);

		/* D. ������ ��������� padding buffer(recon)�� ���� */
		Get_ReconBuff(img, file, Pred_Block, p_size, UpLeft_px_recon); //nWidth + 1
		
		UpLeft_px_recon += p_size;
	}

	free(file->Recon_DCT_blk);
	free(Pred_Block);
}

void Combine_8x8(BUFFER* img, Data* file, uint8 p_size, uint32 ul_recon)
{
	uint8*	Pred_Block	= (uint8*)		calloc(p_size*p_size, sizeof(uint8));
	file->Recon_DCT_blk  = (double*)	calloc(p_size*p_size, sizeof(double));	//������ �����������

	// A0. ������� �б�
	Read_PRED_INFO(file, &file->PredMode, PRED_MODE);

	//A1.DCT skip �б�
#if READ_DCT_SKIP == 1
	Read_PRED_INFO(file, &file->DCT_skip, 2);
#else
	file->DCT_skip = 1;
#endif
	/* B. ���� ��忡 ���� ���� ��� ���� */
	Get_PredBlock(img, Pred_Block, p_size, file->PredMode, ul_recon); //nWidth + 1

	/* C2 ���޹��� ����������� ���� ������ȭ, IDCT ���� */
	Read_PredErr(file, p_size);

	/* D. ������ ��������� padding buffer(recon)�� ���� */
	Get_ReconBuff(img, file, Pred_Block, p_size, ul_recon); //nWidth + 1

	free(file->Recon_DCT_blk);
	free(Pred_Block);
}

/* ���� ���*/
void Select_SplitFlag(BUFFER* img, Data* file, uint8 p_size, uint32 ul_recon)
{
	Read_PRED_INFO(file, &file->split_flag, 2);

	/* flag�� 1�̸� ����, 0�̸� �״�� */
	file->split_flag ? Combine_4x4(img, file, p_size / 2, ul_recon) : Combine_8x8(img, file, p_size, ul_recon);
}