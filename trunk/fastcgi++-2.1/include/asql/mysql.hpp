//! \file mysql.hpp Declares the ASql::MySQL namespace
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


#ifndef MYSQL_HPP
#define MYSQL_HPP

#include <mysql/mysql.h>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <deque>

#include <asql/asql.hpp>

//! Defines classes and functions relating to SQL querying
namespace ASql
{
	//! Defines classes and functions relating to %MySQL querying
	namespace MySQL
	{
		class Statement;

		//! %Connection to a %MySQL database.
		class Connection: public ConnectionPar<MySQL::Statement>
		{
		private:
			/** 
			//! Actual %MySQL C API connection object.
			 */
			boost::scoped_array<MYSQL> m_connection;

			/** 
			//! Pointer to the statement used to return the number of rows found.
			 */
			boost::scoped_array<MYSQL_STMT*> foundRowsStatement;

			/** 
			//! Bind object for storing the total number of results from a query.
			 */
			boost::scoped_array<MYSQL_BIND> foundRowsBinding;

			bool m_initialized;

		public:
			/** 
			 *	@brief Complete Constructor
			 *
			 * \param[in] host The value of host may be either a hostname or an IP address. If host is NULL or the string "localhost", a connection to the local host is assumed. For Windows, the client connects using a shared-memory connection, if the server has shared-memory connections enabled. Otherwise, TCP/IP is used. For Unix, the client connects using a Unix socket file.
			 * \param[in] user The user parameter contains the user's %MySQL login ID. If user is NULL or the empty string "", the current user is assumed. Under Unix, this is the current login name. Under Windows ODBC, the current username must be specified explicitly.
			 * \param[in] passwd The passwd parameter contains the password for user. If passwd is NULL, only entries in the user table for the user that have a blank (empty) password field are checked for a match. This allows the database administrator to set up the %MySQL privilege system in such a way that users get different privileges depending on whether they have specified a password.
			 * \param[in] db db is the database name. If db is not NULL, the connection sets the default database to this value.
			 * \param[in] port If port is not 0, the value is used as the port number for the TCP/IP connection. Note that the host parameter determines the type of the connection.
			 * \param[in] unix_socket If unix_socket is not NULL, the string specifies the socket or named pipe that should be used. Note that the host parameter determines the type of the connection.
			 * \param[in] client_flag The value of client_flag is usually 0, but can be set to a combination of the following flags to enable certain features. See the mysql c api reference manual for more details.
			 * \param[in] charset Null terminated string representation of desired connection character set (latin1, utf8, ...)
			 * \param[in] threads_ Number of threads to have for simultaneous queries. The higher this number is the more concurrent
			 * SQL requeries can be processed.
			 */
			Connection(const char* host, const char* user, const char* passwd, const char* db, unsigned int port, const char* unix_socket, unsigned long client_flag, const char* const charset="latin1", const int threads_=1): ConnectionPar<MySQL::Statement>(threads_), m_initialized(false)
			{
				connect(host, user, passwd, db, port, unix_socket, client_flag, charset);
			}

			/** 
			//! Incomplete Constructor
			 * 
			 * \param[in] threads_ Number of threads to have for simultaneous queries. The higher this number is the more concurrent
			 * SQL requeries can be processed.
			 */
			Connection(const int threads_=1):
				ConnectionPar<MySQL::Statement>(threads_),
				m_connection(new MYSQL[threads_]),
				foundRowsStatement(new MYSQL_STMT*[threads_]),
				foundRowsBinding(new MYSQL_BIND[threads_]),
				m_initialized(false) {}
			~Connection();

			/**
			//! Connect to a MySQL server.
			 *
			 * \param[in] host The value of host may be either a hostname or an IP address. If host is NULL or the string "localhost", a connection to the local host is assumed. For Windows, the client connects using a shared-memory connection, if the server has shared-memory connections enabled. Otherwise, TCP/IP is used. For Unix, the client connects using a Unix socket file.
			 * \param[in] user The user parameter contains the user's %MySQL login ID. If user is NULL or the empty string "", the current user is assumed. Under Unix, this is the current login name. Under Windows ODBC, the current username must be specified explicitly.
			 * \param[in] passwd The passwd parameter contains the password for user. If passwd is NULL, only entries in the user table for the user that have a blank (empty) password field are checked for a match. This allows the database administrator to set up the %MySQL privilege system in such a way that users get different privileges depending on whether they have specified a password.
			 * \param[in] db db is the database name. If db is not NULL, the connection sets the default database to this value.
			 * \param[in] port If port is not 0, the value is used as the port number for the TCP/IP connection. Note that the host parameter determines the type of the connection.
			 * \param[in] unix_socket If unix_socket is not NULL, the string specifies the socket or named pipe that should be used. Note that the host parameter determines the type of the connection.
			 * \param[in] client_flag The value of client_flag is usually 0, but can be set to a combination of the following flags to enable certain features. See the mysql c api reference manual for more details.
			 * \param[in] charset Null terminated string representation of desired connection character set (latin1, utf8, ...)
			 */
			void connect(const char* host, const char* user, const char* passwd, const char* db, unsigned int port, const char* unix_socket, unsigned long client_flag, const char* const charset="latin1");

			/** 
			//! Returns the number of rows found in the last query. Designed for use with SQL_CALC_FOUND_ROWS in the query itself.
			 * 
			 * \param rows A reference to a pointer to the integer to write the value to.
			 */
			void getFoundRows(unsigned long long* const& rows, const unsigned int id);

			MYSQL& connection(unsigned int id) { return m_connection[id]; }

			inline void commit(const unsigned int thread=0)	{ mysql_commit(&m_connection[thread]); }
			inline void rollback(const unsigned int thread=0)	{ mysql_rollback(&m_connection[thread]); }
		};

		//! %MySQL query statement.
		/** 
		 * This class will store up a prepared %MySQL statement for both
		 * synchronous and asynchronous execution.  It should be initialized
		 * either by the full constructor or init().
		 */
		class Statement: public ASql::Statement
		{
		public:
			//! Complete Constructor
			/** 
			 * The constructor builds the query and the associated
			 * parameters/result data structures based on the template objects
			 * provided by resultSet and parameterSet. These should point to data
			 * structures derived from ASql::Data::Set. The objects need not have
			 * any actual data in them as they are only used to find sizes and
			 * types of indexable member objects. A null pointer indicates no
			 * parameter/result data.
			 *
			 * The SQL query string should be a valid SQL query with '?' as a
			 * placeholder for all parameters ex. "SELECT * FROM testTable WHERE
			 * id=?". Note the lack of a terminating semicolon and lack of
			 * quotations around the question marks. The number of question marks
			 * in the query should match up exactly with the value returned by a
			 * call of ASql::Data::Set::numberOfSqlElements() on parameterSet.
			 * Similarly, the number of returned result columns from the query
			 * should match up with the same function called on resultSet.
			 * 
			 * \param[in] connection_ %MySQL connection to run query through.
			 * \param[in] queryString SQL query. No null termination.
			 * \param[in] queryLength Length of SQL query. No null termination.
			 * \param[in] parameterSet Template object of parameter data set. Null means no parameters.
			 * \param[in] resultSet Template object of result data set. Null means no results.
			 */
			Statement(Connection& connection_, const char* const queryString, const size_t queryLength, const Data::Set* const parameterSet, const Data::Set* const resultSet):
				ASql::Statement(connection_.threads()),
				connection(connection_),
				stmt(new MYSQL_STMT*[connection_.threads()]),
				m_initialized(false),
				paramsBindings(new boost::scoped_array<MYSQL_BIND>[connection_.threads()]),
				resultsBindings(new boost::scoped_array<MYSQL_BIND>[connection_.threads()]),
				m_stop(new const bool*[connection_.threads()])
			{
				init(queryString, queryLength, parameterSet, resultSet);
			}

			//! Simple Constructor
			/** 
			 * \param[in] connection_ %MySQL connection to run query through.
			 */
			Statement(Connection& connection_):
				ASql::Statement(connection_.threads()),
				connection(connection_),
				stmt(new MYSQL_STMT*[connection_.threads()]),
				m_initialized(false),
				paramsBindings(new boost::scoped_array<MYSQL_BIND>[connection_.threads()]),
				resultsBindings(new boost::scoped_array<MYSQL_BIND>[connection_.threads()]),
				m_stop(new const bool*[connection_.threads()]) {}

			~Statement();
			
		 	//! Initialize statement.
			/** 
			 *
			 * The initializer builds the query and the associated
			 * parameters/result data structures based on the template objects
			 * provided by resultSet and parameterSet. These should point to data
			 * structures derived from ASql::Data::Set. The objects need not have
			 * any actual data in them as they are only used to find sizes and
			 * types of indexable member objects. A null pointer indicates no
			 * parameter/result data.
			 *
			 * The SQL query string should be a valid SQL query with '?' as a
			 * placeholder for all parameters ex. "SELECT * FROM testTable WHERE
			 * id=?". Note the lack of a terminating semicolon and lack of
			 * quotations around the question marks. The number of question marks
			 * in the query should match up exactly with the value returned by a
			 * call of ASql::Data::Set::numberOfSqlElements() on parameterSet.
			 * Similarly, the number of returned result columns from the query
			 * should match up with the same function called on resultSet.
			 *
			 * \param[in] queryString SQL query. No null termination.
			 * \param[in] queryLength Length of SQL query. No null termination.
			 * \param[in] parameterSet Template object of parameter data set. Null means no parameters.
			 * \param[in] resultSet Template object of result data set. Null means no results.
			 */
			void init(const char* const& queryString, const size_t& queryLength, const Data::Set* const parameterSet, const Data::Set* const resultSet, bool customPlaceholders=false);

			//! Execute multi-row result %MySQL statement.
			/** 
			 *	Executes the built query with the passed parameter data storing the
			 *	results in the passed results container.  The number of rows
			 *	affected or total matching rows can be retrieved by passing the
			 *	proper pointer to rows.  If SQL_CALC_FOUND_ROWS is included in a
			 *	select statement then this value will represent the total matching
			 *	rows regardless of a LIMIT clause. The last insert value on a
			 *	auto-incremented column can also be retrieved with an appropriate
			 *	pointer in insertId.
			 *
			 *	The Data::Set pointed to by parameters must have the same derived
			 *	type as was passed upon construction of the statement. A null
			 *	pointer, as in the constructor, indicates no parameter data. The
			 *	results parameter should be a pointer to a Data::SetContainer
			 *	templated to the same derived type passed upon construction of the
			 *	statement for the result set. As well, a null pointer indicates no
			 *	result data.
			 * 
			 * \param[in] parameters %Data set of %MySQL query parameter data.
			 * \param[out] results %Data set container of %MySQL query result data.
			 * \param[out] insertId Pointer to integer for writing of last auto-increment insert value.
			 * \param[out] rows Pointer to integer for writing the number of rows affected/matching from last query.
			 * \param[in] docommit Set to true a transaction commit should be completed at the end of this query.
			 */
			void execute(const Data::Set* const parameters, Data::SetContainer* const results, unsigned long long int* const insertId=0, unsigned long long int* const rows=0, bool docommit=true, const unsigned int thread=0);

			//! Execute single result row %MySQL statement.
			/** 
			 * Executes the built query with the passed parameter data storing the
			 * results in the passed results set.  This simple alternative exists
			 * for cases where the user is certain that only a single result row
			 * will be returned and there will be no need for a container of sets.
			 *
			 *	The Data::Set pointed to by parameters must have the same derived
			 *	type as was passed upon construction of the statement. A null
			 *	pointer, as in the constructor, indicates no parameter data. The
			 *	results parameter should be a Data::Set templated to the same
			 *	derived type passed upon construction of the statement for the
			 *	single result result row.
			 *
			 * \param[in] parameters %Data set of %MySQL query parameter data.
			 * \param[out] results Set to store single result row in.
			 * \param[in] docommit Set to true a transaction commit should be completed at the end of this query.
			 *
			 * \return True if result data was recieved, false otherwise.
			 */
			bool execute(const Data::Set* const parameters, Data::Set& results, bool docommit=true, const unsigned int thread=0);

			//! Execute result-less multi-row parameter %MySQL statement.
			/** 
			 * Executes the built query with the passed parameter data storing no
			 * results. This alternative exists to allow users to execute
			 * multi-rows parameter queries which obviously will not return any
			 * data. (INSERT UPDATE DELETE) and such.
			 
			 *	The Data::SetContainer referenced to by parameters must have the
			 *	same derived type as was passed upon construction of the statement.
			 *	a Data::SetContainer templated to the same derived type passed upon
			 *	construction of the statement for the single result result row.
			 
			 * \param[in] parameters  %Data set of %MySQL query parameter data.
			 * \param[out] rows Pointer to integer for writing the number of rows affected from last query.
			 * \param[in] docommit Set to true a transaction commit should be completed at the end of this query.
			 */
			void execute(const Data::SetContainer& parameters, unsigned long long int* rows=0, bool docommit=true, const unsigned int thread=0);

			//! Asynchronously execute a %MySQL statement.
			/** 
			 * This function will queue the statement to be executed in a separate
			 * thread and return immediately. The information for execute() applies
			 * here with a few minor differences. For one, shared pointers are
			 * passed to prevent data being destroyed in one thread before it is
			 * finished with in another (segfault). So don't cheat, make sure they
			 * are shared pointer controlled on your end as well.
			 *
			 * For two, a callback function is supplied to be called when the query
			 * is complete. The data passed is a ASql::Error data structure.
			 *
			 * \param[in/out] QueryPar object to manage the asynchronous execution of the query.
			 */
			inline void queue(QueryPar& query, int instance=-1)
			{
				connection.queue(this, query, instance);
			}
		private:
			Connection& connection;

			//! Pointer to actual %MySQL C API prepared statement object.
			boost::scoped_array<MYSQL_STMT*> stmt;

			//! Bind data set object to query parameters
			std::deque<unsigned char> paramOrder;

			//! Array of parameter bindings for use with the query.
			boost::scoped_array<boost::scoped_array<MYSQL_BIND> > paramsBindings;

			//! Array of result bindings for use with the query.
			boost::scoped_array<boost::scoped_array<MYSQL_BIND> > resultsBindings;
			
			//! Build an array of %MySQL C API prepared statement bindings.
			/** 
			 * This static function will take an object derived from a Data::Set
			 * and build a corresponding %MySQL bind array.  The objects need not
			 * have any actual data in them as they are only used to find sizes and
			 * types of indexable member objects. A null pointer indicates no
			 * parameter/result data.
			 *
			 * A Conversions container is also built to accommodate any types that
			 * can not be written/read directly to/from the Data::Set passed and
			 * will require conversion beforehand.
			 *
			 * This function is only called once upon construction of the statement
			 * object. The resulting Conversions and %MySQL bind arrays can be
			 * reused every time the statement is executed with a call to
			 * bindBindings().
			 *
			 * \param[in] stmt Reference to pointer to %MySQL C API statement.
			 * \param[in] set Reference to a template object.
			 * \param[out] conversions Reference to an array to write conversion data to.
			 * \param[out] bindings Reference to a %MySQL bind array to write data to.
			 */
			static void buildBindings(MYSQL_STMT* const& stmt, const Data::Set& set, Data::Conversions& conversions, boost::scoped_array<MYSQL_BIND>& bindings, const std::deque<unsigned char>* order=0);

			//! Bind an array of %MySQL bindings to the passed data set.
			/** 
			 *	This static function will take an object derived from a Data::Set
			 *	and bind a corresponding %MySQL bind array.  If a conversion is
			 *	required the data set is not binded directly to the bindings array
			 *	but through the corresponding conversion in the Conversions
			 *	container.
			 *
			 *	This function is called before any statement is executed for
			 *	parameter data, and before a row is fetched for result data.
			 *
			 * \param[in/out] set Reference to a data set object.
			 * \param[in] conversions Reference to an array to pass conversion data through.
			 * \param[in] bindings Reference to a %MySQL bind array to write data to.
			 */
			static void bindBindings(Data::Set& set, Data::Conversions& conversions, boost::scoped_array<MYSQL_BIND>& bindings, const std::deque<unsigned char>* order=0);

			//! Execute parameter part of statement
			/** 
			 * \param[in] parameters Parameters to use in query
			 */
			void executeParameters(const Data::Set* const& parameters, const unsigned int thread);
			
			//! Fetch a row of the results
			/** 
			 * \param[out] row Set to store results in
			 * 
			 * \return true normally, false if no data
			 */
			bool executeResult(Data::Set& row, const unsigned int thread);

			bool m_initialized;

			boost::scoped_array<const bool*> m_stop;

			friend class ConnectionPar<Statement>;
			friend class Transaction<Statement>;
		};

		typedef ASql::Transaction<Statement> Transaction;

		//! Handle retrieval of variable length data chunks.
		/** 
		 * Although no conversion is necessary for these data chunks, we wait to
		 * retrieve the data until we actually know how many bytes of it there is.
		 * This "conversion" handles that.
		 */
		template<class T> struct TypedConversion: public Data::Conversion
		{
			//! &MySQL buffer type (MYSQL_TYPE_BLOB or MYSQL_TYPE_STRING)
			enum_field_types bufferType;

			//! Actual length of field.
			unsigned long length;

			//! Retrieve the data chunk into the container pointer to by external.
			virtual void convertResult() { grabIt(*(T*)external); }

			//! Set the buffer to the address of the start of the container pointed to be external.
			virtual void convertParam();

			//! Get a pointer to data chunk the %MySQL library should write to. In this case, it won't be writing.
			/** 
			 * \return Null pointer.
			 */
			void* getPointer() { return 0; }

			/** 
			 * \param[in] column_ Associated column/parameter number.
			 * \param[in] statement_ Reference to %MySQL C API statement object.
			 * \param[in] bufferType_ &MySQL buffer type (MYSQL_TYPE_BLOB or MYSQL_TYPE_STRING).
			 * \param[out] buffer_ Reference to associated %MySQL bind object buffer pointer.
			 */
			TypedConversion(const int& column_, MYSQL_STMT* const& statement_, const enum_field_types& bufferType_, void*& buffer_): bufferType(bufferType_), length(0), buffer(buffer_), column(column_), statement(statement_) {}
		protected:
			//! Reference to associated %MySQL bind object buffer pointer.
			void*& buffer;

			//! Associated column/parameter number.
			int column;

			//! Reference to associated %MySQL C API statement object.
			MYSQL_STMT* const& statement;

			//! Retrieve data from the query into the passed vector.
			/** 
			 * \param[out] data Vector to dump the data into. Size will be adjusted.
			 */
			void grabIt(T& data);
		};
		
		//! Handle conversion from MYSQL_TIME to Data::Datetime.
		template<> struct TypedConversion<Data::Datetime>: public Data::Conversion
		{
			//! Internal MYSQL_TIME object.
			MYSQL_TIME internal;

			//! Convert internal to a Data::Datetime object pointer to by external
			virtual void convertResult();

			//! Convert a Data::Datetime object pointed to by external to internal.
			virtual void convertParam();

			//! Return a pointer to the internal MYSQL_TIME object.
			/** 
			 * \return Void pointer to the internal MYSQL_TIME object.
			 */
			void* getPointer() { return &internal; }
		};

		//! Handle conversion from MYSQL_TIME to Data::Date.
		template<> struct TypedConversion<Data::Date>: public TypedConversion<Data::Datetime>
		{
			//! Convert internal to a Data::Date object pointer to by external
			void convertResult();

			//! Convert a Data::Time object pointed to by external to internal.
			void convertParam();
		};

		//! Handle conversion from MYSQL_TIME to Data::Time.
		template<> struct TypedConversion<Data::Time>: public TypedConversion<Data::Datetime>
		{
			//! Convert internal to a Data::Time object pointer to by external
			void convertResult();

			//! Convert a Data::Time object pointed to by external to internal.
			void convertParam();
		};
		
		//! Handle retrieval and code conversion of utf-8 textual data.
		template<> struct TypedConversion<Data::Wtext>: public TypedConversion<Data::Blob>
		{
			/** 
			 * \param[in] column_ Associated column/parameter number.
			 * \param[in] statement_ Reference to %MySQL C API statement object.
			 * \param[out] buffer_ Reference to associated %MySQL bind object buffer pointer.
			 */
			TypedConversion(int column_, MYSQL_STMT* const& statement_, void*& buffer_): TypedConversion<Data::Blob>(column_, statement_, MYSQL_TYPE_STRING, buffer_) {}

			//! Retrieve the result and code convert it to a Data::Wtext pointed to by external.
			void convertResult();

			//! Code convert the Data::Wtext pointed to by external into buffer.
			void convertParam();
		private:
			//! Conversion buffer.
			std::vector<char> inputBuffer;
		};

		//! MySQL Error
		struct Error: public ASql::Error
		{
			Error(MYSQL* mysql);
			Error(MYSQL_STMT* stmt);
		};

		extern const char CodeConversionErrorMsg[];
	}
}

#endif
