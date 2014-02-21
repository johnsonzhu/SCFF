#ifndef ASQLQUERY_HPP
#define ASQLQUERY_HPP

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/enable_if.hpp>

#include <asql/data.hpp>
#include <asql/exception.hpp>

namespace ASql
{
	//! Parent class for storing query data to be passed and retrieved from statements.
	/*!
	 * Most of the data is stored in a shared data structure that is referenced
	 * by copies of a QueryPar object. The only non-shared data stored in the
	 * class is the m_flags object which set's two possible flags. If the query
	 * is built from the default constructor, the FLAG_ORIGINAL flag is set. This
	 * serves the purpose of keeping track of which query object actually owns
	 * the original data. With this, one can have the query cancelled in it's
	 * handler thread if the original query object goes out of scope in it's own
	 * thread.  If one wishes to avoid this behaviour they can have the
	 * FLAG_KEEPALIVE flag set with the keepAlive() function.
	 *
	 * \sa Query
	 */
	class QueryPar
	{
	private:
		//! Sub-structure to store shared data for the query
		struct SharedData
		{
			//! Contains the possible flags a query can have set.
			enum Flags { FLAG_SINGLE_PARAMETERS=1, FLAG_SINGLE_RESULTS=1<<1 };

			//! Only constructor
			/*! 
			 * The default constructor zeros all data and sets the shared flags to 0
			 */
			SharedData(): m_parameters(0), m_results(0), m_insertId(0), m_rows(0), m_cancel(false), m_flags(0) {}
			/** 
			 * @brief Destroys the shared data.
			 *
			 * This should obviously only get called when at last all associated QueryPar objects go out of scope.
			 */

			~SharedData()
			{
				delete m_rows;
				delete m_insertId;
				destroyResults();
				destroyParameters();
			}

			//! Pointer to the data object containing the parameters to be passed to the statement.
			/*! 
			 * This is a void pointer so that it may contain a single row of parameters (Data::Set) or multiple rows of parameters
			 * (Data::SetContainer). Multiple rows of parameters is in effect just executing the query multiple times in a single
			 * transaction.
			 */
			void* m_parameters;

			//! Pointer to the data object that should store the results returned from the statement.
			/*!
			 * This is a void pointer so that it may contain a single row of results (Data::Set) or multiple rows of results
			 * (Data::SetContainer).
			 */
			void* m_results;

			//! Used to return a potential last auto increment insert id.
			unsigned long long int* m_insertId;
			/** 
			 * @brief Used to return the number of rows available/affected by the query.
			 */

			unsigned long long int* m_rows;

			//! Contains the error (if any) returned by the query.
			Error m_error;

			//! A callback function to be called when the query completes.
			boost::function<void()> m_callback;

			boost::mutex m_callbackMutex;

			//! If set true the query should cancel when the opportunity arises.
			bool m_cancel;

			//! flags for the shared data
			/*! 
			 * FLAG_SINGLE_RESULTS: This means that the data pointed to be m_results is a Data::Set
			 * and not a Data::SetContainer.
			 *
			 * FLAG_SINGLE_RESULTS: This means that the data pointed to be m_parameters is a Data::Set
			 * and not a Data::SetContainer.
			 */
			unsigned char m_flags;

			//! Safely destroys(or doesn't) the result data set.
			/*! 
			 * This will destroy the result set and set the m_results pointer to null.
			 */
			void destroyResults()
			{
				if(m_flags&FLAG_SINGLE_RESULTS)
					delete static_cast<Data::Set*>(m_results);
				else
					delete static_cast<Data::SetContainer*>(m_results);
				m_results = 0;
			}

			//! Safely destroys(or doesn't) the parameter data set.
			/*! 
			 * This will destroy the parameter set and set the m_parameters pointer to null.
			 */
			void destroyParameters()
			{
				if(m_flags&FLAG_SINGLE_PARAMETERS)
					delete static_cast<Data::Set*>(m_parameters);
				else
					delete static_cast<Data::SetContainer*>(m_parameters);
				m_parameters = 0;
			}
		};

		//! Shared pointer to query data.
		boost::shared_ptr<SharedData> m_sharedData;

		//! Flags for individual query object.
		/*! 
		 * FLAG_ORIGINAL: Set only if this query object was constructed with the default constructor. Never set if constructed from the
		 * copy constructor.
		 *
		 *	FLAG_KEEPALIVE: Set if the query should not be cancelled if the original object goes out of scope before the query is complete.
		 */
		unsigned char m_flags;

		enum Flags { FLAG_ORIGINAL=1, FLAG_KEEPALIVE=1<<1 };

		//! Lock and call the callback function of it exists.
		void callback()
		{
			boost::lock_guard<boost::mutex> lock(m_sharedData->m_callbackMutex);
			if(!m_sharedData->m_callback.empty())
				m_sharedData->m_callback();
		}

		void clearResults() { m_sharedData->destroyResults(); } 
		void clearParameters() { m_sharedData->destroyParameters(); } 

		template<class T> friend class ConnectionPar;

	protected:
		//! Constructed by derived classes
		/*!
		 * \param[in] singleParameters Set to true if the parameter set is a single row
		 * \param[in] singleResults Set to true if the result set is a single row
		 */
		QueryPar(bool singleParameters, bool singleResults): m_sharedData(new SharedData), m_flags(FLAG_ORIGINAL)
		{
			if(singleParameters)
				m_sharedData->m_flags|=SharedData::FLAG_SINGLE_PARAMETERS;
			if(singleResults)
				m_sharedData->m_flags|=SharedData::FLAG_SINGLE_RESULTS;
		}

		//! Set's the shared data to point to the passed pointer.
		/*!
		 * Note that the QueryPar class assumes responsibility for destroying the
		 * result set unless explicitly released with relinquishResults().
		 *
		 * \param[out] results A pointer to the result data
		 */
		void setResults(void* results) { m_sharedData->destroyResults(); m_sharedData->m_results=results; }

		//! Set's the shared data to point to the passed pointer.
		/*!
		 * Note that the QueryPar class assumes responsibility for destroying the
		 * parameter set unless explicitly released with relinquishParameters().
		 *
		 * \param[in] parameters A pointer to the parameter data
		 */
		void setParameters(void* parameters) { m_sharedData->destroyParameters(); m_sharedData->m_parameters=parameters; }

		//! Relinquishes control over the result data.
		/*!
		 * Calling this will disassociate the query object (and it's copies) from
		 * the result data but won't delete it. Use this if you are done with your
		 * query object but want to keep your result data around. The end result in
		 * the query object (and it's copies) is to have no result data associated
		 * with it.
		 *
		 * \return Pointer to the disassociated result data
		 */
		void* relinquishResults() { void* data = m_sharedData->m_results; m_sharedData->m_results=0; return data; }

		//! Relinquishes control over the parameter data.
		/*!
		 * Calling this will disassociate the query object (and it's copies) from
		 * the parameter data but won't delete it. Use this if you are done with
		 * your query object but want to keep your parameter data around. The end
		 * result in the query object (and it's copies) is to have no parameter
		 * data associated with it.
		 *
		 * \return Pointer to the disassociated parameter data
		 */
		void* relinquishParameters() { void* data = m_sharedData->m_parameters; m_sharedData->m_parameters=0; return data; }

		//! Return a void pointer to the result set.
		/*!
		 * Usually you would keep the original typed pointer around but if you
		 * lost it for some reason you can use this and cast it.
		 */
		void* results() { return m_sharedData->m_results; }

		//! Return a const void pointer to the result set.
		/*!
		 * Usually you would keep the original typed pointer around but if you
		 * lost it for some reason you can use this and cast it.
		 */
		const void* results() const { return m_sharedData->m_results; }

		//! Return a void pointer to the parameter set.
		/*!
		 * Usually you would keep the original typed pointer around but if you
		 * lost it for some reason you can use this and cast it.
		 */
		void* parameters() { return m_sharedData->m_parameters; }

		//! Return a const void pointer to the parameter set.
		/*!
		 * Usually you would keep the original typed pointer around but if you
		 * lost it for some reason you can use this and cast it.
		 */
		const void* parameters() const { return m_sharedData->m_parameters; }

	public:
		 //! Calling the copy constructor will not set FLAG_ORIGINAL or create new shared data.
		 /*! 
		 * @param x May be a copy or original QueryPar object.
		 */
		QueryPar(const QueryPar& x): m_sharedData(x.m_sharedData), m_flags(0) {}

		 //! If the original object and keepalive is not turned on, cancel() is called.
		~QueryPar()
		{
			// This might seem like we aren't checking for keepalive, but we are
			if(m_flags == FLAG_ORIGINAL)
				cancel();
		}

		//! Returns the insert ID returned from the query or 0 if nothing.
		unsigned int insertId() const { return m_sharedData->m_insertId?*(m_sharedData->m_insertId):0; }

		//! Returns the rows affected/available from the query or 0 if nothing.
		/*!
		 * Note that if using for the number of rows from a query this will
		 * represent the number of rows available before a LIMIT.
		 */
		unsigned int rows() const { return m_sharedData->m_rows?*(m_sharedData->m_rows):0; }

		//! Returns true if copies of this query still exist (query is still working in another thread).
		bool busy() const { return m_sharedData.use_count() != 1; }

		//! Return the error object associated with the query.
		/*!
		 * Note this will be a default Error object if there was no error.
		 */
		Error error() const { return m_sharedData->m_error; }

		//! Set the callback function to be called at the end of the query.
		/*! 
		 * Note that this will be called even if there is an error or the query can cancelled.
		 */
		void setCallback(boost::function<void()> callback=boost::function<void()>())
		{
			boost::lock_guard<boost::mutex> lock(m_sharedData->m_callbackMutex);
			m_sharedData->m_callback = callback;
		}

		//! Return true if a callback is associated with this query
		bool isCallback() { return !m_sharedData->m_callback.empty(); }

		//! Get the callback function
		boost::function<void()> getCallback()
		{
			return m_sharedData->m_callback;
		}

		//! Set true if you want the query to not be cancelled when the original object is destroyed.
		/*! 
		 * Note that the default is to cancel and this must be called from the original query object.
		 */
		void keepAlive(bool x) { if(x) m_flags|=FLAG_KEEPALIVE; else m_flags&=~FLAG_KEEPALIVE; }

		//! Call this function to cancel the query.
		/*! 
		 * This will cancel the query at the earliest opportunity. Calling a
		 * cancel will rollback any changes in the associated transaction.
		 */
		void cancel() { m_sharedData->m_cancel = true; }

		//! Call this function to enable the retrieval of a row count (affected/available rows)
		void enableRows()
		{
			if(!m_sharedData->m_rows)
				m_sharedData->m_rows = new unsigned long long;
		}

		//! Call this function to enable the retrieval of an auto-increment insert ID
		void enableInsertId()
		{
			if(!m_sharedData->m_insertId)
				m_sharedData->m_insertId = new unsigned long long;
		}

		//! Resets the object as though it was destroyed and reconstructed anew
		void reset()
		{
			unsigned char flags = m_sharedData->m_flags;
			m_sharedData.reset(new SharedData);
			m_sharedData->m_flags = flags;

			m_flags=FLAG_ORIGINAL;
			keepAlive(false);
		}
	};

	//! Class for storing query data to be passed to and retrieved from statements.
	/*! 
	 * This will contain all data associated with an individual query. With it
	 * one can retrieve results, set arguments, and set various query parameters
	 * that can be checked/set with the public access functions.
	 *
	 * This class needs to be given two template parameters regarding its results
	 * and parameters. The types may be derived from either Data::Set or
	 * Data::SetContainer. The class will be specialized accordingly. Should
	 * there be no parameters (or results) simply pass a type void for that
	 * parameter.
	 *
	 * \tparam Parameters Parameters type
	 * \tparam Results Results type
	 * \tparam ParametersParent Parent type of the parameters. This gets
	 * automatically set and used for specialization.
	 * \tparam ResultsParent Parent type of the results. This gets
	 * automatically set and used for specialization.
	 */
	template<class Parameters = void, class Results = void, class ParametersParent = void, class ResultsParent = void>
	struct Query: public QueryPar
	{
		// This default template should never be instanced
	private:
		Query() {}
		Query(const Query& x) {}
	};

	//! Query specialization for no results or parameters
	template<> struct Query<void, void, void, void>: public QueryPar
	{
		Query(): QueryPar(true, false) {}
		Query(const Query& x): QueryPar(x) {}
	};

	//! Query specialization for results of Data::Set type and parameters of Data::Set type
	/*!
	 * \tparam Parameters Type used to store parameter data. This specialization cover those types derived from Data::Set
	 * \tparam Results Type used to store result data. This specialization cover those types derived from Data::Set
	 */
	template<class Parameters, class Results>
	struct Query<Parameters, Results, typename boost::enable_if<boost::is_base_of<Data::Set, Parameters> >::type,  typename boost::enable_if<boost::is_base_of<Data::Set, Results> >::type>: public QueryPar
	{
		Query(): QueryPar(true, true) {}
		Query(const Query& x): QueryPar(x) {}

		//! Takes a QueryPar reference and return a statically generated Query reference
		static Query& staticRebuild(QueryPar& x) { return static_cast<Query&>(x); }

		//! Takes a QueryPar reference and return a dynamically generated Query reference
		static Query& dynamicRebuild(QueryPar& x) { return dynamic_cast<Query&>(x); }

		//! Create the parameter set for the query object
		/** 
		 * \return A reference to the newly created parameter set.
		 */
		Parameters& createParameters() { Parameters* parameters=new Parameters; QueryPar::setParameters(parameters); return *parameters; }

		//! Set's the parameter data to point to the passed single row parameter set.
		/*!
		 * Note that the Query object assumes responsibility for destroying the
		 * parameter set unless explicitly released with relinquishParameters().
		 *
		 * \param[in] parameters A pointer to the parameter data set
		 */
		void setParameters(Parameters* parameters) { QueryPar::setParameters(parameters); }

		//! Returns a pointer to the parameter set
		Parameters* parameters() { return static_cast<Parameters*>((ASql::Data::Set*)(QueryPar::parameters())); }

		//! Returns a const pointer to the parameter set
		const Parameters* parameters() const { return static_cast<const Parameters*>((const ASql::Data::Set*)(QueryPar::parameters())); }

		//! Relinquishes control over the parameter set.
		/*!
		 * Calling this will disassociate the query object (and it's copies) from
		 * the parameter set but won't delete it. Use this if you are done with
		 * your query object but want to keep your parameter set around. The end
		 * result in the query object (and it's copies) is to have no parameter
		 * set associated with it.
		 *
		 * \return Pointer to the disassociated parameter set
		 */
		Parameters* relinquishParameters() {  return static_cast<Parameters*>((ASql::Data::Set*)(QueryPar::relinquishParameters())); }

		//! Create the result set for the query object
		/** 
		 * \return A reference to the newly created result set.
		 */
		Results& createResults() { Results* results=new Results; QueryPar::setResults(results); return *results; }

		//! Set's the result data to point to the passed single row result set.
		/*!
		 * Note that the Query object assumes responsibility for destroying the
		 * result set unless explicitly released with relinquishResults().
		 *
		 * \param[in] results A pointer to the result data set
		 */
		void setResults(Results* results) { QueryPar::setResults(results); }

		//! Returns a pointer to the result set
		Results* results() { return static_cast<Results*>((ASql::Data::Set*)(QueryPar::results())); }

		//! Returns a const pointer to the result set
		const Results* results() const { return static_cast<const Results*>((const ASql::Data::Set*)(QueryPar::results())); }

		//! Relinquishes control over the result set.
		/*!
		 * Calling this will disassociate the query object (and it's copies) from
		 * the result set but won't delete it. Use this if you are done with
		 * your query object but want to keep your result set around. The end
		 * result in the query object (and it's copies) is to have no result
		 * set associated with it.
		 *
		 * \return Pointer to the disassociated result set
		 */
		Results* relinquishResults() { return static_cast<Results*>((ASql::Data::Set*)(QueryPar::relinquishResults())); }
	};

	//! Query specialization for results of Data::SetContainer type and parameters of Data::Set type
	/*!
	 * \tparam Parameters Type used to store parameter data. This specialization cover those types derived from Data::Set
	 * \tparam Results Type used to store result data. This specialization cover those types derived from Data::SetContainer
	 */
	template<class Parameters, class Results>
	struct Query<Parameters, Results, typename boost::enable_if<boost::is_base_of<Data::Set, Parameters> >::type,  typename boost::enable_if<boost::is_base_of<Data::SetContainer, Results> >::type>: public QueryPar
	{
		Query(): QueryPar(true, false) {}
		Query(const Query& x): QueryPar(x) {}

		//! Takes a QueryPar reference and return a statically generated Query reference
		static Query& staticRebuild(QueryPar& x) { return static_cast<Query&>(x); }

		//! Takes a QueryPar reference and return a dynamically generated Query reference
		static Query& dynamicRebuild(QueryPar& x) { return dynamic_cast<Query&>(x); }

		//! Create the parameter set for the query object
		/** 
		 * \return A reference to the newly created parameter set.
		 */
		Parameters& createParameters() { Parameters* parameters=new Parameters; QueryPar::setParameters(parameters); return *parameters; }

		//! Set's the parameter data to point to the passed single row parameter set.
		/*!
		 * Note that the Query object assumes responsibility for destroying the
		 * parameter set unless explicitly released with relinquishParameters().
		 *
		 * \param[in] parameters A pointer to the parameter data set
		 */
		void setParameters(Parameters* parameters) { QueryPar::setParameters(parameters); }

		//! Returns a pointer to the parameter set
		Parameters* parameters() { return static_cast<Parameters*>((ASql::Data::Set*)(QueryPar::parameters())); }

		//! Returns a const pointer to the parameter set
		const Parameters* parameters() const { return static_cast<const Parameters*>((const ASql::Data::Set*)(QueryPar::parameters())); }

		//! Relinquishes control over the parameter set.
		/*!
		 * Calling this will disassociate the query object (and it's copies) from
		 * the parameter set but won't delete it. Use this if you are done with
		 * your query object but want to keep your parameter set around. The end
		 * result in the query object (and it's copies) is to have no parameter
		 * set associated with it.
		 *
		 * \return Pointer to the disassociated parameter set
		 */
		Parameters* relinquishParameters() {  return static_cast<Parameters*>((ASql::Data::Set*)(QueryPar::relinquishParameters())); }

		//! Create the result set container for the query object
		/** 
		 * \return A reference to the newly created result set container.
		 */
		Results& createResults() { Results* results=new Results; QueryPar::setResults(results); return *results; }

		//! Set's the result data to point to the passed single row result set container.
		/*!
		 * Note that the Query object assumes responsibility for destroying the
		 * result set container unless explicitly released with relinquishResults().
		 *
		 * \param[in] results A pointer to the result data set container
		 */
		void setResults(Results* results) { QueryPar::setResults(results); }

		//! Returns a pointer to the result set container
		Results* results() { return static_cast<Results*>((ASql::Data::SetContainer*)(QueryPar::results())); }

		//! Returns a const pointer to the result set container
		const Results* results() const { return static_cast<const Results*>((const ASql::Data::SetContainer*)(QueryPar::results())); }

		//! Relinquishes control over the result set container.
		/*!
		 * Calling this will disassociate the query object (and it's copies) from
		 * the result set container but won't delete it. Use this if you are done with
		 * your query object but want to keep your result set container around. The end
		 * result in the query object (and it's copies) is to have no result
		 * set container associated with it.
		 *
		 * \return Pointer to the disassociated result set container
		 */
		Results* relinquishResults() { return static_cast<Results*>((ASql::Data::SetContainer*)(QueryPar::relinquishResults())); }
	};

	//! Query specialization for results of Data::Set type and empty parameters (type void)
	/*!
	 * \tparam Results Type used to store result data. This specialization cover those types derived from Data::Set
	 */
	template<class Results>
	struct Query<void, Results, void,  typename boost::enable_if<boost::is_base_of<Data::Set, Results> >::type>: public QueryPar
	{
		Query(): QueryPar(true, true) {}
		Query(const Query& x): QueryPar(x) {}

		//! Takes a QueryPar reference and return a statically generated Query reference
		static Query& staticRebuild(QueryPar& x) { return static_cast<Query&>(x); }

		//! Takes a QueryPar reference and return a dynamically generated Query reference
		static Query& dynamicRebuild(QueryPar& x) { return dynamic_cast<Query&>(x); }

		//! Create the result set for the query object
		/** 
		 * \return A reference to the newly created result set.
		 */
		Results& createResults() { Results* results=new Results; QueryPar::setResults(results); return *results; }

		//! Set's the result data to point to the passed single row result set.
		/*!
		 * Note that the Query object assumes responsibility for destroying the
		 * result set unless explicitly released with relinquishResults().
		 *
		 * \param[in] results A pointer to the result data set
		 */
		void setResults(Results* results) { QueryPar::setResults(results); }

		//! Returns a pointer to the result set
		Results* results() { return static_cast<Results*>((ASql::Data::Set*)(QueryPar::results())); }

		//! Returns a const pointer to the result set
		const Results* results() const { return static_cast<const Results*>((const ASql::Data::Set*)(QueryPar::results())); }

		//! Relinquishes control over the result set.
		/*!
		 * Calling this will disassociate the query object (and it's copies) from
		 * the result set but won't delete it. Use this if you are done with
		 * your query object but want to keep your result set around. The end
		 * result in the query object (and it's copies) is to have no result
		 * set associated with it.
		 *
		 * \return Pointer to the disassociated result set
		 */
		Results* relinquishResults() { return static_cast<Results*>((ASql::Data::Set*)(QueryPar::relinquishResults())); }
	};

	//! Query specialization for results of Data::SetContainer type and empty parameters (type void)
	/*!
	 * \tparam Results Type used to store result data. This specialization cover those types derived from Data::SetContainer
	 */
	template<class Results>
	struct Query<void, Results, void,  typename boost::enable_if<boost::is_base_of<Data::SetContainer, Results> >::type>: public QueryPar
	{
		Query(): QueryPar(true, false) {}
		Query(const Query& x): QueryPar(x) {}

		//! Takes a QueryPar reference and return a statically generated Query reference
		static Query& staticRebuild(QueryPar& x) { return static_cast<Query&>(x); }

		//! Takes a QueryPar reference and return a dynamically generated Query reference
		static Query& dynamicRebuild(QueryPar& x) { return dynamic_cast<Query&>(x); }

		//! Create the result set container for the query object
		/** 
		 * \return A reference to the newly created result set container.
		 */
		Results& createResults() { Results* results=new Results; QueryPar::setResults(results); return *results; }

		//! Set's the result data to point to the passed single row result set container.
		/*!
		 * Note that the Query object assumes responsibility for destroying the
		 * result set container unless explicitly released with relinquishResults().
		 *
		 * \param[in] results A pointer to the result data set container
		 */
		void setResults(Results* results) { QueryPar::setResults(results); }

		//! Returns a pointer to the result set container
		Results* results() { return static_cast<Results*>((ASql::Data::SetContainer*)(QueryPar::results())); }

		//! Returns a const pointer to the result set container
		const Results* results() const { return static_cast<const Results*>((const ASql::Data::SetContainer*)(QueryPar::results())); }

		//! Relinquishes control over the result set container.
		/*!
		 * Calling this will disassociate the query object (and it's copies) from
		 * the result set container but won't delete it. Use this if you are done with
		 * your query object but want to keep your result set container around. The end
		 * result in the query object (and it's copies) is to have no result
		 * set container associated with it.
		 *
		 * \return Pointer to the disassociated result set container
		 */
		Results* relinquishResults() { return static_cast<Results*>((ASql::Data::SetContainer*)(QueryPar::relinquishResults())); }
	};

	//! Query specialization for no results (type void) and parameters of Data::Set type
	/*!
	 * \tparam Parameters Type used to store parameter data. This specialization cover those types derived from Data::Set
	 */
	template<class Parameters>
	struct Query<Parameters, void, typename boost::enable_if<boost::is_base_of<Data::Set, Parameters> >::type, void>: public QueryPar
	{
		Query(): QueryPar(true, false) {}
		Query(const Query& x): QueryPar(x) {}

		//! Takes a QueryPar reference and return a statically generated Query reference
		static Query& staticRebuild(QueryPar& x) { return static_cast<Query&>(x); }

		//! Takes a QueryPar reference and return a dynamically generated Query reference
		static Query& dynamicRebuild(QueryPar& x) { return dynamic_cast<Query&>(x); }

		//! Create the parameter set for the query object
		/** 
		 * \return A reference to the newly created parameter set.
		 */
		Parameters& createParameters() { Parameters* parameters=new Parameters; QueryPar::setParameters(parameters); return *parameters; }

		//! Returns a const pointer to the parameter set
		const Parameters* parameters() const { return static_cast<const Parameters*>((const ASql::Data::Set*)(QueryPar::parameters())); }

		//! Set's the parameter data to point to the passed single row parameter set.
		/*!
		 * Note that the Query object assumes responsibility for destroying the
		 * parameter set unless explicitly released with relinquishParameters().
		 *
		 * \param[in] parameters A pointer to the parameter data set
		 */
		void setParameters(Parameters* parameters) { QueryPar::setParameters(parameters); }

		//! Returns a pointer to the parameter set
		Parameters* parameters() { return static_cast<Parameters*>((ASql::Data::Set*)(QueryPar::parameters()));; }

		//! Relinquishes control over the parameter set.
		/*!
		 * Calling this will disassociate the query object (and it's copies) from
		 * the parameter set but won't delete it. Use this if you are done with
		 * your query object but want to keep your parameter set around. The end
		 * result in the query object (and it's copies) is to have no parameter
		 * set associated with it.
		 *
		 * \return Pointer to the disassociated parameter set
		 */
		Parameters* relinquishParameters() {  return static_cast<Parameters*>((ASql::Data::Set*)(QueryPar::relinquishParameters())); }
	};

	//! Query specialization for no results (type void) and parameters of Data::SetContainer type
	/*!
	 * \tparam Parameters Type used to store parameter data. This specialization cover those types derived from Data::SetContainer
	 */
	template<class Parameters>
	struct Query<Parameters, void, typename boost::enable_if<boost::is_base_of<Data::SetContainer, Parameters> >::type, void>: public QueryPar
	{
		Query(): QueryPar(false, false) {}
		Query(const Query& x): QueryPar(x) {}

		//! Takes a QueryPar reference and return a statically generated Query reference
		static Query& staticRebuild(QueryPar& x) { return static_cast<Query&>(x); }

		//! Takes a QueryPar reference and return a dynamically generated Query reference
		static Query& dynamicRebuild(QueryPar& x) { return dynamic_cast<Query&>(x); }

		//! Create the parameter set container for the query object
		/** 
		 * \return A reference to the newly created parameter set container.
		 */
		Parameters& createParameters() { Parameters* parameters=new Parameters; QueryPar::setParameters(parameters); return *parameters; }

		//! Set's the parameter data to point to the passed single row parameter set container.
		/*!
		 * Note that the Query object assumes responsibility for destroying the
		 * parameter set container unless explicitly released with relinquishParameters().
		 *
		 * \param[in] parameters A pointer to the parameter data set container
		 */
		void setParameters(Parameters* parameters) { QueryPar::setParameters(parameters); }

		//! Returns a pointer to the parameter set container
		Parameters* parameters() { return static_cast<Parameters*>((ASql::Data::SetContainer*)(QueryPar::parameters())); }

		//! Returns a const pointer to the parameter set
		const Parameters* parameters() const { return static_cast<const Parameters*>((const ASql::Data::SetContainer*)(QueryPar::parameters())); }

		//! Relinquishes control over the parameter set container.
		/*!
		 * Calling this will disassociate the query object (and it's copies) from
		 * the parameter set container but won't delete it. Use this if you are done with
		 * your query object but want to keep your parameter set container around. The end
		 * result in the query object (and it's copies) is to have no parameter
		 * set container associated with it.
		 *
		 * \return Pointer to the disassociated parameter set container
		 */
		Parameters* relinquishParameters() {  return static_cast<Parameters*>((ASql::Data::SetContainer*)(QueryPar::relinquishParameters())); }
	};
}

#endif
