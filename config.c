#include <windows.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "config.h"

static const int chuni_io_default_cells[] = {
    'L', 'L', 'L', 'L',
    'K', 'K', 'K', 'K',
    'J', 'J', 'J', 'J',
    'H', 'H', 'H', 'H',
    'G', 'G', 'G', 'G',
    'F', 'F', 'F', 'F',
    'D', 'D', 'D', 'D',
    'S', 'S', 'S', 'S',
};

static const int chuni_io_default_ir[] = {
    '/', '.', '\'', ';', ']', '['
};

void chuni_io_config_load(
        struct chuni_io_config *cfg,
        const wchar_t *filename)
{
    wchar_t key[16];
    int i;
    wchar_t port_input[6];

    assert(cfg != NULL);
    assert(filename != NULL);

    // Technically it's io4 but leave this for compatibility with old configs.
    cfg->vk_test = GetPrivateProfileIntW(L"io3", L"test", VK_F1, filename);
    cfg->vk_service = GetPrivateProfileIntW(L"io3", L"service", VK_F2, filename);
    cfg->vk_coin = GetPrivateProfileIntW(L"io3", L"coin", VK_F3, filename);
    cfg->vk_ir_emu = GetPrivateProfileIntW(L"io3", L"ir", VK_SPACE, filename);

    for (i = 0 ; i < 6 ; i++) {
        swprintf_s(key, _countof(key), L"ir%i", i + 1);
        cfg->vk_ir[i] = GetPrivateProfileIntW(
                L"ir",
                key,
                chuni_io_default_ir[i],
                filename);
    }

    for (i = 0 ; i < 32 ; i++) {
        swprintf_s(key, _countof(key), L"cell%i", i + 1);
        cfg->vk_cell[i] = GetPrivateProfileIntW(
                L"slider",
                key,
                chuni_io_default_cells[i],
                filename);
    }

    cfg->cab_led_output_pipe = GetPrivateProfileIntW(L"led", L"cabLedOutputPipe", 1, filename);
    cfg->cab_led_output_serial = GetPrivateProfileIntW(L"led", L"cabLedOutputSerial", 0, filename);
    
    cfg->controller_led_output_pipe = GetPrivateProfileIntW(L"led", L"controllerLedOutputPipe", 1, filename);
    cfg->controller_led_output_serial = GetPrivateProfileIntW(L"led", L"controllerLedOutputSerial", 0, filename);

    cfg->controller_led_output_openithm = GetPrivateProfileIntW(L"led", L"controllerLedOutputOpeNITHM", 0, filename);

    cfg->led_serial_baud = GetPrivateProfileIntW(L"led", L"serialBaud", 921600, filename);

    GetPrivateProfileStringW(
            L"led",
            L"serialPort",
            L"COM5",
            port_input,
            _countof(port_input),
            filename);

    // Sanitize the output path. If it's a serial COM port, it needs to be prefixed
    // with `\\.\`.
    wcsncpy(cfg->led_serial_port, L"\\\\.\\", 4);
    wcsncat_s(cfg->led_serial_port, 11, port_input, 6);
}
