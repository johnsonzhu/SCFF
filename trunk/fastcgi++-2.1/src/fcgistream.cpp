#include <cstring>
#include <algorithm>
#include <map>
#include <iterator>
#include <boost/iostreams/code_converter.hpp>

#include "fastcgi++/fcgistream.hpp"
#include "utf8_codecvt.hpp"

template<typename charT> template<typename Sink> std::streamsize Fastcgipp::Fcgistream<charT>::Encoder::write(Sink& dest, const charT* s, std::streamsize n)
{
	static std::map<charT, std::basic_string<charT> > htmlCharacters;
	if(!htmlCharacters.size())
	{
		const char quot[]="&quot;";
		std::copy(quot, quot+sizeof(quot)-1, std::back_inserter(htmlCharacters['"']));

		const char gt[]="&gt;";
		std::copy(gt, gt+sizeof(gt)-1, std::back_inserter(htmlCharacters['>']));

		const char lt[]="&lt;";
		std::copy(lt, lt+sizeof(lt)-1, std::back_inserter(htmlCharacters['<']));

		const char amp[]="&amp;";
		std::copy(amp, amp+sizeof(amp)-1, std::back_inserter(htmlCharacters['&']));

		const char apos[]="&apos;";
		std::copy(apos, apos+sizeof(apos)-1, std::back_inserter(htmlCharacters['\'']));
	}

	static std::map<charT, std::basic_string<charT> > urlCharacters;
	if(!urlCharacters.size())
	{
		const char exclaim[]="%21";
		std::copy(exclaim, exclaim+sizeof(exclaim)-1, std::back_inserter(urlCharacters['!']));

		const char rightbrac[]="%5D";
		std::copy(rightbrac, rightbrac+sizeof(rightbrac)-1, std::back_inserter(urlCharacters[']']));

		const char leftbrac[]="%5B";
		std::copy(leftbrac, leftbrac+sizeof(leftbrac)-1, std::back_inserter(urlCharacters['[']));

		const char number[]="%23";
		std::copy(number, number+sizeof(number)-1, std::back_inserter(urlCharacters['#']));

		const char question[]="%3F";
		std::copy(question, question+sizeof(question)-1, std::back_inserter(urlCharacters['?']));

		const char slash[]="%2F";
		std::copy(slash, slash+sizeof(slash)-1, std::back_inserter(urlCharacters['/']));

		const char comma[]="%2C";
		std::copy(comma, comma+sizeof(comma)-1, std::back_inserter(urlCharacters[',']));

		const char money[]="%24";
		std::copy(money, money+sizeof(money)-1, std::back_inserter(urlCharacters['$']));

		const char plus[]="%2B";
		std::copy(plus, plus+sizeof(plus)-1, std::back_inserter(urlCharacters['+']));

		const char equal[]="%3D";
		std::copy(equal, equal+sizeof(equal)-1, std::back_inserter(urlCharacters['=']));

		const char andsym[]="%26";
		std::copy(andsym, andsym+sizeof(andsym)-1, std::back_inserter(urlCharacters['&']));

		const char at[]="%40";
		std::copy(at, at+sizeof(at)-1, std::back_inserter(urlCharacters['@']));

		const char colon[]="%3A";
		std::copy(colon, colon+sizeof(colon)-1, std::back_inserter(urlCharacters[':']));

		const char semi[]="%3B";
		std::copy(semi, semi+sizeof(semi)-1, std::back_inserter(urlCharacters[';']));

		const char rightpar[]="%29";
		std::copy(rightpar, rightpar+sizeof(rightpar)-1, std::back_inserter(urlCharacters[')']));

		const char leftpar[]="%28";
		std::copy(leftpar, leftpar+sizeof(leftpar)-1, std::back_inserter(urlCharacters['(']));

		const char apos[]="%27";
		std::copy(apos, apos+sizeof(apos)-1, std::back_inserter(urlCharacters['\'']));

		const char star[]="%2A";
		std::copy(star, star+sizeof(star)-1, std::back_inserter(urlCharacters['*']));

		const char lt[]="%3C";
		std::copy(lt, lt+sizeof(lt)-1, std::back_inserter(urlCharacters['<']));

		const char gt[]="%3E";
		std::copy(gt, gt+sizeof(gt)-1, std::back_inserter(urlCharacters['>']));

		const char quot[]="%22";
		std::copy(quot, quot+sizeof(quot)-1, std::back_inserter(urlCharacters['"']));

		const char space[]="%20";
		std::copy(space, space+sizeof(space)-1, std::back_inserter(urlCharacters[' ']));

		const char percent[]="%25";
		std::copy(percent, percent+sizeof(percent)-1, std::back_inserter(urlCharacters['%']));
	}

	if(m_state==NONE)
		boost::iostreams::write(dest, s, n);
	else
	{
		std::map<charT, std::basic_string<charT> >* characters;
		switch(m_state)
		{
			case HTML:
				characters = &htmlCharacters;
				break;
			case URL:
				characters = &urlCharacters;
				break;
		}

		const charT* start=s;
		typename std::map<charT, std::basic_string<charT> >::const_iterator it;
		for(const charT* i=s; i < s+n; ++i)
		{
			it=characters->find(*i);
			if(it!=characters->end())
			{
				if(start<i) boost::iostreams::write(dest, start, start-i);
				boost::iostreams::write(dest, it->second.data(), it->second.size());
				start=i+1;
			}
		}
		int size=s+n-start;
		if(size) boost::iostreams::write(dest, start, size);
	}
	return n;
}

std::streamsize Fastcgipp::FcgistreamSink::write(const char* s, std::streamsize n)
{
	using namespace std;
	using namespace Protocol;
	const std::streamsize totalUsed=n;
	while(1)
	{{
		if(!n)
			break;

		int remainder=n%chunkSize;
		size_t size=n+sizeof(Header)+(remainder?(chunkSize-remainder):remainder);
		if(size>numeric_limits<uint16_t>::max()) size=numeric_limits<uint16_t>::max();
		Block dataBlock(m_transceiver->requestWrite(size));
		size=(dataBlock.size/chunkSize)*chunkSize;

		uint16_t contentLength=std::min(size-sizeof(Header), size_t(n));
		memcpy(dataBlock.data+sizeof(Header), s, contentLength);

		s+=contentLength;
		n-=contentLength;

		uint8_t contentPadding=chunkSize-contentLength%chunkSize;
		if(contentPadding==8) contentPadding=0;
		
		Header& header=*(Header*)dataBlock.data;
		header.setVersion(Protocol::version);
		header.setType(m_type);
		header.setRequestId(m_id.fcgiId);
		header.setContentLength(contentLength);
		header.setPaddingLength(contentPadding);

		m_transceiver->secureWrite(size, m_id, false);	
	}}
	return totalUsed;
}

void Fastcgipp::FcgistreamSink::dump(std::basic_istream<char>& stream)
{
	const size_t bufferSize=32768;
	char buffer[bufferSize];

	while(stream.good())
	{
		stream.read(buffer, bufferSize);
		write(buffer, stream.gcount());
	}
}

template<typename T, typename toChar, typename fromChar> T& fixPush(boost::iostreams::filtering_stream<boost::iostreams::output, fromChar>& stream, const T& t, int buffer_size)
{
	stream.push(t, buffer_size);
	return *stream.template component<T>(stream.size()-1);
}

template<> Fastcgipp::FcgistreamSink& fixPush<Fastcgipp::FcgistreamSink, char, wchar_t>(boost::iostreams::filtering_stream<boost::iostreams::output, wchar_t>& stream, const Fastcgipp::FcgistreamSink& t, int buffer_size)
{
	stream.push(boost::iostreams::code_converter<Fastcgipp::FcgistreamSink, utf8CodeCvt::utf8_codecvt_facet>(t, buffer_size));
	return **stream.component<boost::iostreams::code_converter<Fastcgipp::FcgistreamSink, utf8CodeCvt::utf8_codecvt_facet> >(stream.size()-1);
}


template Fastcgipp::Fcgistream<char>::Fcgistream();
template Fastcgipp::Fcgistream<wchar_t>::Fcgistream();
template<typename charT> Fastcgipp::Fcgistream<charT>::Fcgistream():
	m_encoder(fixPush<Encoder, charT, charT>(*this, Encoder(), 0)),
	m_sink(fixPush<FcgistreamSink, char, charT>(*this, FcgistreamSink(), 8192))
{}

template std::basic_ostream<char, std::char_traits<char> >& Fastcgipp::operator<< <char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >& os, const encoding& enc);
template std::basic_ostream<wchar_t, std::char_traits<wchar_t> >& Fastcgipp::operator<< <wchar_t, std::char_traits<wchar_t> >(std::basic_ostream<wchar_t, std::char_traits<wchar_t> >& os, const encoding& enc);
template<class charT, class Traits> std::basic_ostream<charT, Traits>& Fastcgipp::operator<<(std::basic_ostream<charT, Traits>& os, const encoding& enc)
{
	try
	{
		Fcgistream<charT>& stream(dynamic_cast<Fcgistream<charT>&>(os));
		stream.setEncoding(enc.m_type);
	}
	catch(std::bad_cast& bc)
	{
	}

	return os;
}
