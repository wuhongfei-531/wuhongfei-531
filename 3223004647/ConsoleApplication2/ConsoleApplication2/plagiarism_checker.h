
#ifndef PLAGIARISM_CHECKER_H
#define PLAGIARISM_CHECKER_H

#include <stdio.h>  // ����FILE�����Ͷ���

// ��������������г���
int lcs_length(const char* str1, const char* str2);

// ��ȡ�ļ����ݵ��ַ�����ʹ��fopen_s��
char* read_file(const char* file_path);

// д�������ļ���ʹ��fopen_s��
int write_result(const char* file_path, double similarity);

#endif  // PLAGIARISM_CHECKER_H