#include "<? echo $category_lower; ?>_interceptor.h"

#include "webframework/http_request.h"

#include <assert.h>

using namespace web_solution::comm;

namespace web_solution{ namespace web_framework{
    bool C<? echo $category; ?>Interceptor::Init(void* pParameter)
    {
        m_sInterceptorName = "C<? echo $category; ?>Interceptor";
        return true;
    }

    int C<? echo $category; ?>Interceptor::InterceptBeforeProcess(void* pParameter)
    {
		/****************TDDO:参考下面代码进行URL的Rewrite
 
		assert(pParameter);

		CHttpRequest* pHttpRequest = reinterpret_cast<CHttpRequest*>(pParameter);
		std::string sFileName = pHttpRequest->GetFileName();

		C2C_WW_LOG("sFileName = [%s]", sFileName.c_str());

		// 页面请求的url形式，跟旧的cgi程序名一致
		const char* pattern_in[] = {
			"show_item_leavemsg.js",
			"login_post_item_leavemsg",
			"reply_leavemsg",
			"post_replymsg",
			"delete_leavemsg",
			NULL    
		};

		// 转换成REST风格后的url形式
		const char* pattern_out[] = {
			"itemmsg/ShowItemMsg",
			"itemmsg/PostItemMsg",
			"itemmsg/ReplyItemMsg",
			"itemmsg/PostReplyItemMsg",
			"itemmsg/DeleteItemMsg",
			NULL    
		};

		int idx = 0;
		for (; pattern_in[idx]; idx++)
		{       
			if (0 == strcmp(sFileName.c_str(), pattern_in[idx]))
			{       
				break;  
			}       
		}  

		if (!pattern_in[idx])
		{
			idx = 0;
			for (; pattern_out[idx]; idx++)
			{       
				if (0 == strcmp(sFileName.c_str(), pattern_out[idx]))
				{       
					return 0;  
				}       
			}  

			return -1;
		}

		void* pKey = NULL;
		if(CRewrite::CreateKey(pattern_in[idx], &pKey) != 0)
		{
			C2C_WW_LOG("\tCreateKey for[%s] failed\n", pattern_in[idx]);
			return -1;
		}

		assert(pKey);

		std::string sUriOut;
		int iret = CRewrite::DoRewrite(pKey, pattern_out[idx], sFileName.c_str(), sUriOut);
		if(iret > 0)
		{
			C2C_WW_LOG("\trewrite ok. iret=%d, before[%s] after[%s]\n", iret, sFileName.c_str(), sUriOut.c_str());
			pHttpRequest->SetFileName(sUriOut);
		}	
		else
			C2C_WW_LOG("\trewrite failed. iret=%d, before[%s] after[%s]\n", iret, sFileName.c_str(), sUriOut.c_str());

		CRewrite::DestoryKey(pKey);	

		******************************************/

		return 0;
    }

    int C<? echo $category; ?>Interceptor::InterceptAfterProcess(void* pParameter)
    {
		return 0;
    }

}}


