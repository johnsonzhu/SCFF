//! \file http.hpp Defines elements of the HTTP protocol
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


#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <boost/shared_array.hpp>
#include <boost/scoped_array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <ostream>
#include <istream>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>

#include <fastcgi++/exceptions.hpp>
#include <fastcgi++/protocol.hpp>

//! Topmost namespace for the fastcgi++ library
namespace Fastcgipp
{
	//! Defines classes and function relating to the http protocol
	namespace Http
	{
		//! Holds a piece of HTTP post data
		/*!
		 * This structure will hold one of two types of HTTP post data. It can
		 * either contain form data, in which case the data field is empty and
		 * the size is zero; or it can hold an uploaded file, in which case data
		 * contains a pointer to the file data, size contains it's size and value holds it's
		 * filename. The actual name associated with the piece of post data
		 * is omitted from the class so it can be linked in an associative
		 * container.
		 *
		 * @tparam charT Type of character to use in the value string (char or wchar_t)
		 */
		template<class charT> struct Post
		{
			//! Type of POST data piece
			enum Type { file, form } type;
			//! Value of POST data if type=form
			std::basic_string<charT> value;
			//! Filename of POST data if type=file
			std::basic_string<charT>& filename;
			//! Content Type if type=file
			std::basic_string<charT> contentType;

			//! Pointer to file data
			const char* data() const { return m_data; }
			//! Size of file data
			size_t size() const { return m_size; }
			//! Expropriates the file data. Beyond this you must free it when done
			char* steal() const { char* ptr=m_data; m_data=0; m_size=0; return ptr; }

			Post(): filename(value), m_data(0), m_size(0) {}
			Post(const Post& x):
				type(x.type),
				value(x.value),
				filename(value),
				contentType(x.contentType),
				m_data(x.steal()),
				m_size(x.m_size)
			{}
			~Post() { delete [] m_data; }
		private:
			//! Pointer to file data
			mutable char* m_data;
			//! Size of data in bytes pointed to by data.
			mutable size_t m_size;
			template<class T> friend class Environment;
		};

		//! The HTTP request method as an enumeration
		enum RequestMethod
		{
			HTTP_METHOD_ERROR,
			HTTP_METHOD_HEAD,
			HTTP_METHOD_GET,
			HTTP_METHOD_POST,
			HTTP_METHOD_PUT,
			HTTP_METHOD_DELETE,
			HTTP_METHOD_TRACE,
			HTTP_METHOD_OPTIONS,
			HTTP_METHOD_CONNECT
		};
		extern const char* requestMethodLabels[];
		template<class charT, class Traits> inline std::basic_ostream<charT, Traits>& operator<<(std::basic_ostream<charT, Traits>& os, const RequestMethod requestMethod) { return os << requestMethodLabels[requestMethod]; }
	
		//! Efficiently stores IPv6 addresses
		/*!
		 * This class stores IPv6 addresses as a 128 bit array. It does this as
		 * opposed to storing the string itself to facilitate efficient logging
		 * and processing of the address. The class possesses full IO and
		 * comparison capabilities as well as allowing bitwise AND operations for
		 * netmask calculation. It detects when an IPv4 address is stored outputs
		 * it accordingly.
		 */
		class Address
		{
		public:
			//! This is the data length of the IPv6 address
			static const size_t size=16;

			//! Retrieve a const pointer to the raw data of the IPv6 address
			/*!
			 * @return Constant pointer to data array representing the raw IPv6 address
			 */
			const unsigned char* data() const { return m_data; }

			//! Retrieve a pointer to the raw data of the IPv6 address
			/*!
			 * @return Pointer to data array representing the raw IPv6 address
			 */
			unsigned char* data() { return m_data; }

			//! Assign the IPv4 address from a data array
			/*!
			 * @param[in] data_ Pointer to a 16 byte array
			 */
			Address operator=(const unsigned char* data_) { std::memcpy(m_data, data_, size); return *this; }

			Address operator=(const Address& address) { std::memcpy(m_data, address.m_data, size); return *this; }
			Address(const Address& address) { std::memcpy(m_data, address.m_data, size); }
			Address() {}

			//! Construct the IPv6 address from a data array
			/*!
			 * @param[in] data_ Pointer to a 16 byte array
			 */
			explicit Address(const unsigned char* data_) { std::memcpy(m_data, data_, size); }

			//! Assign the IP address from a string of characters
			/*!
			 * In order for this to work the string must represent either an IPv4 address in
			 * standard textual decimal form (127.0.0.1) or an IPv6 in standard form.
			 *
			 * @param[in] start First character of the string
			 * @param[in] end Last character of the string + 1
			 */
			void assign(const char* start, const char* end);
			inline bool operator==(const Address& x) const { return std::memcmp(m_data, x.m_data, size)==0; }
			inline bool operator>(const Address& x) const { return std::memcmp(m_data, x.m_data, size)>0; }
			inline bool operator<(const Address& x) const { return std::memcmp(m_data, x.m_data, size)<0; }
			inline bool operator<=(const Address& x) const { return !(std::memcmp(m_data, x.m_data, size)>0); }
			inline bool operator>=(const Address& x) const { return !(std::memcmp(m_data, x.m_data, size)<0); }
			//! Returns false if the ip address is zeroed. True otherwise
			operator bool() const;
			Address operator&(const Address& x) const;

			Address& operator&=(const Address& x);

			//! Set all bits to zero in ip address
			void zero() { std::memset(m_data, 0, size); }

		private:
			//! Data representation of the IPv6 address
			unsigned char m_data[size];
		};

		//! Address stream insertion operation
		/*!
		 * This stream inserter obeys all stream manipulators regarding
		 * alignment, field width and numerical base.
		 */
		template<class charT, class Traits> std::basic_ostream<charT, Traits>& operator<<(std::basic_ostream<charT, Traits>& os, const Address& address);
		//! Address stream extractor operation
		/*!
		 * In order for this to work the string must represent either an IPv4
		 * address in standard textual decimal form (127.0.0.1) or an IPv6 in
		 * standard form.
		 */
		template<class charT, class Traits> std::basic_istream<charT, Traits>& operator>>(std::basic_istream<charT, Traits>& is, Address& address);

		//! Data structure of HTTP environment data
		/*!
		 * This structure contains all HTTP environment data for each individual request. The data is processed
		 * from FastCGI parameter records.
		 *
		 * @tparam charT Character type to use for strings
		 */
		template<class charT> struct Environment
		{
			//! Hostname of the server
			std::basic_string<charT> host;
			//! User agent string
			std::basic_string<charT> userAgent;
			//! Content types the client accepts
			std::basic_string<charT> acceptContentTypes;
			//! Languages the client accepts
			std::basic_string<charT> acceptLanguages;
			//! Character sets the clients accepts
			std::basic_string<charT> acceptCharsets;
			//! Referral URL
			std::basic_string<charT> referer;
			//! Content type of data sent from client
			std::basic_string<charT> contentType;
			//! HTTP root directory
			std::basic_string<charT> root;
			//! Filename of script relative to the HTTP root directory
			std::basic_string<charT> scriptName;
			//! REQUEST_METHOD
			RequestMethod requestMethod;
			//! REQUEST_URI
			std::basic_string<charT> requestUri;
			//! Path Information
			typedef std::vector<std::basic_string<charT> > PathInfo;
			PathInfo pathInfo;
			//! The etag the client assumes this document should have
			int etag;
			//! How many seconds the connection should be kept alive
			int keepAlive;
			//! Length of content to be received from the client (post data)
			unsigned int contentLength;
			//! IP address of the server
			Address serverAddress;
			//! IP address of the client
			Address remoteAddress;
			//! TCP port used by the server
			uint16_t serverPort;
			//! TCP port used by the client
			uint16_t remotePort;
			//! Timestamp the client has for this document
			boost::posix_time::ptime ifModifiedSince;

			typedef std::map<std::basic_string<charT>, std::basic_string<charT> > Cookies;
			//! Container with all url-encoded cookie data
			Cookies cookies;
			//! Quick and easy way to find a cookie value
			const std::basic_string<charT>& findCookie(const charT* key) const;

			typedef std::map<std::basic_string<charT>, std::basic_string<charT> > Gets;
			//! Container with all url-encoded GET data
			Gets gets;

			//! Quick and easy way to find a GET value
			/*!
			 * \param[in] key C-string representation of the name of the GET value you want
			 * \return Constant reference to the string representation of the GET value. If the
			 * GET value does not exist this will return an empty string;
			 */
			const std::basic_string<charT>& findGet(const charT* key) const;

			//! Quick and easy way to check if a GET value exists.
			/*!
			 * \param[in] key C-string representation of the name of the GET value you want
			 * \return True if the value was passed from the client, false otherwise.
			 */
			bool checkForGet(const charT* key) const;

			typedef std::map<std::basic_string<charT>, Post<charT> > Posts;
			//! STL container associating Post objects with their name
			Posts posts;

			//! Quick and easy way to find a POST value
			/*!
			 * \param[in] key C-string representation of the name of the POST value you want
			 * \return Constant reference to the Post object created for the POST value. If the
			 * POST value does not exist this will return a default constructed Post object.
			 */
			const Post<charT>& findPost(const charT* key) const;

			//! Quick and easy way to check if a POST value exists.
			/*!
			 * \param[in] key C-string representation of the name of the POST value you want
			 * \return True if the value was passed from the client, false otherwise.
			 */
			bool checkForPost(const charT* key) const;

			//! Parses FastCGI parameter data into the data structure
			/*!
			 * This function will take the body of a FastCGI parameter record and parse
			 * the data into the data structure. data should equal the first character of
			 * the records body with size being it's content length.
			 *
			 * @param[in] data Pointer to the first byte of parameter data
			 * @param[in] size Size of data in bytes
			 */
			void fill(const char* data, size_t size);

			//! Consolidates POST data into a single buffer
			/*!
			 * This function will take arbitrarily divided chunks of raw http post
			 * data and consolidate them into postBuffer.
			 *
			 * @param[in] data Pointer to the first byte of post data
			 * @param[in] size Size of data in bytes
			 * @return Returns true unless the buffer overflowed
			 */
			bool fillPostBuffer(const char* data, size_t size);

			//! Parses "multipart/form-data" http post data into the posts object
			void parsePostsMultipart();

			//! Parses "application/x-www-form-urlencoded" post data into the posts object.
			void parsePostsUrlEncoded();

			//! Clear the post buffer
			void clearPostBuffer() { postBuffer.reset(); pPostBuffer=0; }

			Environment(): requestMethod(HTTP_METHOD_ERROR), etag(0), keepAlive(0), contentLength(0), serverPort(0), remotePort(0) {}
		private:
			//! Raw string of characters representing the post boundary
			boost::scoped_array<char> boundary;
			//! Size of boundary
			size_t boundarySize;

			//! Buffer for processing post data
			boost::scoped_array<char> postBuffer;
			//! Pointer in buffer
			char* pPostBuffer;
			//! Returns minimum buffer size remaining
			size_t minPostBufferSize(const size_t size) { return std::min(size, size_t(postBuffer.get()+contentLength-pPostBuffer)); }
		};

		//! Convert a char string to a std::wstring
		/*!
		 * @param[in] data First byte in char string
		 * @param[in] size Size in bytes of the string (no null terminator)
		 * @param[out] string Reference to the wstring that should be modified
		 * @return Returns true on success, false on failure
		 */
		void charToString(const char* data, size_t size, std::wstring& string);

		//! Convert a char string to a std::string
		/*!
		 * @param[in] data First byte in char string
		 * @param[in] size Size in bytes of the string (no null terminator)
		 * @param[out] string Reference to the string that should be modified
		 * @return Returns true on success, false on failure
		 */
		inline void charToString(const char* data, size_t size, std::string& string) { string.assign(data, size); }

		//! Convert a char string to an integer
		/*!
		 * This function is very similar to std::atoi() except that it takes start/end values
		 * of a non null terminated char string instead of a null terminated string. The first
		 * character must be either a number or a minus sign (-). As soon as the end is reached
		 * or a non numerical character is reached, the result is tallied and returned.
		 *
		 * @param[in] start Pointer to the first byte in the string
		 * @param[in] end Pointer to the last byte in the string + 1
		 * @return Integer value represented by the string
		 */
		int atoi(const char* start, const char* end);

		//! Decodes a url-encoded string into a container
		/*! 
		 * @param[in] data Data to decode
		 * @param[in] size Size of data to decode
		 * @param[out] output Container to output data into
		 *
		 * @return Returns false if the name isn't found. True otherwise.
		 */
		template<class charT> void decodeUrlEncoded(const char* data, size_t size, std::map<std::basic_string<charT>, std::basic_string<charT> >& output, const char fieldSeperator='&');

		//! Convert a string with percent escaped byte values to their actual values
		/*!
		 *	Since converting a percent escaped string to actual values can only make it shorter, 
		 *	it is safe to assume that the return value will always be smaller than size. It is
		 *	thereby a safe move to make the destination block of memory the same size as the source.
		 *
		 * @param[in] source Pointer to the first character in the percent escaped string
		 * @param[in] size Size in bytes of the data pointed to by source (no null termination)
		 * @param[out] destination Pointer to the section of memory to write the converted string to
		 * @return Actual size of the new string
		 */
		size_t percentEscapedToRealBytes(const char* source, char* destination, size_t size);

		/** 
		 * @brief List of characters in order for Base64 encoding.
		 */
		extern const char base64Characters[];

		/** 
		 * @brief Convert a binary container of data to a Base64 encoded container.
		 *
		 * If destination is a fixed size container, it should have a size of at least ((end-start-1)/3 + 1)*4 not including null terminators if used and assuming integer arithmetic.
		 *
		 * @param[in] start Iterator to start of binary data.
		 * @param[in] end Iterator to end of binary data.
		 * @param[out] destination Iterator to start of Base64 destination.
		 *
		 * @tparam In Input iterator type. Should be dereferenced to type char.
		 * @tparam Out Output iterator type. Should be dereferenced to type char.
		 */
		template<class In, class Out> void base64Encode(In start, In end, Out destination);

		/** 
		 * @brief Convert a Base64 encoded container to a binary container.
		 *
		 * If destination is a fixed size container, it should have a size of
		 * at least (end-start)*3/4 not including null terminators if used.
		 * 
		 * @param[in] start Iterator to start of Base64 data.
		 * @param[in] end Iterator to end of Base64 data.
		 * @param[out] destination Iterator to start of binary destination.
		 *
		 * @tparam In Input iterator type. Should be dereferenced to type char.
		 * @tparam Out Output iterator type. Should be dereferenced to type char.
		 * 
		 * @return Iterator to last position written+1 (The normal end() iterator). If
		 * the return value equals destination, an error occurred.
		 */
		template<class In, class Out> Out base64Decode(In start, In end, Out destination);

		/** 
		 * @brief Defines ID values for HTTP sessions.
		 */
		class SessionId
		{
			/** 
			 * @brief Size in bytes of the ID data
			 */
			public: static const int size=12;

		private:
			/** 
			 * @brief ID data
			 */
			char data[size];

			/** 
			 * @brief Contains the time this session was last used
			 */
			boost::posix_time::ptime timestamp;

			/** 
			 * @brief Set to true once the random number generator has been seeded
			 */
			static bool seeded;

			template<class T> friend class Sessions;
		public:
			/** 
			 * @brief The default constructor initializes the ID data to a random value
			 */
			SessionId();

			SessionId(const SessionId& x): timestamp(x.timestamp) { std::memcpy(data, x.data, size); }
			const SessionId& operator=(const SessionId& x) { std::memcpy(data, x.data, size); timestamp=x.timestamp; return *this; }

			/** 
			 * @brief Assign the ID data with a base64 encoded string
			 *
			 * Note that only size*4/3 bytes will be read from the string.
			 * 
			 * @param data_ Iterator set at begin of base64 encoded string
			 */
			template<class charT> const SessionId& operator=(charT* data_);

			/** 
			 * @brief Initialize the ID data with a base64 encoded string
			 * 
			 * Note that only size*4/3 bytes will be read from the string.
			 * 
			 * @param data_ 
			 */
			template<class charT> SessionId(charT* data_) { *this=data_; }

			template<class charT, class Traits> friend std::basic_ostream<charT, Traits>& operator<<(std::basic_ostream<charT, Traits>& os, const SessionId& x);

			bool operator<(const SessionId& x) const { return std::memcmp(data, x.data, SessionId::size)<0; }
			bool operator==(const SessionId& x) const { return std::memcmp(data, x.data, SessionId::size)==0; }

			/** 
			 * @brief Resets the last access timestamp to the current time.
			 */
			void refresh() const { *const_cast<boost::posix_time::ptime*>(&timestamp)=boost::posix_time::second_clock::universal_time(); }

			const char* getInternalPointer() const { return data; }
		};

		/** 
		 * @brief Output the ID data in base64 encoding
		 */
		template<class charT, class Traits> std::basic_ostream<charT, Traits>& operator<<(std::basic_ostream<charT, Traits>& os, const SessionId& x) { base64Encode(x.data, x.data+SessionId::size, std::ostream_iterator<charT, charT, Traits>(os)); return os; }

		/** 
		 * @brief Container for HTTP sessions
		 *
		 *	In almost all ways this class behaves like an std::map. The sole addition is a mechanism for
		 *	clearing out expired sessions based on a keep alive time and a frequency of deletion. The first
		 *	part of the std::pair<> is a SessionId object, and the second is a object of class T (passed as
		 *	the template parameter.
		 *
		 * @tparam T Class containing session data.
		 */
		template<class T> class Sessions: public std::map<SessionId, T>
		{
		private:
			/** 
			 * @brief Amount of seconds to keep sessions around for.
			 */
			const boost::posix_time::seconds keepAlive;

			/** 
			 * @brief How often (in seconds) the container should find old sessions and delete them.
			 */
			const boost::posix_time::seconds cleanupFrequency;

			/** 
			 * @brief The time that the next session cleanup should be done.
			 */
			boost::posix_time::ptime cleanupTime;
		public:
			typedef typename std::map<SessionId, T>::iterator iterator;
			typedef typename std::map<SessionId, T>::const_iterator const_iterator;
			/** 
			 * @brief Constructor takes session keep alive times and cleanup frequency.
			 * 
			 * @param[in] keepAlive_ Amount of seconds a session will stay alive for.
			 * @param[in] cleanupFrequency_ How often (in seconds) the container should find old sessions and delete them.
			 */
			Sessions(int keepAlive_, int cleanupFrequency_): keepAlive(boost::posix_time::seconds(keepAlive_)), cleanupFrequency(boost::posix_time::seconds(cleanupFrequency_)), cleanupTime(boost::posix_time::second_clock::universal_time()+cleanupFrequency) { }
				
			/** 
			 * @brief Clean out old sessions.
			 *
			 * Calling this function will clear out any expired sessions from the container. This function must be called for any cleanup to take place, however calling may not actually cause a cleanup. The amount of seconds specified in cleanupFrequency must have expired since the last cleanup for it to actually take place.
			 */
			void cleanup();

			/** 
			 * @brief Generates a new session pair with a random ID value
			 * 
			 * @param[in] value_ Value to place into the data section.
			 * 
			 * @return Iterator pointing to the newly created session.
			 */
			iterator generate(const T& value_ = T());

			boost::posix_time::ptime getExpiry(const_iterator it) const { return it->first.timestamp+keepAlive; }
		};
	}
}

template<class T> void Fastcgipp::Http::Sessions<T>::cleanup()
{
	if(boost::posix_time::second_clock::universal_time() < cleanupTime)
		return;
	boost::posix_time::ptime oldest(boost::posix_time::second_clock::universal_time()-keepAlive);
	iterator it=this->begin();
	while(it!=this->end())
	{
		if(it->first.timestamp < oldest)
			erase(it++);
		else
			++it;
	}
	cleanupTime=boost::posix_time::second_clock::universal_time()+cleanupFrequency;
}

template<class In, class Out> Out Fastcgipp::Http::base64Decode(In start, In end, Out destination)
{
	Out dest=destination;

	for(int buffer, bitPos=-8, padStart; start!=end || bitPos>-6; ++dest)
	{
		if(bitPos==-8)
		{
			bitPos=18;
			padStart=-9;
			buffer=0;
			while(bitPos!=-6)
			{
				if(start==end) return destination;
				int value=*start++;
				if(value >= 'A' && 'Z' >= value) value -= 'A';
				else if(value >= 'a' && 'z' >= value) value -= 'a' - 26;
				else if(value >= '0' && '9' >= value) value -= '0' - 52;
				else if(value == '+') value = 62;
				else if(value == '/') value = 63;
				else if(value == '=') { padStart=bitPos; break; }
				else return destination;

				buffer |= value << bitPos;
				bitPos-=6;
			}
			bitPos=16;
		}

		*dest = (buffer >> bitPos) & 0xff;
		bitPos-=8;
		if(padStart>=bitPos)
		{
			if( (padStart-bitPos)/6 )
				return dest;
			else
				return ++dest;
		}
	}

	return dest;
}

template<class In, class Out> void Fastcgipp::Http::base64Encode(In start, In end, Out destination)
{
	for(int buffer, bitPos=-6, padded; start!=end || bitPos>-6; ++destination)
	{
		if(bitPos==-6)
		{
			bitPos=16;
			buffer=0;
			padded=-6;
			while(bitPos!=-8)
			{
				if(start!=end) 
					buffer |= (uint32_t)*(unsigned char*)start++ << bitPos;
				else padded+=6;
				bitPos-=8;
			}
			bitPos=18;
		}

		if(padded == bitPos)
		{
			*destination='=';
			padded-=6;
		}
		else *destination=base64Characters[ (buffer >> bitPos)&0x3f ];
		bitPos -= 6;
	}
}

template<class T> typename Fastcgipp::Http::Sessions<T>::iterator Fastcgipp::Http::Sessions<T>::generate(const T& value_)
{
	std::pair<iterator,bool> retVal;
	retVal.second=false;
	while(!retVal.second)
		retVal=insert(std::pair<SessionId, T>(SessionId(), value_));
	return retVal.first;
}

#endif
