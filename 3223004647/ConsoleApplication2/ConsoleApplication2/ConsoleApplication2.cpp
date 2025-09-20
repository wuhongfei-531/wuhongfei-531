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

int main() {
	return 0;
}