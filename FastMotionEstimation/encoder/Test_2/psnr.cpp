#include "Functions.h"

void PSNR(DATA* file, char* argv[])
{
	file->fp_ori			= fopen(file->in_img,	 "rb");	// �Է� ����
	file->fp_ori_recon  = fopen(file->recon_img, "rb");	// ���ڴ� ���� ����
	file->fp_txt			= fopen(TEXT_FILE,		 "a");	// ����� ��� �ؽ�Ʈ����

	double MAX_2 = 255.0 * 255.0;

	double OriFileSize		= (HEIGHT * WIDTH * 1.5 * file->VideoFrame);
	double	psnr	= 0;

	double	MSE		= 0;
	uint8	ori		= 0;
	uint8	recon	= 0;

	uint32	d		= 0;
	uint32	sum		= 0;

	uint32 imageSize  = 0;
	
	uint16  frame	  = 0;

	//������ ��ü frame �ڵ�
	while (frame < file->VideoFrame){

		MSE = 0;

		// 1 frame�� ���� ���� ������
		for (int component = 0; component < 3; component++)
		{
			imageSize = !component ? HEIGHT * WIDTH : HEIGHT * WIDTH / 4;

			sum = 0;

			//�� ���к� ���� ������
			for (uint32 i = 0; i < imageSize; i++)
			{
				fread(&ori, sizeof(uint8), 1, file->fp_ori);
				fread(&recon, sizeof(uint8), 1, file->fp_ori_recon);

				d = abs(ori - recon);
				sum += d * d;
			}

			MSE += (double)sum / (double)imageSize;
		}

		psnr += 10 * log10(MAX_2 / MSE);

		frame++;
	}

	psnr /= file->VideoFrame;

	fprintf(file->fp_txt, "%s: PSNR: %fdB\n", argv[3], psnr);
	printf("%s: PSNR: %fdB\n", argv[3], psnr);
	fclose(file->fp_ori_recon);
	fclose(file->fp_ori);
	fclose(file->fp_txt);
}