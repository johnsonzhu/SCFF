/**
 * @brief Http应答类及其序列化辅助类
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.03.21
 */
#include "http_response.h"

#include "comm_def.h"
#include <iostream>
using namespace std;

namespace web_solution{ namespace comm{
    bool CSOHttpResponse::Serialize(CByteStreamNetwork& bs) 
    {
        bs & m_pSerializeObject->m_cProtocolVersion;

        bs & m_pSerializeObject->m_dwSeqNo;
        bs & m_pSerializeObject->m_dwConnectionId;
        bs & m_pSerializeObject->m_dwVhostId;
        bs & m_pSerializeObject->m_dwRequestTime;
        bs & m_pSerializeObject->m_cMethodType;
        bs & m_pSerializeObject->m_cProtocol;
        bs & m_pSerializeObject->m_sUrl;
        bs & m_pSerializeObject->m_sAcceptEncoding;
        bs & m_pSerializeObject->m_sAcceptLanguage;
        bs & m_pSerializeObject->m_cConnection;
        bs & m_pSerializeObject->m_sCookie;
        bs & m_pSerializeObject->m_sHost;
        bs & m_pSerializeObject->m_sUserAgent;
        bs & m_pSerializeObject->m_sClientIp;
        bs & m_pSerializeObject->m_dwResult;
        bs & m_pSerializeObject->m_dwUpdateTime;
        bs & m_pSerializeObject->m_dwWebCacheExpireTime;
        bs & m_pSerializeObject->m_dwBrowserCacheExpireTime;
        bs & m_pSerializeObject->m_sEtags;
        bs & m_pSerializeObject->m_sMimeType;
        bs & m_pSerializeObject->m_sMimeEncoding;
        bs & m_pSerializeObject->m_cUseWebCache;
        bs & m_pSerializeObject->m_cContentEncoding;
        bs & m_pSerializeObject->m_cFrontServerType;
        bs & m_pSerializeObject->m_sLocation;

        bs & m_pSerializeObject->m_sResponseContent;

        return bs.isGood();
    }

    void CSOHttpResponse::Dump(std::ostream& os)
    {
        os << "ProtocolVersion(uint8_t) : [" << m_pSerializeObject->m_cProtocolVersion << "]" << std::endl; 

	    os << "SeqNo(uint32_t) : [" << m_pSerializeObject->m_dwSeqNo << "]" << std::endl; 
	    os << "ConnectionId(uint32_t) : [" << m_pSerializeObject->m_dwConnectionId << "]" << std::endl; 
        os << "VhostId(uint32_t) : [" << m_pSerializeObject->m_dwVhostId << "]" << std::endl; 
	    os << "RequestTime(uint32_t) : [" << m_pSerializeObject->m_dwRequestTime << "]" << std::endl; 
	    os << "MethodType(uint8_t) : [" << (int)m_pSerializeObject->m_cMethodType << "]" << std::endl; 
	    os << "Procotol(uint8_t) : [" << (int)m_pSerializeObject->m_cProtocol << "]" << std::endl; 
	    os << "Url(std::string) : [" << m_pSerializeObject->m_sUrl << "]" << std::endl; 
	    os << "AcceptEncoding(std::string) : [" << m_pSerializeObject->m_sAcceptEncoding << "]" << std::endl; 
	    os << "AcceptLanguage(std::string) : [" << m_pSerializeObject->m_sAcceptLanguage << "]" << std::endl; 
	    os << "Connection(uint8_t) : [" << (int)m_pSerializeObject->m_cConnection << "]" << std::endl; 
	    os << "Cookie(std::string) : [" << m_pSerializeObject->m_sCookie << "]" << std::endl; 
	    os << "Host(std::string) : [" << m_pSerializeObject->m_sHost << "]" << std::endl; 
	    os << "UserAgent(std::string) : [" << m_pSerializeObject->m_sUserAgent << "]" << std::endl; 
	    os << "ClientIp(std::string) : [" << m_pSerializeObject->m_sClientIp << "]" << std::endl; 
	    os << "Result(uint32_t) : [" << (int)m_pSerializeObject->m_dwResult << "]" << std::endl; 
	    os << "UpdateTime(uint32_t) : [" << m_pSerializeObject->m_dwUpdateTime << "]" << std::endl; 
	    os << "WebCacheExpireTime(uint32_t) : [" << m_pSerializeObject->m_dwWebCacheExpireTime << "]" << std::endl; 
	    os << "BrowserCacheExpireTime(uint32_t) : [" << m_pSerializeObject->m_dwBrowserCacheExpireTime << "]" << std::endl; 
	    os << "Etags(std::string) : [" << m_pSerializeObject->m_sEtags << "]" << std::endl; 
	    os << "MimeType(std::string) : [" << m_pSerializeObject->m_sMimeType << "]" << std::endl; 
	    os << "MimeEncoding(std::string) : [" << m_pSerializeObject->m_sMimeEncoding << "]" << std::endl; 
	    os << "UseWebCache(uint8_t) : [" << (int)m_pSerializeObject->m_cUseWebCache << "]" << std::endl; 
	    os << "ContentEncoding(uint8_t) : [" << (int)m_pSerializeObject->m_cContentEncoding << "]" << std::endl;
        os << "FrontServerType(uint8_t) : [" << m_pSerializeObject->m_cFrontServerType << "]" << std::endl; 
        os << "Location(std::string) : [" << m_pSerializeObject->m_sLocation << "]" << std::endl; 

        os << "ResponseContent(std::string) : [" << m_pSerializeObject->m_sResponseContent << "]" << std::endl; 
    }

}}


