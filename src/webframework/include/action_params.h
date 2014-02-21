
#ifndef __WEB_SOLUTION_WEB_FRAMEWORK_ACTION_PARAMS_H__
#define __WEB_SOLUTION_WEB_FRAMEWORK_ACTION_PARAMS_H__

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <stdint.h>

namespace web_solution{ namespace web_framework{
	class CActionInvocation;

#define ECC_TWS_ASYNC_KEY "EccTwsAsyncKey"
#define ECC_TWS_ASYNC_MAGIC_DATA "EccTwsAsyncMagicData"
#define ECC_TWS_ASYNC_BACKEND_RSP "EccTwsAsyncBackendRsp"

	/**
	 * @class CActionParams
	 * @brief 页面参数获取类
	 */
	class CActionParams
	{
		friend class CActionInvocation;
	public:
		/**
		 * @fn GetVal(const std::string& sName, std::string& sVal,uint32_t dwMaxLen = 0, uint32_t dwMinLen = 0, const char *pszPrompt = NULL, char* sErrMsg = NULL) const;
		 * @brief 获取字符串
		 * @param sName 值对的键值
		 * @param sVal  值对的内容
		 * @param dwMaxLen 字符串最大长度限定(0表示不限定)
		 * @param dwMinLen 字符串最小长度限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */
		int GetVal(const std::string& sName, std::string& sVal, 
				uint32_t dwMaxLen = 0, uint32_t dwMinLen = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;

		/**
		 * @fn GetVal(const std::string& sName, std::string& sVal, bool bNeedAntiXss = false, uint32_t dwMaxLen = 0, uint32_t dwMinLen = 0, const char *pszPrompt = NULL, char* sErrMsg = NULL) const;
		 * @brief 获取字符串
		 * @param sName 值对的键值
		 * @param sVal  值对的内容
		 * @param bNeedAntiXss 是否需要进行antixss过滤
		 * @param dwMaxLen 字符串最大长度限定(0表示不限定)
		 * @param dwMinLen 字符串最小长度限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */
		int GetVal(const std::string& sName, std::string& sVal, bool bNeedAntiXss, 
				uint32_t dwMaxLen = 0, uint32_t dwMinLen = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;

		/**
		 * @fn GetVal(const std::string& sName, std::string& sVal, bool bNeedAntiXss, uint32_t dwMaxLen = 0, uint32_t dwMinLen = 0, const char *pszPrompt = NULL, char* sErrMsg = NULL) const;
		 * @brief 获取字符串
		 * @param sName 值对的键值
		 * @param sVal  值对的内容
		 * @param bNeedAntiXss 是否需要进行antixss过滤
		 * @param sConf antixss过滤使用的配置文件
		 * @param dwMaxLen 字符串最大长度限定(0表示不限定)
		 * @param dwMinLen 字符串最小长度限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */
		int GetVal(const std::string& sName, std::string& sVal, bool bNeedAntiXss, const std::string &sConf,
				uint32_t dwMaxLen = 0, uint32_t dwMinLen = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
				

		
		/**
		 * @fn GetVal(const std::string& sName, char* psValBuf, uint32_t dwValBufLen, uint32_t dwMaxLen, uint32_t dwMinLen = 0, const char *pszPrompt = NULL, char* sErrMsg = NULL) const;
		 * @brief 获取字符串
		 * @param sName 值对的键值
		 * @param psValBuf  值对Buf
		 * @param dwValBufLen  值对BufLen
		 * @param dwMaxLen 字符串最大长度限定(0表示不限定)
		 * @param dwMinLen 字符串最小长度限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */
		int GetVal(const std::string& sName, char* psValBuf, uint32_t dwValBufLen,
				uint32_t dwMaxLen = 0, uint32_t dwMinLen = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
	

		/**
		 * @fn GetRawVal(const std::string& sName, std::string& sVal,uint32_t dwMaxLen = 0, uint32_t dwMinLen = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
		 * @brief 获取二进制RAW串
		 * @param sName 值对的键值
		 * @param sVal  值对的内容
		 * @param dwMaxLen 二进制RAW串最大长度限定(0表示不限定)
		 * @param dwMinLen 二进制RAW串最小长度限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */
		int GetRawVal(const std::string& sName, std::string& sVal, 
				uint32_t dwMaxLen = 0, uint32_t dwMinLen = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;

		/**
		 * @fn GetVal(const std::string& sName, uint32_t& dwVal,uint32_t dwMax = 0, uint32_t dwMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
		 * @brief 获取uint8_t类型数值
		 * @param sName 值对的键值
		 * @param cVal 值对的内容
		 * @param cMax 数值最大限定(0表示不限定)
		 * @param cMin 数值最小限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */		
		int GetVal(const std::string& sName, uint8_t& cVal, 			
				uint8_t cMax = 0, uint8_t cMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;

		/**
		 * @fn GetVal(const std::string& sName, uint32_t& dwVal,uint32_t dwMax = 0, uint32_t dwMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
		 * @brief 获取uint16_t类型数值
		 * @param sName 值对的键值
		 * @param wVal 值对的内容
		 * @param wMax 数值最大限定(0表示不限定)
		 * @param wMin 数值最小限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */		
		int GetVal(const std::string& sName, uint16_t& wVal, 			
				uint16_t wMax = 0, uint16_t wMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;

		/**
		 * @fn GetVal(const std::string& sName, uint32_t& dwVal,uint32_t dwMax = 0, uint32_t dwMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
		 * @brief 获取uint32_t类型数值
		 * @param sName 值对的键值
		 * @param dwVal 值对的内容
		 * @param dwMax 数值最大限定(0表示不限定)
		 * @param dwMin 数值最小限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */		
		int GetVal(const std::string& sName, uint32_t& dwVal, 			
				uint32_t dwMax = 0, uint32_t dwMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
				
		/**
		 * @fn GetVal(const std::string& sName, uint32_t& dwVal,uint32_t dwMax = 0, uint32_t dwMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
		 * @brief 获取uint64_t类型数值
		 * @param sName 值对的键值
		 * @param ddwVal 值对的内容
		 * @param ddwMax 数值最大限定(0表示不限定)
		 * @param ddwMin 数值最小限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */		
		int GetVal(const std::string& sName, uint64_t& ddwVal, 			
				uint64_t ddwMax = 0, uint64_t ddwMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;

		/**
		 * @fn GetVal(const std::string& sName, uint32_t& dwVal,uint32_t dwMax = 0, uint32_t dwMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
		 * @brief 获取int8_t类型数值
		 * @param sName 值对的键值
		 * @param cVal 值对的内容
		 * @param cMax 数值最大限定(0表示不限定)
		 * @param cMin 数值最小限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */		
		int GetVal(const std::string& sName, int8_t& cVal, 			
				int8_t cMax = 0, int8_t cMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
	
		/**
		 * @fn GetVal(const std::string& sName, uint32_t& dwVal,uint32_t dwMax = 0, uint32_t dwMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
		 * @brief 获取int16_t类型数值
		 * @param sName 值对的键值
		 * @param wVal 值对的内容
		 * @param wMax 数值最大限定(0表示不限定)
		 * @param wMin 数值最小限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */		
		int GetVal(const std::string& sName, int16_t& wVal, 			
				int16_t wMax = 0, int16_t wMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;

		/**
		 * @fn GetVal(const std::string& sName, uint32_t& dwVal,uint32_t dwMax = 0, uint32_t dwMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
		 * @brief 获取int32_t类型数值
		 * @param sName 值对的键值
		 * @param iVal 值对的内容
		 * @param iMax 数值最大限定(0表示不限定)
		 * @param iMin 数值最小限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */		
		int GetVal(const std::string& sName, int32_t& iVal, 			
				int32_t iMax = 0, int32_t iMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;

	/**
		 * @fn GetVal(const std::string& sName, uint32_t& dwVal,uint32_t dwMax = 0, uint32_t dwMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;
		 * @brief 获取int64_t类型数值
		 * @param sName 值对的键值
		 * @param ddwVal 值对的内容
		 * @param ddwMax 数值最大限定(0表示不限定)
		 * @param ddwMin 数值最小限定(0表示不限定)
		 * @param pszPrompt 出错时候的提示语前缀,便于在获取GetLastErrMsg()时得到人性化的内容
		 * @param sErrMsg[] 如果用户提供错误buffer,则会将GetLastErrMsg()的内容赋于它
		 */		
		int GetVal(const std::string& sName, int64_t& ddVal, 			
				int64_t ddwMax = 0, int64_t ddMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;


		int GetVal(const std::string& sName, std::vector<std::string>& vecVal) const;
		/*
		int GetVal(const std::string& sName, uint8_t& cVal) const;
		int GetVal(const std::string& sName, uint16_t& wVal) const;
		int GetVal(const std::string& sName, uint64_t& ddwVal) const;
		int GetVal(const std::string& sName, int8_t& cVal) const;
		int GetVal(const std::string& sName, int16_t& wVal) const;
		int GetVal(const std::string& sName, int32_t& dwVal) const;
		int GetVal(const std::string& sName, int64_t& ddwVal) const;
		*/

		int GetIntMoney(const std::string& sName, uint32_t& dwMoney, uint32_t dwMax = 0, uint32_t dwMin = 0, const char *pszPrompt = NULL, char sErrMsg[] = NULL) const;

	public:
		int GetCookieVal(const std::string& sName, std::string& sVal) const;
		int GetCookieVal(const std::string& sName, uint8_t& cVal) const;
		int GetCookieVal(const std::string& sName, uint16_t& wVal) const;
		int GetCookieVal(const std::string& sName, uint32_t& dwVal) const;
		int GetCookieVal(const std::string& sName, uint64_t& ddwVal) const;
		int GetCookieVal(const std::string& sName, int8_t& cVal) const;
		int GetCookieVal(const std::string& sName, int16_t& wVal) const;
		int GetCookieVal(const std::string& sName, int32_t& dwVal) const;
		int GetCookieVal(const std::string& sName, int64_t& ddwVal) const;
		
	public:
		void Reset();
		void Dump(std::ostream& os);
		void AddVal(const std::string& sName, const std::string& sVal);
		const char* GetLastErrMsg() const;
		// antixss 过滤
		int DoAntiXss(std::string& sInput) const;
		int DoAntiXss(std::string& sInput, const std::string &sConf) const;
		void AddCookiePair(const std::string& sName, const std::string& sVal); // For CActionInvocation

	private:
		void StringReplace(std::string & strBig, const std::string & strSrc, const std::string &strDst);
		bool IsNumber(const std::string& sVal) const;
		bool IsMoney(const std::string& sMoney) const;
		bool IsPostCode(const std::string& sPostCode) const;
		// 去掉非法字符和半个汉字
		std::string CleanChineseString(const char *pszRawString, uint32_t dwLength) const;

	private:
		typedef struct tagParam
		{
			std::vector<std::string> vecVal;
		} PARAM_T;

		typedef struct tagCookie
		{
			bool bSet; // false-client端传入的原始cookie; true-server回给ie的更新cookie
			std::string sTime;
			std::string sDomain;
			std::string sVal;
		} COOKIE_T;

	private:	
		std::map<std::string, PARAM_T> m_oParamsMap;
		std::map<std::string, COOKIE_T> m_oCookieMap;
		char m_sErrMsg[128];

	private:
		int GetParam(const std::string& sName, PARAM_T** ppParam) const;
	};
}}

#endif /* __WEB_SOLUTION_WEB_FRAMEWORK_ACTION_PARAMS_H__ */

