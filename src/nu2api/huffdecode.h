/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"
#endif

typedef struct nualloc_arena_s NUALLOC_ARENA;

typedef struct pc_filepakhdr_v0_s {
    u32 type;
    u32 item_count;
    u32 file_size;
} PC_FILEPAKHDR_V0;

typedef struct pc_filepakhdr_v1_s {
    u32 type;
    u32 item_count;
    u32 file_size;
    u32 checksum;
    u32 flags;
} PC_FILEPAKHDR_V1;

typedef struct pc_filepakhdr_s {
    u32 type;
    u32 item_count;
    u32 file_size;
    u32 checksum;
    u32 flags;
    u32 decrypted_checksum;
} PC_FILEPAKHDR;

typedef struct pc_filepakitem_s {
    i32 name_offset;
    i32 data_offset;
    i32 size;
    i32 alignment;
    u32 flags;
    u32 pre_move_callback_va;
    u32 post_move_callback_va;
} PC_FILEPAKITEM;

STATIC_ASSERT(sizeof(PC_FILEPAKHDR_V0) == 12);
STATIC_ASSERT(sizeof(PC_FILEPAKHDR_V1) == 20);
STATIC_ASSERT(sizeof(PC_FILEPAKHDR) == 24);
STATIC_ASSERT(sizeof(PC_FILEPAKITEM) == 28);
STATIC_ASSERT(offsetof(PC_FILEPAKITEM, name_offset) == 0);
STATIC_ASSERT(offsetof(PC_FILEPAKITEM, data_offset) == 4);
STATIC_ASSERT(offsetof(PC_FILEPAKITEM, size) == 8);
STATIC_ASSERT(offsetof(PC_FILEPAKITEM, alignment) == 12);
STATIC_ASSERT(offsetof(PC_FILEPAKITEM, flags) == 16);
STATIC_ASSERT(offsetof(PC_FILEPAKITEM, pre_move_callback_va) == 20);
STATIC_ASSERT(offsetof(PC_FILEPAKITEM, post_move_callback_va) == 24);
typedef void (*NUFPCOMFN)(NUFPAR_PC *parser);
typedef i32 (*NURDPGETVARFN)(char *name, f32 *value, i32 *found);

typedef struct pc_nudatfileinfo_s {
    NUDATHDR *header;
    u32 unknown_0004;
    i64 start;
    i64 position;
    i32 file_length;
    i32 decompressed_length;
    i32 open_file_index;
    i32 is_used;
    i32 compression_mode;
    u32 unknown_002c;
} PC_NUDATFILEINFO;

STATIC_ASSERT(sizeof(PC_NUDATFILEINFO) == 0x30);
STATIC_ASSERT(offsetof(PC_NUDATFILEINFO, start) == 8);
STATIC_ASSERT(offsetof(PC_NUDATFILEINFO, position) == 0x10);
STATIC_ASSERT(offsetof(PC_NUDATFILEINFO, file_length) == 0x18);
STATIC_ASSERT(offsetof(PC_NUDATFILEINFO, decompressed_length) == 0x1c);
STATIC_ASSERT(offsetof(PC_NUDATFILEINFO, open_file_index) == 0x20);
STATIC_ASSERT(offsetof(PC_NUDATFILEINFO, is_used) == 0x24);
STATIC_ASSERT(offsetof(PC_NUDATFILEINFO, compression_mode) == 0x28);

typedef struct pc_numemfile_s {
    u8 *buffer;
    u8 *end;
    u8 *position;
    i32 mode;
    i32 used;
} PC_NUMEMFILE;

STATIC_ASSERT(sizeof(PC_NUMEMFILE) == 0x14);
STATIC_ASSERT(offsetof(PC_NUMEMFILE, position) == 8);
STATIC_ASSERT(offsetof(PC_NUMEMFILE, mode) == 0xc);
STATIC_ASSERT(offsetof(PC_NUMEMFILE, used) == 0x10);

typedef struct pc_nufile_readblock_s {
    i32 tag;
    i32 size;
    i64 position;
} PC_NUFILE_READBLOCK;

STATIC_ASSERT(sizeof(PC_NUFILE_READBLOCK) == 0x10);
STATIC_ASSERT(offsetof(PC_NUFILE_READBLOCK, size) == 4);
STATIC_ASSERT(offsetof(PC_NUFILE_READBLOCK, position) == 8);

typedef struct PC_FILEEXTINFO {
    char extension[13];
    char type;
    char platform;
    char len;
} PC_FILEEXTINFO;

STATIC_ASSERT(sizeof(PC_FILEEXTINFO) == 16);
STATIC_ASSERT(offsetof(PC_FILEEXTINFO, extension) == 0);
STATIC_ASSERT(offsetof(PC_FILEEXTINFO, type) == 13);
STATIC_ASSERT(offsetof(PC_FILEEXTINFO, platform) == 14);
STATIC_ASSERT(offsetof(PC_FILEEXTINFO, len) == 15);

typedef struct FRAME_SCANNED_PAD_PC {
    u8 is_valid;
    u8 reserved01[3];
    i32 mapped_to_pad;
    u8 analog_left_x;
    u8 analog_left_y;
    u8 analog_right_x;
    u8 analog_right_y;
    u8 analog_l1;
    u8 analog_l2;
    u8 analog_r1;
    u8 analog_r2;
    u32 digital_buttons;
    u32 digital_buttons_pressed;
    u32 digital_buttons_released;
    u32 digital_buttons_prev;
    i32 analog_button_flags;
    i32 digital_left_x;
    i32 digital_left_y;
    i32 digital_right_x;
    i32 digital_right_y;
    i32 deadzone;
    f32 left_x_normal;
    f32 left_y_normal;
    f32 left_x_unknown;
    f32 left_y_unknown;
    f32 left_x_unknown2;
    f32 left_y_unknown2;
    f32 right_x_unknown2;
    f32 right_y_unknown2;
    f32 right_x_normal;
    f32 right_y_normal;
    f32 right_x_unknown;
    f32 right_y_unknown;
    f32 left_up_ramp;
    f32 left_down_ramp;
    f32 left_left_ramp;
    f32 left_right_ramp;
    u8 analog_left_pad_left;
    u8 analog_left_pad_right;
    u8 analog_left_pad_up;
    u8 analog_left_pad_down;
    u8 unknown_7c;
    u8 unknown_7d;
    u8 unknown_7e;
    u8 unknown_7f;
    u8 unknown_80;
    u8 unknown_81;
    u8 unknown_82;
    u8 unknown_83;
} FRAME_SCANNED_PAD_PC;

typedef union FRAME_RECORD_CURSOR_PC {
    u8 *u8_ptr;
    u32 *u32_ptr;
    f32 *f32_ptr;
} FRAME_RECORD_CURSOR_PC;

typedef struct FRAME_PAD_RECORD_PC {
    i32 mode;
    u8 *buf_start;
    u8 *buf_end;
    FRAME_RECORD_CURSOR_PC record;
    i32 buf_size;
    char *filepath;
    i32 should_end;
    u32 end_record_buttons;
    u32 end_play_buttons;
    i32 is_recording;
} FRAME_PAD_RECORD_PC;

STATIC_ASSERT(sizeof(FRAME_SCANNED_PAD_PC) == 0x84);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, mapped_to_pad) == 4);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_left_x) == 8);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, digital_buttons) == 0x10);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, digital_buttons_prev) == 0x1c);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_button_flags) == 0x20);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, digital_left_x) == 0x24);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, deadzone) == 0x34);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, left_x_normal) == 0x38);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, left_x_unknown2) == 0x48);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, right_x_unknown2) == 0x50);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, right_x_normal) == 0x58);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, left_up_ramp) == 0x68);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_left_pad_left) == 0x78);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, unknown_7c) == 0x7c);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, unknown_82) == 0x82);
STATIC_ASSERT(sizeof(FRAME_PAD_RECORD_PC) == 0x28);
STATIC_ASSERT(offsetof(FRAME_PAD_RECORD_PC, record) == 0xc);
STATIC_ASSERT(offsetof(FRAME_PAD_RECORD_PC, should_end) == 0x18);
STATIC_ASSERT(offsetof(FRAME_PAD_RECORD_PC, is_recording) == 0x24);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, is_valid) == 0x0);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_left_y) == 0x9);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_right_x) == 0xa);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_right_y) == 0xb);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_l1) == 0xc);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_l2) == 0xd);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_r1) == 0xe);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_r2) == 0xf);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, digital_buttons_pressed) == 0x14);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, digital_buttons_released) == 0x18);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, digital_left_y) == 0x28);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, digital_right_x) == 0x2c);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, digital_right_y) == 0x30);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, left_y_normal) == 0x3c);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, left_x_unknown) == 0x40);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, left_y_unknown) == 0x44);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, left_y_unknown2) == 0x4c);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, right_y_unknown2) == 0x54);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, right_y_normal) == 0x5c);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, right_x_unknown) == 0x60);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, right_y_unknown) == 0x64);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, left_down_ramp) == 0x6c);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, left_left_ramp) == 0x70);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, left_right_ramp) == 0x74);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_left_pad_right) == 0x79);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_left_pad_up) == 0x7a);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, analog_left_pad_down) == 0x7b);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, unknown_7d) == 0x7d);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, unknown_7e) == 0x7e);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, unknown_7f) == 0x7f);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, unknown_80) == 0x80);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, unknown_81) == 0x81);
STATIC_ASSERT(offsetof(FRAME_SCANNED_PAD_PC, unknown_83) == 0x83);
STATIC_ASSERT(offsetof(FRAME_PAD_RECORD_PC, mode) == 0x0);
STATIC_ASSERT(offsetof(FRAME_PAD_RECORD_PC, buf_start) == 0x4);
STATIC_ASSERT(offsetof(FRAME_PAD_RECORD_PC, buf_end) == 0x8);
STATIC_ASSERT(offsetof(FRAME_PAD_RECORD_PC, buf_size) == 0x10);
STATIC_ASSERT(offsetof(FRAME_PAD_RECORD_PC, filepath) == 0x14);
STATIC_ASSERT(offsetof(FRAME_PAD_RECORD_PC, end_record_buttons) == 0x1c);
STATIC_ASSERT(offsetof(FRAME_PAD_RECORD_PC, end_play_buttons) == 0x20);
#ifdef __cplusplus
extern "C" {
#endif
    extern char UiSelectPromptLiteralPc[7];
    extern char UiBackPromptLiteralPc[5];
    extern NUDATHDR *curr_dat;
    extern const f64 NuTime_LowScale;
    extern const f64 NuScale1000;
    extern const f64 NuTime_MicroScale;
    extern const f64 NuTime_ScanlineScale;
    extern f32 NuVideoFrameRate;
    extern NUFPAR_PC NuFParRecords[4];
    extern char NuFParBuffers[4][0x1000];
    i32 GetLineW(u16 *buffer, NUFILE file);
    extern void (*fnInterpreterError)(NUFPAR_PC *parser);
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
    extern const char nufile_disc_suffix[3];
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
    extern const char NuFileExt_gcm_PC[5];
    extern const char NuFileExt_dsp_PC[5];
    extern const char NuFileExt_ctx_PC[5];
    extern const char NuFileExt_csc_PC[5];
    extern const char NuFileExt_chg_PC[5];
    extern const char NuFileExt_pss_PC[5];
    extern const char NuFileExt_vag_PC[5];
    extern const char NuFileExt_pnt_PC[5];
    extern const char NuFileExt_ghg_PC[5];
    extern const char NuFileExt_pc_ghg_PC[8];
    extern const char NuFileExt_lr_pc_ghg_PC[11];
    extern const char NuFileExt_pc_gsc_PC[8];
    extern const char NuFileExt_bik_PC[5];
    extern const char NuFileExt_wav_PC[5];
    extern const char NuFileExt_cu2_PC[5];
    extern const char NuFileExt_pc_dds_PC[8];
    extern const char NuFileExt_bsa_PC[5];
    extern const char NuFileExt_an3_PC[5];
    extern const char NuFileExt_pam_PC[5];
    extern const char NuFileExt_mib_PC[5];
    extern const char NuFileExt_msf_PC[5];
    extern const char NuFileExt_ps3_tex_PC[9];
    extern const char NuFileExt_ps3_gsc_PC[9];
    extern const char NuFileExt_ps3_ghg_PC[9];
    extern const char NuFileExt_wmv_PC[5];
    extern const char NuFileExt_wavm_PC[6];
    extern const char NuFileExt_wavx_PC[6];
    extern const char NuFileExt_cc2_PC[5];
    extern const char NuFileExt_360_tex_PC[9];
    extern const char NuFileExt_360_gsc_PC[9];
    extern const char NuFileExt_360_ghg_PC[9];
    extern const char NuFileExt_cbs_PC[5];
    extern const char NuFileExt_ca3_PC[5];
    extern PC_FILEEXTINFO extensions[64];
    extern i32 num_extensions;
    extern PC_FILEEXTINFO extensions[64];
    extern i32 num_extensions;
    i32 NuFileUseHiresPC(void);
    extern const char NuFileSourcePathPC[50];
    extern void (*NuApiEndFrameCallbackPc)(void);
    extern const char NuTextureDdsExtensionPc[4];
    extern i32 gExplodeInitialised;
    extern i32 FramePadProfilePortPc;
    extern char *iop_image_name;
#ifdef __cplusplus
}
#endif
