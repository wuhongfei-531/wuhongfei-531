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
	// 空字符串处理
	if (str1 == NULL || str2 == NULL) return 0;

	int m = wcslen(str1);
	int n = wcslen(str2);

	// 优化：如果任一字符串为空，直接返回0
	if (m == 0 || n == 0) return 0;

	// 创建动态规划表（使用一维数组优化空间）
	int* dp_prev = (int*)calloc(n + 1, sizeof(int));
	int* dp_curr = (int*)calloc(n + 1, sizeof(int));

	// 填充DP表
	for (int i = 1; i <= m; i++) {
		for (int j = 1; j <= n; j++) {
			if (str1[i - 1] == str2[j - 1]) {
				dp_curr[j] = dp_prev[j - 1] + 1;
			}
			else {
				dp_curr[j] = (dp_prev[j] > dp_curr[j - 1]) ? dp_prev[j] : dp_curr[j - 1];
			}
		}
		// 交换两行
		int* temp = dp_prev;
		dp_prev = dp_curr;
		dp_curr = temp;
		// 重置当前行
		memset(dp_curr, 0, (n + 1) * sizeof(int));
	}

	int result = dp_prev[n];

	// 释放内存
	free(dp_prev);
	free(dp_curr);

	return result;
}

// 读取UTF-8编码的文件内容到宽字符串
wchar_t* read_utf8_file(const char* file_path) {
	// 转换路径为宽字符，解决中文路径问题
	wchar_t wfile_path[MAX_PATH];
	if (MultiByteToWideChar(CP_ACP, 0, file_path, -1, wfile_path, MAX_PATH) == 0) {
		fprintf(stderr, "路径转换失败: %s\n", file_path);
		return NULL;
	}

	FILE* file;
	errno_t err = _wfopen_s(&file, wfile_path, L"rb");
	if (err != 0) {
		fprintf(stderr, "无法打开文件: %s (错误码: %d)\n", file_path, err);
		return NULL;
	}

	// 获取文件大小
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// 处理空文件
	if (file_size <= 0) {
		fclose(file);
		wchar_t* empty_str = (wchar_t*)malloc(sizeof(wchar_t));
		empty_str[0] = L'\0';
		return empty_str;
	}

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

	// 计算需要的宽字符数（修正转换参数）
	int wchar_count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, buffer, -1, NULL, 0);
	if (wchar_count == 0) {
		fprintf(stderr, "转换为宽字符失败，错误码: %d\n", GetLastError());
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

	// 执行转换
	if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, buffer, -1, wstr, wchar_count) == 0) {
		fprintf(stderr, "宽字符转换失败，错误码: %d\n", GetLastError());
		free(buffer);
		free(wstr);
		return NULL;
	}

	free(buffer);
	return wstr;
}

// 写入结果到文件
int write_result(const char* file_path, double similarity) {
	// 转换路径为宽字符
	wchar_t wfile_path[MAX_PATH];
	if (MultiByteToWideChar(CP_ACP, 0, file_path, -1, wfile_path, MAX_PATH) == 0) {
		fprintf(stderr, "路径转换失败: %s\n", file_path);
		return 0;
	}

	FILE* file;
	errno_t err = _wfopen_s(&file, wfile_path, L"w, ccs=UTF-8");
	if (err != 0) {
		fprintf(stderr, "无法打开输出文件: %s (错误码: %d)\n", file_path, err);
		return 0;
	}

	// 保留两位小数写入
	fwprintf(file, L"%.2f", similarity);
	fclose(file);
	return 1;
}

int main(int argc, char* argv[]) {
	// 设置本地化，支持宽字符输出
	setlocale(LC_ALL, "zh-CN.UTF-8");
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stderr), _O_U16TEXT);

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
	int plagiarized_len = wcslen(plagiarized_text);
	int max_len = (original_len > plagiarized_len) ? original_len : plagiarized_len;
	int lcs_len = lcs_length(original_text, plagiarized_text);
	double similarity = (max_len > 0) ? (double)lcs_len / max_len : 0.0;

	// 输出结果
	if (write_result(argv[3], similarity)) {
		fwprintf(stdout, L"查重完成，相似度: %.2f%%\n", similarity * 100);
		wchar_t w_output_path[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, argv[3], -1, w_output_path, MAX_PATH);
		wprintf(L"结果已保存至: %ls\n", w_output_path);
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
