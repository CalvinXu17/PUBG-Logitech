#include "logitech_driver.h"
#include <windows.h>
#include <winternl.h>
#include <iostream>
#pragma comment(lib, "ntdll.lib")

typedef struct {
    char button;
    char x;
    char y;
    char wheel;
    char unk1;
} MOUSE_IO;

#define MOUSE_PRESS 1
#define MOUSE_RELEASE 2
#define MOUSE_MOVE 3
#define MOUSE_CLICK 4

static HANDLE g_input;
static IO_STATUS_BLOCK g_io;

BOOL g_found_mouse;

static BOOL callmouse(MOUSE_IO* buffer)
{
    IO_STATUS_BLOCK block;
    return NtDeviceIoControlFile(g_input, 0, 0, 0, &block, 0x2a2010, buffer, sizeof(MOUSE_IO), 0, 0) == 0L;
}

static NTSTATUS device_initialize(PCWSTR device_name)
{
    UNICODE_STRING name;
    OBJECT_ATTRIBUTES attr;

    RtlInitUnicodeString(&name, device_name);
    InitializeObjectAttributes(&attr, &name, 0, NULL, NULL);

    NTSTATUS status = NtCreateFile(&g_input, GENERIC_WRITE | SYNCHRONIZE, &attr, &g_io, 0,
        FILE_ATTRIBUTE_NORMAL, 0, 3, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, 0, 0);

    return status;
}

const wchar_t* f_path[] = {
    L"\\??\\ROOT#SYSTEM#%04d#{1abc05c0-c378-41b9-9cef-df1aba82b015}",
    L"\\??\\ROOT#SYSTEM#%04d#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}",
};

BOOL mouse_open(void)
{
    NTSTATUS status = 0;

    if (g_input == 0) {

        wchar_t path[MAX_PATH];

        for (int i = 0; i < sizeof(f_path) / sizeof(f_path[0]); i++) {
            for (int j=0; j<16; j++) {
                swprintf_s(path, MAX_PATH, f_path[i], j);
                status = device_initialize(path);
                if (NT_SUCCESS(status)) {
                    g_found_mouse = 1;
                    printf("mouse %d\n", i);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}


void mouse_close(void)
{
    if (g_input != 0) {
        NtClose(g_input);
        g_input = 0;
    }
}


void mouse_move(char button, char x, char y, char wheel)
{
    MOUSE_IO io;
    io.unk1 = 0;
    io.button = button;
    io.x = x;
    io.y = y;
    io.wheel = wheel;

    if (!callmouse(&io)) {
        mouse_close();
        mouse_open();
    }
}

void moveR(int x, int y)
{
    if (abs(x) > 127 || abs(y) > 127) {
        int x_left = x; int y_left = y;
        if (abs(x) > 127) {
            mouse_move(0, int(x / abs(x)) * 127, 0, 0);
            x_left = x - int(x / abs(x)) * 127;
        }
        else { mouse_move(0, int(x), 0, 0); x_left = 0; }

        if (abs(y) > 127) {
            mouse_move(0, 0, int(y / abs(y)) * 127, 0);
            y_left = y - int(y / abs(y)) * 127;
        }
        else { mouse_move(0, 0, int(y), 0); y_left = 0; }

        return moveR(x_left, y_left);
    }
    else { mouse_move(0, x, y, 0); }
}

void press(char button)
{
    mouse_move(button, 0, 0, 0);
}

void release()
{
    mouse_move(0, 0, 0, 0);
}


