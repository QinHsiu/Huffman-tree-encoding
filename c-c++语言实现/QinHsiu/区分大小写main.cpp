#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

struct huffmantree
{
char ch;
int weight;
int parent;
int lchild;
int rchild;
char *code;
};

struct char_weight
{
char ch;
int weight;
};

struct size_cw
{
int size;
struct char_weight *cw;
};

struct size_cw *create_scw()
 {
	FILE *fp;
	char ch;
	int i;
	int c;
	int cnt;
	int tmp[256];
	struct size_cw *scw= (struct size_cw *)malloc(sizeof(struct size_cw));
	fp = fopen("1.txt", "r+");
	if (fp == NULL){
	return NULL;}
	for (i = 0; i < 256; i++)
	tmp[i] = 0;
	while ((c = fgetc(fp)) != EOF)
	tmp[c]++;
	scw->size = 0;
	for (i = 0; i < 256; i++)
	if (tmp[i] != 0)
	(scw->size)++;
	scw->cw = (struct char_weight *)malloc(sizeof(struct char_weight) *scw->size);
	cnt = 0;
	for (i = 0; i < 256; i++)
	if (tmp[i] != 0)
	{
	scw->cw[cnt].ch = i;
	scw->cw[cnt].weight = tmp[i];
	cnt++;
	}
	fclose(fp);
	return scw;
}

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
}

void min2_wieght(struct huffmantree *ht, int n, int *s1, int *s2)
{
	*s1 = min_weight(ht, n);
	ht[*s1].parent = 1;
	*s2 = min_weight(ht, n);
	ht[*s1].parent = 0;
}

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
	ht = (struct huffmantree *)malloc(sizeof(struct huffmantree) *(node + 1));
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
}

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
}

void print_cw(struct size_cw *scw)
{
	int i;
	for (i = 0; i < scw->size; i++)
		printf("%c\t %d\n", scw->cw[i].ch, scw->cw[i].weight);
}

void print_ht(struct huffmantree *ht, struct size_cw *scw)
 {
	int i;
	for (i = 1; i <= 2*scw->size-1; i++)
		printf("%-5d%-5d%-5d\n", ht[i].parent, ht[i].lchild, ht[i].rchild);
}

void print_hc(struct huffmantree *ht, struct size_cw *scw)
 {
	int i;
	for (i = 1; i <= scw->size; i++)
	printf("%c\t%s\n", ht[i].ch, ht[i].code);
 }

int code_to_file(struct huffmantree *ht, struct size_cw *scw)
{
	FILE *fpr;
	FILE *fpw;
	char s[500];
	int c;
	int i;
	printf("请输入保存编码的文件名\n");
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
}

void from_file_decode(struct huffmantree *ht, struct size_cw *scw)
{
	FILE *fp, *fpout;
    char s[1000], filename[500];
	int i, c;
	int leave = scw->size;
	printf("请输入要进行译码的文件名:\n");
	gets(s);
	printf("请输入你要保存译码的文件名:\n");
	gets(filename);
	printf("\n");
	printf("译码结果如下:\n");
	printf(" \n");
	fp = fopen(s, "r");
	fpout = fopen(filename, "wt");
	while (1)
	{
		i = 2 * leave - 1;
		while (ht[i].lchild)
		{
			if ((c = fgetc(fp)) == EOF)
			return;
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
     FILE *fp3,*fp4;
    char ch;
    fp3=fopen("1.txt","r+");
    if(fp3==NULL){
        printf("open file failure");
        return -1;
    }
    fp4=fopen("3.txt","w");
    if(fp4==NULL){
    printf("open file failure");
        return -1;
    }
    while(1)
    {
        ch=fgetc(fp3);
        if(feof(fp3))break;
        if(isupper(ch))
        {
            ch=tolower(ch);
        }
        fputc(ch,fp4);
    }
    fclose(fp3);
    fclose(fp4);
    system("del 1.txt>nul");
    system("rename 3.txt 1.txt>nul");
    struct size_cw *scw;;
	struct huffmantree *ht;
	scw = create_scw();
	if (scw == NULL)
	return 1;
	printf("\n");
	printf("字符频度表:\n");
	print_cw(scw);
	printf("\n");
	ht = create_huffmantree(scw);
	printf("哈夫曼树:\n");
	print_ht(ht, scw);
	printf("\n");
	printf("编码表:\n");
	code_huffmantree(ht, scw);
	print_hc(ht, scw);
	printf("\n");
	code_to_file(ht, scw);
	char s[1000];
	while (1)
	{
		printf("输入'q'退出，输入其他进行解码\n");
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
