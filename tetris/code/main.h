#if !defined(MAIN_H)

struct CMD_window_buffer
{
    void* WindowBit;
    int32 Width;
    int32 Height;
    int32 Pitch;
    int32 BytesPerPixel;
};

struct CMD_game_code
{
    HMODULE GameCodeDLL;
    FILETIME DLLLastWriteTime;
    game_update_and_render *UpdateAndRender;

    bool32 IsValid;
};

//NOTE():the name of "MAX_PATH" is misleading.
#define WIN32_STATE_FILE_NAME_COUNT MAX_PATH

struct CMD_replay_buffer
{
    HANDLE FileHandle;
    HANDLE MemoryMap;
    char FileName[WIN32_STATE_FILE_NAME_COUNT];
    void *MemoryBlock;
};

struct CMD_state
{
    uint64 TotalGameMemorySize;
    void* GameMemoryBlock;
    CMD_replay_buffer ReplayBuffers[4];
    
    HANDLE RecordingHandle;
    int InputRecordingIndex;
    
    HANDLE PlayBackHandle;
    int InputPlayingIndex;

    char EXEFileName[WIN32_STATE_FILE_NAME_COUNT];
    char *OnePastLastEXEFileNameSlash; 
};

struct CMD_button_bit
{
    uint8 ButtonBit;
    int PressedKeyCode;

};
#define MAIN_H
#endif
