/**
 * @brief 计时统计拦截器
 * 
 * @author Henry Lu
 * @date 2008.07.03
 */
#ifndef __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_<? echo $category_upper; ?>_REWRITE_INTERCEPTOR_H__
#define __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_<? echo $category_upper; ?>_REWRITE_INTERCEPTOR_H__

#include "interceptor.h"

namespace web_solution{ namespace web_framework{
    class C<? echo $category; ?>Interceptor: public CInterceptor
    {
    public:
        bool Init(void* pParameter);
        int InterceptBeforeProcess(void* pParameter);
        int InterceptAfterProcess(void* pParameter);

    private:
    };
}}

#endif /// __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_<? echo $category_upper; ?>_REWRITE_INTERCEPTOR_H__



