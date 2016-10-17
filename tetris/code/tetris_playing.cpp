#include "tetris_playing.h"

internal uint32 GetPositionOnScreenArea(const screen_area* ScreenArea,
                                        int32 X, int32 Y)
{
    Assert(X * Y >= 0 &&
           X * Y < int32(ScreenArea->Width * ScreenArea->Height));
    
    uint32 Result = (Y * ScreenArea->Width) + X;
    
    return Result;
}

void BlockShape(block *Block, int32 X, int32 Y)
{
    
#if 0 
    Block->Pixel[0] = {X    , Y};
    Block->Pixel[1] = {X    , Y + 1};
    Block->Pixel[2] = {X - 1, Y};
    Block->Pixel[3] = {X + 1, Y};
    
#else
    Block->Pixel[0] = {X    , Y};
    Block->Pixel[1] = {X    , Y + 1};
    Block->Pixel[2] = {X + 1, Y};
    Block->Pixel[3] = {X + 1, Y + 1};
#endif
    
}

internal void GetNextBlock(block *Block, uint32 RandomNumberIndex, int32 X, int32 Y)
{
     BlockShape(Block, X, Y);
}

internal void SetBlockToPlayingScreen(playing_screen* PlayingScreen,
                                      const block* Block)
{
    for(uint32 i = 0; i < ArrayCount(Block->Pixel); i++)
    {
        uint32 Position = GetPositionOnScreenArea(PlayingScreen->ScreenArea, Block->Pixel[i].X, Block->Pixel[i].Y);
        *(PlayingScreen->ScreenArea->Pixel + Position) = '#';
    }
    
}

internal
bool32 IsThisPositionFilled(const screen_area *ScreenArea, int32 X, int32 Y)
{
    int32 ResultPosition = GetPositionOnScreenArea(ScreenArea, X, Y);
            
    return *(ScreenArea->Pixel + ResultPosition);
}

internal void
WriteIntoScreenArea(screen_area* ScreenArea, int32 X, int32 Y, uint8 Pasting)
{
    int32 ResultPosition = GetPositionOnScreenArea(ScreenArea, X, Y);
    
    *(ScreenArea->Pixel + ResultPosition) = Pasting;
}

internal void TransiteBlock(block *Block, int32 MoveVertical, int32 MoveHorizontal)
{

    if(MoveVertical)
    {
        Block->Pixel[0].Y += MoveVertical;
        Block->Pixel[1].Y += MoveVertical;
        Block->Pixel[2].Y += MoveVertical;
        Block->Pixel[3].Y += MoveVertical;
    }

    if(MoveHorizontal)
    {
        Block->Pixel[0].X += MoveHorizontal;
        Block->Pixel[1].X += MoveHorizontal;
        Block->Pixel[2].X += MoveHorizontal;
        Block->Pixel[3].X += MoveHorizontal;
    }      
}

internal void RotateBlock(block* Block)
{
    v2 CenterPoint = Block->Pixel[0];
        
    for(int32 i = 1; i < ArrayCount(Block->Pixel); i++)
    {
        Block->Pixel[i].X -= CenterPoint.X;
        Block->Pixel[i].Y -= CenterPoint.Y;

        int32 Temp = Block->Pixel[i].Y;
        Block->Pixel[i].Y = Block->Pixel[i].X;
        Block->Pixel[i].X = -Temp;

        Block->Pixel[i].X += CenterPoint.X;
        Block->Pixel[i].Y += CenterPoint.Y;

    }
}










