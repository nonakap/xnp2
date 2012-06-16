#include	"compiler.h"
#include	"np2.h"
#include	"macalert.h"
#include	"taskmng.h"


void taskmng_exit(void) {

    if (QuitWarningDialogProc()) {
        np2running = 0;
    }
}

