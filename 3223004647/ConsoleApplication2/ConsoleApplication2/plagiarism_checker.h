#ifndef PLAGIARISM_CHECKER_H
#define PLAGIARISM_CHECKER_H

#include <wchar.h>

// ��������������г��ȣ����ַ��棩
int lcs_length(const wchar_t* str1, const wchar_t* str2);

// ��ȡUTF-8������ļ����ݵ����ַ���
wchar_t* read_utf8_file(const char* file_path);

// д�������ļ�
int write_result(const char* file_path, double similarity);

#endif // PLAGIARISM_CHECKER_H
