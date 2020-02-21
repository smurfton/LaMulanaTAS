// This is more documentation than anything else
#pragma once

#include "LaMulanaMemory.h"
#include <map>
#include <unordered_set>
#include <functional>
#include <windows.h>
#include "xinput.h"

enum
{
    PAD_BTN = 0x100,
    PAD_START = PAD_BTN,
    PAD_SELECT,
    PAD_L3,
    PAD_R3,
    PAD_L1,
    PAD_R1,
    PAD_A,
    PAD_B,
    PAD_X,
    PAD_Y,
    PAD_L2,
    PAD_R2,
    PAD_UP = 0x180,
    PAD_DOWN,
    PAD_LEFT,
    PAD_RIGHT,
};

static const WORD XINPUT_BTN[] = {
    XINPUT_GAMEPAD_START,
    XINPUT_GAMEPAD_BACK,
    XINPUT_GAMEPAD_LEFT_THUMB,
    XINPUT_GAMEPAD_RIGHT_THUMB,
    XINPUT_GAMEPAD_LEFT_SHOULDER,
    XINPUT_GAMEPAD_RIGHT_SHOULDER,
    XINPUT_GAMEPAD_A,
    XINPUT_GAMEPAD_B,
    XINPUT_GAMEPAD_X,
    XINPUT_GAMEPAD_Y,
};

static const WORD XINPUT_DPAD[] = {
    XINPUT_GAMEPAD_DPAD_UP,
    XINPUT_GAMEPAD_DPAD_DOWN,
    XINPUT_GAMEPAD_DPAD_LEFT,
    XINPUT_GAMEPAD_DPAD_RIGHT,
};

#define OVERLAY_LEFT 8
#define OVERLAY_RIGHT 632
#define OVERLAY_TOP 42
#define MAIN_OVERLAY_TOP 426
#define OVERLAY_BOTTOM 474

class TAS
{
public:
    class Overlay
    {
    public:
        TAS &tas;
        LaMulanaMemory &memory;
        Overlay(TAS &tas) : tas(tas), memory(tas.memory) {}
        virtual bool ProcessKeys() { return true; } // return false to inhibit the main overlay's processing
        virtual void Draw() {};
    };

    LaMulanaMemory memory;
    int frame, frame_count, rngsteps;
    std::map<int, std::string> sections;
    std::map<int, std::unordered_set<int>> frame_inputs;
    std::map<int, std::list<std::function<void()>>> frame_actions;
    std::unordered_set<int> cur_frame_inputs;
    std::vector<LaMulanaMemory::objfixup> objfixups;
    short currng = -1;
    using frame_iter = std::map<int, std::unordered_set<int>>::iterator;
    std::map<std::string, int> name2vk;
    IDirect3DDevice9 *curdev;
    std::unique_ptr<BitmapFont> font4x6, font8x12;
    struct {
        CComPtr<IDirect3DTexture9> tex;
        float texel_w, texel_h;
    } hit_parts, hit_hex;
    CComPtr<IDirect3DTexture9> overlay;
    CComPtr<IDirect3DSurface9> overlay_surf;
    int repeat_delay, repeat_speed;

    struct keystate {
        union {
            struct {
                unsigned char held : 1;
                unsigned char pressed : 1;
                unsigned char released : 1;
                unsigned char repeat : 1;
                unsigned char passthrough : 1;
            };
            unsigned char bits;
        };
        unsigned char repeat_counter;
    } keys[256];

    void UpdateKeys()
    {
        bool active = !!memory.kb_enabled;
        for (int vk = 0; vk < 256; vk++)
        {
            bool held = active && GetAsyncKeyState(vk) < 0;
            keys[vk].pressed = held && !keys[vk].held;
            keys[vk].released = !held && keys[vk].held;
            keys[vk].held = keys[vk].passthrough = held;
            if (keys[vk].pressed)
            {
                keys[vk].repeat = true;
                keys[vk].repeat_counter = repeat_delay;
            }
            else if (held && --keys[vk].repeat_counter == 0)
            {
                keys[vk].repeat = true;
                keys[vk].repeat_counter = repeat_speed;
            }
            else
            {
                keys[vk].repeat = false;
            }
        }
        keys[VK_SHIFT].passthrough = false;
        keys[VK_LSHIFT].passthrough = false;
        keys[VK_RSHIFT].passthrough = false;
    }

    enum POLLTYPE { POLL_HELD, POLL_PRESSED, POLL_RELEASED, POLL_REPEAT };
    bool Poll(int vk, bool mod = false, POLLTYPE type = POLL_PRESSED)
    {
        if (mod != !!keys[VK_SHIFT].held)
            return false;
        keys[vk].passthrough = false;
        return !!(keys[vk].bits & 1 << type);
    }

    bool initialised, run = true, pause, resetting, has_reset, ff;
    bool scripting = true, passthrough = true;
    bool show_overlay = true, show_exits, show_solids, show_loc, hide_game;
    int show_tiles;
    unsigned show_hitboxes = 1 << 7 | 1 << 9 | 1 << 11;  // unknown types, I want to know if anyone sees them
    Overlay *shopping_overlay;
    Overlay *extra_overlay;

    TAS(char *base);
    void LoadBindings();
    bool KeyPressed(int vk);
    DWORD GetXInput(DWORD idx, XINPUT_STATE *state);
    void IncFrame();
    void Overlay();
    void DrawOverlay();
    void ProcessKeys();

    void LoadTAS();
};

class ShoppingOverlay : public TAS::Overlay
{
public:
    ShoppingOverlay(TAS &tas) : Overlay(tas) {}
    void Draw() override;
};

extern HMODULE tasModule;

#ifdef __cplusplus
extern "C" {
#endif
    void __stdcall TasInit(int);
    SHORT __stdcall TasGetKeyState(int nVirtKey);
    DWORD __stdcall TasIncFrame(void);
    int __stdcall TasRender(void);
    DWORD __stdcall TasTime(void);
    DWORD __stdcall TasXInputGetState(DWORD idx, XINPUT_STATE *state);
    void TasSleep(int);
#ifdef __cplusplus
}
#endif
