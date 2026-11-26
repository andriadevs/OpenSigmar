#ifdef SIGMAR_TI
#include "Platform_TI.h"
#include "Sigmar.h"

#include <ti/screen.h>
#include <ti/getcsc.h>
#include <keypadc.h>
#include <ti/sprintf.h>
#include <graphx.h>
#include <stdlib.h>

static Sigmar *sigmar;
static int number;

int sigmar_Main() {
    gfx_Begin();
    gfx_ZeroScreen();

    bool running = true;
    //os_DisableCursor();
    int counter = 0;
    number = 0;

    

    sigmar_Init(&sigmar);

    sigmar_Bind(sigmar, EVENT_BOARD_RESET, sigmar_OnBoardReset);

    /* Clear the homescreen */
    //os_ClrHome();

    /* Print a string */
    //os_PutStrFull("Hello, World.");

    uint8_t key;
    char buf[27];
    char buf2[3];

    /* Waits for a key */
    while (running == true) {
        key = os_GetCSC();
        if (key == sk_Clear) {
            running = false;
        } else if (key == sk_Enter) {
            /*os_SetCursorPos(3, 0);
            os_PutStrFull("WOWEE ZOWIE!");

            os_SetCursorPos(8, 0);
            os_PutStrFull("__________________________");
            os_SetCursorPos(9, 0);
            boot_snprintf(buf, 26, "Presses: %i", number);
            os_PutStrFull(buf);
            counter++;*/
            sigmar_Reset(sigmar);
            MarbleType type;
            
            for (int y = 0; y < 11; y++) {
                os_SetCursorPos(y, 0);
                for (int x = 0; x < 11; x++) {
                    type = sigmar->board[y * 11 + x].type;

                    switch (type) {
                        case MARBLE_FIRE:
                            gfx_SetColor(gfx_red);
                        case MARBLE_AIR:
                            gfx_SetColor(gfx_yellow);
                        case MARBLE_EARTH:
                            gfx_SetColor(gfx_green);
                        case MARBLE_WATER:
                            gfx_SetColor(gfx_blue);
                        default:
                            gfx_SetColor(gfx_white);
                    }
                    ///gfx_SetColor(gfx_red);
                    //boot_snprintf(buf2, 2, "%i",  % 10);
                    //buf[x] = buf2[0];
                    if (type != MARBLE_INVALID) {
                        //gfx_SetColor(gfx_white);
                        

                        gfx_FillRectangle(50 + x * 16 - (y * 8), 30 + y * 16, 16, 16);
                    }
                }
                //buf[11] = '\0';
                //os_PutStrFull(buf);
            }
        }
    }

    gfx_End();

    /* Return 0 for success */
    return 0;
}

void sigmar_OnBoardReset() {
    number++;
}
#endif //SIGMAR_TI