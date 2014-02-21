/**
 * @brief 环绕拦截器类
 * 拦截器构建原则：拦截器必须是无状态的，且只可调用CActionInvocation提供的public方法接口
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.07.17
 */
#ifndef __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_H__
#define __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_H__

#include <string>

namespace web_solution{ namespace web_framework{
    class CActionInvocation;

    class CInterceptor
    {
    public:
        /// 在拦截器创建后、Http请求被处理前被调用，主要进行一些初始化工作
        ///
        /// pParameter: 为拦截器初始化时需要用到的一些参数，可传入一个类，并在方法中对其进行强制类型转换
        /// 通常情况下，请设置拦截器名称：m_sInterceptorName，方便日志跟踪定位
        virtual bool Init(void* pParameter)
        {
            return true;
        }

        //// 主要进行一些资源释放工作
        virtual void Destroy()
        {
            return;
        }

        /// 在业务处理前，执行拦截校验处理
        virtual int InterceptBeforeProcess(void* pParameter)
        {
            return OK;
        }

        /// 在业务处理后，执行拦截校验处理
        virtual int InterceptAfterProcess(void* pParameter)
        {
            return OK;
        }

        virtual ~CInterceptor()
        {
        }

        /// 获取拦截器名称
        std::string& GetName()
        {
            return m_sInterceptorName;
        }

    public:
        enum INTERCEPTOR_RETURN_TYPE
        {
		    SUCCESS = 0,                /// 返回SUCCESS,处理成功,将不再执行其他拦截器处理
		    
            OK = 1,                     /// 返回OK,处理成功,继续调用后续的拦截器处理

		    FAIL = 2,                   /// 返回FAIL,处理失败，将不再进行下一步处理

            INPUT_FAIL = 3,             /// 返回INPUT_FAIL,参数不足或参数有误，将不再进行下一步处理

            LOGIN_FAIL = 4,             /// 返回LOGIN_FAIL,需要进行用户登录，将不再进行下一步处理
        };

    protected:
        std::string m_sInterceptorName;
    };
}}

#endif /// __WEB_SOLUTION_WEB_FRAMEWORK_INTERCEPTOR_H__


