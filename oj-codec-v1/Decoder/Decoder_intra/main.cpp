#include "Buffer.h"
#include "input.h"

int main()
{
	BUFFER image;
	Data	file;

	Check_File	(&file,  true);			// ������ �������� ����
	Decoder		(&image, &file);		// ���ڴ� �Է�/���
	Check_File	(&file,  false);			// ������ ���� �ݱ�

	Check_Image(&file);
	return 0;
}