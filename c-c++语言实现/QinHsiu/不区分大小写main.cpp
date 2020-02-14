﻿#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

struct huffmantree
{
char ch;//字符
int weight;//权值
int parent;
int lchild;
int rchild;
char *code;//编码
};//哈夫曼树

struct char_weight
{
char ch;
int weight;
};//字符_权值

struct size_cw
{
int size;
struct char_weight *cw;
};//带大小的字符_权值

struct size_cw *create_scw()
 {
	FILE *fp;
	char ch;
	int i;
	int c;
	int cnt;
	int tmp[256];//哈希法记录字符频度
	struct size_cw *scw= (struct size_cw *)malloc(sizeof(struct size_cw));
	fp = fopen("1.txt", "r+");
	if (fp == NULL){
	return NULL;
	}
	for (i = 0; i < 256; i++)
	tmp[i] = 0;
	while ((c = fgetc(fp)) != EOF)
	tmp[c]++;//统计字符种类
	scw->size = 0;
	for (i = 0; i < 256; i++)
	if (tmp[i] != 0)
	(scw->size)++;
	scw->cw = (struct char_weight *)malloc(sizeof(struct char_weight) *scw->size);
	cnt = 0;//根据tmp创建字符_权值表
	for (i = 0; i < 256; i++)
	if (tmp[i] != 0)
	{
	scw->cw[cnt].ch = i;
	scw->cw[cnt].weight = tmp[i];
	cnt++;
	}
	fclose(fp);
	return scw;
}//创建带大小的字符_权值表

int min_weight(struct huffmantree *ht, int n)
{
	int i;
	int min;
	for (i = 1; ; i++)
	if (ht[i].parent == 0)
	{
		min = i;
		break;
	}
	for (i++; i <= n; i++)
	{
		if (ht[i].parent)
		continue;
		if (ht[min].weight > ht[i].weight)
		min = i;
	}
	return min;
}//在不存在父亲节点的点中，找出1个权值最小的节点

void min2_wieght(struct huffmantree *ht, int n, int *s1, int *s2)
{
	*s1 = min_weight(ht, n);
	ht[*s1].parent = 1;
	*s2 = min_weight(ht, n);
	ht[*s1].parent = 0;
}//在不存在父亲节点的点中，找出2个权值最小的节点

struct huffmantree *create_huffmantree(struct size_cw *scw)
{
	int i;
	int s1, s2;
	struct huffmantree *ht;
	struct char_weight *cw = scw->cw;
	int leave = scw->size;
	int node = 2 * leave - 1;
	if (leave <= 1)
	return NULL;
	ht = (struct huffmantree *)malloc(sizeof(struct huffmantree) *(node + 1));//0号节点不用
	for (i = 1; i <= leave; i++, cw++)
	{
		memset(ht+i, 0, sizeof(struct huffmantree));
		ht[i].ch = cw->ch;
		ht[i].weight = cw->weight;
	}
	for ( ; i <= node; i++)
	memset(ht+i, 0, sizeof(struct huffmantree));
	for (i = leave + 1; i <= node; i++)
	{
		min2_wieght(ht, i - 1, &s1, &s2);
		ht[s1].parent = i;
		ht[s2].parent = i;
		ht[i].lchild = s1;
		ht[i].rchild = s2;
		ht[i].weight = ht[s1].weight + ht[s2].weight;
	}
	return ht;
}//创建哈夫曼树

void code_huffmantree(struct huffmantree *ht, struct size_cw *scw)
{
	int c, f;
	int i, start;
	int leave = scw->size;
	char *code = (char *)malloc(sizeof(char) * scw->size);
	code[leave-1] = '\0';
	for (i = 1; i <= leave; i++)
	{
		start = leave - 1;
		for (c = i, f = ht[i].parent; f != 0; c = f, f = ht[f].parent)
		if (ht[f].lchild == c)
		{
			code[--start] = '0';
		}
		else
		{
			code[--start] = '1';
		}
		ht[i].code = (char *)malloc(sizeof(char) * (leave - start));
		strcpy(ht[i].code, code + start);
	}
	free(code);
}//对哈夫曼树编码

void print_cw(struct size_cw *scw)
{
	int i;
	for (i = 0; i < scw->size; i++)
		printf("%c\t %d\n", scw->cw[i].ch, scw->cw[i].weight);
}//打印字符频度表

void print_ht(struct huffmantree *ht, struct size_cw *scw)
 {
	int i;
	for (i = 1; i <= 2*scw->size-1; i++)
		printf("%-5d%-5d%-5d\n", ht[i].parent, ht[i].lchild, ht[i].rchild);
}//打印哈夫曼树

void print_hc(struct huffmantree *ht, struct size_cw *scw)
 {
	int i;
	for (i = 1; i <= scw->size; i++)
		printf("%c\t%s\n", ht[i].ch, ht[i].code);
 }//打印编码表

int code_to_file(struct huffmantree *ht, struct size_cw *scw)
{
	FILE *fpr;
	FILE *fpw;
	char s[500];
	int c;
	int i;
	printf("请输入保存编码的文件名：\n");
	gets(s);
	fpw = fopen(s, "w");
	if (fpw == NULL)
		return -1;
	fpr = fopen("1.txt", "r");
	if (fpr == NULL)
		return -1;
	while ((c = fgetc(fpr)) != EOF)
		for (i = 1; i <= scw->size; i++)
			if (ht[i].ch == c)
			{
				fprintf(fpw, "%s", ht[i].code);
				break;
			}
		fclose(fpw);
		fclose(fpr);
		return 0;
}//将编码保存到文件中

void from_file_decode(struct huffmantree *ht, struct size_cw *scw)
{
	FILE *fp, *fpout;
    	char s[1000], filename[500];
	int i, c;
	int leave = scw->size;
	printf("请输入需要译码的文件名：\n");
	gets(s);
	printf("请输入保存译码的文件名：\n");
	gets(filename);
	printf("\n");
	printf("译码结果如下：\n");
	printf(" \n");
	fp = fopen(s, "r");
	fpout = fopen(filename, "wt");
	while (1)
	{
		i = 2 * leave - 1;
		while (ht[i].lchild)
		{
			if ((c = fgetc(fp)) == EOF)
            return 0;
			if (c == '0')
			{
				i = ht[i].lchild;
			}
			else
			{
				i = ht[i].rchild;
			}
		}
		printf("%c", ht[i].ch);
		fputc(ht[i].ch, fpout);
	}
	printf("\n");
	fclose(fp);
	fclose(fpout);
}

int main()
{
    struct size_cw *scw;;
	struct huffmantree *ht;
	scw = create_scw();//统计字符频度及字符类型
	if (scw == NULL)
		return 1;
	printf("\n");
	printf("文件字符频度表：\n");
	print_cw(scw);//打印字符频度表
	printf("\n");
	ht = create_huffmantree(scw);//哈夫曼树
	printf("哈夫曼树：\n");
	print_ht(ht, scw);
	printf("\n");
	printf("编码表：\n");
	code_huffmantree(ht, scw);//将编码存进文件
	print_hc(ht, scw);//打印编码表
	printf("\n");
	code_to_file(ht, scw);
	char s[1000];
	while (1)
	{
		printf("输入'q'退出，输入其他解码！\n");
		gets(s);
		if (s[0] == 'q')
		break;
		from_file_decode(ht, scw);
		printf("\n");
		printf("\n");
		printf("\n");
	}
	printf("\n");
	return 0;
}
