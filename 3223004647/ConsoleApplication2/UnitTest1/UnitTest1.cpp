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
		// ������ʱ�ļ�
		void cleanupFile(const wchar_t* filePath) {
			_wremove(filePath);
		}

		// ��char*ת��Ϊwchar_t*
		wchar_t* charToWideChar(const char* str) {
			int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
			wchar_t* wstr = (wchar_t*)malloc(len * sizeof(wchar_t));
			MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, len);
			return wstr;
		}

		// ��wchar_t*ת��Ϊchar*��ʹ��ϵͳĬ�ϱ��룩
		char* wideCharToChar(const wchar_t* wstr) {
			int len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
			char* str = (char*)malloc(len * sizeof(char));
			WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
			return str;
		}

		// ��������UTF-8���ݵ���ʱ�ļ�����BOM��
		bool createUtf8TestFile(const wchar_t* filePath, const wchar_t* content) {
			FILE* f;
			errno_t err = _wfopen_s(&f, filePath, L"wb"); // ������ģʽд��BOM
			if (err != 0) return false;

			// д��UTF-8 BOM
			const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
			fwrite(bom, 1, 3, f);

			// ת��ΪUTF-8��д��
			int len = WideCharToMultiByte(CP_UTF8, 0, content, -1, NULL, 0, NULL, NULL);
			char* utf8Content = (char*)malloc(len * sizeof(char));
			WideCharToMultiByte(CP_UTF8, 0, content, -1, utf8Content, len, NULL, NULL);

			fwrite(utf8Content, 1, len - 1, f); // ��1����д�����Ŀ��ַ�
			free(utf8Content);
			fclose(f);

			return true;
		}

	public:
		// ����LCS�㷨���Ĺ���
		TEST_METHOD(TestLcsLength) {
			Logger::WriteMessage(L"\n===== ����LCS�㷨���Ĺ��� =====\n");

			
			// ������ȫƥ��
			Assert::AreEqual(4, lcs_length(L"�����ı�", L"�����ı�"), L"������ȫ��ͬ�ַ���");
			

			
			// ���Ĳ���ƥ��
			Assert::AreEqual(4, lcs_length(L"�й��Ϻ�����", L"�Ϻ���������"), L"���Ĳ���ƥ��(˳��ͬ)");

			// ��ȫ��ƥ��
			
			Assert::AreEqual(0, lcs_length(L"ƻ���㽶", L"���ϳ���"), L"��ȫ��ͬ�����ַ���");

			// ���ַ������
			Assert::AreEqual(0, lcs_length(L"", L"����"), L"��һ���ַ���Ϊ��");
			Assert::AreEqual(0, lcs_length(L"����", L""), L"�ڶ����ַ���Ϊ��");
			Assert::AreEqual(0, lcs_length(L"", L""), L"�����ַ�����Ϊ��");
		}

		// ����UTF-8�ļ���ȡ����
		TEST_METHOD(TestReadUtf8File) {
			Logger::WriteMessage(L"\n===== ����UTF-8�ļ���ȡ =====\n");
			const wchar_t* temp_file = L"temp_utf8_read.txt";
			const wchar_t* content = L"����UTF-8���������123";

			// ���������ļ�
			Assert::IsTrue(createUtf8TestFile(temp_file, content), L"�޷�����UTF-8�����ļ�");

			// ת��·��Ϊchar*��read_utf8_fileʹ��
			char* c_temp_file = wideCharToChar(temp_file);
			wchar_t* wcontent = read_utf8_file(c_temp_file);
			Assert::IsNotNull(wcontent, L"��ȡ��Ч��UTF-8�ļ�ʧ��");

			// ��鲢�Ƴ����ܵ�BOM�ַ�
			wchar_t* actual_content = wcontent;
			if (wcslen(wcontent) > 0 && (unsigned short)wcontent[0] == 0xFEFF) {
				Logger::WriteMessage(L"��⵽BOM�����Զ��Ƴ�\n");
				actual_content = wcontent + 1; // ����BOM�ַ�
			}

			int compareResult = wcscmp(actual_content, content);
			if (compareResult != 0) {
				wchar_t msg[1024];
				swprintf_s(msg,
					L"���ݲ�ƥ�� - Ԥ�ڳ���: %d, ʵ�ʳ���: %d (ԭʼ����: %d)\nԤ��:[%s]\nʵ��:[%s]",
					wcslen(content), wcslen(actual_content), wcslen(wcontent),
					content, actual_content);
				Logger::WriteMessage(msg);

				// ����ַ��Ƚϲ����Unicode��
				int minLen = min(wcslen(content), wcslen(actual_content));
				for (int i = 0; i < minLen; i++) {
					if (content[i] != actual_content[i]) {
						swprintf_s(msg, L"λ�� %d �ַ���ƥ��: Ԥ��=%x, ʵ��=%x",
							i, content[i], actual_content[i]);
						Logger::WriteMessage(msg);
					}
				}
			}
			Assert::IsTrue(compareResult == 0, L"UTF-8�ļ����ݶ�ȡ����");

			// ���Զ�ȡ�����ڵ��ļ�
			wchar_t* invalid_content = read_utf8_file("nonexistent_utf8.txt");
			Assert::IsNull(invalid_content, L"��ȡ�����ڵ�UTF-8�ļ�δ����NULL");

			// ����
			free(wcontent);
			free(c_temp_file);
			cleanupFile(temp_file);
		}

		// �����ļ�д�빦��
		TEST_METHOD(TestWriteResult) {
			Logger::WriteMessage(L"\n===== �����ļ�д�� =====\n");
			const wchar_t* temp_file = L"temp_write.txt";
			char* c_temp_file = wideCharToChar(temp_file);

			// ��������д��
			int ret = write_result(c_temp_file, 0.85);
			Assert::AreEqual(1, ret, L"д��ɹ�Ӧ����1");

			// ��֤д������
			FILE* f;
			errno_t err = _wfopen_s(&f, temp_file, L"r, ccs=UTF-8");
			if (err == 0) {
				wchar_t wbuf[20] = { 0 };
				fgetws(wbuf, sizeof(wbuf) / sizeof(wchar_t), f);
				fclose(f);

				// �Ƴ����ܵĻ��з�
				if (wcslen(wbuf) > 0 && wbuf[wcslen(wbuf) - 1] == L'\n') {
					wbuf[wcslen(wbuf) - 1] = L'\0';
				}

				Assert::IsTrue(wcscmp(wbuf, L"0.85") == 0, L"д������ƶ�ֵ����ȷ");
			}
			else {
				Assert::Fail(L"�޷���֤д���ļ�");
			}

			// ����
			free(c_temp_file);
			cleanupFile(temp_file);
		}

		// �������ƶȼ��㹦��
		TEST_METHOD(TestSimilarity) {
			Logger::WriteMessage(L"\n===== �������ƶȼ��� =====\n");

			// ��ȫ��Ϯ(100%)-Ӣ��
			const wchar_t* orig1 = L"hello";
			const wchar_t* plag1 = L"hello";
			int lcs1 = lcs_length(orig1, plag1);
			int max_len1 = (wcslen(orig1) > wcslen(plag1)) ? wcslen(orig1) : wcslen(plag1);
			double sim1 = (max_len1 > 0) ? (double)lcs1 / max_len1 : 0.0;
			Assert::AreEqual(1.0, sim1, 0.001, L"Ӣ����ȫ��Ϯ���ƶȴ���");

			// ��ȫ��Ϯ(100%)-����
			const wchar_t* orig2 = L"�Ұ��й�";
			const wchar_t* plag2 = L"�Ұ��й�";
			int lcs2 = lcs_length(orig2, plag2);
			int max_len2 = (wcslen(orig2) > wcslen(plag2)) ? wcslen(orig2) : wcslen(plag2);
			double sim2 = (max_len2 > 0) ? (double)lcs2 / max_len2 : 0.0;
			Assert::AreEqual(1.0, sim2, 0.001, L"������ȫ��Ϯ���ƶȴ���");

			// ���ֳ�Ϯ(75%)-����
			const wchar_t* orig3 = L"����������ĺܺ�";
			const wchar_t* plag3 = L"���������ܺ�";
			int lcs3 = lcs_length(orig3, plag3);
			int max_len3 = (wcslen(orig3) > wcslen(plag3)) ? wcslen(orig3) : wcslen(plag3);
			double sim3 = (max_len3 > 0) ? (double)lcs3 / max_len3 : 0.0;
			Assert::AreEqual(0.75, sim3, 0.001, L"���Ĳ��ֳ�Ϯ���ƶȴ���");

			// ��Ӣ�Ļ�ϲ��ֳ�Ϯ(����Ԥ��ֵ)
			const wchar_t* orig4 = L"C++�Ǻ��õı������";
			const wchar_t* plag4 = L"C#�Ǻ��õı������";
			int lcs4 = lcs_length(orig4, plag4);
			int max_len4 = (wcslen(orig4) > wcslen(plag4)) ? wcslen(orig4) : wcslen(plag4);
			double sim4 = (max_len4 > 0) ? (double)lcs4 / max_len4 : 0.0;
			Assert::AreEqual(0.818, sim4, 0.001, L"��Ӣ�Ļ�ϲ��ֳ�Ϯ���ƶȴ���");

			// ԭ�ļ�Ϊ��
			const wchar_t* orig5 = L"";
			const wchar_t* plag5 = L"�κ�����";
			int lcs5 = lcs_length(orig5, plag5);
			int max_len5 = (wcslen(orig5) > wcslen(plag5)) ? wcslen(orig5) : wcslen(plag5);
			double sim5 = (max_len5 > 0) ? (double)lcs5 / max_len5 : 0.0;
			Assert::AreEqual(0.0, sim5, 0.001, L"ԭ�ļ�Ϊ��ʱ���ƶȼ������");
		}


	
	};
	
}
