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
	typedef Fastcgipp::Http::Sessions<std::string> Sessions;
	bool response()
	{
		using namespace Fastcgipp;
		sessions.cleanup();

		session=sessions.find(environment().findCookie("SESSIONID").data());
		
		const std::string& command=environment().findGet("command");
		
		// We need to call this to set a facet in our requests locale regarding how
		// to format the date upon insertion. It needs to conform to the http standard.
		setloc(std::locale(getloc(), new boost::posix_time::time_facet("%a, %d-%b-%Y %H:%M:%S GMT")));

		if(session!=sessions.end())
		{
			if(command=="logout")
			{
				out << "Set-Cookie: SESSIONID=deleted; expires=Thu, 01-Jan-1970 00:00:00 GMT;\n";
				sessions.erase(session);
				session=sessions.end();
				handleNoSession();
			}
			else
			{
				session->first.refresh();
				out << "Set-Cookie: SESSIONID=" << encoding(URL) << session->first << encoding(NONE) << "; expires=" << sessions.getExpiry(session) << '\n';
				handleSession();
			}
		}
		else
		{
			if(command=="login")
			{
				session=sessions.generate(environment().findPost("data").value);
				out << "Set-Cookie: SESSIONID=" << encoding(URL) << session->first << encoding(NONE) << "; expires=" << sessions.getExpiry(session) << '\n';
				handleSession();
			}
			else
				handleNoSession();
		}

		out << "<p>There are " << sessions.size() << " sessions open</p>\n\
	</body>\n\
</html>";

		// Always return true if you are done. This will let apache know we are done
		// and the manager will destroy the request and free it's resources.
		// Return false if you are not finished but want to relinquish control and
		// allow other requests to operate.
		return true;
	}

	void handleSession()
	{
		using namespace Fastcgipp;
		out << "\
Content-Type: text/html; charset=ISO-8859-1\r\n\r\n\
<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>\n\
<html xmlns='http://www.w3.org/1999/xhtml' xml:lang='en' lang='en'>\n\
	<head>\n\
		<meta http-equiv='Content-Type' content='text/html; charset=ISO-8859-1' />\n\
		<title>fastcgi++: Session Handling example</title>\n\
	</head>\n\
	<body>\n\
		<p>We are currently in a session. The session id is " << session->first << " and the session data is \"" << encoding(HTML) << session->second << encoding(NONE) << "\". It will expire at " << sessions.getExpiry(session) << ".</p>\n\
		<p>Click <a href='?command=logout'>here</a> to logout</p>\n";
	}

	void handleNoSession()
	{
		using namespace Fastcgipp;
		out << "\
Content-Type: text/html; charset=ISO-8859-1\r\n\r\n\
<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>\n\
<html xmlns='http://www.w3.org/1999/xhtml' xml:lang='en' lang='en'>\n\
	<head>\n\
		<meta http-equiv='Content-Type' content='text/html; charset=ISO-8859-1' />\n\
		<title>fastcgi++: Session Handling example</title>\n\
	</head>\n\
	<body>\n\
		<p>We are currently not in a session.</p>\n\
		<form action='?command=login' method='post' enctype='application/x-www-form-urlencoded' accept-charset='ISO-8859-1'>\n\
			<div>\n\
				Text: <input type='text' name='data' value='Hola señor, usted me almacenó en una sesión' /> \n\
				<input type='submit' name='submit' value='submit' />\n\
			</div>\n\
		</form>\n";
	}

	static Sessions sessions;
	Sessions::iterator session;
};

Session::Sessions Session::sessions(30, 30);

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
