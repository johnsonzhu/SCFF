/**
 * @brief 拦截器对象工厂类
 * 可通过拦截器名称获取框架提供的标准拦截器对象
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.07.17
 */
#ifndef __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_FACTORY_H__
#define __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_FACTORY_H__

#include "interceptor.h"

#include <map>

namespace web_solution{ namespace web_framework{
    class CInterceptorFactory
    {
    public:
        CInterceptor* GetInterceptor(const std::string& sInterceptorName);

    private:
        std::map<std::string, CInterceptor*> m_oInterceptorMap;
    };
}}

#endif /// __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_FACTORY_H__




