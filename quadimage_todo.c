#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

//项目: 四叉树模糊
//对图像进行模糊的两种方法:
//1. 均值模糊
//2. 高斯模糊

//像素点
typedef struct color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
}color;
color** colors;
int width, height, max_value; //宽、高、最大值

//四叉树
struct tree_four {
	struct tree_four* left_up, * left_down, * right_up, * right_down;
	int left, right, up, down; //当前四叉树区域的四个角的坐标
};
typedef struct tree_four Tree;

//判断图像中某一区域的颜色是否相近的标准
double standard = 200;

// 高斯模糊 -- 权重
double quanzhong[3][3] = { {0.0947416, 0.118318, 0.0947416},
						   {0.118318, 0.0707355, 0.118318},
						   {0.0947416, 0.118318, 0.0947416} };
// 半径
int radias = 1;

// 利用方差判断是否需要继续建立四叉树
// 1. 需要继续建立四叉树 --> 返回true
// 2. 不需要继续建立四叉树 --> 返回false
bool variance(Tree* tmp) {
	double average_r = 0, average_g = 0, average_b = 0, variance_r = 0, variance_g = 0, variance_b = 0, n = (tmp->right - tmp->left + 1) * (tmp->down - tmp->up + 1);
	// 计算平均值
	for (int i = tmp->up; i <= tmp->down; i++)
		for (int j = tmp->left; j <= tmp->right; j++) average_r += colors[i][j].r, average_g += colors[i][j].g, average_b += colors[i][j].b;
	average_r /= n, average_g /= n, average_b /= n;
	// 计算方差
	for (int i = tmp->up; i <= tmp->down; i++)
		for (int j = tmp->left; j <= tmp->right; j++) variance_r += (colors[i][j].r - average_r) * (colors[i][j].r - average_r), variance_g += (colors[i][j].g - average_g) * (colors[i][j].g - average_g), variance_b += (colors[i][j].b - average_b) * (colors[i][j].b - average_b);
	variance_r /= n, variance_g /= n, variance_b /= n;
	return variance_r > standard || variance_g > standard || variance_b > standard;
}

// 建立四叉树
void build(Tree* tree) {
	// 无法建立四叉树的情况: 1.无法继续分叉为四叉树; 2.方差满足要求 --> 即: 该区域里图像相似
	if (tree->left == tree->right || tree->up == tree->down || !variance(tree)) {
		tree->left_down = tree->left_up = tree->right_down = tree->right_up = NULL;
		return;
	}
	int mid_heng = (tree->left + tree->right) >> 1, mid_zong = (tree->up + tree->down) >> 1;
	// 建立左上子树
	tree->left_up = (Tree*)malloc(sizeof(Tree));
	tree->left_up->left = tree->left, tree->left_up->right = mid_heng, tree->left_up->up = tree->up, tree->left_up->down = mid_zong;
	build(tree->left_up);

	// 建立右上子树
	tree->right_up = (Tree*)malloc(sizeof(Tree));
	tree->right_up->left = mid_heng + 1, tree->right_up->right = tree->right, tree->right_up->up = tree->up, tree->right_up->down = mid_zong;
	build(tree->right_up);

	// 建立左下子树
	tree->left_down = (Tree*)malloc(sizeof(Tree));
	tree->left_down->left = tree->left, tree->left_down->right = mid_heng, tree->left_down->down = tree->down, tree->left_down->up = mid_zong + 1;
	build(tree->left_down);

	// 建立右下子树
	tree->right_down = (Tree*)malloc(sizeof(Tree));
	tree->right_down->left = mid_heng + 1, tree->right_down->right = tree->right, tree->right_down->up = mid_zong + 1, tree->right_down->down = tree->down;
	build(tree->right_down);
}

// 模糊处理
// 1. 均值模糊
void blur(Tree* tree) {
	if (tree->left_up == NULL && tree->left_down == NULL && tree->right_up == NULL && tree->right_down == NULL) {
		double average_r = 0, average_g = 0, average_b = 0;
		int n = (tree->right - tree->left + 1) * (tree->down - tree->up + 1);

		//计算平均值
		for (int i = tree->up; i <= tree->down; i++)
			for (int j = tree->left; j <= tree->right; j++) average_r += colors[i][j].r, average_g += colors[i][j].g, average_b += colors[i][j].b;
		average_r /= n, average_g /= n, average_b /= n;
		for (int i = tree->up; i <= tree->down; i++)
			for (int j = tree->left; j <= tree->right; j++) colors[i][j].r = average_r, colors[i][j].g = average_g, colors[i][j].b = average_b;
		return;
	}
	blur(tree->left_up); blur(tree->left_down); blur(tree->right_up); blur(tree->right_down);
}

// 2. 高斯模糊
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
	FILE* f = fopen(inFile, "rb"); //以二进制只读的方式打开文件
	if (f == NULL) {
		printf("文件打开失败");
		return;
	}

	char u[3]; //占位符
	fscanf(f, "%s%d%d%d%c", u, &width, &height, &max_value, &u[0]); //图像相关信息: ppm的编码格式 图像的宽度(几列) 图像的高度(几行) 最大像素值

	int i;
	colors = (color**)malloc(width * sizeof(color*));
	for (i = 0; i < height; i++) colors[i] = (color*)malloc(width * sizeof(color)); //开辟数组

	for (i = 0; i < height; i++) fread(colors[i], sizeof(color), width, f); //读取图像的RGB值
	fclose(f);
}

void printImage(char* fileName, int height, int width, color** a) {
	FILE* f = fopen(fileName, "wb"); //以二进制只写的方式打开文件
	if (f == NULL) {
		printf("文件打开失败");
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
	char* inFile = "D:\\ComputerScience\\ACM\\Experiment\\lab03_四叉树图像模糊\\a.ppm";
	char* outFile = "D:\\ComputerScience\\ACM\\Experiment\\lab03_四叉树图像模糊\\out.ppm";
	Tree* t = (Tree*)malloc(sizeof(Tree));
	if (t == NULL) {
		printf("Memory is full\n");
		return 0;
	}
	readImage(tolerance, inFile, outFile); //读文件
	t->left_down = t->left_up = t->right_down = t->right_up = NULL;
	t->left = 0, t->right = width - 1, t->up = 0, t->down = height - 1;
	build(t); //建立四叉树
	//blur(t); //对图像进行模糊处理
	Gauss(t); Gauss(t); Gauss(t);
	printImage(outFile, height, width, colors); //输出文件

	return 0;
}