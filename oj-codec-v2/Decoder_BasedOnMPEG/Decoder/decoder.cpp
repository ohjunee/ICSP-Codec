#include "Functions.h"

void Decoder(BUFFER* img, DATA* file, Node* head)
{
	Read_StreamHeader(file);

	head = (Node*)malloc(sizeof(Node));

	if (head == 0)
	{
		printf("memory loss");
		exit(0);
	}

	head->next = NULL;

	//������ ��ü frame ���ڵ�
	while (file->frameCount < file->VideoFrame)
	{
		Node* FirstNode = createFirstNode(head);	//��� �Ѱ� ����

		// ������ 1 frame �ڵ� Y/Cb/Cr ������ ����� ��Ʈ��Ʈ���� ���ڵ�
		for (int component = 0; component < 3; component++)
		{
			file->comp = component;
			Read_YCbCr(img, file, FirstNode);
		}

		if (file->frameCount)
			removeLastNode(head);

		file->frameCount++;
	}
}

void Read_YCbCr(BUFFER* img, DATA* file, Node* POC)
{
	Setup(img, file, POC);		// ���� �Է�
	Partitioning(img, file, POC);		// ���� ����
	ImageSave(img, file);		// ���� ���� ����
}

void Partitioning(BUFFER* img, DATA* file, Node* POC)
{
	// current picture init
	uint32 UpLeftPx = 0;

	//reference picture init
	img->UpLeftPx_ref = img->p_size + img->width_pad * img->p_size;

	while (img->blk_cnt < img->total_blk)
	{
		img->isFirstBlk_in_row_org = !(img->blk_cnt % img->nBlk_width);		// 0�̸� true�� �Ǿ�, �� ���� ù��° ������� ǥ��
		img->isLastBlk_in_row_org = !((img->blk_cnt + 1) % img->nBlk_width);// 0�̸� true�� �Ǿ�, �� ���� ������ ������� ǥ��

		// �ι�° �� �̻��� ù��° ����� ���
		if (img->blk_cnt != 0 && img->isFirstBlk_in_row_org)
		{
			img->row_order++;
			UpLeftPx = img->row_order * img->width * img->p_size;

			if (file->frameCount % (file->IntraPeriod + 1))
				img->UpLeftPx_ref = img->p_size + (img->row_order + 1) * img->width_pad * img->p_size;
		}

		/* Read_INTER_INFO, Read_INTRA_INFO */
		(file->frameCount % (file->IntraPeriod + 1)) ? Read_Inter(img, file, POC->next, UpLeftPx) : Read_Intra(img, file, UpLeftPx);

		UpLeftPx += img->p_size;
		img->blk_cnt++;
	}
}