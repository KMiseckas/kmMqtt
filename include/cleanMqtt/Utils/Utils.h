#ifndef INCLUDE_CLEANMQTT_UTILS_UTILS_H
#define INCLUDE_CLEANMQTT_UTILS_UTILS_H

#include <cstring>
#include <vector>
#include <regex>

namespace cleanMqtt
{
	inline std::vector<std::string> splitByDelimiter(const std::string& target, const char* delimiter)
	{
		std::vector<std::string> tokens;

		std::regex del{ delimiter };
		std::sregex_token_iterator iter{ target.begin(), target.end(), del, -1 };
		std::sregex_token_iterator end;

		while (iter != end)
		{
			tokens.push_back(*iter);
			++iter;
		}

		return tokens;
	}
}

#endif //INCLUDE_CLEANMQTT_UTILS_UTILS_H 