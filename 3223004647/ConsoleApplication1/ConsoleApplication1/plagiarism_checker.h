
#ifndef PLAGIARISM_CHECKER_H
#define PLAGIARISM_CHECKER_H

#include <stdio.h>  // 包含FILE等类型定义

// 计算最长公共子序列长度
int lcs_length(const char* str1, const char* str2);

// 读取文件内容到字符串（使用fopen_s）
char* read_file(const char* file_path);

// 写入结果到文件（使用fopen_s）
int write_result(const char* file_path, double similarity);

#endif  // PLAGIARISM_CHECKER_H