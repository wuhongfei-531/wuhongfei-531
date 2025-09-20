#include "plagiarism_checker.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>

// 计算最长公共子序列长度（宽字符版）
int lcs_length(const wchar_t* str1, const wchar_t* str2) {
	int m = wcslen(str1);
	int n = wcslen(str2);

	// 创建动态规划表
	int** dp = (int**)malloc((m + 1) * sizeof(int*));
	for (int i = 0; i <= m; i++) {
		dp[i] = (int*)calloc(n + 1, sizeof(int));
	}

	// 填充DP表
	for (int i = 1; i <= m; i++) {
		for (int j = 1; j <= n; j++) {
			if (str1[i - 1] == str2[j - 1]) {
				dp[i][j] = dp[i - 1][j - 1] + 1;
			}
			else {
				dp[i][j] = (dp[i - 1][j] > dp[i][j - 1]) ? dp[i - 1][j] : dp[i][j - 1];
			}
		}
	}

	int result = dp[m][n];

	// 释放内存
	for (int i = 0; i <= m; i++) {
		free(dp[i]);
	}
	free(dp);

	return result;
}

// 读取UTF-8编码的文件内容到宽字符串
wchar_t* read_utf8_file(const char* file_path) {
	FILE* file;
	errno_t err;

	// 使用fopen_s打开文件
	if ((err = fopen_s(&file, file_path, "rb")) != 0) {
		fprintf(stderr, "无法打开文件: %s (错误码: %d)\n", file_path, err);
		return NULL;
	}

	// 获取文件大小
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// 读取文件内容到字节缓冲区
	char* buffer = (char*)malloc(file_size + 1);
	if (buffer == NULL) {
		fprintf(stderr, "内存分配失败\n");
		fclose(file);
		return NULL;
	}

	size_t read_size = fread(buffer, 1, file_size, file);
	buffer[read_size] = '\0';
	fclose(file);

	// 计算需要的宽字符数
	int wchar_count = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, NULL, 0);
	if (wchar_count == 0) {
		fprintf(stderr, "转换为宽字符失败\n");
		free(buffer);
		return NULL;
	}

	// 分配宽字符内存并转换
	wchar_t* wstr = (wchar_t*)malloc(wchar_count * sizeof(wchar_t));
	if (wstr == NULL) {
		fprintf(stderr, "宽字符内存分配失败\n");
		free(buffer);
		return NULL;
	}

	MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wstr, wchar_count);
	free(buffer);

	return wstr;
}

// 写入结果到文件
int write_result(const char* file_path, double similarity) {
	FILE* file;
	errno_t err;

	// 使用fopen_s打开文件
	if ((err = fopen_s(&file, file_path, "w")) != 0) {
		fprintf(stderr, "无法打开输出文件: %s (错误码: %d)\n", file_path, err);
		return 0;
	}

	// 保留两位小数写入
	fprintf(file, "%.2f", similarity);
	fclose(file);
	return 1;
}

int main(int argc, char* argv[]) {
	// 设置本地化，支持宽字符输出
	setlocale(LC_ALL, "zh-CN.UTF-8");
	_setmode(_fileno(stdout), _O_U16TEXT);

	// 检查命令行参数
	if (argc != 4) {
		fwprintf(stderr, L"用法: %s 原文文件路径 抄袭文件路径 结果输出路径\n", argv[0]);
		return 1;
	}

	// 读取文件内容（UTF-8编码）
	wchar_t* original_text = read_utf8_file(argv[1]);
	if (original_text == NULL) return 1;

	wchar_t* plagiarized_text = read_utf8_file(argv[2]);
	if (plagiarized_text == NULL) {
		free(original_text);
		return 1;
	}

	// 计算相似度
	int original_len = wcslen(original_text);
	int lcs_len = lcs_length(original_text, plagiarized_text);
	double similarity = (original_len > 0) ? (double)lcs_len / original_len : 0.0;

	// 输出结果
	if (write_result(argv[3], similarity)) {
		fwprintf(stdout, L"查重完成，相似度: %.2f%%\n", similarity * 100);
		wchar_t w_output_path[MAX_PATH];
		MultiByteToWideChar(CP_UTF8, 0, argv[3], -1, w_output_path, MAX_PATH);

		// 使用%ls格式符输出宽字符字符串
		wprintf(L"结果已保存至: %ls\n", w_output_path);
		wprintf(L"结果已保存至: %s\n", argv[3]);
	}
	else {
		free(original_text);
		free(plagiarized_text);
		return 1;
	}

	// 释放内存
	free(original_text);
	free(plagiarized_text);

	return 0;
}
