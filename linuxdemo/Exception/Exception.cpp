#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>
#include "Exception.h"

std::string getStackTrace(bool demangle)
{
	std::string stack;
	const int max_frames = 1024;
	void* frame[max_frames];
	int nptrs = ::backtrace(frame, max_frames);
	char** strings = ::backtrace_symbols(frame, nptrs);
	if (strings)
	{
		size_t len = 256;
		char* demangled = demangle ? static_cast<char*>(::malloc(len)) : nullptr;
		for (int i = 1; i < nptrs; ++i)  // skipping the 0-th, which is this function
		{
			if (demangle)
			{
				char* left_par = nullptr;
				char* plus = nullptr;
				for (char* p = strings[i]; *p; ++p)
				{
					if (*p == '(')
						left_par = p;
					else if (*p == '+')
						plus = p;
				}

				if (left_par && plus)
				{
					*plus = '\0';
					int status = 0;
					char* ret = abi::__cxa_demangle(left_par+1, demangled, &len, &status);
					*plus = '+';
					if (status == 0)
					{
						demangled = ret;  // ret could be realloc()
						stack.append(strings[i], left_par+1);
						stack.append(demangled);
						stack.append(plus);
						stack.push_back('\n');
						continue;
					}
				}
			}
			// Fallback to mangled names
			stack.append(strings[i]);
			stack.push_back('\n');
		}
		free(demangled);
		free(strings);
	}
	
	return stack;
}



Exception::Exception(std::string msg)
  : m_szMessage(msg),
    m_szStack(getStackTrace(/*demangle=*/false))
{
}


