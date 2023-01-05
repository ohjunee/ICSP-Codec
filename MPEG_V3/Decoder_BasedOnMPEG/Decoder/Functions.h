#pragma once
#include "structure.h"
#include "input.h"

void		Setup(BUFFER* img, DATA* file, Node* POC);
void		ImageSave(BUFFER* img, DATA* file);
void		Padding(BUFFER* img, DATA* file);

void		checkFile(DATA* file, int argc, char* argv[], bool Bool);

void		Open_File(DATA* file, int argc, char* argv[]);
void		Close_File(DATA* file);
void		Check_Image(DATA* file, char* argv[]);

double		DCT_Basis_Vector(uint8 k, uint8 n, uint8& p_size);

void		Inverse_Q(BUFFER* img, DATA* file);
void		IDCT(BUFFER* img, DATA* file);

void		Recon_ZigZagScan(BUFFER* img, DATA* file);
void		Inverse_DC_DPCM(BUFFER* img, DATA* file);
void		Inverse_Pixel_DPCM(BUFFER* img, DATA* file, double* PredErr);

void		Decoder(BUFFER* img, DATA* file, Node* head);
void		getPredBlock(BUFFER* img, uint8* Pred_b, uint8& mode, uint32& upleft_px);

void		Read_StreamHeader(DATA* file);
void		Read_PRED_INFO(DATA* file, uint8& code, uint8 level);
void		Read_Symbol(DATA* file, uint8& idx, uint8 bitLen, int type);

void		Read_Inter(BUFFER* img, DATA* file, Node* POC, uint32& upleft_px);
void		Read_Intra(BUFFER* img, DATA* file, uint32& upleft_px);

void		Find_MPM(BUFFER* img, DATA* file);
void		Partitioning(BUFFER* img, DATA* file, Node* POC);

void		Read_YCbCr(BUFFER* img, DATA* file, Node* POC);
void		Read_MV(DATA* file);

Node* createFirstNode(Node* head);
void		removeLastNode(Node* head);


// 2���� �޸� �����Ҵ�
template <typename T>
T** memory_calloc2D(uint16 row, uint16 col)
{
	T** pp = 0;

	pp = (T**)calloc(row, sizeof(T*));

	*pp = (T*)calloc(row * col, sizeof(T));

	for (int j = 1; j < row; j++)
	{
		pp[j] = pp[j - 1] + col;
	}

	return pp;
}
template <typename T>
void swap(T& left, T& right)
{
	T tmp = left;
	left = right;
	right = tmp;
}

//quicksort�� ����� ����: ������� ���� ������ ��� �������� �����ϴµ� �ɸ��� �ð��� ª��
template <typename T>
void QuickSort(T* ref_val, int left, int right)
{
	int L = left;
	int R = right;

	T pivot = ref_val[(L + R) / 2];

	do
	{
		while (ref_val[L] < pivot)
			L++;
		while (ref_val[R] > pivot)
			R--;

		if (L <= R)
		{
			swap(ref_val[L], ref_val[R]);
			L++; R--;
		}

	} while (L <= R);

	//�Ǻ� ��������

	//���� ����: R�� ������ �Ǹ�, ���� �迭�� �������̹Ƿ� ����x
	if (left < R)
		QuickSort(ref_val, left, R);

	//������ ����: L�� ���ҵ� �迭�� ������ ���� �����ϸ� ����x
	if (right > L)
		QuickSort(ref_val, L, right);
}

/* Entropy 4.1 ��ȣ��Ʈ ����: �ɺ����� 0�� �ƴ� ��츸  */
template<typename T>
void Send_Sign(DATA* file, T& sign)
{
	file->output = file->output << 1 | sign;
	file->length++;

	if (file->length == 8)
	{
		fwrite(&file->output, sizeof(uint8), 1, file->fp_out);
		file->output = 0;
		file->length = 0;
	}
}

/* Entropy 4.2 Range ��Ʈ ����: �ɺ����� 2 �̻��� ��츸 range��Ʈ �������̺�ȣȭ */
template<typename T>
void Send_Range_FLC(DATA* file, T& codenum)
{
	uint8 bitLen = ceil(log(codenum, 2));				// �ɺ� ���̴� ����ȭ��ȯ����� ���밪�� �α� ���� ��
	uint16 val = codenum - pow(2, bitLen - 1);				// ���� codeword�� 0���� ���߱� ���� (���� codenum - �ڱ� �ڽ�) �� ����

	uint16 check_bit = pow(2, bitLen - 1);				// �˻��Ʈ
	uint8 buffer;

	for (int i = 0; i < bitLen; i++)
	{
		buffer = val << i & check_bit ? 1 : 0;

		file->output = file->output << 1 | buffer;
		file->length++;

		if (file->length % 8 == 0)
		{
			fwrite(&file->output, sizeof(uint8), 1, file->fp_out);
			file->length = 0;
			file->output = 0;
		}
	}
}


