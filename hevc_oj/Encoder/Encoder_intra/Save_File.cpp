#include "Buffer.h"
#include "Node.h"
#include "input.h"

void Send_Quantized_DCT_Coeff(Data* file, double* Pred_Err, uint8 p_size)
{
	uint8 n_symbol = 0;

	file->PredErr = Pred_Err;

	Send_Sign(file, p_size);

#if TEST_HUFFMAN == 1
	//������
	Tree_memory_alloc1D(file, p_size*p_size);

	Extract_symbol(file, p_size, n_symbol);					//file ����ü ������ ����ȭ ��ȯ��� ����� �ɺ�����, �ɺ���, �󵵼�, �׸��� �Է½������� ����(��ȯ��� ��� �� ȭ�Ұ����� ���밪)

	n_symbol > 2 ? Huffman_coding(file, n_symbol, p_size) : Send_Symbol(file, n_symbol, p_size);

	Tree_memory_free1D(file);
#else
	//�������
	Exponetial_Golomb_coding(file, n_symbol, p_size, SEND_SYMBOL_VAL_);
#endif
}

#if TEST_HUFFMAN == 1
void Send_Symbol(Data* file, uint8 n_symbol, uint8 p_size)
{
	Send_Huffman_Table(file, n_symbol, p_size);

	for (int i = 0; i < p_size*p_size; i++)
	{
		uint8 bitVal = file->input_sequence[i] == file->symbol[0] ? 0 : 1;	//�ɺ��� �� ���� ��� file->symbol[0]�� �ɺ� ���� �����ϹǷ� ��Ʈ�� 0�� ������
		file->output = file->output << 1 | bitVal;
		file->length++;

		if (file->length == 8)
		{
			fwrite(&file->output, sizeof(uint8), 1, file->fp_out);
			file->output = 0;
			file->length = 0;
		}
	}
}

void Fixed_Length_Coding(Data* file, uint8 n_symbol, uint8 p_size, int type)
{
	uint8 bitLen = round(log(p_size*p_size, 2)); //�ɺ����� 4*4����� �ִ� 16��, 4��Ʈ/ 8*8�� �ִ� 64��, 6��Ʈ/ 3: 0010
	uint8 check_bit = pow(2, bitLen - 1);
	uint8 buffer;

	for (int i = 0; i < (type ? 1 : n_symbol); i++)
	{
		for (int j = 0; j < bitLen; j++)
		{
			if (type)
			{
				buffer = n_symbol << j & check_bit ? 1 : 0;
			}	
			else
			{
				buffer = file->freq[i] << j & check_bit ? 1 : 0;
			}

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
}

void Send_Huffman_Table(Data* file, uint8 n_symbol, uint8 p_size)
{
	//����ȭ�� ��ȯ��� ��Ͽ��� �����Ǵ� �ɺ� ���� ���� �����ֱ�			
	Fixed_Length_Coding(file, n_symbol, p_size, SEND_SYMBOL_TYPE);

	//������ �ɺ� �� ���ʷ� ������
	Exponetial_Golomb_coding(file, n_symbol, p_size, SEND_SYMBOL_VAL);

	//�ɺ��� ������ 3�� �̻��� ��츸 �ɺ��� �߻� �󵵼��� ���ʷ� ������
	if (n_symbol > 2)
		Fixed_Length_Coding(file, n_symbol, p_size, SEND_FREQ);
}

void Huffman_coding(Data* file, uint8 n_symbol, uint8 p_size)
{
	int* codeword = (int*)calloc(sizeof(int), p_size*p_size);
	int start_index = 0;

	Node*	head = NULL;																	// �Ӹ���� ����, �ڽĳ��� ��� NULL�� �ʱ�ȭ
	Node**	tail = (Node**)malloc(sizeof(Node*)*n_symbol);

	initialize(file, tail, n_symbol);

	Send_Huffman_Table(file, n_symbol, p_size);

	head = create_HuffmanTree(tail, n_symbol);												// �Ӹ� ��带 ������ Ʈ���� ����

	for (int i = 0; i < p_size*p_size; i++)
	{
		Tree_traversal(head, file, file->input_sequence[i], codeword, start_index);			// ���� ��ȸ�Ͽ� ���� ���������� Ž��
	}

	free_Tree(head);																		// ���� ��ȸ������� �޸� ���� ����
	free(codeword);
}
#endif

void Exponetial_Golomb_coding(Data* file, uint8& n_symbol, uint8 p_size, int type)
{
	uint8 result, codeNum = 0, buffer = 0;

	for (int i = 0; i < (type ? n_symbol : p_size*p_size); i++)
	{
		result = type ? file->symbol[i] : (uint8)fabs(file->PredErr[i]);

		codeNum = result + 1;

		uint8	prefix_num = log(codeNum, 2);
		uint8	bitLen = prefix_num * 2 + 1;

		for (int cnt = 0; cnt < bitLen; cnt++)
		{
			buffer = set_bits(codeNum, prefix_num, cnt);
			file->output = (file->output << 1) | buffer;

			file->length++;

			if (file->length % 8 == 0)
			{
				fwrite(&file->output, sizeof(uint8), 1, file->fp_out);
				file->length = 0;
				file->output = 0;
			}
		}

	}
}

void Send_Sign(Data* file, uint8 p_size)
{
	uint8 sign;

	for (int i = 0; i < p_size*p_size; i++)
	{
		sign = (file->PredErr[i] < 0) ? 1 : 0;

		file->output = file->output << 1 | sign;
		file->length++;

		if (file->length == 8)
		{
			fwrite(&file->output, sizeof(uint8), 1, file->fp_out);
			file->output = 0;
			file->length = 0;
		}
	}
}

void Send_PRED_INFO(Data* file, uint8 code_symbol, uint8 level)
{
	uint8	buffer = 0;
	uint8	bitLen = round(log(level, 2));				//ǥ����ȣ�� ������ ���� ��Ʈ�� ���� ����(�ݿø��ؾ���), ��������� ���� 6�̶�� log2�� 6�� �Ҽ��� ���´�.
	uint8	check_bit = pow(2, bitLen - 1);

	for (int i = 0; i < bitLen; i++)
	{
		buffer = code_symbol << i & check_bit ? 1 : 0;
		file->output = (file->output << 1) | buffer;
		file->length++;

		if (file->length % 8 == 0)
		{
			fwrite(&file->output, sizeof(uint8), 1, file->fp_out);
			file->length = 0;
			file->output = 0;
		}
	}
}

void Send_INTRA(Data* file, double* Quant_blk, uint8 PredMode, uint8 Best_DCT_Mode)
{
	Send_PRED_INFO(file, file->split_flag, 2);					// ���� ���� ���� 

	for (int i = 0; i < (file->split_flag ? 4 : 1); i++)
	{
		Send_PRED_INFO(file, file->split_flag ? file->PredMode_4x4[i] : PredMode, PRED_MODE);

		Send_PRED_INFO(file, file->split_flag ? file->DCT_Mode_4x4[i] : Best_DCT_Mode, 2);

		Send_Quantized_DCT_Coeff(file, file->split_flag ? file->Quant_coeff_4x4[i] : Quant_blk, file->split_flag ? PSIZE / 2 : PSIZE);
	}
}

/* ���� �̹��� ���� ���� */
void Write_Recon_Image(BUFFER* img, Data* file, uint32 height, uint32 width)
{
	Node* head = (Node*)malloc(sizeof(Node));

	head->left = NULL;
	head->right = NULL;

	uint32 init_position = (img->nWidth + 1) + 1;	//nWidth : padding buffer(recon)�� ���α���, //init_position: ���� �о���� �ʱ� ��ġ

	for (int i = 0; i < height; i++)
	{
		int cur_position = init_position;

		for (int j = 0; j < width; j++)
		{
			fwrite(&img->recon_padding[cur_position], sizeof(uint8), 1, file->fp_ori_recon);

			cur_position++;
		}
		init_position += img->nWidth + 1;
	}
}

uint8 set_bits(uint32 code, uint8 prefix_num, uint8 count)
{
	uint32 check_bit = 1;
	uint8  result = 0;
	uint8  codeword_len = prefix_num * 2 + 1;
	uint8  shift = codeword_len - count - 1;	// shift�� ��Ʈ���꿡 ����Ǵ� �ʱⰪ�� discriminator��ġ������ �̵���

	if (count < prefix_num)
	{
		return result = 0;
	}

	else if (count >= prefix_num)
	{
		return result = (check_bit << shift) & code ? 1 : 0;
	}
}