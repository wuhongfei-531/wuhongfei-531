#include "pch.h"
#include "CppUnitTest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// ����������ͷ�ļ�����ȷ��·����ȷ��
#include "D:\rjgc\wuhongfei-531\3223004647\ConsoleApplication1\ConsoleApplication1\plagiarism_checker.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PlagiarismCheckerTests
{
	TEST_CLASS(PlagiarismTests)
	{
	private:
		// ���Ը�����������ʱ�ļ�
		void cleanupFile(const char* filePath) {
			remove(filePath);
		}

	public:
		// ����1��LCS�㷨�����߼�
		TEST_METHOD(TestLcsLength) {
			Logger::WriteMessage("\n===== ����LCS�㷨 =====\n");

			// ��ȫƥ��
			Assert::AreEqual(5, lcs_length("abcde", "abcde"), L"��ȫ��ͬ���ַ���");
			// ����ƥ��
			Assert::AreEqual(3, lcs_length("abcde", "ace"), L"����ƥ�䣨��������");
			// ��ȫ��ƥ��
			Assert::AreEqual(0, lcs_length("abc", "def"), L"��ȫ��ͬ���ַ���");
			// ���ַ�������
			Assert::AreEqual(0, lcs_length("", "test"), L"��һ���ַ���Ϊ��");
			Assert::AreEqual(0, lcs_length("test", ""), L"�ڶ����ַ���Ϊ��");
		}

		// ����2���ļ���ȡ���ܣ�read_file��
		TEST_METHOD(TestReadFile) {
			Logger::WriteMessage("\n===== �����ļ���ȡ =====\n");
			const char* temp_file = "temp_read.txt";

			// ���������ļ�
			FILE* f;
			errno_t err = fopen_s(&f, temp_file, "w");
			if (err == 0) {
				fprintf(f, "���ǲ�������123");
				fclose(f);
			}
			else {
				Assert::Fail(L"�޷����������ļ�");
				return;
			}

			// ����������ȡ
			char* content = read_file(temp_file);
			Assert::IsNotNull(content, L"��ȡ���ڵ��ļ�");
			Assert::AreEqual(std::string("���ǲ�������123"), std::string(content), L"�ļ�������ȷ");

			// ���Զ�ȡ�����ڵ��ļ�
			char* invalid_content = read_file("nonexistent.txt");
			Assert::IsNull(invalid_content, L"��ȡ�����ڵ��ļ�����NULL");

			// ����
			free(content);
			cleanupFile(temp_file);
		}

		// ����3���ļ�д�빦�ܣ�write_result��
		TEST_METHOD(TestWriteResult) {
			Logger::WriteMessage("\n===== �����ļ�д�� =====\n");
			const char* temp_file = "temp_write.txt";

			// ��������д��
			int ret = write_result(temp_file, 0.67);
			Assert::AreEqual(1, ret, L"д��ɹ�����1");

			// ��֤д������
			FILE* f;
			errno_t err = fopen_s(&f, temp_file, "r");
			if (err == 0) {
				char buf[20] = { 0 };
				fgets(buf, sizeof(buf), f);
				fclose(f);
				Assert::AreEqual(std::string("0.67"), std::string(buf), L"д������ƶ�ֵ��ȷ");
			}
			else {
				Assert::Fail(L"�޷���֤д���ļ�");
			}

			// ����
			cleanupFile(temp_file);
		}

		// ����4�����ƶȼ����߼�
		TEST_METHOD(TestSimilarity) {
			Logger::WriteMessage("\n===== �������ƶȼ��� =====\n");

			// ��ȫ��Ϯ��100%��
			const char* orig1 = "hello";
			const char* plag1 = "hello";
			int lcs1 = lcs_length(orig1, plag1);
			double sim1 = (double)lcs1 / strlen(orig1);
			Assert::AreEqual(1.0, sim1, 0.001, L"��ȫ��Ϯ�����ƶ�");

			// ���ֳ�Ϯ��50%��
			const char* orig2 = "1234";
			const char* plag2 = "1256";
			int lcs2 = lcs_length(orig2, plag2);
			double sim2 = (double)lcs2 / strlen(orig2);
			Assert::AreEqual(0.5, sim2, 0.001, L"���ֳ�Ϯ�����ƶ�");
		}
	};
}