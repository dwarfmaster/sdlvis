
#ifndef DEF_TOOLS
#define DEF_TOOLS

#include <string>
#include <sstream>

template<typename T>
std::string toString(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

template<typename T>
T fromString(const std::string& str)
{
	T ret;
	std::istringstream iss(str);
	iss >> ret;
	return ret;
}
 
#endif

