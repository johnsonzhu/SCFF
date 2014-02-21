//! \file asql.hpp Declares the ASql namespace
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

#ifndef ASQL_HPP
#define ASQL_HPP

#include <vector>
#include <queue>
#include <cstring>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/scoped_array.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <asql/query.hpp>
#include <asql/data.hpp>

//! Defines classes and functions relating to SQL querying
namespace ASql
{
	/** 
	 * @brief Build a series of queries into a transaction.
	 *
	 * By using this class to execute a series of queries you let the ASql engine handle insuring they
	 * are executed sequentially. Should an error or cancellation crop up in one query all changes made
	 * by preceding queries in the transaction will be rolled back and proceeding queries in the transaction
	 * will be dumped from the queue.
	 *
	 * Note that if an error or cancellation occurs in a query that has no callback function, proceeding queries
	 * in the transaction will be checked for callback functions and the first one found will be called.
	 *
	 * Note that the statement obviously MUST all be from the same connection.
	 */
	template<class T> class Transaction
	{
	public:
		/** 
		 * @brief Ties query objects to their statements.
		 */
		struct Item
		{
			Item(QueryPar query, T* statement): m_query(query), m_statement(statement) {}
			Item(const Item& x): m_query(x.m_query), m_statement(x.m_statement) {}
			QueryPar m_query;
			T* m_statement;
		};
	private:
		std::vector<Item> m_items;
	public:
		typedef typename std::vector<Item>::iterator iterator;
		/** 
		 * @brief Add a query to the transaction.
		 *
		 * This adds the query to the back of the line.
		 * 
		 * @param query QueryPar to add.
		 * @param statement Associated statement.
		 */
		inline void push(QueryPar& query, T& statement) { m_items.push_back(Item(query, &statement)); }
		/** 
		 * @brief Remove all queries from the transaction.
		 */
		inline void clear() { m_items.clear(); }
		/** 
		 * @brief Return a starting iterator.
		 */
		inline iterator begin() { return m_items.begin(); }
		/** 
		 * @brief Return a end iterator.
		 */
		inline iterator end() { return m_items.end(); }
		/** 
		 * @brief Return true if the transaction is empty.
		 */
		inline bool empty() { return m_items.size()==0; }

		/** 
		 * @brief Cancel all queries in the transaction.
		 */
		void cancel();

		/** 
		 * @brief Initiate the transaction.
		 */
		void start(int instance=-1) { m_items.front().m_statement->connection.queue(*this, instance); }
	};

	/** 
	 * @brief %SQL %Connection.
	 */
	class Connection
	{
	public:
		//! Returns the number of threads
		int threads() const { return maxThreads; }

		//! Return true if the handler is running
		bool running() const { return m_threads; }
	protected:
		/** 
		 * @brief Number of threads to pool for simultaneous queries.
		 */
		const int maxThreads;
		boost::mutex threadsMutex;
		boost::condition_variable threadsChanged;
		int m_threads;

		virtual void commit(const unsigned int thread=0)=0;
		virtual void rollback(const unsigned int thread=0)=0;

		boost::scoped_array<boost::condition_variable> wakeUp;

		boost::mutex terminateMutex;
		bool terminateBool;

		Connection(const int maxThreads_): maxThreads(maxThreads_), m_threads(0), wakeUp(new boost::condition_variable[maxThreads_]) {}
	};

	/** 
	 * @brief Defines some functions and data types shared between ASql engines
	 */
	template<class T> class ConnectionPar: public Connection
	{
	private:
		struct QuerySet
		{
			QuerySet(QueryPar& query, T* const& statement, const bool commit): m_query(query), m_statement(statement), m_commit(commit) {}
			QueryPar m_query;
			bool m_commit;
			T* m_statement;
		};
		/** 
		 * @brief Thread safe queue of queries.
		 */
		class Queries: public std::queue<QuerySet>, public boost::mutex {};
		boost::scoped_array<Queries> queries;

		/** 
		 * @brief Function that runs in threads.
		 */
		void intHandler(const unsigned int id);

		/** 
		 * @brief Locks the mutex on a statement and set's the canceller to the queries canceller
		 */
		class SetCanceler
		{
			const bool*& m_canceler;
		public:
			SetCanceler(const bool*& canceler, bool& dest): m_canceler(canceler) { canceler=&dest; }
			~SetCanceler() { m_canceler=&s_false; }			
		};

	protected:
		ConnectionPar(const int maxThreads_): Connection(maxThreads_), queries(new Queries[maxThreads_]) {}
	public:
		//! Returns the number of queued queries
		int queriesSize() const; 

		/** 
		 * @brief Start all threads of the handler
		 */
		void start();
		/** 
		 * @brief Terminate all thread of the handler
		 */
		void terminate();
		/** 
		 * @brief Queue up a transaction for completion
		 */
		void queue(Transaction<T>& transaction, int instance);
		inline void queue(T* const& statement, QueryPar& query, int instance);

		static const bool s_false;
	};	

	/** 
	 * @brief SQL %Statement.
	 */
	class Statement
	{
	protected:
		boost::scoped_array<Data::Conversions> paramsConversions;
		boost::scoped_array<Data::Conversions> resultsConversions;
	
	Statement(unsigned int threads):
		paramsConversions(new Data::Conversions[threads]),
		resultsConversions(new Data::Conversions[threads]) {}
	};
}

template<class T> void ASql::ConnectionPar<T>::start()
{
	{
		boost::lock_guard<boost::mutex> terminateLock(terminateMutex);
		terminateBool=false;
	}
	
	boost::unique_lock<boost::mutex> threadsLock(threadsMutex);
	while(m_threads<maxThreads)
	{
		boost::thread(boost::bind(&ConnectionPar<T>::intHandler, boost::ref(*this), m_threads));
		threadsChanged.wait(threadsLock);
	}
}

template<class T> void ASql::ConnectionPar<T>::terminate()
{
	{
		boost::lock_guard<boost::mutex> terminateLock(terminateMutex);
		terminateBool=true;
	}
	for(boost::condition_variable* i=wakeUp.get(); i<wakeUp.get()+threads(); ++i)
		i->notify_all();

	boost::unique_lock<boost::mutex> threadsLock(threadsMutex);
	while(m_threads)
		threadsChanged.wait(threadsLock);
}

template<class T> void ASql::ConnectionPar<T>::intHandler(const unsigned int id)
{
	{
		boost::lock_guard<boost::mutex> threadsLock(threadsMutex);
		++m_threads;
	}
	threadsChanged.notify_one();
	
	boost::unique_lock<boost::mutex> terminateLock(terminateMutex, boost::defer_lock_t());
	boost::unique_lock<boost::mutex> queriesLock(queries[id], boost::defer_lock_t());

	while(1)
	{
		terminateLock.lock();
		if(terminateBool)
			break;
		terminateLock.unlock();
		
		queriesLock.lock();
		if(!queries[id].size())
		{
			wakeUp[id].wait(queriesLock);
			queriesLock.unlock();
			continue;
		}
		QuerySet querySet=queries[id].front();
		queries[id].pop();
		queriesLock.unlock();

		Error error;

		try
		{
			SetCanceler SetCanceler(querySet.m_statement->m_stop[id], querySet.m_query.m_sharedData->m_cancel);
			if(querySet.m_query.m_sharedData->m_flags & QueryPar::SharedData::FLAG_SINGLE_PARAMETERS)
			{
				if(querySet.m_query.m_sharedData->m_flags & QueryPar::SharedData::FLAG_SINGLE_RESULTS)
				{
					if(!querySet.m_statement->execute(static_cast<const Data::Set*>(querySet.m_query.parameters()), *static_cast<Data::Set*>(querySet.m_query.results()), false, id)) querySet.m_query.clearResults();
				}
				else
					querySet.m_statement->execute(static_cast<const Data::Set*>(querySet.m_query.parameters()), static_cast<Data::SetContainer*>(querySet.m_query.results()), querySet.m_query.m_sharedData->m_insertId, querySet.m_query.m_sharedData->m_rows, false, id);
			}
			else
			{
				querySet.m_statement->execute(*static_cast<const Data::SetContainer*>(querySet.m_query.parameters()), querySet.m_query.m_sharedData->m_rows, false, id);
			}

			if(querySet.m_commit)
				commit(id);

			querySet.m_query.m_sharedData->m_error=Error();
		}
		catch(const Error& e)
		{
			querySet.m_query.m_sharedData->m_error=e;

			rollback(id);

			queriesLock.lock();
			QuerySet tmpQuerySet=querySet;
			while(!querySet.m_commit && queries[id].size())
			{
				tmpQuerySet=queries[id].front();
				queries[id].pop();
				if(!querySet.m_query.isCallback() && tmpQuerySet.m_query.isCallback())
					querySet.m_query.setCallback(tmpQuerySet.m_query.getCallback());

			}
			queriesLock.unlock();
		}

		querySet.m_query.callback();
	}

	{
		boost::lock_guard<boost::mutex> threadsLock(threadsMutex);
		--m_threads;
	}
	threadsChanged.notify_one();
}

template<class T> void ASql::ConnectionPar<T>::queue(T* const& statement, QueryPar& query, int instance)
{
	if(instance == -1)
	{
		instance=0;
		for(unsigned int i=1; i<threads(); ++i)
		{{
			boost::lock_guard<boost::mutex> queriesLock(queries[i]);
			if(queries[i].size() < queries[instance].size())
				instance=i;
		}}
	}

	boost::lock_guard<boost::mutex> queriesLock(queries[instance]);
	queries[instance].push(QuerySet(query, statement, true));
	wakeUp[instance].notify_one();
}

template<class T> const bool ASql::ConnectionPar<T>::s_false = false;

template<class T> void ASql::ConnectionPar<T>::queue(Transaction<T>& transaction, int instance)
{
	if(instance == -1)
	{
		instance=0;
		for(unsigned int i=1; i<threads(); ++i)
		{{
			boost::lock_guard<boost::mutex> queriesLock(queries[i]);
			if(queries[i].size() < queries[instance].size())
				instance=i;
		}}
	}

	boost::lock_guard<boost::mutex> queriesLock(queries[instance]);

	for(typename Transaction<T>::iterator it=transaction.begin(); it!=transaction.end(); ++it)
		queries[instance].push(QuerySet(it->m_query, it->m_statement, false));
	queries[instance].back().m_commit = true;

	wakeUp[instance].notify_one();
}

template<class T> void ASql::Transaction<T>::cancel()
{
	for(iterator it=begin(); it!=end(); ++it)
		it->m_query.cancel();
}

template<class T> int ASql::ConnectionPar<T>::queriesSize() const
{
	int size=0;
	for(unsigned int i=1; i<threads(); ++i)
	{{
		boost::lock_guard<boost::mutex> queriesLock(queries[i]);
		size += queries[i].size();
	}}
	
	return size;
}

#endif
