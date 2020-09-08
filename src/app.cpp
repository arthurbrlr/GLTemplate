#include "Core/window.h"

int main()
{
    bool success = window_create();
    if (!success) return -1;

    while (!window_sould_close())
    {
        window_update();   
    }

    window_free();
    return 0;
}

