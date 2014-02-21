#include <fstream>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>

#include <asql/mysql.hpp>
#include <fastcgi++/manager.hpp>
#include <fastcgi++/request.hpp>


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

struct Log
{
	Fastcgipp::Http::Address ipAddress;
	ASql::Data::Datetime timestamp;
	Fastcgipp::Http::SessionId sessionId;
	ASql::Data::WtextN referral;

	ASQL_BUILDSET(\
		(ASql::Data::Index(ipAddress))\
		(timestamp)\
		(ASql::Data::Index(sessionId))\
		(referral))
};

struct IpAddress: public ASql::Data::Set
{
	Fastcgipp::Http::Address data;
	ASQL_BUILDSET((ASql::Data::Index(data)))
};

class Database: public Fastcgipp::Request<wchar_t>
{
	static const char insertStatementString[];
	static const char updateStatementString[];
	static const char selectStatementString[];

	static ASql::MySQL::Connection sqlConnection;
	static ASql::MySQL::Statement insertStatement;
	static ASql::MySQL::Statement updateStatement;
	static ASql::MySQL::Statement selectStatement;

	enum Status { START, FETCH } status;

	typedef std::vector<Log> LogContainer;

	bool response();

	ASql::Query<void, ASql::Data::STLSetContainer<LogContainer> > m_query;
public:
	Database(): status(START) {}
	static void initSql();
};

const char Database::insertStatementString[] = "INSERT INTO logs (ipAddress, timeStamp, sessionId, referral) VALUE(?, ?, ?, ?)";
const char Database::updateStatementString[] = "UPDATE logs SET timeStamp=SUBTIME(timeStamp, '01:00:00') WHERE ipAddress=?";
const char Database::selectStatementString[] = "SELECT SQL_CALC_FOUND_ROWS ipAddress, timeStamp, sessionId, referral FROM logs ORDER BY timeStamp DESC LIMIT 10";

ASql::MySQL::Connection Database::sqlConnection(4);
ASql::MySQL::Statement Database::insertStatement(Database::sqlConnection);
ASql::MySQL::Statement Database::updateStatement(Database::sqlConnection);
ASql::MySQL::Statement Database::selectStatement(Database::sqlConnection);

void Database::initSql()
{
	sqlConnection.connect("localhost", "fcgi", "databaseExample", "fastcgipp", 0, 0, 0, "utf8");
	const ASql::Data::SetBuilder<Log> log;
	const IpAddress addy;
	insertStatement.init(insertStatementString, sizeof(insertStatementString)-1, &log, 0);
	updateStatement.init(updateStatementString, sizeof(updateStatementString)-1, &addy, 0);
	selectStatement.init(selectStatementString, sizeof(selectStatementString)-1, 0, &log);
	sqlConnection.start();
}

bool Database::response()
{
	using namespace Fastcgipp;
	switch(status)
	{
		case START:
		{
			m_query.createResults();
			m_query.setCallback(boost::bind(callback(), Fastcgipp::Message(1)));
			m_query.enableRows();
			selectStatement.queue(m_query);
			status=FETCH;
			return false;
		}
		case FETCH:
		{
			out << "Content-Type: text/html; charset=utf-8\r\n\r\n\
<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>\n\
<html xmlns='http://www.w3.org/1999/xhtml' xml:lang='en' lang='en'>\n\
	<head>\n\
		<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />\n\
		<title>fastcgi++: SQL Database example</title>\n\
	</head>\n\
	<body>\n\
		<h2>Showing " << m_query.results()->data.size() << " results out of " << m_query.rows() << "</h2>\n\
		<table>\n\
			<tr>\n\
				<td><b>IP Address</b></td>\n\
				<td><b>Timestamp</b></td>\n\
				<td><b>Session ID</b></td>\n\
				<td><b>Referral</b></td>\n\
			</tr>\n";

			for(LogContainer::iterator it(m_query.results()->data.begin()); it!=m_query.results()->data.end(); ++it)
			{
				out << "\
			<tr>\n\
				<td>" << it->ipAddress << "</td>\n\
				<td>" << it->timestamp << "</td>\n\
				<td>" << it->sessionId << "</td>\n\
				<td>" << encoding(HTML) << it->referral << encoding(NONE) << "</td>\n\
			</tr>\n";
			}

			out << "\
		</table>\n\
		<p><a href='database.fcgi'>Refer Me</a></p>\n\
	</body>\n\
</html>";

			ASql::Query<ASql::Data::SetBuilder<Log>, void> insertQuery;
			insertQuery.createParameters();
			insertQuery.keepAlive(true);
			insertQuery.parameters()->data.ipAddress = environment().remoteAddress;
			insertQuery.parameters()->data.timestamp = boost::posix_time::second_clock::universal_time();
			if(environment().referer.size())
				insertQuery.parameters()->data.referral = environment().referer;

			ASql::Query<IpAddress, void> updateQuery;
			updateQuery.createParameters().data = environment().remoteAddress;
			updateQuery.keepAlive(true);

			ASql::MySQL::Transaction transaction;
			transaction.push(insertQuery, insertStatement);
			transaction.push(updateQuery, updateStatement);
			transaction.start();

			return true;
		}
	}
}

int main()
{
	try
	{
		Database::initSql();
		Fastcgipp::Manager<Database> fcgi;
		fcgi.handler();
	}
	catch(std::exception& e)
	{
		error_log(e.what());
	}
}
