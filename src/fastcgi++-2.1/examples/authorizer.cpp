/***************************************************************************
* Copyright (C) 2007 Eddie Carle [eddie@erctech.org]                       *
*                                                                          *
* This file is part of fastcgi++.                                          *
*                                                                          *
* fastcgi++ is free software: you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as  published   *
* by the Free Software Foundation, either version 3 of the License, or (at *
* your option) any later version.                                          *
*                                                                          *
* fastcgi++ is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    *
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public     *
* License for more details.                                                *
*                                                                          *
* You should have received a copy of the GNU Lesser General Public License *
* along with fastcgi++.  If not, see <http://www.gnu.org/licenses/>.       *
****************************************************************************/


#include <fstream>
#include <string>

#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>

// I like to have an independent error log file to keep track of exceptions while debugging.
// You might want a different filename. I just picked this because everything has access there.
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

// Let's make our request handling class. It must do the following:
// 1) Be derived from Fastcgipp::Request
// 2) Define the virtual response() member function from Fastcgipp::Request()

// First things first let's decide on what kind of character set we will use. Let's just
// use good old ISO-8859-1 this time. No wide characters

class Session: public Fastcgipp::Request<char>
{
	typedef Fastcgipp::Http::Sessions<int> Sessions;
	bool response()
	{
		using namespace Fastcgipp;
		sessions.cleanup();
		session=sessions.find(environment().findCookie("SESSIONID").data());

		switch(role())
		{
			case Fastcgipp::Protocol::RESPONDER:
			{
				// We need to call this to set a facet in our requests locale regarding how
				// to format the date upon insertion. It needs to conform to the http standard.
				setloc(std::locale(getloc(), new boost::posix_time::time_facet("%a, %d-%b-%Y %H:%M:%S GMT")));

				if(session==sessions.end())
				{
					session=sessions.generate(0);
					out << "Set-Cookie: SESSIONID=" << encoding(URL) << session->first << encoding(NONE) << "; expires=" << sessions.getExpiry(session) << '\n';
				}
				else
					session->first.refresh();

				out << "\
Content-Type: text/html; charset=ISO-8859-1\r\n\r\n\
<!DOCTYPE html>\n\
<html lang='en'>\n\
	<head>\n\
		<title>fastcgi++: Authorizer Example</title>\n\
	</head>\n\
\n\
	<body>\n\
		<h1>fastcgi++: Authorizer Example</h1>\n\
		<p>\n\
			You are now authorized to view <a href='locked/locked.png'>the image</a> under the\n\
			session ID " << session->first << " until " << sessions.getExpiry(session) << ".\n\
		</p>\n\
	</body>\n\
</html>";
				break;
			}

			case Fastcgipp::Protocol::AUTHORIZER:
			{
				err << "Asking for permission to view " << environment().requestUri;
				if(session!=sessions.end() && environment().requestUri=="/fastcgipp/locked/locked.png")
					out << "Status: 200 OK\r\n\r\n";
				else
					out << "Status: 403 Forbidden\r\n\r\n";
				break;
			}
		}
		return true;
	}

	static Sessions sessions;
	Sessions::iterator session;
};

Session::Sessions Session::sessions(120, 120);

// The main function is easy to set up
int main()
{
	try
	{
		// First we make a Fastcgipp::Manager object, with our request handling class
		// as a template parameter.
		Fastcgipp::Manager<Session> fcgi;
		// Now just call the object handler function. It will sleep quietly when there
		// are no requests and efficiently manage them when there are many.
		fcgi.handler();
	}
	catch(std::exception& e)
	{
		error_log(e.what());
	}
}
