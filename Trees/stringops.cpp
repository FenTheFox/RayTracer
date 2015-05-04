#include "stdafx.h"
#include "stringops.h"

std::string trim (std::string str, std::string whitespace)
{
	auto strBegin = str.find_first_not_of (whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	auto strEnd = str.find_last_not_of (whitespace);
	auto strRange = strEnd - strBegin + 1;

	return str.substr (strBegin, strRange);
}

std::string substring (std::string str, size_t start, size_t end)
{
	return str.substr (start, str.length () - end);
}