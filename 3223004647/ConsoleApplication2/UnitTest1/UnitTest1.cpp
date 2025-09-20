#include "pch.h"
#include "CppUnitTest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include <windows.h>
#include "D:\rjgc\wuhongfei-531\3223004647\ConsoleApplication2\ConsoleApplication2\plagiarism_checker.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PlagiarismCheckerTests
{
	TEST_CLASS(PlagiarismTests)
	{
	private:
		// 清理临时文件
		void cleanupFile(const wchar_t* filePath) {
			_wremove(filePath);
		}

		// 将char*转换为wchar_t*
		wchar_t* charToWideChar(const char* str) {
			int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
			wchar_t* wstr = (wchar_t*)malloc(len * sizeof(wchar_t));
			MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, len);
			return wstr;
		}

		// 将wchar_t*转换为char*（使用系统默认编码）
		char* wideCharToChar(const wchar_t* wstr) {
			int len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
			char* str = (char*)malloc(len * sizeof(char));
			WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
			return str;
		}

		// 创建包含UTF-8数据的临时文件（带BOM）
		bool createUtf8TestFile(const wchar_t* filePath, const wchar_t* content) {
			FILE* f;
			errno_t err = _wfopen_s(&f, filePath, L"wb"); // 二进制模式写入BOM
			if (err != 0) return false;

			// 写入UTF-8 BOM
			const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
			fwrite(bom, 1, 3, f);

			// 转换为UTF-8并写入
			int len = WideCharToMultiByte(CP_UTF8, 0, content, -1, NULL, 0, NULL, NULL);
			char* utf8Content = (char*)malloc(len * sizeof(char));
			WideCharToMultiByte(CP_UTF8, 0, content, -1, utf8Content, len, NULL, NULL);

			fwrite(utf8Content, 1, len - 1, f); // 减1避免写入额外的空字符
			free(utf8Content);
			fclose(f);

			return true;
		}

	public:
		// 测试LCS算法核心功能
		TEST_METHOD(TestLcsLength) {
			Logger::WriteMessage(L"\n===== 测试LCS算法核心功能 =====\n");

			
			// 中文完全匹配
			Assert::AreEqual(4, lcs_length(L"测试文本", L"测试文本"), L"中文完全相同字符串");
			

			
			// 中文部分匹配
			Assert::AreEqual(4, lcs_length(L"中国上海北京", L"上海北京广州"), L"中文部分匹配(顺序不同)");

			// 完全不匹配
			
			Assert::AreEqual(0, lcs_length(L"苹果香蕉", L"西瓜橙子"), L"完全不同中文字符串");

			// 空字符串情况
			Assert::AreEqual(0, lcs_length(L"", L"测试"), L"第一个字符串为空");
			Assert::AreEqual(0, lcs_length(L"测试", L""), L"第二个字符串为空");
			Assert::AreEqual(0, lcs_length(L"", L""), L"两个字符串都为空");
		}

		// 测试UTF-8文件读取功能
		TEST_METHOD(TestReadUtf8File) {
			Logger::WriteMessage(L"\n===== 测试UTF-8文件读取 =====\n");
			const wchar_t* temp_file = L"temp_utf8_read.txt";
			const wchar_t* content = L"测试UTF-8编码的内容123";

			// 创建测试文件
			Assert::IsTrue(createUtf8TestFile(temp_file, content), L"无法创建UTF-8测试文件");

			// 转换路径为char*供read_utf8_file使用
			char* c_temp_file = wideCharToChar(temp_file);
			wchar_t* wcontent = read_utf8_file(c_temp_file);
			Assert::IsNotNull(wcontent, L"读取有效的UTF-8文件失败");

			// 检查并移除可能的BOM字符
			wchar_t* actual_content = wcontent;
			if (wcslen(wcontent) > 0 && (unsigned short)wcontent[0] == 0xFEFF) {
				Logger::WriteMessage(L"检测到BOM并已自动移除\n");
				actual_content = wcontent + 1; // 跳过BOM字符
			}

			int compareResult = wcscmp(actual_content, content);
			if (compareResult != 0) {
				wchar_t msg[1024];
				swprintf_s(msg,
					L"内容不匹配 - 预期长度: %d, 实际长度: %d (原始长度: %d)\n预期:[%s]\n实际:[%s]",
					wcslen(content), wcslen(actual_content), wcslen(wcontent),
					content, actual_content);
				Logger::WriteMessage(msg);

				// 逐个字符比较并输出Unicode码
				int minLen = min(wcslen(content), wcslen(actual_content));
				for (int i = 0; i < minLen; i++) {
					if (content[i] != actual_content[i]) {
						swprintf_s(msg, L"位置 %d 字符不匹配: 预期=%x, 实际=%x",
							i, content[i], actual_content[i]);
						Logger::WriteMessage(msg);
					}
				}
			}
			Assert::IsTrue(compareResult == 0, L"UTF-8文件数据读取错误");

			// 测试读取不存在的文件
			wchar_t* invalid_content = read_utf8_file("nonexistent_utf8.txt");
			Assert::IsNull(invalid_content, L"读取不存在的UTF-8文件未返回NULL");

			// 清理
			free(wcontent);
			free(c_temp_file);
			cleanupFile(temp_file);
		}

		// 测试文件写入功能
		TEST_METHOD(TestWriteResult) {
			Logger::WriteMessage(L"\n===== 测试文件写入 =====\n");
			const wchar_t* temp_file = L"temp_write.txt";
			char* c_temp_file = wideCharToChar(temp_file);

			// 测试正常写入
			int ret = write_result(c_temp_file, 0.85);
			Assert::AreEqual(1, ret, L"写入成功应返回1");

			// 验证写入内容
			FILE* f;
			errno_t err = _wfopen_s(&f, temp_file, L"r, ccs=UTF-8");
			if (err == 0) {
				wchar_t wbuf[20] = { 0 };
				fgetws(wbuf, sizeof(wbuf) / sizeof(wchar_t), f);
				fclose(f);

				// 移除可能的换行符
				if (wcslen(wbuf) > 0 && wbuf[wcslen(wbuf) - 1] == L'\n') {
					wbuf[wcslen(wbuf) - 1] = L'\0';
				}

				Assert::IsTrue(wcscmp(wbuf, L"0.85") == 0, L"写入的相似度值不正确");
			}
			else {
				Assert::Fail(L"无法验证写入文件");
			}

			// 清理
			free(c_temp_file);
			cleanupFile(temp_file);
		}

		// 测试相似度计算功能
		TEST_METHOD(TestSimilarity) {
			Logger::WriteMessage(L"\n===== 测试相似度计算 =====\n");

			// 完全抄袭(100%)-英文
			const wchar_t* orig1 = L"hello";
			const wchar_t* plag1 = L"hello";
			int lcs1 = lcs_length(orig1, plag1);
			int max_len1 = (wcslen(orig1) > wcslen(plag1)) ? wcslen(orig1) : wcslen(plag1);
			double sim1 = (max_len1 > 0) ? (double)lcs1 / max_len1 : 0.0;
			Assert::AreEqual(1.0, sim1, 0.001, L"英文完全抄袭相似度错误");

			// 完全抄袭(100%)-中文
			const wchar_t* orig2 = L"我爱中国";
			const wchar_t* plag2 = L"我爱中国";
			int lcs2 = lcs_length(orig2, plag2);
			int max_len2 = (wcslen(orig2) > wcslen(plag2)) ? wcslen(orig2) : wcslen(plag2);
			double sim2 = (max_len2 > 0) ? (double)lcs2 / max_len2 : 0.0;
			Assert::AreEqual(1.0, sim2, 0.001, L"中文完全抄袭相似度错误");

			// 部分抄袭(75%)-中文
			const wchar_t* orig3 = L"今天天气真的很好";
			const wchar_t* plag3 = L"今天天气很好";
			int lcs3 = lcs_length(orig3, plag3);
			int max_len3 = (wcslen(orig3) > wcslen(plag3)) ? wcslen(orig3) : wcslen(plag3);
			double sim3 = (max_len3 > 0) ? (double)lcs3 / max_len3 : 0.0;
			Assert::AreEqual(0.75, sim3, 0.001, L"中文部分抄袭相似度错误");

			// 中英文混合部分抄袭(修正预期值)
			const wchar_t* orig4 = L"C++是好用的编程语言";
			const wchar_t* plag4 = L"C#是好用的编程语言";
			int lcs4 = lcs_length(orig4, plag4);
			int max_len4 = (wcslen(orig4) > wcslen(plag4)) ? wcslen(orig4) : wcslen(plag4);
			double sim4 = (max_len4 > 0) ? (double)lcs4 / max_len4 : 0.0;
			Assert::AreEqual(0.818, sim4, 0.001, L"中英文混合部分抄袭相似度错误");

			// 原文件为空
			const wchar_t* orig5 = L"";
			const wchar_t* plag5 = L"任何内容";
			int lcs5 = lcs_length(orig5, plag5);
			int max_len5 = (wcslen(orig5) > wcslen(plag5)) ? wcslen(orig5) : wcslen(plag5);
			double sim5 = (max_len5 > 0) ? (double)lcs5 / max_len5 : 0.0;
			Assert::AreEqual(0.0, sim5, 0.001, L"原文件为空时相似度计算错误");
		}


	
	};
	
}
