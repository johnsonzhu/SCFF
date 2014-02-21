/**
 * @brief 组件对象类，所有容器组件都是从这个类派生出来
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.03.21
 */
#ifndef __WEB_SOLUTION_WEB_CONTAINER_COMPONENT_OBJECT_H__
#define __WEB_SOLUTION_WEB_CONTAINER_COMPONENT_OBJECT_H__

#include <string>

namespace web_solution{ namespace web_container{
    class CComponentObject
    {
    public:
        const std::string& GetErrMsg() const
        {
            return m_sErrMsg;
        }

    protected:
        std::string m_sErrMsg;
    };
}}

#endif ///__WEB_SOLUTION_WEB_CONTAINER_COMPONENT_OBJECT_H__
