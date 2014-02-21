/**
 * @brief 计时统计拦截器
 * 
 * @author Henry Lu
 * @date 2008.07.03
 */
#ifndef __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_ECHO_REWRITE_INTERCEPTOR_H__
#define __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_ECHO_REWRITE_INTERCEPTOR_H__

#include "interceptor.h"

namespace web_solution{ namespace web_framework{
    class CEchoInterceptor: public CInterceptor
    {
    public:
        bool Init(void* pParameter);
        int InterceptBeforeProcess(void* pParameter);
        int InterceptAfterProcess(void* pParameter);

    private:
    };
}}

#endif /// __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_ECHO_REWRITE_INTERCEPTOR_H__




