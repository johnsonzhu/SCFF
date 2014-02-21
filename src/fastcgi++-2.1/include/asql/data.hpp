#ifndef ASQLDATA_HPP
#define ASQLDATA_HPP

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <ostream>
#include <vector>
#include <string>
#include <map>

#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#define ASQL_BUILDSETLINE(r, data, i, elem) case i: return elem;
//! Build the appropriate function to make a Data::Set compatible data structure
#define ASQL_BUILDSET(elements) \
	size_t numberOfSqlElements() const { return BOOST_PP_SEQ_SIZE(elements); } \
	ASql::Data::Index getSqlIndex(size_t index) const \
	{ \
		switch(index) \
		{ \
			BOOST_PP_SEQ_FOR_EACH_I(ASQL_BUILDSETLINE, 0, elements) \
			default: return ASql::Data::Index(); \
		} \
	}

namespace ASql
{
	//! Defines data types and conversion techniques standard to the fastcgipp %SQL facilities.
	namespace Data
	{
		//! Defines data types supported by the fastcgi++ sql facilities.
		/*!
		 * This enumeration provides runtime type identification capabilities to classes derived from
		 * the Set class. All types starting with U_ mean unsigned and all types ending will _N means
		 * they can store null values via the Nullable class.
		 */
		enum Type { U_TINY=0,
						U_SHORT,
						U_INT,
						U_BIGINT,
						TINY,
						SHORT,
						INT,
						BIGINT,
						FLOAT,
						DOUBLE,
						TIME,
						DATE,
						DATETIME,
						BLOB,
						TEXT,
						WTEXT,
						CHAR,
						BINARY,
						BIT,		
						U_TINY_N,
						U_SHORT_N,
						U_INT_N,
						U_BIGINT_N,
						TINY_N,
						SHORT_N,
						INT_N,
						BIGINT_N,
						FLOAT_N,
						DOUBLE_N,
						TIME_N,
						DATE_N,
						DATETIME_N,
						BLOB_N,
						TEXT_N,
						WTEXT_N,
						CHAR_N,
						BINARY_N,
						BIT_N,
						NOTHING };
		
		//! Base class to the Nullable template class.
		/*!
		 * This base class provides a polymorphic method of retrieving a void pointer to the contained
		 * object regardless of it's type along with it's nullness.
		 *
		 * If nullness is true then the value is null.
		 */
		struct NullablePar
		{
			NullablePar(bool _nullness): nullness(_nullness) { }
			bool nullness;
			//! Retrieve a void pointer to the object contained in the class.
			/*! 
			 * \return Void pointer to the object contained in the class.
			 */
			virtual void* getVoid() =0;
		};
		
		//! Class for adding null capabilities to any type. Needed for SQL queries involving
		template<class T> struct Nullable: public NullablePar
		{
			T object;
			void* getVoid() { return &object; }
			operator T() { return object; }
			operator const T() const { return object; }
			Nullable<T>& operator=(const T& x) { object=x; nullness=false; return *this; }
			Nullable(): NullablePar(true) {}
			Nullable(const T& x): NullablePar(false), object(x) { }
		};

		//! Class for adding null capabilities to character arrays.
		template<class T, int size> struct NullableArray: public NullablePar
		{
			T object[size];
			void* getVoid() { return object; }
			operator T*() { return object; }
			NullableArray(): NullablePar(false) {}
			NullableArray(const T& x): NullablePar(false), object(x) { }
		};

		//! A basic, practically none-functional stream inserter for Nullable objects.
		template<class charT, class Traits, class T> inline std::basic_ostream<charT, Traits>& operator<<(std::basic_ostream<charT, Traits>& os, const Nullable<T>& x)
		{
			if(x.nullness)
				os << "NULL";
			else
				os << x.object;

			return os;
		}

		typedef unsigned char Utiny;
		typedef signed char Tiny;
		typedef unsigned short int Ushort;
		typedef short int Short;
		typedef unsigned int Uint;
		typedef int Int;
		typedef unsigned long long int Ubigint;
		typedef long long int Bigint;
		typedef float Float;
		typedef double Double;
		typedef boost::posix_time::time_duration Time;
		typedef boost::gregorian::date Date;
		typedef boost::posix_time::ptime Datetime;
		typedef std::string Text;
		typedef std::wstring Wtext;
		typedef bool Boolean;
		//! Derive from this to create your own Blob types
		/*!
		 * All derivations must provide the three virtual functions and insure
		 * that they behave the same as std::vector does. This means that the data
		 * must be contiguous.
		 */
		struct Blob
		{
			virtual size_t size() const =0;
			virtual void resize(const size_t size) =0;
			virtual char& operator[](const size_t index) =0;
		};
		struct VectorBlob: public Blob
		{
			std::vector<char>& m_data;
			VectorBlob(std::vector<char>& data): m_data(data) {}
			size_t size() const { return m_data.size(); }
			void resize(const size_t size) { m_data.resize(size); };
			char& operator[](const size_t index) { return m_data[index]; };
		};

		typedef Nullable<Utiny> UtinyN;
		typedef Nullable<Tiny> TinyN;
		typedef Nullable<Ushort> UshortN;
		typedef Nullable<Short> ShortN;
		typedef Nullable<Uint> UintN;
		typedef Nullable<Int> IntN;
		typedef Nullable<Ubigint> UbigintN;
		typedef Nullable<Bigint> BigintN;
		typedef Nullable<Float> FloatN;
		typedef Nullable<Double> DoubleN;
		typedef Nullable<Time> TimeN;
		typedef Nullable<Date> DateN;
		typedef Nullable<Datetime> DatetimeN;
		typedef Nullable<Text> TextN;
		typedef Nullable<Wtext> WtextN;
		typedef Nullable<Boolean> BooleanN;

		//! Stores on index value from a Set
		/*! 
		 * All of the constructors allow for implicit construction upon return from 
		 * Set::getSqlIndex() except for the templated generic binary ones.
		 */
		struct Index
		{
			Type type;
			void* data;
			size_t size;

			Index(const Utiny& x): type(U_TINY), data(const_cast<Utiny*>(&x)) { }
			Index(const Tiny& x): type(TINY), data(const_cast<Tiny*>(&x)) { }
			Index(const Ushort& x): type(U_SHORT), data(const_cast<Ushort*>(&x)) { }
			Index(const Short& x): type(SHORT), data(const_cast<Short*>(&x)) { }
			Index(const Uint& x): type(U_INT), data(const_cast<Uint*>(&x)) { }
			Index(const Int& x): type(INT), data(const_cast<Int*>(&x)) { }
			Index(const Ubigint& x): type(U_BIGINT), data(const_cast<Ubigint*>(&x)) { }
			Index(const Bigint& x): type(BIGINT), data(const_cast<Bigint*>(&x)) { }
			Index(const Float& x): type(FLOAT), data(const_cast<Float*>(&x)) { }
			Index(const Double& x): type(DOUBLE), data(const_cast<Double*>(&x)) { }
			Index(const Time& x): type(TIME), data(const_cast<Time*>(&x)) { }
			Index(const Date& x): type(DATE), data(const_cast<Date*>(&x)) { }
			Index(const Datetime& x): type(DATETIME), data(const_cast<Datetime*>(&x)) { }
			Index(const Blob& x): type(BLOB), data(const_cast<Blob*>(&x)) { }
			Index(const Text& x): type(TEXT), data(const_cast<Text*>(&x)) { }
			Index(const Wtext& x): type(WTEXT), data(const_cast<Wtext*>(&x)) { }
			Index(const Boolean& x): type(U_TINY), data(const_cast<Boolean*>(&x)) { }
			Index(const char* const x, const size_t size_): type(CHAR), data(const_cast<char*>(x)), size(size_) { }
			template<class T> explicit Index(const T& x): type(BINARY), data(const_cast<T*>(&x)), size(sizeof(T)) { }
			Index(const UtinyN& x): type(U_TINY_N), data(const_cast<UtinyN*>(&x)) { }
			Index(const TinyN& x): type(TINY_N), data(const_cast<TinyN*>(&x)) { }
			Index(const UshortN& x): type(U_SHORT_N), data(const_cast<UshortN*>(&x)) { }
			Index(const ShortN& x): type(SHORT_N), data(const_cast<ShortN*>(&x)) { }
			Index(const UintN& x): type(U_INT_N), data(const_cast<UintN*>(&x)) { }
			Index(const IntN& x): type(INT_N), data(const_cast<IntN*>(&x)) { }
			Index(const UbigintN& x): type(U_BIGINT_N), data(const_cast<UbigintN*>(&x)) { }
			Index(const BigintN& x): type(BIGINT_N), data(const_cast<BigintN*>(&x)) { }
			Index(const FloatN& x): type(FLOAT_N), data(const_cast<FloatN*>(&x)) { }
			Index(const DoubleN& x): type(DOUBLE_N), data(const_cast<DoubleN*>(&x)) { }
			Index(const TimeN& x): type(TIME_N), data(const_cast<TimeN*>(&x)) { }
			Index(const DateN& x): type(DATE_N), data(const_cast<DateN*>(&x)) { }
			Index(const DatetimeN& x): type(DATETIME_N), data(const_cast<DatetimeN*>(&x)) { }
			Index(const TextN& x): type(TEXT_N), data(const_cast<TextN*>(&x)) { }
			Index(const WtextN& x): type(WTEXT_N), data(const_cast<WtextN*>(&x)) { }
			Index(const BooleanN& x): type(U_TINY_N), data(const_cast<BooleanN*>(&x)) { }
			template<int size_> Index(const NullableArray<char, size_>& x): type(CHAR_N), data(const_cast<NullableArray<char, size_>*>(&x)), size(size_) { }
			template<class T> explicit Index(const Nullable<T>& x): type(BINARY_N), data(const_cast<Nullable<T>*>(&x)), size(sizeof(T)) { }

			Index(const Index& x): type(x.type), data(x.data), size(x.size) {}
			Index(): type(NOTHING), data(0), size(0) {}

			const Index& operator=(const Index& x) { type=x.type; data=x.data; size=x.size; return *this; }
			bool operator==(const Index& x) { return type==x.type && data==x.data && size==x.size; }
		};

		/*! 
		//! Base data set class for communicating parameters and results with SQL queries.
		 *
		 * Note that in most cases you are better off using one the following templated wrapper classes 
		 * to associate you data set with ASql. This eliminates the overhead of virtual functions from
		 * your data type if it is used in a computationally intensive situation.
		 * - SetBuilder
		 * - SetRefBuilder
		 * - SetPtrBuilder
		 * - SetSharedPtrBuilder
		 * - IndySetBuilder
		 * - IndySetRefBuilder
		 * 
		 * Or for ASql::Data::SetContainer objects
		 * - STLSetContainer
		 * - STLSetRefContainer
		 * - STLSharedSetContainer
		 *
		 * If you use this technique you MUST still define the numberOfSqlElements() and getSqlIndex() in
		 * your dataset as below but do not derive from Set. The function will be called from the templates
		 * instead of virtually.
		 *
		 * By deriving from this class any data structure can gain the capability to be binded to
		 * the parameters or results of an SQL query. This is accomplished polymorphically through two
		 * virtual member functions that allow the object to be treated as a container and it's member
		 * data indexed as it's elements. An example derivation follows:
@code
struct TestSet: public ASql::Data::Set
{
	ASql::Data::DoubleN fraction;
	ASql::Data::DateN aDate;
	ASql::Data::Time aTime;
	ASql::Data::DatetimeN timestamp;
	ASql::Data::WtextN someText;
	ASql::Data::BlobN someData;
	char fixedString[16];

	size_t numberOfSqlElements() const { return 7; }
	ASql::Data::Index getSqlIndex(size_t index) const
	{
		switch(index)
		{
			case 0:
				return fraction;
			case 1:
				return aDate;
			case 2:
				return aTime;
			case 3:
				return timeStamp;
			case 4:
				return someText;
			case 5:
				return someData;
			case 6:
				return ASql::Data::Index(fixedChunk, sizeof(fixedString));
			default:
				return ASql::Data::Index();
		}
	}
};
@endcode
		 * Note that this same example can be more easily done with the use of the ASQL_BUILDSET macro.
		 * In the following example we will also use the set builders.
@code
struct TestSet
{
	ASql::Data::DoubleN fraction;
	ASql::Data::DateN aDate;
	ASql::Data::Time aTime;
	ASql::Data::DatetimeN timestamp;
	ASql::Data::WtextN someText;
	ASql::Data::BlobN someData;
	char fixedString[16];

	ASQL_BUILDSET(
		(fraction)\
		(aDate)\
		(aTime)\
		(timeStamp)\
		(someText)\
		(someData)\
		(ASql::Data::Index(fixedChunk, sizeof(fixedString)))\
		)
};
@endcode
		 * Then to make a Data::Set out of it
\code
typedef ASql::Data::SetBuilder<TestSet> FunctionalTestSet;
\endcode
		 * or maybe a Data::SetContainer
\code
typedef ASql::Data::SetContainer<std::deque<TestSet> > FunctionalTestContainer;
\endcode
		 * Note that in both examples the indexing order must match the result column/parameter order of the
		 * SQL query.
		 *
		 * In order to be binded to a particular SQL type indexed elements in the class should be of a
		 * type that is typedefed in ASql::Data (don't worry, they are all standard types).
		 * This however is not a requirement as any plain old data structure can be indexed but it will
		 * be stored as a binary data array. Be sure to examine the constructors for Index. For a default
		 * it is best to return a default constructed Index object.
		 *
		 * \sa ASql::Data::Nullable
		 */
		struct Set
		{
			//! Get total number of indexable data members.
			/*! 
			 * \return Total number of indexable data members.
			 */
			virtual size_t numberOfSqlElements() const =0;

			//! Get constant void pointer to member data.
			/*! 
			 * Because of the implicit constructors in Index, for most types it suffices to just return the
			 * member object itself. 
			 * 
			 * \param[in] index index Index number for member, starting at 0.
			 * 
			 * \return Constant void pointer to member data.
			 */
			virtual Index getSqlIndex(const size_t index) const =0; 

			virtual ~Set() {}
		};

		//! Wraps a Set object around an new auto-allocated dataset of type T
		/*! 
		 * \tparam T object type to create. Must have %numberOfSqlElements() and  %getSqlIndex() function defined as
		 * per the instruction in Data::Set.
		 */
		template<class T> class SetBuilder: public Set
		{
		public:
			//! Dataset object of type T that the Set is wrapped around. This is your object.
			T data;
			SetBuilder() {}
		private:
			//! Wrapper function for the %numberOfSqlElements() function in the data object.
			virtual size_t numberOfSqlElements() const { return data.numberOfSqlElements(); }
			//! Wrapper function for the %getSqlIndex() function in the data object.
			virtual Index getSqlIndex(const size_t index) const { return data.getSqlIndex(index); }
		};

		//! Wraps a Set object around a reference to a dataset of type T
		/*! 
		 * \tparam T object type to reference to. Must have %numberOfSqlElements() and  %getSqlIndex() function defined as
		 * per the instruction in Data::Set.
		 */
		template<class T> class SetRefBuilder: public Set
		{
			//! Wrapper function for the %numberOfSqlElements() function in the data object.
			virtual size_t numberOfSqlElements() const { return m_data.numberOfSqlElements(); }
			//! Wrapper function for the %getSqlIndex() function in the data object.
			virtual Index getSqlIndex(const size_t index) const { return m_data.getSqlIndex(index); }
			//! Reference to the dataset
			const T& m_data;
		public:
			/*! 
			 * \param x Reference to the dataset of type T to reference to.
			 */
			inline SetRefBuilder(const T& x): m_data(x) {}
		};

		//! Wraps a Set object around a pointer to a dataset of type T
		/*! 
		 * This has the one advantage over SetRefBuilder in that the dataset pointed to can be changed with destroying/rebuilding
		 * wrapper object.
		 *
		 * \tparam T object type to point to. Must have %numberOfSqlElements() and  %getSqlIndex() function defined as
		 * per the instruction in Data::Set.
		 */
		template<class T> class SetPtrBuilder: public Set
		{
			//! Pointer to the dataset
			const T* m_data;

			//! Wrapper function for the %numberOfSqlElements() function in the data object.
			virtual size_t numberOfSqlElements() const { return m_data->numberOfSqlElements(); }

			//! Wrapper function for the %getSqlIndex() function in the data object.
			virtual Index getSqlIndex(const size_t index) const { return m_data->getSqlIndex(index); }
		public:
			//! Default constructor set's the pointer to null
			inline SetPtrBuilder(): m_data(0) {}

			//! Set the pointer to the address of the object referenced to by x
			inline SetPtrBuilder(const T& x): m_data(&x) {}

			inline SetPtrBuilder(SetPtrBuilder& x): m_data(x.m_data) {}

			//! Set the pointer to the address of the object referenced to by x
			inline void set(const T& data) { m_data=&data; }

			//! Set the pointer to null
			inline void clear() { m_data=0; }

			//! Return true if the pointer is not null
			operator bool() const { return m_data; }
		};

		//! Wraps a Set object around a shared pointer to a dataset of type T
		/*! 
		 * This has the one advantage over SetRefBuilder in that the dataset pointed to can be changed with destroying/rebuilding
		 * wrapper object and SetPtrBuilder in that the pointer can be shared.
		 *
		 * \tparam T object type to point to. Must have %numberOfSqlElements() and  %getSqlIndex() function defined as
		 * per the instruction in Data::Set.
		 */
		template<class T> class SetSharedPtrBuilder: public Set
		{
			//! Wrapper function for the %numberOfSqlElements() function in the data object.
			virtual size_t numberOfSqlElements() const { return data->numberOfSqlElements(); }
			//! Wrapper function for the %getSqlIndex() function in the data object.
		public:
			virtual Index getSqlIndex(const size_t index) const { return data->getSqlIndex(index); }
			inline SetSharedPtrBuilder() {}
			inline SetSharedPtrBuilder(const boost::shared_ptr<T>& x): data(x) {}
			inline SetSharedPtrBuilder(SetSharedPtrBuilder& x): data(x.data) {}
			//! Shared pointer to the dataset
			boost::shared_ptr<T> data;
		};

		//! Wraps a Set object around an new auto-allocated individual object of type T
		/*! 
		 * \tparam T object type to create.
		 */
		template<class T> class IndySetBuilder: public Set
		{
		public:
			//! Object of type T that the Set is wrapped around. This is your object.
			T data;
			IndySetBuilder() {}
		private:
			//! Returns 1 as it is an individual container
			virtual size_t numberOfSqlElements() const { return 1; }
			//! Just returns an index to data.
			virtual Index getSqlIndex(const size_t index) const { return data; }
		};

		//! Wraps a Set object around a reference to an individual object of type T
		/*! 
		 * \tparam T object type to create.
		 */
		template<class T> class IndySetRefBuilder: public Set
		{
			//! Object of type T that the Set is wrapped around. This is your object.
			const T& data;
			//! Returns 1 as it is an individual container
			virtual size_t numberOfSqlElements() const { return 1; }
			//! Just returns an index to data.
			virtual Index getSqlIndex(const size_t index) const { return data; }
		public:
			/*! 
			 * \param x Reference to the object of type T to reference to.
			 */
			inline IndySetRefBuilder(const T& x): data(x) {}
		};

		 //! Wraps a Set object around a pointer to an individual object of type T
		 /*!
		 * \tparam T object type to create.
		 */
		template<class T> class IndySetPtrBuilder: public Set
		{
			//! Object of type T that the Set is wrapped around. This is your object.
			const T* m_data;

			//! Returns 1 as it is an individual container
			virtual size_t numberOfSqlElements() const { return 1; }
			 
			//! Just returns an index to data.
			virtual Index getSqlIndex(const size_t index) const { return *m_data; }
		public:
			//! Default constructor set's the pointer to null
			inline IndySetPtrBuilder(): m_data(0) {}

			//! Set the pointer to the address of the object referenced to by x
			inline IndySetPtrBuilder(const T& x): m_data(&x) {}

			inline IndySetPtrBuilder(IndySetPtrBuilder& x): m_data(x.m_data) {}

			//! Set the pointer to the address of the object referenced to by x
			inline void set(const T& data) { m_data=&data; }

			//! Set the pointer to null
			inline void clear() { m_data=0; }

			//! Return true if the pointer is not null
			operator bool() const { return m_data; }
		};

		//! Base class for containers of Data::Set objects to be used for result/parameter data in SQL queries.
		struct SetContainer
		{
			//! Appends a row to the container and returns a reference to it.
			virtual Set& manufacture() =0;
			//! Pop a row off the end of the container.
			virtual void trim() =0;
			virtual ~SetContainer() {}
			//! Get a row from the front and move on to the next row.
			/*! 
			 * \return This function should return a pointer to the row or null if at the end.
			 */
			virtual const Set* pull() const =0;
			virtual void init() const =0;
		};

		//! Wraps a SetContainer object around a new auto-allocated STL container of type T
		/*! 
		 * This class defines a basic container for types that can be wrapped by the Set class.
		 *	It is intended for retrieving multi-row results from SQL queries. In order to
		 *	function the passed container type must have the following member functions
		 *	push_back(), back(), pop_back() and it's content type must be wrappable by Set as per the
		 *	instructions there.
		 *
		 *	\tparam Container type. Must be sequential.
		 */
		template<class T> class STLSetContainer: public SetContainer
		{
			mutable SetPtrBuilder<typename T::value_type> m_buffer;
			mutable typename T::iterator m_itBuffer;

			Set& manufacture()
			{
				data.push_back(typename T::value_type());
				m_buffer.set(data.back());
				return m_buffer;
			}
			void trim() { data.pop_back(); }
			const Set* pull() const
			{
				if(m_itBuffer == const_cast<T&>(data).end()) return 0;
				m_buffer.set(*m_itBuffer++);
				return &m_buffer;
			}
		public:
			//! STL container object of type T that the SetContainer is wrapped around. This is your object.
			T data;
			void init() const {  m_itBuffer = const_cast<T&>(data).begin(); }
			STLSetContainer(): m_itBuffer(data.begin()) {}
		};
	
		//! Wraps a SetContainer object around a new auto-allocated STL container of type T
		/*! 
		 * This class defines a basic container for types that can be wrapped by the Set class.
		 *	It is intended for retrieving multi-row results from SQL queries. In order to
		 *	function the passed container type must have the following member functions
		 *	push_back(), back(), pop_back() and it's content type must be wrappable by Set as per the
		 *	instructions there.
		 *
		 *	\tparam Container type. Must be sequential.
		 */
		template<class T> class IndySTLSetContainer: public SetContainer
		{
			mutable IndySetPtrBuilder<typename T::value_type> m_buffer;
			mutable typename T::iterator m_itBuffer;

			Set& manufacture()
			{
				data.push_back(typename T::value_type());
				m_buffer.set(data.back());
				return m_buffer;
			}
			void trim() { data.pop_back(); }
			const Set* pull() const
			{
				if(m_itBuffer == const_cast<T&>(data).end()) return 0;
				m_buffer.set(*m_itBuffer++);
				return &m_buffer;
			}
		public:
			//! STL container object of type T that the SetContainer is wrapped around. This is your object.
			T data;
			void init() const {  m_itBuffer = const_cast<T&>(data).begin(); }
			IndySTLSetContainer(): m_itBuffer(data.begin()) {}
		};
	
		//! Wraps a SetContainer object around a reference to an STL container of type T
		/*! 
		 * This class defines a basic container for types that can be wrapped by the Set class.
		 *	It is intended for retrieving multi-row results from SQL queries. In order to
		 *	function the passed container type must have the following member functions
		 *	push_back(), back(), pop_back() and it's content type must be wrappable by Set as per the
		 *	instructions there.
		 *
		 *	\tparam Container type. Must be sequential.
		 */
		template<class T> class STLSetRefContainer: public SetContainer
		{
			T& data;
			mutable SetPtrBuilder<typename T::value_type> m_buffer;
			mutable typename T::iterator m_itBuffer;

			Set& manufacture()
			{
				data.push_back(typename T::value_type());
				m_buffer.set(data.back());
				return m_buffer;
			}
			void trim() { data.pop_back(); }
			const Set* pull() const
			{
				if(m_itBuffer == const_cast<T&>(data).end()) return 0;
				m_buffer.set(*m_itBuffer++);
				return &m_buffer;
			}
		public:
			void init() const {  m_itBuffer = const_cast<T&>(data).begin(); }
			/*! 
			 * \param x Reference to the STL container of type T to reference to.
			 */
			STLSetRefContainer(T& x): data(x), m_itBuffer(data.begin()) {}
		};
	
		//! Wraps a SetContainer object around a shared pointer to an STL container of type T
		/*! 
		 * This class defines a basic container for types that can be wrapped by the Set class.
		 *	It is intended for retrieving multi-row results from SQL queries. In order to
		 *	function the passed container type must have the following member functions
		 *	push_back(), back(), pop_back() and it's content type must be wrappable by Set as per the
		 *	instructions there.
		 *
		 *	\tparam Container type. Must be sequential.
		 */
		template<class T> class STLSharedSetContainer: public SetContainer
		{
			mutable SetPtrBuilder<typename T::value_type> m_buffer;
			mutable typename T::iterator m_itBuffer;

			Set& manufacture()
			{
				data->push_back(typename T::value_type());
				m_buffer.set(data->back());
				return m_buffer;
			}
			void trim() { data->pop_back(); }
			const Set* pull() const
			{
				if(m_itBuffer == const_cast<T*>(data)->end()) return 0;
				m_buffer.set(*m_itBuffer++);
				return &m_buffer;
			}
		public:
			//! Shared pointer to the STL container
			boost::shared_ptr<T> data;
			STLSharedSetContainer(const boost::shared_ptr<T>& x): data(x) {}
			STLSharedSetContainer(): m_itBuffer(data->begin()) {}
			void init() const {  m_itBuffer = const_cast<T*>(data)->begin(); }
		};

		//! Handle data conversion from standard data types to internal SQL engine types.
		struct Conversion
		{
			//! Get a pointer to the internal data.
			/*! 
			 * \return Void pointer to internal data.
			 */
			virtual void* getPointer() =0;

			//! Convert SQL query results.
			virtual void convertResult() =0;

			//! Convert SQL query parameters.
			virtual void convertParam() =0;

			//! Pointer to standard data type.
			void* external;
		};
		
		typedef std::map<int, boost::shared_ptr<Conversion> > Conversions;
	}
}

#endif
