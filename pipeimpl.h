/*
    Pipe implementation for chuniio
    
    Credits:
    somewhatlurker, skogaby
*/

#pragma once

#include <windows.h>

#include "leddata.h"

HRESULT led_pipe_init();
void led_pipe_update(struct _chuni_led_data_buf_t* data);
