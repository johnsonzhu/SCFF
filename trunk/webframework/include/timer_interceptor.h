/**
 * @brief 计时统计拦截器
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.07.17
 */
#ifndef __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_TIMER_INTERCEPTOR_H__
#define __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_TIMER_INTERCEPTOR_H__

#include "interceptor.h"

#include <sys/time.h>

namespace web_solution{ namespace web_framework{
    class CTimerInterceptor: public CInterceptor
    {
    public:
        bool Init(void* pParameter);

        int InterceptBeforeProcess(void* pParameter);

        int InterceptAfterProcess(void* pParameter);

    private:
        std::string m_sLoggerName;

        struct timeval m_tBeginTime;

        struct timeval m_tEndTime;
    };
}}

#endif /// __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_TIMER_INTERCEPTOR_H__



