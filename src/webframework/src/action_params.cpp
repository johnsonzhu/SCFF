
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <cctype>
#include "action_params.h"
#include "framework_helper.h"
//#include "c2cplatform/library/the3/sec/secapi_antixss.h"

using namespace std;

namespace web_solution{ namespace web_framework{
	void CActionParams::Reset()
	{
		m_oParamsMap.clear();
		m_oCookieMap.clear();
	}

	const char* CActionParams::GetLastErrMsg() const
	{
		return m_sErrMsg;
	}

	void CActionParams::Dump(std::ostream& os) 
	{
		os << "-----------------MAP------------------------" << std::endl;
		std::map<std::string, PARAM_T>::iterator it = m_oParamsMap.begin();
		for(; it != m_oParamsMap.end(); ++it)
		{
			PARAM_T& rParam = (*it).second;
			os << "Name[" << (*it).first << "]" << std::endl;
			os << "Value : " << std::endl;

			int index = 0;
			std::vector<std::string>::iterator it2 = rParam.vecVal.begin();
			for(; it2 != rParam.vecVal.end(); ++it2)
				os << "\t[" << index++ << "][" << (*it2) << "]" << std::endl;

		}
		os << "-----------------COOKIE----------------------" << std::endl;
		std::map<std::string, COOKIE_T>::iterator it3 = m_oCookieMap.begin();
		for(; it3 != m_oCookieMap.end(); ++it3)
		{
			COOKIE_T& rCookie = (*it3).second;
			os << "Name[" << (*it3).first << "] Value[" << rCookie.sVal << "] Set[" << (rCookie.bSet ? "true" : "false") << "]" << std::endl;
		}
		os << "-----------------END------------------------" << std::endl;
	}

	// Invocation对象调用,传入的Name和Val都是Escape过的数据
	void CActionParams::AddCookiePair(const std::string& sName, const std::string& sVal)
	{
		COOKIE_T stCookie;
		stCookie.sVal = sVal;
		stCookie.bSet = false; 
		m_oCookieMap[sName] = stCookie;
	}

	// Invocation对象调用,传入的Name和Val都是未Escape过的数据
	void CActionParams::AddVal(const std::string& sName, const std::string& sVal)
	{
		std::map<std::string, PARAM_T>::iterator it = m_oParamsMap.find(sName);
		
		if(it != m_oParamsMap.end())
		{
			PARAM_T& rParam = (*it).second;
			rParam.vecVal.push_back(sVal);
		}
		else
		{
			PARAM_T stParam;
			stParam.vecVal.push_back(sVal);
			m_oParamsMap[sName] = stParam;
		}
	}

	// 业务对象调用,传入的Name是Unescape过的数据
	int CActionParams::GetParam(const std::string& sName, PARAM_T** ppParam) const
	{
		if(sName.empty())
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "cgi参数名为空");
			return -1;
		}

		std::string l_sName = sName;
		std::map<std::string, PARAM_T>::const_iterator it = m_oParamsMap.find(l_sName);
		if(it == m_oParamsMap.end())
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "没有找到对应的值对 Key[%s]", sName.c_str());
			return -2;
		}	
		
		*ppParam = const_cast<PARAM_T*>(&((*it).second));

		return 0;
	}

	int CActionParams::GetCookieVal(const std::string& sName, std::string& sVal) const
	{
		std::map<std::string, COOKIE_T>::const_iterator it = m_oCookieMap.find(sName);
		if(it == m_oCookieMap.end())
		{
			sVal = "";
			return -1;
		}

		const COOKIE_T& rCookie = (*it).second;
		sVal = rCookie.sVal;
		
		return 0;
	}
	
	int CActionParams::GetCookieVal(const std::string& sName, int8_t& cVal) const
	{
		std::string sVal;
		int iRetCode = GetCookieVal(sName, sVal);
		if(iRetCode != 0)
		{
			cVal = 0;
			return -1;
		}
		if(!IsNumber(sVal))
			return -99;
		
		cVal = static_cast<int8_t>(strtol(sVal.c_str(), NULL, 10));
		return 0;
	}
	int CActionParams::GetCookieVal(const std::string& sName, int16_t& wVal) const
	{
		std::string sVal;
		int iRetCode = GetCookieVal(sName, sVal);
		if(iRetCode != 0)
		{
			wVal = 0;
			return -1;
		}
		if(!IsNumber(sVal))
			return -99;
		
		wVal = static_cast<int16_t>(strtol(sVal.c_str(), NULL, 10));
		return 0;
	}
	int CActionParams::GetCookieVal(const std::string& sName, int32_t& dwVal) const
	{
		std::string sVal;
		int iRetCode = GetCookieVal(sName, sVal);
		if(iRetCode != 0)
		{
			dwVal = 0;
			return -1;
		}
		if(!IsNumber(sVal))
			return -99;
		
		dwVal = static_cast<int32_t>(strtol(sVal.c_str(), NULL, 10));
		return 0;
	}
	int CActionParams::GetCookieVal(const std::string& sName, int64_t& ddwVal) const
	{
		std::string sVal;
		int iRetCode = GetCookieVal(sName, sVal);
		if(iRetCode != 0)
		{
			ddwVal = 0;
			return -1;
		}
		if(!IsNumber(sVal))
			return -99;
		
		ddwVal = static_cast<int64_t>(strtoll(sVal.c_str(), NULL, 10));
		return 0;
	}

	int CActionParams::GetRawVal(const std::string& sName, std::string& sVal,
			uint32_t dwMaxLen, uint32_t dwMinLen, const char *pszPrompt, char sErrMsg[]) const
	{
		PARAM_T* pParam = NULL;
		if(GetParam(sName, &pParam) != 0)
		{
			sVal = "";
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "找不到 %s 字段", pszPrompt);
				if(sErrMsg)
					sprintf(sErrMsg, "找不到 %s 字段", pszPrompt);
			}
			else
			{					
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "找不到 %s 字段", sName.c_str());
				if(sErrMsg)
					sprintf(sErrMsg, "找不到 %s 字段", sName.c_str());
			}
			return -1;
		}
		assert(pParam);
		if(pParam->vecVal.empty())
		{
			sVal = "";
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段内容为空", pszPrompt);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段内容为空", pszPrompt);
			}
			else
			{					
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段内容为空", sName.c_str());
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段内容为空", sName.c_str());
			}
			return -2;
		}
		sVal = pParam->vecVal.front();

		if(dwMinLen > 0 && sVal.length() < dwMinLen)
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段输入太少，不能少于 %u 字节", pszPrompt, dwMinLen);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段输入太少，不能少于 %u 字节", pszPrompt, dwMinLen);
			}
			else
			{	
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "字段输入太少，不能少于 %u 字节", dwMinLen);
				if(sErrMsg)
					sprintf(sErrMsg, "字段输入太少，不能少于 %u 字节", dwMinLen);
						
			}

			return -3;
		}

		if(dwMaxLen > 0 && sVal.length() > dwMaxLen)
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段输入太多，不能多于等于 %u 字节", pszPrompt, dwMaxLen);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段输入太多，不能多于等于 %u 字节", pszPrompt, dwMaxLen);
			}
			else
			{	
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "字段输入太多，不能多于等于 %u 字节", dwMaxLen);
				if(sErrMsg)
					sprintf(sErrMsg, "字段输入太多，不能多于等于 %u 字节", dwMaxLen);
			}
			return -6;
		}

		return 0;
	}

	// 去掉非法字符和半个汉字
	std::string CActionParams::CleanChineseString(const char *pszRawString, uint32_t dwLength) const
	{
            const char flag1 = '"';
            const char flag2[3] = "＂";
            const char flag3 = '\\';
            const char flag4[3] = "、";

            std::string sTmpBuffer;
            for(uint32_t i = 0; i < dwLength; i++)
            {       
                if(CFrameworkHelper::ISGBKLEAD(pszRawString[i]))
                {       
                    if(i == dwLength - 1)
                        break;  

                    if(CFrameworkHelper::ISGBKNEXT(pszRawString[i+1]))
                    {       
                        sTmpBuffer += pszRawString[i];
                        i++;    
                    }       
                    else
                        continue;
                } 
                else if(!isprint(pszRawString[i]) && (pszRawString[i] != '\r' && pszRawString[i] != '\n'))
                {
                    continue;
                }
                else    
                {       
                    if(flag1 == pszRawString[i])
                    {       
                        sTmpBuffer += flag2[0];
                        sTmpBuffer += flag2[1];
                        continue;
                    }       
                    if(flag3 == pszRawString[i])
                    {
                        sTmpBuffer += flag4[0];
                        sTmpBuffer += flag4[1];
                        continue;
                    }
                }

                sTmpBuffer += pszRawString[i];
            }

            return sTmpBuffer;
        }

        int CActionParams::GetVal(const std::string& sName, std::string& sVal,
                uint32_t dwMaxLen, uint32_t dwMinLen, const char *pszPrompt, char sErrMsg[]) const
        {
            PARAM_T* pParam = NULL;
            if(GetParam(sName, &pParam) != 0)
            {
			sVal = "";
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "找不到 %s 字段", pszPrompt);
				if(sErrMsg)
					sprintf(sErrMsg, "找不到 %s 字段", pszPrompt);
			}
			else
			{					
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "找不到 %s 字段", sName.c_str());
				if(sErrMsg)
					sprintf(sErrMsg, "找不到 %s 字段", sName.c_str());
			}
			return -1;
		}
		assert(pParam);
		if(pParam->vecVal.empty())
		{
			sVal = "";
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段内容为空", pszPrompt);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段内容为空", pszPrompt);
			}
			else
			{					
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段内容为空", sName.c_str());
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段内容为空", sName.c_str());
			}
			return -2;
		}
		sVal = pParam->vecVal.front();

		if(dwMinLen > 0 && sVal.length() < dwMinLen)
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段输入太少，不能少于 %u 字节", pszPrompt, dwMinLen);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段输入太少，不能少于 %u 字节", pszPrompt, dwMinLen);
			}
			else
			{	
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "字段输入太少，不能少于 %u 字节", dwMinLen);
				if(sErrMsg)
					sprintf(sErrMsg, "字段输入太少，不能少于 %u 字节", dwMinLen);
						
			}

			return -3;
		}

		if(dwMinLen > 0 || dwMaxLen > 0) // 非二进制数据,上层希望过滤一些黑客字符
		{
			sVal = CleanChineseString(sVal.c_str(), sVal.length());
		}

		if(dwMaxLen > 0 && sVal.length() > dwMaxLen)
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段输入太多，不能多于 %u 字节", pszPrompt, dwMaxLen);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段输入太多，不能多于 %u 字节", pszPrompt, dwMaxLen);
			}
			else
			{	
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "字段输入太多，不能多于 %u 字节", dwMaxLen);
				if(sErrMsg)
					sprintf(sErrMsg, "字段输入太多，不能多于 %u 字节", dwMaxLen);
			}
			return -6;
		}

		return 0;
	}


	int CActionParams::GetVal(const std::string& sName, std::string& sVal, bool bNeedAntiXss,
			uint32_t dwMaxLen, uint32_t dwMinLen, const char *pszPrompt, char sErrMsg[]) const
	{
		int iRetCode = GetVal(sName, sVal, dwMaxLen, dwMinLen, pszPrompt, sErrMsg);
		if (iRetCode) return iRetCode;
		if (bNeedAntiXss) iRetCode = DoAntiXss(sVal);
		return iRetCode;
	}

	int CActionParams::GetVal(const std::string& sName, std::string& sVal, bool bNeedAntiXss,
			const std::string &sConf, uint32_t dwMaxLen, uint32_t dwMinLen, 
			const char *pszPrompt, char sErrMsg[]) const
	{
		int iRetCode = GetVal(sName, sVal, dwMaxLen, dwMinLen, pszPrompt, sErrMsg);
		if (iRetCode) return iRetCode;
		if (bNeedAntiXss) 
		{
			if(sConf.empty())
			{
				sErrMsg = "Empty conf file!";
				return -1;
			}
			iRetCode = DoAntiXss(sVal, sConf);
		}
		return iRetCode;
	}

	int CActionParams::DoAntiXss(std::string& sInput) const
	{
		/*
		const std::string sConfFile = "/usr/local/c2c/bin/secapi/global.conf";
	    int iRetCode = 0;
	    try
	    {
	    	secapi::AntiXss oAntiXss;
			oAntiXss.Initialize(sConfFile);
			oAntiXss.SetCustomIP("");
			std::string sTmp = oAntiXss.JavaScriptEncode(sInput);
			iRetCode= oAntiXss.GetLastFilterFlag();
	        if(iRetCode > 0)
	        {
	        	sInput = sTmp;
	        	return 0;
	        }
	    } 
	    catch(secapi::antixss_error& e)
	    {   
			return -1;
	    }
		*/
		return 0;
	}

	int CActionParams::DoAntiXss(std::string& sInput, const std::string &sConf) const
	{
		/*
		const std::string sConfFile = sConf;
	    int iRetCode = 0;
	    try
	    {
	    	secapi::AntiXss oAntiXss;
			oAntiXss.Initialize(sConfFile);
			oAntiXss.SetCustomIP("");
			std::string sTmp = oAntiXss.FilterAllActiveContent(sInput);
			iRetCode= oAntiXss.GetLastFilterFlag();
	        if(iRetCode > 0)
	        {
	        	sInput = sTmp;
	        	return iRetCode;
	        }
	    } 
	    catch(secapi::antixss_error& e)
	    {   
			return -1;
	    }
		*/
		return 0;
	}

	//scott adds this
	int CActionParams::GetVal(const std::string& sName, char* psValBuf, uint32_t dwValBufLen,
			uint32_t dwMaxLen, uint32_t dwMinLen, const char *pszPrompt, char sErrMsg[]) const
	{
		assert(psValBuf);
		assert(dwValBufLen > 0);

		int iRetCode = 0;

		std::string strVal;

		if((iRetCode = GetVal(sName, strVal, dwMaxLen, dwMinLen, pszPrompt, sErrMsg)) != 0)
			return iRetCode;

		snprintf(psValBuf, dwValBufLen, "%s", strVal.c_str());
		return iRetCode;
	}

	int CActionParams::GetIntMoney(const std::string& sName, uint32_t& dwMoney, uint32_t dwMax, uint32_t dwMin, const char *pszPrompt, char sErrMsg[]) const
	{
		std::string sMoney;
		int iRetCode = 0;
		if((iRetCode = GetVal(sName, sMoney)) != 0)
			return iRetCode;

		if(sMoney.empty())	
			return -88;

		if(!IsMoney(sMoney))
			return -99;

		const static int       TRANSFER_MONEY_MULTIPLE 	= 100;	///>c2c网站与运营支持的货币单位换算培数
		const static uint32_t  UINT_NOT_NEED_CHECK     	= 0;	///>不对整型数字大小进行验证

		//处理四舍五入
		uint64_t ddwTmp = (uint64_t)((atof(sMoney.c_str()) + 0.005) * TRANSFER_MONEY_MULTIPLE);
		if(ddwTmp > 0x7FFFFFFF)
		{   
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能大于%d ", pszPrompt, 0x7FFFFFFF);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 输入有误，不能大于%d ", pszPrompt, 0x7FFFFFFF);
			}
			else
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能大于%d ", sName.c_str(), 0x7FFFFFFF);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 输入有误，不能大于%d ", sName.c_str(), 0x7FFFFFFF);
			}
			
			return -11;
		}
		//检查参数的是否在最大值和最小值的区间内    
		if (dwMin != UINT_NOT_NEED_CHECK)
		{
			if (ddwTmp < dwMin)
			{
				CActionParams& rThis = const_cast<CActionParams&>(*this);
				if(pszPrompt)
				{
					snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能小于%d ", pszPrompt, dwMin);
					if(sErrMsg)
						sprintf(sErrMsg, "%s 输入有误，不能小于%d ", pszPrompt, dwMin);
				}
				else
				{
					snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能小于%d ", pszPrompt, dwMin);
					if(sErrMsg)
						sprintf(sErrMsg, "%s 输入有误，不能小于%d ", pszPrompt, dwMin);
				}
			
				return -9;
			}
		}
		if (dwMax != UINT_NOT_NEED_CHECK)
		{
			if (ddwTmp > dwMax)
			{
				CActionParams& rThis = const_cast<CActionParams&>(*this);
				if(pszPrompt)
				{
					snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能大于%d ", pszPrompt, dwMax);
					if(sErrMsg)
						sprintf(sErrMsg, "%s 输入有误，不能大于%d ", pszPrompt, dwMax);
				}
				else
				{
					snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能大于%d ", pszPrompt, dwMax);
					if(sErrMsg)
						sprintf(sErrMsg, "%s 输入有误，不能大于%d ", pszPrompt, dwMax);
				}
				
				return -10;
			}
		}
		dwMoney = (uint32_t)(ddwTmp & 0xFFFFFFFF);

		return 0;
	}

/*
	int CActionParams::GetVal(const std::string& sName, uint8_t& cVal) const 
	{
		std::string sVal;
		int iRetCode = GetVal(sName, sVal);
		if(iRetCode != 0)
		{
			cVal = 0;
			return iRetCode;
		}
		if(!IsNumber(sVal))
			return -99;
		cVal = static_cast<uint8_t>(strtoul(sVal.c_str(), NULL, 10));
		return 0;
	}

	int CActionParams::GetVal(const std::string& sName, uint16_t& wVal) const 
	{
		std::string sVal;
		int iRetCode = GetVal(sName, sVal);
		if(iRetCode != 0)
		{
			wVal = 0;
			return iRetCode;
		}
		if(!IsNumber(sVal))
			return -99;
		wVal = static_cast<uint16_t>(strtoul(sVal.c_str(), NULL, 10));
		return 0;
	}
	*/

	int CActionParams::GetVal(const std::string& sName, uint32_t& dwVal, 
			uint32_t dwMax, uint32_t dwMin, const char *pszPrompt, char sErrMsg[]) const
	{
		std::string sVal;
		int iRetCode = GetVal(sName, sVal);
		if(iRetCode != 0)
		{
			dwVal = 0;
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "找不到 %s 字段", pszPrompt);
				if(sErrMsg)
					sprintf(sErrMsg, "找不到 %s 字段", pszPrompt);
			}
			else
			{					
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "找不到 %s 字段", sName.c_str());
				if(sErrMsg)
					sprintf(sErrMsg, "找不到 %s 字段", sName.c_str());
			}
			return iRetCode;
		}
		if(!IsNumber(sVal))
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段内容不是数字", pszPrompt);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段内容不是数字", pszPrompt);
			}
			else
			{					
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段内容不是数字", sName.c_str());
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段内容不是数字", sName.c_str());
			}
			return -99;
		}
		dwVal = static_cast<uint32_t>(strtoul(sVal.c_str(), NULL, 10));

		if(dwMax > 0 && dwVal > dwMax)
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能大于%u ", pszPrompt, dwMax);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 输入有误，不能大于%u ", pszPrompt, dwMax);
			}
			else
			{	
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "输入有误，不能大于%u ", dwMax);
				if(sErrMsg)
					sprintf(sErrMsg, "输入有误，不能大于%u ", dwMax);
				
			}	
			return -1;
		}
		if(dwMin > 0 && dwVal < dwMin)
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能小于%u ", pszPrompt, dwMin);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 输入有误，不能小于%u ", pszPrompt, dwMin);
			}
			else
			{	
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "输入有误，不能大于%u ", dwMin);
				if(sErrMsg)
					sprintf(sErrMsg, "输入有误，不能大于%u ", dwMin);
			}
			return -2;
		}
		return 0;
	}


	//scott adds this
	int CActionParams::GetVal(const std::string& sName, uint64_t& ddwVal, 
			uint64_t ddwMax, uint64_t ddwMin, const char *pszPrompt, char sErrMsg[]) const
	{
		std::string sVal;
		int iRetCode = GetVal(sName, sVal);
		if(iRetCode != 0)
		{
			ddwVal = 0;
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "找不到 %s 字段", pszPrompt);
				if(sErrMsg)
					sprintf(sErrMsg, "找不到 %s 字段", pszPrompt);
			}
			else
			{					
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "找不到 %s 字段", sName.c_str());
				if(sErrMsg)
					sprintf(sErrMsg, "找不到 %s 字段", sName.c_str());
			}
			return iRetCode;
		}
		if(!IsNumber(sVal))
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段内容不是数字", pszPrompt);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段内容不是数字", pszPrompt);
			}
			else
			{					
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段内容不是数字", sName.c_str());
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段内容不是数字", sName.c_str());
			}
			return -99;
		}
		ddwVal = static_cast<uint64_t>(strtoull(sVal.c_str(), NULL, 10));

		if(ddwMax > 0 && ddwVal > ddwMax)
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能大于%liu ", pszPrompt, ddwMax);
#else
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能大于%llu ", pszPrompt, ddwMax);
#endif
				if(sErrMsg)
				{
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
					sprintf(sErrMsg, "%s 输入有误，不能大于%liu ", pszPrompt, ddwMax);
#else
					sprintf(sErrMsg, "%s 输入有误，不能大于%llu ", pszPrompt, ddwMax);
#endif
				}
			}
			else
			{	
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "输入有误，不能大于%liu ", ddwMax);
#else
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "输入有误，不能大于%llu ", ddwMax);
#endif
				if(sErrMsg)
				{
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
					sprintf(sErrMsg, "输入有误，不能大于%liu ", ddwMax);
#else
					sprintf(sErrMsg, "输入有误，不能大于%llu ", ddwMax);
#endif
				}
				
			}	
			return -1;
		}
		if(ddwMin > 0 && ddwVal < ddwMin)
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能小于%liu ", pszPrompt, ddwMin);
#else
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能小于%llu ", pszPrompt, ddwMin);
#endif
				if(sErrMsg)
				{
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
					sprintf(sErrMsg, "%s 输入有误，不能小于%liu ", pszPrompt, ddwMin);
#else
					sprintf(sErrMsg, "%s 输入有误，不能小于%llu ", pszPrompt, ddwMin);
#endif
				}
			}
			else
			{	
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "输入有误，不能大于%liu ", ddwMin);
#else
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "输入有误，不能大于%llu ", ddwMin);
#endif
				if(sErrMsg)
				{
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
					sprintf(sErrMsg, "输入有误，不能大于%liu ", ddwMin);
#else
					sprintf(sErrMsg, "输入有误，不能大于%llu ", ddwMin);
#endif
				}
			}
			return -2;
		}
		return 0;
	}


	//scott adds this
	int CActionParams::GetVal(const std::string& sName, uint8_t& cVal, 			
			uint8_t cMax, uint8_t cMin, const char *pszPrompt, char sErrMsg[]) const
	{
		uint32_t dwVal = 0;
		int iRetCode = GetVal(sName, dwVal, (uint32_t)cMax, (uint32_t)cMin, pszPrompt, sErrMsg);
		if(iRetCode != 0)
		{
			cVal = 0;
			return iRetCode;
		}

		cVal = dwVal;

		return iRetCode;
	}


	//scott adds this
	int CActionParams::GetVal(const std::string& sName, uint16_t& wVal, 			
			uint16_t wMax, uint16_t wMin, const char *pszPrompt, char sErrMsg[]) const
	{
		uint32_t dwVal = 0;
		int iRetCode = GetVal(sName, dwVal, (uint32_t)wMax, (uint32_t)wMin, pszPrompt, sErrMsg);
		if(iRetCode != 0)
		{
			wVal = 0;
			return iRetCode;
		}

		wVal = dwVal;

		return iRetCode;
	}

	//scott adds this
	int CActionParams::GetVal(const std::string& sName, int8_t& cVal, 			
			int8_t cMax, int8_t cMin, const char *pszPrompt, char sErrMsg[]) const
	{
		int32_t dwVal = 0;
		int iRetCode = GetVal(sName, dwVal, (int32_t)cMax, (int32_t)cMin, pszPrompt, sErrMsg);
		if(iRetCode != 0)
		{
			cVal = 0;
			return iRetCode;
		}

		cVal = dwVal;

		return iRetCode;
	}


	//scott adds this
	int CActionParams::GetVal(const std::string& sName, int16_t& wVal, 			
			int16_t wMax, int16_t wMin, const char *pszPrompt, char sErrMsg[]) const
	{
		int32_t dwVal = 0;
		int iRetCode = GetVal(sName, dwVal, (int32_t)wMax, (int32_t)wMin, pszPrompt, sErrMsg);
		if(iRetCode != 0)
		{
			wVal = 0;
			return iRetCode;
		}

		wVal = dwVal;

		return iRetCode;
	}


	//scott adds this
	int CActionParams::GetVal(const std::string& sName, int32_t& iVal, 
			int32_t iMax, int32_t iMin, const char *pszPrompt, char sErrMsg[]) const
	{
		std::string sVal;
		int iRetCode = GetVal(sName, sVal);
		if(iRetCode != 0)
		{
			iVal = 0;
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "找不到 %s 字段", pszPrompt);
				if(sErrMsg)
					sprintf(sErrMsg, "找不到 %s 字段", pszPrompt);
			}
			else
			{					
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "找不到 %s 字段", sName.c_str());
				if(sErrMsg)
					sprintf(sErrMsg, "找不到 %s 字段", sName.c_str());
			}
			return iRetCode;
		}
		if(!IsNumber(sVal))
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段内容不是数字", pszPrompt);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段内容不是数字", pszPrompt);
			}
			else
			{					
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段内容不是数字", sName.c_str());
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段内容不是数字", sName.c_str());
			}
			return -99;
		}
		iVal = static_cast<int32_t>(strtol(sVal.c_str(), NULL, 10));

		if(iVal > iMax)
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能大于%d ", pszPrompt, iMax);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 输入有误，不能大于%d ", pszPrompt, iMax);
			}
			else
			{	
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "输入有误，不能大于%d ", iMax);
				if(sErrMsg)
					sprintf(sErrMsg, "输入有误，不能大于%d ", iMax);
				
			}	
			return -1;
		}
		if(iVal < iMin)
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能小于%d ", pszPrompt, iMin);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 输入有误，不能小于%d ", pszPrompt, iMin);
			}
			else
			{	
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "输入有误，不能大于%d ", iMin);
				if(sErrMsg)
					sprintf(sErrMsg, "输入有误，不能大于%d ", iMin);
			}
			return -2;
		}
		return 0;
	}


	//scott adds this
	int CActionParams::GetVal(const std::string& sName, int64_t& ddwVal, 
			int64_t ddwMax, int64_t ddwMin, const char *pszPrompt, char sErrMsg[]) const
	{
		std::string sVal;
		int iRetCode = GetVal(sName, sVal);
		if(iRetCode != 0)
		{
			ddwVal = 0;
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "找不到 %s 字段", pszPrompt);
				if(sErrMsg)
					sprintf(sErrMsg, "找不到 %s 字段", pszPrompt);
			}
			else
			{					
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "找不到 %s 字段", sName.c_str());
				if(sErrMsg)
					sprintf(sErrMsg, "找不到 %s 字段", sName.c_str());
			}
			return iRetCode;
		}
		if(!IsNumber(sVal))
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段内容不是数字", pszPrompt);
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段内容不是数字", pszPrompt);
			}
			else
			{					
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 字段内容不是数字", sName.c_str());
				if(sErrMsg)
					sprintf(sErrMsg, "%s 字段内容不是数字", sName.c_str());
			}
			return -99;
		}
		ddwVal = static_cast<int64_t>(strtoll(sVal.c_str(), NULL, 10));

		if(ddwVal > ddwMax)
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能大于%lid ", pszPrompt, ddwMax);
#else
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能大于%lld ", pszPrompt, ddwMax);
#endif
				if(sErrMsg)
				{
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
					sprintf(sErrMsg, "%s 输入有误，不能大于%lid ", pszPrompt, ddwMax);
#else
					sprintf(sErrMsg, "%s 输入有误，不能大于%lld ", pszPrompt, ddwMax);
#endif
				}
			}
			else
			{	
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "输入有误，不能大于%lid ", ddwMax);
#else
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "输入有误，不能大于%lld ", ddwMax);
#endif
				if(sErrMsg)
				{
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
					sprintf(sErrMsg, "输入有误，不能大于%lid ", ddwMax);
#else
					sprintf(sErrMsg, "输入有误，不能大于%lld ", ddwMax);
#endif
				}
			}	
			return -1;
		}
		if(ddwVal < ddwMin)
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			if(pszPrompt)
			{
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能小于%lid ", pszPrompt, ddwMin);
#else
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s 输入有误，不能小于%lld ", pszPrompt, ddwMin);
#endif
				if(sErrMsg)
				{
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
					sprintf(sErrMsg, "%s 输入有误，不能小于%lid ", pszPrompt, ddwMin);
#else
					sprintf(sErrMsg, "%s 输入有误，不能小于%lld ", pszPrompt, ddwMin);
#endif
				}
			}
			else
			{	
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "输入有误，不能大于%lid ", ddwMin);
#else
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "输入有误，不能大于%lld ", ddwMin);
#endif
				if(sErrMsg)
				{
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)  
					sprintf(sErrMsg, "输入有误，不能大于%lid ", ddwMin);
#else
					sprintf(sErrMsg, "输入有误，不能大于%lld ", ddwMin);
#endif
				}
			}
			return -2;
		}
		return 0;
	}


/*
	int CActionParams::GetVal(const std::string& sName, uint64_t& ddwVal) const 
	{
		std::string sVal;
		int iRetCode = GetVal(sName, sVal);
		if(iRetCode != 0)
		{
			ddwVal = 0;
			return iRetCode;
		}
		if(!IsNumber(sVal))
			return -99;
		ddwVal = static_cast<uint64_t>(strtoull(sVal.c_str(), NULL, 10));
		return 0;
	}
	*/

	int CActionParams::GetVal(const std::string& sName, std::vector<std::string>& vecVal) const
	{
		PARAM_T* pParam = NULL;
		if(GetParam(sName, &pParam) != 0)
		{
			return -1;
		}
		assert(pParam);
		vecVal = pParam->vecVal;
		return 0;
	}

	void CActionParams::StringReplace(std::string & strBig, const std::string & strSrc, const std::string &strDst) 
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

/*
	int CActionParams::GetVal(const std::string& sName, int8_t& cVal) const 
	{
		std::string sVal;
		int iRetCode = GetVal(sName, sVal);
		if(iRetCode != 0)
		{
			cVal = 0;
			return iRetCode;
		}
		if(!IsNumber(sVal))
			return -99;
		cVal = static_cast<uint8_t>(strtol(sVal.c_str(), NULL, 10));
		return 0;
	}

	int CActionParams::GetVal(const std::string& sName, int16_t& wVal) const 
	{
		std::string sVal;
		int iRetCode = GetVal(sName, sVal);
		if(iRetCode != 0)
		{
			wVal = 0;
			return iRetCode;
		}
		if(!IsNumber(sVal))
			return -99;
		wVal = static_cast<uint16_t>(strtol(sVal.c_str(), NULL, 10));
		return 0;
	}

	int CActionParams::GetVal(const std::string& sName, int32_t& dwVal) const 
	{
		std::string sVal;
		int iRetCode = GetVal(sName, sVal);
		if(iRetCode != 0)
		{
			dwVal = 0;
			return iRetCode;
		}
		if(!IsNumber(sVal))
			return -99;
		dwVal = static_cast<uint32_t>(strtol(sVal.c_str(), NULL, 10));
		return 0;
	}

	int CActionParams::GetVal(const std::string& sName, int64_t& ddwVal) const 
	{
		std::string sVal;
		int iRetCode = GetVal(sName, sVal);
		if(iRetCode != 0)
		{
			ddwVal = 0;
			return iRetCode;
		}
		if(!IsNumber(sVal))
			return -99;
		ddwVal = static_cast<uint64_t>(strtoll(sVal.c_str(), NULL, 10));
		return 0;
	}
	*/

	int CActionParams::GetCookieVal(const std::string& sName, uint8_t& cVal) const
	{
		std::string sVal;
		int iRetCode = GetCookieVal(sName, sVal);
		if(iRetCode != 0)
		{
			cVal = 0;
			return -1;
		}
		if(!IsNumber(sVal))
			return -99;
		
		cVal = static_cast<uint8_t>(strtoul(sVal.c_str(), NULL, 10));
		return 0;
	}
	int CActionParams::GetCookieVal(const std::string& sName, uint16_t& wVal) const
	{
		std::string sVal;
		int iRetCode = GetCookieVal(sName, sVal);
		if(iRetCode != 0)
		{
			wVal = 0;
			return -1;
		}
		if(!IsNumber(sVal))
			return -99;
		
		wVal = static_cast<uint16_t>(strtoul(sVal.c_str(), NULL, 10));
		return 0;
	}
	int CActionParams::GetCookieVal(const std::string& sName, uint32_t& dwVal) const
	{
		std::string sVal;
		int iRetCode = GetCookieVal(sName, sVal);
		if(iRetCode != 0)
		{
			dwVal = 0;
			return -1;
		}

		char szBuff[64] = {0};
		snprintf(szBuff, sizeof(szBuff), "%s", sVal.c_str());
		char* pszForValue = szBuff;
		//去掉前的o, uin=o0088881336（其中88881336为当前登录了QQ工具条的QQ号码）
		if(szBuff[0] == 'o')
		{
			pszForValue++;
			if('\0' == *pszForValue)
				return -98;
		}	

		//去掉前的零字符
		while(*pszForValue == '0' && *pszForValue != '\0')
			pszForValue++;

		//检查数是否越界
		uint32_t dwLen = strlen(pszForValue);
		if(dwLen > 10)
			return -2;

		if(!IsNumber(pszForValue))
			return -99;

		dwVal = static_cast<uint32_t>(strtoul(pszForValue, NULL, 10));
		return 0;
	}
	int CActionParams::GetCookieVal(const std::string& sName, uint64_t& ddwVal) const
	{
		std::string sVal;
		int iRetCode = GetCookieVal(sName, sVal);
		if(iRetCode != 0)
		{
			ddwVal = 0;
			return -1;
		}
		if(!IsNumber(sVal))
			return -99;
		
		ddwVal = static_cast<uint64_t>(strtoull(sVal.c_str(), NULL, 10));
		return 0;
	}

	bool CActionParams::IsNumber(const std::string& sVal) const
	{       
		if(sVal.empty())
		{
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "字符串为空");
			return false;
		}	

		for(std::string::const_iterator it = sVal.begin(); it != sVal.end(); ++it)
		{   
			if(0 == isdigit(*it))
			{
				CActionParams& rThis = const_cast<CActionParams&>(*this);
				snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "字符串不是数字");
				return false;
			}	
		}   
		return true;
	}         

	//检查字符串是否货币
	bool CActionParams::IsMoney(const std::string& sMoney) const
	{
		for(std::string::const_iterator it = sMoney.begin(); it != sMoney.end(); ++it)
		{
			if(!(isdigit(*it) != 0 || (*it) == '.'))
				return false;
		}   
		return true;
	}

	bool CActionParams::IsPostCode(const std::string& sPostCode) const
	{       
		if(sPostCode.length() != 6)
		{   
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s","邮政编码长度有误，必须等于6");
			return false;
		}       
		if (!IsNumber(sPostCode))
		{       
			CActionParams& rThis = const_cast<CActionParams&>(*this);
			snprintf(rThis.m_sErrMsg, sizeof(rThis.m_sErrMsg), "%s","邮政编码有误，必须由数字组成");
			return false;
		}       

		return true;
	}           

}}

