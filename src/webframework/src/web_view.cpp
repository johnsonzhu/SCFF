
#include "web_view.h"
#include "framework_helper.h"
#include "google/template.h"
#include <stdio.h>
#include <assert.h>

#define WEBFRAMEWORK_VERSION "WEBFRAMEWORK_VERSION 3.0."__DATE__   // version = major.minor.date
const std::string webframework_nouse = WEBFRAMEWORK_VERSION ; // 一定要此句,否则会被优化掉（不一定做全局量）

using namespace std;

namespace web_solution{ namespace web_framework{

CWebViewDict::CWebViewDict() : m_pDict(NULL), m_bRoot(false)
{
}

CWebViewDict::CWebViewDict(const std::string& sName)
{
	google::TemplateDictionary* pDict = new google::TemplateDictionary(sName);
	assert(pDict);
	m_pDict = reinterpret_cast<void*>(pDict);
	m_bRoot = true;
}


/*
没有办法恢复m_SubDictList
CWebViewDict* CWebViewDict::Clone()
{
	if (!m_pDict) return NULL; 	
	google::TemplateDictionary* pNewGoogleDic = reinterpret_cast<google::TemplateDictionary*>(m_pDict)->MakeCopy(Name());
	CWebViewDict* pWebViewDict =  new CWebViewDict;
	pWebViewDict->m_pDict = pNewGoogleDic;
	pWebViewDict->m_bRoot = m_bRoot;
	
	return pWebViewDict;
}
*/

CWebViewDict::~CWebViewDict()
{
	std::list<CWebViewDict*>::iterator it = m_SubDictList.begin();
	for(; it != m_SubDictList.end(); ++it)
	{
		CWebViewDict* pTmp = (*it);
		assert(pTmp);
		delete pTmp;
	}

	if(m_bRoot && m_pDict)
	{
		google::TemplateDictionary* pDict = reinterpret_cast<google::TemplateDictionary*>(m_pDict);
		delete pDict;
	}
}

const std::string& CWebViewDict::Name()
{
	return (reinterpret_cast<google::TemplateDictionary*>(m_pDict))->name();
}

void CWebViewDict::SetValue(const std::string& sKey, const std::string& sVal)
{
	(reinterpret_cast<google::TemplateDictionary*>(m_pDict))->SetValue(
		google::TemplateString(sKey), google::TemplateString(sVal));
}

void CWebViewDict::SetValue(const std::string& sKey, int8_t v)
{
	char x[20]={0};
	snprintf(x,sizeof(x),"%d", v);
	SetValue(sKey,x);
}

void CWebViewDict::SetValue(const std::string& sKey, int16_t v)
{
	char x[20]={0};
	snprintf(x,sizeof(x),"%d", v);
	SetValue(sKey,x);
}

void CWebViewDict::SetValue(const std::string& sKey, int32_t v)
{
	char x[20]={0};
	snprintf(x,sizeof(x),"%d", v);
	SetValue(sKey,x);
}

void CWebViewDict::SetValue(const std::string& sKey, int64_t v)
{
	char x[20]={0};
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
	snprintf(x,sizeof(x),"%lid", v);
#else   
	snprintf(x,sizeof(x),"%lld", v);
#endif

	SetValue(sKey,x);
}

void CWebViewDict::SetValue(const std::string& sKey, uint8_t v)
{
	char x[20]={0};
	snprintf(x,sizeof(x),"%u", v);
	SetValue(sKey,x);
}

void CWebViewDict::SetValue(const std::string& sKey, uint16_t v)
{
	char x[20]={0};
	snprintf(x,sizeof(x),"%u", v);
	SetValue(sKey,x);
}

void CWebViewDict::SetValue(const std::string& sKey, uint32_t v)
{
	char x[20]={0};
	snprintf(x,sizeof(x),"%u", v);
	SetValue(sKey,x);
}

void CWebViewDict::SetValue(const std::string& sKey, uint64_t v)
{
	char x[20]={0};
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
	snprintf(x,sizeof(x),"%liu", v);
#else   
	snprintf(x,sizeof(x),"%llu", v);
#endif
	SetValue(sKey,x);
}

void CWebViewDict::SetIntValue(const std::string& sKey, int iVal)
{
	(reinterpret_cast<google::TemplateDictionary*>(m_pDict))->SetIntValue(
		google::TemplateString(sKey), iVal);
}

void CWebViewDict::SetMoney(const std::string& sKey, double val)
{
	char x[20]={0};
	snprintf(x,sizeof(x),"%0.02f", val);
	SetValue(sKey,x);
}

CWebViewDict* CWebViewDict::AddSectionDictionary(const std::string& sSectionName)
{
	// Generator one
	google::TemplateDictionary* pDict = 
		(reinterpret_cast<google::TemplateDictionary*>(m_pDict))->AddSectionDictionary(
		google::TemplateString(sSectionName));
	if(NULL == pDict)
		return NULL;

	// SubDict, ::TemplateDictionary内部保存Section实例,并会批量析构
	// 所以此SubDict的m_pDict无需在析构函数中二次析构
	CWebViewDict* pTmp = new CWebViewDict; 
	pTmp->m_pDict = reinterpret_cast<void*>(pDict);
	m_SubDictList.push_back(pTmp);

	return pTmp;
}

void CWebViewDict::ShowSection(const std::string& sSectionName)
{
	(reinterpret_cast<google::TemplateDictionary*>(m_pDict))->ShowSection(google::TemplateString(sSectionName));
}

void CWebViewDict::SetValueAndShowSection(const std::string& sKey, const std::string& sVal, const std::string& sSectionName)
{
	(reinterpret_cast<google::TemplateDictionary*>(m_pDict))->SetValueAndShowSection(
		google::TemplateString(sKey), google::TemplateString(sVal), google::TemplateString(sSectionName));
}

void CWebViewDict::Dump(int indent) const
{
	(reinterpret_cast<google::TemplateDictionary*>(m_pDict))->Dump(indent);
}

void CWebViewDict::DumpToString(std::string& sOut, int indent) const
{
	(reinterpret_cast<google::TemplateDictionary*>(m_pDict))->DumpToString(&sOut, indent);
}

/////////////////////////////////////////////////////////
std::map<std::string, CWebViewTemplate> CWebViewTemplate::g_TplMap;

CWebViewTemplate* CWebViewTemplate::GetTemplate(const std::string& sFileName, Strip strip)
{
	std::map<std::string, CWebViewTemplate>::iterator it = g_TplMap.find(sFileName);
	if(it != g_TplMap.end())
		return &((*it).second);

	google::Strip g_strip = google::DO_NOT_STRIP;
	switch(strip)
	{
	case DO_NOT_STRIP:
		g_strip = google::DO_NOT_STRIP;
		break;
	case STRIP_BLANK_LINES:
		g_strip = google::STRIP_BLANK_LINES;
		break;
	case STRIP_WHITESPACE:
		g_strip = google::STRIP_WHITESPACE;
		break;
	case NUM_STRIPS:
		g_strip = google::NUM_STRIPS;
		break;
	default:
		assert(0);
	}
	google::Template* pTpl = google::Template::GetTemplate(sFileName, g_strip);
	if(!pTpl)
		return NULL;

	void* pTmp = reinterpret_cast<void*>(pTpl);

	CWebViewTemplate oViewTpl;
	oViewTpl.m_pTpl = pTmp;
	g_TplMap[sFileName] = oViewTpl;

	return &(g_TplMap[sFileName]);
}

void CWebViewTemplate::ClearCache()
{
	google::Template::ClearCache();
	g_TplMap.clear();
}

const char *CWebViewTemplate::GetTemplateFileName() const
{
	if(!m_pTpl)
		return "";
	return (reinterpret_cast<google::Template*>(m_pTpl))->template_file();
}

CWebViewTemplate::CWebViewTemplate() : m_pTpl(NULL)
{
}

CWebViewTemplate::~CWebViewTemplate()
{
}

bool CWebViewTemplate::SetTemplateRootDirectory(const std::string& sDir)
{
	return google::Template::SetTemplateRootDirectory(sDir);
}

std::string CWebViewTemplate::GetTemplateRootDirectory()
{
	return google::Template::template_root_directory();
}

bool CWebViewTemplate::Expand(std::string& sOutBuffer, const CWebViewDict& rDict) const
{
	assert(m_pTpl);

	return (reinterpret_cast<google::Template*>(m_pTpl))->Expand(
		&sOutBuffer, reinterpret_cast<google::TemplateDictionary*>(rDict.m_pDict));
}

CWebViewDict& CWebViewData::GetDict() 
{ 
	return m_oDict; 
}

CWebViewData::CWebViewData() 
: m_oDict("root"), m_bEncodeWithChn(true) 
{
}

void CWebViewData::AddCookieVal(const std::string& sName, const std::string& sVal, 
	const std::string& sTime, const std::string& sDomain)
{
	COOKIE_T stCookie;
	stCookie.sVal = sVal;
	stCookie.sTime = sTime;
	stCookie.sDomain = sDomain;
	stCookie.bSet = true;
	m_oCookieMap[sName] = stCookie;
}

void CWebViewData::AddCookieVal(const std::string& sName, const std::string& sVal, 
	const std::string& sTime, const std::string& sDomain, const std::string& sEncodeType)
{
	AddCookieVal(sName, sVal, sTime, sDomain);
	m_oCookieMap[sName].sEncodeType = sEncodeType;
}

void CWebViewData::AddCookieVal(const std::string& sName, int iVal, const std::string& sTime, const std::string& sDomain)
{
	char buf[32] = {0};
	snprintf(buf, sizeof(buf), "%d", iVal);
	AddCookieVal(sName, buf, sTime, sDomain);
}

void CWebViewData::AddCookieVal(const std::string& sName, uint32_t dwVal, const std::string& sTime, const std::string& sDomain)
{
	char buf[32] = {0};
	snprintf(buf, sizeof(buf), "%u", dwVal);
	AddCookieVal(sName, buf, sTime, sDomain);
}

void CWebViewData::AddCookieVal(const std::string& sName, int64_t ddwVal, const std::string& sTime, const std::string& sDomain)
{
	char buf[32] = {0};
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
	snprintf(buf, sizeof(buf), "%lid", ddwVal);
#else   
	snprintf(buf, sizeof(buf), "%lld", ddwVal);
#endif
	AddCookieVal(sName, buf, sTime, sDomain);
}

void CWebViewData::AddCookieVal(const std::string& sName, uint64_t ddwVal, const std::string& sTime, const std::string& sDomain)
{
	char buf[32] = {0};
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
	snprintf(buf, sizeof(buf), "%liu", ddwVal);
#else   
	snprintf(buf, sizeof(buf), "%llu", ddwVal);
#endif
	AddCookieVal(sName, buf, sTime, sDomain);
}

void CWebViewData::UpdateCookieVal(const std::string& sName, 
        int iVal, const std::string& sTime, const std::string& sDomain, bool bEncodeWithChn)
{
	if(m_bEncodeWithChn) // 一旦前次被设成false，说明业务有这种裸编码的需求，就一贯到底
		m_bEncodeWithChn = bEncodeWithChn;

	AddCookieVal(sName, iVal, sTime, sDomain);
}

void CWebViewData::UpdateCookieVal(const std::string& sName, uint32_t dwVal, const std::string& sTime, const std::string& sDomain, bool bEncodeWithChn)
{
	if(m_bEncodeWithChn) // 一旦前次被设成false，说明业务有这种裸编码的需求，就一贯到底
		m_bEncodeWithChn = bEncodeWithChn;

	AddCookieVal(sName, dwVal, sTime, sDomain);
}

void CWebViewData::UpdateCookieVal(const std::string& sName, int64_t ddwVal, const std::string& sTime, const std::string& sDomain, bool bEncodeWithChn)
{
	if(m_bEncodeWithChn) // 一旦前次被设成false，说明业务有这种裸编码的需求，就一贯到底
		m_bEncodeWithChn = bEncodeWithChn;

	AddCookieVal(sName, ddwVal, sTime, sDomain);
}

void CWebViewData::UpdateCookieVal(const std::string& sName, uint64_t ddwVal, const std::string& sTime, const std::string& sDomain, bool bEncodeWithChn)
{
	if(m_bEncodeWithChn) // 一旦前次被设成false，说明业务有这种裸编码的需求，就一贯到底
		m_bEncodeWithChn = bEncodeWithChn;

	AddCookieVal(sName, ddwVal, sTime, sDomain);
}

void CWebViewData::UpdateCookieVal(const std::string& sName, 
	const std::string& sVal, const std::string& sTime, const std::string& sDomain, bool bEncodeWithChn)
{
	if(m_bEncodeWithChn)
		m_bEncodeWithChn = bEncodeWithChn;

	AddCookieVal(sName, sVal, sTime, sDomain);
}

void CWebViewData::UpdateCookieVal2(const std::string& sName, const std::string& sVal, 
	const std::string& sTime, const std::string& sDomain, const std::string& sEncode)
{
	AddCookieVal(sName, sVal, sTime, sDomain, sEncode);
}

void CWebViewData::DelCookieVal(const std::string& sName)
{
	std::map<std::string, COOKIE_T>::iterator it = m_oCookieMap.find(sName);
	if(it != m_oCookieMap.end())
	{
		COOKIE_T& rCookie = (*it).second;
		rCookie.sTime = "Thursday, 30-Sep-03 12:00:00 GMT";
		rCookie.bSet = true;
	}
}

// OutputCookie调用,实际写入
void CWebViewData::SetCookie(const char *name, const char *value, 
	const char *expires, const char *path, const char *domain, short secure, const char *encode)
{
	// Clean CRLF
	std::string sVal = value;
	std::string::size_type pos = sVal.find("\015\012");
	if(pos != std::string::npos)
		sVal.erase(pos, sVal.length());

	/* in later version, do checks for valid variables */
	std::string l_sName;
	std::string l_sVal;

	if (encode && strcmp(encode, "UTF-16") == 0)
	{
		//CFrameworkHelper::EncodeWithUTF16(name, l_sName);
		//CFrameworkHelper::EncodeWithUTF16(sVal.c_str(), l_sVal);
	}
	else
	{
		//CFrameworkHelper::Encode(name, l_sName, m_bEncodeWithChn);
		//CFrameworkHelper::Encode(sVal.c_str(), l_sVal, m_bEncodeWithChn);
	}
	
	printf("Set-Cookie: %s=%s;", name, value);
	if (expires != NULL)
		printf(" EXPIRES=%s;",expires);
	if (path != NULL)
		printf(" PATH=%s;",path);
	if (domain != NULL)
		printf(" DOMAIN=%s;",domain);
	if (secure)
		printf(" SECURE");
	printf("\n");

}

void CWebViewData::SetCookie(std::string& sOutput, const char *name, 
	const char *value, const char *expires, const char *path, const char *domain, short secure, const char *encode)
{
	// Clean CRLF
	std::string sVal = value;
	std::string::size_type pos = sVal.find("\015\012");
	if(pos != std::string::npos)
		sVal.erase(pos, sVal.length());

	/* in later version, do checks for valid variables */
	std::string l_sName;
	std::string l_sVal;
	
	if (encode && strcmp(encode, "UTF-16") == 0)
	{
		//CFrameworkHelper::EncodeWithUTF16(name, l_sName);
		//CFrameworkHelper::EncodeWithUTF16(sVal.c_str(), l_sVal);
	}
	else
	{
		//CFrameworkHelper::Encode(name, l_sName, m_bEncodeWithChn);
		//CFrameworkHelper::Encode(sVal.c_str(), l_sVal, m_bEncodeWithChn);
	}

	char sBuf[1024] = {0};
	snprintf(sBuf, sizeof(sBuf), "Set-Cookie: %s=%s;", name, value);
	sOutput = sBuf;
	if (expires != NULL)
	{
		sOutput += " EXPIRES=";
		sOutput += expires;
		sOutput += ";";
	}	
	if (path != NULL)
	{
		sOutput += " PATH=";
		sOutput += path;
		sOutput += ";";
	}	
	if (domain != NULL)
	{
		sOutput += " DOMAIN=";
		sOutput += domain;
		sOutput += ";";
	}	
	if (secure)
	{
		sOutput += " SECURE";
	}	
	sOutput += "\n";

}

// 针对TWS的
bool CWebViewData::OutputCookie(std::string& sSetCookie)
{
	std::map<std::string, COOKIE_T>::iterator it = m_oCookieMap.begin();
	for(; it != m_oCookieMap.end(); ++it)
	{
		COOKIE_T& rCookie = (*it).second;
		if(rCookie.bSet)
		{
			std::string sCookie;
			SetCookie(
				sCookie,
				(*it).first.c_str(), 
				rCookie.sVal.c_str(), 
				(rCookie.sTime.empty() ? NULL : rCookie.sTime.c_str()), 
				"/", 
				rCookie.sDomain.c_str(), 
				0,
				rCookie.sEncodeType.c_str());

				sSetCookie += sCookie;
		} // 未被Set的Cookie项表示无需让ie更新了
	}
	return m_bEncodeWithChn;
}

bool CWebViewData::OutputCookie()
{
	std::map<std::string, COOKIE_T>::iterator it = m_oCookieMap.begin();
	for(; it != m_oCookieMap.end(); ++it)
	{
		COOKIE_T& rCookie = (*it).second;
		if(rCookie.bSet)
		{
			SetCookie(
				(*it).first.c_str(), 
				rCookie.sVal.c_str(), 
				(rCookie.sTime.empty() ? NULL : rCookie.sTime.c_str()),
				"/", 
				rCookie.sDomain.c_str(), 
				0,
				rCookie.sEncodeType.c_str()); 	
		} // 未被Set的Cookie项表示无需让ie更新了
	}
	return m_bEncodeWithChn;
}

void CWebViewData::StringReplace(std::string & strBig, const std::string & strSrc, const std::string &strDst) 
{
	string::size_type iPos=0;
	string::size_type iSrcLen=strSrc.size();
	string::size_type iDstLen=strDst.size();
	while( (iPos=strBig.find(strSrc, iPos)) != string::npos)
	{
		strBig.replace(iPos, iSrcLen, strDst);
		iPos += iDstLen;
	}
}

void CWebViewData::SetUserData(const std::string& sName, const std::string& sVal)
{
	m_oUserStrDataMap[sName] = sVal;
}
void CWebViewData::SetUserData(const std::string& sName, int8_t cVal)
{
	m_oUserIntDataMap[sName] = cVal;
}
void CWebViewData::SetUserData(const std::string& sName, int16_t wVal)
{
	m_oUserIntDataMap[sName] = wVal;
}
void CWebViewData::SetUserData(const std::string& sName, int32_t dwVal)
{
	m_oUserIntDataMap[sName] = dwVal;
}
void CWebViewData::SetUserData(const std::string& sName, int64_t ddwVal)
{
	m_oUserIntDataMap[sName] = ddwVal;
}
void CWebViewData::SetUserData(const std::string& sName, uint8_t cVal)
{
	m_oUserUIntDataMap[sName] = cVal;
}
void CWebViewData::SetUserData(const std::string& sName, uint16_t wVal)
{
	m_oUserUIntDataMap[sName] = wVal;
}
void CWebViewData::SetUserData(const std::string& sName, uint32_t dwVal)
{
	m_oUserUIntDataMap[sName] = dwVal;
}
void CWebViewData::SetUserData(const std::string& sName, uint64_t ddwVal)
{
	m_oUserUIntDataMap[sName] = ddwVal;
}

int CWebViewData::GetUserData(const std::string& sName, std::string& sVal)
{
	std::map<std::string, std::string>::iterator it = m_oUserStrDataMap.find(sName);
	if(it == m_oUserStrDataMap.end())
	{
		sVal = "";
		return -1;
	}
	sVal = (*it).second;
	return 0;
}
int CWebViewData::GetUserData(const std::string& sName, int8_t& cVal)
{
	int64_t ddwVal = 0;
	if(GetUserData(sName, ddwVal) != 0)
	{
		cVal = 0;
		return -1;
	}
	cVal = static_cast<int8_t>(ddwVal);
	return 0;
}
int CWebViewData::GetUserData(const std::string& sName, int16_t& wVal)
{
	int64_t ddwVal = 0;
	if(GetUserData(sName, ddwVal) != 0)
	{
		wVal = 0;
		return -1;
	}
	wVal = static_cast<int16_t>(ddwVal);
	return 0;
}
int CWebViewData::GetUserData(const std::string& sName, int32_t& dwVal)
{
	int64_t ddwVal = 0;
	if(GetUserData(sName, ddwVal) != 0)
	{
		dwVal = 0;
		return -1;
	}
	dwVal = static_cast<int32_t>(ddwVal);
	return 0;
}
int CWebViewData::GetUserData(const std::string& sName, int64_t& ddwVal)
{
	std::map<std::string, int64_t>::iterator it = m_oUserIntDataMap.find(sName);
	if(it == m_oUserIntDataMap.end())
	{
		ddwVal = 0;
		return -1;
	}
	ddwVal = (*it).second;
	return 0;
}
int CWebViewData::GetUserData(const std::string& sName, uint8_t& cVal)
{
	uint64_t ddwVal = 0;
	if(GetUserData(sName, ddwVal) != 0)
	{
		cVal = 0;
		return -1;
	}
	cVal = static_cast<uint8_t>(ddwVal);
	return 0;
}
int CWebViewData::GetUserData(const std::string& sName, uint16_t& wVal)
{
	uint64_t ddwVal = 0;
	if(GetUserData(sName, ddwVal) != 0)
	{
		wVal = 0;
		return -1;
	}
	wVal = static_cast<uint16_t>(ddwVal);
	return 0;
}
int CWebViewData::GetUserData(const std::string& sName, uint32_t& dwVal)
{
	uint64_t ddwVal = 0;
	if(GetUserData(sName, ddwVal) != 0)
	{
		dwVal = 0;
		return -1;
	}
	dwVal = static_cast<uint32_t>(ddwVal);
	return 0;
}
int CWebViewData::GetUserData(const std::string& sName, uint64_t& ddwVal)
{
	std::map<std::string, uint64_t>::iterator it = m_oUserUIntDataMap.find(sName);
	if(it == m_oUserUIntDataMap.end())
	{
		ddwVal = 0;
		return -1;
	}
	ddwVal = (*it).second;
	return 0;
}
}}

