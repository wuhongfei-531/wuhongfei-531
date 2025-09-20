#include "pch.h"
#include "CppUnitTest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// 引用主程序头文件（需确保路径正确）
#include "D:\rjgc\wuhongfei-531\3223004647\ConsoleApplication1\ConsoleApplication1\plagiarism_checker.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PlagiarismCheckerTests
{
	TEST_CLASS(PlagiarismTests)
	{
	private:
		// 测试辅助：清理临时文件
		void cleanupFile(const char* filePath) {
			remove(filePath);
		}

	public:
		// 测试1：LCS算法核心逻辑
		TEST_METHOD(TestLcsLength) {
			Logger::WriteMessage("\n===== 测试LCS算法 =====\n");

			// 完全匹配
			Assert::AreEqual(5, lcs_length("abcde", "abcde"), L"完全相同的字符串");
			// 部分匹配
			Assert::AreEqual(3, lcs_length("abcde", "ace"), L"部分匹配（非连续）");
			// 完全不匹配
			Assert::AreEqual(0, lcs_length("abc", "def"), L"完全不同的字符串");
			// 空字符串处理
			Assert::AreEqual(0, lcs_length("", "test"), L"第一个字符串为空");
			Assert::AreEqual(0, lcs_length("test", ""), L"第二个字符串为空");
		}

		// 测试2：文件读取功能（read_file）
		TEST_METHOD(TestReadFile) {
			Logger::WriteMessage("\n===== 测试文件读取 =====\n");
			const char* temp_file = "temp_read.txt";

			// 创建测试文件
			FILE* f;
			errno_t err = fopen_s(&f, temp_file, "w");
			if (err == 0) {
				fprintf(f, "这是测试内容123");
				fclose(f);
			}
			else {
				Assert::Fail(L"无法创建测试文件");
				return;
			}

			// 测试正常读取
			char* content = read_file(temp_file);
			Assert::IsNotNull(content, L"读取存在的文件");
			Assert::AreEqual(std::string("这是测试内容123"), std::string(content), L"文件内容正确");

			// 测试读取不存在的文件
			char* invalid_content = read_file("nonexistent.txt");
			Assert::IsNull(invalid_content, L"读取不存在的文件返回NULL");

			// 清理
			free(content);
			cleanupFile(temp_file);
		}

		// 测试3：文件写入功能（write_result）
		TEST_METHOD(TestWriteResult) {
			Logger::WriteMessage("\n===== 测试文件写入 =====\n");
			const char* temp_file = "temp_write.txt";

			// 测试正常写入
			int ret = write_result(temp_file, 0.67);
			Assert::AreEqual(1, ret, L"写入成功返回1");

			// 验证写入内容
			FILE* f;
			errno_t err = fopen_s(&f, temp_file, "r");
			if (err == 0) {
				char buf[20] = { 0 };
				fgets(buf, sizeof(buf), f);
				fclose(f);
				Assert::AreEqual(std::string("0.67"), std::string(buf), L"写入的相似度值正确");
			}
			else {
				Assert::Fail(L"无法验证写入文件");
			}

			// 清理
			cleanupFile(temp_file);
		}

		// 测试4：相似度计算逻辑
		TEST_METHOD(TestSimilarity) {
			Logger::WriteMessage("\n===== 测试相似度计算 =====\n");

			// 完全抄袭（100%）
			const char* orig1 = "hello";
			const char* plag1 = "hello";
			int lcs1 = lcs_length(orig1, plag1);
			double sim1 = (double)lcs1 / strlen(orig1);
			Assert::AreEqual(1.0, sim1, 0.001, L"完全抄袭的相似度");

			// 部分抄袭（50%）
			const char* orig2 = "1234";
			const char* plag2 = "1256";
			int lcs2 = lcs_length(orig2, plag2);
			double sim2 = (double)lcs2 / strlen(orig2);
			Assert::AreEqual(0.5, sim2, 0.001, L"部分抄袭的相似度");
		}
	};
}