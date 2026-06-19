#ifndef DATA_H
#define DATA_H

// 数据结构定义 - GB2312 编码
typedef struct {
    int id;
    char name[64];
    double value;
} Record;

#endif // DATA_H