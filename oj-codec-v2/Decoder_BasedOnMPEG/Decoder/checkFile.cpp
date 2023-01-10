#include "Functions.h"
#pragma warning(disable: 6387)

void Open_File(DATA* file, int argc, char* argv[])
{
	char ori[] = { "G:\\images\\" };
	char recon_en[] = { "G:\\images\\encoder\\ver_1.1\\recon_���ڴ�\\" };
	char recon_de[] = { "G:\\images\\decoder\\recon_���ڴ�\\" };
	char bitstream[] = { "G:\\images\\encoder\\ver_1.1\\bitstream\\" };

	char yuv[] = { ".yuv" };
	char raw[] = { ".raw" };

	file->ori = (char*)malloc(sizeof(char) * (strlen(argv[3]) + strlen(ori) + strlen(yuv)));
	file->recon_en = (char*)malloc(sizeof(char) * (strlen(argv[3]) + strlen(recon_en)) + strlen(yuv));
	file->recon_de = (char*)malloc(sizeof(char) * (strlen(argv[3]) + strlen(recon_de)) + strlen(yuv));
	file->infile = (char*)malloc(sizeof(char) * (strlen(argv[3]) + strlen(bitstream) + strlen(raw)));

	if (file->ori == 0)
		exit(0);
	if (file->recon_en == 0)
		exit(0);
	if (file->recon_de == 0)
		exit(0);
	if (file->infile == 0)
		exit(0);

	// ���� ���� ��ġ ����
	strcpy(file->infile, bitstream);
	strcpy(file->recon_en, recon_en);
	strcpy(file->recon_de, recon_de);
	strcpy(file->ori, ori);

	// ���� ���� �̸��� ������ġ�� �̾� ���̱�
	strcat(file->ori, argv[3]);

	// ��������, ��Ʈ��Ʈ�� ���� ��ȣ ����
	memmove(argv[3], argv[2], strlen(argv[2])); // 3�� �� ù��° ���ں��� 2������ ����

	// ��������, ��Ʈ��Ʈ�� ���� �̸��� ������ġ�� �̾���̱�
	strcat(file->recon_en, argv[3]);
	strcat(file->recon_de, argv[3]);
	strcat(file->infile, argv[3]);

	// Ȯ���� ���� ������ġ�� �̾���̱�
	strcat(file->infile, raw);
	strcat(file->recon_de, yuv);

	strcat(file->ori, yuv);
	strcat(file->recon_en, yuv);

	file->fp_in = fopen(file->infile, "rb");		// �Է� ����(��Ʈ��Ʈ��)
	file->fp_out = fopen(file->recon_de, "wb");		// ���ڴ� ���� ����

	file->VideoFrame = atoi(argv[1]);

	file->frameCount = 0;
	file->output = 0;
	file->length = 0;

	file->AC_flag = 0;
}

void Close_File(DATA* file)
{
	fclose(file->fp_in);
	fclose(file->fp_out);
}

void checkFile(DATA* file, int argc, char* argv[], bool Bool)
{
	(Bool) ? Open_File(file, argc, argv) : Close_File(file);
}

void Check_Image(DATA* file, char* argv[])
{
	double MAX_2 = 255.0 * 255.0, psnr = 0, MSE = 0;
	double OriFileSize = (file->height * file->width * 1.5 * file->VideoFrame);

	uint8 ori = 0, recon = 0;
	uint32 d = 0, sum = 0;

	uint16 frame = 0;
	uint32 imageSize = 0;
	uint32 cnt = 0;

	fopen_s(&file->fp_in, file->infile, "rb");	// ��Ʈ��Ʈ�� ����
	fopen_s(&file->fp_out, file->recon_de, "rb");	// ���ڴ� ��������

	fopen_s(&file->fp_ori, file->ori, "rb");	// ��������
	fopen_s(&file->fp_recon_en, file->recon_en, "rb");	// ���ڴ� ��������
	fopen_s(&file->fp_txt, TEXT_FILE, "a");	// ����� �ؽ�Ʈ����

	fseek(file->fp_in, 0L, SEEK_END);
	file->inFileSize = ftell(file->fp_in);

	double compressibility = ((double)file->inFileSize / OriFileSize) * 100;


	/* ���ڴ� ���ڴ� ���������� mismatch Ȯ��*/
	while (feof(file->fp_recon_en) == 0 && feof(file->fp_out) == 0)
	{
		uint8 file_1_pixel, file_2_pixel;

		fread(&file_1_pixel, sizeof(uint8), 1, file->fp_recon_en);
		fread(&file_2_pixel, sizeof(uint8), 1, file->fp_out);

		cnt++;

		if (file_1_pixel != file_2_pixel)
		{
			printf("�� ������ �ٸ��ϴ�. cnt: %d\n\n", cnt);
			break;
		}
	}
	printf("�� ������ �����ϴ�.\n");

	fseek(file->fp_out, 0L, SEEK_SET);

	/* psnr �� ����� ��� */
	while (frame < file->VideoFrame) {

		MSE = 0;

		// 1 frame�� ���� ���� ������
		for (int component = 0; component < 3; component++)
		{
			imageSize = !component ? file->height * file->width : file->height * file->width / 4;

			sum = 0;

			//�� ���к� ���� ������
			for (uint32 i = 0; i < imageSize; i++)
			{
				fread(&ori, sizeof(uint8), 1, file->fp_ori);
				fread(&recon, sizeof(uint8), 1, file->fp_out);

				d = abs(ori - recon);
				sum += d * d;
			}

			MSE += (double)sum / (double)imageSize;
		}

		psnr += 10 * log10(MAX_2 / MSE);

		frame++;
	}

	psnr /= file->VideoFrame;

	fprintf(file->fp_txt, "%s: pixel_dpcm: %d, dc_dpcm: %d, intra_enable: %d, intra_period: %d, QP_dc: %d, QP_ac: %d,PSNR: %fdB, compressibility %f%\n", argv[3], file->pxDPCM_MODE, file->DC_DPCM_MODE, file->IntraEnable, file->IntraPeriod, file->QP_DC, file->QP_AC, psnr, compressibility);
	printf("%s: pixel_dpcm: %d, dc_dpcm: %d, intra_enable: %d, intra_period: %d, QP_dc: %d, QP_ac: %d,PSNR: %fdB, compressibility %f%\n", argv[3], file->pxDPCM_MODE, file->DC_DPCM_MODE, file->IntraEnable, file->IntraPeriod, file->QP_DC, file->QP_AC, psnr, compressibility);

	fclose(file->fp_ori);
	fclose(file->fp_recon_en);

	fclose(file->fp_txt);
	fclose(file->fp_in);
	fclose(file->fp_out);
}