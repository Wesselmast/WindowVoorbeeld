#pragma once

// 
#include "Utility.h"


/**
@brief UID is a 128-bit integer that can be used to uniquely identify resources
**/
class UID
{
public:
	///@name Static create function
	static UID	sCreate();

	///@name No constructor, use UID::sCreate instead
				UID() = delete;

	///@name Operators
	bool		operator==(const UID& inUID); ///< Compare UIDs
	bool		operator!=(const UID& inUID); ///< Compare UIDs

private:
	///@name Properties
	uint32_t	mData1;
	uint16_t	mData2;
	uint16_t	mData3;
	uint64_t	mData4;
};

