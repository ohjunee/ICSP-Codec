#include "Buffer.h"
#include "Node.h"
#include "input.h"

int main()
{
	BUFFER	image;
	Data	file;

	checkFile	(&file, true);	// ������ ���� ����
	encoder		(&image, &file);// ���ڴ� �Է�/���
	checkFile	(&file, false);	// ������ ���� �ݱ�

	return 0;
}