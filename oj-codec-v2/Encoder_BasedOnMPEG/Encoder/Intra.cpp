#include "Functions.h"
#pragma warning(disable: 6011)

void INTRA_Prediction(BUFFER* img, DATA* file, uint32& upleft_px)
{
	double* DCT_blk = (double*)calloc(img->macroSize, sizeof(double));
	double* Quant_blk = (double*)calloc(img->macroSize, sizeof(double));
	double* Recon_DCT_blk = (double*)calloc(img->macroSize, sizeof(double));

	uint8** Pred_blk = 0;
	double** PredErr_blk = 0;

	double* Pixel_DPCM_blk = 0;

	double* ImgBuf = 0;

	uint8 p_size = img->p_size;

	uint8 mode = 0, PredMode = 0;

	uint32 cost[nPREDMODE] = { 0 };
	uint32 min_val = 0;

	if (file->pixel_dpcm_mode != 6)
		Pixel_DPCM_blk = (double*)calloc(img->macroSize, sizeof(double));

	// Intra ���� ���� Ȯ��
	if (file->intra_enable)
	{
		PredErr_blk = memory_calloc2D<double>(nPREDMODE, img->macroSize);
		Pred_blk = memory_calloc2D<uint8>(nPREDMODE, img->macroSize);

		//INTRA ������� cost���
		while (mode < nPREDMODE)
		{
			getPredBlock(img, Pred_blk[mode], mode, upleft_px);

			/* ���� - ���� = ��������(DCT ����ȭx) */
			for (int j = 0; j < p_size; j++) {
				for (int i = 0; i < p_size; i++)
					PredErr_blk[mode][j + i * p_size] = img->org[upleft_px + j + i * img->width] - Pred_blk[mode][j + i * p_size];
			}

			//�� ù��° ����� ��� ������忡 ���� ������ �������, ��������� ������. ���� ������ ������� 0���� ����
			if (img->blk_cnt == 0)
				break;

			/* cost ��� : sum(���� - ����) */
			for (int i = 0; i < img->macroSize; i++)
				cost[mode] += (uint32)fabs(PredErr_blk[mode][i]);

			if (mode == 0)
				min_val = cost[0];

			/* ������ ������� ���ϱ� */
			if (cost[mode] < min_val)
			{
				PredMode = file->PM_Arr[img->blk_cnt] = mode;
				min_val = cost[mode];
			}
			mode++;
		}

		ImgBuf = PredErr_blk[PredMode];
	}

	else
	{
		ImgBuf = (double*)calloc(img->macroSize, sizeof(double));

		for (int j = 0; j < p_size; j++) {
			for (int i = 0; i < p_size; i++) {
				ImgBuf[j + i * p_size] = img->org[upleft_px + j + i * img->width];
			}
		}
	}

	// ������������� DPCM���� ��ȣȭ
	Pixel_DPCM(img, file, ImgBuf, Pixel_DPCM_blk);										// ImgBuf -> Pixel_DPCM_blk	

	// ��ȣȭ�� ������������� DCT, ����ȭ																					
	DCT(file->pixel_dpcm_mode == 6 ? ImgBuf : Pixel_DPCM_blk, DCT_blk, p_size);

	Quantization(file, DCT_blk, Quant_blk, p_size, true);										// DCT_blk			-> Quant_blk	(forward)
	DC_DPCM(img, file, Quant_blk);

	/* ����ȭ�� ��������������� Reordering, Entropy */
	ZigZagScan(file, Quant_blk, p_size);														//Quant_blk			-> file->ZigZag_blk 

	if (file->intra_enable)
	{
		if (img->blk_cnt)	//ù��° ��ϸ� mpm_flag 0(���� ���)
		{
			Find_MPM(img, file);

			/* Entropy 1.1 MPM_Flag = 1; ǥ����ȣ�� 2���� (0, 1) : ���ڴ� �ʿ��� Find_MPM���� ��忹��, bit�� ���� */
			Send_PredInfo_FLC(file, file->MPM_flag, 2);
		}
		/* Entropy 1.2 MPM_Flag = 0; ǥ����ȣ�� 4���� (0, 1, 2, 3�� ���) : ���� ������ ������� ������ ���ڴ��� �����־�� �� */
		if (!file->MPM_flag)
			Send_PredInfo_FLC(file, PredMode, 4);												// �����������, ǥ����ȣ�� 4����
	}
	file->nbyte = 0;


	/* Entropy 2 ����ȭ�� ��ȯ����� ��ȣȭ */
	Send_Quantized_DCT_Coeff(file);

	// ����ȭ ��ȯ�������� DC���� DPCM ��ȣȭ
	Inverse_DC_DPCM(img, file, Quant_blk);
	Quantization(file, DCT_blk, Quant_blk, p_size, false);									// Quant_blk	-> DCT_blk		(inverse)
	IDCT(DCT_blk, Recon_DCT_blk, p_size);											// DCT_blk		-> Recon_DCT_blk

	for (int i = 0; i < p_size * p_size; i++) {
		Recon_DCT_blk[i] = round(Recon_DCT_blk[i]);
	}

	// DPCM���� ��ȣȭ
	Inverse_Pixel_DPCM(img, file, Recon_DCT_blk, ImgBuf);										// Recon_DCT_blk -> ImgBuf (recon)

	/* ������� + ������ ����������� = ������� -> �����̹��� */
	for (int j = 0; j < p_size; j++) {
		for (int i = 0; i < p_size; i++)
		{
			int recon_px = 0;
			int recon_pred_err = 0;

			/* no pixel dpcm */
			if (file->pixel_dpcm_mode == 6)
				recon_pred_err = (int)Recon_DCT_blk[j + i * p_size];

			/* use pixel dpcm */
			else
				recon_pred_err = (int)round(ImgBuf[j + i * p_size]);

			/* use Intra */
			if (file->intra_enable)
				recon_px = Pred_blk[PredMode][j + i * p_size] + recon_pred_err;

			/* no Intra */
			else
				recon_px = recon_pred_err;

			// Ŭ����
			if (recon_px > 255)
				img->Recon[upleft_px + j + i * img->width] = 255;
			else if (recon_px < 0)
				img->Recon[upleft_px + j + i * img->width] = 0;
			else
				img->Recon[upleft_px + j + i * img->width] = recon_px;
		}
	}

	// ���������� �Ҵ��� �����޸� ����
	free(Quant_blk);
	free(DCT_blk);
	free(Recon_DCT_blk);

	if (file->pixel_dpcm_mode != 6)
		free(Pixel_DPCM_blk);

	if (file->intra_enable)
	{
		free(PredErr_blk[0]);
		free(PredErr_blk);
		free(Pred_blk[0]);
		free(Pred_blk);
	}

	else
		free(ImgBuf);
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

	//�ֺ� ������κ��� ������ ���� �������� ���������� ������ ��ȣȭ��� ����� ���������� ������ MPM_flag = 1
	file->MPM_flag = (file->PM_Arr[img->blk_cnt] == ref_Pmode[1]) ? 1 : 0;
}