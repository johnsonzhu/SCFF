/**
 * @brief 框架帮助工具类
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.07.17
 */
#ifndef __WEB_SOLUTION_WEB_FRAMEWORK_FRAMEWORK_HELPER_H__
#define __WEB_SOLUTION_WEB_FRAMEWORK_FRAMEWORK_HELPER_H__

#include <stdint.h>
#include <vector>
#include <string>

namespace web_solution{ namespace web_framework{
    class CFrameworkHelper
    {
    public:
        static char x2c(char* pszWhat);
		static char x2c(char first, char second);
        static void StrToLowercase(char* s);

		static void LRTrim(std::string& sValue);
		static void SpliteStrings(char cDelima, const std::string& sString, std::vector<std::string>& VecStrs);

		static bool StringsAreEqual(const std::string& s1, const std::string& s2);
		static bool StringsAreEqual(const std::string& s1, const std::string& s2, uint32_t n);

		static void ExtractBetween(
			const std::string& data, 
			const std::string& separator1, 
			const std::string& separator2,
			std::string& result);

		static void Decode(std::string& src);

		/**
		 * bWithChn:	
		 *     true  -- 中文将被编码成%uxxxx(注:若传入的中文是GBK的，那么编出来的%uxxxx不是Unicode的)
		 *     false -- 中文将被裸编码(原样保留,中文被当做2字节ASCII码来编码%xx)
		 */
		static void Encode(const char* pszStr, std::string& sResult, bool bWithChn = true);
		static void EncodeWithUTF16(const char* pszStr, std::string& src);

		// add by leywang
		static void DecodeWithUTF16(const std::string& sInput, std::string& sOutput);

		static void OutputHead(const char* pszCharSet = "gb2312");
		static void RedirectLocation(const char* pszLocation);

		static bool ISGBKLEAD(uint8_t c);
		static bool ISGBKNEXT(uint8_t c);

    private:
		static void EncodeWithChn(const char* pszStr, std::string& sResult);
		static void EncodeWithoutChn(const char* pszStr, std::string& sResult);

    private:
        struct stInputEntry
        {
            std::string sName;
            std::string sValue;
        };
    };
}}


#endif /// __WEB_SOLUTION_WEB_FRAMEWORK_FRAMEWORK_HELPER_H__



