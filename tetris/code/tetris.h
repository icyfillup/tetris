#if !defined(TETRIS_H)

#include "tetris_platform.h"

#if TETRIS_SLOW
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#define internal static 
#define local_persist static 
#define global_variable static

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)

#define ArrayCount(Array) (sizeof((Array)) / sizeof((Array)[0]))


inline uint32 SafeTruncateUInt64(uint64 Value)
{
    //TODO: Define Max Integer Value for 0XFFFFFFFF
    Assert(Value <= 0xFFFFFFFF);
    uint32 Result = (uint32) Value;
    return Result;
}

struct memory_arena
{
    memory_index Size;
    memory_index Used;
    uint8 *Base;
};

#define PushArray(Arena, Count, type) (type *)PushSize_(Arena, Count * sizeof(type))
#define PushStruct(Arena, type) (type *)PushSize_(Arena,  sizeof(type))
void *PushSize_(memory_arena *Arena, memory_index Size)
{
    Assert((Arena->Used + Size) <= Arena->Size)
    
    void *Result = Arena->Base + Arena->Used;
    Arena->Used += Size; 
    return Result;
}

internal void InitArena(memory_arena *Arena, uint8 *Base, memory_index Size)
{
    Arena->Size = Size;
    Arena->Base = Base;
    Arena->Used = 0;
}

internal void DescSort(int32 Array[], int32 Size)
{
    bool32 Swap = 1;
     
    while(Swap)
    {
        Swap = 0;
        for(int32 i = 1; i < Size; i++)
        {
            if(Array[i - 1] < Array[i])
            {
                Swap = Array[i - 1] < Array[i];
                int32 Temp = Array[i];
                Array[i] = Array[i - 1];
                Array[i - 1] = Temp;
            }
        }
    }    
}

#include "tetris_math.h"

struct screen_area
{
    v2 WorldPos;

    uint32 Width;
    uint32 Height;

    uint8* Pixel;
};

#include "tetris_playing.h"

struct player
{
    //NOTE: Player's Position and Block->Pixel[0]
    //      is the center point implicitly. Be careful
    v2 *Position;
    v2 Delta;
        
    block *Block;
};

struct tetris_world
{
    playing_screen *PlayingScreen;
    next_block_screen *NextBlockScreen;
    score_screen *ScoreScreen;
};

struct game_state
{
    memory_arena TetrisArena;

    tetris_world *TetrisWorld;
    player Player;
};

#define TETRIS_H
#endif
