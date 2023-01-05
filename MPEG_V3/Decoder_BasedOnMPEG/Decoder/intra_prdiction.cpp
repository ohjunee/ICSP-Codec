#include "Functions.h"

void Read_Intra(BUFFER* img, DATA* file, uint32& upleft_px)
{
	uint8 p_size = img->p_size;

	uint8* Pred_blk = 0;
	double* ImgBuf = (double*)calloc(img->macroSize, sizeof(double));

	if (!ImgBuf)
	{
		printf("memory error");
		exit(0);
	}

	memset(file->recon_bit, 0, sizeof(int) * img->macroSize);

	//INTRA_ENABLE ������ ���� ������ ���� ����
	if (file->IntraEnable)
	{
		Pred_blk = (uint8*)calloc(img->macroSize, sizeof(uint8));

		if (!Pred_blk)
		{
			printf("memory error");
			exit(0);
		}

		// A0. MPM_flag �б�
		if (img->blk_cnt)
			Read_PRED_INFO(file, file->MPM_flag, 2);

		// A1. MPM_flag�� 0�� ��� ���ڴ����� ���� ������������� �о����, 
		if (!file->MPM_flag)
			Read_PRED_INFO(file, file->PredMode, 4);					// �����������, ǥ����ȣ�� 4����(0, 1, 2, 3�� ���)

		// A2. 1�� ��� Find_MPM���� ��忹��, bit�� ����
		else
			Find_MPM(img, file);

		// A3. ������� �迭�� ��ȣȭ�� ������� ���� ����
		file->PM_Arr[img->blk_cnt] = file->PredMode;

		/* B. ���� ��忡 ���� ���� ��� ���� */
		getPredBlock(img, Pred_blk, file->PredMode, upleft_px); //nWidth + 1
	}

	/* C2 ���޹��� ����������� ���� ��ȣȭ */
	for (uint8 i = 0; i < img->macroSize; i++)
	{
		uint8 bitCnt = 0;
		uint16 result = 0, buffer = 0;

		if (i == 1)
		{
			file->AC_flag = file->output << (file->length % 8) & 128 ? 1 : 0;
			file->length++;

			if (file->length % 8 == 0)
			{
				fread(&file->output, sizeof(uint8), 1, file->fp_in);
				file->length = 0;
			}

			/* AC flag = 1; all-zero��� �ǹ̷� 1��Ʈ ����, AC���� ���ڵ� x, ���ڴ����� 1�̶�� ��Ʈ�� ������, AC���а��� ��� 0���� ����) */
			if (file->AC_flag)
				break;
		}

		while (bitCnt < 10)
		{
			// read code word
			buffer = file->output << (file->length % 8) & 128 ? 1 : 0;

			result = result << 1 | buffer;

			file->length++;
			bitCnt++;

			if (file->length % 8 == 0)
			{
				fread(&file->output, sizeof(uint8), 1, file->fp_in);
				file->length = 0;
			}

			switch (result)
			{
			case 0:
				if (bitCnt == 2)
				{
					file->recon_bit[i] = 0;
					bitCnt = 10;
				}
				break;

			case 2:
				if (bitCnt == 3)
				{
					int sign = file->output << (file->length % 8) & 128 ? 1 : 0;
					file->length++;
					file->recon_bit[i] = pow(-1, sign);

					if (file->length % 8 == 0)
					{
						fread(&file->output, sizeof(uint8), 1, file->fp_in);
						file->length = 0;
					}

					bitCnt = 10;
				}
				break;

			case 3:
				if (bitCnt == 3)
				{
					Read_Symbol(file, i, 1, QT_COEFF);
					bitCnt = 10;
				}
				break;

			case 4:
				if (bitCnt == 3)
				{
					Read_Symbol(file, i, 2, QT_COEFF);
					bitCnt = 10;
				}
				break;

			case 5:
				if (bitCnt == 3)
				{
					Read_Symbol(file, i, 3, QT_COEFF);
					bitCnt = 10;
				}
				break;

			case 6:
				if (bitCnt == 3)
				{
					Read_Symbol(file, i, 4, QT_COEFF);
					bitCnt = 10;
				}
				break;

			case 14:
				if (bitCnt == 4)
				{
					Read_Symbol(file, i, 5, QT_COEFF);
					bitCnt = 10;
				}
				break;

			case 30:
				if (bitCnt == 5)
				{
					Read_Symbol(file, i, 6, QT_COEFF);
					bitCnt = 10;
				}
				break;

			case 62:
				if (bitCnt == 6)
				{
					Read_Symbol(file, i, 7, QT_COEFF);
					bitCnt = 10;
				}
				break;

			case 126:
				if (bitCnt == 7)
				{
					Read_Symbol(file, i, 8, QT_COEFF);
					bitCnt = 10;
				}
				break;

			case 254:
				if (bitCnt == 8)
				{
					Read_Symbol(file, i, 9, QT_COEFF);
					bitCnt = 10;
				}
				break;

			case 510:
				if (bitCnt == 9)
				{
					Read_Symbol(file, i, 10, QT_COEFF);
					bitCnt = 10;
				}
				break;

			case 1022:
				Read_Symbol(file, i, 11, QT_COEFF);
				break;
			}
		}
	}

	/* Reordering (Decoder���� ��ȣȭ�� ����ȭ ������� ������) */
	Recon_ZigZagScan(img, file);										// file->recon_bit -> file->ZigZag_blk

	/* ����ȭ ��ȯ�������� DC���� DPCM ��ȣȭ */
	Inverse_DC_DPCM(img, file);
	Inverse_Q(img, file);
	IDCT(img, file);										// file->ZigZag_blk -> img->Recon_DCT_blk

	for (int i = 0; i < p_size * p_size; i++) {
		img->Recon_DCT_blk[i] = round(img->Recon_DCT_blk[i]);
	}

	Inverse_Pixel_DPCM(img, file, ImgBuf);

	/* D. ������� + ������ ����������� = ������� -> �����̹��� */
	for (uint8 j = 0; j < p_size; j++) {
		for (uint8 i = 0; i < p_size; i++)
		{
			int recon_px = 0;
			int recon_pred_err = 0;

			/* no pixel dpcm */
			if (file->pxDPCM_MODE == 6)
				recon_pred_err = (int)img->Recon_DCT_blk[j + i * p_size];

			/* use px dpcm */
			else
				recon_pred_err = (int)round(ImgBuf[j + i * p_size]);

			/* use Intra */
			if (file->IntraEnable)
				recon_px = Pred_blk[j + i * p_size] + recon_pred_err;

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

	if (file->IntraEnable)
		free(Pred_blk);

	free(ImgBuf);
}