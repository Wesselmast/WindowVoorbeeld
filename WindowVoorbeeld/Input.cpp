#include "Input.h"

// Additional includes
#include "Utility.h"



/**
@brief Lookup table for Win32 KeyCodes to Keys
**/
static HashMap<KeyCode, Key> gKeyCodeToKeyLUT = 
{
    // Mouse Events
    { 0x01, MOUSE_L }, 
    { 0x04, MOUSE_M }, 
    { 0x02, MOUSE_R }, 

    // Key Events
    { 0x1B, KEY_ESCAPE }, 
    { 0x08, KEY_BACKSPACE },  
    { 0x09, KEY_TAB },  
    { 0x0D, KEY_ENTER },  
    { 0x11, KEY_CTRL },  
    { 0x10, KEY_SHIFT },  
    { 0x12, KEY_ALT },
    { 0x20, KEY_SPACE },
    { 0x2E, KEY_DELETE },

    { 0x31, KEY_1 },      
    { 0x32, KEY_2 },  
    { 0x33, KEY_3 },  
    { 0x34, KEY_4 },  
    { 0x35, KEY_5 },  
    { 0x36, KEY_6 },  
    { 0x37, KEY_7 },  
    { 0x38, KEY_8 },  
    { 0x39, KEY_9 },  
    { 0x30, KEY_0 },  

    { 0x41, KEY_A },       
    { 0x42, KEY_B }, 		
    { 0x43, KEY_C }, 		
    { 0x44, KEY_D }, 		
    { 0x45, KEY_E }, 		
    { 0x46, KEY_F }, 		
    { 0x47, KEY_G }, 		
    { 0x48, KEY_H }, 		
    { 0x49, KEY_I }, 		
    { 0x4A, KEY_J }, 		
    { 0x4B, KEY_K }, 		
    { 0x4C, KEY_L }, 		
    { 0x4D, KEY_M }, 		
    { 0x4E, KEY_N }, 		
    { 0x4F, KEY_O }, 		
    { 0x50, KEY_P }, 		
    { 0x51, KEY_Q }, 		
    { 0x52, KEY_R }, 		
    { 0x53, KEY_S }, 		
    { 0x54, KEY_T }, 		
    { 0x55, KEY_U }, 		
    { 0x56, KEY_V }, 		
    { 0x57, KEY_W }, 		
    { 0x58, KEY_X }, 		
    { 0x59, KEY_Y }, 		
    { 0x5A, KEY_Z }, 		

    { 0x70, KEY_F1 },          
    { 0x71, KEY_F2 },	
    { 0x72, KEY_F3 },	
    { 0x73, KEY_F4 },	
    { 0x74, KEY_F5 },	
    { 0x75, KEY_F6 },	
    { 0x76, KEY_F7 },	
    { 0x77, KEY_F8 },	
    { 0x78, KEY_F9 },	
    { 0x79, KEY_F10 },	
    { 0x7A, KEY_F11 },	
    { 0x7B, KEY_F12 }
};



/**
@brief Warning for keys that have not been implemented yet
**/
#define gLogKeyNotImplemented(inKeyCode) gLog(R"(
[WARNING]
Key with KeyCode [0x%x] has not yet been implemented!
To implement, please refer to https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes, 
and implement in Input.h and Input.cpp. Thank you!

)", inKeyCode)



/**
@brief KeyRegistry class and instance
**/
static class KeyRegistry
{
public:
    KeyRegistry()
    {
        // Allocate in blocks of 64 bits. Calculation works like this:
        // Let LUT size be 100: 100 >> 6 = 1 + 1 = 2 blocks or 128 bits
        // Let LUT size be 18:  18  >> 6 = 0 + 1 = 1 block or 64 bits
        size_t size = ((gKeyCodeToKeyLUT.size() >> 6) + 1) * 64;
        if (mData = (uint64_t*)malloc(size))
            memset(mData, 0, size);
    };

    ~KeyRegistry()
    {
        free(mData);
    }

    void SetKeyDown(const Key& inKey, uint64_t inDown)
    {
        // Get the block size from the index by right shifting to the 6th bit
        uint8_t block_index = inKey.mIndex >> 6;

        // Set the keys index bit in the correct data registry. Calculation works like this:
        // Let KEY be 100: 
        //      - Block Index: 100 >> 6 = 1
        //      - Bit to set:  100 - (1 * 64) = 36
        // Bit 36 is set in the second data registry.
        // Let KEY be 18: 
        //      - Block Index: 18 >> 6 = 0
        //      - Bit to set:  18 - (0 * 64) = 18
        // Bit 18 is set in the first data registry.
        uint64_t& data_registry = mData[block_index];
        uint8_t shifted_key_index = inKey.mIndex - (block_index * 64);
        data_registry = (data_registry & ~(1ULL << shifted_key_index)) | (inDown << shifted_key_index);
    }

    bool GetKeyDown(const Key& inKey) const
    {
        // Get the block size from the index by right shifting to the 6th bit
        uint8_t block_index = inKey.mIndex >> 6;
        return mData[block_index] & 1ULL << (inKey.mIndex - (block_index * 64));
    }

private:
    uint64_t* mData;
} gKeyRegistry;



/**
@brief Check if @a inKey is down
**/
bool Input::sIsDown(const Key& inKey)
{
    return gKeyRegistry.GetKeyDown(inKey);
}



/**
@brief Check if @a inCombination is down
**/
bool Input::sIsDown(const KeyCombination& inCombination)
{
    bool all_down = true;
    for (const Key& key : inCombination.mKeys)
    {
        all_down &= gKeyRegistry.GetKeyDown(key);
        if (!all_down)
            return false;
    }
    return true;
}



/**
@brief Set @a inKeyCode to @a inDown
**/
void Input::sSetDown(KeyCode inKeyCode, bool inDown)
{
#ifdef _DEBUG
    // In debug mode, check first if the KeyCode was implemented
    if (gKeyCodeToKeyLUT.find(inKeyCode) == gKeyCodeToKeyLUT.end())
    {
        gLogKeyNotImplemented(inKeyCode);
        return;
    }
#endif
    gKeyRegistry.SetKeyDown(gKeyCodeToKeyLUT.at(inKeyCode), (uint8_t)inDown);
}



/**
@brief OR keys together
**/
KeyCombination& KeyCombination::operator|(const Key& inKey)
{
    mKeys.push_back(inKey);
    return *this;
}



/**
@brief OR keys together
**/
KeyCombination operator|(const Key& inFirst, const Key& inSecond)
{
    return ((KeyCombination)inFirst) | inSecond;
}
