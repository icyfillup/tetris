#include <iostream>

#define CMD_BUTTON_UP 'w'
#define CMD_BUTTON_LEFT 'a'
#define CMD_BUTTON_DOWN 's'
#define CMD_BUTTON_RIGHT 'd'
#define CMD_BUTTON_BUTTON1 'f'

#define NO_VALID_INPUT_KEY 0 
#define BIT_BUTTON_UP 0x01 
#define BIT_BUTTON_LEFT 0x02
#define BIT_BUTTON_DOWN 0x04
#define BIT_BUTTON_RIGHT 0x08
#define BIT_BUTTON_BUTTON1 0x10

#include "tetris.h"

#include <conio.h>
#include <stdlib.h>
#include <windows.h>

#include "main.h"
DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory)
{
    if(Memory)
    {
        VirtualFree(Memory, 0, MEM_RELEASE);
    }
}

DEBUG_PLATFORM_READ_FROM_FILE(DEBUGPlatformReadFromFile)
{
    debug_read_file_result Result = {};

    HANDLE FileHandle =  CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ,
                                    0, OPEN_EXISTING, 0, 0);
    if(FileHandle)
    {
        LARGE_INTEGER FileSize;
        if(GetFileSizeEx(FileHandle, &FileSize))
        {
            DWORD FileSize32 = SafeTruncateUInt64(FileSize.QuadPart);
            Result.Content = VirtualAlloc(0, FileSize32,
                                          MEM_COMMIT|MEM_RESERVE,
                                          PAGE_READWRITE);
            if(Result.Content)
            {
                DWORD BytesRead;
                if(ReadFile(FileHandle, Result.Content, FileSize32,
                            &BytesRead, 0) && (FileSize32 == BytesRead))
                {
                    //NOTE: File Read successfully
                    Result.ContentSize = FileSize32;
                }
                else
                {
                    DEBUGPlatformFreeFileMemory(Thread, Result.Content);
                    Result.Content = 0;
                }
            }
            else
            {

            }
        }
        else
        {

        }
        CloseHandle(FileHandle);
    }
    else
    {
        
    }
    
    return Result;
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile)
{
    bool32 Result = false;
    
    HANDLE FileHandle =  CreateFileA(FileName, GENERIC_WRITE, 0,
                                    0, CREATE_ALWAYS, 0, 0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD BytesWritten;
        if(WriteFile(FileHandle, Memory, MemorySize, &BytesWritten, 0))
        {
            Result = BytesWritten == MemorySize;
        }
        else
        {
                
        }       
        CloseHandle(FileHandle);
    }
    else
    {
        
    }
    return Result;
}


DEBUG_OUTPUT_MESSAGE(DEBUGOutputMessage)
{
    char Message[256];
    _snprintf_s(Message, sizeof(Message),
                "%s, %.2f\n", StringMsg, NumberMsg);
    OutputDebugStringA(Message);
}

void CatStrings(size_t SourceACount, char *SourceA,
                size_t SourceBCount, char *SourceB,
                size_t DestCount, char *Dest)
{
    for(int Index = 0;
        Index < SourceACount;
        Index++)
    {
        *Dest++ = *SourceA++;
    }

    for(int Index = 0;
        Index < SourceBCount;
        Index++)
    {
        *Dest++ = *SourceB++;
    }

    *Dest++ = 0;
}

internal void Win32GetEXEFileName(CMD_state *State)
{
    GetModuleFileName(0, State->EXEFileName, sizeof(State->EXEFileName));
    State->OnePastLastEXEFileNameSlash = State->EXEFileName;
    for(char *Scan = State->EXEFileName; *Scan; ++Scan)
    {
        if(*Scan == '\\')
        {
            State->OnePastLastEXEFileNameSlash = Scan + 1;
        }
    }
}

internal int StringLength(char* String)
{
    int Count = 0;

    while(*String++)
    {
        ++Count;
    }
    return Count;
}

internal void CMDBuildEXEPathFileName(CMD_state* State, char *FileName,
                                      int DestCount, char *Dest)
{
    CatStrings(State->OnePastLastEXEFileNameSlash - State->EXEFileName, State->EXEFileName,
               StringLength(FileName), FileName,
               sizeof(DestCount), Dest);   
}

//TODO(): research what is the "Back" in "GlobalBackBuffer"
global_variable CMD_window_buffer GlobalBackBuffer;

global_variable bool32 GlobalRunning;
global_variable int64 GlobalPerfCountFrequency;

internal void CMDRender(CMD_window_buffer *Buffer)
{
    int32 pitch = Buffer->Width * Buffer->Height;
    for(int32 y = 0; y < Buffer->Height; y++)
    {
        pitch -= Buffer->Width;
        for(int32 x = 0; x < Buffer->Width; x++)
        {
            printf("%c", *(((int8*)Buffer->WindowBit) + pitch + x));
        }
#if TETRIS_SLOW
        printf(" %d\n", (Buffer->Height - 1) - y);
#else
        printf("\n");
#endif
    }
}

inline LARGE_INTEGER Win32GetWallClock()
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return Result;
}

inline real32 Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    real32 Result =  ((real32)(End.QuadPart - Start.QuadPart)) /
        (real32)GlobalPerfCountFrequency;
    return Result;
}

inline FILETIME CMDGetLastWriteTime(char *FileName)
{
    FILETIME LastWriteTime = {};
    WIN32_FIND_DATA FindData = {};
    
    if(GetFileAttributesEx(FileName, GetFileExInfoStandard, &FindData))
    {
        LastWriteTime = FindData.ftLastWriteTime;
    }
    
    return LastWriteTime;
}

internal CMD_game_code CMDLoadGameCode(char *SourceDLLName,
                                       char *TempDLLName)
{
    CMD_game_code Result = {};
    
    Result.DLLLastWriteTime = CMDGetLastWriteTime(SourceDLLName);
    
    CopyFile(SourceDLLName, TempDLLName, FALSE);
    Result.GameCodeDLL = LoadLibraryA(TempDLLName);
    
    if(Result.GameCodeDLL)
    {
        Result.UpdateAndRender = (game_update_and_render *)GetProcAddress(Result.GameCodeDLL, "GameUpdateAndRender");
        
        Result.IsValid = Result.UpdateAndRender && Result.GameCodeDLL; 
    }
    
    if(!Result.IsValid)
    {
        Result.UpdateAndRender = GameUpdateAndRenderStub;
    }
    
    return Result;
}

internal void CMDUnloadGameCode(CMD_game_code* GameCode)
{
    if(GameCode->GameCodeDLL)
    {
        FreeLibrary(GameCode->GameCodeDLL);
        GameCode->GameCodeDLL = 0;
    }
    
    GameCode->IsValid = false;
    GameCode->UpdateAndRender = GameUpdateAndRenderStub;
}

internal void CMDInitWindow(CMD_window_buffer *Buffer)
{
    Buffer->Width = 20;
    Buffer->Height = 20;
    Buffer->Pitch = Buffer->Width;
    uint32 BufferMemberSize = Buffer->Width * Buffer->Height;
    
    //Buffer->WindowBit = calloc(WindowSize, sizeof(int8));
    Buffer->WindowBit = VirtualAlloc(0, BufferMemberSize,
                                     MEM_COMMIT|MEM_RESERVE,
                                     PAGE_READWRITE);
    
#if TETRIS_SLOW
    for(int32 i = 0; i < Buffer->Width * Buffer->Height; ++i)
    {
        *((int8*)Buffer->WindowBit + i) = '*';
    }
#endif
}

internal void CMDGetInputFileLocation(CMD_state *State, bool InputStream, int SlotIndex, int DestCount, char *Dest)
{
    char Temp[64];
    wsprintf(Temp, "loop_edit_%d_%s.ti", SlotIndex, InputStream ? "Input" : "State");
    CMDBuildEXEPathFileName(State, Temp, DestCount, Dest);
}

internal CMD_replay_buffer* CMDGetReplayBuffer(CMD_state* State,
                                               int unsigned Index)
{
    Assert(Index < ArrayCount(State->ReplayBuffers));
    CMD_replay_buffer *Result = &State->ReplayBuffers[Index];
    return Result;
    
}

internal void CMDBeginRecordingInput(CMD_state* State,
                                     int InputRecordingIndex)
{
    CMD_replay_buffer *ReplayBuffer = CMDGetReplayBuffer(State, InputRecordingIndex);
    
    if(ReplayBuffer->MemoryBlock)
    {                                        
        State->InputRecordingIndex = InputRecordingIndex;
        char FileName[WIN32_STATE_FILE_NAME_COUNT];
        CMDGetInputFileLocation(State, true,
                                InputRecordingIndex,
                                sizeof(FileName),
                                FileName);
        State->RecordingHandle = CreateFileA(FileName, GENERIC_WRITE,
                                             0, 0, CREATE_ALWAYS, 0, 0);
        
#if 0
        LARGE_INTEGER FilePosition;
        FilePosition.QuadPart = State->TotalGameMemorySize;
        SetFilePointerEx(State->RecordingHandle, FilePosition, 0, FILE_BEGIN);
#endif
        
        CopyMemory(ReplayBuffer->MemoryBlock,
                   State->GameMemoryBlock,
                   State->TotalGameMemorySize);
        
    }
    
}

internal void CMDEndRecordingInput(CMD_state* State)
{
    State->InputRecordingIndex = 0;
    CloseHandle(State->RecordingHandle);
}

internal void CMDRecordInput(CMD_state* State, game_controller* NewInput)
{
    DWORD BytesWritten;
    WriteFile(State->RecordingHandle, NewInput,
              sizeof(*NewInput), &BytesWritten, 0);    
}


internal void CMDBeginInputPlayBack(CMD_state* State,
                                    int InputPlayingIndex)
{
    CMD_replay_buffer *ReplayBuffer = CMDGetReplayBuffer(State, InputPlayingIndex);
    
    if(ReplayBuffer->MemoryBlock)
    {
        State->InputPlayingIndex = InputPlayingIndex;        
        char FileName[WIN32_STATE_FILE_NAME_COUNT];
        CMDGetInputFileLocation(State, true,
                                State->InputPlayingIndex,
                                sizeof(FileName),
                                FileName);
        State->PlayBackHandle = CreateFileA(FileName, GENERIC_READ,
                                            0, 0, OPEN_EXISTING, 0, 0);
#if 0
        LARGE_INTEGER FilePosition;
        FilePosition.QuadPart = State->TotalGameMemorySize;
        SetFilePointerEx(State->PlayBackHandle, FilePosition, 0, FILE_BEGIN);
#endif
        CopyMemory(State->GameMemoryBlock,
                   ReplayBuffer->MemoryBlock,
                   State->TotalGameMemorySize);
        
    }
    
}

internal void CMDEndInputPlayBack(CMD_state* State)
{
    State->InputPlayingIndex = 0;
    CloseHandle(State->PlayBackHandle);
}

internal void CMDPlayBackInput(CMD_state* State,
                               game_controller* NewInput)
{
    DWORD BytesRead;
    if(ReadFile(State->PlayBackHandle, NewInput,
                sizeof(*NewInput), &BytesRead, 0))
    {
        if(BytesRead == 0)
        {
            int PlayingIndex = State->InputPlayingIndex;
            CMDEndInputPlayBack(State);
            CMDBeginInputPlayBack(State, PlayingIndex);
            ReadFile(State->PlayBackHandle, NewInput,
                     sizeof(*NewInput), &BytesRead, 0);
        }
    }
}

internal CMD_button_bit CMDKeybroadInputProcess(uint8 ButtonBit, int KeyCode)
{
    CMD_button_bit Result = {};
    
    Result.ButtonBit = ButtonBit;
    Result.PressedKeyCode = KeyCode;
    
    return Result;
}

internal void CMDKeybroadMessageInput(CMD_state *State,
                                      CMD_button_bit* NewUserButton,
                                      CMD_button_bit* OldUserButton)
{
    if(_kbhit())
    {
        int KeyCode = _getch();
        
        if(OldUserButton->PressedKeyCode != KeyCode)
        {
            switch(KeyCode)
            {
                case CMD_BUTTON_UP:
                {
                    *NewUserButton = CMDKeybroadInputProcess(BIT_BUTTON_UP,
                                                             KeyCode);
                }break;
                
                case CMD_BUTTON_LEFT:
                {
                    *NewUserButton = CMDKeybroadInputProcess(BIT_BUTTON_LEFT,
                                                             KeyCode);
                }break;
                
                case CMD_BUTTON_DOWN:
                {
                    *NewUserButton = CMDKeybroadInputProcess(BIT_BUTTON_DOWN,
                                                             KeyCode);
                }break;
                
                case CMD_BUTTON_RIGHT:
                {
                    *NewUserButton = CMDKeybroadInputProcess(BIT_BUTTON_RIGHT,
                                                             KeyCode);
                }break;
                
                case CMD_BUTTON_BUTTON1:
                {
                    *NewUserButton = CMDKeybroadInputProcess(BIT_BUTTON_BUTTON1,
                                                             KeyCode);
                }break;            
                
                
                case 'L':
                {
                    
                    OutputDebugStringA("Pressing\n");
                    *NewUserButton = CMDKeybroadInputProcess(NO_VALID_INPUT_KEY, KeyCode);
                    
                    if(State->InputPlayingIndex == 0)
                    {
                        if(State->InputRecordingIndex == 0)
                        {
                            CMDBeginRecordingInput(State, 1);
                        }
                        else
                        {
                            CMDEndRecordingInput(State);
                            CMDBeginInputPlayBack(State, 1);
                        }
                    }
                    else
                    {
                        CMDEndInputPlayBack(State);
                    }
                    
                }
                break;
                
                default:
                {
                    *NewUserButton = CMDKeybroadInputProcess(NO_VALID_INPUT_KEY, 0);
                }break;
            }
            
        }
        else
        {
            *NewUserButton = CMDKeybroadInputProcess(OldUserButton->ButtonBit, OldUserButton->PressedKeyCode);
        }
    }
    else
    {
        *NewUserButton = {};
    }
}

internal void CMDProcessButton(int32 UserButton, int32 ButtonBit,
                               game_button_state* OldState,
                               game_button_state* NewState)
{
    NewState->EndedDown = (UserButton & ButtonBit) == ButtonBit;
    NewState->HalfTransitionCount = (OldState->EndedDown !=
                                     NewState->EndedDown) ? 1 : 0;
}

int main()
{
    /*
        system("PAUSE");
    */
    CMD_state CMDState = {};
    Win32GetEXEFileName(&CMDState);
    
    char SourceGameCodeDLLFullPath[WIN32_STATE_FILE_NAME_COUNT];
    CMDBuildEXEPathFileName(&CMDState, "tetris.dll",
                            sizeof(SourceGameCodeDLLFullPath), SourceGameCodeDLLFullPath);
    
    char TempGameCodeDLLFullPath[WIN32_STATE_FILE_NAME_COUNT];
    CMDBuildEXEPathFileName(&CMDState, "tetris_temp.dll",
                            sizeof(TempGameCodeDLLFullPath), TempGameCodeDLLFullPath);
    
    LARGE_INTEGER PerfCountFrequenceResult;
    QueryPerformanceFrequency(&PerfCountFrequenceResult);
    GlobalPerfCountFrequency = PerfCountFrequenceResult.QuadPart;
    
    UINT DesiredSchedulerMS = 1;
    bool32 SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);
    
    int MonitorRefreshHz = 60;
    int GameUpdateHz = MonitorRefreshHz / 2;
    real32 TargetSecondsPerFrame = 1.0f / (real32)GameUpdateHz;
    
    CMDInitWindow(&GlobalBackBuffer);
    
    if(GlobalBackBuffer.WindowBit)
    {
        GlobalRunning = true;
        
#if TETRIS_INTERNAL
        LPVOID BaseAddress = (LPVOID) Terabytes(2);
#else
        LPVoid BaseAddress = 0;
#endif
        
        game_memory GameMemory = {};
        GameMemory.PermanentStorageSize = Megabytes(64);
        GameMemory.TransientStorageSize = Gigabytes(1);
        
        GameMemory.DEBUGPlatformFreeFileMemory = DEBUGPlatformFreeFileMemory;
        GameMemory.DEBUGPlatformReadFromFile = DEBUGPlatformReadFromFile;
        GameMemory.DEBUGPlatformWriteEntireFile = DEBUGPlatformWriteEntireFile;
        GameMemory.DEBUGOutputMessage = DEBUGOutputMessage;
        
        CMDState.TotalGameMemorySize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;
        
        CMDState.GameMemoryBlock = VirtualAlloc(BaseAddress, (size_t)CMDState.TotalGameMemorySize,
                                                MEM_COMMIT|MEM_RESERVE,
                                                PAGE_READWRITE);
        GameMemory.TransientStorage = (int8 *)GameMemory.PermanentStorage + GameMemory.PermanentStorageSize;
        GameMemory.PermanentStorage = CMDState.GameMemoryBlock;
        
        for(int ReplayIndex = 0; ReplayIndex < ArrayCount(CMDState.ReplayBuffers); ReplayIndex++)
        {
            CMD_replay_buffer *ReplayBuffer = &CMDState.ReplayBuffers[ReplayIndex];
            
            CMDGetInputFileLocation(&CMDState, false, ReplayIndex,
                                    sizeof(ReplayBuffer->FileName),
                                    ReplayBuffer->FileName);
            
            ReplayBuffer->FileHandle =
                CreateFileA(ReplayBuffer->FileName, GENERIC_READ|GENERIC_WRITE,
                            0, 0, CREATE_ALWAYS, 0, 0);
            
            LARGE_INTEGER MaxSize;
            MaxSize.QuadPart = CMDState.TotalGameMemorySize;
            ReplayBuffer->MemoryMap =
                CreateFileMapping(ReplayBuffer->FileHandle, 0, PAGE_READWRITE,
                                  MaxSize.HighPart, MaxSize.LowPart, 0);
            
            ReplayBuffer->MemoryBlock =
                MapViewOfFile(ReplayBuffer->MemoryMap,
                              FILE_MAP_ALL_ACCESS, 0, 0,
                              CMDState.TotalGameMemorySize);
            
            if(ReplayBuffer->MemoryBlock)
            {
                
            }
            else
            {
                //TODO():Diagnostic
            }
        }
        
        if(GameMemory.PermanentStorage && GameMemory.TransientStorage)
        {
            game_controller GameInput[2] = {};
            game_controller *OldInput = &GameInput[0];
            game_controller *NewInput = &GameInput[1];
            
            LARGE_INTEGER LastCounter = Win32GetWallClock();
            
            CMD_button_bit UserButton[2] = {};
            CMD_button_bit *OldUserButton = &UserButton[0];
            CMD_button_bit *NewUserButton = &UserButton[1];
            
            CMD_game_code Game = CMDLoadGameCode(SourceGameCodeDLLFullPath,
                                                 TempGameCodeDLLFullPath);
            
            uint64 LastCycleCount = __rdtsc();
            while(GlobalRunning)
            {
                NewInput->MSPerFrame = TargetSecondsPerFrame;
                
                FILETIME NewDLLWriteTime =
                    CMDGetLastWriteTime(SourceGameCodeDLLFullPath);
                if(CompareFileTime(&Game.DLLLastWriteTime, &NewDLLWriteTime) != 0)
                {
                    CMDUnloadGameCode(&Game);
                    Game = CMDLoadGameCode(SourceGameCodeDLLFullPath,
                                           TempGameCodeDLLFullPath);
                }
                
                
                CMDKeybroadMessageInput(&CMDState, NewUserButton, OldUserButton);
                
                CMDProcessButton(NewUserButton->ButtonBit, BIT_BUTTON_UP,
                                 &OldInput->Up, &NewInput->Up);
                CMDProcessButton(NewUserButton->ButtonBit, BIT_BUTTON_LEFT,
                                 &OldInput->Left, &NewInput->Left);
                CMDProcessButton(NewUserButton->ButtonBit, BIT_BUTTON_DOWN,
                                 &OldInput->Down, &NewInput->Down);
                CMDProcessButton(NewUserButton->ButtonBit, BIT_BUTTON_RIGHT,
                                 &OldInput->Right, &NewInput->Right);
                CMDProcessButton(NewUserButton->ButtonBit, BIT_BUTTON_BUTTON1,
                                 &OldInput->Right, &NewInput->Button1);
                
                thread_context Thread = {};
                
                game_screen_buffer Buffer = {};
                Buffer.ScreenBit = GlobalBackBuffer.WindowBit;
                Buffer.Width = GlobalBackBuffer.Width;
                Buffer.Height = GlobalBackBuffer.Height;
                Buffer.Pitch = GlobalBackBuffer.Pitch;
                
                if(CMDState.InputRecordingIndex)
                {
                    CMDRecordInput(&CMDState, NewInput);
                }
                
                if(CMDState.InputPlayingIndex)
                {
                    CMDPlayBackInput(&CMDState, NewInput);
                }
                
                Game.UpdateAndRender(&Thread, &GameMemory, &Buffer, NewInput);
#if 0
                for(int i = 0; i < (GlobalBackBuffer.Width * GlobalBackBuffer.Height) + 50; i++)
                {
                    printf("%c\n", *(((int8*)GlobalBackBuffer.WindowBit) + i));
                }
#endif
                
#if 1
                LARGE_INTEGER WorkCounter = Win32GetWallClock();
                real32 WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, WorkCounter);
                
                real32 SecondElapsedForFrame = WorkSecondsElapsed;
                if(SecondElapsedForFrame < TargetSecondsPerFrame)
                {
                    if(SleepIsGranular)
                    {
                        DWORD SleepMS = (DWORD)(1000.0f * (TargetSecondsPerFrame -
                                                           SecondElapsedForFrame));
                        if(SleepMS > 0)
                        {
                            Sleep(SleepMS);
                        }
                    }
                    
                    real32 TestSecondElapsedForFrame = Win32GetSecondsElapsed
                        (LastCounter, Win32GetWallClock());
                    //            Assert(TestSecondElapsedForFrame < TargetSecondsPerFrame);
                    while(SecondElapsedForFrame < TargetSecondsPerFrame)
                    {
                        SecondElapsedForFrame = Win32GetSecondsElapsed(LastCounter, Win32GetWallClock());
                    }
                }
#endif
                
                CMDRender(&GlobalBackBuffer);
                
                LARGE_INTEGER EndCounter = Win32GetWallClock();
                real32 MSPerFrame = 1000.0f * Win32GetSecondsElapsed(LastCounter, EndCounter);
                LastCounter = EndCounter;
                
                uint64 EndCycleCount = __rdtsc();
                uint64 CyclesElapsed = EndCycleCount - LastCycleCount;
                LastCycleCount = EndCycleCount;
                
                real64 FPS = 0.0f;
                real64 MCPF = ((real64)CyclesElapsed / (1000.0f * 1000.0f));
                
#if 0
                char FPSBuffer[256];
                _snprintf_s(FPSBuffer, sizeof(FPSBuffer),
                            "%.02f ms/f,  %.02f f/s,  %.02f mc/f\n", MSPerFrame, FPS, MCPF);
                OutputDebugStringA(FPSBuffer);
#endif
                
                CMD_button_bit *TempUserButton = NewUserButton;
                NewUserButton = OldUserButton;
                OldUserButton = TempUserButton;
                
                game_controller *TempGameController = OldInput;
                OldInput = NewInput;
                NewInput = TempGameController;
#if 0
                Sleep(500);
#endif
                system("cls");
            }
        }
        
    }
    return 0;
    
}
