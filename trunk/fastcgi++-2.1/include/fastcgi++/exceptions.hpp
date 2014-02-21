//! \file exceptions.hpp Defines fastcgi++ exceptions
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


#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>

//! Topmost namespace for the fastcgi++ library
namespace Fastcgipp
{
	//! Includes all exceptions used by the fastcgi++ library
	namespace Exceptions
	{
		/** 
		 * @brief General exception class for container errnos and messages.
		 */
		struct CodedException: public std::exception
		{
			/** 
			 * @brief Associated error number.
			 */
			const int erno;
			/** 
			 * @brief Pointer to string data explaining error.
			 */
			const char* msg;
			/** 
			 * @param[in] msg_ Pointer to string explaining error.
			 * @param[in] erno_ Associated error number.
			 */
			CodedException(const char* msg_, const int erno_): erno(erno_), msg(msg_) {}
			const char* what() const throw() { return msg; }
		};

		/** 
		 * @brief Exception for code conversion errors between UTF-16/32 and UTF-8
		 */
		struct CodeCvt: public std::exception
		{
			const char* what() const throw() { return "Code Conversion error"; }
		};
	}
}

#endif
