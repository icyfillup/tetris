#include "tetris.h"
#include "tetris_random.h"
#include "tetris_playing.cpp"

internal screen_area* CreateScreenArea(memory_arena* TetrisArena,
                                       int32 WorldX, int32 WorldY,
                                       uint32 Width, uint32 Height)
{
    screen_area *Result = PushStruct(TetrisArena, screen_area);

    Result->WorldPos = {WorldX, WorldY};

    Result->Width = Width;
    Result->Height = Height;

    Result->Pixel = PushArray(TetrisArena,
                              Result->Height * Result->Width,
                              uint8);

    return Result;
}

//TODO: Current using screen position as array index; a i - 1 problem. fix

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    
    game_state *GameState = (game_state *) Memory->PermanentStorage;
    
    //TODO: Should i introduce a new file for tetris block or
    //      make a tetris map file with the tetris block info

    local_persist v2 StartingPoint = {};

    
    if(!Memory->IsInit)
    {
        InitArena(&GameState->TetrisArena,
                  (uint8 *)Memory->PermanentStorage + sizeof(game_state),
                  Memory->PermanentStorageSize - sizeof(game_state));

        GameState->TetrisWorld = PushStruct(&GameState->TetrisArena, tetris_world);
        
        {//Playing Screen
            GameState->TetrisWorld->PlayingScreen =
                PushStruct(&GameState->TetrisArena, playing_screen);

            GameState->TetrisWorld->PlayingScreen->ScreenArea =
                CreateScreenArea(&GameState->TetrisArena,
                                 0, 0, 10, 20);
                
            GameState->TetrisWorld->PlayingScreen->Pitch = Buffer->Width;
            GameState->TetrisWorld->PlayingScreen->TimePassed = 0;
        }

        {//Player
            GameState->Player.Block =
                PushStruct(&GameState->TetrisArena, block);
            
            StartingPoint = {GameState->TetrisWorld->PlayingScreen->ScreenArea->Width/2,
                             GameState->TetrisWorld->PlayingScreen->ScreenArea->Height - 2};

            {
                GetNextBlock(GameState->Player.Block, 0,
                             StartingPoint.X,
                             StartingPoint.Y);
                GameState->Player.Position = &GameState->Player.Block->Pixel[0];
            }
        }
        
        {//Next Block Screen
            GameState->TetrisWorld->NextBlockScreen =
                PushStruct(&GameState->TetrisArena, next_block_screen);
            GameState->TetrisWorld->NextBlockScreen->NextBlock =
                PushStruct(&GameState->TetrisArena, block);
            GameState->TetrisWorld->NextBlockScreen->ScreenArea =
                CreateScreenArea(&GameState->TetrisArena,
                                 13, 10, 4, 4);

            GameState->TetrisWorld->NextBlockScreen->Pitch = Buffer->Width;
            GameState->TetrisWorld->NextBlockScreen->ScreenPos.X =
                ((GameState->TetrisWorld->NextBlockScreen->ScreenArea->Width / 2) - 1);
            GameState->TetrisWorld->NextBlockScreen->ScreenPos.Y =
                ((GameState->TetrisWorld->NextBlockScreen->ScreenArea->Height / 2) - 1);
            GetNextBlock(GameState->TetrisWorld->NextBlockScreen->NextBlock,
                         0,
                         GameState->TetrisWorld->NextBlockScreen->ScreenPos.X,
                         GameState->TetrisWorld->NextBlockScreen->ScreenPos.Y);
        }

        {//Score Screen
            GameState->TetrisWorld->ScoreScreen =
                PushStruct(&GameState->TetrisArena, score_screen);
            GameState->TetrisWorld->ScoreScreen->ScreenArea =
                CreateScreenArea(&GameState->TetrisArena,
                                 11, 15, 8, 2);
            GameState->TetrisWorld->ScoreScreen->ScreenPos.X = 0;            
            GameState->TetrisWorld->ScoreScreen->ScreenPos.Y = 0;
            GameState->TetrisWorld->ScoreScreen->Pitch = Buffer->Width;
            GameState->TetrisWorld->ScoreScreen->Score = 0;
        }
                
        Memory->IsInit = true;
    }

    tetris_world *TetrisWorld = GameState->TetrisWorld;
    playing_screen *PlayingScreen = TetrisWorld->PlayingScreen;
    next_block_screen *NextBlockScreen = TetrisWorld->NextBlockScreen;
    score_screen *ScoreScreen = TetrisWorld->ScoreScreen;
    player *Player = &GameState->Player;
    
    //TODO: When the block lands, get it a one second(?) pause before
    //      getting the next block

    //TODO     : Once or if the PlayingScreen move to another position
    //IMPORTANT  on the Buffer Screen, anything related to position on
    //           the PlayingScreen will be Offset. Canonicalize position
    //           later.

    bool32 FastDrop = 0;
    
    Player->Delta = {0, 0};
    
    Player->Delta.X += (NewInput->Left.EndedDown) ? -1 : 0;
    Player->Delta.X += (NewInput->Right.EndedDown) ? 1 : 0;
    
    if(NewInput->Down.EndedDown)
    {
        Player->Delta.Y += -1;
        PlayingScreen->TimePassed = 0;
    }
    FastDrop = (NewInput->Up.EndedDown);

    if(NewInput->Button1.EndedDown)
    {
        RotateBlock(Player->Block);
    }
    
    if(FastDrop)
    {
        bool32 BlockHits = 0;
        bool32 OutOfBoundHit = 0;
        do
        {
            BlockHits = 0;
            OutOfBoundHit = 0;
            for(uint32 i = 0; i < ArrayCount(Player->Block->Pixel); i++)
            {
                if((Player->Block->Pixel[i].Y - 1) < 0)
                {
                    OutOfBoundHit = 1;
                    break;
                }
                if(IsThisPositionFilled(PlayingScreen->ScreenArea,
                                        Player->Block->Pixel[i].X,
                                        Player->Block->Pixel[i].Y - 1))
                {
                    BlockHits = 1;
                    break;
                }
            
            }
            
            if(OutOfBoundHit)
            {
                break;
            }
            if(!BlockHits)
            {
                TransiteBlock(Player->Block, -1, 0);
            }
        }
        while(!BlockHits);
     
        Player->Block->Landed = 1;
    }
    
    //NOTE: Controling the rate in switch the Player->Block Falls
    {
        if(!FastDrop)
        {
#if 1
            PlayingScreen->TimePassed += NewInput->MSPerFrame + 0.3f;
#else
            PlayingScreen->TimePassed += NewInput->MSPerFrame;
#endif        
        }
        
        if(PlayingScreen->TimePassed > 1)
        {
            PlayingScreen->TimePassed = 0;
            Player->Delta.Y -= 1;
        }
    }

    
    v2 RelativePlayerPos = {Player->Delta.X, Player->Delta.Y};

    block RelativePlayerBlock = {};
    RelativePlayerBlock.Pixel[0] = Player->Block->Pixel[0] + RelativePlayerPos;
    RelativePlayerBlock.Pixel[1] = Player->Block->Pixel[1] + RelativePlayerPos;
    RelativePlayerBlock.Pixel[2] = Player->Block->Pixel[2] + RelativePlayerPos;
    RelativePlayerBlock.Pixel[3] = Player->Block->Pixel[3] + RelativePlayerPos;
    
    
        
    {//NOTE: Collision with the PlayingScreen
 
        for(int32 i = 0; i < ArrayCount(RelativePlayerBlock.Pixel); i++)
        {
            if(!(RelativePlayerBlock.Pixel[i].X <
                 (int32)PlayingScreen->ScreenArea->Width))
            {
                TransiteBlock(&RelativePlayerBlock, 0, -1);
            }

            if(RelativePlayerBlock.Pixel[i].X < 0)
            {
                TransiteBlock(&RelativePlayerBlock, 0, 1);
            }

            if(!(RelativePlayerBlock.Pixel[i].Y <
                 (int32)PlayingScreen->ScreenArea->Height))
            {// TODO: block should not hit screen height until block stacks hits it
                TransiteBlock(&RelativePlayerBlock, -1, 0);
            }    

            if(Player->Delta.Y && RelativePlayerBlock.Pixel[i].Y < 0)
            {
                TransiteBlock(&RelativePlayerBlock, 1, 0);
                Player->Block->Landed = RelativePlayerBlock.Pixel[i].Y <= 0;
            } 
        }
    }    

    {//NOTE: Collision with other blocks
        
        {//NOTE:Collision Right
            if(RelativePlayerPos.X > 0)
            {
                for(uint32 i = 0; i < ArrayCount(RelativePlayerBlock.Pixel); i++)
                {
                    if(IsThisPositionFilled(PlayingScreen->ScreenArea,
                                            RelativePlayerBlock.Pixel[i].X,
                                            RelativePlayerBlock.Pixel[i].Y))
                    {
                        TransiteBlock(&RelativePlayerBlock, 0, -1);
                        continue;
                    }
                }
            }
        }
        
        {//NOTE:Collision Left
            if(RelativePlayerPos.X < 0)
            {
                for(uint32 i = 0; i < ArrayCount(RelativePlayerBlock.Pixel); i++)
                {
                    if(IsThisPositionFilled(PlayingScreen->ScreenArea,
                                            RelativePlayerBlock.Pixel[i].X,
                                            RelativePlayerBlock.Pixel[i].Y))
                    {
                        TransiteBlock(&RelativePlayerBlock, 0, 1);
                        continue;
                    }
                }
            }
        }
#if 1        
        {//NOTE:Collision Bottom
            if(RelativePlayerPos.Y < 0)
            {
                for(uint32 i = 0; i < ArrayCount(RelativePlayerBlock.Pixel); i++)
                {
                    if(IsThisPositionFilled(PlayingScreen->ScreenArea,
                                            RelativePlayerBlock.Pixel[i].X,
                                            RelativePlayerBlock.Pixel[i].Y))
                    {
                        TransiteBlock(&RelativePlayerBlock, 1, 0);
                        Player->Block->Landed = 1;
                    }
                }
            }
        }
#endif
    }
    
    Player->Block->Pixel[0] = RelativePlayerBlock.Pixel[0];
    Player->Block->Pixel[1] = RelativePlayerBlock.Pixel[1];
    Player->Block->Pixel[2] = RelativePlayerBlock.Pixel[2];
    Player->Block->Pixel[3] = RelativePlayerBlock.Pixel[3];
    
    if(Player->Block->Landed)
    {
        SetBlockToPlayingScreen(PlayingScreen, Player->Block);
        {
            int32 BlockPixelY[4] = {Player->Block->Pixel[0].Y, Player->Block->Pixel[1].Y, Player->Block->Pixel[2].Y, Player->Block->Pixel[3].Y};
            DescSort(BlockPixelY, ArrayCount(BlockPixelY));

            for(uint32 j = 0; j < ArrayCount(BlockPixelY); j++)
            {
                bool32 IsSlotStillFilled = 0;
                for(uint32 i = 0; i < PlayingScreen->ScreenArea->Width; i++)
                {
                    IsSlotStillFilled = IsThisPositionFilled(PlayingScreen->ScreenArea, i, BlockPixelY[j]); 
                    
                    if(IsSlotStillFilled)
                    {
                        continue;
                    }
                    break;
                }
                if(IsSlotStillFilled)
                {
                    //TODO: Check this for error
                    for(uint32 x = 0; x < PlayingScreen->ScreenArea->Width; x++)
                    {
                        WriteIntoScreenArea(PlayingScreen->ScreenArea,
                                            x, BlockPixelY[j], 0);
                    }

                    for(uint32 y = BlockPixelY[j] + 1; y < PlayingScreen->ScreenArea->Height; y++)
                    {
                        for(uint32 x = 0; x < PlayingScreen->ScreenArea->Width; x++)
                        {
                            uint8 Copy = PlayingScreen->ScreenArea->Pixel[GetPositionOnScreenArea(PlayingScreen->ScreenArea, x, y)];
                            WriteIntoScreenArea(PlayingScreen->ScreenArea,
                                                x, y - 1, Copy);
                            WriteIntoScreenArea(PlayingScreen->ScreenArea,
                                                x, y, 0);

                        }
                    }
                }
            }
        }

        Player->Block->Landed = 0;
        PlayingScreen->TimePassed = 0;
        block *Temp = Player->Block;
        Player->Block = NextBlockScreen->NextBlock;
        NextBlockScreen->NextBlock = Temp;

        GetNextBlock(NextBlockScreen->NextBlock, 0,
                     NextBlockScreen->ScreenPos.X,
                     NextBlockScreen->ScreenPos.Y);

        {
            GetNextBlock(Player->Block, 0,
                         StartingPoint.X,
                         StartingPoint.Y);
            Player->Position = &Player->Block->Pixel[0];
        }
        
    }

    for(uint32 i = 0; i < ArrayCount(NextBlockScreen->NextBlock->Pixel); i++)
    {
        WriteIntoScreenArea(NextBlockScreen->ScreenArea,
                            NextBlockScreen->NextBlock->Pixel[i].X,
                            NextBlockScreen->NextBlock->Pixel[i].Y, '#');
        
    }


//NOTE: Write to the screen Buffer
    for(uint32 Y_Index = 0; Y_Index < PlayingScreen->ScreenArea->Height; Y_Index++)
    {
        for(uint32 X_Index = 0; X_Index < PlayingScreen->ScreenArea->Width; X_Index++)
        {            
            if(IsThisPositionFilled(PlayingScreen->ScreenArea, X_Index, Y_Index))
            {
                *(((uint8 *)Buffer->ScreenBit) +
                  ((Y_Index + PlayingScreen->ScreenArea->WorldPos.Y) * PlayingScreen->Pitch) +
                  (X_Index+ PlayingScreen->ScreenArea->WorldPos.X))
                    =
                    *(PlayingScreen->ScreenArea->Pixel +
                      GetPositionOnScreenArea(PlayingScreen->ScreenArea,
                                              X_Index, Y_Index));
            }
            else
            {
                *(((uint8 *)Buffer->ScreenBit) +
                  (Y_Index * PlayingScreen->Pitch) +
                  X_Index) = ' ';
            }
        }
    }

    for(uint32 Y_Index = 0; Y_Index < NextBlockScreen->ScreenArea->Height; Y_Index++)
    {
        for(uint32 X_Index = 0; X_Index < NextBlockScreen->ScreenArea->Width; X_Index++)
        {                
            if(IsThisPositionFilled(NextBlockScreen->ScreenArea,
                                    X_Index, Y_Index))
            {
                *(((uint8 *)Buffer->ScreenBit) +
                  ((NextBlockScreen->ScreenArea->WorldPos.Y + Y_Index) * NextBlockScreen->Pitch) +
                  (X_Index + NextBlockScreen->ScreenArea->WorldPos.X))
                    =
                    *(NextBlockScreen->ScreenArea->Pixel +
                      GetPositionOnScreenArea(NextBlockScreen->ScreenArea,
                                              X_Index, Y_Index));
            }
            else
            {
                *(((uint8 *)Buffer->ScreenBit) +
                  ((Y_Index + NextBlockScreen->ScreenArea->WorldPos.Y) * PlayingScreen->Pitch) +
                  (X_Index + NextBlockScreen->ScreenArea->WorldPos.X)) = ' ';

            }                
        }
    }

    for(uint32 Y_Index = 0; Y_Index < ScoreScreen->ScreenArea->Height; Y_Index++)
    {
        for(uint32 X_Index = 0; X_Index < ScoreScreen->ScreenArea->Width; X_Index++)
        {
            if(IsThisPositionFilled(ScoreScreen->ScreenArea, X_Index, Y_Index))
            {
                *(((uint8 *)Buffer->ScreenBit) +
                  ((ScoreScreen->ScreenArea->WorldPos.Y + Y_Index) *
                   ScoreScreen->Pitch) +
                  (ScoreScreen->ScreenArea->WorldPos.X + X_Index))
                    =
                    *(ScoreScreen->ScreenArea->Pixel +
                      GetPositionOnScreenArea(ScoreScreen->ScreenArea,
                                              X_Index, Y_Index));
            }
            else
            {
                *(((uint8 *)Buffer->ScreenBit) +
                  ((ScoreScreen->ScreenPos.Y + ScoreScreen->ScreenArea->WorldPos.Y + Y_Index) *
                   ScoreScreen->Pitch) +
                  (ScoreScreen->ScreenPos.X + ScoreScreen->ScreenArea->WorldPos.X + X_Index)) = ' ';
            }
            
        }
    }

    for(int32 i = 0; i < ArrayCount(Player->Block->Pixel); i++)
    {
        *(((uint8 *)Buffer->ScreenBit) +
          ((Player->Block->Pixel[i].Y) *
           PlayingScreen->Pitch
           ) +
          (Player->Block->Pixel[i].X)) = '#';
    }

    Memory->DEBUGOutputMessage("Player X: ", (real32)Player->Position->X);
    Memory->DEBUGOutputMessage("Player Y: ", (real32)Player->Position->Y);

    Memory->DEBUGOutputMessage("BlockCenter X: ", (real32)Player->Block->Pixel[0].X);
    Memory->DEBUGOutputMessage("BlockCenter Y: ", (real32)Player->Block->Pixel[0].Y);
}

