// Simple citro2d untextured shape example
#include <citro2d.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

#include "snake.h"

//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    //---------------------------------------------------------------------------------
    // Init libs
    gfxInitDefault();
    //gfxSetWide(true);
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    consoleInit(GFX_BOTTOM, NULL);
    
    // Create screens
    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    
    u32 clear_colour = C2D_Color32(0x00, 0x00, 0x00, 0xFF);
    
    GameState game_state = GameStateInit();
    
    // Main loop
    while (aptMainLoop())
    {
        hidScanInput();
        
        // Respond to user input
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            break; // break in order to return to hbmenu
        printf("\x1b[1;1HSnake for 3DS by abiab");
        printf("\x1b[3;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
        printf("\x1b[4;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
        printf("\x1b[5;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);
        
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, clear_colour);
        C2D_SceneBegin(top);
        
        GameStep(&game_state);
        
        C3D_FrameEnd(0);
    }
    
    // Deinit libs
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
