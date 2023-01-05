#include "Buffer.h"
#include "Node.h"
#include "input.h"

#if TEST_HUFFMAN == 1
void Read_Sign(Data* file, uint8 p_size)
{
	uint8 sign;

	for (int i = 0; i < p_size*p_size; i++)
	{
		sign = (file->output << (file->length % 8) & 128) ? 1 : 0;

		file->Recon_DCT_blk[i] = pow(-1, sign);

		file->length++;

		if (file->length % 8 == 0)
		{
			fread(&file->output, sizeof(uint8), 1, file->fp_in);
			file->length = 0;
		}
	}
}

void Read_Huffman_Table(Data* file, uint8 p_size)
{
	// �ɺ� ������ ���� �б�
	Fixed_Length_Decoding(file, p_size, READ_SYMBOL_NUM);

	//�ɺ� ��, �� ���� ������� �޸� �Ҵ�
	Tree_memory_alloc1D(file);

	//�ɺ� ������ ������ŭ �ɺ� �� �б�
	Exponetial_Golomb_Decoding(file, p_size, READ_SYMBOL_VAL);

	//���ũ�⸸ŭ �� �ɺ��� �� �� �� �о����
	if (file->n_symbol > 2)
		Fixed_Length_Decoding(file, p_size, READ_FREQ);
}

void Read_AbsVal(Data* file, uint8 p_size)
{
	file->index = 0;		// ��ȯ��� ������ �����ε��� ��ġ �ʱ�ȭ
							
	file->n_symbol > 2 ? Huffman_decoding(file, p_size) : Read_Symbol(file, p_size); // �ɺ� ���� ���� 3�� �̻��� ��쿡�� ������ Ʈ�������, ������� Ʈ�������� ���� ��Ʈ��Ʈ�� �б�
																					 // 2�� ������ ���� ��Ʈ��Ʈ�� ������ �ɺ��� ����
	//������ ���̺� ���� �޸� ����
	Tree_memory_free1D(file);
}

void Read_Symbol(Data* file, uint8 p_size)
{
	for (int i = 0; i < p_size*p_size; i++)
	{
		file->Recon_DCT_blk[i] *= (file->output << (file->length % 8) & 128) ? file->symbol[1] : file->symbol[0];		// ���ǹ��� ��-> ���� ��Ʈ�� 1, table�� �ι�° ��� ��// �ɺ������� 1������ ��� 0��° �ε������� �����
		file->length++;

		if (file->length % 8 == 0)
		{
			fread(&file->output, sizeof(uint8), 1, file->fp_in);
			file->length = 0;
		}
	}
}

void Fixed_Length_Decoding(Data* file, uint8 p_size, int type)
{
	uint8 buffer = 0, bitLen = round(log(p_size*p_size, 2)); //ǥ����ȣ�� ������ ���� ��Ʈ�� ���� ����

	for (int i = 0; i < (type ? 1 : file->n_symbol); i++)
	{
		if (type)
			file->n_symbol = 0;

		for (int j = 0; j < bitLen; j++)
		{
			buffer = (file->output << (file->length % 8) & 128) ? 1 : 0;

			if (type)
			{
				file->n_symbol = file->n_symbol << 1 | buffer;
			}
			else
			{
				file->freq[i] = file->freq[i] << 1 | buffer;
			}

			file->length++;

			if (file->length % 8 == 0)
			{
				fread(&file->output, sizeof(uint8), 1, file->fp_in);
				file->length = 0;
			}
		}

	}
}
#else

void Read_Sign(Data* file, uint8 p_size)
{
	uint8 sign;

	for (int i = 0; i < p_size*p_size; i++)
	{
		sign = (file->output << (file->length % 8) & 128) ? 1 : 0;

		file->length++;

		if (file->length % 8 == 0)
		{
			fread(&file->output, sizeof(uint8), 1, file->fp_in);
			file->length = 0;
		}
		file->Recon_DCT_blk[i] = pow(-1, sign);
	}
}

#endif

void Read_PredErr(Data* file, uint8 p_size)
{
#if TEST_HUFFMAN == 1
	Read_Sign			(file, p_size);
	Read_Huffman_Table	(file, p_size);
	Read_AbsVal			(file, p_size);
#else
	Read_Sign(file, p_size);
	Exponetial_Golomb_Decoding(file, p_size, READ_SYMBOL_VAL_);
#endif
	/* C2 ���޹��� ����������� ���� ������ȭ, IDCT ���� */
	I_Quantization(file, p_size);

	IDCT(file, p_size, file->DCT_skip);
}

void Read_PRED_INFO(Data* file, uint8* code, uint8 level)
{
	uint8 buffer = 0;
	uint8 bitLen = round(log(level, 2));		//ǥ����ȣ�� ������ ���� ��Ʈ�� ���� ����

	*code = 0;

	for (int cnt = 0; cnt < bitLen; cnt++)
	{
		buffer	= (file->output << (file->length % 8) & 128) ? 1 : 0;
		*code	= *code << 1 | buffer;

		file->length++;

		if (file->length % 8 == 0)
		{
			fread(&file->output, sizeof(uint8), 1, file->fp_in);
			file->length = 0;
		}
	}
}

void Exponetial_Golomb_Decoding(Data* file, uint8 p_size, int type)
{
	for (int i = 0; i < (type ? file->n_symbol : p_size*p_size); i++)		//type 1 : �ɺ� �� �б�, 0 : �������� ���밪�б�
	{
		uint8 result = 0, buffer = 0, bitLen = 8;

		for (int cnt = 0; cnt < bitLen; cnt++)
		{
			buffer = (file->output << (file->length % 8) & 128) ? 1 : 0;
			result = result << 1 | buffer;

			file->length++;

			if (result == 1)
			{
				bitLen = cnt * 2 + 1;
			}

			if (file->length % 8 == 0)
			{
				fread(&file->output, sizeof(uint8), 1, file->fp_in);
				file->length = 0;
			}
		}
		result -= 1;
		type ? file->symbol[i] = result : file->Recon_DCT_blk[i] *= result;
	}
}