/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#endif

struct nufpar_s;

typedef struct nualloc_arena_s NUALLOC_ARENA;

typedef struct pc_filepakhdr_v0_s { // size 0xc
    u32 type;                       // 0x0
    u32 item_count;                 // 0x4
    u32 file_size;                  // 0x8
} PC_FILEPAKHDR_V0;

typedef struct pc_filepakhdr_v1_s { // size 0x14
    u32 type;                       // 0x00
    u32 item_count;                 // 0x04
    u32 file_size;                  // 0x08
    u32 checksum;                   // 0x0c
    u32 flags;                      // 0x10
} PC_FILEPAKHDR_V1;

typedef struct pc_filepakhdr_s { // size 0x18
    u32 type;                    // 0x00
    u32 item_count;              // 0x04
    u32 file_size;               // 0x08
    u32 checksum;                // 0x0c
    u32 flags;                   // 0x10
    u32 decrypted_checksum;      // 0x14
} PC_FILEPAKHDR;

typedef struct pc_filepakitem_s { // size 0x1c
    i32 name_offset;              // 0x00
    i32 data_offset;              // 0x04
    i32 size;                     // 0x08
    i32 alignment;                // 0x0c
    u32 flags;                    // 0x10
    u32 pre_move_callback_va;     // 0x14
    u32 post_move_callback_va;    // 0x18
} PC_FILEPAKITEM;

typedef i32 (*NURDPGETVARFN)(char *name, f32 *value, i32 *found);

typedef struct pc_nudatfileinfo_s { // size 0x30
    NUDATHDR *header;               // 0x00
    u32 unknown_0004;               // 0x04
    i64 start;                      // 0x08
    i64 position;                   // 0x10
    i32 file_length;                // 0x18
    i32 decompressed_length;        // 0x1c
    i32 open_file_index;            // 0x20
    i32 is_used;                    // 0x24
    i32 compression_mode;           // 0x28
    u32 unknown_002c;               // 0x2c
} PC_NUDATFILEINFO;

typedef struct pc_numemfile_s { // size 0x14
    u8 *buffer;                 // 0x00
    u8 *end;                    // 0x04
    u8 *position;               // 0x08
    i32 mode;                   // 0x0c
    i32 used;                   // 0x10
} PC_NUMEMFILE;

typedef struct pc_nufile_readblock_s { // size 0x10
    i32 tag;                           // 0x0
    i32 size;                          // 0x4
    i64 position;                      // 0x8
} PC_NUFILE_READBLOCK;

typedef struct PC_FILEEXTINFO { // size 0x10
    char extension[13];         // 0x0
    char type;                  // 0xd
    char platform;              // 0xe
    char len;                   // 0xf
} PC_FILEEXTINFO;

typedef struct FRAME_SCANNED_PAD_PC { // size 0x84
    u8 is_valid;                      // 0x00
    u8 reserved01[3];                 // 0x01
    i32 mapped_to_pad;                // 0x04
    u8 analog_left_x;                 // 0x08
    u8 analog_left_y;                 // 0x09
    u8 analog_right_x;                // 0x0a
    u8 analog_right_y;                // 0x0b
    u8 analog_l1;                     // 0x0c
    u8 analog_l2;                     // 0x0d
    u8 analog_r1;                     // 0x0e
    u8 analog_r2;                     // 0x0f
    u32 digital_buttons;              // 0x10
    u32 digital_buttons_pressed;      // 0x14
    u32 digital_buttons_released;     // 0x18
    u32 digital_buttons_prev;         // 0x1c
    i32 analog_button_flags;          // 0x20
    i32 digital_left_x;               // 0x24
    i32 digital_left_y;               // 0x28
    i32 digital_right_x;              // 0x2c
    i32 digital_right_y;              // 0x30
    i32 deadzone;                     // 0x34
    f32 left_x_normal;                // 0x38
    f32 left_y_normal;                // 0x3c
    f32 left_x_unknown;               // 0x40
    f32 left_y_unknown;               // 0x44
    f32 left_x_unknown2;              // 0x48
    f32 left_y_unknown2;              // 0x4c
    f32 right_x_unknown2;             // 0x50
    f32 right_y_unknown2;             // 0x54
    f32 right_x_normal;               // 0x58
    f32 right_y_normal;               // 0x5c
    f32 right_x_unknown;              // 0x60
    f32 right_y_unknown;              // 0x64
    f32 left_up_ramp;                 // 0x68
    f32 left_down_ramp;               // 0x6c
    f32 left_left_ramp;               // 0x70
    f32 left_right_ramp;              // 0x74
    u8 analog_left_pad_left;          // 0x78
    u8 analog_left_pad_right;         // 0x79
    u8 analog_left_pad_up;            // 0x7a
    u8 analog_left_pad_down;          // 0x7b
    u8 unknown_7c;                    // 0x7c
    u8 unknown_7d;                    // 0x7d
    u8 unknown_7e;                    // 0x7e
    u8 unknown_7f;                    // 0x7f
    u8 unknown_80;                    // 0x80
    u8 unknown_81;                    // 0x81
    u8 unknown_82;                    // 0x82
    u8 unknown_83;                    // 0x83
} FRAME_SCANNED_PAD_PC;

typedef union FRAME_RECORD_CURSOR_PC {
    u8 *u8_ptr;
    u32 *u32_ptr;
    f32 *f32_ptr;
} FRAME_RECORD_CURSOR_PC;

typedef struct FRAME_PAD_RECORD_PC { // size 0x28
    i32 mode;                        // 0x00
    u8 *buf_start;                   // 0x04
    u8 *buf_end;                     // 0x08
    FRAME_RECORD_CURSOR_PC record;   // 0x0c
    i32 buf_size;                    // 0x10
    char *filepath;                  // 0x14
    i32 should_end;                  // 0x18
    u32 end_record_buttons;          // 0x1c
    u32 end_play_buttons;            // 0x20
    i32 is_recording;                // 0x24
} FRAME_PAD_RECORD_PC;

#ifdef __cplusplus
extern "C" {
#endif
    extern char UiSelectPromptLiteral[7];
    extern char UiBackPromptLiteral[5];
    extern NUDATHDR *curr_dat;
    extern const f64 NuTime_LowScale;
    extern const f64 NuTime_MicroScale;
    extern const f64 NuTime_ScanlineScale;
    extern f32 NuVideoFrameRate;
    extern char NuFParBuffers[4][0x1000];
    i32 GetLineW(u16 *buffer, NUFILE file);
    extern void (*fnInterpreterError)(struct nufpar_s *parser);
    extern i32 ShowingError;
    extern PC_NUDATFILEINFO *unpack_file_info;
    extern NUDATOPENFILEINFO *unpack_file_odi;
    extern i64 unpack_file_pos;
    extern FILEINFO file_info[33];
    extern FILEBUFF file_buff[4];
    extern PC_NUDATFILEINFO pc_dat_file_infos[20];
    extern PC_NUMEMFILE pc_memfiles[20];
    extern i32 datsys_offset;
    extern i32 nufile_lasterror;
    extern i32 nufile_try_packed;
    extern i32 numdevices;
    extern i32 DontDoFileUpperCaseHack;
    extern i32 nufile_blkcnt;
    extern PC_NUFILE_READBLOCK nufile_blkinfo[1024];
    extern NUALLOC_ARENA *NuAllocArena;
    extern void *NuAllocManager;
    extern void *NuAllocPeakEnd0;
    extern char *NuBlankString;
    extern const f64 BoltRayAccelerationTimeSquared;
    extern const u16 bad_fluff[26];
    extern NUSTRINGBANK StringBank[3];
    extern i16 CurrentStringBank;
    extern u32 nustring_format;
    extern u16 bad_equiv[8][2];
    i32 NuWindowsProcessMessages(void);
    extern u32 PCImplodeWordMask;
    extern i32 SoundVoiceEnabledGate;
    extern i32 nufile_lsn_allowed;
    extern const char NuFileExt_gcm[5];
    extern const char NuFileExt_dsp[5];
    extern const char NuFileExt_ctx[5];
    extern const char NuFileExt_csc[5];
    extern const char NuFileExt_chg[5];
    extern const char NuFileExt_pc_ghg[8];
    extern const char NuFileExt_lr_pc_ghg[11];
    extern const char NuFileExt_pc_gsc[8];
    extern const char NuFileExt_bik[5];
    extern const char NuFileExt_pc_dds[8];
    extern const char NuFileExt_pam[5];
    extern const char NuFileExt_msf[5];
    extern const char NuFileExt_ps3_tex[9];
    extern const char NuFileExt_ps3_gsc[9];
    extern const char NuFileExt_ps3_ghg[9];
    extern const char NuFileExt_360_tex[9];
    extern const char NuFileExt_360_gsc[9];
    extern const char NuFileExt_360_ghg[9];
    extern PC_FILEEXTINFO extensions[64];
    extern i32 num_extensions;
    extern PC_FILEEXTINFO extensions[64];
    extern i32 num_extensions;
    i32 NuFileUseHiresPC(void);
    extern void (*NuApiEndFrameCallback)(void);
    extern const char NuTextureDdsExtension[4];
    extern i32 gExplodeInitialised;
    extern i32 FramePadProfilePortPc;
    extern char *iop_image_name;
#ifdef __cplusplus
}
#endif

typedef struct nualloc_manager_prefix_s { // size 0x10
    void *base;                           // 0x0
    u32 size;                             // 0x4
    u32 block_count;                      // 0x8
    u32 used_bytes;                       // 0xc
} NUALLOC_MANAGER_PREFIX_PC;

typedef struct nualloc_manager_block_s { // size 0xc

    union { // size 0x4
        u32 size_flags;

        struct {           // size 0x4
            u32 size : 31; // 0x0:0
            u32 used : 1;  // 0x0:31
        };
    }; // 0x0
    struct nualloc_manager_block_s *next;     // 0x4
    struct nualloc_manager_block_s *previous; // 0x8
} NUALLOC_MANAGER_BLOCK;
