#include "Buffer.h"
#include "Node.h"


void initialize(Data* file, Node** tail, int n_symbol)
{
	for (int i = 0; i < n_symbol; i++)
	{
		Node* node = (Node*)malloc(sizeof(Node));

		node->freq = file->freq[i];				//�߻�Ƚ��
		node->symbol = file->symbol[i];				//�ɺ���

		node->left = NULL;
		node->right = NULL;

		tail[i] = node;
	}
}
#if TEST_HUFFMAN == 1
void write_AbsValue(Data* file, int codeword[], int length)
{
	int buffer = 0;

	for (int i = 0; i < length; i++)
	{
		buffer = codeword[i];
		file->output = file->output << 1 | buffer;
		file->length++;

		if (file->length == 8)
		{
			fwrite(&file->output, sizeof(uint8), 1, file->fp_out);
			file->output = 0;
			file->length = 0;
		}
	}
}

Node* create_HuffmanTree(Node** nodeArr, uint8& n_symbol)
{
	Node* result;

	n_symbol--;
	
	/* ��� ���� */
	QuickSort(nodeArr, 0, n_symbol);								// ��带 �ɺ����� �߻�Ȯ���� ���� ������������ ����

	Node** newLayer;												// ����Ʈ������ ���� ������� ��� ��

	Node* newNode = (Node*)malloc(sizeof(Node));					// ������ ��� �޸� �Ҵ�


	newNode->left = nodeArr[0];
	newNode->right = nodeArr[1];

	newNode->freq = nodeArr[0]->freq + nodeArr[1]->freq;			// ������ ����� �� ��
	newNode->symbol = 0;

	/* ����Ʈ������ ���� ������� ��� �� */
	newLayer = createNextFloor(newNode, nodeArr, n_symbol);
	result = newNode;

	if (n_symbol > 1)
	{
		/* ��� ��ġ�� */
		result = create_HuffmanTree(newLayer, n_symbol);
	}

	return result;
}

Node** createNextFloor(Node* newNode, Node** nodeArr, uint8 arrLen)
{
	Node** newNodeArr = (Node**)malloc(sizeof(Node*) * arrLen);

	newNodeArr[0] = newNode;

	for (int i = 1; i < arrLen; i++)
	{
		newNodeArr[i] = nodeArr[i + 1];
	}
	return newNodeArr;
}


void Tree_traversal(Node* node, Data* file, uint8 symbol, int codeword[], int length)
{
	/* ���� ����� �ڽĳ�� Ž�� */

	//���� �ڽĳ�尡 �����ϸ�
	if (node->left != NULL)
	{
		codeword[length] = 0;
		Tree_traversal(node->left, file, symbol, codeword, length + 1);
	}

	//������ �ڽĳ�尡 �����ϸ�
	if (node->right != NULL)
	{
		codeword[length] = 1;
		Tree_traversal(node->right, file, symbol, codeword, length + 1);
	}

	// ���� ����� �ڽĳ�尡 NULL�� ��� ��������̹Ƿ�, �ش� �ɺ����� ����ϰ� ��ȣ�����
	else
	{
		if (symbol == node->symbol)
		{
			write_AbsValue(file, codeword, length);
		}
	}
}

bool isEndNode(Node* node)
{
	bool result = (node->left == NULL) && (node->right == NULL);

	return result;
}

void Extract_symbol(Data* file, uint8 p_size, uint8& index)
{
	uint8 absErr;

	for (int i = 0; i < p_size*p_size; i++)
	{
		absErr = file->input_sequence[i] = fabs(file->PredErr[i]);

		//ù��° �ɺ����� 0��° �ε����� ����
		if (index == 0)
		{
			file->symbol[0] = absErr;
			file->freq[0]++;

			index++;
		}

		//�ι�° ���ʹ� �տ� ���� ��
		else
		{
			for (int i = 0; i < index; i++)
			{
				if (file->symbol[i] == absErr)		//���� �Է����� ������ �ɺ����̶� ������ �ִ� �ɺ����̶� ���� ��� �񱳸� ����
				{
					file->freq[i]++;
					break;
				}

				if (i == index - 1)						//���������ұ��� ���ߴµ��� ���� ���� ������ ���� �ε����� �ɺ��� ����
				{
					file->symbol[index++] = absErr;
				}
			}
		}
	}
	
}

void QuickSort(Node** nodeArr, int left, int right)
{
	int L = left;
	int R = right;

	Node* pivot = nodeArr[(L + R) / 2];

	do
	{
		while (nodeArr[L]->freq < pivot->freq)
			L++;
		while (nodeArr[R]->freq > pivot->freq)
			R--;

		if (L <= R)
		{
			swap(&nodeArr[L], &nodeArr[R]);
			L++; R--;
		}

	} while (L <= R);

	//�Ǻ� ��������

	//���� ����: R�� ������ �Ǹ�, ���� �迭�� �������̹Ƿ� ����x
	if (left < R)
		QuickSort(nodeArr, left, R);

	//������ ����: L�� ���ҵ� �迭�� ������ ���� �����ϸ� ����x
	if (right > L)
		QuickSort(nodeArr, L, right);
}

void swap(Node **left, Node **right)
{
	Node* tmp = *left;
	*left = *right;
	*right = tmp;
}
#endif