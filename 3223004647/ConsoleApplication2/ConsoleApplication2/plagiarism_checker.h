#ifndef PLAGIARISM_CHECKER_H
#define PLAGIARISM_CHECKER_H

#include <wchar.h>

// 计算最长公共子序列长度（宽字符版）
int lcs_length(const wchar_t* str1, const wchar_t* str2);

// 读取UTF-8编码的文件内容到宽字符串
wchar_t* read_utf8_file(const char* file_path);

// 写入结果到文件
int write_result(const char* file_path, double similarity);

#endif // PLAGIARISM_CHECKER_H
