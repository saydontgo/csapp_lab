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
    int valid;//有效位
    int tag;//标记位
    int time_count;//访问时间计算
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
void getinfo(int argc, char* argv[], int* s, int* e, int* b, char* filename)//从argv[]中读数据出来，此处用地址传递，以修改main函数中对应变量的值
{
    for (int i = 1; i < argc; i += 2)//由于argc[0]是"./test-csim", 没有必要读，所以从1开始                                     //i+=2是因为每个参数前面都有对应的提示符，因此两个两个一组，对于h和v语句，特殊处理即可
    {
        switch (argv[i][1])//对应提示符的格式是"-X",所以是读第二个字符
        {
        case 's':
            *s = atoi(argv[i + 1]);
            break;
        case 'E':
            *e = atoi(argv[i + 1]);
            break;
        case 't':
            strcpy(filename, argv[i + 1]);//拿到文件名
            break;
        case'b':
            *b = atoi(argv[i + 1]);
            break;
        case'h'://处理h和v语句
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
    int hang = c->E;//每组的行数
    int address;
    char mode;
    int matched_set;
    int longest_time = 0;//初始化被替换行的下标
    int temp_tag;
    sscanf(this_line, " %c %x", &mode, &address);

    address >>= B;
    matched_set = address & (0x7fffffff >> (31 - S));//计算对应的组位置
    temp_tag = address >> S;//得到标记位
    for (int i = 0; i < hang; i++) {
        if (c->Set[matched_set].l[i].valid == 1 && temp_tag == c->Set[matched_set].l[i].tag)//命中
        {
            if (mode == 'M')hit++;
            hit++;
            for (int j = 0; j < hang; j++)//更新时间
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
        if (c->Set[matched_set].l[i].valid == 0)//遇到空行
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
                longest_time = i;//更新被替换行
        c->Set[matched_set].l[longest_time].tag = temp_tag;//进行替换
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
    fp = fopen(filename, "r");//读文件
    if (fp == NULL)return -1;//文件打开异常则返回
    char temp_line[max];//每一行的缓冲区
    while (fgets(temp_line, max, fp) != NULL)//fgets函数读文件的每一行
    {
        if (temp_line[0] == 'I')continue;//跳过“I”行
        cal_hit_miss_evi(temp_line, &cc, B, S);//计算本行的hit,miss,evi
    }    fclose(fp);

    for (int i = 0; i < cc.S_count; i++)//释放内存
    {
        free(cc.Set[i].l);
    }
    free(cc.Set);
    
    printSummary(hit, miss, e);//按要求调用函数
    return 0;
}