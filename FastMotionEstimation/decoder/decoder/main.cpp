#include "Functions.h"

int main()
{
	BUFFER	image;
	DATA	file;
	Node	POC;

	Check_File(&file, true);		// ������ �������� ����
	Decoder(&image, &file, &POC);	// ���ڴ� �Է�/���
	Check_File(&file, false);		// ������ ���� �ݱ�

	Check_Image(&file);
	return 0;
}