#ifndef SAGA_H
#define SAGA_H

#include <stddef.h>

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // NOLINT(readability-identifier-naming): SDK-required Windows XP API selector.
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <shlobj.h>
#include <objbase.h>
#include <oleauto.h>

#include "externals.h"

// NOLINTBEGIN(google-runtime-int): These aliases must spell their compiler-provided base types.
typedef signed char i8;
typedef unsigned char u8;
typedef signed short i16;
typedef unsigned short u16;
typedef signed int i32;
typedef unsigned int u32;
typedef signed __int64 i64;
typedef unsigned __int64 u64;
typedef float f32;
typedef double f64;
// NOLINTEND(google-runtime-int)

#define SAGA_JOIN_IMPL(a, b) a##b
#define SAGA_JOIN(a, b) SAGA_JOIN_IMPL(a, b)
#define STATIC_ASSERT(expression) typedef char SAGA_JOIN(saga_static_assert_, __LINE__)[(expression) ? 1 : -1]
#define STUBBED() ((void)0)
#define STUBBED_NAMED(name) ((void)0)

// VC8-compatible C/C++ checks for the retail x86 data model.
STATIC_ASSERT(sizeof(i8) == 1);
STATIC_ASSERT(sizeof(i16) == 2);
STATIC_ASSERT(sizeof(i32) == 4);
STATIC_ASSERT(sizeof(i64) == 8);
STATIC_ASSERT(sizeof(f32) == 4);
STATIC_ASSERT(sizeof(f64) == 8);
STATIC_ASSERT(sizeof(void *) == 4);
STATIC_ASSERT(sizeof(MSG) == 28);

typedef i32 NUANG;

typedef struct nunrand_s {
    i32 value;
} NURAND;

STATIC_ASSERT(sizeof(NURAND) == 4);

typedef struct nulistlnk_s {
    struct nulistlnk_s *next;
    struct nulistlnk_s *prev;
} NULISTLNK;
typedef struct nulisthdr_s {
    NULISTLNK *head;
    NULISTLNK *tail;
} NULISTHDR;
STATIC_ASSERT(sizeof(NULISTLNK) == 8);
STATIC_ASSERT(offsetof(NULISTLNK, next) == 0);
STATIC_ASSERT(offsetof(NULISTLNK, prev) == 4);
STATIC_ASSERT(sizeof(NULISTHDR) == 8);
STATIC_ASSERT(offsetof(NULISTHDR, head) == 0);
STATIC_ASSERT(offsetof(NULISTHDR, tail) == 4);

typedef struct gizaimessage_s {
    NULISTLNK links;
    char name[32];
    f32 value;
    i8 output_values[8];
    i8 output_count;
    u8 unknown_35;
    u16 flags;
} GIZAIMESSAGE;
STATIC_ASSERT(sizeof(GIZAIMESSAGE) == 0x38);
STATIC_ASSERT(offsetof(GIZAIMESSAGE, name) == 8);
STATIC_ASSERT(offsetof(GIZAIMESSAGE, value) == 0x28);
STATIC_ASSERT(offsetof(GIZAIMESSAGE, output_values) == 0x2c);
STATIC_ASSERT(offsetof(GIZAIMESSAGE, output_count) == 0x34);
STATIC_ASSERT(offsetof(GIZAIMESSAGE, flags) == 0x36);

typedef struct gizaimessagesys_s {
    i32 count;
    GIZAIMESSAGE *messages;
    NULISTHDR free_list;
    NULISTHDR active_list;
} GIZAIMESSAGESYS;
STATIC_ASSERT(sizeof(GIZAIMESSAGESYS) == 0x18);
STATIC_ASSERT(offsetof(GIZAIMESSAGESYS, messages) == 4);
STATIC_ASSERT(offsetof(GIZAIMESSAGESYS, free_list) == 8);
STATIC_ASSERT(offsetof(GIZAIMESSAGESYS, active_list) == 0x10);

typedef struct nudathdr_s NUDATHDR;
typedef struct vufnt_s VUFNT;

typedef struct vucharidx_s {
    u16 unicode;
    u16 index;
} VUCHARIDX;
STATIC_ASSERT(sizeof(VUCHARIDX) == 4);
STATIC_ASSERT(offsetof(VUCHARIDX, index) == 2);

typedef struct vufntchar_s {
    f32 x;
    f32 y;
    f32 width;
} VUFNTCHAR;
STATIC_ASSERT(sizeof(VUFNTCHAR) == 12);
STATIC_ASSERT(offsetof(VUFNTCHAR, x) == 0);
STATIC_ASSERT(offsetof(VUFNTCHAR, y) == 4);
STATIC_ASSERT(offsetof(VUFNTCHAR, width) == 8);

struct vufnt_s {
    u8 unknown_0000[0x0c];
    i32 glyph_count;
    i32 unicode_count;
    f32 height;
    f32 baseline;
    f32 space_width;
    i32 mode;
    f32 ic_gap;
    void *legacy_font;
    f32 *x_scale;
    f32 *y_scale;
    VUFNTCHAR *glyphs;
    VUCHARIDX *unicode_map;
    u8 unknown_003c[4];
    struct numtl_s *material;
    u8 unknown_0044[4];
    struct nuqfnt_render_data_pc_s *platform_data;
};
STATIC_ASSERT(offsetof(VUFNT, glyph_count) == 0x0c);
STATIC_ASSERT(offsetof(VUFNT, unicode_count) == 0x10);
STATIC_ASSERT(offsetof(VUFNT, height) == 0x14);
STATIC_ASSERT(offsetof(VUFNT, baseline) == 0x18);
STATIC_ASSERT(offsetof(VUFNT, space_width) == 0x1c);
STATIC_ASSERT(offsetof(VUFNT, mode) == 0x20);
STATIC_ASSERT(offsetof(VUFNT, ic_gap) == 0x24);
STATIC_ASSERT(offsetof(VUFNT, x_scale) == 0x2c);
STATIC_ASSERT(offsetof(VUFNT, glyphs) == 0x34);
STATIC_ASSERT(offsetof(VUFNT, unicode_map) == 0x38);
STATIC_ASSERT(offsetof(VUFNT, material) == 0x40);
STATIC_ASSERT(offsetof(VUFNT, platform_data) == 0x48);

typedef struct menu_s {
    u8 unknown_0000[8];
    i16 selected_column;
    i16 selected_row;
    u8 unknown_000c[8];
    i16 menu;
    u8 unknown_0016[0x1e];
    i32 selected_item;
    i32 selected_item_column;
    u8 unknown_003c[0x20];
    i32 left_pressed;
    i32 right_pressed;
    i32 confirm_pressed;
    i32 cancel_pressed;
    u8 unknown_006c[0x20];
    i32 draw_item;
    f32 item_scale;
    u8 unknown_0094[0x4c];
} MENU;
STATIC_ASSERT(sizeof(MENU) == 0xe0);
STATIC_ASSERT(offsetof(MENU, selected_column) == 0x08);
STATIC_ASSERT(offsetof(MENU, selected_row) == 0x0a);
STATIC_ASSERT(offsetof(MENU, menu) == 0x14);
STATIC_ASSERT(offsetof(MENU, selected_item) == 0x34);
STATIC_ASSERT(offsetof(MENU, selected_item_column) == 0x38);
STATIC_ASSERT(offsetof(MENU, left_pressed) == 0x5c);
STATIC_ASSERT(offsetof(MENU, right_pressed) == 0x60);
STATIC_ASSERT(offsetof(MENU, confirm_pressed) == 0x64);
STATIC_ASSERT(offsetof(MENU, cancel_pressed) == 0x68);
STATIC_ASSERT(offsetof(MENU, draw_item) == 0x8c);
STATIC_ASSERT(offsetof(MENU, item_scale) == 0x90);

typedef struct menufninfo_s {
    i32 id;
    void (*enter_fn)(MENU *);
    void (*draw_fn)(MENU *);
    void (*update_fn)(MENU *);
    void (*exit_fn)(MENU *);
    i16 memory_x;
    i16 memory_y;
    i8 wrap;
    i8 confirm_prompt;
    i8 cancel_prompt;
    u8 unknown_001b;
} MENUFNINFO;
STATIC_ASSERT(sizeof(MENUFNINFO) == 0x1c);
STATIC_ASSERT(offsetof(MENUFNINFO, enter_fn) == 4);
STATIC_ASSERT(offsetof(MENUFNINFO, exit_fn) == 0x10);
STATIC_ASSERT(offsetof(MENUFNINFO, memory_x) == 0x14);
STATIC_ASSERT(offsetof(MENUFNINFO, wrap) == 0x18);

typedef struct optionssave_s {
    u8 unknown_0000[3];
    u8 sound_volume;
    u8 music_volume;
    u8 master_volume;
    u8 music_enabled;
    u8 unknown_0007[4];
    u8 widescreen;
    u8 brightness;
} OPTIONSSAVE;
STATIC_ASSERT(sizeof(OPTIONSSAVE) == 0x0d);
STATIC_ASSERT(offsetof(OPTIONSSAVE, sound_volume) == 3);
STATIC_ASSERT(offsetof(OPTIONSSAVE, music_volume) == 4);
STATIC_ASSERT(offsetof(OPTIONSSAVE, master_volume) == 5);
STATIC_ASSERT(offsetof(OPTIONSSAVE, widescreen) == 0x0b);

typedef struct gamesave_s {
    u32 unknown_0000;
    OPTIONSSAVE options_save;
    u8 unknown_0011[0x7e3b];
} GAMESAVE;
STATIC_ASSERT(sizeof(GAMESAVE) == 0x7e4c);
STATIC_ASSERT(offsetof(GAMESAVE, options_save) == 4);

typedef struct playerprogress_s {
    u32 coins;
    u16 unknown_0004;
    u8 active;
    u8 unknown_0007[9];
} PLAYERPROGRESS;
STATIC_ASSERT(sizeof(PLAYERPROGRESS) == 0x10);
STATIC_ASSERT(offsetof(PLAYERPROGRESS, active) == 6);

typedef struct numtlattrib_s {
    u32 alpha_mode : 4;
    u32 filter_mode : 2;
    u32 unknown_0_64_128 : 2;
    u32 unknown_1_1_2 : 2;
    u32 unknown_1_4_8 : 2;
    u32 cull_mode : 2;
    u32 z_mode : 2;
    u32 unknown_2_1_2 : 2;
    u32 unknown_2_4 : 1;
    u32 unknown_19_31 : 13;
} NUMTLATTRIB;
STATIC_ASSERT(sizeof(NUMTLATTRIB) == 4);
typedef struct nudisplaylist_pc_s NUDISPLAYLIST_PC;
typedef struct numtl_s {
    u8 unknown_0000[0x3c];
    NUDISPLAYLIST_PC *display_list;
    NUMTLATTRIB attribs;
    u8 unknown_0044[0x30];
    i16 texture_id;
} NUMTL;
STATIC_ASSERT(offsetof(NUMTL, display_list) == 0x3c);
STATIC_ASSERT(offsetof(NUMTL, attribs) == 0x40);
STATIC_ASSERT(offsetof(NUMTL, texture_id) == 0x74);

typedef struct nudisplayscene_pc_s {
    u8 unknown_0000[0x58];
    u8 *material_used_front;
    u8 *material_used_back;
    u8 unknown_0060[0x14];
    u16 flags;
} NUDISPLAYSCENE_PC;
STATIC_ASSERT(offsetof(NUDISPLAYSCENE_PC, material_used_front) == 0x58);
STATIC_ASSERT(offsetof(NUDISPLAYSCENE_PC, material_used_back) == 0x5c);
STATIC_ASSERT(offsetof(NUDISPLAYSCENE_PC, flags) == 0x74);

struct nudisplaylist_pc_s {
    NUDISPLAYSCENE_PC *scene;
    i32 material_id;
    void *render_state;
    u8 unknown_000c[8];
    void *material_last;
    void *dynamic_geometry;
    u8 unknown_001c[8];
    void *items;
};
STATIC_ASSERT(offsetof(NUDISPLAYLIST_PC, scene) == 0x00);
STATIC_ASSERT(offsetof(NUDISPLAYLIST_PC, material_id) == 0x04);
STATIC_ASSERT(offsetof(NUDISPLAYLIST_PC, render_state) == 0x08);
STATIC_ASSERT(offsetof(NUDISPLAYLIST_PC, material_last) == 0x14);
STATIC_ASSERT(offsetof(NUDISPLAYLIST_PC, dynamic_geometry) == 0x18);
STATIC_ASSERT(offsetof(NUDISPLAYLIST_PC, items) == 0x24);

typedef struct nutexdim_pc_s {
    i32 width;
    i32 height;
} NUTEXDIM_PC;
STATIC_ASSERT(sizeof(NUTEXDIM_PC) == 8);

typedef struct part_emit_pc_s {
    i32 effect_id;
    u8 unknown_0004[0x22];
    i8 page;
    u8 unknown_0027[0x0d];
    f32 camera_distance;
    u8 unknown_0038[0x34];
} PART_EMIT_PC;
STATIC_ASSERT(sizeof(PART_EMIT_PC) == 0x6c);
STATIC_ASSERT(offsetof(PART_EMIT_PC, page) == 0x26);
STATIC_ASSERT(offsetof(PART_EMIT_PC, camera_distance) == 0x34);

typedef struct nupad_init_state_s {
    u8 unknown_00[0x88];
    u8 init_type;
    u8 init_flags;
    u8 init_port;
} NUPAD_INIT_STATE;
STATIC_ASSERT(offsetof(NUPAD_INIT_STATE, init_type) == 0x88);
STATIC_ASSERT(offsetof(NUPAD_INIT_STATE, init_flags) == 0x89);
STATIC_ASSERT(offsetof(NUPAD_INIT_STATE, init_port) == 0x8a);

typedef struct gamepad_init_record_s {
    NUPAD_INIT_STATE *pad;
    u8 unknown_04[0x56];
    u16 allocated;
    u8 unknown_5c[4];
} GAMEPAD_INIT_RECORD;
STATIC_ASSERT(sizeof(GAMEPAD_INIT_RECORD) == 0x60);
STATIC_ASSERT(offsetof(GAMEPAD_INIT_RECORD, pad) == 0);
STATIC_ASSERT(offsetof(GAMEPAD_INIT_RECORD, allocated) == 0x5a);

typedef struct NuKeyboardState_PC {
    u8 unknown_0000_to_4e10[0x4e10];
    u8 keyboard_polled;
    u8 unknown_4e11_to_68b0[0x68b0 - 0x4e11];
    u8 *keyboard_state;
} NuKeyboardState_PC;
STATIC_ASSERT(sizeof(NuKeyboardState_PC) == 0x68b4);
STATIC_ASSERT(offsetof(NuKeyboardState_PC, keyboard_polled) == 0x4e10);
STATIC_ASSERT(offsetof(NuKeyboardState_PC, keyboard_state) == 0x68b0);

typedef struct NuKeyboardDevice_PC NuKeyboardDevice_PC;
typedef struct NuKeyboardOwner_PC {
    f32 unknown_0000;
    i32 unknown_0004;
    f32 unknown_0008;
    u8 unknown_000c_to_0024[0x18];
    NuKeyboardDevice_PC *device;
    i32 poll_status;
} NuKeyboardOwner_PC;
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, device) == 0x24);
STATIC_ASSERT(offsetof(NuKeyboardOwner_PC, poll_status) == 0x28);

typedef struct nualloc_record_s NUALLOC_RECORD;
typedef struct nualloc_pool_s NUALLOC_POOL;
typedef struct nualloc_arena_s NUALLOC_ARENA;

typedef struct bgprocinfo_s BGPROCINFO;

typedef union variptr_u {
    void *void_ptr;
    char *char_ptr;
    u32 addr;
} VARIPTR;
STATIC_ASSERT(sizeof(VARIPTR) == 4);

typedef i32 NUFILE;

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

typedef struct nufpar_pc_s {
    char *file_buffer;
    NUFILE file_handle;
    char file_name[0x100];
    char line_buffer[0x202];
    char word_buffer[0x202];
    char *line_buffer_ptr;
    char *word_buffer_ptr;
    i32 line_buffer_size;
    i32 word_buffer_size;
    i32 line_number;
    i32 line_position;
    i32 character_position;
    i32 buffer_start;
    i32 buffer_end;
    u8 unknown_0530[0x40];
    i32 command_position;
    i32 file_size;
    u8 is_utf16;
    u8 is_utf8;
    u8 unknown_057a[2];
    char *separator_list;
    char *separator_tokens;
} NUFPAR_PC;

STATIC_ASSERT(offsetof(NUFPAR_PC, file_handle) == 0x4);
STATIC_ASSERT(offsetof(NUFPAR_PC, line_buffer) == 0x108);
STATIC_ASSERT(offsetof(NUFPAR_PC, word_buffer) == 0x30a);
STATIC_ASSERT(offsetof(NUFPAR_PC, line_buffer_ptr) == 0x50c);
STATIC_ASSERT(offsetof(NUFPAR_PC, word_buffer_ptr) == 0x510);
STATIC_ASSERT(offsetof(NUFPAR_PC, line_buffer_size) == 0x514);
STATIC_ASSERT(offsetof(NUFPAR_PC, word_buffer_size) == 0x518);
STATIC_ASSERT(offsetof(NUFPAR_PC, line_number) == 0x51c);
STATIC_ASSERT(offsetof(NUFPAR_PC, line_position) == 0x520);
STATIC_ASSERT(offsetof(NUFPAR_PC, character_position) == 0x524);
STATIC_ASSERT(offsetof(NUFPAR_PC, buffer_start) == 0x528);
STATIC_ASSERT(offsetof(NUFPAR_PC, buffer_end) == 0x52c);
STATIC_ASSERT(offsetof(NUFPAR_PC, is_utf16) == 0x578);
STATIC_ASSERT(offsetof(NUFPAR_PC, is_utf8) == 0x579);
STATIC_ASSERT(offsetof(NUFPAR_PC, separator_list) == 0x57c);
STATIC_ASSERT(offsetof(NUFPAR_PC, separator_tokens) == 0x580);
STATIC_ASSERT(sizeof(NUFPAR_PC) == 0x584);

typedef struct nufparpos_pc_s {
    i32 line_number;
    i32 line_position;
    i32 character_position;
    i32 buffer_start;
    i32 buffer_end;
} NUFPARPOS_PC;
STATIC_ASSERT(sizeof(NUFPARPOS_PC) == 0x14);
STATIC_ASSERT(offsetof(NUFPARPOS_PC, line_number) == 0x00);
STATIC_ASSERT(offsetof(NUFPARPOS_PC, line_position) == 0x04);
STATIC_ASSERT(offsetof(NUFPARPOS_PC, character_position) == 0x08);
STATIC_ASSERT(offsetof(NUFPARPOS_PC, buffer_start) == 0x0c);
STATIC_ASSERT(offsetof(NUFPARPOS_PC, buffer_end) == 0x10);

typedef struct nuwind_s {
    u8 unknown_0000[0x28];
    f32 phase_speed_scale;
    f32 phase;
    f32 accumulated_time;
    f32 scaled_accumulated_time;
} NUWIND;
STATIC_ASSERT(sizeof(NUWIND) == 0x38);
STATIC_ASSERT(offsetof(NUWIND, phase_speed_scale) == 0x28);
STATIC_ASSERT(offsetof(NUWIND, phase) == 0x2c);
STATIC_ASSERT(offsetof(NUWIND, accumulated_time) == 0x30);
STATIC_ASSERT(offsetof(NUWIND, scaled_accumulated_time) == 0x34);

typedef struct nudatfinfo_s {
    i32 file_offset;
    i32 file_len;
    i32 decompressed_len;
    i32 compression_mode;
} NUDATFINFO;
STATIC_ASSERT(sizeof(NUDATFINFO) == 0x10);

typedef struct nudfnode_s {
    i16 child_idx;
    i16 sibling_idx;
    char *name;
} NUDFNODE;
STATIC_ASSERT(sizeof(NUDFNODE) == 8);

typedef struct nudatopenfileinfo_s {
    NUFILE dat_file;
    i32 info_idx;
    i64 position;
} NUDATOPENFILEINFO;
STATIC_ASSERT(sizeof(NUDATOPENFILEINFO) == 0x10);
STATIC_ASSERT(offsetof(NUDATOPENFILEINFO, position) == 8);
STATIC_ASSERT(offsetof(NUDATOPENFILEINFO, dat_file) == 0);
STATIC_ASSERT(offsetof(NUDATOPENFILEINFO, info_idx) == 4);

struct nudathdr_s {
    i32 version;
    i32 file_count;
    NUDATFINFO *file_info;
    i32 tree_node_count;
    NUDFNODE *file_tree;
    i32 leaf_names_len;
    char *leaf_names;
    u32 *hash_idxs;
    i32 hash_count;
    i32 hashes_len;
    char *hashes;
    NUDATOPENFILEINFO open_files[8];
    i16 unknown_b0;
    i16 mode;
    i32 unknown_b4;
    char *name;
};
STATIC_ASSERT(sizeof(NUDATHDR) == 0xc0);
STATIC_ASSERT(offsetof(NUDATHDR, open_files) == 0x30);
STATIC_ASSERT(offsetof(NUDATHDR, unknown_b0) == 0xb0);
STATIC_ASSERT(offsetof(NUDATHDR, mode) == 0xb2);
STATIC_ASSERT(offsetof(NUDATHDR, name) == 0xb8);

typedef struct fileinfo_s FILEINFO;
typedef struct filebuff_s {
    FILEINFO *file_info;
    i32 time;
    char data[1024];
} FILEBUFF;
STATIC_ASSERT(sizeof(FILEBUFF) == 0x408);

struct fileinfo_s {
    i32 handle;
    i64 read_pos;
    i64 file_pos;
    i64 file_len;
    i64 buf_start;
    i32 buf_len;
    i32 use_buf;
    i32 unused;
    i32 mode;
    FILEBUFF *buf;
};
STATIC_ASSERT(sizeof(FILEINFO) == 0x40);
STATIC_ASSERT(offsetof(FILEINFO, read_pos) == 8);
STATIC_ASSERT(offsetof(FILEINFO, file_pos) == 0x10);
STATIC_ASSERT(offsetof(FILEINFO, file_len) == 0x18);
STATIC_ASSERT(offsetof(FILEINFO, buf_start) == 0x20);
STATIC_ASSERT(offsetof(FILEINFO, buf_len) == 0x28);
STATIC_ASSERT(offsetof(FILEINFO, use_buf) == 0x2c);
STATIC_ASSERT(offsetof(FILEINFO, mode) == 0x34);
STATIC_ASSERT(offsetof(FILEINFO, buf) == 0x38);

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

typedef struct nufile_device_s NUFILE_DEVICE;
struct nufile_device_s {
    i32 id;
    i32 unit;
    i32 attr;
    i32 status;
    i32 dir_separator;
    i32 max_name_size;
    i32 max_extension_size;
    i32 max_directory_size;
    i32 free_space;
    u8 unknown_0024[0x10];
    char name[0x10];
    i32 match_len;
    i32 name_len;
    char root[0x20];
    char description[0x40];
    char cur_dir[0x80];
    char sys_dir[0x80];
    char dll_dir[0x80];
    i32 (*format_name_fn)(NUFILE_DEVICE *, char *, char *, i32);
    i32 (*interrogate_fn)(NUFILE_DEVICE *);
};
STATIC_ASSERT(sizeof(NUFILE_DEVICE) == 0x234);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, dir_separator) == 0x10);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, name) == 0x34);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, match_len) == 0x44);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, name_len) == 0x48);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, root) == 0x4c);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, description) == 0x6c);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, cur_dir) == 0xac);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, sys_dir) == 0x12c);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, dll_dir) == 0x1ac);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, format_name_fn) == 0x22c);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, interrogate_fn) == 0x230);

#define NU_PORTAL_STACK_SIZE 0x3000

typedef struct NUGSCNROOM_s {
    u8 unknown_0000[0x10];
    u8 visibility_flags;
    u8 unknown_0011[7];
} NUGSCNROOM;
STATIC_ASSERT(sizeof(NUGSCNROOM) == 0x18);
STATIC_ASSERT(offsetof(NUGSCNROOM, visibility_flags) == 0x10);

typedef struct nuspecial_legacy_pc_s {
    u8 unknown_0000[0x44];
    char *name;
    u8 unknown_0048[4];
} NUSPECIAL_LEGACY_PC;
STATIC_ASSERT(sizeof(NUSPECIAL_LEGACY_PC) == 0x4c);
STATIC_ASSERT(offsetof(NUSPECIAL_LEGACY_PC, name) == 0x44);

typedef struct nudisplay_special_pc_s {
    u8 unknown_0000[0xb4];
    char *name;
    u8 unknown_00b8[0x18];
} NUDISPLAY_SPECIAL_PC;
STATIC_ASSERT(sizeof(NUDISPLAY_SPECIAL_PC) == 0xd0);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, name) == 0xb4);

typedef struct nudisplay_list_pc_s {
    u8 unknown_0000[0x6c];
    i32 special_count;
    NUDISPLAY_SPECIAL_PC *specials;
} NUDISPLAY_LIST_PC;
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, special_count) == 0x6c);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, specials) == 0x70);

typedef struct NUGSCN_s {
    u8 unknown_0000[0x24];
    i32 special_count;
    NUSPECIAL_LEGACY_PC *specials;
    u8 unknown_002c[0x48];
    i32 num_rooms;
    NUGSCNROOM *rooms;
    u8 unknown_007c[0x94];
    NUDISPLAY_LIST_PC *display_list;
} NUGSCN;
STATIC_ASSERT(offsetof(NUGSCN, special_count) == 0x24);
STATIC_ASSERT(offsetof(NUGSCN, specials) == 0x28);
STATIC_ASSERT(offsetof(NUGSCN, num_rooms) == 0x74);
STATIC_ASSERT(offsetof(NUGSCN, rooms) == 0x78);
STATIC_ASSERT(offsetof(NUGSCN, display_list) == 0x110);

typedef struct terrain_surface_pc_s {
    f32 movement_scale;
    u32 flags;
    u32 field_08;
} TERRAIN_SURFACE_PC;
STATIC_ASSERT(sizeof(TERRAIN_SURFACE_PC) == 0x0c);
STATIC_ASSERT(offsetof(TERRAIN_SURFACE_PC, flags) == 4);
STATIC_ASSERT(offsetof(TERRAIN_SURFACE_PC, field_08) == 8);

typedef struct edanim_param_pc_s {
    i32 instance_id;
    u8 unknown_004[0x2cc];
    i8 page;
    u8 unknown_2d1[3];
} EDANIM_PARAM_PC;
STATIC_ASSERT(sizeof(EDANIM_PARAM_PC) == 0x2d4);
STATIC_ASSERT(offsetof(EDANIM_PARAM_PC, page) == 0x2d0);

typedef struct levelspline_pc_s {
    NUGSCN **scene;
    const char *name;
    u16 min_points;
    u16 max_points;
    i16 level;
    i16 area;
} LEVELSPLINE_PC;
STATIC_ASSERT(sizeof(LEVELSPLINE_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, name) == 4);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, min_points) == 8);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, area) == 0xe);

typedef struct rtlset_pc_s RTLSET_PC;

// Only fields with confirmed PC offsets are exposed.
typedef struct WORLDINFO_s {
    u32 unknown_0000[0x40];
    void *buffer_start;
    VARIPTR giz_buffer;
    VARIPTR buffer_end;
    u32 unknown_010c;
    i32 loaded;
    u32 unknown_0114[2];
    i32 level_idx;
    u32 unknown_0120[2];
    struct level_data_pc_s *current_level;
    struct area_data_pc_s *area;
    u32 unknown_0130[3];
    NUGSCN *current_gscn;
    u32 unknown_0140[0xa07];
    void *terrain;
    u32 unknown_2960[0x0c];
    u8 room_visibility_flag;
    u8 rooms_visible[0x100];
    u8 unknown_2a91[3];
    u8 *rooms_visible_ptr;
    u32 unknown_2a98;
    i32 page_part;
    u32 unknown_2aa0[5];
    RTLSET_PC *rtl_set;
    u32 unknown_2ab8[0x9bd];
} WORLDINFO;
STATIC_ASSERT(sizeof(WORLDINFO) == 0x51ac);
STATIC_ASSERT(offsetof(WORLDINFO, buffer_start) == 0x100);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, buffer_end) == 0x108);
STATIC_ASSERT(offsetof(WORLDINFO, loaded) == 0x110);
STATIC_ASSERT(offsetof(WORLDINFO, level_idx) == 0x11c);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLDINFO, area) == 0x12c);
STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(WORLDINFO, terrain) == 0x295c);
STATIC_ASSERT(offsetof(WORLDINFO, room_visibility_flag) == 0x2990);
STATIC_ASSERT(offsetof(WORLDINFO, rooms_visible) == 0x2991);
STATIC_ASSERT(offsetof(WORLDINFO, rooms_visible_ptr) == 0x2a94);
STATIC_ASSERT(offsetof(WORLDINFO, page_part) == 0x2a9c);
STATIC_ASSERT(offsetof(WORLDINFO, rtl_set) == 0x2ab4);

typedef struct area_data_pc_s {
    u8 unknown_0000[0x7a];
    u8 flags;
    u8 unknown_007b[0x1d];
    i16 *hub_player_ids;
} AREA_DATA_PC;
STATIC_ASSERT(sizeof(AREA_DATA_PC) == 0x9c);
STATIC_ASSERT(offsetof(AREA_DATA_PC, flags) == 0x7a);
STATIC_ASSERT(offsetof(AREA_DATA_PC, hub_player_ids) == 0x98);

typedef struct apicharactermodellist_s {
    i16 model_id;
    i16 count;
} APICHARACTERMODELLIST;
STATIC_ASSERT(sizeof(APICHARACTERMODELLIST) == 4);
STATIC_ASSERT(offsetof(APICHARACTERMODELLIST, count) == 2);

typedef struct level_display_pc_s {
    f32 value_00;
    f32 value_04;
    i16 value_08;
    u8 colors[6];
} LEVEL_DISPLAY_PC;
STATIC_ASSERT(sizeof(LEVEL_DISPLAY_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_DISPLAY_PC, colors) == 0x0a);

typedef struct level_data_pc_s {
    char dir[0x40];
    char name[0x20];
    i16 unknown_0060;
    i16 index;
    u32 flags;
    void *callbacks[7];
    LEVEL_DISPLAY_PC display;
    i16 music_index;
    i16 unknown_0096;
    i16 max_ter_platforms;
    i16 max_ter_groups;
    i16 unknown_009c;
    i16 unknown_009e;
    u8 mode;
    u8 alpha;
    u8 unknown_00a2;
    i8 area_index;
    f32 camera_distance;
    f32 hover_distance;
    u8 unknown_00ac[8];
    f32 unknown_00b4[4];
    u8 unknown_00c4[4];
    i8 area_level_index;
    u8 unknown_00c9[7];
    f32 conveyor_speed;
    f32 conveyor_width;
    u8 unknown_00d8[0x40];
    i32 music_tracks[6];
} LEVELDATA_PC;
STATIC_ASSERT(sizeof(LEVELDATA_PC) == 0x130);
STATIC_ASSERT(offsetof(LEVELDATA_PC, dir) == 0x00);
STATIC_ASSERT(offsetof(LEVELDATA_PC, name) == 0x40);
STATIC_ASSERT(offsetof(LEVELDATA_PC, index) == 0x62);
STATIC_ASSERT(offsetof(LEVELDATA_PC, flags) == 0x64);
STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) == 0x68);
STATIC_ASSERT(offsetof(LEVELDATA_PC, display) == 0x84);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_index) == 0x94);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_ter_platforms) == 0x98);
STATIC_ASSERT(offsetof(LEVELDATA_PC, mode) == 0xa0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_index) == 0xa3);
STATIC_ASSERT(offsetof(LEVELDATA_PC, camera_distance) == 0xa4);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_level_index) == 0xc8);
STATIC_ASSERT(offsetof(LEVELDATA_PC, conveyor_speed) == 0xd0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_tracks) == 0x118);

typedef struct dat_handles_pc_s {
    NUDATHDR *previous;
    NUDATHDR *episode[6];
} DAT_HANDLES_PC;
STATIC_ASSERT(sizeof(DAT_HANDLES_PC) == 0x1c);
STATIC_ASSERT(offsetof(DAT_HANDLES_PC, episode) == 4);

typedef struct main_frame_counters_s {
    u8 warmup_frame;
    u8 every_frame;
    u8 alternate_frame;
    u8 third_frame;
    u8 fourth_frame;
} MAINFRAMECOUNTERS;
STATIC_ASSERT(sizeof(MAINFRAMECOUNTERS) == 5);

typedef struct nuvec_s {
    f32 x;
    f32 y;
    f32 z;
} NUVEC;
STATIC_ASSERT(sizeof(NUVEC) == 0xc);

typedef struct numtx_s {
    f32 m00, m01, m02, m03;
    f32 m10, m11, m12, m13;
    f32 m20, m21, m22, m23;
    f32 m30, m31, m32, m33;
} NUMTX;
STATIC_ASSERT(sizeof(NUMTX) == 0x40);
STATIC_ASSERT(offsetof(NUMTX, m30) == 0x30);
STATIC_ASSERT(offsetof(NUMTX, m33) == 0x3c);

typedef struct nuglobalrndrstate_pc_s {
    u8 unknown_0000[0x1e];
    u16 counter_001e;
    u32 counter_0020;
    u8 unknown_0024[0xd0];
    i32 state_00f4;
    i32 state_00f8;
    i32 state_00fc;
    u8 unknown_0100[0x80];
    i32 state_0180;
    u8 unknown_0184[0x14];
    i32 viewport_width;
    u8 unknown_019c[8];
} NUGLOBALRNDRSTATE_PC;
STATIC_ASSERT(sizeof(NUGLOBALRNDRSTATE_PC) == 0x1a4);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, counter_001e) == 0x1e);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, counter_0020) == 0x20);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, state_00f4) == 0xf4);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, state_0180) == 0x180);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, viewport_width) == 0x198);

typedef struct nuqfnt_render_data_pc_s {
    u32 colour;
    f32 x;
    f32 y;
    f32 z;
    NUMTX matrix;
} NUQFNT_RENDER_DATA_PC;
STATIC_ASSERT(offsetof(NUQFNT_RENDER_DATA_PC, colour) == 0);
STATIC_ASSERT(offsetof(NUQFNT_RENDER_DATA_PC, x) == 4);
STATIC_ASSERT(offsetof(NUQFNT_RENDER_DATA_PC, y) == 8);
STATIC_ASSERT(offsetof(NUQFNT_RENDER_DATA_PC, z) == 0xc);
STATIC_ASSERT(offsetof(NUQFNT_RENDER_DATA_PC, matrix) == 0x10);

typedef struct nucamera_s {
    NUMTX mtx;
    u8 unknown_40[0x38];
} NUCAMERA;
STATIC_ASSERT(sizeof(NUCAMERA) == 0x78);
STATIC_ASSERT(offsetof(NUCAMERA, mtx) == 0);

typedef struct nuspecial_handle_s {
    void *scene;
    NUMTX *special;
    NUMTX *display_special;
} NUSPECIALHANDLE;
STATIC_ASSERT(sizeof(NUSPECIALHANDLE) == 0xc);
STATIC_ASSERT(offsetof(NUSPECIALHANDLE, special) == 4);
STATIC_ASSERT(offsetof(NUSPECIALHANDLE, display_special) == 8);

typedef struct gameanimobj_s GAMEANIMOBJ;
typedef struct gameanimobjpool_s GAMEANIMOBJPOOL;
typedef struct gameanimset_s GAMEANIMSET;
typedef struct gameanimsys_s GAMEANIMSYS;

/* This is a verified field view, not a declaration of full retail owner extent. */
typedef struct nuinstanim_time_view_s {
    u8 unknown_00[0x40];
    f32 tfactor;
    u8 unknown_44[8];
    f32 ltime;
    u32 flags;
    f32 fparam1;
} NUINSTANIM_TIME_VIEW;
STATIC_ASSERT(offsetof(NUINSTANIM_TIME_VIEW, tfactor) == 0x40);
STATIC_ASSERT(offsetof(NUINSTANIM_TIME_VIEW, ltime) == 0x4c);
STATIC_ASSERT(offsetof(NUINSTANIM_TIME_VIEW, flags) == 0x50);
STATIC_ASSERT(offsetof(NUINSTANIM_TIME_VIEW, fparam1) == 0x54);

struct gameanimobj_s {
    GAMEANIMOBJ *next;
    NUSPECIALHANDLE special;
    NUINSTANIM_TIME_VIEW *instance_animation;
    void *animation;
    f32 start_frame;
    f32 end_frame;
    u8 unknown_flags_20[4];
    void *object_data;
};
STATIC_ASSERT(sizeof(GAMEANIMOBJ) == 0x28);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, next) == 0);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, special) == 4);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, instance_animation) == 0x10);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, animation) == 0x14);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, start_frame) == 0x18);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, end_frame) == 0x1c);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, unknown_flags_20) == 0x20);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, object_data) == 0x24);

struct gameanimobjpool_s {
    u16 object_data_size;
    u16 active_count;
    u16 capacity;
    u16 unknown_06;
    GAMEANIMOBJ *objects;
    GAMEANIMOBJ *free_objects;
    void *object_data;
};
STATIC_ASSERT(sizeof(GAMEANIMOBJPOOL) == 0x14);
STATIC_ASSERT(offsetof(GAMEANIMOBJPOOL, object_data_size) == 0);
STATIC_ASSERT(offsetof(GAMEANIMOBJPOOL, active_count) == 2);
STATIC_ASSERT(offsetof(GAMEANIMOBJPOOL, capacity) == 4);
STATIC_ASSERT(offsetof(GAMEANIMOBJPOOL, objects) == 8);
STATIC_ASSERT(offsetof(GAMEANIMOBJPOOL, free_objects) == 0xc);
STATIC_ASSERT(offsetof(GAMEANIMOBJPOOL, object_data) == 0x10);

struct gameanimset_s {
    NULISTLNK links;
    u8 object_count;
    u8 animated_object_count;
    u16 flags;
    i32 state;
    GAMEANIMOBJPOOL *object_pool;
    GAMEANIMSYS *system;
    GAMEANIMOBJ *objects;
};
STATIC_ASSERT(sizeof(GAMEANIMSET) == 0x1c);
STATIC_ASSERT(offsetof(GAMEANIMSET, links) == 0);
STATIC_ASSERT(offsetof(GAMEANIMSET, object_count) == 8);
STATIC_ASSERT(offsetof(GAMEANIMSET, animated_object_count) == 9);
STATIC_ASSERT(offsetof(GAMEANIMSET, flags) == 0xa);
STATIC_ASSERT(offsetof(GAMEANIMSET, state) == 0xc);
STATIC_ASSERT(offsetof(GAMEANIMSET, object_pool) == 0x10);
STATIC_ASSERT(offsetof(GAMEANIMSET, system) == 0x14);
STATIC_ASSERT(offsetof(GAMEANIMSET, objects) == 0x18);

struct gameanimsys_s {
    NULISTHDR active_sets;
    i32 set_count;
    GAMEANIMSET **sets;
};
STATIC_ASSERT(sizeof(GAMEANIMSYS) == 0x10);
STATIC_ASSERT(offsetof(GAMEANIMSYS, active_sets) == 0);
STATIC_ASSERT(offsetof(GAMEANIMSYS, set_count) == 8);
STATIC_ASSERT(offsetof(GAMEANIMSYS, sets) == 0xc);

typedef struct gameanimprogress_s {
    i32 count;
    i32 entry_size;
    u8 **entries;
} GAMEANIMPROGRESS;
STATIC_ASSERT(sizeof(GAMEANIMPROGRESS) == 0xc);
STATIC_ASSERT(offsetof(GAMEANIMPROGRESS, count) == 0);
STATIC_ASSERT(offsetof(GAMEANIMPROGRESS, entry_size) == 4);
STATIC_ASSERT(offsetof(GAMEANIMPROGRESS, entries) == 8);

typedef struct language_data_s {
    i32 language;
    i32 unknown_04;
} LANGUAGEDATA;
STATIC_ASSERT(sizeof(LANGUAGEDATA) == 8);

typedef struct text_entry_s {
    i16 *text_id;
    i16 value;
    i16 pad;
} TEXTENTRY;
STATIC_ASSERT(sizeof(TEXTENTRY) == 8);
STATIC_ASSERT(offsetof(TEXTENTRY, value) == 4);

typedef struct NUKEYCHAR_s {
    u32 key;
    u8 normal;
    u8 shifted;
    u8 padding[2];
} NUKEYCHAR;
STATIC_ASSERT(sizeof(NUKEYCHAR) == 8);
STATIC_ASSERT(offsetof(NUKEYCHAR, normal) == 4);
STATIC_ASSERT(offsetof(NUKEYCHAR, shifted) == 5);

typedef struct NuTime_s {
    u32 low;
    u32 high;
} NuTime;
STATIC_ASSERT(sizeof(NuTime) == 8);

typedef struct TIMER_s {
    f32 time_elapsed;
    f32 last_time_elapsed;
    f32 time_elapsed_mod_seconds;
    i32 update_count;
} TIMER;
STATIC_ASSERT(sizeof(TIMER) == 0x10);
STATIC_ASSERT(offsetof(TIMER, last_time_elapsed) == 4);
STATIC_ASSERT(offsetof(TIMER, time_elapsed_mod_seconds) == 8);
STATIC_ASSERT(offsetof(TIMER, update_count) == 0xc);

typedef struct OCCLUDERSET_s {
    u8 unknown_0000[0xc];
    u32 count;
    u8 queries_prepared;
    u8 unknown_0011[0x8f];
} OCCLUDERSET;
STATIC_ASSERT(sizeof(OCCLUDERSET) == 0xa0);
STATIC_ASSERT(offsetof(OCCLUDERSET, count) == 0xc);
STATIC_ASSERT(offsetof(OCCLUDERSET, queries_prepared) == 0x10);

typedef struct OCCLUSIONMANAGER_s {
    u8 initialized;
    u8 enabled;
    u8 unknown_0002[0xe];
    OCCLUDERSET sets[2];
    OCCLUDERSET *building_set;
    OCCLUDERSET *current_set;
    f32 unknown_0158;
    f32 unknown_015c;
    u32 unknown_0160;
    u32 unknown_0164;
    u8 unknown_0168[8];
} OCCLUSIONMANAGER;
STATIC_ASSERT(sizeof(OCCLUSIONMANAGER) == 0x170);
STATIC_ASSERT(offsetof(OCCLUSIONMANAGER, building_set) == 0x150);
STATIC_ASSERT(offsetof(OCCLUSIONMANAGER, current_set) == 0x154);
STATIC_ASSERT(offsetof(OCCLUSIONMANAGER, unknown_0160) == 0x160);
STATIC_ASSERT(offsetof(OCCLUSIONMANAGER, unknown_0164) == 0x164);

typedef struct nuapi_s {
    i32 language;
    i32 region;
    i32 video_mode;
    i32 video_swap_mode;
    i32 video_aspect;
    i32 screen_width;
    i32 screen_height;
    i32 video_is_pal;
    f32 fps;
    i32 frame_begin_count;
    u8 unknown_0028[4];
    NuTime time;
    NuTime time2;
    f32 frame_time;
    i32 frame_count;
    f32 forced_frame_time;
    i32 max_fps;
    u8 unknown_004c[0x2c];
    void *wind;
    i32 reversible_characters;
    u8 unknown_0080[4];
    i32 force_shadows_on_characters;
    i32 frame_end_state;
    f32 video_brightness;
} NUAPI;
STATIC_ASSERT(sizeof(NUAPI) == 0x90);
STATIC_ASSERT(offsetof(NUAPI, language) == 0);
STATIC_ASSERT(offsetof(NUAPI, region) == 4);
STATIC_ASSERT(offsetof(NUAPI, video_mode) == 8);
STATIC_ASSERT(offsetof(NUAPI, screen_width) == 0x14);
STATIC_ASSERT(offsetof(NUAPI, screen_height) == 0x18);
STATIC_ASSERT(offsetof(NUAPI, video_swap_mode) == 0xc);
STATIC_ASSERT(offsetof(NUAPI, video_aspect) == 0x10);
STATIC_ASSERT(offsetof(NUAPI, video_is_pal) == 0x1c);
STATIC_ASSERT(offsetof(NUAPI, fps) == 0x20);
STATIC_ASSERT(offsetof(NUAPI, frame_begin_count) == 0x24);
STATIC_ASSERT(offsetof(NUAPI, time) == 0x2c);
STATIC_ASSERT(offsetof(NUAPI, time2) == 0x34);
STATIC_ASSERT(offsetof(NUAPI, frame_time) == 0x3c);
STATIC_ASSERT(offsetof(NUAPI, frame_count) == 0x40);
STATIC_ASSERT(offsetof(NUAPI, forced_frame_time) == 0x44);
STATIC_ASSERT(offsetof(NUAPI, max_fps) == 0x48);
STATIC_ASSERT(offsetof(NUAPI, wind) == 0x78);
STATIC_ASSERT(offsetof(NUAPI, frame_end_state) == 0x88);
STATIC_ASSERT(offsetof(NUAPI, reversible_characters) == 0x7c);
STATIC_ASSERT(offsetof(NUAPI, force_shadows_on_characters) == 0x84);
STATIC_ASSERT(offsetof(NUAPI, video_brightness) == 0x8c);

#ifdef __cplusplus
extern "C" {
#endif

    int __cdecl NuMain(int argc, char **argv);
    void __cdecl NuCommandLine(i32 *__restrict argc, char ***__restrict argv);
    NUDATHDR *__cdecl NuDatSet(NUDATHDR *header);
    NUDATHDR *__cdecl NuDatGet(void);
    i32 __cdecl NuDatFileFindHash(NUDATHDR *header, char *name);
    i32 __cdecl NuDatFileFindTree(NUDATHDR *header, char *name);
    i32 __cdecl NuFileExistQuiet(char *filepath);
    i32 __cdecl NuFileExists(char *filepath);
    f32 __cdecl NuFileReadFloat(NUFILE file);
    i32 __cdecl NuFileReadInt(NUFILE file);
    i8 __cdecl NuFileReadChar(NUFILE file);
    u16 __cdecl NuFileReadWChar(NUFILE file);
    void __cdecl PCByteCopy_candidate(void *destination, const void *source, u32 count);
    u32 __fastcall PCFilePakChecksum_candidate(i32 count, const u8 *data);
    extern DAT_HANDLES_PC NuDatHandles;
    extern const char EpisodeIDat[14];
    extern const char EpisodeIIDat[15];
    extern const char EpisodeIIIDat[16];
    extern const char EpisodeIVDat[15];
    extern const char EpisodeVDat[14];
    extern const char EpisodeVIDat[15];
    extern i32 argc;
    extern char **argv;
    extern NUDATHDR *curr_dat;
    extern NUAPI nuapi;
    extern OCCLUSIONMANAGER g_OcclusionManager;
    void __cdecl NuOcclusionManagerBeginFrame(void);
    void __cdecl NuFrameBegin(void);
    extern void *Game_NuPad_Store[2];
    extern void **Game_NuPad;
    extern void *NuTimeBar_SetList[16];
    extern i32 NuTimeBar_Initialised;
    extern i64 NuTime_TickBase;
    void __cdecl NuTimeGetTicksPS(u32 *low, u32 *high);
    void __cdecl NuTimeGet(NuTime *time);
    void __cdecl NuTimeSub(NuTime *time, NuTime *a, NuTime *b);
    void __cdecl NuTimeGetTicksPerSecondPS(u32 *low, u32 *high);
    f32 __cdecl NuTimeSeconds(NuTime *time);
    f32 __cdecl NuTimeMilliSeconds(NuTime *time);
    f32 __cdecl NuTimeMicroSeconds(NuTime *time);
    f32 __cdecl NuTimeScanlines(NuTime *time);
    extern i64 NuTime_PerformanceFrequency;
    extern const f64 NuTime_LowScale;
    extern const f64 NuScale1000;
    extern const f64 NuTime_MicroScale;
    extern const f64 NuTime_ScanlineScale;
    extern const f64 NuTime_RefreshRate;
    void __cdecl NuTimeBarSlotReset(i32 set, i32 slot);
    void __cdecl _NuTimeBarSlotBegin(i32 set, i32 slot, const char *name);
    i32 __cdecl NuVideoGetMode(void);
    void __cdecl NuVideoSetMode(i32 mode);
    void __cdecl NuVideoSetResolution(i32 width, i32 height);
    i32 __cdecl NuVideoGetAspect(void);
    i32 __cdecl NuVideoGetSwapMode(void);
    i32 __cdecl NuVideoSetSwapMode(i32 mode);
    void __cdecl NuVideoSetSwapModePS(i32 mode);
    extern f32 NuVideoFrameRate;
    void __cdecl NuVideoRollingFrameRateReset(void);
    void __cdecl NuVideoSetBrightnessPS(void);
    void __cdecl NuVideoSetBrightness(f32 brightness);
    i32 __cdecl NuLanguageGet(void);
    void __cdecl NuLanguageSet(i32 language);
    i32 __cdecl NuRegionGet(void);
    void __cdecl NuRegionSet(i32 region);
    void __cdecl NuMtxMulVU0(NUMTX *result, NUMTX *left, NUMTX *right);
    void __cdecl NuUnicodeToUTF8(u8 *output, u16 *input);
    void __cdecl NuUTF8ToUnicode(u16 *output, u8 *input);
    void __cdecl NuAsciiToUnicode(u16 *output, char *input);
    i32 __cdecl NuIsAlNum(char character);
    i32 __cdecl NuSpecialCompare(NUSPECIALHANDLE *left, NUSPECIALHANDLE *right);
    void __cdecl NuSpecialClear(NUSPECIALHANDLE *special);
    extern VUFNT *QFont2DButtons;
    extern VUFNT *system_qfont;
    extern f32 nuqfnt_space_width;
    extern f32 qfnt_len_scale;
    extern f32 qfnt_height_scale;
    extern f32 qfnt_justify_squash;
    extern f32 qfnt_justify_stretch;
    extern u32 NuQFntMode;
    extern i32 NuQFntModeStackIndex;
    extern u32 NuQFntModeStack[16];
    extern i32 nuqfnt_cs_mode;
    extern f32 qfnt_offscale_x;
    extern f32 qfnt_offscale_y;
    extern f32 qfnt_rezscale_w;
    extern f32 qfnt_rezscale_h;
    void __cdecl NuQFntMoveRS(void *stream, VUFNT *font, f32 x, f32 y, f32 z);
    void __cdecl NuQFntPrintU(VUFNT *font, char *text);
    void __cdecl NuQFntPrintCharW(VUFNT *font, u16 *text, u32 flags);
    void __cdecl NuQFntPrintEx(VUFNT *font, i32 x, i32 y, i32 alignment, const char *format, ...);
    void __cdecl NuQFntSetMtxRS(void *stream, VUFNT *font, NUMTX *matrix);
    void __cdecl NuQFntSetRS(void *stream, VUFNT *font);
    void __cdecl NuQFntSetScaleRS(void *stream, VUFNT *font, f32 x_scale, f32 y_scale);
    void __cdecl NuQFntSetColourRS(void *stream, VUFNT *font, u32 colour);
    extern const f64 NuQFntHalfScale;
    extern NUTEXDIM_PC **NuTextureDimensions;
    extern i32 NuPrimVertexCount;
    extern i32 *NuPrimVertexCountPointer;
    extern VARIPTR *NuPrimStreamBufferPointer;
    extern i32 NuPrimCurrentPrimitiveType;
    extern i32 NuPrimCSPos;
    extern f32 NuPrimXScale;
    extern f32 NuPrimYScale;
    extern f32 NuPrimXBias;
    extern f32 NuPrimYBias;
    extern const f32 NuPrimPS2XScale;
    extern const f32 NuPrimPS2YScale;
    extern const f32 NuPrimTwo;
    extern const f32 NuPrimMinusTwo;
    void __cdecl NuPrimSetCoordinateSystem(i32 scale_mode);
    void __cdecl NuPrim2DEnd(void);
    void __cdecl NuPrim3DEnd(void);
    void __cdecl NuPrim2DBegin(u32 primitive_type, u32 vertex_format, NUMTL *material);
    void __cdecl NuPrim3DBegin(u32 primitive_type, u32 vertex_format, NUMTL *material, NUMTX *matrix);
    void __cdecl NuPrim2DAddXYZ(f32 x, f32 y, f32 z);
    extern NUFPAR_PC NuFParRecords[4];
    extern char NuFParBuffers[4][0x1000];
    extern NUFPAR_PC *NuFParRecordBase;
    extern char *NuFParBufferBase;
    NUFPAR_PC *__cdecl NuFParOpen(NUFILE file_handle);
    NUFPAR_PC *__cdecl NuFParCreate(char *file_name);
    NUFPAR_PC *__cdecl NuFParCreateGivenFH(char *file_name, NUFILE file_handle);
    void __cdecl NuFParDestroy(NUFPAR_PC *parser);
    i32 __cdecl NuFParGetLine(NUFPAR_PC *parser);
    void __cdecl NuFParGetPos(NUFPAR_PC *parser, NUFPARPOS_PC *position);
    void __cdecl NuFParSetPos(NUFPAR_PC *parser, NUFPARPOS_PC *position);
    void __cdecl NuFParSuspend(NUFPAR_PC *parser);
    void __cdecl NuFParResume(NUFPAR_PC *parser);
    void __cdecl NuFreeFPar(NUFPAR_PC *parser);
    u16 __cdecl UnicodeToIndexFast(VUCHARIDX *map, i32 count, u16 unicode);
    u16 __cdecl NuQFntEncodeUnicodeChar(VUFNT *font, u16 character);
    void __cdecl NuQFntEncodeUnicodeString(VUFNT *font, u16 *text);
    void __cdecl NuQFntUTF8toQCode(VUFNT *font, char *text, u16 *encoded);
    f32 __cdecl NuQFntPrintLenW(VUFNT *font, u16 *text);
    f32 __cdecl NuQFntPrintLenU(VUFNT *font, char *text);
    f32 __cdecl NuQFntHeight(VUFNT *font);
    f32 __cdecl NuQFntBaseline(VUFNT *font);
    void __cdecl NuQFntSetJustifiedTolerances(f32 squash, f32 stretch);
    f32 __cdecl NuFloor(f32 value);
    f32 __cdecl NuFsign(f32 value);
    extern const f32 nu_fsign_minus_one;
    void __cdecl InitOnce(i32 argc, char **argv);
    void __cdecl StartupParseArguments(i32 count, char **arguments);
    i32 __cdecl StartupParseArgument(i32 count, char **arguments);
    void __cdecl StartupOpenDats(const char **names, i32 count, VARIPTR *arena);
    extern NUDATHDR *StartupDatList[7];
    extern i32 StartupDatCount;
    extern const char *StartupDatNames[7];
    extern i32 StartupGameFlag0;
    extern i32 StartupGameFlag1;
    extern i32 SUPERBUFFERSIZE;
    extern VARIPTR original_permbuffer_base;
    extern const char InitOnceAllocSource[15];
    extern char InitOnceCurrentDirectory[1];
    extern u32 InitOnceOptionInput0;
    extern u32 InitOnceOptionInput1;
    extern u32 InitOnceOptionInput2;
    extern GAMESAVE Game;
    extern OPTIONSSAVE TempOptions;
    extern OPTIONSSAVE *Game_OptionsSave;
    extern PLAYERPROGRESS PlayerProgress[8];
    extern MENUFNINFO MenuInfo[100];
    extern MENUFNINFO GameMenuInfo[23];
    extern MENU GameMenu[10];
    extern i32 MenuSFX;
    extern i32 GameMenuLevel;
    extern i32 MenuLanguages;
    extern i32 MenusUsed;
    extern void (*drawslotinfofn)(f32, f32, i32, i32);
    extern void (*drawslotsfn)(MENU *, f32);
    extern char MenuHeader[64];
    extern u8 MENUHEADERR;
    extern u8 MENUHEADERG;
    extern u8 MENUHEADERB;
    extern i32 header_r;
    extern i32 header_g;
    extern i32 header_b;
    extern i32 MenuFadeEnabled;
    extern i32 MenuDrawDropShadows;
    extern NUMTL *MenuFadeMtl;
    extern const char Menu_UsedFormat[15];
    extern const char Menu_OptionsVolumeFormat[10];
    extern const char Menu_ColonSeparator[3];
    extern f32 dme_sy;
    extern i32 menu_flash;
    extern i16 tAUDIOVOLUME, tMUSIC, tON, tOFF, tACCEPT, tBACK;
    extern f32 opts_sfx_wait;
    extern i32 opts_sfx_i;
    extern LEVELDATA_PC *TITLES_LDATA;
    void __cdecl MenuInitialiseEx(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count,
                                  void (*draw_save_slots_info_fn)(f32, f32, i32, i32), i32 is_fade_enabled,
                                  i32 is_shadow_enabled);
    void __cdecl MenuInitialise(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count,
                                void (*draw_save_slots_fn)(MENU *, f32), i32 is_fade_enabled, i32 is_shadow_enabled);
    void __cdecl MenuReset(void);
    void __cdecl MenuEnterNewGame(MENU *menu);
    void __cdecl MenuEnterOptions(MENU *menu);
    void __cdecl MenuUpdateOptions(MENU *menu);
    void __cdecl MenuDrawOptions(MENU *menu);
    void __cdecl MenuExitOptions(MENU *menu);
    void __cdecl RestoreOptions(void);
    f32 __cdecl GameGetMusicVolume(OPTIONSSAVE *options);
    f32 __cdecl GameSetMusicVolume(OPTIONSSAVE *options);
    f32 __cdecl GameSetSoundVolume(OPTIONSSAVE *options);
    void __cdecl SfxCheckMusicOnOff(OPTIONSSAVE *options);
    extern f32 MASTERVOLUME;
    extern f32 AUDIOFADELEVEL;
    void __cdecl SetSoundVolume(f32 volume);
    f32 __cdecl GetSoundVolume(void);
    void __cdecl MenuEnterTitles(MENU *menu);
    i32 __cdecl GetParentMenuID(void);
    extern i32 MenuLoadOccurred;
    extern i32 startnewgame_initiated;
    extern i32 startnewgame;
    extern i32 saveload_autosave;
    void __cdecl MenuSetPlatformState(i32 state);
    extern i32 BGLOAD;
    extern i32 PermDataLoaded;
    extern i32 LOADEROFF;
    extern volatile i32 LoadPerm_LanguageSelect;
    extern NUMTL *legal_mtl;
    extern f32 DEFAULTFRAMETIME;
    extern i32 PAL;
    void __cdecl SetBackgroundMusic(i32 track);
    void __cdecl LoadPerm(void);
    void __cdecl GameAISysSetGame(void);
    void __cdecl GameAISysInit(void);
    extern PART_EMIT_PC part_emits[40];
    extern i32 part_page_on[8];
    extern GAMEPAD_INIT_RECORD GamePadStorage[64];
    void __cdecl GamePads_Init(void);
    extern i32 bgproc_initialized;
    extern CRITICAL_SECTION bgproc_critical_section;
    extern HANDLE bgproc_work_event;
    extern HANDLE bgproc_resume_event;
    extern BGPROCINFO *bgproc_active;
    extern DWORD bgproc_thread_id;
    extern i32 bgproc_frozen;
    extern i32 bgproc_freeze;
    extern const char bgproc_resume_event_name[9];
    extern const char bgproc_work_event_name[11];
    void __cdecl bgProcInit(void);
    void __cdecl bgprocFreeze(void);
    void __cdecl bgprocUnFreeze(void);
    i32 __cdecl bgprocIsFrozen(void);
    i32 __cdecl bgprocIsFreezing(void);
    i32 __cdecl bgProcIsBgThread(void);
    void __cdecl APITransparentInit(void);
    void __cdecl InitShadowLights(void);
    void __cdecl NuKeyboardRead(void);
    f32 __cdecl NuFrameEnd(void);
    i32 __cdecl NuHasError(void);
    void __cdecl NuClearError(void);
    extern i32 ShowingError;
    extern i32 bHaveErr;
    void __cdecl FinishLoop(i32 frames);
    extern i32 finishloop_backdroponly;
    NUDATHDR *__cdecl NuDatOpenEx(const char *filepath, VARIPTR *buf, i32 *size_out, i16 mode);
    NUDATHDR *__cdecl NuDatOpen(const char *filepath, VARIPTR *buf, i32 *size_out);
    i64 __cdecl NuDatCalcPos(NUDATHDR *header, i32 offset);
    i32 __cdecl NuDatFileGetFreeInfo(void);
    i32 __cdecl NuDatFileGetFreeHandleIX(NUDATHDR *header, i32 info_index);
    NUFILE __cdecl NuDatFileOpen(NUDATHDR *header, char *filepath, i32 mode);
    i32 __cdecl NuDatFileRead(NUFILE file, void *buffer, i32 size);
    i32 __cdecl NuDatFileLoadBuffer(NUDATHDR *header, char *filepath, void *buffer, i32 capacity);
    i32 __cdecl NuMemFileRead(NUFILE file, void *buffer, i32 size);
    i64 __cdecl NuMemFileSeek(NUFILE file, i64 offset, i32 origin);
    void __cdecl NuMemFileClose(NUFILE file);
    extern i32 file_criticalsection;
    extern PC_NUDATFILEINFO *unpack_file_info;
    extern NUDATOPENFILEINFO *unpack_file_odi;
    extern i64 unpack_file_pos;
    extern i32 decode_buffer_left;
    extern HANDLE pc_file_handles[32];
    i32 __cdecl NuPSFileOpen(const char *filepath, i32 mode);
    i32 __cdecl NuPSFileClose(i32 handle);
    i32 __cdecl NuPSFileRead(i32 handle, void *buffer, i32 size);
    i32 __cdecl NuPSFileWrite(i32 handle, const void *buffer, i32 size);
    i64 __cdecl NuPSFileLSeek(i32 handle, i64 position, i32 origin);
    extern FILEINFO file_info[33];
    extern FILEBUFF file_buff[4];
    extern PC_NUDATFILEINFO pc_dat_file_infos[20];
    extern PC_NUMEMFILE pc_memfiles[20];
    extern i32 old_line_pos;
    extern i32 nufile_buffering_enabled;
    extern i32 datsys_offset;
    extern i32 nufile_lasterror;
    extern i32 nufile_try_packed;
    extern i32 fpk_err;
    extern i32 file_time_count;
    extern const char nufile_disc_suffix[3];
    extern NUFILE_DEVICE devices[16];
    extern i32 numdevices;
    extern NUFILE_DEVICE host_device;
    extern NUFILE_DEVICE *default_device;
    extern i32 DontDoFileUpperCaseHack;
    NUFILE_DEVICE *__cdecl NuFileGetDeviceFromPath(const char *path);
    NUFILE_DEVICE *__cdecl NuFileAddDevice(NUFILE_DEVICE *device);
    i32 __cdecl NuFileGetCurrentDirectory(char *destination);
    void __cdecl NuFileSetCurrentDirectory(const char *path);
    void __cdecl NuFileGetCurrentDllPath(char *destination);
    void __cdecl NuFileSetCurrentDllDirectory(const char *path);
    i32 __cdecl DEV_FormatName(NUFILE_DEVICE *device, char *destination, char *source, i32 capacity);
    i32 __cdecl DEVHOST_Interrogate(NUFILE_DEVICE *device);
    NUFILE __cdecl NuFileOpenDF(const char *filepath, i32 mode, NUDATHDR *header, i32 flags);
    void __cdecl NuFileClose(NUFILE file);
    i64 __cdecl NuFileSeek(NUFILE file, i64 offset, i32 origin);
    i64 __cdecl NuMemFilePos(NUFILE file);
    i64 __cdecl NuFilePos(NUFILE file);
    i64 __cdecl NuFileSize(char *filepath);
    i64 __cdecl NuFileOpenSize(NUFILE file);
    i64 __cdecl NuDatFileSeek(NUFILE file, i64 offset, i32 origin);
    i32 __cdecl NuMcFileOpenSize(NUFILE file);
    i32 __cdecl NuMcOpen(i32 port, i32 slot, char *filepath, i32 mode, i32 async);
    i32 __cdecl NuMcRead(i32 file, void *destination, i32 size, i32 async);
    i32 __cdecl NuMcClose(i32 file, i32 async);
    i32 __cdecl NuMcSeek(i32 file, i32 offset, i32 origin, i32 async);
    i32 __cdecl NuFileRead(NUFILE file, void *buffer, i32 size);
    i32 __cdecl NuFileLoadBuffer(char *filepath, void *buffer, i32 capacity);
    i32 __cdecl NuFileLoadBufferVP(char *filepath, VARIPTR *current, VARIPTR *end);
    i32 __cdecl NuFilePakGetLastError(void);
    u32 __cdecl NuFilePakGetSize(void *pak);
    i32 __cdecl NuFilePakGetItem(void *pak, char *name);
    i32 __cdecl NuFilePakGetItemInfo(void *pak, i32 index, void **data, i32 *size);
    i32 __cdecl NuFilePakSetItemRequired(void *pak, i32 index, i32 required);
    void __cdecl NuFilePakRelocate(void *pak, void *destination);
    i32 __cdecl NuFilePakCondense(void *pak);
    i32 __cdecl NuFileStatus(NUFILE file);
    void __cdecl NuFileError(void);
    void __cdecl NuFileRetry(void);
    extern NuKeyboardState_PC *nu_input_state;
    extern NuKeyboardOwner_PC nu_keyboard_owner;
    extern u8 nu_keyboard_pages[0x200];
    extern u8 nu_keyboard_page_index;
    extern i32 nu_keyboard_device_flags;
    extern i32 nu_keyboard_key_index;
    extern i32 nu_keyboard_key_count;
    extern i32 nu_keyboard_repeat_countdown;
    extern i32 nu_keyboard_previous_key_index;
    extern i32 nu_keyboard_no_key_state;
    extern HWND PCInputWindow_candidate;
    extern HINSTANCE PCInputInstance_candidate;
    i32 __cdecl NuKeyboardPlatformStartup_candidate(void);
    void __cdecl NuKeyboardCreateWindowState_candidate(HWND window, HINSTANCE instance);
    u8 *__fastcall NuKeyboardPoll(NuKeyboardOwner_PC *owner);
    extern const f32 PCKeyboardDefaultValue_candidate;
    NuKeyboardState_PC *__fastcall NuKeyboardStateInitialize_candidate(NuKeyboardState_PC *state);
    void __fastcall NuKeyboardResetInputRecords_candidate(NuKeyboardState_PC *state);
    void __fastcall NuKeyboardInsertInputRecord_candidate(NuKeyboardState_PC *state, i32 unused_edx, i32 index,
                                                          i32 field_00, i32 field_01, i32 field_02, u16 field_04);
    void __cdecl NuKeyboardApplyActionKeys_candidate(const i16 *keys, i32 count);
    extern i32 PCThreadCriticalSectionCount_candidate;
    extern CRITICAL_SECTION PCThreadCriticalSections_candidate[13];
    extern i32 PCFrameCriticalSectionIndex_candidate;
    extern NUALLOC_RECORD *NuAllocRecordHead;
    extern u32 NuAllocTrackedBytes;
    extern NUALLOC_POOL *NuAllocPool;
    extern u32 NuAllocRecordSerial;
    extern NUALLOC_ARENA *NuAllocArena;
    extern void *NuAllocManager;
    extern void *NuAllocPeakEnd0;
    extern void *NuAllocPeakEnd1;
    extern u32 NuAllocHeaderMetadata[3];
    extern u32 NuAllocTailMetadata[3];
    void *__cdecl NuAlloc(i32 size, const char *file, i32 line);
    void __cdecl NuFree(void *user_data, const char *file, i32 line);
    void __cdecl edpartStartPage(u8 page);
    void __cdecl edpartStopPage(u8 page);
    void __cdecl Parts_Start(WORLDINFO *world);
    void __cdecl Parts_Stop(WORLDINFO *world);
    extern f32 INTROTEXT_Y;
    extern f32 INTROTEXT_SCALE;
    extern void (*Text_GameSetLanguageFn)(i32);
    extern const char Text_TimeHMSC[18];
    extern const char Text_TimeHMS[13];
    extern const char Text_TimeMSC[13];
    extern const char Text_TimeMS[8];
    extern const char Text_TimeSC[8];
    extern const char Text_TimeS[3];
    void __cdecl Text_SetLanguage(i32 language);
    void __cdecl Text_SetLanguage_Game(i32 language);
    void __cdecl Text_MakeTime(f32 time, i32 show_hours, i32 show_minutes, i32 show_centiseconds, char *text);
    i32 __cdecl SplitTextFindNextWS(u8 *text, i32 position);
    void __cdecl FixUpButtonsInFont(VUFNT *game_font, VUFNT *button_font);
    i32 __cdecl NuHGobjReversibleCharacters(i32 enabled);
    i32 __cdecl NuHGobjForceShadowsOnCharacters(i32 enabled);
    extern u8 NuSound3Reverb;
    extern i32 NuSound3LoopHoldTime;
    extern NUDATHDR *NuSoundDat;
    void __cdecl NuSound3SetDat(NUDATHDR *header);
    i32 __cdecl NuSound3SetReverb(i32 enable);
    void __cdecl NuSound3SetLoopHoldTime(f32 time);
    void __cdecl NuSound3FlushLoops(void);
    void __cdecl NuSound3StopRumble(void);
    void __cdecl NuSound3SetRumblePads(void *pad0, void *pad1);
    void __cdecl NuSound3StopStereoStream(i32 index);
    void __cdecl NuSound3PauseStereoStream(i32 index);
    void __cdecl NuSound3ResumeStereoStream(i32 index);
    i32 __cdecl NuSound3SetStreamVolume(i32 index, i32 volume);
    i32 __cdecl NuSound3GetStereoStreamStatus(i32 index);
    f32 __cdecl numusicGetDuckVolume(void);
    extern f32 NuSound3VolumeTable[0x4000];
    extern i32 NuSound3VolumeOffset;
    extern GAMEANIMPROGRESS gameanimsysprogress;
    void *__cdecl GameBufferAlloc(VARIPTR *buf, VARIPTR *end, i32 size);
    void __cdecl GameAnimSys_AllocateLevelProgressData(VARIPTR *buf, VARIPTR *end, i32 capacity, i32 level_count);
    void __cdecl GameAnimSys_StoreProgress(GAMEANIMSYS *system, i32 index);
    u8 *__cdecl GameAnimSys_GetProgressData(i32 index);
    void __cdecl GameAnimSys_ClearProgress(i32 index);
    GAMEANIMSYS *__cdecl GameAnimSys_Create(VARIPTR *buf, VARIPTR *end);
    GAMEANIMSET *__cdecl GameAnimSet_Create(VARIPTR *buf, VARIPTR *end, GAMEANIMOBJPOOL *pool, GAMEANIMSYS *system);
    void __cdecl GameAnimSet_AddToSystemList(GAMEANIMSET *set);
    void __cdecl GameAnimSet_EvaluateState(GAMEANIMSET *set);
    void __cdecl GameAnimSet_RemoveFromSystemList(GAMEANIMSET *set);
    void __cdecl GameAnimSys_ReStoreProgress(GAMEANIMSYS *system, i32 index);
    GAMEANIMOBJPOOL *__cdecl GameAnimSet_CreateObjectPool(VARIPTR *buf, VARIPTR *end, i32 object_data_size,
                                                          i32 capacity);
    void __cdecl GameAnimSet_RemoveObject(GAMEANIMSET *set, GAMEANIMOBJ *object);
    void __cdecl GameAnimSet_RemoveAllObjects(GAMEANIMSET *set);
    void __cdecl GameAnimSet_SetTFactor(GAMEANIMSET *set, f32 factor);
    void __cdecl GameAnimSet_ScaleFParam1(GAMEANIMSET *set, f32 scale);
    i32 __cdecl GameAnimSet_IsAnimationReset(GAMEANIMSET *set);
    i32 __cdecl GameAnimSet_Reset(GAMEANIMSET *set);
    i32 __cdecl GameAnimSet_Play(GAMEANIMSET *set, f32 speed, i32 evaluate_state);
    i32 __cdecl GameAnimSet_Stop(GAMEANIMSET *set);
    i32 __cdecl GameAnimSet_SetRepeating(GAMEANIMSET *set, i32 repeating);
    i32 __cdecl GameAnimSet_JumpToEnd(GAMEANIMSET *set);
    i32 __cdecl GameAnimSet_JumpToStart(GAMEANIMSET *set);
    void __cdecl GameAnimSet_JumpToAnimPos(GAMEANIMSET *set, f32 position);
    i32 __cdecl GameAnimSet_Playing(GAMEANIMSET *set);
    f32 __cdecl GameAnimSet_GetCurrentFrame(GAMEANIMSET *set);
    f32 __cdecl GameAnimSet_GetAnimPos(GAMEANIMOBJ *object);
    void __cdecl GameAnimSet_SetAnimPos(GAMEANIMOBJ *object, f32 position);
    f32 __cdecl GameAnimSet_GetCompletionRatio(GAMEANIMSET *set);
    extern AREA_DATA_PC *ADataList;
    extern AREA_DATA_PC *LastAData;
    extern AREA_DATA_PC *HUB_ADATA;
    extern LEVELDATA_PC *LDataList;
    extern i32 MAXLDATA;
    extern LEVELDATA_PC *NEWGAME_LDATA;
    extern LEVELDATA_PC *LOADGAME_LDATA;
    extern LEVELDATA_PC *LastLData;
    extern LEVELDATA_PC *NewLData;
    extern LEVELDATA_PC *HUB_LDATA;
    extern LEVELDATA_PC *STATUS_LDATA;
    extern LEVELDATA_PC *CREDITS_LDATA;
    extern f32 MainRenderTime;
    extern f32 MainRenderTargetTime;
    extern f32 WaitingForLevelTime;
    extern f32 WaitingForCharacterTime;
    extern f32 MAXFRAMETIME;
    extern i32 save_paused;
    extern i32 netclient;
    extern i32 NOSOUND;
    extern volatile i32 nuvideo_global_vbcnt;
    extern i32 nurndr_tritot_this_frame;
    extern i32 openlevels;
    extern i32 PlayerID[2];
    extern i16 PlayerList[8];
    extern APICHARACTERMODELLIST Hub_ModelList[6];
    void __cdecl Hub_MakeModelList(void);
    extern MAINFRAMECOUNTERS MainFrameCounters;
    extern i32 do_multiframe_update;
    extern f32 FRAMETIME;
    extern void *debris_cutoff_camera_vec;
    extern i32 debris_render_group;
    void __cdecl DebrisRegisterCutoffCameraVec(void *position);
    void __cdecl DebrisSetRenderGroup(i32 group);
    void __cdecl ResetFrameCounters(void);
    void __cdecl InitCables(WORLDINFO *world);
    void __cdecl InitSnakes(WORLDINFO *world);
    i32 __cdecl NuSpecialFind(NUGSCN *scene, NUSPECIALHANDLE *handle, char *name, i32 flags);
    extern u8 cables[0xf60];
    extern f32 default_targetmomf, targetmomf;
    extern f32 default_ownermomf, ownermomf;
    extern f32 default_max_cable_length, max_cable_length;
    extern f32 default_tow_length, tow_length;
    extern AREA_DATA_PC *GUNSHIP_ADATA;
    void __cdecl UpdateFrameCounters(void);
    void __cdecl ResetTimer(TIMER *timer, f32 reset_time);
    void __cdecl UpdateTimer(TIMER *timer);
    void __cdecl WorldInfo_ReArrangeBuffers(i32 area1, i32 area2);
    extern f32 PanelProjectionX;
    extern f32 PanelProjectionY;
    extern f32 PanelWideWidth;
    extern f32 PanelWideHeight;
    extern f32 PANEL3DMULX;
    extern f32 PANEL3DMULY;
    void __cdecl InitPanel(i32 mode);
    void __cdecl RotateGameMatrix(NUMTX *matrix, i32 order, u16 x, u16 y, u16 z);
    extern LANGUAGEDATA Game_LanguageList[7];
    extern LANGUAGEDATA Text_LanguageList_Default[7];
    extern LANGUAGEDATA *Text_LanguageList;
    extern i32 LANGUAGECOUNT;
    extern i32 Text_Language;
    extern void *Text_AppFont;
    extern char **TTab_Original;
    extern char **TTab;
    extern i32 Text_MaxStrings_Overall;
    extern u32 *Text_StringBits;
    extern char *Text_ErrorString;
    extern char *Text_PathEnglish;
    extern char *Text_PathFrench;
    extern char *Text_PathGerman;
    extern char *Text_PathItalian;
    extern char *Text_PathSpanish;
    extern char *Text_PathDanish;
    extern char *Text_PathJapanese;
    extern i32 INTROTEXT_ID;
    extern char *txt_NULL;
    extern char *txt_UNKNOWN;
    extern char *apitxt_ENGLISH;
    extern char *apitxt_FRENCH;
    extern char *apitxt_GERMAN;
    extern char *apitxt_ITALIAN;
    extern char *apitxt_SPANISH;
    extern char *apitxt_DANISH;
    extern i16 tNULL;
    extern i16 tUNKNOWN;
    extern i16 tENGLISH;
    extern i16 tFRENCH;
    extern i16 tGERMAN;
    extern i16 tITALIAN;
    extern i16 tSPANISH;
    extern i16 tDANISH;
    void __cdecl Text_InitLanguageList(LANGUAGEDATA *language_list);
    char *__cdecl Text_GetLanguagePath(i32 language);
    void *__cdecl Text_IsFontLoaded(void);
    void __cdecl Text_InitStringTable(i32 count, VARIPTR *buf, VARIPTR *buf_end);
    i32 __cdecl Text_GetMaxOverallStrings(void);
    void __cdecl Text_InitTable(TEXTENTRY *entry, i32 first, i32 last);
    void __cdecl Text_InsertCommasIntoNumber(char *number, char *text, i32 length);
    void __cdecl Text_MakeScore(u32 score, char *text);
    i32 __cdecl Text_StripComments(char *text, char *dest, i32 separators);
    void __cdecl IntroText_SetTextID(i32 id);
    void __cdecl Text_InitDefaultStrings(void);
    extern void (*buttonmapfn)(char *, char *);
    extern i32 smarttext_fwn;
    extern VUFNT *SmartTextFont;
    extern VUFNT *QFont2D;
    extern VUFNT *QFont3DZ;
    extern f32 QFONTSCALEX;
    extern f32 STCOORDSCALE;
    extern const char Text_WibbleCodeword[7];
    extern const char Text_CrossCodeword[6];
    void __cdecl Text_DecodeButtons(char *source, char *destination);
    void __cdecl TextRegisterButtonMapFn(void (*fn)(char *, char *));
    void __cdecl SmartTextSetFWNMode(i32 mode);
    void __cdecl SmartTextSetFont(VUFNT *font);
    void __cdecl SetGameFont(VUFNT *font);
    void __cdecl Set3DGameFont(VUFNT *font);
    void __cdecl SmartTextSetWidescreen(f32 font_scale_x, f32 coordinate_scale);
    void __cdecl SmartTextGetWidescreen(f32 *font_scale_x, f32 *coordinate_scale);
    i32 __cdecl NuStrLen(const char *text);
    i32 __cdecl NuStrICmp(const char *a, const char *b);
    char *__cdecl NuStrIStr(char *text, char *substring);
    void __cdecl NuLinkedListAppend(NULISTHDR *list, NULISTLNK *node);
    void __cdecl NuLinkedListRemove(NULISTHDR *list, NULISTLNK *node);
    NULISTLNK *__cdecl NuLinkedListGetHead(NULISTHDR *list);
    NULISTLNK *__cdecl NuLinkedListGetNext(NULISTHDR *list, NULISTLNK *node);
    void __cdecl ResetGizAIMessageSys(GIZAIMESSAGESYS *sys);
    void __cdecl ClearGizAIMessageSys(GIZAIMESSAGESYS *sys);
    GIZAIMESSAGE *__cdecl CheckGizAIMessage(GIZAIMESSAGESYS *sys, const char *name, GIZAIMESSAGE *message);
    GIZAIMESSAGE *__cdecl SetGizAIMessage(GIZAIMESSAGESYS *sys, const char *name, f32 value, GIZAIMESSAGE *message);
    f32 __cdecl GetGizAIMessage(GIZAIMESSAGESYS *sys, const char *name, GIZAIMESSAGE *message);
    GIZAIMESSAGE *__cdecl QueryGizAIMessage(GIZAIMESSAGESYS *sys, GIZAIMESSAGE *message);
    char *__cdecl GizAIMessage_GetName(GIZAIMESSAGE *message);
    extern char gizaimessage_prefix[5];
    extern const char GizAIMessage_NameFormat[3];
    extern const char GizAIMessage_PrefixedNameFormat[5];
    extern GIZAIMESSAGESYS *gizaimessagesys;
    void *__cdecl AISysBufferAlloc(VARIPTR *cursor, VARIPTR *end, u32 size);
    GIZAIMESSAGESYS *__cdecl CreateGizAIMessageSys(VARIPTR *cursor, VARIPTR *end, i32 count);
    i32 __cdecl NuStrNICmp(const char *a, const char *b, i32 n);
    void __cdecl NuStrUpr(char *dst, const char *src);
    void __cdecl NuStrLwr(char *dst, const char *src);
    i32 __cdecl NuHexStringToI(char *string);
    u8 *__cdecl NuUnicodeCharFromUTF8(u16 *out, u8 *text);
    u8 *__cdecl NuUTF8CharFromUnicode(u8 *out, u16 codepoint);
    i32 __cdecl ishexnum(char value);
    i32 __cdecl hexcodetoint(char value);
    void __cdecl NuStrCat(char *str, const char *ext);
    i32 __cdecl NuStrNCat(char *str, const char *ext, i32 n);
    i32 __cdecl NuStrCpy(char *dst, const char *src);
    i32 __cdecl NuStrNCpy(char *dst, const char *src, i32 n);
    char *__cdecl NuStrChr(char *src, char c);
    char *__cdecl NuStrRChr(char *src, char c);
    i32 __cdecl NuStrLenU(u8 *str);
    i32 __cdecl NuStrCmp(const char *a, const char *b);
    i32 __cdecl NuStrNCmp(const char *a, const char *b, i32 n);
    f32 __cdecl NuFmod(f32 a, f32 b);
    void __cdecl NuMtxSetTranslation(NUMTX *matrix, NUVEC *translation);
    void __cdecl NuMtxSetScale(NUMTX *matrix, NUVEC *scale);
    void __cdecl NuMtxTranslate(NUMTX *matrix, NUVEC *translation);
    void __cdecl NuMtxTranslateNeg(NUMTX *matrix, NUVEC *translation);
    void __cdecl NuMtxPreTranslate(NUMTX *matrix, NUVEC *translation);
    void __cdecl NuMtxScale(NUMTX *matrix, NUVEC *scale);
    void __cdecl NuMtxScaleU(NUMTX *matrix, f32 scale);
    void __cdecl NuMtxPreScaleU(NUMTX *matrix, f32 scale);
    void __cdecl NuMtxPreScale(NUMTX *matrix, NUVEC *scale);
    void __cdecl NuMtxPreScaleX(NUMTX *matrix, f32 scale_x);
    void __cdecl NuMtxGetXAxis(NUMTX *matrix, NUVEC *axis);
    void __cdecl NuMtxGetYAxis(NUMTX *matrix, NUVEC *axis);
    void __cdecl NuMtxGetZAxis(NUMTX *matrix, NUVEC *axis);
    void __cdecl NuMtxGetTranslation(NUMTX *matrix, NUVEC *translation);
    void __cdecl NuVecAdd(NUVEC *dst, NUVEC *a, NUVEC *b);
    void __cdecl NuVecSub(NUVEC *dst, NUVEC *a, NUVEC *b);
    void __cdecl NuVecScale(NUVEC *dst, NUVEC *src, f32 scale);
    void __cdecl NuVecAddScale(NUVEC *dst, NUVEC *a, NUVEC *b, f32 scale);
    f32 __cdecl NuVecDot(NUVEC *a, NUVEC *b);
    void __cdecl NuVecNeg(NUVEC *dst, NUVEC *src);
    void __cdecl NuVecCross(NUVEC *dst, NUVEC *a, NUVEC *b);
    void __cdecl NuVecMax(NUVEC *dst, NUVEC *a, NUVEC *b);
    void __cdecl NuVecMin(NUVEC *dst, NUVEC *a, NUVEC *b);
    f32 __cdecl NuVecMagSqr(NUVEC *vec);
    void __cdecl NuVecLerp(NUVEC *dst, NUVEC *a, NUVEC *b, f32 t);
    void __cdecl NuVecMtxRotate(NUVEC *dst, NUVEC *src, NUMTX *matrix);
    void __cdecl NuVecMtxTransform(NUVEC *dst, NUVEC *src, NUMTX *matrix);
    void __cdecl NuVecInvMtxRotate(NUVEC *dst, NUVEC *src, NUMTX *matrix);
    void __cdecl NuVecInvMtxTransform(NUVEC *dst, NUVEC *src, NUMTX *matrix);
    NUMTX *__cdecl NuSpecialGetMtx(NUSPECIALHANDLE *special);
    void __cdecl NuSpecialSetMtx(NUSPECIALHANDLE *special, const NUMTX *matrix);
    i32 __cdecl NuSpecialExistsFn(NUSPECIALHANDLE *special);
    i32 __cdecl NuSpecialTestAnim(NUSPECIALHANDLE *special);
    NUVEC *__cdecl NuSpecialGetPos(NUSPECIALHANDLE *special);
    NUVEC *__cdecl NuSpecialGetDrawPos(NUSPECIALHANDLE *special);
    char *__cdecl NuSpecialGetName(NUSPECIALHANDLE *special);
    NUMTX *__cdecl NuSpecialGetDrawMtx(NUSPECIALHANDLE *special);
    NUMTX *__cdecl NuSpecialGetInstanceMtx(NUSPECIALHANDLE *special);
    void *__cdecl NuSpecialGetInstAnim(NUSPECIALHANDLE *special);
    void __cdecl NuSpecialSetCollision(NUSPECIALHANDLE *special, i32 enabled);
    i32 __cdecl NuSpecialGetCollision(NUSPECIALHANDLE *special);
    void __cdecl NuSpecialSetNoVisiTest(NUSPECIALHANDLE *special, i32 enabled);
    i32 __cdecl NuSpecialGetNoVisiTestFn(NUSPECIALHANDLE *special);
    extern f32 nuspecial_const_alpha;
    extern NUVEC nuspecial_const_tint;
    extern i32 nuspecial_const_alpha_enabled;
    extern i32 nuspecial_const_tint_enabled;
    extern i32 nuspecial_draw_state;
    void __cdecl NuSpecialConstAlpha(i32 enabled, f32 alpha);
    void __cdecl NuSpecialConstTint(i32 enabled, const NUVEC *tint);
    void __cdecl NuSpecialGetNext(NUSPECIALHANDLE *special);
    i32 __cdecl NuSpecialGetNumSpecials(NUGSCN *scene);
    i32 __cdecl NuSpecialGetOnScreenFn(NUSPECIALHANDLE *special);
    i32 __cdecl NuSpecialGetVisibilityFn(NUSPECIALHANDLE *special);
    extern i32 nu_water_reset_state;
    extern i32 nu_water_reset_status;
    void __cdecl NuWaterReset(void);
    extern u8 fstackmem[NU_PORTAL_STACK_SIZE];
    extern VARIPTR fstack;
    extern VARIPTR fstack_top;
    extern VARIPTR fstack_limit;
    void __cdecl NuPortalInit(void);
    extern void (*nuapi_endframe_callbackfn)(void);
    void __cdecl NuRegisterEndFrameCallBackFn(void (*callback)(void));
    void __cdecl NuMouseRead(void);
    void __cdecl NuPause(i32 paused);
    i32 __cdecl NuKeyToAscii(u32 key, i32 shifted);
    extern WORLDINFO WorldInfo[2];
    extern WORLDINFO *WORLD;
    extern WORLDINFO *LWORLD;
    extern u16 ang[513];
    extern f32 NuTrigTable[32768];
    void __cdecl NuTrigInit(void);
    void __cdecl NuMtxRotateX(NUMTX *matrix, NUANG angle);
    void __cdecl NuMtxRotateY(NUMTX *matrix, NUANG angle);
    void __cdecl NuMtxRotateZ(NUMTX *matrix, NUANG angle);
    void __cdecl NuMtxTransposeR(NUMTX *out, NUMTX *source);
    void __cdecl NuMtxTranspose(NUMTX *out, NUMTX *source);
    extern NUCAMERA global_camera;
    extern NUVEC cam_axes;
    void __cdecl NuCameraGet(NUCAMERA *camera);
    NUMTX *__cdecl NuCameraGetMtx(void);
    void __cdecl NuCameraSetAxes(NUVEC *axes);
    void __cdecl NuCameraGetAxes(NUVEC *axes);
    NUCAMERA *__cdecl NuCameraGetCam(void);
    NUMTX *__cdecl NuCameraGetViewMtx(void);
    NUMTX *__cdecl NuCameraGetProjectionMtx(void);
    NUMTX *__cdecl NuCameraGetClippingMtx(void);
    NUMTX *__cdecl NuCameraGetScalingMtx(void);
    NUMTX *__cdecl NuCameraGetVPCSMtx(void);
    NUMTX *__cdecl NuCameraGetVPCMtx(void);
    NUMTX *__cdecl NuCameraGetVPMtx(void);
    NUMTX *__cdecl NuCameraGetPCMtx(void);
    void __cdecl NuCameraGetClipMtx(NUMTX *viewport, NUMTX *scissor);
    NUMTX *__cdecl NuCameraGetClipPlanes(void);
    void __cdecl NuMtxCalcCheapFaceOn(NUMTX *matrix, NUVEC *position);
    void __cdecl NuMtxCalcDebrisFaceOn(NUMTX *matrix);
    extern NUMTX vmtx;
    extern NUMTX pmtx;
    extern NUMTX cmtx;
    extern NUMTX smtx;
    extern NUMTX vpmtx;
    extern NUMTX vpc_sci_mtx;
    extern NUMTX vpc_vport_mtx;
    extern NUMTX pc_vport_mtx;
    extern NUMTX vpsmtx;
    extern NUMTX clip_planes;
    extern u32 crctab[256];
    extern const u8 KeyStructSizes[16];
    extern u16 bad_equiv[8][2];
    extern i32 SkinFlipTab[8];
    extern i32 TrooperStepFrames[4];
    extern f32 trooperspeedtab[4];
    extern f32 vehicle_timebase_dist[4];
    extern i32 vehicle_timebase_nframes[4];
    extern NUKEYCHAR NuKeyChar[65];
    extern i32 GAMEDEMO;
    extern i32 PLAYERCOUNT;
    extern i32 Level;
    extern i32 Area;
    extern i32 last_area;
    extern i32 NextArea_FreePlay;
    extern i32 FreePlay;
    extern i32 makefreeplaymodellist;
    extern i32 SuperStory;
    extern i32 reset_area;
    extern i32 PlayTrailer;
    extern i32 loadareacharacters_loadedlevel;
    extern i32 level_already_loaded;
    extern i32 next_level;
    extern volatile i32 abort_load;
    extern i32 dagobah_training;
    extern i32 party_cant_be_under_cover;
    extern i32 ResetOldFStop;
    extern i32 new_level_from_menu;
    extern i32 reset_restart;
    extern i32 no_more_loads;
    extern i32 BonusWinner;
    extern i32 BonusCoinTotal;
    extern i32 NewMode;
    extern i32 reset_load;
    extern i32 newlevel_resumecutaudio;
    extern i32 frameout;
    extern i32 other_level;
    extern volatile i32 waiting_for_level;
    extern i32 gone_through_door_to_new_level;
    extern i32 waiting_for_new_level;
    extern i32 screendump;
    extern i32 MiniCutCam;
    extern i32 pause_rndr_on;
    extern i32 clear_screen_onstill;
    extern i32 HubMainRenderTimeHack;
    extern i32 peak_poly_count;
    extern i32 poly_count;
    extern i32 CUTSTOPGAME;
    extern i32 Paused;
    extern i32 FinishLoop_On;
    extern i32 LevelChange;
    extern i32 LevelChangesInArea;
    extern i32 Arcade;
    extern i32 hub_from_arcade;
    extern i32 UsePlayerList;
    extern i32 Door_UseCutCam;
    extern i32 waiting_for_character;
    extern i32 ChallengeMode;
    extern i32 cubeEdgeIndices[12][2];
    extern i32 cubeEdgeTopologies[12][2];
    extern MSG win_message;
    i32 __cdecl NuWindowsProcessMessages(void);
    extern i32 permbuffer_size;
    extern i32 CHARACTERBUFFERSIZE;
    extern i32 EDITBUFFERENDSIZE;
    extern VARIPTR permbuffer_base, permbuffer_ptr, permbuffer_end;
    extern VARIPTR characterbuffer_base, characterbuffer_ptr, characterbuffer_end;
    extern VARIPTR superbuffer_base, superbuffer_ptr, superbuffer_end, editbuffer_end;
    void __cdecl StartPerm(void);
    void __cdecl EndPerm(void);
    void __cdecl WorldInfo_InitOnce(void);
    WORLDINFO *__cdecl WorldInfo_CurrentlyActive(void);
    WORLDINFO *__cdecl WorldInfo_CurrentlyLoading(void);
    i32 __cdecl WorldInfo_OtherLevel(WORLDINFO *world);
    void __cdecl WorldInfo_UpdateRoomVisibility(WORLDINFO *world, i32 param);
    void __cdecl edGraEnableTerrainSwap(void);
    void __cdecl edGraDisableTerrainSwap(void);
    void __cdecl edGraInitTerrainSwapProtection(void);
    int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR commandLine, int show);

    extern u32 fseed;

    i32 __cdecl NuRand(NURAND *rand);
    f32 __cdecl NuRandFloat(void);
    f32 __cdecl NuRandFloatSeeded(u32 *seed);
    u32 __cdecl NuRandInt(void);
    u32 __cdecl NuRandIntSeeded(u32 *seed);
    void __cdecl NuRandSeed(u32 seed);
    void __cdecl NuRandSetSeed(NURAND *rand, i32 seed);
    u32 __cdecl NuRandGetSeed(void);
    f32 __cdecl NuFloatRand(NURAND *rand);

    extern u32 NuWindQS;
    extern const f64 nu_wind_phase_scale;
    extern const f64 nu_wind_scaled_time_multiplier;
    void __cdecl NuWindAnimate(NUWIND *wind, f32 frame_time);
    extern u32 partseed;
    extern RTLSET_PC *PartRTL;
    void __cdecl SetPartRTLSet(RTLSET_PC *rtl_set);
    extern NUGSCN *edbits_base_scene;
    extern void *edbits_base_terrain;
    extern i32 NuSound3Initialized;
    extern TERRAIN_SURFACE_PC TerSurface[32];
    extern EDANIM_PARAM_PC AnimParams[64];
    extern u16 SfxBits[100];
    extern u16 GlobalSfxBits[100];
    extern i32 edanim_page_on[8];
    extern i32 edanim_page_used[8];
    extern NUGSCN *edanim_page_scene[8];
    extern i32 edanim_next_param;
    extern i32 edanim_params_used;
    extern const char LevelConfigKeywordFile[5];
    extern const char LevelConfigKeywordDir[4];
    extern const char LevelConfigKeywordStart[12];
    extern const char LevelConfigKeywordEnd[10];
    extern const char LevelConfigKeywordTest[11];
    extern const char LevelConfigKeywordIntro[12];
    extern const char LevelConfigKeywordMidtro[13];
    extern const char LevelConfigKeywordCutscene[15];
    extern const char LevelConfigKeywordOutro[12];
    extern const char LevelConfigKeywordStatus[13];
    extern const char LevelConfigKeywordNewGame[14];
    extern const char LevelConfigKeywordLoadGame[15];
    extern const f32 LevelConfigFarDistance;
    extern NUMTL *numtl_defaultmtl2d;
    extern NUMTL *numtl_defaultmtl3d;
    extern VARIPTR *NuDisplayListStreamCursor;
    extern NUGLOBALRNDRSTATE_PC render_state;
    extern u8 NuPrimNeedsOverbrightening;
    extern NUMTX numtx_identity;
    void __cdecl edbitsRegisterBaseScene(NUGSCN *scene);
    void __cdecl edanimRegisterBaseScene(NUGSCN *scene);
    void __cdecl edbitsRegisterBaseTerrain(void *terrain);
    void __cdecl WorldInfo_Register(WORLDINFO *world);
    extern u32 debrisseed;
    extern f32 debris_time_increment;
    extern i32 DebrisCutSceneMode;
    void __cdecl DebrisSetCutSceneMode(i32 enabled);
    u32 __cdecl NuWindRand(void);
    void __cdecl NuPartSetSeed(i32 seed);
    u32 __cdecl NuPartGetSeed(void);
    void __cdecl DebrisSetSeed(i32 seed);
    u32 __cdecl DebrisGetSeed(void);
    void __cdecl DebrisSetTimeIncrement(f32 increment);

    NUANG __cdecl NuAngAdd(NUANG a, NUANG b);
    NUANG __cdecl NuAngSub(NUANG a, NUANG b);
    void __cdecl NuAng2AltSol(NUANG *out_x, NUANG *out_y, NUANG *out_z, NUANG x, NUANG y, NUANG z);

#ifdef __cplusplus
}

// These retain C++ linkage and are not exposed as C declarations.
extern i32 qseed;
i32 __cdecl qrand(void);
i32 __cdecl getqseed(void);
extern i32 GAMERAND;
extern i32 NetPaused;
void __cdecl ClearPause(void);
void __cdecl ResetSeeds(void);
extern void *pNuCam;
extern i32 app_tbgameset;
extern i32 app_tbplayerset;
extern i32 app_tbaiset;
extern i32 app_tbdrawset;

struct nusound_filename_info_s;
class NuMusic {
  public:
    class Track {
      public:
        u8 unknown_0000[8];
        char *ident;
        i32 file_indexes[2];
        u32 clazz;
        f32 *entry_times;
        i32 entry_count;
        i32 entry_index;
        f32 start_offset;
        f32 pitch;
        f32 duck_volume;
        f32 duck_fade;
        f32 attenuation;
        u32 flags;
    };
    class Voice {
      public:
        i32 stream_index;
        Track *tracks[2];
        i32 track_sub[2];
        i32 track_index;
        i32 status;
        f32 gain;
        f32 volume;
        f32 last_volume;
        f32 fade_rate;
        f32 play_time;
        u8 flags;
        u8 unknown_0031[3];
    };
    struct Album {
        char *name;
        Track *tracks_source;
        i32 tracks_count;
        Track *tracks[6];
        Track *GetTrack(u32 class_mask);
        i32 GetTracks(u32 class_mask, Track **out_tracks);
        void Initialise(void);
    };
    Album *albums;
    i32 album_count;
    Track *extra_tracks;
    i32 extra_track_count;
    u8 unknown_0010[0x120];
    Voice voices[2];
    f32 master_volume;
    f32 fader_current;
    f32 fader_target;
    f32 fader_rate;
    u8 unknown_01a8[4];
    f32 duck_current;
    u8 unknown_01b0[4];
    f32 class_volumes[6];
    u8 unknown_01cc[4];
    i32 track_index;
    u8 unknown_01d4[4];
    Album *album;
    nusound_filename_info_s *fileinfo;
    i32 file_count;

    void SetFader(f32 target, f32 fade_seconds);
    void SetMasterVolume(f32 volume);
    i32 GetAlbumHandle(const char *name);
    static i32 ClassToIX(u32 class_mask);
    void SetClassVolume(u32 class_mask, f32 volume);
    void GetSoundFiles(nusound_filename_info_s **files, i32 *count);
    i32 PauseTrack(u32 class_mask);
    i32 ResumeTrack(u32 class_mask);
    bool SelectTrackByHandle(u32 class_mask, i32 handle);
    i32 GetTrackHandle(u32 class_mask, const char *name);
    i32 GetStatus(u32 class_mask, i32 *per_class_status);
    bool SelectTrack(u32 class_mask, const char *name);
    Voice *FindVoiceByTrack(Track *track);
    Voice *FindVoiceByClass(u32 class_mask);
    Voice *FindVoiceByClassAndStatus(u32 class_mask, u32 status);
    Voice *FindIdleVoice(void);
    i32 StopAll(i32 mode);
    i32 PlayTrack(u32 class_mask);
    i32 PlayTrack(u32 class_mask, u32 unused);
    void Process(f32 delta);
};
STATIC_ASSERT(sizeof(NuMusic::Track) == 0x3c);
STATIC_ASSERT(offsetof(NuMusic::Track, ident) == 0x08);
STATIC_ASSERT(offsetof(NuMusic::Track, file_indexes) == 0x0c);
STATIC_ASSERT(offsetof(NuMusic::Track, clazz) == 0x14);
STATIC_ASSERT(offsetof(NuMusic::Track, start_offset) == 0x24);
STATIC_ASSERT(offsetof(NuMusic::Track, entry_times) == 0x18);
STATIC_ASSERT(offsetof(NuMusic::Track, entry_count) == 0x1c);
STATIC_ASSERT(offsetof(NuMusic::Track, entry_index) == 0x20);
STATIC_ASSERT(offsetof(NuMusic::Track, flags) == 0x38);
STATIC_ASSERT(sizeof(NuMusic::Voice) == 0x34);
STATIC_ASSERT(offsetof(NuMusic::Voice, tracks) == 0x04);
STATIC_ASSERT(offsetof(NuMusic::Voice, track_index) == 0x14);
STATIC_ASSERT(offsetof(NuMusic::Voice, status) == 0x18);
STATIC_ASSERT(offsetof(NuMusic::Voice, track_sub) == 0x0c);
STATIC_ASSERT(offsetof(NuMusic::Voice, gain) == 0x1c);
STATIC_ASSERT(offsetof(NuMusic::Voice, play_time) == 0x2c);
STATIC_ASSERT(offsetof(NuMusic::Voice, flags) == 0x30);
STATIC_ASSERT(sizeof(NuMusic::Album) == 0x24);
STATIC_ASSERT(offsetof(NuMusic::Album, name) == 0);
STATIC_ASSERT(offsetof(NuMusic::Album, tracks) == 0x0c);
STATIC_ASSERT(offsetof(NuMusic, albums) == 0);
STATIC_ASSERT(offsetof(NuMusic, album_count) == 4);
STATIC_ASSERT(offsetof(NuMusic, extra_tracks) == 0x08);
STATIC_ASSERT(offsetof(NuMusic, extra_track_count) == 0x0c);
STATIC_ASSERT(offsetof(NuMusic, voices) == 0x130);
STATIC_ASSERT(offsetof(NuMusic, master_volume) == 0x198);
STATIC_ASSERT(offsetof(NuMusic, fader_current) == 0x19c);
STATIC_ASSERT(offsetof(NuMusic, fader_target) == 0x1a0);
STATIC_ASSERT(offsetof(NuMusic, fader_rate) == 0x1a4);
STATIC_ASSERT(offsetof(NuMusic, duck_current) == 0x1ac);
STATIC_ASSERT(offsetof(NuMusic, class_volumes) == 0x1b4);
STATIC_ASSERT(offsetof(NuMusic, track_index) == 0x1d0);
STATIC_ASSERT(offsetof(NuMusic, album) == 0x1d8);
STATIC_ASSERT(offsetof(NuMusic, fileinfo) == 0x1dc);
STATIC_ASSERT(offsetof(NuMusic, file_count) == 0x1e0);
extern NuMusic music_man;
extern NuMusic *the_music_player;
void SetAudioFadeLevel(f32 level);
f32 GetAudioFadeLevel(void);
#endif

#endif
