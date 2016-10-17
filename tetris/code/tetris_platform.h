#if !defined(TETRIS_PLATFORM_H)

#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

typedef size_t memory_index;

struct thread_context
{
    int PlaceHolder;
};

#if TETRIS_INTERNAL

struct debug_read_file_result
{
    void *Content;
    uint32 ContentSize;
};

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(thread_context *Thread, void* Memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_READ_FROM_FILE(name) debug_read_file_result name(thread_context *Thread, char* FileName)
typedef DEBUG_PLATFORM_READ_FROM_FILE(debug_platform_read_from_file);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(thread_context *Thread, char* FileName, uint32 MemorySize, void* Memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);

#define DEBUG_OUTPUT_MESSAGE(name) void name(char* StringMsg, real32 NumberMsg)
typedef DEBUG_OUTPUT_MESSAGE(debug_output_message);

#endif

struct game_screen_buffer
{
    void* ScreenBit;
    int32 Width;
    int32 Height;
    int32 Pitch;
};

struct game_button_state
{
    int32 HalfTransitionCount;
    bool32 EndedDown;
};

struct game_controller
{
    real32 MSPerFrame; 
    union
    {
        game_button_state Button[5];
        struct
        {
            game_button_state Up;
            game_button_state Left;
            game_button_state Down;
            game_button_state Right;
            game_button_state Button1;
        };
    };
};

struct game_memory
{
    bool32 IsInit;
    
    debug_platform_free_file_memory* DEBUGPlatformFreeFileMemory;
    debug_platform_read_from_file* DEBUGPlatformReadFromFile;
    debug_platform_write_entire_file* DEBUGPlatformWriteEntireFile;
    debug_output_message* DEBUGOutputMessage;

    void* PermanentStorage;
    uint64 PermanentStorageSize;

    uint64 TransientStorageSize;
    void* TransientStorage;
};

#define GAME_UPDATE_AND_RENDER(name) void name(thread_context *Thread, game_memory* Memory, game_screen_buffer* Buffer, game_controller* NewInput)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub)
{

}


#define TETRIS_PLATFORM_H
#endif
