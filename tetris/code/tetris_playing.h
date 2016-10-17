#if !defined(TETRIS_PLAYING_H)

struct playing_screen
{
    uint32 Pitch;
    real32 TimePassed;
    
    screen_area *ScreenArea;
};

struct block
{
    v2 Pixel[4];
    bool32 Landed;
};

struct next_block_screen
{
    v2 ScreenPos;
    uint32 Pitch;

    screen_area *ScreenArea;
    block *NextBlock;
};

struct score_screen
{
    v2 ScreenPos;
    
    uint32 Pitch;
    uint32 Score;

    screen_area *ScreenArea;
};

#define TETRIS_PLAYING_H
#endif
