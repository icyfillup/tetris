#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include "tetris.h"
#include "win32_tetris.h"

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

// TODO(casey): This is a global for now.
global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;
global_variable int64 GlobalPerfCountFrequency;

win32_window_dimension
Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    
    return(Result);
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
    // TODO(casey): Bulletproof this.
    // Maybe don't free first, free after, then free first if that fails.
    
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }
    
    Buffer->Width = Width;
    Buffer->Height = Height;
    
    Buffer->BytesPerPixel = 4;
    
    // NOTE(casey): When the biHeight field is negative, this is the clue to
    // Windows to treat this bitmap as top-down, not bottom-up, meaning that
    // the first three bytes of the image are the color for the top left pixel
    // in the bitmap, not the bottom left!
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
    
    // NOTE(casey): Thank you to Chris Hecker of Spy Party fame
    // for clarifying the deal with StretchDIBits and BitBlt!
    // No more DC for us.
    int BitmapMemorySize = (Buffer->Width*Buffer->Height) * Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    
    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;
    
    // TODO(casey): Probably clear this to black    
}

global_variable GLuint GlobalBlitTextureHandle;

internal void
Win32DisplayBufferInWindow(HDC DeviceContext,
                           int WindowWidth, int WindowHeight,
                           win32_offscreen_buffer Buffer)
{
#if 0
    // TODO(casey): Aspect ratio correction
    // TODO(casey): Play with stretch modes
    StretchDIBits(DeviceContext,
                  /*
                  X, Y, Width, Height,
                  X, Y, Width, Height,
                  */
                  0, 0, WindowWidth, WindowHeight,
                  0, 0, Buffer.Width, Buffer.Height,
                  Buffer.Memory,
                  &Buffer.Info,
                  DIB_RGB_COLORS, SRCCOPY);
#else
    
    glViewport(0, 0, WindowWidth, WindowHeight);

    glBindTexture(GL_TEXTURE_2D, GlobalBlitTextureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, 
                 GL_RGBA8, 
                 Buffer.Width, 
                 Buffer.Height, 0, 
                 GL_BGRA_EXT, 
                 GL_UNSIGNED_BYTE, 
                 Buffer.Memory);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glEnable(GL_TEXTURE_2D);
    
    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glMatrixMode(GL_PROJECTION);
    real32 a = 2.0f / (real32)Buffer.Width;
    real32 b = 2.0f / (real32)Buffer.Height;
    
    real32 Proj[] = 
    {
        a,  0,  0,  0,
        0,  b,  0,  0,
        0,  0,  0,  0,
        -1, -1,  0,  1,
    };
    
    glLoadMatrixf(Proj);
    
    glBegin(GL_TRIANGLES);
    
    real32 P = 0.9f;
    
    //lower
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0, 0);
    
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f((real32)Buffer.Width, 0);
    
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f((real32)Buffer.Width, (real32)Buffer.Height);
    
    //upper
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0, 0);
    
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f((real32)Buffer.Width, (real32)Buffer.Height);
    
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0, (real32)Buffer.Height);
    
    glEnd();
    
    SwapBuffers(DeviceContext);
    
#endif
}

internal void Win32InitOpenGL(HWND Window)
{
    HDC WindowDC = GetDC(Window);
    
    PIXELFORMATDESCRIPTOR DesiredPixelFormat = {0};
    DesiredPixelFormat.nSize = sizeof(DesiredPixelFormat);
    DesiredPixelFormat.nVersion = 1;
    DesiredPixelFormat.dwFlags = 
        PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
    DesiredPixelFormat.cColorBits = 32;
    DesiredPixelFormat.cAlphaBits = 8;
    DesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
    
    int SuggestedPixelFormatIndex = ChoosePixelFormat(WindowDC, &DesiredPixelFormat);
    PIXELFORMATDESCRIPTOR SuggestedPixelFormat = {0};
    DescribePixelFormat(WindowDC, SuggestedPixelFormatIndex, 
                        sizeof(SuggestedPixelFormat), &SuggestedPixelFormat);
    SetPixelFormat(WindowDC, SuggestedPixelFormatIndex, &SuggestedPixelFormat);
    
    HGLRC OpenGLRC = wglCreateContext(WindowDC);
    if(wglMakeCurrent(WindowDC, OpenGLRC))
    {
        glGenTextures(1, &GlobalBlitTextureHandle);
    }
    else
    {
        Assert("can't go here");
    }
    
    ReleaseDC(Window, WindowDC);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                        UINT Message,
                        WPARAM WParam,
                        LPARAM LParam)
{       
    LRESULT Result = 0;
    
    switch(Message)
    {
        case WM_CLOSE:
        {
            // TODO(casey): Handle this with a message to the user?
            GlobalRunning = false;
        } break;
        
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;
        
        case WM_DESTROY:
        {
            // TODO(casey): Handle this as an error - recreate window?
            GlobalRunning = false;
        } break;
        
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            Assert(!"Keyboard input came through this function path");
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height,
                                       GlobalBackbuffer);
            EndPaint(Window, &Paint);
        } break;
        
        default:
        {
            //            OutputDebugStringA("default\n");
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    
    return(Result);
}

internal void Win32ProcessKeyboardInput(game_button_state *NewButtonState, bool32 IsDown)
{
    if(NewButtonState->EndedDown != IsDown)
    {
        NewButtonState->EndedDown = IsDown;
        NewButtonState->HalfTransitionCount++;
    }
}

internal void Win32KeybroadProcessMessage(game_controller *KeyboardController)
{
    MSG Message;
    
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        
        switch(Message.message)
        {
            case WM_QUIT:
            {
                GlobalRunning = false;
            }break;
            
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                uint32 VKCode = (uint32)Message.wParam;
                bool32 WasDown = (Message.lParam & (1 << 30)) != 0;
                bool32 IsDown = (Message.lParam & (1 << 31)) == 0;
                
                if(WasDown != IsDown)
                {
                    if(VKCode == 'W')
                    {
                        //OutputDebugStringA("W\n");
                        Win32ProcessKeyboardInput(&KeyboardController->Up, IsDown);
                    }
                    else if(VKCode == 'A')
                    {
                        Win32ProcessKeyboardInput(&KeyboardController->Left, IsDown);
                        //OutputDebugStringA("A\n");
                    }
                    else if(VKCode == 'S')
                    {
                        Win32ProcessKeyboardInput(&KeyboardController->Down, IsDown);
                        //OutputDebugStringA("S\n");
                    }
                    else if(VKCode == 'D')
                    {
                        Win32ProcessKeyboardInput(&KeyboardController->Right, IsDown);
                        //OutputDebugStringA("D\n");
                    }
                    else if(VKCode == VK_DOWN)
                    {
                        Win32ProcessKeyboardInput(&KeyboardController->Down, IsDown);
                        //OutputDebugStringA("Down\n");
                    }
                    else if(VKCode == VK_UP)
                    {
                        Win32ProcessKeyboardInput(&KeyboardController->Up, IsDown);
                        //OutputDebugStringA("Up\n");
                    }
                    else if(VKCode == VK_RIGHT)
                    {
                        Win32ProcessKeyboardInput(&KeyboardController->Right, IsDown);
                        //OutputDebugStringA("Right\n");
                    }
                    else if(VKCode == VK_LEFT)
                    {
                        //OutputDebugStringA("Left\n");
                        Win32ProcessKeyboardInput(&KeyboardController->Left, IsDown);
                    }
                    else if(VKCode == VK_SPACE)
                    {
                        Win32ProcessKeyboardInput(&KeyboardController->Button1, IsDown);
                        //OutputDebugStringA("Space\n");
                    }
                }
            } break;
            
            default:
            {
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            }break;
        }
        
    }
    
}

internal LARGE_INTEGER Win32GetWallClock()
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return Result;
}

internal real32 Win32GetSecondElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    real32 Result  = ((real32)(End.QuadPart - Start.QuadPart)) / (real32)GlobalPerfCountFrequency;
    
    return Result;
}

internal win32_game_code Win32LoadGameCode()
{
    win32_game_code Result = {};
    
    Result.GameCodeDLL = LoadLibraryA("../../build/tetris.dll");
    
    if(Result.GameCodeDLL)
    {
        Result.UpdateAndRender = 
            (game_update_and_render *)GetProcAddress(Result.GameCodeDLL, "GameUpdateAndRender");
        
        Result.IsValid = Result.GameCodeDLL != 0;
    }
    
    if(!Result.IsValid)
    {
        Result.UpdateAndRender = GameUpdateAndRenderStub;
    }
    
    return Result;
}

void
RenderWeirdGradient(game_screen_buffer *Buffer, int BlueOffset, int GreenOffset)
{
    // TODO(casey): Let's see what the optimizer does
    
    uint8 *Row = (uint8 *)Buffer->ScreenBit;    
    for(int Y = 0; Y < Buffer->Height; ++Y)
    {
        uint32 *Pixel = (uint32 *)Row;
        for(int X = 0;
            X < Buffer->Width;
            ++X)
        {
            uint8 Blue = (X + BlueOffset);
            uint8 Green = (Y + GreenOffset);
            
            *Pixel++ = ((Green << 8) | Blue);
        }
        
        Row += Buffer->Pitch;
        if(Y > 400)
            break;
    }
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    GlobalPerfCountFrequency = PerfCountFrequencyResult.QuadPart;
    
    UINT DesiredSchedulerMS = 1;
    bool32 SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);
    
    int32 MonitorRefreshHz = 60;
    int32 GameUpdateHz = MonitorRefreshHz / 2;
    real32 TargetSecondsPerFrame = 1.0f / (real32)GameUpdateHz;
    
    WNDCLASS WindowClass = {};
    
    Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);
    
    WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    //    WindowClass.hIcon;
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";
    
    if(RegisterClassA(&WindowClass))
    {
        HWND Window =
            CreateWindowExA(
            0,
            WindowClass.lpszClassName,
            "Handmade Hero",
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            Instance,
            0);
        if(Window)
        {
            Win32InitOpenGL(Window);
            GlobalRunning = true;
            
            // NOTE(casey): Since we specified CS_OWNDC, we can just
            // get one device context and use it forever because we
            // are not sharing it with anyone.
            HDC DeviceContext = GetDC(Window);
            
#if TETRIS_INTERNAL
            LPVOID BaseAddress = (LPVOID) Terabytes(2);
#else
            LPVoid BaseAddress = 0;
#endif
            
            game_memory GameMemory = {};
            GameMemory.PermanentStorageSize = Megabytes(64);
            GameMemory.TransientStorageSize = Gigabytes(1);
            
            uint64 TotalGameMemorySize = GameMemory.TransientStorageSize +GameMemory.PermanentStorageSize;
            GameMemory.PermanentStorage = VirtualAlloc(BaseAddress, TotalGameMemorySize,
                             MEM_COMMIT|MEM_RESERVE,
                             PAGE_READWRITE);
            GameMemory.TransientStorage = (int8 *)GameMemory.PermanentStorage + GameMemory.PermanentStorageSize;
            
            GameMemory.DEBUGPlatformFreeFileMemory = DEBUGPlatformFreeFileMemory;
            GameMemory.DEBUGPlatformReadFromFile = DEBUGPlatformReadFromFile;
            GameMemory.DEBUGPlatformWriteEntireFile = DEBUGPlatformWriteEntireFile;
            GameMemory.DEBUGOutputMessage = DEBUGOutputMessage;
            
            if(GameMemory.PermanentStorage && GameMemory.TransientStorage)
            {
                int XOffset = 0;
                int YOffset = 0;
                
                game_controller GameInput[2] = {};
                game_controller *NewInput = &GameInput[0];
                game_controller *OldInput = &GameInput[1];
                
                thread_context Thread = {};
                
                win32_game_code Game = Win32LoadGameCode();
                
                LARGE_INTEGER LastCounter = Win32GetWallClock();
                int64 LastCycleCount = __rdtsc();
                while(GlobalRunning)
                {
                    NewInput->MSPerFrame = TargetSecondsPerFrame;
                    *NewInput = {};
                    
                    for(int ButtonIndex = 0; ButtonIndex < ArrayCount(NewInput->Button); ButtonIndex++)
                    {
                        NewInput->Button[ButtonIndex].EndedDown = OldInput->Button[ButtonIndex].EndedDown;
                    }
                    
                    Win32KeybroadProcessMessage(NewInput);
                    
                    #if 0 //Keyboard input check
                    if(NewInput->Left.EndedDown)
                    {
                        XOffset -= 1;
                    }
                    else if(NewInput->Right.EndedDown)
                    {
                        XOffset += 1;
                    }
                    else if(NewInput->Up.EndedDown)
                    {
                         YOffset += 1;
                    }
                    else if(NewInput->Down.EndedDown)
                    {
                        YOffset -= 1;
                    }
                    #endif
                    
                    game_screen_buffer Buffer = {};
                    Buffer.ScreenBit = GlobalBackbuffer.Memory;
                    Buffer.Width = GlobalBackbuffer.Width;
                    Buffer.Height = GlobalBackbuffer.Height;
                    Buffer.Pitch = GlobalBackbuffer.Pitch;
                    Buffer.BytesPerPixel = GlobalBackbuffer.BytesPerPixel;
                    
                    Game.UpdateAndRender(&Thread, &GameMemory, &Buffer, NewInput);
                    
                    real32 WorkSecondElapsedForFrame = Win32GetSecondElapsed(LastCounter, Win32GetWallClock());
                    if(WorkSecondElapsedForFrame < TargetSecondsPerFrame)
                    {
                        if(SleepIsGranular)
                        {
                            DWORD SleepMS = (DWORD)(1000.0f * (TargetSecondsPerFrame - WorkSecondElapsedForFrame));
                            
                            if(SleepMS > 0)
                            {
                                Sleep(SleepMS);
                            }
                        }
                        
                        WorkSecondElapsedForFrame = Win32GetSecondElapsed(LastCounter, Win32GetWallClock());
                        
                        while(WorkSecondElapsedForFrame < TargetSecondsPerFrame)
                        {
                            WorkSecondElapsedForFrame = Win32GetSecondElapsed(LastCounter, 
                                                                              Win32GetWallClock());
                        }
                    }
                    
                    win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                    Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height,
                                               GlobalBackbuffer);
                    
                    /*
                    ++XOffset;
                    YOffset += 2;
                    */
                    game_controller *temp = NewInput;
                    NewInput = OldInput;
                    OldInput = temp;
                    
                    int64 EndCycleCount = __rdtsc();
                    
                    LARGE_INTEGER EndCounter = Win32GetWallClock();
                    
                    int64 CycleElapsed = EndCycleCount - LastCycleCount;
                    int64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
                    
                    real32 MSPerFrame = 1000.0f * Win32GetSecondElapsed(LastCounter, EndCounter);
                    real32 FPS = (real32)GlobalPerfCountFrequency / (real32)CounterElapsed;
                    real32 MCPF = (real32)((real32)CycleElapsed / (1000.0f * 1000.0f));
                    
                    #if 0
                    char Buffer[256];
                    sprintf(Buffer, "MSPerFrame: %f   |   FPS: %f   |   MCPF: %f\n", MSPerFrame, FPS, MCPF);
                    OutputDebugStringA(Buffer);
                    #endif
                    LastCycleCount = EndCycleCount;
                    LastCounter = EndCounter;
                    
                }
            }
            }
            
        else
        {
            // TODO(casey): Logging
        }
    }
    else
    {
        // TODO(casey): Logging
    }
    
    return(0);
}