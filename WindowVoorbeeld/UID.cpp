#include "UID.h"

// Win32 includes
#include <windows.h>


#pragma warning (push)
#pragma warning (disable : 6031)
/**
@brief Create a unique identifier
**/
UID UID::sCreate()
{
	GUID id;
	CoCreateGuid(&id);
	return *(UID*)&id;
}
#pragma warning (pop)


/**
@brief Compare UIDs
**/
bool UID::operator==(const UID& inUID)
{
	return IsEqualGUID(*(GUID*)this, *(GUID*)&inUID);
}



/**
@brief Compare UIDs
**/
bool UID::operator!=(const UID& inUID)
{
	return !(*this == inUID);
}
