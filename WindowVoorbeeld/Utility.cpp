#include "Utility.h"



/**
@brief Create a WString from a String
**/
WString WString::sFromUTF8(const String& inUTF8Str)
{
	return WString(std::wstring(inUTF8Str.begin(), inUTF8Str.end()));
}
