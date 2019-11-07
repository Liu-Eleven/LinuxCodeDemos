
#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <string>


class Exception : public std::exception
{
public:
	Exception(std::string what);
	~Exception() noexcept override = default;

// default copy-ctor and operator= are okay.

	const char* what() const noexcept override
	{
		return m_szMessage.c_str();
	}

	const char* stackTrace() const noexcept
	{
		return m_szStack.c_str();
	}

private:
	std::string m_szMessage;
	std::string m_szStack;
};

extern std::string getStackTrace(bool demangle);

#endif  // _EXCEPTION_H_
