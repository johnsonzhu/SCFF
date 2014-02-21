#ifndef FCGI_DEMO_ECHO_H 
#define FCGI_DEMO_ECHO_H 

#include <string>
#include <stdint.h>
#include "help_macros4web.h"
#include "web_view.h"
#include "action_params.h"
#include "web_svc_cntl.h"

namespace fcgi
{
	namespace demo
	{
		namespace echo
		{
			SERVICE class CEcho
			{
				public:
					CEcho();
					~CEcho();
					int Initialize(const char* pszVhostRootPath, const char* pszServiceConfigFile);
					void Reset();

				public:
					EXPORT uint32_t GetLoc(
							const web_solution::web_framework::CActionParams& rParams,
							web_solution::web_framework::CWebViewData& rData,
							web_solution::web_framework::CWebSvcCntl& rCntl
							);
				public:
					int RenderGetLoc( std::string& sOutputBuffer,
							web_solution::web_framework::CWebViewData& rData,
							web_solution::web_framework::CWebSvcCntl& rCntl
							);
				private:
					int VerifyGetLoc(
							const web_solution::web_framework::CActionParams& rParams);
			};
		}
	}
}

#endif /* FCGI_DEMO_ECHO_H*/
