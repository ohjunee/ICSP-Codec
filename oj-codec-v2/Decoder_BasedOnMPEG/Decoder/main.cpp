#include "Functions.h"

int main(int argc, char* argv[])
{
	BUFFER	image;
	DATA	file;
	Node	POC;

	checkFile(&file, argc, argv, true);				// ������ ���� ����
	Decoder(&image, &file, &POC);	// ���ڴ� �Է�/���
	checkFile(&file, argc, argv, false);			// ������ ���� �ݱ�

	Check_Image(&file, argv);
	return 0;
}