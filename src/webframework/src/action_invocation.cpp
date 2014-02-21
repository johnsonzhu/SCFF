/**
 * @brief Action InvocationÀà
 * ´´½¨¡¢³ÖÓÐActionÊµÀýºÍ¶à¸öÀ¹½ØÆ÷ÊµÀýÁÐ±í£¬²¢¸ºÔð¶Ôinterceptors¡¢action¡¢resultµÈÄ£¿é½øÐÐµ÷ÓÃ
 * ÒÔÍê³ÉÇëÇóµÄ´¦Àí¼°ÏìÓ¦
 *
 * ÔÚ½Ó¿ÚÉÏÒÔµ¥¼þµÄ·½Ê½Ìá¹©£¬±ÜÃâ¶à´Î´´½¨¡¢Ïú»ÙµÄ¿ªÏú
 */
#include <algorithm>
#include <sstream>
#include <stdlib.h>
#include <assert.h>
#include "action_invocation.h"
#include "framework_helper.h"
//#include "http_response_process.h"

namespace web_solution{ namespace web_framework{
	CActionInvocation* CActionInvocation::m_pInstance = NULL;

	CActionInvocation* CActionInvocation::GetInstance()
	{
	
		if(!m_pInstance)
			m_pInstance = new CActionInvocation;

		return m_pInstance;
	}

	CActionInvocation::~CActionInvocation()
	{
		/// É¾³ýËùÓÐµÄÀ¹½ØÆ÷¶ÔÏó
		std::list<CInterceptor*>::iterator pInterceptorPos;

		for (pInterceptorPos = m_oInterceptorList.begin(); 
				pInterceptorPos != m_oInterceptorList.end(); ++pInterceptorPos)
		{
			if(*pInterceptorPos)
			{
				(*pInterceptorPos)->Destroy();

				delete *pInterceptorPos;
				*pInterceptorPos = NULL; 
			}
		}
	}

	bool CActionInvocation::SetInterceptor(CInterceptor* pInterceptor, void* pParameter)
	{
		assert(pInterceptor);

		if(pInterceptor->Init(pParameter))
		{
			/// Èô³õÊ¼»¯³É¹¦£¬ÔòÌí¼Óµ½À¹½ØÆ÷ÁÐ±íÎ²¶Ë
			m_oInterceptorList.push_back(pInterceptor);
			return true;
		}
		return false;
	}

	bool CActionInvocation::SetStdInterceptorByName(const std::string& sInterceptorName, 
			void* pParameter)
	{
		CInterceptor* pInterceptor = 
			m_oInterceptorFactory.GetInterceptor(sInterceptorName);
		if(!pInterceptor)
		{
			return false;
		}

		if(pInterceptor->Init(pParameter))
		{
			/// Èô³õÊ¼»¯³É¹¦£¬ÔòÌí¼Óµ½À¹½ØÆ÷ÁÐ±íÎ²¶Ë
			m_oInterceptorList.push_back(pInterceptor);
			return true;
		}

		return false;
	}

	void CActionInvocation::SetLoggerName(std::string& sLoggerName)
	{
		m_sLoggerName = sLoggerName;
	}

	const std::string& CActionInvocation::GetLoggerName() const
	{
		return m_sLoggerName;
	}

	const std::string& CActionInvocation::GetLastErrMsg() const
	{
		return m_sLastErrMsg;
	}

	/// ¸ù¾ÝÇëÇóurl Ñ¡ÔñÏàÓ¦µÄaction
	int CActionInvocation::ParseActionName(
			uint8_t cMethodType,
			const std::string& sFileName, 
			const std::string& sQueryString, 
			const std::string& sPostInfo,
			const std::string& sContentType,
			const std::string& sCookies,
			std::string& sControllerName, 
			std::string& sActionName, 
			CActionParams& oActionParam)
	{
	
		if(sFileName.empty())
		{
			m_sLastErrMsg = "sFileName is empty";
			return -1;
		}

		static const std::string standard_type	= "application/x-www-form-urlencoded";
		static const std::string multipart_type = "multipart/form-data";

		// Make FileName like "shop/add?name=3&test=6" or  "shop/add/3/6"
		/// http://www.paipai.com/shop/add/1/2 ÔòsFileName = shop/add/1/2
		/// http://www.paipai.com/shop/add     ÔòsFileName = shop/add
		/// ÉÏÊöµÄaction = add
		/// ¼´Ñ¡ÔñµÚÒ»´Î³öÏÖ'/'ÓëÖ®ºó×î½ü³öÏÖ'/'Ö®¼äµÄ×Ö´®×÷Îªaction name

		std::string l_sQueryString = sQueryString;
		CFrameworkHelper::LRTrim(l_sQueryString);
		std::string l_sPostInfo = sPostInfo;
		CFrameworkHelper::LRTrim(l_sPostInfo);
		std::string l_sFileName = sFileName;
		CFrameworkHelper::LRTrim(l_sFileName);

		bool bMultiPartForm = false;
		if(0x03 == cMethodType) // POST
		{
			if(CFrameworkHelper::StringsAreEqual(sContentType, multipart_type, multipart_type.length())) // Multi-Part POST
			{
				l_sFileName += "?";
				bMultiPartForm = true;
			}
			else
			{
				if(!l_sPostInfo.empty())
				{
					l_sFileName += "?";
					l_sFileName += l_sPostInfo;
				}
			}
			//LOGGER2_DEBUG(m_sLoggerName.c_str())("Parse HTTP POST(Method 0x%x) FileName(%s) Content-Type(%s)", cMethodType, l_sFileName.c_str(), sContentType.c_str()); 
		}
		else // GET
		{
			if(!l_sQueryString.empty())
			{
				l_sFileName += "?";
				l_sFileName += l_sQueryString;
			}
			//LOGGER2_DEBUG(m_sLoggerName.c_str())("Parse HTTP GET(Method 0x%x) FileName(%s)", cMethodType, l_sFileName.c_str()); 
		}

		// Split sFileName by '/'
		std::vector<std::string> oVecStr;
		CFrameworkHelper::SpliteStrings('/', l_sFileName, oVecStr);
		if(oVecStr.size() < 2)
		{
			m_sLastErrMsg = "SpliteStringsBySlash failed for sFileName[";
			m_sLastErrMsg += l_sFileName;
			m_sLastErrMsg += "]";
			return -2;
		}

		sControllerName = oVecStr[0];

		/// Èç¹ûÊÇÕâÖÖÀàÐÍhttp://www.paipai.com/shop/add?p1=1&p2=2 ÔòsFileName = shop/add?p1=1&p2=2
		/// ÉÏÊöµÄaction = add
		std::string::size_type it2 = l_sFileName.find_first_of('?', 0);
		if(it2 != std::string::npos)
		{
			std::string::size_type it1 = l_sFileName.rfind('/', it2);
			if(it1 == std::string::npos || it1 > it2)
			{
				m_sLastErrMsg = "? before / sFileName[";
				m_sLastErrMsg += l_sFileName;
				m_sLastErrMsg += "]";
				return -3;
			}

			sActionName = std::string(l_sFileName.begin() + it1 + 1, l_sFileName.begin() + it2);

			int iRetCode = 0;
			if(bMultiPartForm)
			{
				if((iRetCode = DoMultiPartParse(l_sPostInfo, sContentType, oActionParam)) != 0)
					return iRetCode;
			}
			else
			{
				std::string l_sParamPairs = std::string(l_sFileName.begin() + it2 + 1, l_sFileName.end());
				if((iRetCode = DoStandardParse(l_sParamPairs, oActionParam)) != 0)
					return iRetCode;
			}
		}
		else
		{
			sActionName = oVecStr[1];
			std::vector<std::string>::iterator it = oVecStr.begin();
			++it; 
			++it;
			int iIndex = 0;
			char sBuf[5] = {0};
			std::string sVal;
			for(; it != oVecStr.end(); ++it)
			{
				snprintf(sBuf, sizeof(sBuf) - 1, "%d", iIndex);
				sVal = (*it);
				//CFrameworkHelper::Decode(sVal);
				CFrameworkHelper::LRTrim(sVal);
				oActionParam.AddVal(sBuf, sVal);
				++iIndex;
			}	
		}

		if(sCookies.empty())
			return 0;

		return DoParseCookie(sCookies, oActionParam);
	}

	void CActionInvocation::ParseHeader(
		const std::string& data, std::string& disposition, std::string& name, std::string& filename, std::string& type)
	{
		CFrameworkHelper::ExtractBetween(data, "Content-Disposition: ", ";", disposition);
		CFrameworkHelper::ExtractBetween(data, "name=\"", "\"", name);
		CFrameworkHelper::ExtractBetween(data, "filename=\"", "\"", filename);
		CFrameworkHelper::ExtractBetween(data, "Content-Type: ", "\r\n\r\n", type);
		
		// This is hairy: Netscape and IE don't encode the filenames
		// The RFC says they should be encoded, so I will assume they are.
		//CFrameworkHelper::Decode(filename);
	}

	int CActionInvocation::ParseMIME(const std::string& data, std::string& name, std::string& value)
	{
		// Find the header
		std::string end = "\r\n\r\n";
		std::string::size_type headLimit = data.find(end, 0);
		
		// Detect error
		if(std::string::npos == headLimit)
		{
			m_sLastErrMsg = "Malformed input";
			return -1;
		}
		
		// Extract the value - there is still a trailing CR/LF to be subtracted off
		std::string::size_type valueStart = headLimit + end.length();
		value = data.substr(valueStart, data.length() - valueStart - 2);
		
		// Parse the header - pass trailing CR/LF x 2 to parseHeader
		std::string disposition;
		std::string filename;
		std::string type;
		ParseHeader(data.substr(0, valueStart), disposition, name, filename, type);
		
		return 0;
	}

    int CActionInvocation::ParseMIME2(const std::string& data, std::string& name, std::string& value, std::string& filename)
	{
		// Find the header
		std::string end = "\r\n\r\n";
		std::string::size_type headLimit = data.find(end, 0);
		
		// Detect error
		if(std::string::npos == headLimit)
		{
			m_sLastErrMsg = "Malformed input";
			return -1;
		}
		
		// Extract the value - there is still a trailing CR/LF to be subtracted off
		std::string::size_type valueStart = headLimit + end.length();
		value = data.substr(valueStart, data.length() - valueStart - 2);
		
		// Parse the header - pass trailing CR/LF x 2 to parseHeader
		std::string disposition;
		std::string type;
		ParseHeader(data.substr(0, valueStart), disposition, name, filename, type);
		
		return 0;
	}

	int CActionInvocation::DoMultiPartParse(const std::string& sPostInfo, const std::string& sContentType, CActionParams& oActionParam)
	{
		// Find out what the separator is
		std::string bType = "boundary=";
		std::string::size_type pos = sContentType.find(bType);
		
		//        boundary=---------------------------7d89b3313109c
		// Éú³ÉÇÐ·Ö·û(eg.-----------------------------7d89b3313109c)
		std::string sep1 = sContentType.substr(pos + bType.length());
		sep1.append("\r\n");
		sep1.insert(0, "--");
		
		// Éú³É½áÊø·û(eg.-----------------------------7d89b3313109c--)
		std::string sep2 = sContentType.substr(pos + bType.length());
		sep2.append("--\r\n");
		sep2.insert(0, "--");

		/* multipart/form-data¶ÎÊý¾ÝÑÝÊ¾
		-----------------------------7d89b3313109c\r\n
		Content-Disposition: form-data; name="cAddType"\r\n
		\r\n
		2\r\n
		-----------------------------7d89b3313109c\r\n
		Content-Disposition: form-data; name="photo"; filename="C:\DSCN0651640_480.jpg"\r\n
		Content-Type: image/pjpeg\r\n
		\r\n
ÿ		?ášEExif xxx jpg content\r\n
		-----------------------------7d89b3313109c--
		*/
		
		// Find the data between the separators
		std::string::size_type start  = sPostInfo.find(sep1);
		std::string::size_type sepLen = sep1.length();
		std::string::size_type oldPos = start + sepLen;
		
		std::string name;
		std::string value;
		while(1) 
		{
			pos = sPostInfo.find(sep1, oldPos);
			
			// If sep1 wasn't found, the rest of the data is an item
			if(std::string::npos == pos)
				break;

            std::string filename;
            
			// parse the data
			if(ParseMIME2(sPostInfo.substr(oldPos, pos - oldPos), name, value, filename) != 0)
				return -1;

			oActionParam.AddVal(name, value);
            if (!filename.empty())
                oActionParam.AddVal(name + "_filename", filename);

			// update position
			oldPos = pos + sepLen;
		}
		
		// The data is terminated by sep2
		pos = sPostInfo.find(sep2, oldPos);
        
		// parse the data, if found
		if(std::string::npos != pos) 
		{
            std::string filename;
			if(ParseMIME2(sPostInfo.substr(oldPos, pos - oldPos), name, value, filename) != 0)
				return -1;

			oActionParam.AddVal(name, value);
            if (!filename.empty())
                oActionParam.AddVal(name + "_filename", filename);
		}

		return 0;
	}

	int CActionInvocation::DoStandardParse(std::string& sParamPairs, CActionParams& oActionParam)
	{
		std::vector<std::string> oVecStr;
		CFrameworkHelper::SpliteStrings('&', sParamPairs, oVecStr);
		std::vector<std::string>::iterator it = oVecStr.begin();
		std::string sKey;
		std::string sVal;
		for(; it != oVecStr.end(); ++it)
		{
			std::string& rElement = (*it);
			std::string::size_type it3 = rElement.find_first_of('=', 0);
			if(it3 == std::string::npos)
			{
				m_sLastErrMsg = "no find = in param element[";
				m_sLastErrMsg += rElement;
				m_sLastErrMsg += "] for sParamPairs[";
				m_sLastErrMsg += sParamPairs;
				m_sLastErrMsg += "]";
				//return -4;
				continue;
			}

			sKey = std::string(rElement.begin(), rElement.begin() + it3);
			sVal = std::string(rElement.begin() + it3 + 1, rElement.end());
			//CFrameworkHelper::Decode(sKey);
			//CFrameworkHelper::Decode(sVal);
			CFrameworkHelper::LRTrim(sKey);
			CFrameworkHelper::LRTrim(sVal);
			oActionParam.AddVal(sKey, sVal);
		}
		return 0;
	}

	int CActionInvocation::DoParseCookie(const std::string& sCookies, CActionParams& oActionParam)
	{
		// Parse Cookies
		////Set-Cookie: RMID=732423sdfs73242; expires=Fri, 31-Dec-2010 23:59:59 GMT; path=/; domain=.example.net
		std::vector<std::string> oVecStr;
		std::string l_sCookies = sCookies;
		//printf("COOKIE[%s]\n", l_sCookies.c_str());
		CFrameworkHelper::SpliteStrings(';', l_sCookies, oVecStr);
		std::vector<std::string>::iterator it = oVecStr.begin();
		std::string sKey;
		std::string sVal;
		for(; it != oVecStr.end(); ++it)
		{
			std::string& rElement = (*it);
			std::string::size_type it3 = rElement.find_first_of('=', 0);
			if(it3 == std::string::npos)
			{
				m_sLastErrMsg = "no find = in param element[";
				m_sLastErrMsg += rElement;
				m_sLastErrMsg += "] for sCookies[";
				m_sLastErrMsg += l_sCookies;
				m_sLastErrMsg += "]";
				//return -4;
				continue;
			}
			/*
			//check only one = should be ok
			if(rElement.find_first_of('=', it3 + 1) != std::string::npos)
			{
				m_sLastErrMsg = "too much = in param element[";
				m_sLastErrMsg += rElement;
				m_sLastErrMsg += "] for sCookies[";
				m_sLastErrMsg += l_sCookies;
				m_sLastErrMsg += "]";
				return -5;
			}
			*/
			sKey = std::string(rElement.begin(), rElement.begin() + it3);
			sVal = std::string(rElement.begin() + it3 + 1, rElement.end());
			//CFrameworkHelper::Decode(sKey);
			//CFrameworkHelper::Decode(sVal);
			CFrameworkHelper::LRTrim(sKey);
			CFrameworkHelper::LRTrim(sVal);
			oActionParam.AddCookiePair(sKey, sVal);
		}

		return 0;
	}

	int CActionInvocation::InvokeInterceptorBefore(void* pParam)
	{
		int iRetCode = 0;
		/// ´©¹ýÒ»ÏµÁÐ¶¨ÒåµÄÀ¹½ØÆ÷
		std::list<CInterceptor*>::iterator pInterceptorPos;
		for (pInterceptorPos = m_oInterceptorList.begin(); 
				pInterceptorPos != m_oInterceptorList.end(); ++pInterceptorPos)
		{
			if(*pInterceptorPos)
			{
				iRetCode = (*pInterceptorPos)->InterceptBeforeProcess(pParam);
				if(iRetCode != 0 && iRetCode != CInterceptor::OK)
					return -1;
			}
		}

		return 0;
	}

	int CActionInvocation::InvokeInterceptorAfter(void* pParam)
	{
		int iRetCode = 0;
		/// ´©¹ýÒ»ÏµÁÐ¶¨ÒåµÄÀ¹½ØÆ÷
		std::list<CInterceptor*>::iterator pInterceptorPos;
		for (pInterceptorPos = m_oInterceptorList.begin(); 
				pInterceptorPos != m_oInterceptorList.end(); ++pInterceptorPos)
		{
			if(*pInterceptorPos)
			{
				iRetCode = (*pInterceptorPos)->InterceptAfterProcess(pParam);
				if(iRetCode != 0 && iRetCode != CInterceptor::OK)
					return -1;
			}
		}

		return 0;
	}

	int CActionInvocation::InvokeInterceptor(bool bBefore)
	{
		// Do nothing
		return 0;
	}
}}



