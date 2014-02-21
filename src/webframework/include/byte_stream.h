//$Id: byte_stream.h,v 1.3 2005/09/30 04:17:04 henrylu Exp $
#ifndef BYTE_STREAM_H
#define BYTE_STREAM_H

#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>
#include <string>
#include <vector>
#include <map>

template <class ConvertorType>
class CByteStreamT
{
public:
    CByteStreamT(char* pStreamBuf = NULL, uint32_t nBufLen = 0, bool bRealWrite = true);
    char* resetStreamBuf(char* pStreamBuf, uint32_t nBufLen, bool bRealWrite = true);

    // bStore - true  (Serialize) , Default
    //        - false (Unserialize)
    void isStoring(bool bStore);
    bool isStoring() const;

	void setVersion(uint16_t wVersion);
	uint16_t getVersion() const;
    
    /////////////////////// Write ////////////////////////
    CByteStreamT<ConvertorType>& operator << (int8_t c);
    CByteStreamT<ConvertorType>& operator << (uint8_t c);
    CByteStreamT<ConvertorType>& operator << (int16_t n);
    CByteStreamT<ConvertorType>& operator << (uint16_t n);
    CByteStreamT<ConvertorType>& operator << (int32_t n);
    CByteStreamT<ConvertorType>& operator << (uint32_t n);
    CByteStreamT<ConvertorType>& operator << (std::string& str);
    CByteStreamT<ConvertorType>& operator << (uint64_t n);
    CByteStreamT<ConvertorType>& operator << (bool b);
    template<class DATA>
    CByteStreamT<ConvertorType>& operator << (std::vector<DATA>& v);
    template<class DATA1, class DATA2>
    CByteStreamT<ConvertorType>& operator << (std::map<DATA1, DATA2>& v);
    template<class DATA1, class DATA2>
    CByteStreamT<ConvertorType>& operator << (std::multimap<DATA1, DATA2>& v);    
    template<class DATA>
    CByteStreamT<ConvertorType>& operator << (DATA& v);
    
    /////////////////////// Read ////////////////////////
    CByteStreamT<ConvertorType>& operator >> (int8_t& c);
    CByteStreamT<ConvertorType>& operator >> (uint8_t& c);
    CByteStreamT<ConvertorType>& operator >> (int16_t& n);
    CByteStreamT<ConvertorType>& operator >> (uint16_t& n);
    CByteStreamT<ConvertorType>& operator >> (int32_t& n);
    CByteStreamT<ConvertorType>& operator >> (uint32_t& n);
    CByteStreamT<ConvertorType>& operator >> (std::string& str);
    CByteStreamT<ConvertorType>& operator >> (uint64_t& n);
    CByteStreamT<ConvertorType>& operator >> (bool& b);
    template<class DATA>
    CByteStreamT<ConvertorType>& operator >> (std::vector<DATA>& v);
    template<class DATA1, class DATA2>
    CByteStreamT<ConvertorType>& operator >> (std::map<DATA1, DATA2>& v);
    template<class DATA1, class DATA2>
    CByteStreamT<ConvertorType>& operator >> (std::multimap<DATA1, DATA2>& v);    
    template<class DATA>
    CByteStreamT<ConvertorType>& operator >> (DATA& v);
	CByteStreamT<ConvertorType>& mmap (void** pStr, uint32_t n);
    /////////////////////// Read or Write ////////////////////////
    CByteStreamT<ConvertorType>& operator & (int8_t& c);
    CByteStreamT<ConvertorType>& operator & (uint8_t& c);
    CByteStreamT<ConvertorType>& operator & (int16_t& n);
    CByteStreamT<ConvertorType>& operator & (uint16_t& n);
    CByteStreamT<ConvertorType>& operator & (int32_t& n);
    CByteStreamT<ConvertorType>& operator & (uint32_t& n);
    CByteStreamT<ConvertorType>& operator & (std::string& str);
    CByteStreamT<ConvertorType>& operator & (uint64_t& n);
    CByteStreamT<ConvertorType>& operator & (bool& b);
    template<class DATA>
    CByteStreamT<ConvertorType>& operator & (std::vector<DATA>& v);
    template<class DATA1, class DATA2>
    CByteStreamT<ConvertorType>& operator & (std::map<DATA1, DATA2>& v);
    template<class DATA1, class DATA2>
    CByteStreamT<ConvertorType>& operator & (std::multimap<DATA1, DATA2>& v);    
    template<class DATA>
    CByteStreamT<ConvertorType>& operator & (DATA& v);
    
    ///////////////// Read & Write directly /////////////////
    CByteStreamT<ConvertorType>& Read(void *pvDst, uint32_t nLen);
    CByteStreamT<ConvertorType>& Write(const void *pvSrc, uint32_t nLen);
    
    //////////////////////// Utils //////////////////////
    /*
    *	Check this after one read or write
    */
    bool isGood();
    
    /*
    *	Get valid length after written
    */
    uint32_t getWrittenLength();

    /*
    *	Get valid length after read
    */
    uint32_t getReadLength();

	/*
    *	Get remain length to read
    */
	uint32_t getRemainLength2Read();
protected:
    
    void assignStdString(std::string& str, uint32_t nLen);
    
private:
    bool m_bReadGood;
    bool m_bWriteGood;
    
    char* m_pStreamBuf;
    uint32_t m_nBufLen;
    uint32_t m_nReadBufLenLeft;
    uint32_t m_nWriteBufLenLeft;

    static const uint32_t UINT64_HEX_BUF_LEN = 16 + 1;
    char m_sllBuffer[UINT64_HEX_BUF_LEN];

    bool m_bStore;
    bool m_bRealWrite;  // false for calculate written length

	uint16_t m_wVersion;
};

struct CHostNetworkConvertorNormal
{
    static void Host2Network(uint32_t &dwVal)
    {
        dwVal = htonl(dwVal);
    }
    
    static void Host2Network(uint16_t &wVal)
    {
        wVal = htons(wVal);
    }
    static void Network2Host(uint32_t &dwVal)
    {
        dwVal = ntohl(dwVal);
    }
    static void Network2Host(uint16_t &wVal)
    {
        wVal = ntohs(wVal);
    }
};

struct CHostNetworkConvertorNull
{
    static void Host2Network(uint32_t &dwVal) {};
    static void Host2Network(uint16_t &wVal) {};
    static void Network2Host(uint32_t &dwVal) {};
    static void Network2Host(uint16_t &wVal) {};
};

///////////////////////////////////////////////////////
template <class ConvertorType>
inline CByteStreamT<ConvertorType>::CByteStreamT(
                                                 char* pStreamBuf, 
                                                 uint32_t nBufLen,
                                                 bool bRealWrite)
                                                 : m_bReadGood(true)
                                                 , m_bWriteGood(true)
                                                 , m_pStreamBuf(pStreamBuf)
                                                 , m_nBufLen(nBufLen)
                                                 , m_nReadBufLenLeft(nBufLen)
                                                 , m_nWriteBufLenLeft(nBufLen)
                                                 , m_bStore(true)
                                                 , m_bRealWrite(bRealWrite)
												 , m_wVersion(0)
{
}

template <class ConvertorType>
inline 
char* CByteStreamT<ConvertorType>::resetStreamBuf(
                                                  char* pStreamBuf, 
                                                  uint32_t nBufLen,
                                                  bool bRealWrite)
{
    char* l_pStreamBuf = m_pStreamBuf;
    m_pStreamBuf = pStreamBuf;
    m_bReadGood = true;
    m_bWriteGood = true;
    m_nBufLen = nBufLen;
    m_nReadBufLenLeft = nBufLen;
    m_nWriteBufLenLeft = nBufLen;
    m_bRealWrite = bRealWrite;
    
    return l_pStreamBuf;
}

template <class ConvertorType>
inline 
void CByteStreamT<ConvertorType>::isStoring(bool bStore)
{
    m_bStore = bStore;
}

template <class ConvertorType>
inline 
bool CByteStreamT<ConvertorType>::isStoring() const
{
    return m_bStore;
}

template <class ConvertorType>
inline 
void CByteStreamT<ConvertorType>::setVersion(uint16_t wVersion)
{
    m_wVersion = wVersion;
}

template <class ConvertorType>
inline 
uint16_t CByteStreamT<ConvertorType>::getVersion() const
{
    return m_wVersion;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(int8_t c)
{
    Write(&c, sizeof(int8_t));
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(uint8_t c)
{
    Write(&c, sizeof(uint8_t));
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(bool b)
{
	uint8_t c = (b ? 1 : 0);
    Write(&c, sizeof(uint8_t));
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(int16_t n)
{
    return *this << (uint16_t)n;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(uint16_t n)
{
    ConvertorType::Host2Network(n);
    Write(&n, sizeof(uint16_t));
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(int32_t n)
{
    return *this << (uint32_t)n;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(uint32_t n)
{
    ConvertorType::Host2Network(n);
    Write(&n, sizeof(uint32_t));
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(std::string& str)
{
    uint32_t nLen = str.length();
    (*this) << nLen;
    
    if(nLen > 0)
    {
        Write(reinterpret_cast<const void*>(str.c_str()), nLen);
    }
    
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(uint64_t n)
{
    memset(m_sllBuffer, 0, sizeof(m_sllBuffer));
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)	
    snprintf(m_sllBuffer, UINT64_HEX_BUF_LEN, "%016lix", n);
#else	
    snprintf(m_sllBuffer, UINT64_HEX_BUF_LEN, "%016llx", n);
#endif
    Write(m_sllBuffer, UINT64_HEX_BUF_LEN);

    return *this;
}

template <class ConvertorType>
template <class DATA>
inline
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(std::vector<DATA>& v)
{
	uint32_t sz = v.size();
	(*this) << sz;
	for (uint32_t i = 0; i < sz; ++i)
		(*this) << v[i];
	return *this;
}

template <class ConvertorType>
template <class DATA1, class DATA2>
inline
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(std::map<DATA1, DATA2>& v)
{
	uint32_t sz = v.size();
	(*this) << sz;
	typename std::map<DATA1, DATA2>::const_iterator it;
	for (it = v.begin(); it != v.end() ; ++it)
	{
		DATA1& key = (*const_cast<DATA1*>(&(*it).first));
		DATA2& val = (*const_cast<DATA2*>(&(*it).second));
		(*this) << key;
		(*this) << val;
	}	
	return *this;
}


template <class ConvertorType>
template <class DATA1, class DATA2>
inline
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(std::multimap<DATA1, DATA2>& v)
{
	uint32_t sz = v.size();
	(*this) << sz;
	typename std::multimap<DATA1, DATA2>::const_iterator it;
	for (it = v.begin(); it != v.end() ; ++it)
	{
		DATA1& key = (*const_cast<DATA1*>(&(*it).first));
		DATA2& val = (*const_cast<DATA2*>(&(*it).second));
		(*this) << key;
		(*this) << val;
	}	
	return *this;
}

template <class ConvertorType>
template <class DATA>
inline
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator<<(DATA& v)
{
	v.Serialize(*this);
	return *this;
}
////////////////////////////////////////////////////////////////
template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(int8_t& c)
{
    Read(&c, sizeof(int8_t));
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(uint8_t& c)
{
    Read(&c, sizeof(uint8_t));
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(bool& b)
{
	uint8_t c = 0;
    Read(&c, sizeof(uint8_t));
	b = ((c != 0) ? true : false);
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(int16_t& n)
{
    return *this >> (uint16_t&)n;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(uint16_t& n)
{
    Read(&n, sizeof(uint16_t));
    ConvertorType::Network2Host(n);
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(int32_t& n)
{
    return *this >> (uint32_t&)n;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(uint32_t& n)
{
    Read(&n, sizeof(uint32_t));
    ConvertorType::Network2Host(n);
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(std::string& str)
{
	//str.resize(0);
    uint32_t nLen = 0;
    (*this) >> nLen;

	if(0 == nLen && !str.empty())
		str = "";
    
    if (nLen > 0) 
    {
        assignStdString(str, nLen);
    }
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(uint64_t& n)
{
    memset(m_sllBuffer, 0, sizeof(m_sllBuffer));
    Read(m_sllBuffer, UINT64_HEX_BUF_LEN);
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)	
    sscanf(m_sllBuffer, "%016lix", &n);
#else
    sscanf(m_sllBuffer, "%016llx", &n);
#endif	
	return *this;
}

template <class ConvertorType>
template <class DATA>
inline
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(std::vector<DATA>& v)
{
	uint32_t sz = 0;
	(*this) >> sz;
	if(0 == sz)
		return *this;
	v.resize(sz);	
	for (uint32_t i = 0; i < sz; ++i)
		(*this) >> v[i];
	return *this;
}

template <class ConvertorType>
template <class DATA1, class DATA2>
inline
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(std::map<DATA1, DATA2>& v)
{
	uint32_t sz = 0;
	(*this) >> sz;
	if(0 == sz)
		return *this;

	DATA1 key;
	DATA2 val;
	for(uint32_t i = 0; i < sz; ++i)	
	{
		(*this) >> key;
		(*this) >> val;
		v[key] = val;
	}

	return *this;
}


template <class ConvertorType>
template <class DATA1, class DATA2>
inline
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(std::multimap<DATA1, DATA2>& v)
{
	uint32_t sz = 0;
	(*this) >> sz;
	if(0 == sz)
		return *this;

	DATA1 key;
	DATA2 val;
	for(uint32_t i = 0; i < sz; ++i)	
	{
		(*this) >> key;
		(*this) >> val;
        v.insert(std::pair<DATA1,DATA2>(key,val));
	}

	return *this;
}


template <class ConvertorType>
template <class DATA>
inline
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator>>(DATA& v)
{
	v.Serialize(*this);
	return *this;
}

template <class ConvertorType>
inline
CByteStreamT<ConvertorType>&
CByteStreamT<ConvertorType>::mmap(void** pAddr, uint32_t nLen)
{
    if (m_bReadGood)
    {
        if(nLen > m_nReadBufLenLeft)
        {
            m_bReadGood = false;
            return *this;
        }
        (*pAddr)=m_pStreamBuf;
        m_nReadBufLenLeft -= nLen;
        m_pStreamBuf += nLen;
    }
    return *this;
}


template <class ConvertorType>
inline 
bool CByteStreamT<ConvertorType>::isGood()
{
    if (m_bWriteGood && m_bReadGood)
        return true;
    else
        return false;
}

template <class ConvertorType>
inline 
uint32_t CByteStreamT<ConvertorType>::getWrittenLength()
{
    return m_nBufLen - m_nWriteBufLenLeft;
}

template <class ConvertorType>
inline 
uint32_t CByteStreamT<ConvertorType>::getReadLength()
{
    return m_nBufLen - m_nReadBufLenLeft;
}

template <class ConvertorType>
inline
uint32_t CByteStreamT<ConvertorType>::getRemainLength2Read()
{
    return m_nReadBufLenLeft;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::Read(void *pvDst, uint32_t nLen)
{
    if (m_bReadGood) 
    {
        if(nLen > m_nReadBufLenLeft)
        {
            m_bReadGood = false;
            return *this;
        }
        
        memcpy(pvDst, m_pStreamBuf, nLen);
        m_nReadBufLenLeft -= nLen;
        m_pStreamBuf += nLen;
    }
    return *this;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::Write(const void *pvSrc, uint32_t nLen)
{
    if(!m_bRealWrite)
    {
        m_nWriteBufLenLeft -= nLen;
        return *this;
    }

    if(m_bWriteGood) 
    {
        if(nLen > m_nWriteBufLenLeft)
        {
            m_bWriteGood = false;
            return *this;
        }
        
        memcpy(m_pStreamBuf, pvSrc, nLen);
        m_nWriteBufLenLeft -= nLen;
        m_pStreamBuf += nLen;
    }
    return *this;
}

template <class ConvertorType>
inline 
void CByteStreamT<ConvertorType>::assignStdString(std::string& str, uint32_t nLen)
{
    if (m_bReadGood) 
    {
        if(nLen > m_nReadBufLenLeft)
        {
            m_bReadGood = false;
            return;
        }
        
        //str.resize(0);
        //str.resize(nLen);
        str.assign(m_pStreamBuf, nLen);
        
        m_nReadBufLenLeft -= nLen;
        m_pStreamBuf += nLen;
    }
}


/////////////////////// Read or Write ////////////////////////
template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator & (int8_t& c)
{
    if(m_bStore)
        return *this << c;

    return *this >> c;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator & (uint8_t& c)
{
    if(m_bStore)
        return *this << c;

    return *this >> c;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator & (bool& b)
{
    if(m_bStore)
        return *this << b;

    return *this >> b;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>&
CByteStreamT<ConvertorType>::operator & (int16_t& n)
{
    if(m_bStore)
        return *this << n;

    return *this >> n;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>&
CByteStreamT<ConvertorType>::operator & (uint16_t& n)
{
    if(m_bStore)
        return *this << n;

    return *this >> n;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>&
CByteStreamT<ConvertorType>::operator & (int32_t& n)
{
    if(m_bStore)
        return *this << n;

    return *this >> n;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>&
CByteStreamT<ConvertorType>::operator & (uint32_t& n)
{
    if(m_bStore)
        return *this << n;

    return *this >> n;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>&
CByteStreamT<ConvertorType>::operator & (std::string& str)
{
    if(m_bStore)
        return *this << str;

    return *this >> str;
}

template <class ConvertorType>
inline 
CByteStreamT<ConvertorType>&
CByteStreamT<ConvertorType>::operator & (uint64_t& n)
{
    if(m_bStore)
        return *this << n;

    return *this >> n;
}

template <class ConvertorType>
template <class DATA>
inline
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator & (std::vector<DATA>& v)
{
    if(m_bStore)
        return *this << v;

    return *this >> v;
}

template <class ConvertorType>
template <class DATA1, class DATA2>
inline
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator & (std::map<DATA1, DATA2>& v)
{
    if(m_bStore)
        return *this << v;

    return *this >> v;
}


template <class ConvertorType>
template <class DATA1, class DATA2>
inline
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator & (std::multimap<DATA1, DATA2>& v)
{
    if(m_bStore)
        return *this << v;

    return *this >> v;
}

template <class ConvertorType>
template <class DATA>
inline
CByteStreamT<ConvertorType>& 
CByteStreamT<ConvertorType>::operator & (DATA& v)
{
    if(m_bStore)
        return *this << v;

    return *this >> v;
}
/////////////////////// Type Definition ////////////////////////
typedef CByteStreamT<CHostNetworkConvertorNormal> CByteStreamNetwork;
typedef CByteStreamT<CHostNetworkConvertorNull> CByteStreamMemory;

#endif /* BYTE_STREAM_H */

