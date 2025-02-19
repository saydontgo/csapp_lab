#include "cachelab.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<limits.h>
#define max 100
int hit = 0, miss = 0, e = 0;
typedef struct
{
    int valid;//��Чλ
    int tag;//���λ
    int time_count;//����ʱ�����
}line;
typedef struct
{
    line* l;
}set;
typedef struct
{
    int S_count;
    set* Set;
    int E;
}cache;
void getinfo(int argc, char* argv[], int* s, int* e, int* b, char* filename)//��argv[]�ж����ݳ������˴��õ�ַ���ݣ����޸�main�����ж�Ӧ������ֵ
{
    for (int i = 1; i < argc; i += 2)//����argc[0]��"./test-csim", û�б�Ҫ�������Դ�1��ʼ                                     //i+=2����Ϊÿ������ǰ�涼�ж�Ӧ����ʾ���������������һ�飬����h��v��䣬���⴦����
    {
        switch (argv[i][1])//��Ӧ��ʾ���ĸ�ʽ��"-X",�����Ƕ��ڶ����ַ�
        {
        case 's':
            *s = atoi(argv[i + 1]);
            break;
        case 'E':
            *e = atoi(argv[i + 1]);
            break;
        case 't':
            strcpy(filename, argv[i + 1]);//�õ��ļ���
            break;
        case'b':
            *b = atoi(argv[i + 1]);
            break;
        case'h'://����h��v���
        case'v':
            i--;
        default:break;
        }
    }
}
void initialize_cache(cache* cc, int S, int E)
{
    cc->S_count = pow(2, S);
    cc->E = E;
    cc->Set = (set*)malloc(cc->S_count * sizeof(set));
    for (int i = 0; i < cc->S_count; i++) {
        cc->Set[i].l = (line*)malloc(cc->E * sizeof(line));
        for (int j = 0; j < cc->E; j++)
        {
            cc->Set[i].l[j].valid = 0;
            cc->Set[i].l[j].tag = 0;
            cc->Set[i].l[j].time_count = 0;
        }
    }
}
void cal_hit_miss_evi(char* this_line, cache* c, int B, int S)
{
    int hang = c->E;//ÿ�������
    int address;
    char mode;
    int matched_set;
    int longest_time = 0;//��ʼ�����滻�е��±�
    int temp_tag;
    sscanf(this_line, " %c %x", &mode, &address);

    address >>= B;
    matched_set = address & (0x7fffffff >> (31 - S));//�����Ӧ����λ��
    temp_tag = address >> S;//�õ����λ
    for (int i = 0; i < hang; i++) {
        if (c->Set[matched_set].l[i].valid == 1 && temp_tag == c->Set[matched_set].l[i].tag)//����
        {
            if (mode == 'M')hit++;
            hit++;
            for (int j = 0; j < hang; j++)//����ʱ��
            {
                if (c->Set[matched_set].l[j].valid == 1) {
                    if (i == j) c->Set[matched_set].l[j].time_count = 0;
                    else c->Set[matched_set].l[j].time_count++;
                }
            }
            return;
        }
    }
    miss++;
    for (int i = 0; i < hang; i++)
    {
        if (c->Set[matched_set].l[i].valid == 0)//��������
        {
            c->Set[matched_set].l[i].valid = 1;
            c->Set[matched_set].l[i].tag = temp_tag;
            for (int j = 0; j < hang; j++)
                if (i != j && c->Set[matched_set].l[i].valid == 1)
                    c->Set[matched_set].l[j].time_count++;
            if (mode == 'M')hit++;
            return;
        }
    }
        e++;
        for (int i = 0; i < hang; i++)
            if (c->Set[matched_set].l[i].time_count > c->Set[matched_set].l[longest_time].time_count)
                longest_time = i;//���±��滻��
        c->Set[matched_set].l[longest_time].tag = temp_tag;//�����滻
        for (int j = 0; j < hang; j++)
            if (c->Set[matched_set].l[j].valid == 1)
                c->Set[matched_set].l[j].time_count++;
        c->Set[matched_set].l[longest_time].time_count = 0;
        if (mode == 'M')hit++;
}
int main(int argc, char* argv[])
{
    int S, E, B;
    
    char filename[30];
    cache cc;
    getinfo(argc, argv, &S, &E, &B, filename);
    initialize_cache(&cc, S, E);

    FILE* fp;
    fp = fopen(filename, "r");//���ļ�
    if (fp == NULL)return -1;//�ļ����쳣�򷵻�
    char temp_line[max];//ÿһ�еĻ�����
    while (fgets(temp_line, max, fp) != NULL)//fgets�������ļ���ÿһ��
    {
        if (temp_line[0] == 'I')continue;//������I����
        cal_hit_miss_evi(temp_line, &cc, B, S);//���㱾�е�hit,miss,evi
    }    fclose(fp);

    for (int i = 0; i < cc.S_count; i++)//�ͷ��ڴ�
    {
        free(cc.Set[i].l);
    }
    free(cc.Set);
    
    printSummary(hit, miss, e);//��Ҫ����ú���
    return 0;
}