/**
 * @brief 拦截器对象工厂类
 * 可通过拦截器名称获取框架提供的标准拦截器对象
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.07.17
 */
#include "interceptor_factory.h"

#include "timer_interceptor.h"

namespace web_solution{ namespace web_framework{
    CInterceptor* CInterceptorFactory::GetInterceptor(const std::string& sInterceptorName)
    {
        CInterceptor* pInterceptor = NULL;

        std::map<std::string, CInterceptor*>::iterator pos = m_oInterceptorMap.find(sInterceptorName);
        if (pos == m_oInterceptorMap.end())
        {
            if(sInterceptorName == "TimerInterceptor")
            {
                pInterceptor = new CTimerInterceptor;
            }
        }
        else
        {
            pInterceptor = pos->second;
        }        
        
        return pInterceptor;
    }
}}


