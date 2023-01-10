#include "Functions.h"

int main(int argc, char* argv[])
{
	BUFFER	image;
	DATA	file;
	Node	ref_image;

	checkFile(&file, argc, argv, true);				// ������ ���� ����
	encoder(&image, &file, &ref_image);				// ���ڴ� �Է�/���
	checkFile(&file, argc, argv, false);			// ������ ���� �ݱ�

	PSNR(&file, argv);

	return 0;
}