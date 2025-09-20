#include "plagiarism_checker.h"  // 引用自定义头文件
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

// 计算最长公共子序列长度
int lcs_length(const char* str1, const char* str2) {
	int m = strlen(str1);
	int n = strlen(str2);

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


// 读取文件内容到字符串（使用fopen_s）
char* read_file(const char* file_path) {
	FILE* file;
	errno_t err;

	// 使用fopen_s打开文件，返回错误码
	if ((err = fopen_s(&file, file_path, "r")) != 0) {
		fprintf(stderr, "无法打开文件: %s (错误码: %d)\n", file_path, err);
		return NULL;
	}

	// 获取文件大小
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// 分配内存（+1用于存储字符串结束符）
	char* content = (char*)malloc(file_size + 1);
	if (content == NULL) {
		fprintf(stderr, "内存分配失败\n");
		fclose(file);
		return NULL;
	}

	// 读取文件内容
	size_t read_size = fread(content, 1, file_size, file);
	content[read_size] = '\0';  // 添加字符串结束符

	fclose(file);
	return content;
}


// 写入结果到文件（使用fopen_s）
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

// main函数单独保留在源文件中（也可根据需要移到单独的main.c）
int main(int argc, char* argv[]) {
	// 检查命令行参数
	if (argc != 4) {
		fprintf(stderr, "用法: %s 原文文件路径 抄袭文件路径 结果输出路径\n", argv[0]);
		return 1;
	}

	// 读取文件内容
	char* original_text = read_file(argv[1]);
	if (original_text == NULL) return 1;

	char* plagiarized_text = read_file(argv[2]);
	if (plagiarized_text == NULL) {
		free(original_text);
		return 1;
	}

	// 计算相似度
	int original_len = strlen(original_text);
	int lcs_len = lcs_length(original_text, plagiarized_text);
	double similarity = (original_len > 0) ? (double)lcs_len / original_len : 0.0;

	// 输出结果
	if (write_result(argv[3], similarity)) {
		printf("查重完成，相似度: %.2f%%\n", similarity * 100);
		printf("结果已保存至: %s\n", argv[3]);
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
