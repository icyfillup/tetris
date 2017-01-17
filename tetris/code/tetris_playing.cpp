#include "tetris.h"
#include "tetris_playing.h"

internal uint32 GetPositionOnScreenArea(const screen_area* ScreenArea,
                                        int32 X, int32 Y)
{
    Assert(X * Y >= 0 &&
           X * Y < int32(ScreenArea->Width * ScreenArea->Height));
    
    uint32 Result = (Y * ScreenArea->Width) + X;
    
    return Result;
}

internal void SetBlockPixel(block *Block, int32 X, int32 Y)
{
    switch(Block->Type)
    {
        
        case O:
        {
            Block->Pixel[0] = {X, Y};
            Block->Pixel[1] = {X, Y + 1};
            Block->Pixel[2] = {X + 1, Y};
            Block->Pixel[3] = {X + 1, Y + 1};
        }break;
        
        case l:
        {
            Block->Pixel[0] = {X, Y};
            Block->Pixel[1] = {X, Y - 1};
            Block->Pixel[2] = {X, Y - 2};
            Block->Pixel[3] = {X, Y + 1};
        }break;
        
        case T:
        {
            Block->Pixel[0] = {X, Y};
            Block->Pixel[1] = {X, Y + 1};
            Block->Pixel[2] = {X - 1, Y};
            Block->Pixel[3] = {X + 1, Y};
        }break;
        
        case L:
        {
            Block->Pixel[0] = {X, Y};
            Block->Pixel[1] = {X, Y + 1};
            Block->Pixel[2] = {X, Y - 1};
            Block->Pixel[3] = {X + 1, Y - 1};
        }break;
        
        case J:
        {
            Block->Pixel[0] = {X, Y};
            Block->Pixel[1] = {X, Y + 1};
            Block->Pixel[2] = {X, Y - 1};
            Block->Pixel[3] = {X - 1, Y - 1};
        }break;
        
        case Z:
        {
            Block->Pixel[0] = {X, Y};
            Block->Pixel[1] = {X - 1, Y};
            Block->Pixel[2] = {X, Y - 1};
            Block->Pixel[3] = {X + 1, Y - 1};
        }break;
        
        case S:
        {
            Block->Pixel[0] = {X, Y};
            Block->Pixel[1] = {X + 1, Y};
            Block->Pixel[2] = {X, Y - 1};
            Block->Pixel[3] = {X - 1, Y - 1};
        }break;
        
        default:
        {
            int i = 1;
            Assert(i == 0);
        }break;
    }
}

internal void GetNextBlock(block *Block, uint32 RandomNumber)
{
    // set block type
#if 1
    
    uint32 RandomBlockType = RandomNumber % 7;
    
    switch(RandomBlockType)
    {
        case 0:
        {
            Block->Type = O;
        }break;
        
        case 1:
        {
            Block->Type = l;
        }break;
        
        case 2:
        {
            Block->Type = T;
        }break;
        
        case 3:
        {
            Block->Type = L;
        }break;
        
        case 4:
        {
            Block->Type = J;
        }break;
        
        case 5:
        {
            Block->Type = Z;
        }break;
        
        case 6:
        {
            Block->Type = S;
        }break;
        
        default:
        {
            int i = 0;
            Assert(i == 1);
        }break;
    }
    
#else
    
    Block->Type = O;
    
#endif
    RandomNumberIndex++;
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










