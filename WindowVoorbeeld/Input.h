#pragma once

// Includes
#include <cstdint>
#include <vector>



/**
@brief Simple helper class to combine key checks
**/
class Key;
class KeyCombination
{
public:
	///@name Construction
							KeyCombination(const Key& inKey)	{ mKeys.push_back(inKey); }

	///@name Logic
	KeyCombination&			operator|(const Key& inKey);		///< OR keys together

private:
	friend class Input;											///< Input uses KeyCombination for verification

	///@name Properties
	std::vector<Key>		mKeys;								///< Combination of keys
};



/**
@brief Represents a keyboard key or mouse button
**/
class Key
{
public:
	///@name Construction
							Key(uint8_t inIndex) :	mIndex(inIndex) { }

	///@name Conversion
	operator				KeyCombination()		{ return KeyCombination(*this); } ///< Implicitly convert to a key combination for ORing keys together

private:
	friend class KeyRegistry;						///< KeyRegistry keys for indexing

	///@name Properties
	uint8_t					mIndex;					///< Key index into the registry
};



/**
@brief XOR keys together
**/
KeyCombination operator|(const Key& inFirst, const Key& inSecond);



/**
@brief Input static class
**/
using KeyCode = uint8_t;
class Input
{
public:
	static bool sIsDown(const Key& inKey);						///< Check if a key is down (e.g. Input::sIsDown(KEY_TAB))
	static bool sIsDown(const KeyCombination& inCombination);	///< Check if a key combination is down (e.g. Input::sIsDown(KEY_CTRL | KEY_ALT | KEY_DEL))

private:
	friend struct InputKey;										///< Allow InputKey classes with an input key to be able to also set input

	static void sSetDown(KeyCode inKeyCode, bool inDown);		///< Set @a inKeyCode to @a inDown
};



/**
@brief List of platform independent keys is here
**/

// Mouse Events
#define  MOUSE_L        Key(0)
#define  MOUSE_M        Key(1)
#define  MOUSE_R        Key(2)

// Key Events
#define  KEY_ESCAPE     Key(3)    
#define  KEY_BACKSPACE  Key(4)
#define  KEY_TAB        Key(5)
#define  KEY_ENTER      Key(6)
#define  KEY_CTRL       Key(7)
#define  KEY_SHIFT      Key(8)
#define  KEY_ALT 		Key(9)
#define  KEY_SPACE 		Key(10)
#define  KEY_DELETE 	Key(11)
							  
#define  KEY_1 	    	Key(12)    
#define  KEY_2 	    	Key(13)
#define  KEY_3 	    	Key(14)
#define  KEY_4 	    	Key(15)
#define  KEY_5 	    	Key(16)
#define  KEY_6 	    	Key(17)
#define  KEY_7 	    	Key(18)
#define  KEY_8 	    	Key(19)
#define  KEY_9 	    	Key(20)
#define  KEY_0 	    	Key(21)
							  
#define  KEY_A			Key(22)    
#define  KEY_B			Key(23)
#define  KEY_C			Key(24)
#define  KEY_D			Key(25)
#define  KEY_E			Key(26)
#define  KEY_F			Key(27)
#define  KEY_G			Key(28)
#define  KEY_H			Key(29)
#define  KEY_I			Key(30)
#define  KEY_J			Key(31)
#define  KEY_K			Key(32)
#define  KEY_L			Key(33)
#define  KEY_M			Key(34)
#define  KEY_N			Key(35)
#define  KEY_O			Key(36)
#define  KEY_P			Key(37)
#define  KEY_Q			Key(38)
#define  KEY_R			Key(39)
#define  KEY_S			Key(40)
#define  KEY_T			Key(41)
#define  KEY_U			Key(42)
#define  KEY_V			Key(43)
#define  KEY_W			Key(44)
#define  KEY_X			Key(45)
#define  KEY_Y			Key(46)
#define  KEY_Z			Key(47)
								  
#define  KEY_F1 		Key(48)    
#define  KEY_F2 		Key(49)
#define  KEY_F3 		Key(50)
#define  KEY_F4 		Key(51)
#define  KEY_F5 		Key(52)
#define  KEY_F6 		Key(53)
#define  KEY_F7 		Key(54)
#define  KEY_F8 		Key(55)
#define  KEY_F9 		Key(56)
#define  KEY_F10		Key(57)
#define  KEY_F11		Key(58)
#define  KEY_F12		Key(59)
