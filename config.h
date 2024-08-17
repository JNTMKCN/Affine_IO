#pragma once

#include <stddef.h>
#include <stdint.h>

struct chuni_io_config {
    uint8_t vk_test;
    uint8_t vk_service;
    uint8_t vk_coin;
    uint8_t vk_ir_emu;
    uint8_t vk_ir[6];
    uint8_t vk_cell[32];

    // Which ways to output LED information are enabled
    bool cab_led_output_pipe;
    bool cab_led_output_serial;
    
    bool controller_led_output_pipe;
    bool controller_led_output_serial;

    bool controller_led_output_openithm;

    // The name of a COM port to output LED data on, in serial mode
    wchar_t led_serial_port[12];
    int32_t led_serial_baud;
};

void chuni_io_config_load(
        struct chuni_io_config *cfg,
        const wchar_t *filename);
