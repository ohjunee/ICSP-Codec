#include "Functions.h"

void Decoder(BUFFER* img, DATA* file, Node* head)
{
	// �� ó�� ������ �о����
	fread(&file->output, sizeof(uint8), 1, file->fp_in);

	head = (Node*)malloc(sizeof(Node));
	head->next = NULL;
	
	//������ ��ü frame ���ڵ�
	while (file->frameCount < VIDEO_FRAME)
	{
		Node* FirstNode = createFirstNode(head);	//��� �Ѱ� ����

		// ������ 1 frame �ڵ� Y/Cb/Cr ������ ����� ��Ʈ��Ʈ���� ���ڵ�
		for (int component = 0; component < 1; component++)
			component == 0 ? Read_Y(img, file, FirstNode, HEIGHT, WIDTH) : Read_CbCr(img, file, HEIGHT / 2, WIDTH / 2);

		if (file->frameCount)
			removeLastNode(head);

		file->frameCount++;
	}
}

// luma coding
void Read_Y(BUFFER* img, DATA* file, Node* POC, uint32 height, uint32 width)
{
	Setup_Memory(img, file, POC, height, width);	// ���� �Է�
	Partitioning(img, file, POC, height, width);	// ���� ����
	ImageSave(img, file, POC, height, width);		// ���� ���� ����
}

// chroma coding
void Read_CbCr(BUFFER* img, DATA* file, uint32 height, uint32 width)
{
	uint32 img_size = HEIGHT / 2 * WIDTH / 2;
	uint8* CbCr_img = (uint8*)calloc(img_size, sizeof(uint8));
	fread(CbCr_img, sizeof(uint8), img_size, file->fp_in);
	free(CbCr_img);
}

void Partitioning(BUFFER* img, DATA* file, Node* POC, uint32 height, uint32 width)
{
	uint32 UpLeftPx = 0;

	while (img->blk_cnt < img->total_blk)
	{
		img->isFirstBlk_in_row = !(img->blk_cnt % img->nBlk_width);	// 0�̸� true�� �Ǿ�, �� ���� ù��° ������� ǥ��
		img->isLastBlk_in_row = !((img->blk_cnt + 1) % img->nBlk_width);// 0�̸� true�� �Ǿ�, �� ���� ������ ������� ǥ��

		// �ι�° �� �̻��� ù��° ����� ���
		if (img->blk_cnt != 0 && img->isFirstBlk_in_row)
		{
			img->row_order++;
			UpLeftPx = img->row_order * width * PSIZE;
		}
		
		/* Read_INTER_INFO, Read_INTRA_INFO */
		(file->frameCount % INTRA_PERIOD) ? Read_Inter(img, file, POC->next, width, UpLeftPx) : Read_Intra(img, file, width, UpLeftPx);
		UpLeftPx += PSIZE;
		img->blk_cnt++;
	}
}

void Read_Intra(BUFFER* img, DATA* file, uint32 width, uint32 upleft_px)
{
	uint8* Pred_blk = (uint8*)calloc(PSIZE * PSIZE, sizeof(uint8));
	file->buffer = (double*)calloc(PSIZE * PSIZE, sizeof(double));	//������ �����������

	// A0. ������� �б�
	Read_PRED_INFO(file, &file->MPM_flag, 2);

	// A1. MPM_flag�� 0�� ��� ���ڴ����� ���� ������������� �о����, 
	if (!file->MPM_flag)
		Read_PRED_INFO(file, &file->PredMode, 4);					// �����������, ǥ����ȣ�� 4����(0, 1, 2, 3�� ���)

	// A2. 1�� ��� Find_MPM���� ��忹��, bit�� ����
	else
		Find_MPM(img, file, width);

	// A3. ������� �迭�� ��ȣȭ�� ������� ���� ����
	file->PM_Arr[img->blk_cnt] = file->PredMode;

	/* B. ���� ��忡 ���� ���� ��� ���� */
	getPredBlock(img, Pred_blk, file->PredMode, upleft_px); //nWidth + 1

	/* C2 ���޹��� ����������� ���� ��ȣȭ */
	for (int i = 0; i < PSIZE * PSIZE; i++)
	{
		int cnt = 0;

		while (cnt < 12)
		{
			uint8 result = 0, buffer = 0;

			buffer = file->output << (file->length % 8) & 128;

			result = result << 1 | buffer;

			file->length++;

			if (file->length % 8 == 0)
			{
				fread(&file->output, sizeof(uint8), 1, file->fp_in);
				file->length = 0;
			}

			if (cnt)
			{
				switch (result)
				{
				case 0:
					file->buffer[i] = 0;
					break;
				case 2:
					Read_Symbol(file, i, 0);
					break;
				case 3:
					Read_Symbol(file, i, 1);
					break;
				case 4:
					Read_Symbol(file, i ,2);
					break;
				case 5:
					Read_Symbol(file, i, 3);
					break;
				case 6:
					Read_Symbol(file, i, 4);
					break;
				case 14:
					Read_Symbol(file, i, 5);
					break;
				case 30:
					Read_Symbol(file, i, 6);
					break;
				case 62:
					Read_Symbol(file, i, 7);
					break;
				case 126:
					Read_Symbol(file, i, 8);
					break;
				case 254:
					Read_Symbol(file, i, 9);
					break;
				case 510:
					Read_Symbol(file, i, 10);
					break;
				case 1022:
					Read_Symbol(file, i, 11);
					break;
				case 2046:
					Read_Symbol(file, i, 12);
					break;
				}
			}
		
		}
	}

	/* Reordering (Decoder���� ��ȣȭ�� ����ȭ ������� ������) */
	Recon_ZigZagScan(img, file);										// file->buffer -> file->ZigZag_blk
	
	/* ����ȭ ��ȯ�������� DC���� DPCM ��ȣȭ */
	Inverse_DC_DPCM	(img, file);
	Inverse_Q		(img, file);
	IDCT			(img, file);										// file->ZigZag_blk -> img->Recon_DCT_blk

	/* D. ������� + ������ ����������� = ������� -> �����̹��� */
	for (uint8 j = 0; j < PSIZE; j++) {
		for (uint8 i = 0; i < PSIZE; i++)
		{
			int recon_px = Pred_blk[j + i * PSIZE] + (int)round(img->Recon_DCT_blk[j + i * PSIZE]);

			// Ŭ����
			if (recon_px > 255)
				img->Recon[upleft_px + j + i * width] = 255;
			else if (recon_px < 0)
				img->Recon[upleft_px + j + i * width] = 0;
			else
				img->Recon[upleft_px + j + i * width] = recon_px;
		}
	}
	free(file->buffer);
	free(Pred_blk);
}

#if TEST == 1
void Read_Inter(BUFFER* img, DATA* file, Node* POC, uint32 width, uint32 upleft_px)
{
	uint8* Pred_Block = (uint8*)calloc(PSIZE * PSIZE, sizeof(uint8));
	file->buffer = (double*)calloc(PSIZE * PSIZE, sizeof(double));	//������ �����������

	// A0. ������� �б�
	Read_PRED_INFO(file, &file->PredMode, 4);

	/* B. ���� ��忡 ���� ���� ��� ���� */
	getPredBlock(img, Pred_Block, file->PredMode, upleft_px); //nWidth + 1

	/* C2 ���޹��� ����������� ���� ��ȣȭ -> ������ȭ, IDCT ���� */

	/* Reordering (Decoder���� ��ȣȭ�� ����ȭ ������� ������) */
	Recon_ZigZagScan(img, file);										// file->buffer -> file->ZigZag_blk

	/* ����ȭ ��ȯ�������� DC���� DPCM ��ȣȭ */
	Inverse_DC_DPCM(img, file);
	Inverse_Q(img, file);
	IDCT(img, file);										// file->ZigZag_blk -> img->Recon_DCT_blk

	/* D. ������� + ������ ����������� = ������� -> �����̹��� */
	for (uint8 j = 0; j < PSIZE; j++) {
		for (uint8 i = 0; i < PSIZE; i++)
		{
			int recon_px = Pred_Block[j + i * PSIZE] + (int)round(file->Recon_DCT_blk[j + i * PSIZE]);

			// Ŭ����
			if (recon_px > 255)
				img->Recon[upleft_px + j + i * width] = 255;
			else if (recon_px < 0)
				img->Recon[upleft_px + j + i * width] = 0;
			else
				img->Recon[upleft_px + j + i * width] = recon_px;
		}
	}

	free(file->buffer);
	free(Pred_Block);
}
#endif


void Read_Symbol(DATA* file, uint8 idx, uint8 bitLen)
{
	/* read sign bit */
	uint8	sign = file->output << (file->length % 8) & 128, buffer = 0;
	uint32	result = 0;

	file->length++;

	if (file->length % 8 == 0)
	{
		fread(&file->output, sizeof(uint8), 1, file->fp_in);
		file->length = 0;
	}

	/* FLC */
	for (int i = 0; i < bitLen; i++)
	{
		buffer = file->output << (file->length % 8) & 128;

		result = result << 1 | buffer;

		file->length++;

		if (file->length % 8 == 0)
		{
			fread(&file->output, sizeof(uint8), 1, file->fp_in);
			file->length = 0;
		}
	}

	file->buffer[idx] = pow(-1, sign) * (result + pow(2, bitLen));
}


void Read_PRED_INFO(DATA* file, uint8* code, uint8 level)
{
	uint8 buffer = 0;
	uint8 bitLen = round(log(level, 2));		//ǥ����ȣ�� ������ ���� ��Ʈ�� ���� ����

	*code = 0;

	for (int cnt = 0; cnt < bitLen; cnt++)
	{
		buffer = (file->output << (file->length % 8) & 128) ? 1 : 0;
		*code = *code << 1 | buffer;

		file->length++;

		if (file->length % 8 == 0)
		{
			fread(&file->output, sizeof(uint8), 1, file->fp_in);
			file->length = 0;
		}
	}
}