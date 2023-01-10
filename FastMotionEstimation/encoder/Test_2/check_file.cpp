#include "Functions.h"

void Open_File(DATA* file, int argc, char* argv[])
{
	char ori[]			= { "G:\\images\\" };
	char recon[]			= { "G:\\images\\encoder\\reconImg\\" };
	char yuv[]			= { ".yuv" };

	file->in_img		= (char*)malloc(sizeof(char) * (strlen(argv[3]) + strlen(ori) + strlen(yuv)));
	file->recon_img = (char*)malloc(sizeof(char) * (strlen(argv[3]) + strlen(recon)) + strlen(yuv));

	if (file->in_img == 0 )
		exit(0);

	if (file->recon_img == 0)
		exit(0);

	// ���� ���� ��ġ ����
	strcpy(file->in_img, ori);
	strcpy(file->recon_img, recon);

	// ���� ���� �̸��� ������ġ�� �̾� ���̱�
	strcat(file->in_img, argv[9]);

	// ��������, ��Ʈ��Ʈ�� ���� ��ȣ ����
	memmove(argv[3], argv[2], strlen(argv[2]));

	// ��������, ��Ʈ��Ʈ�� ���� �̸��� ������ġ�� �̾���̱�
	strcat(file->recon_img, argv[3]);

	// Ȯ���� ���� ������ġ�� �̾���̱�
	strcat(file->in_img,		yuv);
	strcat(file->recon_img, yuv);

	file->fp_ori			= fopen(file->in_img,		"rb");			// �Է� ����
	file->fp_ori_recon	= fopen(file->recon_img,		"wb");			// ���ڴ� ���� ����

	file->VideoFrame			= atoi(argv[1]);

	file->frameCount		= 0;
	file->output			= 0;
	file->length			= 0;
	file->MVmodeflag		= 0;
}

void Close_File(DATA* file)
{
	fclose(file->fp_ori);
	fclose(file->fp_ori_recon);
}

void checkFile(DATA* file, int argc, char* argv[], bool Bool)
{
	(Bool) ? Open_File(file, argc, argv) : Close_File(file);
}