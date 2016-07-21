
#include "ccUtils.h"

namespace CrossApp {

CC_DLL unsigned long ccNextPOT(unsigned long x)
{
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >>16);
    return x + 1;
}

CC_DLL std::vector<std::string> Parse2StrVector(const std::string& str, const std::string& strDelimiter, bool bIncludeEmptyStr)
{
	std::vector<std::string> strVector;

	size_t pos = 0;
	if (!str.empty())
	{
		size_t first = str.find(strDelimiter);
		while (first != std::string::npos)
		{
			std::string cszStrTemp = str.substr(pos, first - pos);
			if (!cszStrTemp.empty() || bIncludeEmptyStr)
				strVector.push_back(cszStrTemp);

			pos = first + strDelimiter.size();
			first = str.find(strDelimiter, pos);
		}
		std::string cszStrTemp = str.substr(pos, str.size() - pos);
		if (!cszStrTemp.empty() || bIncludeEmptyStr)
			strVector.push_back(cszStrTemp);
	}
	return strVector;
}

#define MAX_ITOA_BUFFER_SIZE 256
    double atof(const char* str)
    {
        if (str == nullptr)
        {
            return 0.0;
        }
        
        char buf[MAX_ITOA_BUFFER_SIZE];
        strncpy(buf, str, MAX_ITOA_BUFFER_SIZE);
        
        // strip string, only remain 7 numbers after '.'
        char* dot = strchr(buf, '.');
        if (dot != nullptr && dot - buf + 8 <  MAX_ITOA_BUFFER_SIZE)
        {
            dot[8] = '\0';
        }
        
        return ::atof(buf);
    }
    
}