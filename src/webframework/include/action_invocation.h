/**
 * @brief Action Invocation类
 * 创建、持有Action实例和多个拦截器实例列表，在invoke()方法中对interceptor list、action、result等模块依次进行调用
 * 以完成请求的处理及响应
 *
 * 在接口上以单件的方式提供，避免多次创建、销毁的开销
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.07.17
 */
#ifndef __WEB_SOLUTION_WEB_FRAMEWORK_ACTION_INVOCATION_H__
#define __WEB_SOLUTION_WEB_FRAMEWORK_ACTION_INVOCATION_H__

#include "interceptor_factory.h"
#include "action_params.h"
#include <list>

namespace web_solution{ namespace web_framework{
    class CActionInvocation
    {
    public:
        /// 要使用的拦截器以setter的方式注入，并由ActionInvocation负责拦截器的销毁和资源的释放
        bool SetInterceptor(CInterceptor* pInterceptor, void* pParameter);

        /// 设置内嵌于框架内标准提供的拦截器
        ///
        /// 现提供的标准拦截器有：
        ///     TimerInterceptor 计时统计拦截器;
        /// 
        bool SetStdInterceptorByName(const std::string& sInterceptorName, void* pParameter);
       
        /// 设置日志模块logger名称
        void SetLoggerName(std::string& sLoggerName);

        /// 获取日志模块logger名称
        const std::string& GetLoggerName() const;     

		const std::string& GetLastErrMsg() const;

    public:
		static CActionInvocation* GetInstance();
		int ParseActionName(
				uint8_t cMethodType,
				const std::string& sFileName, 
				const std::string& sQueryString, 
				const std::string& sPostInfo, 
				const std::string& sContentType,
				const std::string& sCookies,
				std::string& sControllerName, 
				std::string& sActionName, 
				CActionParams& oActionParams);
		int InvokeInterceptorBefore(void* pParam);
		int InvokeInterceptorAfter(void* pParam);

		int InvokeInterceptor(bool bBefore); // 已经废除,占位以便老版本的adaptor可以编译通过

        ~CActionInvocation();

	private:
		void ParseHeader(const std::string& data, std::string& disposition, std::string& name, std::string& filename, std::string& type);
		int ParseMIME(const std::string& data, std::string& name, std::string& value);
		int ParseMIME2(const std::string& data, std::string& name, std::string& value, std::string& filename);
		int DoMultiPartParse(const std::string& sPostInfo, const std::string& sContentType, CActionParams& oActionParam);
		int DoStandardParse(std::string& sParamPairs, CActionParams& oActionParam);
		int DoParseCookie(const std::string& sCookie, CActionParams& oActionParam);

    private:
        static CActionInvocation* m_pInstance;
        std::list<CInterceptor*> m_oInterceptorList;
        std::string m_sLoggerName;
        CInterceptorFactory m_oInterceptorFactory;
        std::string m_sLastErrMsg;
    };
}}

#endif /// __WEB_SOLUTION_WEB_FRAMEWORK_ACTION_INVOCATION_H__


