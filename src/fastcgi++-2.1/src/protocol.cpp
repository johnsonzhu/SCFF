//! \file protocol.cpp Defines FastCGI protocol
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


#include <fastcgi++/protocol.hpp>

void Fastcgipp::Protocol::processParamHeader(const char* data, size_t dataSize, const char*& name, size_t& nameSize, const char*& value, size_t& valueSize)
{
	if(*data>>7)
	{
		nameSize=readBigEndian(*(uint32_t*)data) & 0x7fffffff;
		data+=sizeof(uint32_t);
	}
	else nameSize=*data++;

	if(*data>>7)
	{
		valueSize=readBigEndian(*(uint32_t*)data) & 0x7fffffff;
		data+=sizeof(uint32_t);
	}
	else valueSize=*data++;
	name=data;
	value=name+nameSize;
}

Fastcgipp::Protocol::ManagementReply<14, 2, 8> Fastcgipp::Protocol::maxConnsReply("FCGI_MAX_CONNS", "10");
Fastcgipp::Protocol::ManagementReply<13, 2, 1> Fastcgipp::Protocol::maxReqsReply("FCGI_MAX_REQS", "50");
Fastcgipp::Protocol::ManagementReply<15, 1, 8> Fastcgipp::Protocol::mpxsConnsReply("FCGI_MPXS_CONNS", "1");

const char* Fastcgipp::Protocol::recordTypeLabels[] = { "INVALID", "BEGIN_REQUEST", "ABORT_REQUEST", "END_REQUEST", "PARAMS", "IN", "OUT", "ERR", "DATA", "GET_VALUES", "GET_VALUES_RESULT", "UNKNOWN_TYPE" };

const char Fastcgipp::version[]=PACKAGE_VERSION;
