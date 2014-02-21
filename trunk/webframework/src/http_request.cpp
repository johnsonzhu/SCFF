/**
 * @brief Http请求类及其序列化辅助类
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.03.21
 */
#include "http_request.h"
#include "comm_def.h"

#include <iostream>
using namespace std;

namespace web_solution{ namespace comm{
    bool CSOHttpRequest::Serialize(CByteStreamNetwork& bs) 
    {
        bs & m_pSerializeObject->m_cProtocolVersion;

        bs & m_pSerializeObject->m_dwSeqNo;
        bs & m_pSerializeObject->m_dwConnectionId;
        bs & m_pSerializeObject->m_dwRequestTime;
        bs & m_pSerializeObject->m_cMethodType;
        bs & m_pSerializeObject->m_cProtocol;
        bs & m_pSerializeObject->m_sUrl;
        bs & m_pSerializeObject->m_sAcceptEncoding;
        bs & m_pSerializeObject->m_sAcceptLanguage;
        bs & m_pSerializeObject->m_cConnection;
        bs & m_pSerializeObject->m_sCookie;
        bs & m_pSerializeObject->m_sHost;
        bs & m_pSerializeObject->m_sFileName;
        bs & m_pSerializeObject->m_sReferer;
        bs & m_pSerializeObject->m_dwIfModifySince;
        bs & m_pSerializeObject->m_sUserAgent;
        bs & m_pSerializeObject->m_sClientIp;
        bs & m_pSerializeObject->m_dwClientPort;
        bs & m_pSerializeObject->m_sNoneMatchInfo;
        bs & m_pSerializeObject->m_sQueryString;
        bs & m_pSerializeObject->m_sPostInfo;
        bs & m_pSerializeObject->m_cFrontServerType;
        bs & m_pSerializeObject->m_sPathInfo;
        bs & m_pSerializeObject->m_dwContentLength;
        bs & m_pSerializeObject->m_sAccept;

        return bs.isGood();
    }

    void CSOHttpRequest::Dump(std::ostream& os)
    {
        os << "ProtocolVersion(uint8_t) : [" << m_pSerializeObject->m_cProtocolVersion << "]" << std::endl; 

	    os << "SeqNo(uint32_t) : [" << m_pSerializeObject->m_dwSeqNo << "]" << std::endl; 
	    os << "ConnectionId(uint32_t) : [" << m_pSerializeObject->m_dwConnectionId << "]" << std::endl; 
	    os << "RequestTime(uint32_t) : [" << m_pSerializeObject->m_dwRequestTime << "]" << std::endl; 
	    os << "MethodType(uint8_t) : [" << (int)m_pSerializeObject->m_cMethodType << "]" << std::endl; 
	    os << "Procotol(uint8_t) : [" << (int)m_pSerializeObject->m_cProtocol << "]" << std::endl; 
	    os << "Url(std::string) : [" << m_pSerializeObject->m_sUrl << "]" << std::endl; 
	    os << "AcceptEncoding(std::string) : [" << m_pSerializeObject->m_sAcceptEncoding << "]" << std::endl; 
	    os << "AcceptLanguage(std::string) : [" << m_pSerializeObject->m_sAcceptLanguage << "]" << std::endl; 
	    os << "Connection(uint8_t) : [" << (int)m_pSerializeObject->m_cConnection << "]" << std::endl; 
	    os << "Cookie(std::string) : [" << m_pSerializeObject->m_sCookie << "]" << std::endl; 
	    os << "Host(std::string) : [" << m_pSerializeObject->m_sHost << "]" << std::endl; 
        os << "FileName(std::string) : [" << m_pSerializeObject->m_sFileName << "]" << std::endl; 
	    os << "Referer(std::string) : [" << m_pSerializeObject->m_sReferer << "]" << std::endl; 
	    os << "IfModifySince(uint32_t) : [" << m_pSerializeObject->m_dwIfModifySince << "]" << std::endl; 
	    os << "UserAgent(std::string) : [" << m_pSerializeObject->m_sUserAgent << "]" << std::endl; 
	    os << "ClientIp(std::string) : [" << m_pSerializeObject->m_sClientIp << "]" << std::endl; 
        os << "ClientPort(std::uint32_t) : [" << m_pSerializeObject->m_dwClientPort << "]" << std::endl; 
	    os << "NoneMatchInfo(std::string) : [" << m_pSerializeObject->m_sNoneMatchInfo << "]" << std::endl; 
	    os << "QueryString(std::string) : [" << m_pSerializeObject->m_sQueryString << "]" << std::endl; 
	    os << "PostInfo(std::string) : [" << m_pSerializeObject->m_sPostInfo << "]" << std::endl; 
        os << "FrontServerType(uint8_t) : [" << m_pSerializeObject->m_cFrontServerType << "]" << std::endl; 
        os << "PathInfo(std::string) : [" << m_pSerializeObject->m_sPathInfo << "]" << std::endl; 
        os << "ContentLength(uint32_t) : [" << m_pSerializeObject->m_dwContentLength << "]" << std::endl; 
        os << "Accept(std::string) : [" << m_pSerializeObject->m_sAccept << "]" << std::endl; 
    }

    bool CSOHttpRequest2::Serialize(CByteStreamNetwork& bs) 
    {
		CSOHttpRequest::Serialize(bs);

        bs & m_pSerializeObject2->m_sContentType;

		/*add by anthonywei*/
		if(m_pSerializeObject2->m_cProtocolVersion >= HTTP_REQUEST_VERSION_1)
		{
			bs & m_pSerializeObject2->m_HTTP_X_FORWARDED_FOR;
			bs & m_pSerializeObject2->m_HTTP_X_REAL_IP;
			bs & m_pSerializeObject2->m_HTTP_X_ORIGINAL_HOST;
			bs & m_pSerializeObject2->m_HTTP_X_ORIGINAL_URL;
			bs & m_pSerializeObject2->m_HTTP_X_ARR_LOG_ID;
		}

        return bs.isGood();
    }

    void CSOHttpRequest2::Dump(std::ostream& os)
	{
		CSOHttpRequest::Dump(os);
        os << "ContentType(std::string) : [" << m_pSerializeObject2->m_sContentType << "]" << std::endl; 
		os << "m_HTTP_X_FORWARDED_FOR : [" << m_pSerializeObject2->m_HTTP_X_FORWARDED_FOR << "]" << std::endl; 
		os << "m_HTTP_X_REAL_IP : [" << m_pSerializeObject2->m_HTTP_X_REAL_IP << "]" << std::endl; 
		os << "m_HTTP_X_ORIGINAL_HOST : [" << m_pSerializeObject2->m_HTTP_X_ORIGINAL_HOST << "]" << std::endl; 
		os << "m_HTTP_X_ORIGINAL_URL : [" << m_pSerializeObject2->m_HTTP_X_ORIGINAL_URL << "]" << std::endl; 
		os << "m_HTTP_X_ARR_LOG_ID : [" << m_pSerializeObject2->m_HTTP_X_ARR_LOG_ID << "]" << std::endl; 
	}
}}


