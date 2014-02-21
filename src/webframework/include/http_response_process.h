/**
 * @brief Http应答处理器
 * 主要处理CHttpResponse中有关http协议头的属性设置，及针对相应的出错码进行处理
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.03.29
 */
#ifndef __WEB_SOLUTION_WEB_CONTAINER_HTTP_RESPONSE_PROCESS_H__
#define __WEB_SOLUTION_WEB_CONTAINER_HTTP_RESPONSE_PROCESS_H__

#include "component_object.h"
#include "http_request.h"
#include "http_response.h"

#include <map>

using namespace std;
namespace web_solution{ namespace web_container{
    class CHttpResponseProcess: public CComponentObject
    {
    public:
        static CHttpResponseProcess* Instance(
            std::map<uint32_t, string>* pErrorDocConfigMap = NULL);

        void DoBadRequestResponse(web_solution::comm::CHttpRequest& oHttpRequest, 
            web_solution::comm::CHttpResponse& oHttpResponse);

        void DoForbiddenResponse(web_solution::comm::CHttpRequest& oHttpRequest, 
            web_solution::comm::CHttpResponse& oHttpResponse);

        void DoNotFoundResponse(web_solution::comm::CHttpRequest& oHttpRequest, 
            web_solution::comm::CHttpResponse& oHttpResponse);

        void DoTimeoutResponse(web_solution::comm::CHttpRequest& oHttpRequest, 
            web_solution::comm::CHttpResponse& oHttpResponse);

        void DoSvrInternalErrResponse(web_solution::comm::CHttpRequest& oHttpRequest, 
            web_solution::comm::CHttpResponse& oHttpResponse);

    private:
        CHttpResponseProcess(std::map<uint32_t, string>& oErrorDocConfigMap)
            :m_oErrorDocConfigMap(oErrorDocConfigMap)
        {
            Init();
        }

        void Init();

        void GetErrorDocument(string& sErrDocPath, string& sErrorDocument);

        void ErrorResponse(uint32_t dwErrorCode, 
            web_solution::comm::CHttpRequest& oHttpRequest, 
            web_solution::comm::CHttpResponse& oHttpResponse);

    public:
        enum
        {
            SUCCESS = 200,

            NOT_MODIFIED = 304,

            BAD_REQUEST = 400,
            FORBIDDEN = 403,
            NOT_FOUND = 404,
            TIMEOUT = 408,

            SERVER_INTERNAL_ERROR = 500,
        };

    private:
        static CHttpResponseProcess* m_pHttpResponseProcess;

        std::map<uint32_t, string>& m_oErrorDocConfigMap;            /// 出错文档配置信息

        std::map<uint32_t, string> m_oErrorDocumentMap;             /// 出错码对应返回文档内容映射表
    };
}}

#endif /// __WEB_SOLUTION_WEB_CONTAINER_HTTP_RESPONSE_PROCESS_H__


