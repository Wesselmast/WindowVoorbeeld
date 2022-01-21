#pragma once 

// STL includes
#include <cassert>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>



/**
@brief Dirty typedefs for STL stuff. Macros are sometimes used over typedef for generic types.
TODO: Write our own version instead of using STL
**/
#define	HashMap			std::unordered_map
#define	Array			std::vector
#define	Pair			std::pair
using	String			= std::string;

/**
@brief Same as above but for functions
**/
#define gLog(...)		printf(__VA_ARGS__)
#define gAssert(...)	assert(__VA_ARGS__)



/**
@brief Integer rectangle class
**/
struct IRect
{
	int mX = 0;		///< X position
	int mY = 0;		///< Y position
	int mW = 0;		///< Width
	int mH = 0;		///< Height
};



/**
@brief WString STL wrapper
TODO: Write our own version instead of wrapping STL
**/
class WString 
{
public:
	///@name Construction
					WString()									= default;
					WString(std::wstring inUTF16Str) :			mBase(inUTF16Str) {}

	///@name Format conversion
	static WString	sFromUTF8(const String& inUTF8Str);			///< Create a WString from a String

	///@name Overloads
	operator		const wchar_t*() const						{ return mBase.c_str(); } ///< Implicitly convert to a c-style wide char pointer
	WString			operator+(const WString& inOther) const		{ return mBase + inOther.mBase; } ///< Concatenate two WStrings and return the result

private:
	///@name Properties
	std::wstring mBase;											///< STL base
};
