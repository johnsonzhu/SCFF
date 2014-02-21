/**
 * @brief 序列化接口类
 * 
 * @author Binqiang Huang(yota)
 * @date 2007.03.21
 */
#ifndef __WEB_SOLUTION_COMM_SERIALIZABLE_H__
#define __WEB_SOLUTION_COMM_SERIALIZABLE_H__

#include "byte_stream.h"

namespace web_solution{ namespace comm{
    class ISerializable
    {
    public:
        virtual ~ISerializable() 
        {
        };
        
    public:
        virtual bool Serialize(CByteStreamNetwork& bs) = 0;  
        
        virtual void Dump(std::ostream& os) = 0;
    };

}}

#endif ///__WEB_SOLUTION_COMM_SERIALIZABLE_H__


