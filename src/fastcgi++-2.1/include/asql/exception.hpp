#include <exception>

namespace ASql
{
	//! SQL Error
	struct Error: public std::exception
	{
		//! Associated error number.
		int erno;

		//! Pointer to string data explaining error.
		const char* msg;

		/*!
		 * @param[in] msg_ Pointer to string explaining error.
		 * @param[in] erno_ Associated error number.
		 */
		Error(const char* msg_, const int erno_): erno(erno_), msg(msg_) {}
		Error(): erno(0), msg(0) {}
		const char* what() const throw() { return msg; }

		Error(const Error& e): erno(e.erno), msg(e.msg)  {}
	};
}
