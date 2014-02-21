#ifndef __WEB_SOLUTION_WEB_FRAMEWORK_VIEW_H__
#define __WEB_SOLUTION_WEB_FRAMEWORK_VIEW_H__

#include <stdint.h>
#include <map>
#include <list>
#include <string>

namespace web_solution{ namespace web_framework{

	class CWebViewTemplate;
	/**
	* @class CWebViewDict
	* @brief View字典类
	*/
	class CWebViewDict 
	{
		friend class  CWebViewTemplate;
	public:
		/**
		* @fn explicit CWebViewDict(const std::string& name)
		* @brief 显式构造函数
		* @param sName 字典名称(只是为了调试方便)
		*/
		explicit CWebViewDict(const std::string& sName);
		~CWebViewDict();
		/**
		* @fn const std::string& Name()
		* @brief 获取字典名称
		* @return 字典名称(只是为了调试方便)
		*/
		const std::string& Name();

	public:
		/**
		* @fn void SetValue(const std::string& sKey, const std::string& sVal)
		* @brief 设置string型字典项(key-value对)
		*/
		void SetValue(const std::string& sKey, const std::string& sVal);
		void SetValue(const std::string& sKey, int8_t v);
		void SetValue(const std::string& sKey, int16_t v);
		void SetValue(const std::string& sKey, int32_t v);
		void SetValue(const std::string& sKey, int64_t v);
		void SetValue(const std::string& sKey, uint8_t v);
		void SetValue(const std::string& sKey, uint16_t v);
		void SetValue(const std::string& sKey, uint32_t v);
		void SetValue(const std::string& sKey, uint64_t v);
		/**
		* @fn void SetIntValue(const std::string& sKey, int iVal)
		* @brief 设置int型字典项(key-value对)
		*/
		void SetIntValue(const std::string& sKey, int iVal);
		/**
		* @fn void SetMoney(const std::string& sKey, double val)
		* @brief 设置money型字典项(key-value对)
		*/
		void SetMoney(const std::string& sKey, double val);
		/**
		* @fn CWebViewDict& AddSectionDictionary(const std::string section_name)
		* @brief 添加一个Section字典
		* @param sSectionName Section名称
		* @return 返回一个ViewDict对象(NULL为失败),可被用来进行SetValue以及ShowSection等操作
		*/
		CWebViewDict* AddSectionDictionary(const std::string& sSectionName);
		/**
		* @fn void ShowSection(const std::string& sSectionName)
		* @brief 标记显示一个Section字典
		* @param sSectionName Section名称
		*/
		void ShowSection(const std::string& sSectionName);
		/**
		* @fn void SetValueAndShowSection(const std::string& sKey, const std::string& sVal, const std::string& sSectionName)
		* @brief 根据字段值是否非空来标记显示一个Section字典(字段非空则标记显示Section,否则隐藏Section)
		* @param sKey 字段名称
		* @param sVal 字段值
		* @param sSectionName Section名称
		* @return 返回一个ViewDict对象,可被用来进行SetValue以及ShowSection等操作
		*/
		void SetValueAndShowSection(const std::string& sKey, const std::string& sVal, const std::string& sSectionName);
		/**
		* @fn void Dump(int indent = 0) const
		* @brief Dump此字典
		* @param indent 每行缩进字符个数(缺省为不缩进)
		*/
		void Dump(int indent = 0) const;
		/**
		* @fn void DumpToString(std::string& sOut, int indent = 0) const
		* @brief Dump此字典到字符串
		* @param sOut 目的字符串
		* @param indent 每行缩进字符个数(缺省为不缩进)
		*/
		void DumpToString(std::string& sOut, int indent = 0) const;

		/**
		* @fn const std::string& Name()
		* @brief Clone
		* @return 新实例的指针
		*/
		//CWebViewDict* Clone();		

	protected:
		CWebViewDict();
		CWebViewDict(const CWebViewDict&);
		void operator=(const CWebViewDict&);

	protected:
		void* m_pDict;
		bool m_bRoot;
		std::list<CWebViewDict*> m_SubDictList;
	};

	/**
	* @class CWebViewTemplate
	* @brief View模板类
	*/
	class  CWebViewTemplate
	{
	public:
		/**
		* @enum Strip
		* @brief 输出的Strip选项
		*/
		enum Strip 
		{ 
			DO_NOT_STRIP, 
			STRIP_BLANK_LINES, 
			STRIP_WHITESPACE,
			NUM_STRIPS 
		};   
	public:
		virtual ~CWebViewTemplate();
		/**
		* @fn static CWebViewTemplate *GetTemplate(const std::string& sFileName, Strip strip = DO_NOT_STRIP)
		* @brief 从磁盘文件载入模板
		* @param sFileName 磁盘文件名
		* @param strip 输出的Strip选项@ref enum Strip
		* @return CWebViewTemplate对象(NULL为失败)
		*/
		static CWebViewTemplate* GetTemplate(const std::string& sFileName, Strip strip = DO_NOT_STRIP);
		const char *GetTemplateFileName() const;

		/**
		* @fn static CWebViewTemplate* StringToTemplate(const char* pszTplContent, Strip strip = DO_NOT_STRIP)
		* @brief 从字符串内容转换成模板
		* @param pszTplContent 以'\0'结尾字符串内容
		* @param strip 输出的Strip选项@ref enum Strip
		* @return CWebViewTemplate对象(NULL为失败)
		*/
		static CWebViewTemplate* StringToTemplate(const char* pszTplContent, Strip strip = DO_NOT_STRIP);

		/**
		* @fn static CWebViewTemplate* StringToTemplate(const std::string& sTplContent, Strip strip = DO_NOT_STRIP)
		* @brief 从字符串内容转换成模板
		* @param sTplContent STL String对象字符串内容
		* @param strip 输出的Strip选项@ref enum Strip
		* @return CWebViewTemplate对象(NULL为失败)
		*/
		static CWebViewTemplate* StringToTemplate(const std::string& sTplContent, Strip strip = DO_NOT_STRIP);

		/**
		* @fn static void ClearCache()
		* @brief 清空所有内部cache,析构所有Template句柄
		*/
		static void ClearCache();

		/**
		* @fn static bool SetTemplateRootDirectory(const std::string& sDir)
		* @brief 设置模板文件的根路径
		* @param sDir 根路径
		* @return true成功, false失败,路径不合法
		*/
		static bool SetTemplateRootDirectory(const std::string& sDir);
		static std::string GetTemplateRootDirectory();

		/**
		* @fn bool Expand(std::string& sOutBuffer, const CWebViewDict& rDict, bool bReset = true) const
		* @brief 填充并展开模板
		* @param sOutBuffer 保持内容的buffer
		* @param rDict View字典对象
		* @return true成功, false失败
		*/
		bool Expand(std::string& sOutBuffer, const CWebViewDict& rDict) const;

	public:
		
		CWebViewTemplate();

	protected:
		void* m_pTpl;
		static std::map<std::string, CWebViewTemplate> g_TplMap;
	};

	/**
	* @class CWebViewData
	* @brief View数据类,包含Dict和用户自定义控制数据
	*/
	class CWebViewData
	{
	public:
		/**
		* @fn CWebViewDict& GetDict()
		* @brief 获取模板字典对象
		*/
		CWebViewDict& GetDict();

	public:
		void SetUserData(const std::string& sName, const std::string& sVal);
		void SetUserData(const std::string& sName, int8_t cVal);
		void SetUserData(const std::string& sName, int16_t wVal);
		void SetUserData(const std::string& sName, int32_t dwVal);
		void SetUserData(const std::string& sName, int64_t ddwVal);
		void SetUserData(const std::string& sName, uint8_t cVal);
		void SetUserData(const std::string& sName, uint16_t wVal);
		void SetUserData(const std::string& sName, uint32_t dwVal);
		void SetUserData(const std::string& sName, uint64_t ddwVal);

		int GetUserData(const std::string& sName, std::string& sVal);
		int GetUserData(const std::string& sName, int8_t& cVal);
		int GetUserData(const std::string& sName, int16_t& wVal);
		int GetUserData(const std::string& sName, int32_t& dwVal);
		int GetUserData(const std::string& sName, int64_t& ddwVal);
		int GetUserData(const std::string& sName, uint8_t& cVal);
		int GetUserData(const std::string& sName, uint16_t& wVal);
		int GetUserData(const std::string& sName, uint32_t& dwVal);
		int GetUserData(const std::string& sName, uint64_t& ddwVal);

		/* Cookie相关 */
		void DelCookieVal(const std::string& sName);
		void UpdateCookieVal(const std::string& sName, const std::string& sVal, const std::string& sTime, const std::string& sDomain, bool bEncodeWithChn = true);
		void UpdateCookieVal2(const std::string& sName, const std::string& sVal, const std::string& sTime, const std::string& sDomain, const std::string& sEncodeType);
		void UpdateCookieVal(const std::string& sName, int iVal, const std::string& sTime, const std::string& sDomain, bool bEncodeWithChn = true);
		void UpdateCookieVal(const std::string& sName, uint32_t dwVal, const std::string& sTime, const std::string& sDomain, bool bEncodeWithChn = true);
		void UpdateCookieVal(const std::string& sName, int64_t ddwVal, const std::string& sTime, const std::string& sDomain, bool bEncodeWithChn = true);
		void UpdateCookieVal(const std::string& sName, uint64_t ddwVal, const std::string& sTime, const std::string& sDomain, bool bEncodeWithChn = true);
		
	public:
		bool OutputCookie();
		bool OutputCookie(std::string& sSetCookie);

	public:
		CWebViewData();

	private:
		void AddCookieVal(const std::string& sName, int iVal, const std::string& sTime, const std::string& sDomain);
		void AddCookieVal(const std::string& sName, uint32_t dwVal, const std::string& sTime, const std::string& sDomain);
		void AddCookieVal(const std::string& sName, int64_t ddwVal, const std::string& sTime, const std::string& sDomain);
		void AddCookieVal(const std::string& sName, uint64_t ddwVal, const std::string& sTime, const std::string& sDomain);
		void AddCookieVal(const std::string& sName, const std::string& sVal, const std::string& sTime, const std::string& sDomain);
		void AddCookieVal(const std::string& sName, const std::string& sVal, const std::string& sTime, const std::string& sDomain, const std::string& sEncodeType);
		void SetCookie(const char *name, const char *value, const char *expires, const char *path, const char *domain, short secure, const char *encode);
		void SetCookie(std::string& sOutput, const char *name, 
			const char *value, const char *expires, const char *path, const char *domain, short secure, const char *encode);
		
		void StringReplace(std::string & strBig, const std::string & strSrc, const std::string &strDst);
		char* UrlEncodeForCookie(char *sDest, const char *sSrc); 

	private:
		typedef struct tagCookie
		{
			bool bSet; // false-client端传入的原始cookie; true-server回给ie的更新cookie
			std::string sTime;
			std::string sDomain;
			std::string sVal;
			std::string sEncodeType;
		} COOKIE_T;
		std::map<std::string, COOKIE_T> m_oCookieMap;
		CWebViewDict m_oDict;
		std::map<std::string, std::string> m_oUserStrDataMap;
		std::map<std::string, int64_t> m_oUserIntDataMap;
		std::map<std::string, uint64_t> m_oUserUIntDataMap;
		bool m_bEncodeWithChn;
	};

}}

#endif /// __WEB_SOLUTION_WEB_FRAMEWORK_VIEW_H__
