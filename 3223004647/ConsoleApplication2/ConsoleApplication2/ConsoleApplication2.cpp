#include "plagiarism_checker.h"  // 引用自定义头文件
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

// 计算最长公共子序列长度
int lcs_length(const char* str1, const char* str2) {
	size_t m = strlen(str1);
	size_t n = strlen(str2);

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


int main() {
	return 0;
}