#include "Buffer.h"
#include "Node.h"
#include "input.h"


#if TEST_HUFFMAN == 1

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

void Huffman_decoding(Data* file, uint8 p_size)
{
	uint8	n_symbol = file->n_symbol;
	uint8	start_index = 0;

	int*	codeword = (int*)calloc(sizeof(int), p_size*p_size);

	Node*	head = NULL;											// �Ӹ���� ����, �ڽĳ��� ��� NULL�� �ʱ�ȭ
	Node**	tail = (Node**)malloc(sizeof(Node*)*n_symbol);

	initialize(file, tail, n_symbol);

	head = create_HuffmanTree(tail, n_symbol);						// �Ӹ� ��带 ������ Ʈ���� ����

//	fread(&file->output, sizeof(uint8), 1, file->fp_in);			// ����Լ� �ٱ����� fread

	while (file->index < p_size*p_size)
	{
		Tree_traversal(file, head);					// ���� ��ȸ�Ͽ� ���� ���������� Ž��
	}

	free_Tree(head);												// ���� ��ȸ������� �޸� ���� ����
	free(codeword);
}

void Tree_traversal(Data* file, Node* node)
{
	//����尡 �ƴ϶�� ��Ʈ������ ���� �����´�.
	if (!isEndNode(node))
	{
		file->direction = file->output << (file->length % 8) & (uint8)128 ? 1 : 0;
	}

	//��Ʈ���� 0�̰� ���� �ڽĳ�尡 �����ϸ�
	if (!file->direction && node->left != NULL)
	{
		file->length++;

		if (file->length % 8 == 0)
		{
			fread(&file->output, sizeof(uint8), 1, file->fp_in);
			file->length = 0;
		}

		Tree_traversal(file, node->left);
	}

	//��Ʈ���� 1�̰� ������ �ڽĳ�尡 �����ϸ�
	if (file->direction && node->right != NULL)
	{
		file->length++;

		if (file->length % 8 == 0)
		{
			fread(&file->output, sizeof(uint8), 1, file->fp_in);
			file->length = 0;
		}
		Tree_traversal(file, node->right);
	}

	// ������� ��� ����ȭ ��ȯ��� ���ۿ� ������� �ɺ����� ����
	if (isEndNode(node))
	{
		file->Recon_DCT_blk[file->index++] *= node->symbol;
		file->direction = 0;
	}
}

bool isEndNode(Node* node)
{
	bool result = (node->left == NULL) && (node->right == NULL);

	return result;
}

void Tree_memory_alloc1D(Data* file)
{
	file->symbol = (uint8*)calloc(sizeof(uint8), file->n_symbol);
	file->freq = (uint8*)calloc(sizeof(uint8), file->n_symbol);
}

void Tree_memory_free1D(Data* file)
{
	free(file->symbol);
	free(file->freq);
}

void free_Tree(Node* node)
{
	if (node->left != NULL)
	{
		free_Tree(node->left);
		node->left = NULL;
	}

	if (node->right != NULL)
	{
		free_Tree(node->right);
		node->right = NULL;
	}

	if (isEndNode(node))// ��������� ���, �ش� �ɺ����� ����ϰ� ��ȣ�����
	{
		free(node);
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