#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>
#include <fastcgi++/http.hpp>
#include "action_invocation.h"
#include <sstream>
#include <string.h>
#include <sys/time.h>
#include "action_invocation.h"
#include "http_request.h"
#include "http_response.h"
#include "http_response_process.h"
#include "web_view.h"
#include "web_svc_cntl.h"

#include "<? echo $svc_h; ?>"

<? $svc_header_arr = explode(".", $svc_h);
$svc_header = strtoupper($svc_header_arr[0]);
?>

#include "<? echo $category_lower; ?>_interceptor.h"


using namespace Fastcgipp;
using namespace Fastcgipp::Http;

using namespace web_solution::web_framework;
using namespace <? echo $category_namespace; ?>;

void error_log(const char* msg)
{
	using namespace std;
	using namespace boost;
	static ofstream error;
	if(!error.is_open())
	{
		error.open("/tmp/errlog", ios_base::out | ios_base::app);
		error.imbue(locale(error.getloc(), new posix_time::time_facet()));
	}

	error << '[' << posix_time::second_clock::local_time() << "] " << msg << endl;
}


class <? echo $category; ?>: public Fastcgipp::Request<char>
{
	bool response()
	{
		sysmain();
		return true;
	};

	int sysmain()
	{
		CEcho oMyWebSvc;
		out << "<p>EnterSysMain</p>";

		web_solution::comm::CHttpRequest2 oHttpRequest;
		if(SetHttpRequest(oHttpRequest) != 0)
		{
			out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
			out << "set http request error <br />";
			return 0;
		}

		if(oMyWebSvc.Initialize(NULL, NULL) != 0)
		{
			std::string sErrMsg = "WebService Init failed";
			out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
			out << sErrMsg;
			return 0;
		}

		if(InterceptorInitialize(NULL, NULL) != 0)
		{
			std::string sErrMsg = "Interceptor Init failed";
			out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
			out << sErrMsg;
			return 0;
		}

		CActionInvocation* pActionInvocation = CActionInvocation::GetInstance();
		int iRetCode = 0;
		void* pParam = reinterpret_cast<void*>(&oHttpRequest);
		if((iRetCode = pActionInvocation->InvokeInterceptorBefore(pParam)) != 0)
		{
			std::string sErrMsg = "InvokeInterceptorBefore failed";
			out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
			out << sErrMsg;
			return 0;
		}

		CActionParams oActionParams;
		CWebViewData oData;
		CWebSvcCntl oCntl;
		web_solution::comm::CHttpResponse oHttpResponse;
		if((iRetCode = MyWebSvcDispatch(oActionParams, oData, oCntl, oMyWebSvc, oHttpRequest, oHttpResponse)) != 0)
		{
			std::string sErrMsg;
			if(-2 == iRetCode) // Render Error
			{
				sErrMsg = "Render Action Error for Url[";
				sErrMsg += oHttpRequest.GetUrl();
				sErrMsg += "] FileName[";
				sErrMsg += oHttpRequest.GetFileName();
				sErrMsg += "]";
			}
			else
			{
				sErrMsg = "Parsing Url[";
				sErrMsg += oHttpRequest.GetUrl();
				sErrMsg += "] FileName[";
				sErrMsg += oHttpRequest.GetFileName();
				sErrMsg += "] failed, ";
				sErrMsg += pActionInvocation->GetLastErrMsg();
			}
			out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
			out << "EN:" << iRetCode;
			out << sErrMsg;
			return 0;
		}

		if((iRetCode = pActionInvocation->InvokeInterceptorAfter(NULL)) != 0)
		{
			std::string sErrMsg = "InvokeInterceptorAfter failed";
			out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
			out << sErrMsg;
			return 0;
		}

		stringstream ss;
		if(oHttpResponse.GetResult() == 200)
		{
			out << oHttpResponse.GetCookie();
			if(oCntl.GetOutputHead().empty())
				out << "Content-Type: text/html; charset=utf-8";
			else
				out << oCntl.GetOutputHead();
			error_log(oCntl.GetOutputHead().c_str());
			error_log(oHttpResponse.GetCookie().c_str());
			
			out << "\r\n\r\n";

			uint32_t dwLen = oHttpResponse.GetResponseContent().length();
			const unsigned char* pCh = (unsigned char*) oHttpResponse.GetResponseContent().c_str();
			for(uint32_t dwIndex = 0; dwIndex < dwLen; ++dwIndex)
			{
				out << *(pCh + dwIndex);
			}
		}
		else if(oHttpResponse.GetResult() == 302) 
		{
			out << "Content-Type: text/html; charset=utf-8\n";
			RedirectLocation(oHttpResponse.GetLocation().c_str());
			error_log(oHttpResponse.GetCookie().c_str());
			out << oHttpResponse.GetCookie() << "\n";
			out << "\r\n\r\n";
		}
		else if(oHttpResponse.GetResult() == 404)
		{
			// Not Found
			out << "Content-Type: text/html; charset=utf-8\n";
			out << "Status: 404 Not Found\r\n\r\n";
		}
		else
		{
			std::string sErrMsg = "Unsupported Http RetCode : ";
			std::stringstream oss;
			oss << oHttpResponse.GetResult();
			sErrMsg += oss.str();
			out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
			out << sErrMsg;
		}


		return 0;
	};

	int MyWebSvcDispatch(
			CActionParams& rActionParams,
			CWebViewData& rData,
			CWebSvcCntl& rCntl,
			C<? echo $category; ?>& rSvc,
			web_solution::comm::CHttpRequest2& rHttpRequest, 
			web_solution::comm::CHttpResponse& rHttpResponse)
	{
		CActionInvocation* pActionInvocation = CActionInvocation::GetInstance();
		std::string sController;
		std::string sAction;

		int iRetCode = pActionInvocation->ParseActionName(
				rHttpRequest.GetMethodType(),
				rHttpRequest.GetFileName(), 
				rHttpRequest.GetQueryString(), 
				rHttpRequest.GetPostInfo(), 
				rHttpRequest.GetContentType(),
				rHttpRequest.GetCookie(),
				sController, sAction, rActionParams);
		if(iRetCode != 0)
		{
			out << "parse action error";
			return -1;
		}

		std::string sOutputBuffer;

		SetActionParams(rHttpRequest, rActionParams);

		rCntl.SetResult(200);
		
		<?
for($i = 0; $i < count($statful_services); $i++)
{
	$service_name = $statful_services[$i]['attrs'][NAME];
	$service_name_lower = strtolower($service_name);
	$request_items = $statful_services[$i]['child'][0]['child'];
	$response_items = $statful_services[$i]['child'][1]['child'];
	$cmd_name_prefix = $category_upper.'_'.strtoupper($statful_services[$i]['attrs'][NAME]);
	$TableSpace = "";
	$params_list = "";	
	if($i > 0)
		echo $TableSpace."else if(strcasecmp(\"$service_name_lower\", sAction.c_str()) == 0)";
	else	
		echo $TableSpace."if(strcasecmp(\"$service_name_lower\", sAction.c_str()) == 0)";
?>	
		{
			rSvc.Reset();
			iRetCode = rSvc.<? echo $service_name; ?>(rActionParams, rData, rCntl);
			iRetCode = rSvc.Render<? echo $service_name; ?>(sOutputBuffer, rData, rCntl);
			CWebViewTemplate::ClearCache(); 
			if(iRetCode != 0)
			{	
					out << "ERR:" << iRetCode;
					rCntl.SetResult(500);
					return -2;
			}
		}
<?			
}
?>	
		else
		{
			rCntl.SetResult(400);
			return -3;
		}
		/// 显示视图数据
		rHttpResponse.SetResult(rCntl.GetResult());
		rHttpResponse.SetUpdateTime(rCntl.GetUpdateTime());
		rHttpResponse.SetWebCacheExpireTime(rCntl.GetWebCacheExpireTime());
		rHttpResponse.SetBrowserCacheExpireTime(rCntl.GetBrowserCacheExpireTime());
		rHttpResponse.SetEtags(rCntl.GetEtags());
		rHttpResponse.SetMimeType(rCntl.GetMimeType());
		rHttpResponse.SetMimeEncoding(rCntl.GetMimeEncoding());
		rHttpResponse.SetUseWebCache(rCntl.GetUseWebCache());
		rHttpResponse.SetContentEncoding(rCntl.GetContentEncoding());

		// Clean CRLF
		std::string sLocation = rCntl.GetLocation();
		std::string::size_type pos = sLocation.find("\015\012");
		if(pos != std::string::npos)
			sLocation.erase(pos, sLocation.length());

		rHttpResponse.SetLocation(sLocation);

		std::string sSetCookie;
		rData.OutputCookie(sSetCookie);

		rHttpResponse.SetCookie(sSetCookie);

		rHttpResponse.SetResponseContent(sOutputBuffer);

		return 0;
	};

	int InterceptorInitialize(const char* pszVhostRootPath, const char* pszServiceConfigFile) 
	{       
		return 0;
	};  

	int SetHttpRequest(web_solution::comm::CHttpRequest2& rHttpRequest)
	{
		rHttpRequest.SetProtocol(web_solution::comm::CHttpRequest::HTTP_PROCOTOL_11);
		rHttpRequest.SetHost(environment().host);

		rHttpRequest.SetFileName(environment().scriptName);

		rHttpRequest.SetReferer(environment().referer);

		rHttpRequest.SetAcceptEncoding(environment().acceptCharsets );

		rHttpRequest.SetAcceptLanguage(environment().acceptLanguages );

		//rHttpRequest.SetCookie(environment().cookies);

		rHttpRequest.SetUserAgent(environment().userAgent);

		rHttpRequest.SetClientIp(reinterpret_cast<const char*>(environment().remoteAddress.data()));

		rHttpRequest.SetClientPort(environment().remotePort);

		//rHttpRequest.SetPathInfo(environment().pathInfo);

		rHttpRequest.SetAccept(environment().acceptContentTypes);

		rHttpRequest.SetMethodType(environment().requestMethod);

		rHttpRequest.SetContentLength(environment().contentLength);

		//rHttpRequest.SetPostInfo(pBuf, iLen);

		if (HTTP_METHOD_GET  == environment().requestMethod)
		{
			rHttpRequest.SetMethodType(web_solution::comm::CHttpRequest::GET_METHOD_TYPE);
		}
		else if (HTTP_METHOD_POST  == environment().requestMethod)
		{
			rHttpRequest.SetMethodType(web_solution::comm::CHttpRequest::POST_METHOD_TYPE);
		}

		rHttpRequest.SetQueryString(environment().requestUri);

		rHttpRequest.SetContentType(environment().contentType);

		// Now
		rHttpRequest.SetRequestTime(time(NULL));

		return 0;
	};

	int SetActionParams(web_solution::comm::CHttpRequest2& rHttpRequest, CActionParams& rActionParams)
	{
		rActionParams.AddVal("ClientIP", rHttpRequest.GetClientIp());
		rActionParams.AddVal("FileName", rHttpRequest.GetFileName());
		rActionParams.AddVal("QueryString", rHttpRequest.GetQueryString());
		rActionParams.AddVal("PostInfo", rHttpRequest.GetPostInfo());
		rActionParams.AddVal("ContentType", rHttpRequest.GetContentType());
		rActionParams.AddVal("Cookie", rHttpRequest.GetCookie());
		rActionParams.AddVal("UserAgent", rHttpRequest.GetUserAgent());
		rActionParams.AddVal("Referer", rHttpRequest.GetReferer());

		std::stringstream oss;
		oss << rHttpRequest.GetClientPort();
		rActionParams.AddVal("ClientPort", oss.str());
		if(0x03 == rHttpRequest.GetMethodType())
			rActionParams.AddVal("HttpMethod", "POST");
		else
			rActionParams.AddVal("HttpMethod", "GET");

		// add get param 
        for(Http::Environment<char>::Gets::const_iterator it=environment().gets.begin(); it!=environment().gets.end(); ++it)
		{
			rActionParams.AddVal(it->first, it->second);
		}

		// add post param 
        for(Http::Environment<char>::Posts::const_iterator it=environment().posts.begin(); it!=environment().posts.end(); ++it)
		{
            if(it->second.type==Http::Post<char>::form)
            {
				rActionParams.AddVal(it->first, it->second.value);
            }
            else
            {
               rActionParams.AddVal(it->first+"_filename", it->second.filename);
               rActionParams.AddVal(it->first, it->second.data());
            }
		}

		// add cookie param 
        for(Http::Environment<char>::Cookies::const_iterator it=environment().cookies.begin(); it!=environment().cookies.end(); ++it)
		{
			rActionParams.AddCookiePair(it->first, it->second);
		}

		return 0;
	};

	void RedirectLocation(const char* pszLocation)
	{
		assert(pszLocation && strlen(pszLocation) > 0);

		time_t ltime;
		time(&ltime);

		out << "Location:" << pszLocation << "\n";
		/*if (strstr(pszLocation, "?") != NULL)
		{
			out << "Location:" << pszLocation << "&CacheTime=" << ltime << "\n";
		}
		else
		{
			out << "Location:" << pszLocation << "?CacheTime=" << ltime << "\n";
		}*/

		out << "Cache-control: private\n";
	};


};

int main()
{
	try
	{
		Fastcgipp::Manager<<? echo $category; ?>> fcgi;
		fcgi.handler();
	}
	catch(std::exception& e)
	{
		error_log(e.what());
	}
}




