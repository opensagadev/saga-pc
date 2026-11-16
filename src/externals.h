#pragma once

#include <windows.h>
#include <objbase.h>

typedef DWORD D3DCOLOR;
typedef struct _D3DXMACRO D3DXMACRO;
typedef struct _D3DXIMAGE_INFO D3DXIMAGE_INFO;
#ifdef __cplusplus
typedef struct D3DXMATRIX D3DXMATRIX;
typedef struct D3DXVECTOR3 D3DXVECTOR3;
#else
typedef struct _D3DMATRIX D3DXMATRIX;
typedef struct _D3DVECTOR D3DXVECTOR3;
#endif
typedef struct IDirect3D9 IDirect3D9;
typedef struct IDirect3DDevice9 IDirect3DDevice9;
typedef struct IDirect3DCubeTexture9 IDirect3DCubeTexture9;
typedef struct IDirect3DTexture9 IDirect3DTexture9;
typedef struct IDirect3DSurface9 IDirect3DSurface9;
typedef struct ID3DXInclude ID3DXInclude;
typedef struct ID3DXEffectPool ID3DXEffectPool;
typedef struct ID3DXEffect ID3DXEffect;
typedef struct ID3DXBuffer ID3DXBuffer;
typedef struct ID3DXFont ID3DXFont;
typedef struct ID3DXRenderToSurface ID3DXRenderToSurface;
typedef struct ID3DXEffectCompiler ID3DXEffectCompiler;
typedef struct ID3DXConstantTable ID3DXConstantTable;
typedef struct IDirectSound8 IDirectSound8;
typedef struct IDirectSound8 *LPDIRECTSOUND8;
typedef struct IDirectSound8Vtbl IDirectSound8Vtbl;
typedef struct IDirectSoundBuffer IDirectSoundBuffer;
typedef struct IDirectSoundBufferVtbl IDirectSoundBufferVtbl;
typedef struct IDirectSound3DBuffer IDirectSound3DBuffer;
typedef struct IDirectSound3DBufferVtbl IDirectSound3DBufferVtbl;

typedef void *DSOUND_OPAQUE_METHOD;

typedef struct PC_DS_CAPS { // size 0x60
    DWORD size;             // 0x00
    DWORD remaining[23];    // 0x04
} PC_DS_CAPS;

typedef struct PC_DS_BUFFER_DESC { // size 0x24
    DWORD size;                    // 0x00
    DWORD flags;                   // 0x04
    DWORD bytes;                   // 0x08
    DWORD reserved;                // 0x0c
    WAVEFORMATEX *format;          // 0x10
    GUID algorithm;                // 0x14
} PC_DS_BUFFER_DESC;

struct IDirectSound8 {
    const IDirectSound8Vtbl *vtable;
};

struct IDirectSound8Vtbl {                                                 // size 0x24
    HRESULT(__stdcall *query_interface)(IDirectSound8 *, REFIID, void **); // 0x00
    ULONG(__stdcall *add_ref)(IDirectSound8 *);                            // 0x04
    ULONG(__stdcall *release)(IDirectSound8 *);                            // 0x08
    HRESULT(__stdcall *create_sound_buffer)                                // 0x0c
    (IDirectSound8 *, const PC_DS_BUFFER_DESC *, IDirectSoundBuffer **, IUnknown *);
    HRESULT(__stdcall *get_caps)(IDirectSound8 *, PC_DS_CAPS *);                                              // 0x10
    HRESULT(__stdcall *duplicate_sound_buffer)(IDirectSound8 *, IDirectSoundBuffer *, IDirectSoundBuffer **); // 0x14
    HRESULT(__stdcall *set_cooperative_level)(IDirectSound8 *, HWND, DWORD);                                  // 0x18
    void *compact;                                                                                            // 0x1c
    HRESULT(__stdcall *get_speaker_config)(IDirectSound8 *, DWORD *);                                         // 0x20
};

struct IDirectSoundBuffer {
    const IDirectSoundBufferVtbl *vtable;
};

struct IDirectSoundBufferVtbl {                                                                              // size 0x54
    HRESULT(__stdcall *query_interface)(IDirectSoundBuffer *, REFIID, void **);                              // 0x00
    ULONG(__stdcall *add_ref)(IDirectSoundBuffer *);                                                         // 0x04
    ULONG(__stdcall *release)(IDirectSoundBuffer *);                                                         // 0x08
    DSOUND_OPAQUE_METHOD get_caps;                                                                           // 0x0c
    HRESULT(__stdcall *get_current_position)(IDirectSoundBuffer *, DWORD *, DWORD *);                        // 0x10
    DSOUND_OPAQUE_METHOD get_format;                                                                         // 0x14
    HRESULT(__stdcall *get_volume)(IDirectSoundBuffer *, LONG *);                                            // 0x18
    DSOUND_OPAQUE_METHOD get_pan;                                                                            // 0x1c
    DSOUND_OPAQUE_METHOD get_frequency;                                                                      // 0x20
    HRESULT(__stdcall *get_status)(IDirectSoundBuffer *, DWORD *);                                           // 0x24
    DSOUND_OPAQUE_METHOD initialize;                                                                         // 0x28
    HRESULT(__stdcall *lock)(IDirectSoundBuffer *, DWORD, DWORD, void **, DWORD *, void **, DWORD *, DWORD); // 0x2c
    HRESULT(__stdcall *play)(IDirectSoundBuffer *, DWORD, DWORD, DWORD);                                     // 0x30
    HRESULT(__stdcall *set_current_position)(IDirectSoundBuffer *, DWORD);                                   // 0x34
    HRESULT(__stdcall *set_format)(IDirectSoundBuffer *, const WAVEFORMATEX *);                              // 0x38
    HRESULT(__stdcall *set_volume)(IDirectSoundBuffer *, LONG);                                              // 0x3c
    DSOUND_OPAQUE_METHOD set_pan;                                                                            // 0x40
    HRESULT(__stdcall *set_frequency)(IDirectSoundBuffer *, DWORD);                                          // 0x44
    HRESULT(__stdcall *stop)(IDirectSoundBuffer *);                                                          // 0x48
    HRESULT(__stdcall *unlock)(IDirectSoundBuffer *, void *, DWORD, void *, DWORD);                          // 0x4c
    HRESULT(__stdcall *restore)(IDirectSoundBuffer *);                                                       // 0x50
};

struct IDirectSound3DBuffer {
    const IDirectSound3DBufferVtbl *vtable;
};

struct IDirectSound3DBufferVtbl {                                                         // size 0x50
    HRESULT(__stdcall *query_interface)(IDirectSound3DBuffer *, REFIID, void **);         // 0x00
    ULONG(__stdcall *add_ref)(IDirectSound3DBuffer *);                                    // 0x04
    ULONG(__stdcall *release)(IDirectSound3DBuffer *);                                    // 0x08
    DSOUND_OPAQUE_METHOD get_all_parameters;                                              // 0x0c
    DSOUND_OPAQUE_METHOD get_cone_angles;                                                 // 0x10
    DSOUND_OPAQUE_METHOD get_cone_orientation;                                            // 0x14
    DSOUND_OPAQUE_METHOD get_cone_outside_volume;                                         // 0x18
    DSOUND_OPAQUE_METHOD get_max_distance;                                                // 0x1c
    DSOUND_OPAQUE_METHOD get_min_distance;                                                // 0x20
    DSOUND_OPAQUE_METHOD get_mode;                                                        // 0x24
    DSOUND_OPAQUE_METHOD get_position;                                                    // 0x28
    DSOUND_OPAQUE_METHOD get_velocity;                                                    // 0x2c
    DSOUND_OPAQUE_METHOD set_all_parameters;                                              // 0x30
    DSOUND_OPAQUE_METHOD set_cone_angles;                                                 // 0x34
    DSOUND_OPAQUE_METHOD set_cone_orientation;                                            // 0x38
    DSOUND_OPAQUE_METHOD set_cone_outside_volume;                                         // 0x3c
    HRESULT(__stdcall *set_max_distance)(IDirectSound3DBuffer *, float, DWORD);           // 0x40
    HRESULT(__stdcall *set_min_distance)(IDirectSound3DBuffer *, float, DWORD);           // 0x44
    HRESULT(__stdcall *set_mode)(IDirectSound3DBuffer *, DWORD, DWORD);                   // 0x48
    HRESULT(__stdcall *set_position)(IDirectSound3DBuffer *, float, float, float, DWORD); // 0x4c
};

typedef struct _XINPUT_STATE XINPUT_STATE;

// The XInput SDK spells these members; NOLINT keeps them byte- and name-compatible with xinput.h.
typedef struct _XINPUT_VIBRATION {
    WORD wLeftMotorSpeed;  // NOLINT(readability-identifier-naming)
    WORD wRightMotorSpeed; // NOLINT(readability-identifier-naming)
} XINPUT_VIBRATION;
typedef struct _XINPUT_CAPABILITIES XINPUT_CAPABILITIES;

// The enum tags and widths are fixed; add named values as source needs them.
typedef enum _D3DFORMAT { D3DFMT_UNKNOWN = 0, D3DFMT_FORCE_DWORD = 0x7fffffff } D3DFORMAT;

typedef enum _D3DPOOL {
    D3DPOOL_DEFAULT = 0,
    D3DPOOL_MANAGED = 1,
    D3DPOOL_SYSTEMMEM = 2,
    D3DPOOL_SCRATCH = 3,
    D3DPOOL_FORCE_DWORD = 0x7fffffff
} D3DPOOL;

typedef enum _D3DXIMAGE_FILEFORMAT { D3DXIFF_BMP = 0, D3DXIFF_FORCE_DWORD = 0x7fffffff } D3DXIMAGE_FILEFORMAT;

#ifdef __cplusplus
extern "C" {
#endif

    int __stdcall D3DPERF_BeginEvent(D3DCOLOR, const WCHAR *);
    int __stdcall D3DPERF_EndEvent(void);
    HRESULT __stdcall D3DXCompileShader(const char *, UINT, const D3DXMACRO *, ID3DXInclude *, const char *, const char *, DWORD,
                                        ID3DXBuffer **, ID3DXBuffer **, ID3DXConstantTable **);
    HRESULT __stdcall D3DXCreateBuffer(DWORD, ID3DXBuffer **);
    HRESULT __stdcall D3DXCreateCubeTextureFromFileInMemory(IDirect3DDevice9 *, const void *, UINT, IDirect3DCubeTexture9 **);
    HRESULT __stdcall D3DXCreateEffect(IDirect3DDevice9 *, const void *, UINT, const D3DXMACRO *, ID3DXInclude *, DWORD, ID3DXEffectPool *,
                                       ID3DXEffect **, ID3DXBuffer **);
    HRESULT __stdcall D3DXCreateEffectCompiler(const char *, UINT, const D3DXMACRO *, ID3DXInclude *, DWORD, ID3DXEffectCompiler **,
                                               ID3DXBuffer **);
    HRESULT __stdcall D3DXCreateEffectFromFileA(IDirect3DDevice9 *, const char *, const D3DXMACRO *, ID3DXInclude *, DWORD,
                                                ID3DXEffectPool *, ID3DXEffect **, ID3DXBuffer **);
    HRESULT __stdcall D3DXCreateFontA(IDirect3DDevice9 *, INT, UINT, UINT, UINT, BOOL, DWORD, DWORD, DWORD, DWORD, const char *,
                                      ID3DXFont **);
    HRESULT __stdcall D3DXCreateRenderToSurface(IDirect3DDevice9 *, UINT, UINT, D3DFORMAT, BOOL, D3DFORMAT, ID3DXRenderToSurface **);
    HRESULT __stdcall D3DXCreateTextureFromFileInMemory(IDirect3DDevice9 *, const void *, UINT, IDirect3DTexture9 **);
    HRESULT __stdcall D3DXCreateTextureFromFileInMemoryEx(IDirect3DDevice9 *, const void *, UINT, UINT, UINT, UINT, DWORD, D3DFORMAT,
                                                          D3DPOOL, DWORD, DWORD, D3DCOLOR, D3DXIMAGE_INFO *, PALETTEENTRY *,
                                                          IDirect3DTexture9 **);
    D3DXMATRIX *__stdcall D3DXMatrixLookAtLH(D3DXMATRIX *, const D3DXVECTOR3 *, const D3DXVECTOR3 *, const D3DXVECTOR3 *);
    D3DXMATRIX *__stdcall D3DXMatrixMultiply(D3DXMATRIX *, const D3DXMATRIX *, const D3DXMATRIX *);
    D3DXMATRIX *__stdcall D3DXMatrixOrthoLH(D3DXMATRIX *, FLOAT, FLOAT, FLOAT, FLOAT);
    HRESULT __stdcall D3DXSaveSurfaceToFileInMemory(ID3DXBuffer **, D3DXIMAGE_FILEFORMAT, IDirect3DSurface9 *, const PALETTEENTRY *,
                                                    const RECT *);
    IDirect3D9 *__stdcall Direct3DCreate9(UINT);
    HRESULT __stdcall DirectInput8Create(HINSTANCE, DWORD, REFIID, void **, IUnknown *);
    HRESULT __stdcall DirectSoundCreate8(LPCGUID, LPDIRECTSOUND8 *, LPUNKNOWN);
    DWORD __stdcall XInputGetCapabilities(DWORD, DWORD, XINPUT_CAPABILITIES *);
    DWORD __stdcall XInputGetState(DWORD, XINPUT_STATE *);
    DWORD __stdcall XInputSetState(DWORD, XINPUT_VIBRATION *);

    // The old SDK winternl.h omits the calling convention on RtlUnwind.
    // The imported x86 entry uses stdcall; do not use its cdecl declaration.
    __declspec(dllimport) void __stdcall RtlUnwind(void *, void *, EXCEPTION_RECORD *, void *);

    // binkw32.dll export suffixes establish stack sizes, not parameter types.
    // These symbols name opaque IAT slots; their function signatures are unknown.
    extern void *saga_import_binkw32_dll__BinkSetSoundTrack_8;     // _BinkSetSoundTrack@8
    extern void *saga_import_binkw32_dll__BinkOpen_8;              // _BinkOpen@8
    extern void *saga_import_binkw32_dll__BinkNextFrame_4;         // _BinkNextFrame@4
    extern void *saga_import_binkw32_dll__BinkSetVolume_12;        // _BinkSetVolume@12
    extern void *saga_import_binkw32_dll__BinkOpenDirectSound_4;   // _BinkOpenDirectSound@4
    extern void *saga_import_binkw32_dll__BinkSetSoundSystem_8;    // _BinkSetSoundSystem@8
    extern void *saga_import_binkw32_dll__BinkDoFrame_4;           // _BinkDoFrame@4
    extern void *saga_import_binkw32_dll__BinkCopyToBufferRect_44; // _BinkCopyToBufferRect@44
    extern void *saga_import_binkw32_dll__BinkGetRealtime_12;      // _BinkGetRealtime@12
    extern void *saga_import_binkw32_dll__BinkWait_4;              // _BinkWait@4
    extern void *saga_import_binkw32_dll__BinkGoto_12;             // _BinkGoto@12
    extern void *saga_import_binkw32_dll__BinkClose_4;             // _BinkClose@4

#ifdef __cplusplus
}
#endif
