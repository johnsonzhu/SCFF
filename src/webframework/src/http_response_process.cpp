/**
 * @brief Http应答处理器
 * 主要处理CHttpResponse中有关http协议头的属性设置，及针对相应的出错码进行处理
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.03.29
 */
#include "http_response_process.h"

//#include "pl_inner_logger.h"
//using namespace pl_inner_logger::comm;

namespace web_solution{ namespace web_container{
    CHttpResponseProcess* CHttpResponseProcess::m_pHttpResponseProcess = NULL;

    CHttpResponseProcess* CHttpResponseProcess::Instance(
        std::map<uint32_t, string>* pErrorDocConfigMap)
    {
        if(!m_pHttpResponseProcess)
        {
            m_pHttpResponseProcess = new CHttpResponseProcess(*pErrorDocConfigMap);
        }

        return m_pHttpResponseProcess;
    }

    void CHttpResponseProcess::Init()
    {
/*
        LOGGER_RUN("====== begin load error document ======");
        std::map<uint32_t, string>::iterator pErrorDocConfigPos;

        for (pErrorDocConfigPos = m_oErrorDocConfigMap.begin(); 
            pErrorDocConfigPos != m_oErrorDocConfigMap.end(); ++pErrorDocConfigPos) 
        {
            string sErrorDocument;
            
            GetErrorDocument(pErrorDocConfigPos->second, sErrorDocument);

            m_oErrorDocumentMap[pErrorDocConfigPos->first] = sErrorDocument;
        }

        LOGGER_RUN("====== load error document succ ======");
*/
        return;
    }

    void CHttpResponseProcess::GetErrorDocument(string& sErrDocPath, string& sErrorDocument)
    {
        FILE* fp = fopen(sErrDocPath.c_str() , "rb");
		if(fp == NULL)
		{
            //LOGGER_RUN("Attention:GetErrorDocument() cant open file %s!", 
            //    sErrDocPath.c_str());

            sErrorDocument = "";
			return ;
		}

        const uint32_t dwSizePerRead = 200;
		char szTmpBuf[dwSizePerRead];
		int iRes;
		int iCount = 0;
		while(true)
		{
			iRes = fread(szTmpBuf, sizeof(szTmpBuf), 1, fp);
			if(iRes == 1)
			{
				iCount += sizeof(szTmpBuf);
				sErrorDocument.append(szTmpBuf, sizeof(szTmpBuf));
			}
			else
			{
				int iPos = 0;
				fseek(fp, iCount, SEEK_SET);

				while(!feof(fp))
				{
					iPos = fread(szTmpBuf, 1, sizeof(szTmpBuf), fp);
					sErrorDocument.append(szTmpBuf, iPos);

					iCount += iPos;
				}

				break;
			}
		}      

		fclose(fp);	

        //LOGGER_RUN("Load error document %s succ.", sErrDocPath.c_str());

        return ;
    }

    void CHttpResponseProcess::DoForbiddenResponse(web_solution::comm::CHttpRequest& oHttpRequest, 
            web_solution::comm::CHttpResponse& oHttpResponse)
    {
        return ErrorResponse(FORBIDDEN, oHttpRequest, oHttpResponse);
    }

    void CHttpResponseProcess::DoBadRequestResponse(web_solution::comm::CHttpRequest& oHttpRequest, 
            web_solution::comm::CHttpResponse& oHttpResponse)
    {
        return ErrorResponse(BAD_REQUEST, oHttpRequest, oHttpResponse);
    }

    void CHttpResponseProcess::DoNotFoundResponse(web_solution::comm::CHttpRequest& oHttpRequest, 
            web_solution::comm::CHttpResponse& oHttpResponse)
    {
        return ErrorResponse(NOT_FOUND, oHttpRequest, oHttpResponse);
    }

    void CHttpResponseProcess::DoTimeoutResponse(web_solution::comm::CHttpRequest& oHttpRequest, 
            web_solution::comm::CHttpResponse& oHttpResponse)
    {
        return ErrorResponse(TIMEOUT, oHttpRequest, oHttpResponse);
    }

    void CHttpResponseProcess::DoSvrInternalErrResponse(web_solution::comm::CHttpRequest& oHttpRequest, 
            web_solution::comm::CHttpResponse& oHttpResponse)
    {
        return ErrorResponse(SERVER_INTERNAL_ERROR, oHttpRequest, oHttpResponse);
    }

    void CHttpResponseProcess::ErrorResponse(uint32_t dwErrorCode, 
            web_solution::comm::CHttpRequest& oHttpRequest, 
            web_solution::comm::CHttpResponse& oHttpResponse)
    {
		oHttpResponse.Reset();
        oHttpResponse.SetSeqNo(oHttpRequest.GetSeqNo());
        oHttpResponse.SetConnectionId(oHttpRequest.GetConnectionId());
        oHttpResponse.SetRequestTime(oHttpRequest.GetRequestTime());
        oHttpResponse.SetMethodType(oHttpRequest.GetMethodType());
        oHttpResponse.SetProtocol(oHttpRequest.GetProtocol());
        oHttpResponse.SetUrl(oHttpRequest.GetUrl());
        oHttpResponse.SetAcceptEncoding(oHttpRequest.GetAcceptEncoding());
        oHttpResponse.SetAcceptLanguage(oHttpRequest.GetAcceptLanguage());

        /// 出错时通知前端关闭连接
        oHttpResponse.SetConnection(web_solution::comm::CHttpRequest::NO_KEEP_ALIVE);
        //oHttpResponse.SetCookie();
        oHttpResponse.SetHost(oHttpRequest.GetHost());
        oHttpResponse.SetUserAgent(oHttpRequest.GetUserAgent());
        oHttpResponse.SetClientIp(oHttpRequest.GetClientIp());
        oHttpResponse.SetResult(dwErrorCode);
        oHttpResponse.SetUpdateTime(0);
        oHttpResponse.SetWebCacheExpireTime(0);
        oHttpResponse.SetBrowserCacheExpireTime(0);
        //oHttpResponse.SetEtags();

        oHttpResponse.SetUseWebCache(
            web_solution::comm::CHttpResponse::NOT_USE_WEB_CACHE);
        oHttpResponse.SetContentEncoding(
            web_solution::comm::CHttpResponse::NO_ENCODING);
        oHttpResponse.SetFrontServerType(oHttpRequest.GetFrontServerType());
		
		if("application/x-javascript" == oHttpResponse.GetMimeType()
				|| "text/javascript" == oHttpResponse.GetMimeType())
		{
			oHttpResponse.SetResult(200);
            oHttpResponse.SetResponseContent("//var tws=1;");
			//LOGGER_DEBUG("ErrorResponse Mime[%s], SetResponseContent to empty string.", oHttpResponse.GetMimeType().c_str());
			return;
		}

        //if(oHttpResponse.GetMimeType().empty())
        //{
            oHttpResponse.SetMimeType("text/html"); /// default mime type
        //}

		// Load err_page every time, so we can get the fresh one
		std::map<uint32_t, string>::iterator pPos = m_oErrorDocConfigMap.find(dwErrorCode);
		if(pPos != m_oErrorDocConfigMap.end())
		{
			string sErrorDocument;
			GetErrorDocument((*pPos).second, sErrorDocument);
			oHttpResponse.SetResponseContent(sErrorDocument);
		}

		/*
		   std::map<uint32_t, string>::iterator pPos = m_oErrorDocumentMap.find(dwErrorCode);
		   if(pPos != m_oErrorDocumentMap.end())
		   {
		   oHttpResponse.SetResponseContent(m_oErrorDocumentMap.find(dwErrorCode)->second);
		   }
		   */
    }

}}


