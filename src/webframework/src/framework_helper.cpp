/**
 * @brief 框架帮助工具类
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.07.17
 */
#include "framework_helper.h"
#include <stdio.h>
#include <assert.h>
#include <cctype> 	// for toupper, isxdigit

namespace web_solution{ namespace web_framework{


    char CFrameworkHelper::x2c(char* pszWhat)
    {
        char cDigit;

        cDigit = (pszWhat[0] >= 'A' ? ((pszWhat[0] & 0xdf) - 'A')+10 : (pszWhat[0] - '0'));
        cDigit *= 16;
        cDigit += (pszWhat[1] >= 'A' ? ((pszWhat[1] & 0xdf) - 'A')+10 : (pszWhat[1] - '0'));

        return cDigit;
    }

	char CFrameworkHelper::x2c(char first, char second)
	{
		char cDigit;

        cDigit = (first >= 'A' ? ((first & 0xdf) - 'A')+10 : (first - '0'));
        cDigit *= 16;
        cDigit += (second >= 'A' ? ((second & 0xdf) - 'A')+10 : (second - '0'));

        return cDigit;
	}

    void CFrameworkHelper::StrToLowercase(char* s)
    {
        char* p;

        p = s;
		while (*p != 0)
		{
			*p = tolower(*p);
			p++;
		}

		return ;
	}

	void CFrameworkHelper::LRTrim(std::string& sValue)
	{
		if(sValue.empty())
			return;

		char chSpace = ' ';
		char chTab = '\t';
		bool bFind = false;
		std::string::const_iterator it1, it2;

		for(it1 = sValue.begin(); it1 != sValue.end(); it1++)
		{
			if(*it1 != chSpace && *it1 != chTab)
			{
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{
			sValue = "";
			return;
		}

		it2 = sValue.end() - 1;
		if(it2 == it1)
		{
			it2 = sValue.end();
		}
		else
		{
			for(; it2 >= it1; it2--)
			{
				if(*it2 != chSpace && *it2 != chTab)
				{
					it2++;
					break;
				}
			}
		}

		sValue = std::string(it1, it2);
	}

	void CFrameworkHelper::SpliteStrings(char cDelima, const std::string& sString, std::vector<std::string>& VecStrs)
	{
		std::string sStr = sString;
		std::string::const_iterator itItemStart;
		std::string::size_type itStartPos, itEndPos;
		char ch[2] = {0};
		ch[0] = cDelima;
		std::string sDelima = ch;

		// Append Delima if need
		if(*(sStr.rbegin()) != cDelima)
			sStr += sDelima;

		itStartPos = 0;
		itItemStart = sStr.begin();
		std::string sTmp;

		while(1)
		{
			itEndPos = sStr.find_first_of(cDelima, itStartPos);
			if(itEndPos == std::string::npos)
			{
				break;
			}

			sTmp = std::string(
					itItemStart + itStartPos,
					itItemStart + itEndPos);

			itStartPos = itEndPos + 1;

			if(sTmp.length() == 0) // No strValue between two Delimas
			{
				continue;
			}

			CFrameworkHelper::LRTrim(sTmp);

			VecStrs.push_back(sTmp);
		}
	}

	// case-insensitive string comparison
	// This code based on code from 
	// "The C++ Programming Language, Third Edition" by Bjarne Stroustrup
	bool CFrameworkHelper::StringsAreEqual(
		const std::string& s1, 
		const std::string& s2)
	{
		std::string::const_iterator p1 = s1.begin();
		std::string::const_iterator p2 = s2.begin();
		std::string::const_iterator l1 = s1.end();
		std::string::const_iterator l2 = s2.end();
		
		while(p1 != l1 && p2 != l2) {
			if(std::toupper(*(p1++)) != std::toupper(*(p2++)))
				return false;
		}
		
		return (s2.size() == s1.size());
	}

	bool CFrameworkHelper::StringsAreEqual(
		const std::string& s1, 
		const std::string& s2,
		uint32_t n)
	{
		std::string::const_iterator p1 = s1.begin();
		std::string::const_iterator p2 = s2.begin();
		bool good = (n <= s1.length() && n <= s2.length());
		std::string::const_iterator l1 = good ? (s1.begin() + n) : s1.end();
		std::string::const_iterator l2 = good ? (s2.begin() + n) : s2.end();
		
		while(p1 != l1 && p2 != l2) {
			if(std::toupper(*(p1++)) != std::toupper(*(p2++)))
				return false;
		}
		
		return good;
	}

	// locate data between separators, and return it
	void CFrameworkHelper::ExtractBetween(
		const std::string& data, 
		const std::string& separator1, 
		const std::string& separator2,
		std::string& result)
	{
		std::string::size_type start, limit;
		
		start = data.find(separator1, 0);
		if(std::string::npos != start) 
		{
			start += separator1.length();
			limit = data.find(separator2, start);
			if(std::string::npos != limit)
				result = data.substr(start, limit - start);
			else
				result = "";
		}
	}

	void CFrameworkHelper::OutputHead(const char* pszCharSet)
	{
		printf("Content-Type: text/html; charset=%s\n\n", pszCharSet);
	}

	void CFrameworkHelper::RedirectLocation(const char* pszLocation)
	{
		assert(pszLocation && strlen(pszLocation) > 0);
		
		time_t ltime;
		time(&ltime);
		
		//检查url的最后是否有?，如果没有么附加之, 如果有那么加上&
		if (strstr(pszLocation, "?") != NULL)
		{
			printf("Location: %s&CacheTime=%ld", pszLocation, ltime);
		}
		else
		{
			printf("Location: %s?CacheTime=%ld", pszLocation, ltime);
		}
		
		printf("\nCache-control: private");
		printf("\n\n\n");
	}

	/*
	void CFrameworkHelper::Encode(const char* pszStr, std::string& src, bool bWithChn)
	{
		if(bWithChn)
			return EncodeWithChn(pszStr, src);

		return EncodeWithoutChn(pszStr, src);
	}
	*/

	bool CFrameworkHelper::ISGBKLEAD(uint8_t c)
	{
		return(c>0x80 && c<0xff);
	}
	bool CFrameworkHelper::ISGBKNEXT(uint8_t c)
	{
		return((c>0x3f && c<0x7f)||(c>0x7f && c<0xff));
	}

	/*
	void CFrameworkHelper::EncodeWithUTF16(const char* pszStr, std::string& src)
	{
		assert(pszStr);

		static unsigned char HEXCHARS[] = "0123456789ABCDEF";
		register int x, y;

		uint32_t dwLen = strlen(pszStr); 
		//分配内存
		src.resize(3 * dwLen);
		//进行编码
		for (x = 0, y = 0; dwLen > 0; x++, y++)
		{
			src[y] = pszStr[x];
			if (pszStr[x] == ' ')
			{
				src[y++] = '%';
				src[y++] = '2';
				src[y]   = '0';
			}
			else if (pszStr[x] == '&')
			{
				src[y++] = '%';
				src[y++] = '2';
				src[y]   = '6';
			}
			else if (pszStr[x] == '=')
			{
				src[y++] = '%';
				src[y++] = '3';
				src[y]   = 'D';
			}
			else if (pszStr[x] == '%')
			{
				src[y++] = '%';
				src[y++] = '2';
				src[y]   = '5';
			}
			else if (pszStr[x] == '+')
			{
				src[y++] = '%';
				src[y++] = '2';
				src[y]   = 'B';
			}
			else if (pszStr[x] == '?')
			{
				src[y++] = '%';
				src[y++] = '3';
				src[y]   = 'F';
			}
			else if (pszStr[x] == ':')
			{
				src[y++] = '%';
				src[y++] = '3';
				src[y]   = 'A';
			}
			else if (pszStr[x] == '/')
			{
				src[y++] = '%';
				src[y++] = '2';
				src[y]   = 'F';
			}
			else if (pszStr[x] == '#')
			{
				src[y++] = '%';
				src[y++] = '2';
				src[y]   = '3';
			}
			else if(dwLen > 1 && ISGBKLEAD(pszStr[x]) && ISGBKNEXT(pszStr[x + 1]))
			{
				char szGBK[] = {pszStr[x], pszStr[x + 1], 0};
				char szUTF8[5] = {0};  // max 4
				uint16_t szUTF16[2] = {0};  // max 1
				size_t iUtf8Len = sizeof(szUTF8);
				tconv_gbk2utf8(szGBK, strlen(szGBK), szUTF8, &iUtf8Len);
				tconv_utf8to16((const unsigned char *)szUTF8, iUtf8Len, szUTF16, (size_t)sizeof(szUTF16));
				src[y++] = '%';
				src[y++] = 'u';
				src[y++]   = HEXCHARS[(unsigned char)((szUTF16[0] >> 12) & 15)];
				src[y++]   = HEXCHARS[(unsigned char)((szUTF16[0] >> 8) & 15)];
				src[y++]   = HEXCHARS[(unsigned char)((szUTF16[0] >> 4) & 15)];
				src[y]     = HEXCHARS[(unsigned char)((szUTF16[0] >> 0) & 15)];
				x++;
				--dwLen;
			}
			
			else if ((pszStr[x] < '0' && pszStr[x] != '-' && pszStr[x] != '.') || 
					(pszStr[x] < 'A' && pszStr[x] > '9') ||
					(pszStr[x] > 'Z' && pszStr[x] < 'a' && pszStr[x] != '_') ||
					(pszStr[x] > 'z'))
			{
				src[y++] = '%';
				src[y++] = HEXCHARS[(unsigned char) pszStr[x] >> 4];
				src[y]   = HEXCHARS[(unsigned char) pszStr[x] & 15];
			}
			
			--dwLen;
		}
	}

	void CFrameworkHelper::DecodeWithUTF16(const std::string& sInput, std::string& sOutput)
	{
		uint32_t dwLen = sInput.length();
		sOutput.resize(dwLen);
		int j = 0;

		for(uint32_t i = 0; i < dwLen; ++i)
		{
			C2C_WW_LOG("[%d] : %c   ", i, sInput[i]);
		}
		
		for(uint32_t i = 0; i < dwLen; ++i)
		{
			if (i < dwLen && sInput[i] == '%' && sInput[i + 1] == 'u')
			{
				char szUTF16[5] = {sInput[i + 2], sInput[i + 3], sInput[i + 4], sInput[i + 5], 0};
				uint16_t awUTF16[2] = {strtoul(szUTF16, 0, 16) & 0xffff, 0};
				uint8_t szUTF8[5] = {0};  // max 4
				int iUTF8Len = tconv_utf16to8((const uint16_t *)awUTF16, (size_t)1, szUTF8, (size_t)sizeof(szUTF8));
				char szGBK[3] = {0};
				size_t iGBKLen = sizeof(szGBK);
				tconv_utf82gbk((const char*)szUTF8, iUTF8Len, szGBK, &iGBKLen);
				sOutput[j++] = szGBK[0];
				sOutput[j++] = szGBK[1];
				i = i + 5;
			}
			else
			{
				sOutput[j++] = sInput[i];
			}
		}
	
	}

	
	void CFrameworkHelper::EncodeWithoutChn(const char* pszStr, std::string& src)
	{
		assert(pszStr);

		static unsigned char HEXCHARS[] = "0123456789ABCDEF";
		register int x, y;

		uint32_t dwLen = strlen(pszStr); 
		//分配内存
		src.resize(3 * dwLen);
		//进行编码
		for (x = 0, y = 0; dwLen > 0; x++, y++)
		{
			src[y] = pszStr[x];
			if (pszStr[x] == ' ')
			{
				src[y++] = '%';
				src[y++] = '2';
				src[y]   = '0';
			}
			else if (pszStr[x] == '&')
			{
				src[y++] = '%';
				src[y++] = '2';
				src[y]   = '6';
			}
			else if (pszStr[x] == '=')
			{
				src[y++] = '%';
				src[y++] = '3';
				src[y]   = 'D';
			}
			else if (pszStr[x] == '%')
			{
				src[y++] = '%';
				src[y++] = '2';
				src[y]   = '5';
			}
			else if (pszStr[x] == '+')
			{
				src[y++] = '%';
				src[y++] = '2';
				src[y]   = 'B';
			}
			else if (pszStr[x] == '?')
			{
				src[y++] = '%';
				src[y++] = '3';
				src[y]   = 'F';
			}
			else if (pszStr[x] == ':')
			{
				src[y++] = '%';
				src[y++] = '3';
				src[y]   = 'A';
			}
			else if (pszStr[x] == '/')
			{
				src[y++] = '%';
				src[y++] = '2';
				src[y]   = 'F';
			}
			else if (pszStr[x] == '#')
			{
				src[y++] = '%';
				src[y++] = '2';
				src[y]   = '3';
			}
			else if(dwLen > 1 && ISGBKLEAD(pszStr[x]) && ISGBKNEXT(pszStr[x + 1]))
			{
				src[y++] = pszStr[x++];
				src[y] = pszStr[x];
				--dwLen;
			}
			else if ((pszStr[x] < '0' && pszStr[x] != '-' && pszStr[x] != '.') || 
					(pszStr[x] < 'A' && pszStr[x] > '9') ||
					(pszStr[x] > 'Z' && pszStr[x] < 'a' && pszStr[x] != '_') ||
					(pszStr[x] > 'z'))
			{
				src[y++] = '%';
				src[y++] = HEXCHARS[(unsigned char) pszStr[x] >> 4];
				src[y]   = HEXCHARS[(unsigned char) pszStr[x] & 15];
			}
			
			--dwLen;
		}
	}
	*/

	

	/*
	void CFrameworkHelper::Decode(std::string& src)
	{    
		const static char VAL_MATRIX[256] = { 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x00, 0x01,    
			0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,    
			0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F };

		std::string sBuf;    
		int i = 0;    
		int iLen = src.length();    
		uint8_t ch = 0;
		uint8_t cint = 0;    
		while (i < iLen)
		{    
			ch = src[i];    
			if (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || ('0' <= ch && ch <= '9'))
			{ 
				sBuf += ch;    
			} 
			else if (ch == '-' || ch == '_'|| ch == '.' || ch == '!' || ch == '~' || ch == '*'|| ch == '\'' || ch == '(' || ch == ')')
			{   
				sBuf += ch;    
			} 
			else if (ch == '+')
			{
				sBuf += ' ';    
			}
			else if (ch == '%')
			{    
				if ('u' != src[i + 1])
				{    
					if(i + 2 >= iLen) // Out of bounds
						break;

					cint = 0;    
					cint = (cint << 4) | VAL_MATRIX[static_cast<uint8_t>(src[i + 1])];    
					cint = (cint << 4) | VAL_MATRIX[static_cast<uint8_t>(src[i + 2])];    
					i += 2;    
					sBuf += cint;    
				} 
				else
				{    
					if(i + 5 >= iLen) // Out of bounds
						break;

					char szUTF16[] = {'0', 'x', src[i + 2], src[i + 3], src[i + 4], src[i + 5], 0};
					uint16_t awUTF16[2] = {strtoul(szUTF16, 0, 16) & 0xffff, 0};
					uint8_t szUTF8[5] = {0};  // max 4
					int iUTF8Len = tconv_utf16to8((const uint16_t *)awUTF16, (size_t)1, szUTF8, (size_t)sizeof(szUTF8));
					char szGBK[3] = {0};
					size_t iGBKLen = sizeof(szGBK);
					tconv_utf82gbk((const char*)szUTF8, iUTF8Len, szGBK, &iGBKLen);
					sBuf += szGBK[0];
					sBuf += szGBK[1];
					i = i + 5;

					cint = 0;    
					cint = (cint << 4) | VAL_MATRIX[static_cast<uint8_t>(src[i + 2])];    
					cint = (cint << 4) | VAL_MATRIX[static_cast<uint8_t>(src[i + 3])];    
					sBuf += cint;  
					cint = 0;    
					cint = (cint << 4) | VAL_MATRIX[static_cast<uint8_t>(src[i + 4])];    
					cint = (cint << 4) | VAL_MATRIX[static_cast<uint8_t>(src[i + 5])];    
					sBuf += cint;    
					i += 5;    
				}    
			} 
			else
			{    
				sBuf += ch;    
			}    
			i++;    
		}    
		src = sBuf;
	}  
	*/
	
	/*
	void CFrameworkHelper::EncodeWithChn(const char* pszStr, std::string& src)
	{
		assert(pszStr);
		const static char HEX_MATRIX[256][3] = { "00", "01", "02", "03", "04", "05",    
			"06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", "10",    
			"11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B",    
			"1C", "1D", "1E", "1F", "20", "21", "22", "23", "24", "25", "26",    
			"27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F", "30", "31",    
			"32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C",    
			"3D", "3E", "3F", "40", "41", "42", "43", "44", "45", "46", "47",    
			"48", "49", "4A", "4B", "4C", "4D", "4E", "4F", "50", "51", "52",    
			"53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D",    
			"5E", "5F", "60", "61", "62", "63", "64", "65", "66", "67", "68",    
			"69", "6A", "6B", "6C", "6D", "6E", "6F", "70", "71", "72", "73",    
			"74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E",    
			"7F", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",    
			"8A", "8B", "8C", "8D", "8E", "8F", "90", "91", "92", "93", "94",    
			"95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",    
			"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA",    
			"AB", "AC", "AD", "AE", "AF", "B0", "B1", "B2", "B3", "B4", "B5",    
			"B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF", "C0",    
			"C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB",    
			"CC", "CD", "CE", "CF", "D0", "D1", "D2", "D3", "D4", "D5", "D6",    
			"D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF", "E0", "E1",    
			"E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC",    
			"ED", "EE", "EF", "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7",    
			"F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF" };

		std::string sBuf;    
		int iLen = strlen(pszStr);    
		uint8_t ch = 0;    
		for (int i = 0; i < iLen; ++i)
		{    
			ch = static_cast<uint8_t>(pszStr[i]);    
			if (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || ('0' <= ch && ch <= '9'))
			{    
				sBuf += (ch);    
			} 
			else if (ch == '-' || ch == '_' || ch == '.' || ch == '!' || ch == '~' || ch == '*' || ch == '\'' || ch == '(' || ch == ')')
			{    
				sBuf += (ch);    
			} 
			else if (ch <= 0x7F)
			{    
				sBuf += ('%');    
				sBuf += (HEX_MATRIX[ch]);    
			} 
			else
			{    
				if(i + 1 < iLen) // 双字节中文
				{
					sBuf += ('%');    
					sBuf += ('u');    
					sBuf += (HEX_MATRIX[ch]);    
					ch = static_cast<uint8_t>(pszStr[i + 1]);
					sBuf += (HEX_MATRIX[ch]);    
					++i;
				}
				else
				{
					sBuf += (HEX_MATRIX[ch]);    
				}
			}    
		}    
		src = sBuf;
	}    
	*/
}}

