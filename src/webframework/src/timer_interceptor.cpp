/**
 * @brief 计时统计拦截器
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.07.17
 */
#include "timer_interceptor.h"
#include <assert.h>

namespace web_solution{ namespace web_framework{
    bool CTimerInterceptor::Init(void* pParameter)
    {
        assert(pParameter);

        m_sLoggerName = *static_cast<std::string*>(pParameter);

        m_sInterceptorName = "TimerInterceptor";
        return true;
    }

    int CTimerInterceptor::InterceptBeforeProcess(void* pParameter)
    {
        gettimeofday(&m_tBeginTime, 0);
        return OK;
    }

    int CTimerInterceptor::InterceptAfterProcess(void* pParameter)
    {
        gettimeofday(&m_tEndTime, 0);
        return OK; 
    }

}}


