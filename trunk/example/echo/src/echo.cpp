#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "echo.h"

using namespace std;
using namespace web_solution::web_framework;
using namespace fcgi::demo::echo;

int CEcho::Initialize(const char* pszVhostRootPath, const char* pszServiceConfigFile)
{
	int iRet = 0;

	return iRet;
}

uint32_t CEcho::GetLoc(
							const web_solution::web_framework::CActionParams& rParams,
							web_solution::web_framework::CWebViewData& rData,
							web_solution::web_framework::CWebSvcCntl& rCntl
							)
{
	uint32_t dwRet = 0;
	string areaData = "Hello Echo Demo!";

	string gettest = "";
	if (rParams.GetVal("gettest", gettest) != 0)
	{
		gettest = "";
	}

	string method = "";
	if (rParams.GetVal("HttpMethod", method) != 0)
	{
		method = "";
	}
	
	string prid = "";
	if (rParams.GetCookieVal("prid", prid) != 0)
	{
		prid = "";
	}

	areaData += "<br />gettest get:" + gettest;
	areaData += "<br />prid cookie:" + prid;
	areaData += "<br />method:" + method;
	rData.SetUserData("content", areaData);

	cout << "test cout"<<"<br />";
	// test cookie
	rData.UpdateCookieVal("echo_test_cookie", "johnsonzhu", "Sat, 08 Nov 2014 09:26:30 GMT", "yixun.com");
	rData.UpdateCookieVal("echo_test_cookie2", "johnsonzhu2", "Sat, 08 Nov 2014 09:26:30 GMT", "yixun.com");
	rData.DelCookieVal("echo_test_cookie2");

	// test 302
	//string redirectUrl = "http://u.yixun.com/abc?noRedirect=1";
	//rCntl.SetLocation(redirectUrl);
	//rCntl.SetResult(302);
	return dwRet;
}

int CEcho::RenderGetLoc( std::string& sOutputBuffer,
							web_solution::web_framework::CWebViewData& rData,
							web_solution::web_framework::CWebSvcCntl& rCntl
							)
{
	uint32_t dwRet = 0;

	//设置max-age,暂时设置10秒
	uint32_t maxAge = 10;
	rCntl.SetBrowserCacheExpireTime(maxAge);
	rCntl.SetMimeEncoding("abc");
	rCntl.SetMimeType("text/html; charset=utf-8");

	string content = "";
	string errMsg = "";
	rData.GetUserData("errMsg", errMsg);
	if (!errMsg.empty())
	{
		sOutputBuffer = errMsg;
	}
	else
	{
		rData.GetUserData("content", content);
		sOutputBuffer = content;
	}
	return dwRet;
}

int CEcho::VerifyGetLoc(
							const web_solution::web_framework::CActionParams& rParams)
{
	uint32_t dwRet = 0;
	return dwRet;
}

CEcho::CEcho()
{
}

CEcho::~CEcho()
{
}

void CEcho::Reset(){}
