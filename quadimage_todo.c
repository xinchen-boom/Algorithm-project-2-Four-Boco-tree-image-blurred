#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

//��Ŀ: �Ĳ���ģ��
//��ͼ�����ģ�������ַ���:
//1. ��ֵģ��
//2. ��˹ģ��

//���ص�
typedef struct color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
}color;
color** colors;
int width, height, max_value; //���ߡ����ֵ

//�Ĳ���
struct tree_four {
	struct tree_four* left_up, * left_down, * right_up, * right_down;
	int left, right, up, down; //��ǰ�Ĳ���������ĸ��ǵ�����
};
typedef struct tree_four Tree;

//�ж�ͼ����ĳһ�������ɫ�Ƿ�����ı�׼
double standard = 200;

// ��˹ģ�� -- Ȩ��
double quanzhong[3][3] = { {0.0947416, 0.118318, 0.0947416},
						   {0.118318, 0.0707355, 0.118318},
						   {0.0947416, 0.118318, 0.0947416} };
// �뾶
int radias = 1;

// ���÷����ж��Ƿ���Ҫ���������Ĳ���
// 1. ��Ҫ���������Ĳ��� --> ����true
// 2. ����Ҫ���������Ĳ��� --> ����false
bool variance(Tree* tmp) {
	double average_r = 0, average_g = 0, average_b = 0, variance_r = 0, variance_g = 0, variance_b = 0, n = (tmp->right - tmp->left + 1) * (tmp->down - tmp->up + 1);
	// ����ƽ��ֵ
	for (int i = tmp->up; i <= tmp->down; i++)
		for (int j = tmp->left; j <= tmp->right; j++) average_r += colors[i][j].r, average_g += colors[i][j].g, average_b += colors[i][j].b;
	average_r /= n, average_g /= n, average_b /= n;
	// ���㷽��
	for (int i = tmp->up; i <= tmp->down; i++)
		for (int j = tmp->left; j <= tmp->right; j++) variance_r += (colors[i][j].r - average_r) * (colors[i][j].r - average_r), variance_g += (colors[i][j].g - average_g) * (colors[i][j].g - average_g), variance_b += (colors[i][j].b - average_b) * (colors[i][j].b - average_b);
	variance_r /= n, variance_g /= n, variance_b /= n;
	return variance_r > standard || variance_g > standard || variance_b > standard;
}

// �����Ĳ���
void build(Tree* tree) {
	// �޷������Ĳ��������: 1.�޷������ֲ�Ϊ�Ĳ���; 2.��������Ҫ�� --> ��: ��������ͼ������
	if (tree->left == tree->right || tree->up == tree->down || !variance(tree)) {
		tree->left_down = tree->left_up = tree->right_down = tree->right_up = NULL;
		return;
	}
	int mid_heng = (tree->left + tree->right) >> 1, mid_zong = (tree->up + tree->down) >> 1;
	// ������������
	tree->left_up = (Tree*)malloc(sizeof(Tree));
	tree->left_up->left = tree->left, tree->left_up->right = mid_heng, tree->left_up->up = tree->up, tree->left_up->down = mid_zong;
	build(tree->left_up);

	// ������������
	tree->right_up = (Tree*)malloc(sizeof(Tree));
	tree->right_up->left = mid_heng + 1, tree->right_up->right = tree->right, tree->right_up->up = tree->up, tree->right_up->down = mid_zong;
	build(tree->right_up);

	// ������������
	tree->left_down = (Tree*)malloc(sizeof(Tree));
	tree->left_down->left = tree->left, tree->left_down->right = mid_heng, tree->left_down->down = tree->down, tree->left_down->up = mid_zong + 1;
	build(tree->left_down);

	// ������������
	tree->right_down = (Tree*)malloc(sizeof(Tree));
	tree->right_down->left = mid_heng + 1, tree->right_down->right = tree->right, tree->right_down->up = mid_zong + 1, tree->right_down->down = tree->down;
	build(tree->right_down);
}

// ģ������
// 1. ��ֵģ��
void blur(Tree* tree) {
	if (tree->left_up == NULL && tree->left_down == NULL && tree->right_up == NULL && tree->right_down == NULL) {
		double average_r = 0, average_g = 0, average_b = 0;
		int n = (tree->right - tree->left + 1) * (tree->down - tree->up + 1);

		//����ƽ��ֵ
		for (int i = tree->up; i <= tree->down; i++)
			for (int j = tree->left; j <= tree->right; j++) average_r += colors[i][j].r, average_g += colors[i][j].g, average_b += colors[i][j].b;
		average_r /= n, average_g /= n, average_b /= n;
		for (int i = tree->up; i <= tree->down; i++)
			for (int j = tree->left; j <= tree->right; j++) colors[i][j].r = average_r, colors[i][j].g = average_g, colors[i][j].b = average_b;
		return;
	}
	blur(tree->left_up); blur(tree->left_down); blur(tree->right_up); blur(tree->right_down);
}

// 2. ��˹ģ��
void Gauss(Tree* tree) {
	for (int i = 1; i < width - 1; i++)
		for (int j = 1; j < width - 1; j++){
			double gave = 0, rave = 0, bave = 0;
			for (int k = -1; k <= radias; k++)
				for (int l = -1; l <= radias; l++)
					rave += (double)(colors[j + k][i + l].r) * quanzhong[k + 1][l + 1], gave += (double)(colors[j + k][i + l].g) * quanzhong[k + 1][l + 1], bave += (double)(colors[j + k][i + l].b) * quanzhong[k + 1][l + 1];
			colors[j][i].r = rave, colors[j][i].g = rave, colors[j][i].b = rave;
		}
}

// To get ppm image from jpeg file, please visit https://convertio.co/jpg-ppm/
void readImage(int p, char* inFile, char* outFile) {
	FILE* f = fopen(inFile, "rb"); //�Զ�����ֻ���ķ�ʽ���ļ�
	if (f == NULL) {
		printf("�ļ���ʧ��");
		return;
	}

	char u[3]; //ռλ��
	fscanf(f, "%s%d%d%d%c", u, &width, &height, &max_value, &u[0]); //ͼ�������Ϣ: ppm�ı����ʽ ͼ��Ŀ��(����) ͼ��ĸ߶�(����) �������ֵ

	int i;
	colors = (color**)malloc(width * sizeof(color*));
	for (i = 0; i < height; i++) colors[i] = (color*)malloc(width * sizeof(color)); //��������

	for (i = 0; i < height; i++) fread(colors[i], sizeof(color), width, f); //��ȡͼ���RGBֵ
	fclose(f);
}

void printImage(char* fileName, int height, int width, color** a) {
	FILE* f = fopen(fileName, "wb"); //�Զ�����ֻд�ķ�ʽ���ļ�
	if (f == NULL) {
		printf("�ļ���ʧ��");
		return;
	}

	fprintf(f, "P6\n");
	fprintf(f, "%d %d\n", width, height);
	fprintf(f, "255\n");

	int i;
	for (i = 0; i < height; i++) fwrite(a[i], sizeof(color), width, f);

	fclose(f);
}
int main() {
	int tolerance = 4;
	char* inFile = "D:\\ComputerScience\\ACM\\Experiment\\lab03_�Ĳ���ͼ��ģ��\\a.ppm";
	char* outFile = "D:\\ComputerScience\\ACM\\Experiment\\lab03_�Ĳ���ͼ��ģ��\\out.ppm";
	Tree* t = (Tree*)malloc(sizeof(Tree));
	if (t == NULL) {
		printf("Memory is full\n");
		return 0;
	}
	readImage(tolerance, inFile, outFile); //���ļ�
	t->left_down = t->left_up = t->right_down = t->right_up = NULL;
	t->left = 0, t->right = width - 1, t->up = 0, t->down = height - 1;
	build(t); //�����Ĳ���
	//blur(t); //��ͼ�����ģ������
	Gauss(t); Gauss(t); Gauss(t);
	printImage(outFile, height, width, colors); //����ļ�

	return 0;
}