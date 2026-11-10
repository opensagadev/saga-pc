#include "saga.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct numemblklink_s {
    struct numemblklink_s *next;
} NUMEMBLKLINK;

typedef struct numemblk_s {
    NUMEMBLKLINK *free_list;
    u32 stride;
    i32 capacity;
    i16 flags;
    u16 free_count;
} NUMEMBLK;

STATIC_ASSERT(sizeof(NUMEMBLKLINK) == 4);
STATIC_ASSERT(sizeof(NUMEMBLK) == 16);
STATIC_ASSERT(offsetof(NUMEMBLK, free_list) == 0);
STATIC_ASSERT(offsetof(NUMEMBLK, stride) == 4);
STATIC_ASSERT(offsetof(NUMEMBLK, capacity) == 8);
STATIC_ASSERT(offsetof(NUMEMBLK, flags) == 12);
STATIC_ASSERT(offsetof(NUMEMBLK, free_count) == 14);

static i32 decode_buffer_left = 0;
static i32 decode_buffer_pos = 0;
static u8 decode_buffer[262144] = {0};
/* Preserve the existing canonical working view; this is not a PC capacity claim. */
static u8 read_buffer[0x40000] = {0};
static i32 read_buffer_size = 0;
static i32 read_buffer_decoded_size = 0;

extern i32 file_criticalsection;
extern PC_NUDATFILEINFO *unpack_file_info;
extern NUDATOPENFILEINFO *unpack_file_odi;
extern i64 unpack_file_pos;

extern void __cdecl PCThreadCriticalSectionEnter_candidate(i32 section_index);
extern void __cdecl PCThreadCriticalSectionLeave_candidate(i32 section_index);

extern i32 numdevices;
extern NUFILE_DEVICE devices[16];
extern i32 DontDoFileUpperCaseHack;

void __cdecl NuDatFileMissingPc(char *filepath);
void __cdecl NuRDPIVariableErrorPc(char *name);

// Fetch one control bit; the marker bit refills the buffer from the next input byte.
#define NUPP_GETBIT()                                                                                                  \
    bits += bits;                                                                                                      \
    bit = (bits >> 8) & 1;                                                                                             \
    bits &= 0xff;                                                                                                      \
    if (bits == 0) {                                                                                                   \
        bits = bit + *src++ * 2;                                                                                       \
        bit = (bits >> 8) & 1;                                                                                         \
    }

struct NUPPHEADER_PC {
    u32 magic;
    u32 unpacked_size;
    u32 packed_size;
};
STATIC_ASSERT(sizeof(struct NUPPHEADER_PC) == 12);
#define NUPP_SWAP32(value)                                                                                             \
    (((value) >> 24) | (((value) & 0x00ff0000) >> 8) | (((value) & 0x0000ff00) << 8) | ((value) << 24))

extern const char SceneFileExtensionPc[5];
extern const char NuFileExt_dds_PC[5];
extern const char NuFileExt_at3_PC[5];
extern const char NuFileExt_psc_PC[5];
extern const char NuFileExt_phg_PC[5];
extern const char NuFileExt_nx2_PC[5];
extern const char NuFileExt_hx2_PC[5];
extern const char NuFileExt_h4m_PC[5];
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

STATIC_ASSERT(offsetof(PC_NUDATFILEINFO, header) == 0);

extern const char NuFileSourcePathPC[50];

/* Append to nufile.c: real existing NuFileWrite is intentionally in this TU. */
typedef union PC_FILE_WRITE_WORD32 {
    u32 value;
    u8 bytes[4];
} PC_FILE_WRITE_WORD32;
STATIC_ASSERT(sizeof(PC_FILE_WRITE_WORD32) == 4);

typedef union PC_FILE_WRITE_FLOAT32 {
    f32 value;
    u32 representation;
    u8 bytes[4];
} PC_FILE_WRITE_FLOAT32;
STATIC_ASSERT(sizeof(PC_FILE_WRITE_FLOAT32) == 4);
STATIC_ASSERT(offsetof(PC_FILE_WRITE_FLOAT32, value) == 0);
STATIC_ASSERT(offsetof(PC_FILE_WRITE_FLOAT32, representation) == 0);
STATIC_ASSERT(offsetof(PC_FILE_WRITE_FLOAT32, bytes) == 0);

extern i32 nufile_lsn_allowed;

STATIC_ASSERT(offsetof(NUDATHDR, unknown_b4) == 0xb4);

extern char *NuBlankString;

#define NU_WIDE_LENGTH(text, length)                                                                                   \
    do {                                                                                                               \
        const u16 *nu_cursor = (const u16 *)(text);                                                                    \
        (length) = 0;                                                                                                  \
        while (*nu_cursor++ != 0) {                                                                                    \
            ++(length);                                                                                                \
        }                                                                                                              \
    } while (0)

#define NU_UTF16_UPPER(character)                                                                                      \
    (((character) >= 0x0061 && (character) <= 0x007a) || ((character) >= 0x00e0 && (character) <= 0x00ff)              \
         ? (u16)((character) - 0x20)                                                                                   \
         : (u16)(character))

PUSH_BLOCK_PC *__cdecl BlockInBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, PUSH_BLOCK_PC **);
void __cdecl MoveBlocksOverBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
void __cdecl MoveBlocks(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
void __cdecl UpdatePushBlocks(void *, void *, f32);
extern f32 PushVelocitySeekRatePc;
extern f32 PushFallingSpeedPc;
extern f32 PushCornerInsetPc;
extern f32 PushGroundOffsetPc;
extern f32 PushRumblePeriodPc;
extern f32 PushRumbleWindowPc;
extern f32 PushBlockOverlapShrinkPc;
extern i32 PushBlockHintIdPc;
void __cdecl KnockPushBlock(PUSH_BLOCK_PC *, NUVEC *);
PUSH_BLOCK_PC *__cdecl NearestPushBlock(WORLDINFO *, NUVEC *, f32);
char *__cdecl NuIToA(i32, char *, i32);
extern char *PushBlocksNamePc;

extern i32 old_line_pos;

#define CLAMP_LINE(position) ((position) & (parser->line_buffer_size - 1))
#define CLAMP_WORD(position) ((position) & (parser->word_buffer_size - 1))
#define CLAMP_WIDE_LINE(position) ((position) & ((parser->line_buffer_size >> 1) - 1))
#define CLAMP_WIDE_WORD(position) ((position) & ((parser->word_buffer_size >> 1) - 1))

typedef struct nurdpfcontext_pc_s {
    i32 token;
    f32 value;
    char *input;
    i32 position;
    i32(__cdecl *get_variable)(char *, f32 *, i32 *);
} NURDPFCTX_PC;

STATIC_ASSERT(sizeof(NURDPFCTX_PC) == 20);
STATIC_ASSERT(offsetof(NURDPFCTX_PC, token) == 0);
STATIC_ASSERT(offsetof(NURDPFCTX_PC, value) == 4);
STATIC_ASSERT(offsetof(NURDPFCTX_PC, input) == 8);
STATIC_ASSERT(offsetof(NURDPFCTX_PC, position) == 12);
STATIC_ASSERT(offsetof(NURDPFCTX_PC, get_variable) == 16);

enum {
    RDPF_ADD,
    RDPF_SUBTRACT,
    RDPF_MULTIPLY,
    RDPF_DIVIDE,
    RDPF_LEFT_PAREN,
    RDPF_RIGHT_PAREN,
    RDPF_END,
    RDPF_NUMBER,
    RDPF_VARIABLE
};

static f32 NuRDPFExpression(NURDPFCTX_PC *context);

typedef struct nurdpicontext_pc_s {
    i32 token;
    i32 value;
    char *input;
    i32 position;
    NURDPGETVARFN get_variable;
} NURDPICTX_PC;

STATIC_ASSERT(sizeof(NURDPICTX_PC) == 20);
STATIC_ASSERT(offsetof(NURDPICTX_PC, token) == 0);
STATIC_ASSERT(offsetof(NURDPICTX_PC, value) == 4);
STATIC_ASSERT(offsetof(NURDPICTX_PC, input) == 8);
STATIC_ASSERT(offsetof(NURDPICTX_PC, position) == 12);
STATIC_ASSERT(offsetof(NURDPICTX_PC, get_variable) == 16);

enum {
    RDPI_ADD,
    RDPI_SUBTRACT,
    RDPI_MULTIPLY,
    RDPI_DIVIDE,
    RDPI_MODULO,
    RDPI_LEFT_PAREN,
    RDPI_RIGHT_PAREN,
    RDPI_END,
    RDPI_NUMBER,
    RDPI_OR,
    RDPI_AND,
    RDPI_NOT,
    RDPI_XOR,
    RDPI_LOGICAL_OR,
    RDPI_LOGICAL_AND,
    RDPI_LOGICAL_NOT,
    RDPI_EQUAL,
    RDPI_NOT_EQUAL,
    RDPI_LESS,
    RDPI_GREATER,
    RDPI_LESS_EQUAL,
    RDPI_GREATER_EQUAL
};

static i32 NuRDPIExpression(NURDPICTX_PC *context);

/* PC manager prefix and block header touched by the retail allocator. */
typedef struct nualloc_manager_block_pc_s {
    u32 size_flags;
    struct nualloc_manager_block_pc_s *next;
    struct nualloc_manager_block_pc_s *previous;
} NUALLOC_MANAGER_BLOCK_PC;

typedef struct nualloc_manager_trailer_pc_s {
    NUALLOC_MANAGER_BLOCK_PC *previous_block;
} NUALLOC_MANAGER_TRAILER_PC;

typedef struct nualloc_manager_prefix_pc_s {
    void *base;
    u32 size;
    u32 block_count;
    u32 used_bytes;
} NUALLOC_MANAGER_PREFIX_PC;

STATIC_ASSERT(sizeof(NUALLOC_MANAGER_BLOCK_PC) == 12);
STATIC_ASSERT(offsetof(NUALLOC_MANAGER_BLOCK_PC, next) == 4);
STATIC_ASSERT(offsetof(NUALLOC_MANAGER_BLOCK_PC, previous) == 8);
STATIC_ASSERT(sizeof(NUALLOC_MANAGER_TRAILER_PC) == 4);
STATIC_ASSERT(offsetof(NUALLOC_MANAGER_TRAILER_PC, previous_block) == 0);
STATIC_ASSERT(sizeof(NUALLOC_MANAGER_PREFIX_PC) == 16);
STATIC_ASSERT(offsetof(NUALLOC_MANAGER_PREFIX_PC, size) == 4);
STATIC_ASSERT(offsetof(NUALLOC_MANAGER_PREFIX_PC, block_count) == 8);
STATIC_ASSERT(offsetof(NUALLOC_MANAGER_PREFIX_PC, used_bytes) == 12);

#define NUGENERICPAD FRAME_SCANNED_PAD_PC
#define ANALOG_CENTER 0x80
#define NUPAD_PLAY 2
#define NUPAD_RECORD 1
#define NUPAD_NORM 0
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
typedef struct FRAME_PAD_RAMP_PC {
    f32 threshold, value;
} FRAME_PAD_RAMP_PC;

static FRAME_SCANNED_PAD_PC g_nupadScannedPads[16] = {0};
static f32 FramePadRampRatePc = 0.1f;
static FRAME_PAD_RAMP_PC FramePadRampValuesPc[5] = {
    {0.0f, 0.0f}, {0.5f, 0.1f}, {0.75f, 0.25f}, {0.85f, 0.5f}, {1.0f, 1.0f}};
static i32 FramePadDirectMappingPc = 0, FramePadActivatedPc = 0;
enum {
    PAD_BUTTON_JUMP_SELECT = 1 << 0x6,
    PAD_BUTTON_START = 1 << 0xb,
    PAD_BUTTON_UP = 1 << 0xc,
    PAD_BUTTON_RIGHT = 1 << 0xd,
    PAD_BUTTON_DOWN = 1 << 0xe,
    PAD_BUTTON_LEFT = 1 << 0xf,
};

extern void NuPadOpenPS(FRAME_PAD_PC *pad);
u32 ImplodeDecodeC(void);
u32 ImplodeDecodeOffset(void);
void ImplodeMakeTable(i32, u8 *, i32, u16 *);
void ImplodeHufDecodeStart(void);
void ImplodeInitGetBits(void);
u32 ImplodeGetBits(i32 count);
void ImplodeFillBuf(i32 count);
u8 ImplodeGetByteFromMem(void);
void ImplodeError(char *msg, ...);
extern u32 implode_bitbuf;
extern u16 implode_left[0x3fb];
extern u16 implode_right[0x3fb];
extern i32 gExplodeInitialised;

extern char *implode_inbuffer;
extern char *implode_outbuffer;
extern u32 implode_origsize;
extern u32 implode_compsize;

static i32 ImplodePendingCopyPc = 0;

static unsigned char ImplodeRingBufferPc[0x2000] = {0};

#define SELECT(dst, table, shift, count, initial_mask, len)                                                            \
    do {                                                                                                               \
        (dst) = (table)[implode_bitbuf >> shift];                                                                      \
                                                                                                                       \
        if ((dst) > (count)) {                                                                                         \
            u32 mask = (initial_mask);                                                                                 \
                                                                                                                       \
            do {                                                                                                       \
                if ((implode_bitbuf & mask) != 0) {                                                                    \
                    (dst) = implode_right[(dst)];                                                                      \
                } else {                                                                                               \
                    (dst) = implode_left[(dst)];                                                                       \
                }                                                                                                      \
                                                                                                                       \
                mask >>= 1;                                                                                            \
            } while ((dst) > (count));                                                                                 \
        }                                                                                                              \
                                                                                                                       \
        ImplodeFillBuf((len)[(dst)]);                                                                                  \
    } while (0)

static u32 ImplodeBlockRemainingPc = 0;

static u16 ImplodeOffsetTablePc[0x100] = {0};
static u8 ImplodeOffsetLengthsPc[0x13] = {0};

static u16 ImplodeLiteralTablePc[0x1000] = {0};
static u8 ImplodeLiteralLengthsPc[0x1fe] = {0};
static u32 ImplodeSubBitsPc = 0;
static u32 ImplodeAvailableBitsPc = 0;

extern NUMTL *NuTimeBar_FrameOutMtl;

typedef struct TimeBarSet {
    i32 uses_buffer;
    i32 slot_count;
    i32 *accumulators[2];
    NuTime *start_times;
    i32 *field_14;
    i32 *field_18;
    i32 *colours;
    i32 *toggle_flags;
    const char **slot_names;
    i32 field_28;
} TimeBarSet;

STATIC_ASSERT(sizeof(TimeBarSet) == 0x2c);
STATIC_ASSERT(offsetof(TimeBarSet, slot_count) == 4);
STATIC_ASSERT(offsetof(TimeBarSet, accumulators) == 8);
STATIC_ASSERT(offsetof(TimeBarSet, start_times) == 0x10);
STATIC_ASSERT(offsetof(TimeBarSet, field_14) == 0x14);
STATIC_ASSERT(offsetof(TimeBarSet, field_18) == 0x18);
STATIC_ASSERT(offsetof(TimeBarSet, colours) == 0x1c);
STATIC_ASSERT(offsetof(TimeBarSet, toggle_flags) == 0x20);
STATIC_ASSERT(offsetof(TimeBarSet, slot_names) == 0x24);
STATIC_ASSERT(offsetof(TimeBarSet, field_28) == 0x28);

/* Original source: nu2api/nucore/nuapi.cpp; PC initializes only the fields
 * below after clearing the real 0x90-byte API record. */
static i32 NUAPI_PADREC_DEFAULT_BUFFERSIZE = 0x500000;

extern void __cdecl NuKeyboardRead(void);
extern void __cdecl PCFrameCriticalSectionEnter_candidate(void);
extern void __cdecl PCFrameCriticalSectionLeave_candidate(void);

extern i32 FrameSleepMillisecondsPc;
extern f32 FrameStartupDelayPc;
extern i32 NuApiTextLinePc;
extern f32 NuFrameAccumulatedTimePc;
extern const char *__cdecl PcFrameDelayMessagePc(void);
extern u32 __cdecl PcGetRefreshRatePc(void);
extern void __cdecl bgprocFrameSleepPc(i32 milliseconds);
extern void __cdecl NuFrameEndLeafPc(void);
extern void __cdecl FmvPlatformUpdatePc(void);

#define NUFRAME_ELAPSED(elapsed, now, start)                                                                           \
    if ((now).low >= (start).low) {                                                                                    \
        (elapsed).low = (now).low - (start).low;                                                                       \
        (elapsed).high = (now).high - (start).high;                                                                    \
    } else {                                                                                                           \
        (elapsed).high = (now).high - (start).high;                                                                    \
        (elapsed).low = (now).low - (start).low;                                                                       \
        (elapsed).high -= 1;                                                                                           \
    }

extern void(__cdecl *NuApiEndFrameCallbackPc)(void);

typedef void(__cdecl *NUDEBUGMSGFUNCTION_PC)(char *, ...);

STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEPAD_PC, bound_pad) == 0);

static void DecryptData(PC_FILEPAKHDR *header, u32 length, const u8 *key, i32 key_length);
static void AquireFileBuffer(FILEINFO *info);
static PC_FILEPAKITEM *GetItems(PC_FILEPAKHDR_V0 *header);
static NUFILE OpenDatFileBase(NUDATHDR *header, i32 file_index);
static i32 NuFileMatchExtensionPC(char *extension, char *path_end, i32 path_len);
static PC_FILEEXTINFO *NuFileExtGetInfoPC(char *path, i32 path_len);
static i32 ishexnum(char value);
static i32 hexcodetoint(char value);
static void NuRDPIGetToken(NURDPICTX_PC *context);
static i32 NuRDPIPrimary(NURDPICTX_PC *context);
static i32 NuRDPITerm(NURDPICTX_PC *context);
static i32 NuRDPIExpression(NURDPICTX_PC *context);
static void NuRDPFGetToken(NURDPFCTX_PC *context);
static f32 NuRDPFPrimary(NURDPFCTX_PC *context);
static f32 NuRDPFTerm(NURDPFCTX_PC *context);
static f32 NuRDPFExpression(NURDPFCTX_PC *context);
static NUFPAR_PC *NuAllocFPar(void);
static void NuAllocManagerUnlinkPc(NUALLOC_MANAGER_BLOCK_PC *block);
static void NuAllocManagerSplitPc(NUALLOC_MANAGER_BLOCK_PC **allocated, NUALLOC_MANAGER_BLOCK_PC **remainder,
                                  NUALLOC_MANAGER_BLOCK_PC *block, u32 *size, u32 alignment);
static i32 NuStringCharEquivalentPc(u16 a, u16 b);
static i32 NuStringIsFluffPc(u16 character);
static void NuFileAddExtensionPC(const char *extension, i32 type, i32 platform);
static void DeadZoneValueXYProper(i32 *x_value, i32 *y_value, i32 deadzone);
static void DeadZoneValueXY(i32 *x_value, i32 *y_value, i32 deadzone);
static f32 CalcRamp(f32 ramp);
static void ImplodeDecodeRingPrivatePc(u32 size, unsigned char *buf);
static void ImplodeReadOffsetLengthsPrivatePc(i32 table_size, i32 bit_count, i32 zero_value);
static void ImplodeReadLiteralLengthsPrivatePc();

void __cdecl NuMemSet128(void *destination, u32 value, i32 blocks) {
    memset(destination, (u8)value, (u32)blocks << 4);
}

void __cdecl NuDatFileDecodeNext(void) {
    char char_buf[12];

    NuFileRead(unpack_file_odi->dat_file, char_buf, 0xc);

    read_buffer_decoded_size = ExplodeBufferSize(char_buf);
    read_buffer_size = ExplodeCompressedSize(char_buf) - 0xc;
    NuFileRead(unpack_file_odi->dat_file, read_buffer, read_buffer_size);

    unpack_file_info->position += read_buffer_size + 0xc;
    unpack_file_odi->position = unpack_file_info->position;
}

void __cdecl NuFileUpCase(NUFILE_DEVICE *device, char *filepath) {
    char separator;

    if (device != NULL) {
        separator = device->dir_separator;
    } else {
        separator = '\\';
    }

    if (DontDoFileUpperCaseHack) {
        for (; *filepath != '\0'; filepath++) {
            if (*filepath == '\\' || *filepath == '/') {
                *filepath = separator;
            }
        }
    } else {
        for (; *filepath != '\0'; filepath++) {
            switch (*filepath) {
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':
                    *filepath -= 0x20;
                    break;
                case '/':
                case '\\':
                    *filepath = separator;
                    break;
            }
        }
    }
}

void __cdecl NuFileReldirFix(NUFILE_DEVICE *device, char *path) {
    char *destination = path;
    char *source = path;
    char separator;
    char previous;

    while (*source != '\0') {
        separator = device->dir_separator;
        if (source[0] == separator && source[1] == '.' && source[2] == '.' && source[3] == separator) {
            destination = source;
            while (destination > path) {
                previous = *--destination;
                if (previous == separator) {
                    source += 3;
                    break;
                }
                if (previous == ':') {
                    ++destination;
                    source += 4;
                    break;
                }
            }
        }
        *destination++ = *source++;
    }
    *destination = '\0';
}

NUFILE_DEVICE *__cdecl NuFileAddDevice(NUFILE_DEVICE *device) {
    if (numdevices >= 16) {
        NuFileError();
    }

    devices[numdevices] = *device;
    NuStrCpy(devices[numdevices].cur_dir, default_device->cur_dir);
    NuStrCpy(devices[numdevices].sys_dir, default_device->sys_dir);
    NuStrCpy(devices[numdevices].dll_dir, default_device->dll_dir);
    ++numdevices;
    return &devices[numdevices - 1];
}

void __cdecl NuFileGetCurrentDirectory(char *destination) {
    NuStrCpy(destination, default_device->cur_dir);
}

void __cdecl NuFileSetCurrentDirectory(const char *path) {
    i32 length;
    char separator[2] = "\\";
    if (*path != '\0') {
        length = NuStrCpy(default_device->cur_dir, path);
        if (length != 0 && path[length - 1] != '\\' && path[length - 1] != '/') {
            separator[0] = (char)default_device->dir_separator;
            NuStrCat(default_device->cur_dir, separator);
        }
    } else {
        default_device->cur_dir[0] = '\0';
    }
}

void __cdecl NuFileSetCurrentSysDirectory(char *path) {
    char *source;
    char *dest;
    char *separator = "\\";
    if (*path != '\0') {
        NuStrCpy(default_device->sys_dir, path);
        NuStrCat(default_device->sys_dir, separator);
    } else {
        NuStrCpy(default_device->sys_dir, "sys");
        source = iop_image_name + 5;
        dest = default_device->sys_dir + strlen(default_device->sys_dir);
        while (*source != '.') {
            *dest = *source;
            ++dest;
            ++source;
        }
        *dest++ = '\\';
        *dest = '\0';
    }
    NuFileUpCase(default_device, default_device->sys_dir);
}

void __cdecl NuFileGetCurrentDllPath(char *destination) {
    i32 root_length;

    NuStrCpy(destination, default_device->root);
    NuStrCat(destination, default_device->dll_dir);
    root_length = NuStrLen(default_device->root);
    NuFileUpCase(default_device, destination + root_length);
}

void __cdecl NuFileSetCurrentDllDirectory(const char *path) {
    char *separator = "\\";
    if (*path != '\0') {
        NuStrCpy(default_device->dll_dir, path);
        NuStrCat(default_device->dll_dir, separator);
    } else {
        default_device->dll_dir[0] = '\0';
    }
}

i32 __cdecl FileDeviceFormatDefaultNamePc(char *destination, char *path, i32 maximum_size) {
    NUFILE_DEVICE *device = default_device;
    return device->format_name_fn(device, destination, path, maximum_size);
}

NUFILE_DEVICE *__cdecl NuFileFindDevice(i32 id, i32 unit) {
    i32 index;

    if (id == -3) {
        return default_device;
    }
    if (id == 1 || id == 3) {
        id = 2;
    }
    for (index = 0; index < numdevices; ++index) {
        if (devices[index].id == id && (unit < 0 || unit == devices[index].unit)) {
            return &devices[index];
        }
    }
    return NULL;
}

void *__cdecl NuFileLoad(char *path) {
    i32 buffering = nufile_buffering_enabled;
    void *data = NULL;
    NUFILE file;

    nufile_buffering_enabled = 0;
    file = NuFileOpenDF(path, 0, curr_dat, 0);
    if (file != 0) {
        i32 size = (i32)NuFileOpenSize(file);
        if (size > 0) {
            data = NuAlloc(size, NuFileSourcePathPC, 0x64b);
            if (data != NULL) {
                NuFileRead(file, data, size);
            }
        }
        NuFileClose(file);
    }
    nufile_buffering_enabled = buffering;
    return data;
}

NUFILE_DEVICE *__cdecl NuFileGetDeviceFromPath(const char *path) {
    NUFILE_DEVICE *device;
    i32 prefix_end;
    i32 device_index;

    device = NULL;

    for (prefix_end = 0; prefix_end < 8; prefix_end++) {
        if (path[prefix_end] == ':') {
            break;
        }
    }

    if (prefix_end < 8) {
        for (device_index = 0; device_index < numdevices; device_index++) {
            if (NuStrNICmp(path, devices[device_index].name, devices[device_index].match_len) == 0) {
                device = &devices[device_index];
                break;
            }
        }
    }

    return device;
}

i32 __cdecl NuFileInitEx(i32 device_id, i32 reboot_iop, i32 eject) {
    NuPSFileInitDevices(device_id, reboot_iop, eject);
    memset(pc_memfiles, 0, sizeof(pc_memfiles));
    memset(pc_dat_file_infos, 0, sizeof(pc_dat_file_infos));
    file_criticalsection = PCThreadCriticalSectionCreate_candidate();
    NuFileExtInit(NuFileUseHiresPC() == 0);
    unpack_file_info = NULL;
    unpack_file_odi = NULL;
    unpack_file_pos = -1;
    return device_id;
}

void __cdecl NuFileInit(i32 device_id) {
    NuFileInitEx(device_id, 1, 0);
}

// RNC method 2 decoder; the header is skipped since NuPPLoadBuffer already knows the sizes.
void __cdecl NuPPUnpack(void *source, void *destination) {
    u8 *src;
    u8 *dst;
    u8 *copy;
    u32 first;
    u32 second;
    u32 bits;
    u32 bit;
    u32 length;
    u32 odd;
    i32 count;
    i32 run;
    u32 distance;
    u32 stack_top;
    u16 stack[66];

    src = (u8 *)source + 0x12;
    // The packer's saved-byte stack holds the output size and an empty byte count.
    stack_top = (u32)&stack[64];
    *(u32 *)stack_top = src[-0xe] << 24 | src[-0xd] << 16 | src[-0xc] << 8 | src[-0xb];
    stack_top -= 2;
    *(u16 *)stack_top = 0;
    dst = destination;
    // Skip the lock and key flags at the head of the bit stream.
    bits = *src++ * 4 + 2;
    for (;;) {
        // Literal bytes are the common case, so the empty-buffer test only follows a set bit.
        bits += bits;
        bit = (bits >> 8) & 1;
        while (bit == 0) {
            first = *src++;
            *dst++ = first;
            bits += bits;
            bit = (bits >> 8) & 1;
        }
        bits &= 0xff;
        if (bits == 0) {
            bits = bit + *src++ * 2;
            if ((bits & 0x100) == 0) {
                first = *src++;
                *dst++ = first;
                continue;
            }
        }
        length = 2;
        distance = 0;
        NUPP_GETBIT();
        if (bit == 0) {
            NUPP_GETBIT();
            length = bit + 4;
            NUPP_GETBIT();
            if (bit != 0) {
                length--;
                NUPP_GETBIT();
                length = length * 2 + bit;
                count = length - 9;
                if (count == 0) {
                    // Literal run of 12..72 bytes copied in groups of four.
                    for (count += 3; count >= 0; count--) {
                        NUPP_GETBIT();
                        distance = distance * 2 + bit;
                    }
                    run = distance + 2;
                    do {
                        second = src[1];
                        first = src[0];
                        dst[0] = first;
                        dst[1] = second;
                        second = src[3];
                        first = src[2];
                        dst[2] = first;
                        dst[3] = second;
                        run--;
                        src += 4;
                        dst += 4;
                    } while (run >= 0);
                    continue;
                }
            }
        } else {
            NUPP_GETBIT();
            if (bit == 0) {
                goto copy_match;
            }
            length = 3;
            NUPP_GETBIT();
            if (bit != 0) {
                length = *src++;
                if (length == 0) {
                    NUPP_GETBIT();
                    if (bit != 0) {
                        continue;
                    }
                    count = *(u16 *)stack_top;
                    stack_top += 2;
                    for (; count != 0; count--) {
                        *dst++ = *(u8 *)stack_top;
                        stack_top += 2;
                    }
                    return;
                }
                length += 8;
            }
        }
        NUPP_GETBIT();
        if (bit != 0) {
            NUPP_GETBIT();
            distance = bit;
            NUPP_GETBIT();
            if (bit != 0) {
                NUPP_GETBIT();
                distance = (distance * 2 + bit) | 4;
                NUPP_GETBIT();
                if (bit != 0) {
                    goto swap_offset;
                }
            } else if (distance != 0) {
                goto swap_offset;
            } else {
                distance = 1;
            }
            NUPP_GETBIT();
            distance = distance * 2 + bit;
        swap_offset:
            distance = (distance >> 8) | (u8)distance << 8;
        }
    copy_match:
        distance = *src++ | (distance & 0xff00);
        copy = dst - distance - 1;
        odd = length & 1;
        length >>= 1;
        if (odd != 0) {
            *dst++ = *copy++;
        }
        count = length - 1;
        if (distance == 0) {
            // A zero offset repeats the previous byte.
            distance = *copy | (distance & 0xff00);
            do {
                dst[0] = distance;
                count--;
                dst[1] = distance;
                dst += 2;
            } while (count >= 0);
        } else {
            do {
                first = copy[0];
                second = copy[1];
                dst[1] = second;
                dst[0] = first;
                count--;
                copy += 2;
                dst += 2;
            } while (count >= 0);
        }
    }
}

i64 __cdecl NuFileOpenSize(NUFILE file) {
    if (file >= 0x1000) {
        return NuMcFileOpenSize(file - 0x1000);
    }

    if (file >= 0x800) {
        PC_NUDATFILEINFO *info = &pc_dat_file_infos[file - 0x800];
        if (info->compression_mode != 0) {
            return info->decompressed_length;
        }
        return info->file_length;
    }

    if (file >= 0x400) {
        return pc_memfiles[file - 0x400].end - pc_memfiles[file - 0x400].buffer;
    }

    return file_info[file - 1].file_len;
}

NUFILE __cdecl NuFileOpenDF(const char *filepath, i32 mode, NUDATHDR *header, i32 quiet) {
    char path[256];
    i64 length;
    NUFILE_DEVICE *device;
    i32 mc_slot;
    i32 mc_port;
    i32 file_index;
    i32 ps_index;

    device = NuFileGetDeviceFromPath(filepath);
    if (device == NULL) {
        if (header != NULL && mode != 1 && mode != 2) {
            NUFILE file = NuDatFileOpen(header, (char *)filepath, mode);
            if (file != 0)
                return file;
        }
        device = default_device;
    }

    device->format_name_fn(device, path, (char *)filepath, sizeof(path));
    if (device->id == 2)
        NuStrCat(path, nufile_disc_suffix);
    if (device->id == 1) {
        mc_port = path[2] - '0';
        mc_slot = path[3] - '0';
        file_index = NuMcOpen(mc_port, mc_slot, path + device->name_len, mode, 0);
        if (file_index >= 0)
            return file_index + 0x1000;
        return 0;
    }

    file_index = NuPSFileOpen(path, mode);
    if (file_index < 0)
        return 0;

    ps_index = file_index;
    memset(&file_info[ps_index], 0, sizeof(FILEINFO));
    file_info[ps_index].handle = file_index;
    file_info[ps_index].mode = mode;

    if (mode != 1) {
        if (file_index <= 16) {
            do {
                length = NuFileSeek(ps_index + 1, 0, 2);
                if (length < 0 && quiet == 0)
                    NuFileRetry();
            } while (length < 0);
            file_info[ps_index].file_len = length;

            if (mode == 3) {
                while (NuFileStatus(ps_index + 1) != 0) {
                }
            }

            do {
                length = NuFileSeek(ps_index + 1, 0, 0);
                if (length < 0 && quiet == 0)
                    NuFileRetry();
            } while (length < 0);

            if (mode == 3) {
                while (NuFileStatus(ps_index + 1) != 0) {
                }
            }
        }
    } else {
        file_info[ps_index].file_len = 0;
    }

    if (mode == 0)
        file_info[ps_index].use_buf = nufile_buffering_enabled;
    return ps_index + 1;
}

NUFILE __cdecl NuFileOpen(const char *filepath, i32 mode) {
    return NuFileOpenDF(filepath, mode, curr_dat, 0);
}

static void DecryptData(PC_FILEPAKHDR *header, u32 length, const u8 *key, i32 key_length) {
    u8 *bytes = (u8 *)(header + 1);
    i32 index = 0;
    length -= sizeof(PC_FILEPAKHDR);
    while (length != 0) {
        --length;
        *bytes -= key[index++];
        if (index >= key_length)
            index = 0;
        ++bytes;
    }
    header->flags &= ~1u;
}

void __cdecl NuFileClose(NUFILE file) {
    FILEINFO *info;

    if (file >= 0x1000) {
        NuMcClose(file - 0x1000, 0);
        return;
    }
    if (file >= 0x400) {
        NuMemFileClose(file);
        return;
    }

    file--;
    while (NuPSFileClose(file) < 0)
        NuFileError();
    info = &file_info[file];
    if (info->buf != NULL)
        info->buf->file_info = NULL;
    memset(info, 0, sizeof(FILEINFO));
}

i32 __cdecl NuFileStatus(NUFILE file) {
    PC_NUDATFILEINFO *info;

    if (file >= 0x400)
        return 0;
    if (file >= 0x800) {
        info = &pc_dat_file_infos[file - 0x800];
        return NuFileStatus(info->header->open_files[info->open_file_index].dat_file);
    }
    return 0;
}

static void AquireFileBuffer(FILEINFO *info) {
    i32 index;
    i32 selected = 0;
    i32 time = file_time_count;
    FILEBUFF *buffer;

    for (index = 0; index < 4; index++) {
        if (file_buff[index].time < time) {
            time = file_buff[index].time;
            selected = index;
        }
    }

    buffer = &file_buff[selected];
    if (buffer->file_info != NULL)
        buffer->file_info->buf = NULL;
    buffer->file_info = info;
    buffer->time = file_time_count++;
    info->buf = buffer;

    if (info->buf_len != 0) {
        NuPSFileLSeek(info->handle, -info->buf_len, 1);
        NuPSFileRead(info->handle, buffer->data, info->buf_len);
    }
}

NUFILE __cdecl NuMemFileOpen(void *buffer, i32 buffer_size, i32 mode) {
    i32 index;

    if (buffer_size > 0 && (mode == 0 || mode == 1)) {
        for (index = 0; index < 20; index++) {
            if (pc_memfiles[index].used == 0) {
                pc_memfiles[index].buffer = (u8 *)buffer;
                pc_memfiles[index].end = (u8 *)buffer + buffer_size - 1;
                pc_memfiles[index].position = pc_memfiles[index].buffer;
                pc_memfiles[index].mode = mode;
                pc_memfiles[index].used = 1;
                return index + 0x400;
            }
        }
    }

    return 0;
}

void __cdecl NuMemFileClose(NUFILE file) {
    if (file >= 0x800) {
        PC_NUDATFILEINFO *info = &pc_dat_file_infos[file - 0x800];

        if (info->open_file_index >= 0) {
            info->header->open_files[info->open_file_index].info_idx = -1;
        } else {
            NuFileError();
        }
        info->is_used = 0;
    } else {
        pc_memfiles[file - 0x400].used = 0;
    }
}

i64 __cdecl NuMemFilePos(NUFILE file) {
    if (file >= 0x800) {
        PC_NUDATFILEINFO *info = &pc_dat_file_infos[file - 0x800];
        return info->position - info->start;
    }

    file -= 0x400;
    return pc_memfiles[file].position - pc_memfiles[file].buffer;
}

void *__cdecl NuMemFileAddr(NUFILE file) {
    file -= 0x400;
    return pc_memfiles[file].position;
}

i32 __cdecl NuMemFileRead(NUFILE file, void *destination, i32 size) {
    PC_NUMEMFILE *memory;
    i32 available;

    if (file >= 0x800) {
        return NuDatFileRead(file, destination, size);
    }

    memory = &pc_memfiles[file - 0x400];
    available = (i32)(memory->end - memory->position) + 1;
    if (size > available) {
        size = available;
    }
    if (size != 0) {
        memcpy(destination, memory->position, size);
        memory->position += size;
    }
    return size;
}

i32 __cdecl NuMemFileWrite(NUFILE file, const void *data, i32 size) {
    i32 remaining;

    file -= 0x400;
    remaining = pc_memfiles[file].end - pc_memfiles[file].position + 1;
    if (size > remaining) {
        size = remaining;
    }
    if (size != 0) {
        memcpy(pc_memfiles[file].position, data, size);
        pc_memfiles[file].position += size;
    }
    return size;
}

i64 __cdecl NuMemFileSeek(NUFILE file, i64 offset, i32 origin) {
    if (file >= 0x800) {
        return NuDatFileSeek(file, offset, origin);
    }

    file -= 0x400;
    switch (origin) {
        case 1:
            pc_memfiles[file].position += (i32)offset;
            break;
        case 2:
            pc_memfiles[file].position = pc_memfiles[file].end - (i32)offset;
            break;
        default:
            pc_memfiles[file].position = pc_memfiles[file].buffer + (i32)offset;
            break;
    }
    return pc_memfiles[file].position - pc_memfiles[file].buffer;
}

i64 __cdecl NuFilePos(NUFILE file) {
    i64 position;

    if (file >= 0x1000) {
        return NuMcSeek(file - 0x1000, 0, 2, 0);
    }

    if (file >= 0x400) {
        return NuMemFilePos(file);
    }

    file--;
    if (file_info[file].use_buf != 0) {
        return file_info[file].read_pos;
    }

    do {
        position = NuPSFileLSeek(file, 0, 1);
    } while (position < 0);

    return position;
}

i32 NuFileEOF(NUFILE file) {
    i64 position = NuFilePos(file);
    i64 size = NuFileOpenSize(file);
    return position >= size;
}

i64 __cdecl NuFileSeek(NUFILE file, i64 offset, i32 origin) {
    FILEINFO *info;

    if (file >= 0x1000) {
        return NuMcSeek(file - 0x1000, (i32)offset, origin, 0);
    }
    if (file >= 0x400) {
        return NuMemFileSeek(file, (i32)offset, origin);
    }

    file--;
    info = &file_info[file];
    if (info->use_buf != 0) {
        switch (origin) {
            case 1:
                info->read_pos += offset;
                break;
            case 2:
                info->read_pos = info->file_len - offset;
                break;
            default:
                info->read_pos = offset;
                break;
        }
        return info->read_pos;
    }
    return NuPSFileLSeek(file, offset, origin);
}

i32 __cdecl NuFileExistQuiet(char *filepath) {
    NUFILE file = 0;
    i32 saved_buffering;

    if (filepath == NULL || *filepath == '\0') {
        return 0;
    }

    saved_buffering = nufile_buffering_enabled;
    nufile_buffering_enabled = 0;

    if (curr_dat != NULL) {
        i32 index = NuDatFileFindTree(curr_dat, filepath);
        if (index >= 0) {
            nufile_buffering_enabled = saved_buffering;
            return curr_dat->file_info[index].decompressed_len;
        }
    }

    file = NuFileOpenDF(filepath, 0, curr_dat, 1);
    if (file != 0) {
        NuFileClose(file);
    }

    nufile_buffering_enabled = saved_buffering;
    return file != 0;
}

i64 __cdecl NuFileSize(char *filepath) {
    NUDATHDR *dat = curr_dat;
    i32 file;
    i64 pos;
    i32 buffering;

    buffering = nufile_buffering_enabled;
    nufile_buffering_enabled = 0;

    if (dat != NULL) {
        file = NuDatFileFindTree(dat, filepath);
        if (file >= 0) {
            /* The PC early return leaves file buffering disabled. */
            return dat->file_info[file].decompressed_len;
        }
    }

    pos = -1;

    if (filepath != NULL && *filepath != '\0') {
        file = NuFileOpenDF(filepath, 0, dat, 0);
        if (file != 0) {
            do {
                pos = NuFileSeek(file, 0, 2);
            } while (pos < 0);

            pos = NuFilePos(file);
            NuFileClose(file);
        }
    }

    nufile_buffering_enabled = buffering;

    return pos;
}

i32 __cdecl NuFileBeginBlkRead(NUFILE file, i32 expected_tag) {
    i32 index = nufile_blkcnt++;
    i64 position = NuFilePos(file);
    PC_NUFILE_READBLOCK *block;

    if (index > 1 && nufile_blkinfo[index - 1].position + nufile_blkinfo[index - 1].size <= position) {
        --nufile_blkcnt;
        return 0;
    }
    block = &nufile_blkinfo[index];
    block->position = NuFilePos(file);
    NuFileRead(file, block, 8);
    if (block->size < 0) {
        block->size = -block->size;
    }
    if (expected_tag != 0 && expected_tag != block->tag) {
        NuFileError();
    }
    return block->tag;
}

i32 __cdecl NuFileAlignRead(NUFILE file, i32 mask) {
    i32 count = 0;
    i64 position = NuFilePos(file);
    while ((position & (i64)mask) != 0) {
        i8 byte = 0;
        ++position;
        ++count;
        NuFileRead(file, &byte, 1);
    }
    return count;
}

void __cdecl NuFileEndBlkRead(NUFILE file) {
    i32 index = --nufile_blkcnt;
    i64 position = NuFilePos(file);
    i64 end = nufile_blkinfo[index].position + nufile_blkinfo[index].size;
    if (position != end) {
        NuFileSeek(file, end, 0);
    }
}

i32 __cdecl NuFileGetBlkSize(void) {
    return nufile_blkinfo[nufile_blkcnt - 1].size;
}

i32 NuFileExtractPath(char *dest, char *path) {
    i32 length;
    char *end = strrchr(path, '\\');
    if (end != NULL) {
        ++end;
    } else {
        end = path;
    }
    length = end - path;
    strncpy(dest, path, length);
    dest[length] = '\0';
    return length;
}

i32 NuFileExtractFile(char *dest, char *path) {
    char *end;
    i32 length;
    char *start = strrchr(path, '\\');
    if (start != NULL) {
        ++start;
    } else {
        start = path;
    }
    end = strrchr(path, '.');
    if (end == NULL) {
        end = path + strlen(path);
    }
    length = end - start;
    strncpy(dest, start, length);
    dest[length] = '\0';
    return length;
}

i32 __cdecl NuFileLoadBuffer(char *filepath, void *destination, i32 capacity) {
    NUFILE file;
    i32 loaded = 0;

    nufile_lasterror = 0;
    if (curr_dat != NULL) {
        loaded = NuDatFileLoadBuffer(curr_dat, filepath, destination, capacity);
        if (nufile_lasterror == -1) {
            return 0;
        }
    }

    if (loaded == 0) {
        if (NuFileExists(filepath)) {
            file = NuFileOpenDF(filepath, 0, curr_dat, 0);
            if (file != 0) {
                if (nufile_try_packed != 0) {
                    loaded = NuPPLoadBuffer(file, destination, capacity);
                } else {
                    loaded = (i32)NuFileOpenSize(file);
                    if (loaded <= capacity && loaded != 0) {
                        while (NuFileRead(file, destination, loaded) < 0) {
                            NuFileRetry();
                            while (NuFileSeek(file, 0, 0) < 0) {
                                NuFileRetry();
                            }
                        }
                    } else {
                        nufile_lasterror = -1;
                        loaded = 0;
                    }
                }
                NuFileClose(file);
            } else {
                nufile_lasterror = -3;
            }
        } else {
            nufile_lasterror = -2;
        }
    }
    return loaded;
}

i32 __cdecl NuFileLoadBufferVP(char *filepath, VARIPTR *buffer, VARIPTR *buffer_end) {
    i32 loaded = NuFileLoadBuffer(filepath, buffer->void_ptr, buffer_end->addr - buffer->addr);

    buffer->addr += loaded;
    return loaded;
}

i32 __cdecl NuFileRead(NUFILE file, void *destination, i32 size) {
    FILEINFO *info;
    i32 bytes_read;
    i32 available;
    char *output;

    if (file >= 0x1000) {
        return NuMcRead(file - 0x1000, destination, size, 0);
    }
    if (file >= 0x400) {
        return NuMemFileRead(file, destination, size);
    }

    file--;
    info = &file_info[file];
    if (info->use_buf == 0)
        return NuPSFileRead(file, destination, size);

    if (info->buf == NULL)
        AquireFileBuffer(info);
    bytes_read = 0;
    output = (char *)destination;

    while (size > 0) {
        if (info->read_pos >= info->file_len)
            return bytes_read;

        if (info->read_pos < info->buf_start || info->read_pos >= info->buf_start + info->buf_len) {
            if (info->read_pos != info->file_pos) {
                NuPSFileLSeek(info->handle, info->read_pos, 0);
                info->file_pos = info->read_pos;
            }

            info->buf_len = NuPSFileRead(info->handle, info->buf->data, sizeof(info->buf->data));
            info->buf_start = info->file_pos;
            info->file_pos += info->buf_len;
        }

        available = info->buf_len + (i32)(info->buf_start - info->read_pos);
        if (available >= size)
            available = size;
        if (available != 0)
            memcpy(output, info->buf->data + (i32)(info->read_pos - info->buf_start), available);
        output += available;
        bytes_read += available;
        size -= available;
        info->read_pos += available;
    }

    return bytes_read;
}

f32 __cdecl NuFileReadFloat(NUFILE file) {
    f32 value;

    NuFileRead(file, &value, sizeof(value));
    return value;
}

i32 __cdecl NuFileReadInt(NUFILE file) {
    i32 value;

    NuFileRead(file, &value, sizeof(value));
    return value;
}

i8 __cdecl NuFileReadChar(NUFILE file) {
    i8 value = 0;

    NuFileRead(file, &value, sizeof(value));
    return value;
}

u16 __cdecl NuFileReadWChar(NUFILE file) {
    u16 value = 0;

    NuFileRead(file, &value, sizeof(value));
    return value;
}

i32 __cdecl NuFileWrite(NUFILE file, const void *data, i32 size) {
    if (file >= 0x1000) {
        return NuMcWrite(file - 0x1000, data, size, 0);
    }

    if (file >= 0x400) {
        return NuMemFileWrite(file, data, size);
    }

    file -= 1;
    return NuPSFileWrite(file, data, size);
}

void __cdecl NuFileWriteString(NUFILE file, const char *text) {
    NuFileWrite(file, text, (i32)strlen(text));
}

i32 __cdecl NuFileCopyEx(char *destination, char *source, void *buffer, i32 capacity) {
    i32 remaining;
    i32 copied = 0;
    NUFILE input = NuFileOpenDF(source, 0, curr_dat, 0);
    NUFILE output = NuFileOpenDF(destination, 1, curr_dat, 0);

    if (input != 0 && output != 0) {
        remaining = (i32)NuFileOpenSize(input);
        while (remaining != 0) {
            i32 count = NuFileRead(input, buffer, remaining < capacity ? remaining : capacity);
            NuFileWrite(output, buffer, count);
            copied += count;
            remaining -= count;
        }
    }

    if (input != 0) {
        NuFileClose(input);
    }
    if (output != 0) {
        NuFileClose(output);
    }
    return copied;
}

i32 __cdecl NuFileCopy(char *destination, char *source) {
    char buffer[1024];

    return NuFileCopyEx(destination, source, buffer, sizeof(buffer));
}

void __cdecl PCByteCopy_candidate(void *destination, const void *source, u32 count) {
    u8 *dst = (u8 *)destination;
    const u8 *src = (const u8 *)source;

    if (dst < src) {
        while (count != 0) {
            count--;
            *dst = *src;
            dst = dst + 1;
            src = src + 1;
        }
    } else {
        dst += count;
        src += count;
        while (count != 0) {
            count--;
            dst--;
            src--;
            *dst = *src;
        }
    }
}

static u32 CalcChecksum(void *data, i32 len) {
    unsigned char *bytes;
    u32 sum;

    sum = 0x12345678;
    bytes = (unsigned char *)data;

    while (len != 0) {
        len--;
        sum += *(bytes++);
    }

    return sum;
}

static PC_FILEPAKITEM *GetItems(PC_FILEPAKHDR_V0 *header) {
    switch (header->type) {
        case 0x12345678:
            return (PC_FILEPAKITEM *)(header + 1);
        case 0x1234567a:
            return (PC_FILEPAKITEM *)((PC_FILEPAKHDR *)header + 1);
        default:
            return (PC_FILEPAKITEM *)((PC_FILEPAKHDR_V1 *)header + 1);
    }
}

i32 __cdecl NuFileWriteFloat(NUFILE file, f32 value) {
    i32 swap_endian = NuFile_SwapEndianOnWrite;
    PC_FILE_WRITE_FLOAT32 input;
    PC_FILE_WRITE_WORD32 output;
    input.value = value;
    if (swap_endian != 0) {
        output.bytes[0] = input.bytes[3];
        output.bytes[1] = input.bytes[2];
        output.bytes[2] = input.bytes[1];
        output.bytes[3] = input.bytes[0];
        input.representation = output.value;
    }
    return NuFileWrite(file, &input.value, 4);
}

i32 __cdecl NuFileWriteInt(NUFILE file, i32 value) {
    PC_FILE_WRITE_WORD32 input, output;
    input.value = (u32)value;
    output.value = input.value;
    if (NuFile_SwapEndianOnWrite != 0) {
        output.bytes[0] = input.bytes[3];
        output.bytes[1] = input.bytes[2];
        output.bytes[2] = input.bytes[1];
        output.bytes[3] = input.bytes[0];
    }
    return NuFileWrite(file, &output.value, 4);
}

u32 __cdecl NuFileWriteUnsignedInt(NUFILE file, u32 value) {
    PC_FILE_WRITE_WORD32 input, output;
    input.value = value;
    output.value = input.value;
    if (NuFile_SwapEndianOnWrite != 0) {
        output.bytes[0] = input.bytes[3];
        output.bytes[1] = input.bytes[2];
        output.bytes[2] = input.bytes[1];
        output.bytes[3] = input.bytes[0];
    }
    return (u32)NuFileWrite(file, &output.value, 4);
}

i32 __cdecl NuFileWriteShort(NUFILE file, i16 value) {
    PC_FILE_WRITE_WORD32 input, output;
    input.value = (u16)value;
    output.value = input.value;
    if (NuFile_SwapEndianOnWrite != 0) {
        output.bytes[0] = input.bytes[1];
        output.bytes[1] = input.bytes[0];
    }
    return NuFileWrite(file, &output.value, 2);
}

i32 __cdecl NuFileWriteUnsignedShort(NUFILE file, u16 value) {
    PC_FILE_WRITE_WORD32 input, output;
    input.value = value;
    output.value = input.value;
    if (NuFile_SwapEndianOnWrite != 0) {
        output.bytes[0] = input.bytes[1];
        output.bytes[1] = input.bytes[0];
    }
    return NuFileWrite(file, &output.value, 2);
}

i32 __cdecl NuFileWriteChar(NUFILE file, i8 value) {
    return NuFileWrite(file, &value, 1);
}

i32 __cdecl NuFileWriteUnsignedChar(NUFILE file, u8 value) {
    return NuFileWrite(file, &value, 1);
}

i64 __cdecl NuDatCalcPos(NUDATHDR *header, i32 offset) {
    i64 position = offset;

    if (header->version <= -2) {
        position = (i64)((u64)position << 8);
    }
    return position + datsys_offset;
}

i32 __cdecl NuDatFileLoadBuffer(NUDATHDR *header, char *filepath, void *destination, i32 capacity) {
    NUFILE file;
    PC_NUDATFILEINFO *info;
    i32 size;

    nufile_lasterror = 0;
    file = NuDatFileOpen(header, filepath, 0);
    if (file != 0) {
        if (header->mode == 3) {
            while (NuFileStatus(file) != 0) {
            }
        }

        info = &pc_dat_file_infos[file - 0x800];
        if (info->compression_mode != 0) {
            size = info->decompressed_length;
        } else {
            size = info->file_length;
        }

        if (size <= capacity && size != 0) {
            while (NuDatFileRead(file, destination, size) < 0) {
                do {
                    NuFileRetry();
                } while (NuDatFileSeek(file, 0, 0) < 0);
            }

            if (header->mode == 3) {
                while (NuFileStatus(file) != 0) {
                }
            }
            NuFileClose(file);
            return size;
        }

        if (size != 0) {
            nufile_lasterror = -1;
        }
        NuFileClose(file);
    }
    return 0;
}

i32 __cdecl NuDatFileRead(NUFILE file, void *destination, i32 size) {
    PC_NUDATFILEINFO *info = &pc_dat_file_infos[file - 0x800];
    NUDATOPENFILEINFO *open_file = &info->header->open_files[info->open_file_index];
    i64 remaining;
    i32 to_read;
    i32 bytes_read;

    if (info->compression_mode == 2) {
        i32 total_read = 0;
        u8 *output = (u8 *)destination;
        while (size != 0) {
            if (decode_buffer_left == 0) {
                NuDatFileDecodeNext();
                if (read_buffer_size == read_buffer_decoded_size) {
                    memcpy(decode_buffer, read_buffer, read_buffer_size);
                } else {
                    ExplodeBufferNoHeader((char *)read_buffer, (char *)decode_buffer, read_buffer_size,
                                          read_buffer_decoded_size);
                }
                decode_buffer_pos = 0;
                decode_buffer_left = read_buffer_decoded_size;
            }
            bytes_read = size < decode_buffer_left ? size : decode_buffer_left;
            memcpy(output, decode_buffer + decode_buffer_pos, bytes_read);
            output += bytes_read;
            decode_buffer_pos += bytes_read;
            decode_buffer_left -= bytes_read;
            size -= bytes_read;
            total_read += bytes_read;
        }
        return total_read;
    }

    if (info->position != open_file->position) {
        NuFileSeek(open_file->dat_file, info->position, 0);
        open_file->position = info->position;
    }

    remaining = info->file_length + info->start - info->position;
    if (remaining < 0) {
        remaining = 0;
    }
    to_read = size;
    if ((i64)to_read >= remaining) {
        to_read = (i32)remaining;
    }
    if (to_read != 0) {
        bytes_read = NuFileRead(open_file->dat_file, destination, to_read);
        if (bytes_read >= 0) {
            info->position += bytes_read;
            open_file->position = info->position;
        }
        return bytes_read;
    }
    return 0;
}

i64 __cdecl NuDatFileSeek(NUFILE file, i64 offset, i32 whence) {
    PC_NUDATFILEINFO *info = &pc_dat_file_infos[file - 0x800];
    NUDATOPENFILEINFO *open_file = &info->header->open_files[info->open_file_index];
    i64 target_position;

    switch (whence) {
        case 1:
            target_position = info->position + offset;
            break;
        case 2:
            target_position = info->start + info->file_length - offset;
            break;
        default:
            target_position = info->start + offset;
            break;
    }

    info->position = NuFileSeek(open_file->dat_file, target_position, 0);
    open_file->position = info->position;
    return info->position;
}

i32 __cdecl NuDatFileFindHash(NUDATHDR *header, char *name) {
    u32 hash = 0x811c9dc5;
    VARIPTR ptr;
    i32 i;
    char *cursor;

    for (cursor = name; *cursor != '\0'; cursor++) {
        hash ^= *cursor;
        hash *= 0x199933;
    }

    for (i = 0; i < header->file_count; i++) {
        if (header->hash_idxs[i] == hash) {
            return i;
        }
    }

    if (header->hash_count != 0) {
        ptr.char_ptr = header->hashes;

        for (i = 0; i < header->hash_count; i++) {
            if (NuStrCmp(ptr.char_ptr, name) == 0) {
                ptr.addr += NuStrLen(ptr.char_ptr) + 1;

                if ((ptr.addr & 1) != 0) {
                    ptr.addr++;
                }

                return *(i16 *)ptr.void_ptr;
            }

            ptr.addr += NuStrLen(ptr.char_ptr) + 3;

            if ((ptr.addr & 1) != 0) {
                ptr.addr++;
            }
        }
    }

    return -1;
}

i32 __cdecl NuDatFileFindTree(NUDATHDR *header, char *name) {
    char buf[256];
    char *backslash;
    i32 node_idx;

    if (*name == '@') {
        name = name + 4;
    }

    NuStrCpy(buf, name);
    NuFileUpCase(NULL, buf);

    name = buf;

    if (header->file_tree == NULL) {
        return NuDatFileFindHash(header, name);
    }

    node_idx = header->file_tree->child_idx;

    backslash = NuStrStr(name, "\\");
    if (backslash != NULL) {
        *backslash = '\0';
    }

    do {
        if (NuStrICmp(header->file_tree[node_idx].name, name) == 0) {
            if (backslash == NULL) {
                if (header->file_tree[node_idx].child_idx <= 0) {
                    return -header->file_tree[node_idx].child_idx;
                }

                return -1;
            }

            node_idx = header->file_tree[node_idx].child_idx;

            name = backslash + 1;

            backslash = NuStrStr(name, "\\");
            if (backslash != NULL) {
                *backslash = '\0';
            }
        } else {
            node_idx = header->file_tree[node_idx].sibling_idx;
        }
    } while (node_idx != 0);

    return -1;
}

void __cdecl NuDatFileClose(NUFILE file) {
    PC_NUDATFILEINFO *info;
    NUDATHDR *header;

    file -= 0x800;
    info = pc_dat_file_infos + file;
    header = info->header;
    if (info->open_file_index >= 0) {
        header->open_files[info->open_file_index].info_idx = -1;
    }
    info->is_used = 0;
}

i32 __cdecl NuDatFileGetFreeInfo(void) {
    i32 found = -1;
    i32 index;

    PCThreadCriticalSectionEnter_candidate(file_criticalsection);
    for (index = 0; index < 20; index++) {
        if (pc_dat_file_infos[index].is_used == 0) {
            pc_dat_file_infos[index].is_used = 1;
            found = index;
            break;
        }
    }
    PCThreadCriticalSectionLeave_candidate(file_criticalsection);
    return found;
}

i32 __cdecl NuDatFileGetFreeHandleIX(NUDATHDR *header, i32 info_index) {
    i32 found = -1;
    i32 index;

    PCThreadCriticalSectionEnter_candidate(file_criticalsection);
    for (index = 0; index < 8; index++) {
        if (header->open_files[index].info_idx == -1) {
            header->open_files[index].info_idx = info_index;
            found = index;
            break;
        }
    }
    PCThreadCriticalSectionLeave_candidate(file_criticalsection);
    return found;
}

static NUFILE OpenDatFileBase(NUDATHDR *header, i32 file_index) {
    NUDATOPENFILEINFO *open_file = &header->open_files[file_index];

    if (open_file->dat_file == 0) {
        open_file->dat_file = NuFileOpenDF(header->name, header->mode, NULL, 0);
        open_file->position = 0;
    }
    return open_file->dat_file;
}

static void NuDatFileDecodeNewFile(PC_NUDATFILEINFO *file, NUDATOPENFILEINFO *open_file) {
    unpack_file_info = file;
    unpack_file_odi = open_file;
    unpack_file_pos = file->start;
    decode_buffer_left = 0;
}

NUFILE __cdecl NuDatFileOpen(NUDATHDR *hdr, char *filepath, i32 mode) {
    i32 node_idx;
    i32 info_idx;
    i32 file_idx;
    NUDATOPENFILEINFO *open_info;
    i64 seek_pos;

    if (mode == 0) {
        node_idx = NuDatFileFindTree(hdr, filepath);
        if (node_idx >= 0 && hdr->file_info[node_idx].file_len != 0) {
            info_idx = NuDatFileGetFreeInfo();
            if (info_idx != -1) {
                file_idx = NuDatFileGetFreeHandleIX(hdr, info_idx);
                if (file_idx != -1) {
                    open_info = &hdr->open_files[file_idx];

                    OpenDatFileBase(hdr, file_idx);

                    pc_dat_file_infos[info_idx].header = hdr;
                    pc_dat_file_infos[info_idx].start = NuDatCalcPos(hdr, hdr->file_info[node_idx].file_offset);
                    pc_dat_file_infos[info_idx].position = pc_dat_file_infos[info_idx].start;

                    pc_dat_file_infos[info_idx].file_length = hdr->file_info[node_idx].file_len;
                    pc_dat_file_infos[info_idx].decompressed_length = hdr->file_info[node_idx].decompressed_len;
                    pc_dat_file_infos[info_idx].compression_mode = hdr->file_info[node_idx].compression_mode;

                    pc_dat_file_infos[info_idx].open_file_index = file_idx;

                    open_info->position = pc_dat_file_infos[info_idx].position;

                    do {
                        seek_pos = NuFileSeek(open_info->dat_file, pc_dat_file_infos[info_idx].start, 0);
                        if (seek_pos < 0) {
                            NuFileRetry();
                        }
                    } while (seek_pos < 0);

                    if (pc_dat_file_infos[info_idx].compression_mode == 2) {
                        NuDatFileDecodeNewFile(&pc_dat_file_infos[info_idx], open_info);
                    }

                    return info_idx + 0x800;
                }

                pc_dat_file_infos[info_idx].is_used = 0;
            } else {
                NuFileRetry();
            }
        } else {
            NuDatFileMissingPc(filepath);
        }
    }

    return 0;
}

i32 __cdecl NuDatFileOpenSize(NUFILE file) {
    PC_NUDATFILEINFO *info;

    file -= 0x800;
    info = &pc_dat_file_infos[file];
    if (info->compression_mode != 0) {
        return info->decompressed_length;
    }
    return info->file_length;
}

i32 __cdecl NuFileGetLastError(void) {
    return nufile_lasterror;
}

NUDATHDR *__cdecl NuDatOpenEx(const char *filepath, VARIPTR *arena, i32 *size_out, i16 mode) {
    i32 saved_buffering = nufile_buffering_enabled;
    NUFILE file = NuFileOpenDF(filepath, mode, NULL, 0);
    i32 locator[2];
    i64 metadata_pos;
    char formatted_name[256];
    i32 name_len;
    i32 name_storage;
    i32 allocation_size;
    NUDATHDR *header;
    u8 *metadata;
    u8 *cursor;
    u8 *hashes_start;
    char *hash;
    i32 index;

    if (file != 0) {
        if (mode == 3) {
            while (NuFileStatus(file) != 0) {
            }
        }

        if (datsys_offset != 0)
            NuFileSeek(file, datsys_offset, 0);
        while (NuFileRead(file, locator, sizeof(locator)) < 0) {
            do {
                NuFileRetry();
            } while (NuFileSeek(file, datsys_offset, 0) < 0);
        }
        if (mode == 3) {
            while (NuFileStatus(file) != 0) {
            }
        }

        metadata_pos = locator[0];
        if (metadata_pos < 0)
            metadata_pos *= -256;
        metadata_pos += datsys_offset;
        while (NuFileSeek(file, metadata_pos, 0) < 0) {
            NuFileRetry();
        }
        if (mode == 3) {
            while (NuFileStatus(file) != 0) {
            }
        }

        default_device->format_name_fn(default_device, formatted_name, (char *)filepath, sizeof(formatted_name));
        name_len = NuStrLen(formatted_name);
        name_storage = (name_len + 16) & ~15;
        allocation_size = sizeof(NUDATHDR) + name_storage + locator[1];

        if (arena == NULL) {
            NuFileError();
            NuFileClose(file);
            nufile_buffering_enabled = saved_buffering;
            return NULL;
        }
        header = (NUDATHDR *)arena->void_ptr;
        arena->char_ptr += allocation_size;
        if (size_out != NULL)
            *size_out = allocation_size;
        header->unknown_b0 = 0;
        header->name = (char *)(header + 1);
        metadata = (u8 *)(header + 1) + name_storage;
        NuStrCpy(header->name, formatted_name);
        while (NuFileRead(file, metadata, locator[1]) < 0) {
            do {
                NuFileRetry();
            } while (NuFileSeek(file, metadata_pos, 0) < 0);
        }
        if (mode == 3) {
            while (NuFileStatus(file) != 0) {
            }
        }

        header->version = ((i32 *)metadata)[0];
        header->file_count = ((i32 *)metadata)[1];
        header->file_info = (NUDATFINFO *)(metadata + 8);
        cursor = (u8 *)(header->file_info + header->file_count);
        header->tree_node_count = *(i32 *)cursor;
        cursor += sizeof(i32);
        header->file_tree = (NUDFNODE *)cursor;
        cursor += header->tree_node_count * sizeof(NUDFNODE);
        header->leaf_names_len = *(i32 *)cursor;
        cursor += sizeof(i32);
        header->leaf_names = (char *)cursor;
        for (index = 0; index < header->tree_node_count; ++index) {
            header->file_tree[index].name = header->leaf_names + (u32)header->file_tree[index].name;
        }
        header->file_tree[0].name = NULL;
        cursor += header->leaf_names_len;

        header->hash_idxs = NULL;
        header->hash_count = 0;
        header->hashes_len = 0;
        header->hashes = NULL;
        if (header->version <= -2) {
            header->hash_idxs = (u32 *)cursor;
            cursor += header->file_count * sizeof(u32);
            header->hash_count = *(i32 *)cursor;
            cursor += sizeof(i32);
            header->hashes_len = *(i32 *)cursor;
            cursor += sizeof(i32);
            header->hashes = (char *)cursor;
            /* Android endian-swaps each hash entry here; on PC the walk is left without a swap. */
            hash = header->hashes;
            for (index = 0; index < header->hash_count; ++index) {
                name_len = NuStrLen(hash);
                hash += name_len + 1;
                if (((u32)hash & 1) != 0) {
                    hash++;
                }
                hash += 2;
            }
            hashes_start = (u8 *)header->file_tree;
            PCByteCopy_candidate(hashes_start, header->hash_idxs, header->file_count * sizeof(u32));
            header->hash_idxs = (u32 *)hashes_start;
            cursor = hashes_start + header->file_count * sizeof(u32);
            if (header->hashes_len != 0) {
                header->hashes = (char *)cursor;
                PCByteCopy_candidate(cursor, cursor, header->hashes_len);
                cursor += header->hashes_len;
            }
            arena->void_ptr = cursor;
            header->tree_node_count = 0;
            header->file_tree = NULL;
            header->leaf_names_len = 0;
            header->leaf_names = NULL;
        }

        for (index = 0; index < 8; ++index) {
            header->open_files[index].info_idx = -1;
            header->open_files[index].dat_file = 0;
            header->open_files[index].position = 0;
        }
        header->open_files[0].dat_file = file;
        header->open_files[0].position = 0;
        header->mode = mode;
        if (nufile_lsn_allowed != 0 && default_device->id == 2) {
            /* Disc path for the console LSN lookup; the PC build never resolves it. */
            NuStrCpy(formatted_name, WorldPathSeparator);
            NuStrCat(formatted_name, default_device->cur_dir);
            NuStrCat(formatted_name, filepath);
            NuStrCat(formatted_name, nufile_disc_suffix);
        }
        header->unknown_b4 = 0;
        nufile_buffering_enabled = saved_buffering;
        return header;
    }
    nufile_buffering_enabled = saved_buffering;
    return NULL;
}

NUDATHDR *__cdecl NuDatOpen(const char *filepath, VARIPTR *buf, i32 *unused) {
    return NuDatOpenEx(filepath, buf, unused, 0);
}

NUDATHDR *__cdecl NuDatSet(NUDATHDR *header) {
    NUDATHDR *dat = curr_dat;
    curr_dat = header;
    return dat;
}

NUDATHDR *__cdecl NuDatGet(void) {
    return curr_dat;
}

void __cdecl NuFileExtInit(i32 low_resolution) {
    memset(extensions, 0, sizeof(extensions));
    num_extensions = 0;

    NuFileAddExtensionPC(NuFileExt_ca3_PC, 0, 6);
    NuFileAddExtensionPC(NuFileExt_cbs_PC, 1, 6);
    NuFileAddExtensionPC(NuFileExt_360_ghg_PC, 2, 6);
    NuFileAddExtensionPC(NuFileExt_360_gsc_PC, 3, 6);
    NuFileAddExtensionPC(NuFileExt_360_tex_PC, 4, 6);
    NuFileAddExtensionPC(NuFileExt_cc2_PC, 5, 6);
    NuFileAddExtensionPC(NuFileExt_wavx_PC, 6, 6);
    NuFileAddExtensionPC(NuFileExt_wavm_PC, 7, 6);
    NuFileAddExtensionPC(NuFileExt_wmv_PC, 8, 6);

    NuFileAddExtensionPC(NuFileExt_ca3_PC, 0, 5);
    NuFileAddExtensionPC(NuFileExt_cbs_PC, 1, 5);
    NuFileAddExtensionPC(NuFileExt_ps3_ghg_PC, 2, 5);
    NuFileAddExtensionPC(NuFileExt_ps3_gsc_PC, 3, 5);
    NuFileAddExtensionPC(NuFileExt_ps3_tex_PC, 4, 5);
    NuFileAddExtensionPC(NuFileExt_cc2_PC, 5, 5);
    NuFileAddExtensionPC(NuFileExt_msf_PC, 6, 5);
    NuFileAddExtensionPC(NuFileExt_mib_PC, 7, 5);
    NuFileAddExtensionPC(NuFileExt_pam_PC, 8, 5);

    NuFileAddExtensionPC(NuFileExt_an3_PC, 0, 4);
    NuFileAddExtensionPC(NuFileExt_bsa_PC, 1, 4);

    NuFileAddExtensionPC(NuFileExt_pc_dds_PC, 4, 4);
    NuFileAddExtensionPC(NuFileExt_cu2_PC, 5, 4);
    NuFileAddExtensionPC(NuFileExt_wav_PC, 6, 4);
    NuFileAddExtensionPC(NuFileExt_mib_PC, 7, 4);
    NuFileAddExtensionPC(NuFileExt_bik_PC, 8, 4);
    NuFileAddExtensionPC(NuFileExt_pc_gsc_PC, 3, 4);
    NuFileAddExtensionPC(low_resolution ? NuFileExt_lr_pc_ghg_PC : NuFileExt_pc_ghg_PC, 2, 4);

    NuFileAddExtensionPC(NuFileExt_an3_PC, 0, 0);
    NuFileAddExtensionPC(NuFileExt_bsa_PC, 1, 0);
    NuFileAddExtensionPC(NuFileExt_ghg_PC, 2, 0);
    NuFileAddExtensionPC(SceneFileExtensionPc, 3, 0);
    NuFileAddExtensionPC(NuFileExt_pnt_PC, 4, 0);
    NuFileAddExtensionPC(NuFileExt_cu2_PC, 5, 0);
    NuFileAddExtensionPC(NuFileExt_vag_PC, 6, 0);
    NuFileAddExtensionPC(NuFileExt_mib_PC, 7, 0);
    NuFileAddExtensionPC(NuFileExt_pss_PC, 8, 0);

    NuFileAddExtensionPC(NuFileExt_ca3_PC, 0, 2);
    NuFileAddExtensionPC(NuFileExt_cbs_PC, 1, 2);
    NuFileAddExtensionPC(NuFileExt_chg_PC, 2, 2);
    NuFileAddExtensionPC(NuFileExt_csc_PC, 3, 2);
    NuFileAddExtensionPC(NuFileExt_ctx_PC, 4, 2);
    NuFileAddExtensionPC(NuFileExt_cc2_PC, 5, 2);
    NuFileAddExtensionPC(NuFileExt_dsp_PC, 6, 2);
    NuFileAddExtensionPC(NuFileExt_gcm_PC, 7, 2);
    NuFileAddExtensionPC(NuFileExt_h4m_PC, 8, 2);

    NuFileAddExtensionPC(NuFileExt_an3_PC, 0, 1);
    NuFileAddExtensionPC(NuFileExt_bsa_PC, 1, 1);
    NuFileAddExtensionPC(NuFileExt_hx2_PC, 2, 1);
    NuFileAddExtensionPC(NuFileExt_nx2_PC, 3, 1);
    NuFileAddExtensionPC(NuFileExt_dds_PC, 4, 1);
    NuFileAddExtensionPC(NuFileExt_cu2_PC, 5, 1);
    NuFileAddExtensionPC(NuFileExt_wavx_PC, 6, 1);
    NuFileAddExtensionPC(NuFileExt_wavm_PC, 7, 1);
    NuFileAddExtensionPC(NuFileExt_wmv_PC, 8, 1);

    NuFileAddExtensionPC(NuFileExt_an3_PC, 0, 3);
    NuFileAddExtensionPC(NuFileExt_bsa_PC, 1, 3);
    NuFileAddExtensionPC(NuFileExt_phg_PC, 2, 3);
    NuFileAddExtensionPC(NuFileExt_psc_PC, 3, 3);
    NuFileAddExtensionPC(NuFileExt_pnt_PC, 4, 3);
    NuFileAddExtensionPC(NuFileExt_cu2_PC, 5, 3);
    NuFileAddExtensionPC(NuFileExt_vag_PC, 6, 3);
    NuFileAddExtensionPC(NuFileExt_at3_PC, 7, 3);
    NuFileAddExtensionPC(NuFileExt_pss_PC, 8, 3);
}

static i32 NuFileMatchExtensionPC(char *extension, char *path_end, i32 path_len) {
    while (*extension != '\0') {
        u8 value;
        --path_end;
        if (path_len == 0) {
            return 0;
        }
        value = (u8)*path_end;
        if ((value >= 'A' && value <= 'Z') || (value >= 0xc0 && value <= 0xdf)) {
            value += 0x20;
        }
        if (*extension != (char)value) {
            return 0;
        }
        ++extension;
        --path_len;
    }
    return 1;
}

static PC_FILEEXTINFO *NuFileExtGetInfoPC(char *path, i32 path_len) {
    PC_FILEEXTINFO *info;
    if (path_len == 0) {
        while (path[path_len] != '\0') {
            ++path_len;
        }
    }
    info = extensions;
    while (info->extension[0] != '\0') {
        if (NuFileMatchExtensionPC(info->extension, path + path_len, path_len) != 0) {
            return info;
        }
        ++info;
    }
    return NULL;
}

i32 __cdecl NuFileExtGetType(char *path, i32 path_len) {
    PC_FILEEXTINFO *info = NuFileExtGetInfoPC(path, path_len);
    if (info != NULL) {
        return info->type;
    }
    return -1;
}

i32 __cdecl NuFileExtConvert(char *dest, char *path) {
    i32 path_len;
    char *out = dest;
    const char *in = path;
    PC_FILEEXTINFO *source;
    PC_FILEEXTINFO *target;
    if (in != NULL) {
        while (*in != '\0') {
            *out++ = *in++;
        }
    }
    *out = '\0';
    path_len = (i32)(out - dest);
    source = NuFileExtGetInfoPC(path, path_len);
    if (source != NULL) {
        if (source->platform == 4) {
            return 1;
        }
        /* Retail uses the same pointer termination here. */
        for (target = extensions; target != NULL; ++target) {
            if (target->platform == 4 && target->type == source->type) {
                char *extension;
                out = dest + path_len - source->len + target->len;
                *out = '\0';
                for (extension = target->extension; *extension != '\0'; ++extension) {
                    *--out = *extension;
                }
                return 1;
            }
        }
    }
    return 0;
}

i32 __cdecl NuFileExtGetExt(char *dest, i32 dest_size, i32 type) {
    PC_FILEEXTINFO *info;
    /* Retail terminates this scan when the pointer wraps to zero. */
    for (info = extensions; info != NULL; ++info) {
        if (info->platform == 4 && info->type == type) {
            i32 i;
            const char *source;
            if (info->len > dest_size) {
                return 0;
            }
            source = info->extension + info->len;
            for (i = 0; i < info->len; ++i) {
                dest[i] = *--source;
            }
            dest[info->len] = '\0';
            return 1;
        }
    }
    return 0;
}

i32 __cdecl NuFileExtRemove(char *dest, char *path) {
    i32 length;
    char *out = dest;
    const char *in = path;
    PC_FILEEXTINFO *info;
    if (in != NULL) {
        while (*in != '\0') {
            *out++ = *in++;
        }
    }
    *out = '\0';
    length = (i32)(out - dest);
    info = NuFileExtGetInfoPC(path, length);
    if (info != NULL) {
        dest[length - info->len] = '\0';
        return info->type;
    }
    return -1;
}

i32 __cdecl NuFilePakGetLastError(void) {
    return fpk_err;
}

void *__cdecl NuFilePakLoadKey(char *filepath, VARIPTR *buffer, VARIPTR end, i32 alignment, const u8 *key,
                               u32 key_length) {
    PC_FILEPAKHDR *header;
    i32 length;
    u32 checksum;
    fpk_err = 0;
    if (alignment > 0x2000) {
        NuFileRetry();
        fpk_err = 5;
        return NULL;
    }
    if (alignment <= 4)
        alignment = 4;
    buffer->addr = (buffer->addr + alignment - 1) & ~(alignment - 1);
    header = (PC_FILEPAKHDR *)buffer->void_ptr;
    length = NuFileLoadBufferVP(filepath, buffer, &end);
    if (length == 0) {
        fpk_err = 1;
        return NULL;
    }
    if ((u32)length != header->file_size) {
        NuFileRetry();
        fpk_err = 5;
        return NULL;
    }
    switch (header->type) {
        case 0x12345679:
            checksum = header->checksum;
            header->checksum = 0;
            if (checksum != CalcChecksum(header, length)) {
                NuFileRetry();
                fpk_err = 3;
                return NULL;
            }
            break;
        case 0x1234567a:
            checksum = header->checksum;
            header->checksum = 0;
            if (checksum != CalcChecksum(header, length)) {
                NuFileRetry();
                fpk_err = 3;
                return NULL;
            }

            if ((header->flags & 1) != 0) {
                if (key == NULL) {
                    NuFileRetry();
                    fpk_err = 6;
                    return NULL;
                }

                DecryptData(header, length, key, (i32)key_length);

                checksum = header->decrypted_checksum;
                header->decrypted_checksum = 0;
                if (checksum != CalcChecksum(header, length)) {
                    NuFileRetry();
                    fpk_err = 6;
                    return NULL;
                }
            }
            break;
        case 0x12345678:
            break;
        default:
            NuFileRetry();
            fpk_err = 4;
            return NULL;
    }
    return header;
}

void *__cdecl NuFilePakLoad(char *filepath, VARIPTR *buffer, VARIPTR end, i32 alignment) {
    return NuFilePakLoadKey(filepath, buffer, end, alignment, NULL, 0);
}

i32 __cdecl NuFilePakCondense(void *pak) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    PC_FILEPAKITEM *items;
    i32 removed_size;
    u32 index;

    fpk_err = 0;
    items = GetItems(header);
    removed_size = 0;
    for (index = 0; index < header->item_count; ++index) {
        PC_FILEPAKITEM *item = &items[index];

        if ((item->flags & 2) != 0) {
            continue;
        }
        if ((item->flags & 1) != 0) {
            if (removed_size != 0 && item->alignment != 0) {
                removed_size &= ~(item->alignment - 1);
            }
            if (removed_size != 0) {
                PCByteCopy_candidate((char *)header + item->data_offset - removed_size,
                                     (char *)header + item->data_offset, item->size);
                item->data_offset -= removed_size;
            }
        } else {
            item->flags |= 2;
            removed_size = (removed_size + item->size + item->alignment - 1) & ~(item->alignment - 1);
        }
    }
    header->file_size -= removed_size;
    return removed_size;
}

void __cdecl NuFilePakRelocate(void *pak, void *destination) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    u32 size = header->file_size;

    fpk_err = 0;
    PCByteCopy_candidate(destination, header, size);
}

i32 __cdecl NuFilePakGetItem(void *pak, char *name) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    PC_FILEPAKITEM *items;
    u32 index;

    fpk_err = 0;
    items = GetItems(header);
    for (index = 0; index < header->item_count; ++index) {
        if (NuStrICmp((char *)header + items[index].name_offset, name) == 0) {
            if ((items[index].flags & 2) != 0) {
                return 0;
            }
            return index + 1;
        }
    }
    return 0;
}

i32 __cdecl NuFilePakGetItemInfo(void *pak, i32 handle, void **address, i32 *size) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    PC_FILEPAKITEM *item;

    fpk_err = 0;
    --handle;
    if (handle >= 0 && (u32)handle < header->item_count) {
        item = GetItems(header) + handle;
        if ((item->flags & 2) == 0) {
            if (address != NULL) {
                *address = (char *)header + item->data_offset;
            }
            if (size != NULL) {
                *size = item->size;
            }
            return 1;
        }
    }
    return 0;
}

i32 __cdecl NuFilePakSetItemRequired(void *pak, i32 handle, i32 required) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    PC_FILEPAKITEM *item;

    fpk_err = 0;
    --handle;
    if (handle < 0 || (u32)handle >= header->item_count) {
        return 0;
    }
    item = GetItems(header) + handle;
    if ((item->flags & 2) == 0) {
        item->flags = (item->flags & ~1u) | (required & 1);
    }
    return 1;
}

u32 __cdecl NuFilePakGetSize(void *pak) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    u32 size = header->file_size;

    fpk_err = 0;
    return size;
}

char __cdecl NuGetChar(NUFPAR_PC *parser) {
    char c;
    i32 len;

    i32 read_to = parser->buffer_end >= 0 ? parser->buffer_end : 0;

    if (parser->character_position > parser->buffer_end) {
        if (parser->buffer_end + 1 <= parser->file_size) {
            len = NuFileRead(parser->file_handle, parser->file_buffer,
                             parser->file_size - read_to <= 0x1000 ? parser->file_size - read_to : 0x1000);

            parser->buffer_start = parser->buffer_end + 1;
            parser->buffer_end += len;
        } else {
            return '\0';
        }

        if (len == 0) {
            return '\0';
        }
    }

    c = parser->file_buffer[parser->character_position - parser->buffer_start];
    parser->character_position++;

    return c;
}

u16 __cdecl NuGetWChar(NUFPAR_PC *parser) {
    unsigned char lo;
    unsigned char hi;
    u16 c;

    lo = NuGetChar(parser);
    hi = NuGetChar(parser);

    c = lo + (hi << 8);

    return c;
}

i32 __cdecl NuFParGetLineW(NUFPAR_PC *parser) {
    u16 c;
    i32 len;
    u16 *line;
    i32 at_end_of_line;
    i32 is_done;
    i32 in_quoted_text;

    in_quoted_text = 0;
    line = (u16 *)parser->line_buffer_ptr;
    len = 0;
    parser->line_position = 0;
    parser->line_number++;
    is_done = 0;

    do {
        c = NuGetWChar(parser);
        switch (c) {
            case '\r':
                NuGetWChar(parser);
            case '\n':
                if (len == 0 || !is_done) {
                    parser->line_number++;
                    is_done = 0;

                    break;
                }
            case '\0':
                line[len] = '\0';
                return len;
            case '"':
                in_quoted_text = 1 - in_quoted_text;
                is_done = 1;
                line[CLAMP_WIDE_WORD(len)] = c;
                len++;
                break;
            case ';':
                if (!in_quoted_text) {
                    if (len == 0 || !is_done) {
                        at_end_of_line = 0;

                        do {
                            c = NuGetWChar(parser);
                            switch (c) {
                                case '\r':
                                    NuGetWChar(parser);
                                case '\n':
                                case '\0':
                                    at_end_of_line = 1;
                                    break;
                                default:
                                    break;
                            }
                        } while (!at_end_of_line);

                        len = 0;
                        parser->line_position = 0;
                        parser->line_number++;
                        is_done = 0;
                    } else {
                        do {
                            c = NuGetWChar(parser);
                            switch (c) {
                                case '\r':
                                    NuGetWChar(parser);
                                case '\n':
                                case '\0':
                                    line[len] = '\0';
                                    return len;
                                default:
                                    break;
                            }
                        } while (1);

                        return len;
                    }

                    break;
                }
            default:
                is_done = 1;
            case '\t':
            case ' ':
                line[CLAMP_WIDE_WORD(len)] = c;
                len++;
                break;
        }
    } while (1);
}

i32 __cdecl NuFParGetLine(NUFPAR_PC *parser) {
    char c;
    i32 len;
    i32 at_end_of_line;
    i32 is_done;
    i32 in_quoted_text;

    in_quoted_text = 0;

    if (parser->is_utf16) {
        return NuFParGetLineW(parser);
    }

    len = 0;
    parser->line_position = 0;
    parser->line_number++;
    is_done = 0;

    do {
        c = NuGetChar(parser);
        switch (c) {
            case '\r':
                NuGetChar(parser);
            case '\n':
                if (len == 0 || !is_done) {
                    parser->line_number++;
                    is_done = 0;

                    break;
                }
            case '\0':
                parser->line_buffer_ptr[len] = '\0';
                return len;
            case '"':
                in_quoted_text = 1 - in_quoted_text;
                is_done = 1;
                parser->line_buffer_ptr[CLAMP_LINE(len)] = c;
                len++;
                break;
            case ';':
                if (!in_quoted_text) {
                    if (len == 0 || !is_done) {
                        at_end_of_line = 0;

                        do {
                            c = NuGetChar(parser);
                            switch (c) {
                                case '\r':
                                    NuGetChar(parser);
                                case '\n':
                                case '\0':
                                    at_end_of_line = 1;
                                    break;
                                default:
                                    break;
                            }
                        } while (!at_end_of_line);

                        len = 0;
                        parser->line_position = 0;
                        parser->line_number++;
                        is_done = 0;
                    } else {
                        do {
                            c = NuGetChar(parser);
                            switch (c) {
                                case '\r':
                                    NuGetChar(parser);
                                case '\n':
                                case '\0':
                                    parser->line_buffer_ptr[len] = '\0';
                                    return len;
                                default:
                                    break;
                            }
                        } while (1);

                        return len;
                    }

                    break;
                }
            default:
                is_done = 1;
            case '\t':
            case ' ':
                parser->line_buffer_ptr[CLAMP_LINE(len)] = c;
                len++;
                break;
        }
    } while (1);
}

void __cdecl NuFParUnGetWord(NUFPAR_PC *parser) {
    parser->line_position = old_line_pos;
}

i32 __cdecl NuFParGetWordW(NUFPAR_PC *parser) {
    i32 length;
    i32 in_quoted_text;
    u16 *line;
    u16 *word;

    old_line_pos = parser->line_position;
    line = (u16 *)parser->line_buffer_ptr;
    word = (u16 *)parser->word_buffer_ptr;
    length = 0;
    in_quoted_text = 0;

    while (line[parser->line_position & ((parser->line_buffer_size >> 1) - 1)] != 0) {
        u16 character = line[parser->line_position];

        if (parser->separator_tokens != NULL && !in_quoted_text &&
            NuStrChr(parser->separator_tokens, (char)character) != NULL) {
            if (length == 0) {
                word[0] = character;
                length++;
                parser->line_position++;
            }
            word[length & ((parser->word_buffer_size >> 1) - 1)] = 0;
            return length;
        }

        if (parser->separator_list != NULL && !in_quoted_text &&
            NuStrChr(parser->separator_list, (char)character) != NULL) {
            character = ' ';
        }

        switch (character & 0xff) {
            case ' ':
            case ',':
            case '\t':
                if (!in_quoted_text) {
                    if (length != 0) {
                        word[length & ((parser->word_buffer_size >> 1) - 1)] = 0;
                        return length;
                    }
                    break;
                }
                /* fall through */
            default:
                if (character == '"') {
                    if (in_quoted_text && line[parser->line_position + 1] == '"') {
                        word[length & ((parser->word_buffer_size >> 1) - 1)] = '"';
                        length++;
                        parser->line_position++;
                    } else {
                        in_quoted_text = 1 - in_quoted_text;
                    }
                } else {
                    word[length & ((parser->word_buffer_size >> 1) - 1)] = character;
                    length++;
                }
                break;
        }

        parser->line_position++;
    }

    word[length & ((parser->word_buffer_size >> 1) - 1)] = 0;
    return length;
}

i32 __cdecl NuFParGetWord(NUFPAR_PC *parser) {
    i32 length = 0;
    i32 in_quoted_text = 0;
    i32 found_quotes = 0;

    if (parser->is_utf16 != 0) {
        return NuFParGetWordW(parser);
    }

    old_line_pos = parser->line_position;

    while (parser->line_buffer_ptr[parser->line_position & (parser->line_buffer_size - 1)] != '\0') {
        char character = parser->line_buffer_ptr[parser->line_position];

        if (parser->separator_tokens != NULL && !in_quoted_text &&
            NuStrChr(parser->separator_tokens, character) != NULL) {
            if (length == 0) {
                parser->word_buffer_ptr[0] = character;
                length++;
                parser->line_position++;
            }
            parser->word_buffer_ptr[length & (parser->word_buffer_size - 1)] = '\0';
            return length;
        }

        if (parser->separator_list != NULL && !in_quoted_text && NuStrChr(parser->separator_list, character) != NULL) {
            character = ' ';
        }

        switch (character) {
            case ' ':
            case ',':
            case '\t':
                if (!in_quoted_text) {
                    if (length != 0) {
                        parser->word_buffer_ptr[length & (parser->word_buffer_size - 1)] = '\0';
                        return length;
                    }
                    break;
                }
                /* fall through */
            default:
                if (character == '"') {
                    in_quoted_text = 1 - in_quoted_text;
                    found_quotes = 1;
                } else {
                    parser->word_buffer_ptr[length & (parser->word_buffer_size - 1)] = character;
                    length++;
                }
                break;
        }

        parser->line_position++;
        if (found_quotes && in_quoted_text == 0 && length == 0) {
            break;
        }
    }

    parser->word_buffer_ptr[length & (parser->word_buffer_size - 1)] = '\0';
    return length;
}

i32 __cdecl NuFParGetInt(NUFPAR_PC *parser) {
    char buffer[64];

    NuFParGetWord(parser);
    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }

    if (buffer[0] == '\0') {
        return 0;
    }
    if (buffer[0] == '$') {
        return NuHexStringToI(buffer + 1);
    }
    if (buffer[0] == '0' && (buffer[1] == 'x' || buffer[1] == 'X')) {
        return NuHexStringToI(buffer + 2);
    }
    return NuAToI(buffer);
}

f32 __cdecl NuFParGetFloat(NUFPAR_PC *parser) {
    char buffer[64];

    NuFParGetWord(parser);
    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }

    return buffer[0] != '\0' ? NuAToF(buffer) : 0.0f;
}

void __cdecl NuFParGetOptionalFloat(NUFPAR_PC *parser, f32 *value) {
    char buffer[64];

    NuFParGetWord(parser);
    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }

    if (value != NULL && buffer[0] != '\0') {
        *value = NuAToF(buffer);
        if (*value == 0.0f) {
            NuFParUnGetWord(parser);
        }
    }
}

void __cdecl NuFParGetOptionalInt(NUFPAR_PC *parser, i32 *value) {
    char buffer[64];

    NuFParGetWord(parser);
    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }

    if (value != NULL && buffer[0] != '\0') {
        *value = NuAToI(buffer);
        if (*value == 0) {
            NuFParUnGetWord(parser);
        }
    }
}

i32 __cdecl NuFParGetIntRDP(NUFPAR_PC *parser) {
    char buffer[64];

    NuFParGetWord(parser);
    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }
    return buffer[0] != '\0' ? NuRDPI(buffer) : 0;
}

f32 __cdecl NuFParGetFloatRDP(NUFPAR_PC *parser) {
    char buffer[64];

    NuFParGetWord(parser);
    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }

    if (buffer[0] != '\0') {
        return NuRDPF(buffer);
    } else {
        return 0.0f;
    }
}

static i32 isnum(char c) {
    switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return 1;
        default:
            return 0;
    }
}

static void NuRDPIGetToken(NURDPICTX_PC *context) {
    char variable[256];
    i32 length;

    for (;;) {
        if (context->input[context->position] == '\0') {
            context->token = RDPI_END;
            return;
        }
        if (context->input[context->position] != ' ' && context->input[context->position] != '\t') {
            break;
        }
        /* The PC increments the whitespace byte, not the input position. */
        context->input[context->position]++;
    }

    switch (context->input[context->position]) {
        case ';':
            context->token = RDPI_END;
            return;
        case '+':
            context->token = RDPI_ADD;
            context->position++;
            return;
        case '-':
            context->token = RDPI_SUBTRACT;
            context->position++;
            return;
        case '*':
            context->token = RDPI_MULTIPLY;
            context->position++;
            return;
        case '/':
            context->token = RDPI_DIVIDE;
            context->position++;
            return;
        case '%':
            context->token = RDPI_MODULO;
            context->position++;
            return;
        case '(':
            context->token = RDPI_LEFT_PAREN;
            context->position++;
            return;
        case ')':
            context->token = RDPI_RIGHT_PAREN;
            context->position++;
            return;
        case '|':
            context->token = RDPI_OR;
            context->position++;
            if (context->input[context->position] == '|') {
                context->token = RDPI_LOGICAL_OR;
                context->position++;
            }
            return;
        case '&':
            context->token = RDPI_AND;
            context->position++;
            if (context->input[context->position] == '&') {
                context->token = RDPI_LOGICAL_AND;
                context->position++;
            }
            return;
        case '=':
            context->token = RDPI_EQUAL;
            context->position++;
            if (context->input[context->position] == '=') {
                context->position++;
            }
            return;
        case '>':
            context->token = RDPI_GREATER;
            context->position++;
            if (context->input[context->position] == '=') {
                context->position++;
                context->token = RDPI_GREATER_EQUAL;
            }
            return;
        case '<':
            context->token = RDPI_LESS;
            context->position++;
            if (context->input[context->position] == '=') {
                context->position++;
                context->token = RDPI_LESS_EQUAL;
            }
            return;
        case '^':
            context->token = RDPI_XOR;
            context->position++;
            return;
        case '!':
            context->token = RDPI_LOGICAL_NOT;
            context->position++;
            if (context->input[context->position] == '=') {
                context->token = RDPI_NOT_EQUAL;
                context->position++;
            }
            return;
        case '~':
            context->token = RDPI_NOT;
            context->position++;
            return;
        case '$':
            context->position++;
            context->token = RDPI_NUMBER;
            context->value = 0;
            while (ishexnum(context->input[context->position])) {
                context->value *= 16;
                context->value += hexcodetoint(context->input[context->position]);
                context->position++;
            }
            return;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            context->token = RDPI_NUMBER;
            context->value = 0;
            while (isnum(context->input[context->position])) {
                context->value *= 10;
                context->value += context->input[context->position] - '0';
                context->position++;
            }
            return;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
            length = 0;
            while (NuIsAlNum(context->input[context->position])) {
                variable[length++] = context->input[context->position++];
            }
            variable[length] = '\0';
            if (context->get_variable == NULL) {
                NuRDPIVariableErrorPc(variable);
            }
            context->value = 0;
            context->token = RDPI_NUMBER;
            context->get_variable(variable, NULL, &context->value);
            return;
        default:
            return;
    }
}

static i32 NuRDPIPrimary(NURDPICTX_PC *context) {
    i32 value;

    switch (context->token) {
        case RDPI_NUMBER:
            NuRDPIGetToken(context);
            return context->value;
        case RDPI_SUBTRACT:
            NuRDPIGetToken(context);
            return -NuRDPIPrimary(context);
        case RDPI_NOT:
            NuRDPIGetToken(context);
            return ~NuRDPIPrimary(context);
        case RDPI_LOGICAL_NOT:
            NuRDPIGetToken(context);
            return !NuRDPIPrimary(context);
        case RDPI_LEFT_PAREN:
            NuRDPIGetToken(context);
            value = NuRDPIExpression(context);
            if (context->token != RDPI_RIGHT_PAREN) {
                NuFileError();
            }
            NuRDPIGetToken(context);
            return value;
        case RDPI_END:
            return 1;
        default:
            NuFileError();
            return 0;
    }
}

static i32 NuRDPITerm(NURDPICTX_PC *context) {
    i32 value = NuRDPIPrimary(context);

    for (;;) {
        switch (context->token) {
            case RDPI_MULTIPLY:
                NuRDPIGetToken(context);
                value *= NuRDPIPrimary(context);
                break;
            case RDPI_DIVIDE:
                NuRDPIGetToken(context);
                value /= NuRDPIPrimary(context);
                break;
            case RDPI_MODULO:
                NuRDPIGetToken(context);
                value %= NuRDPIPrimary(context);
                break;
            case RDPI_EQUAL:
                NuRDPIGetToken(context);
                value = value == NuRDPIPrimary(context);
                break;
            case RDPI_NOT_EQUAL:
                NuRDPIGetToken(context);
                value = value != NuRDPIPrimary(context);
                break;
            case RDPI_LESS:
                NuRDPIGetToken(context);
                value = value < NuRDPIPrimary(context);
                break;
            case RDPI_GREATER:
                NuRDPIGetToken(context);
                value = value > NuRDPIPrimary(context);
                break;
            case RDPI_LESS_EQUAL:
                NuRDPIGetToken(context);
                value = value <= NuRDPIPrimary(context);
                break;
            case RDPI_GREATER_EQUAL:
                NuRDPIGetToken(context);
                value = value >= NuRDPIPrimary(context);
                break;
            default:
                return value;
        }
    }
}

static i32 NuRDPIExpression(NURDPICTX_PC *context) {
    i32 right;
    i32 value = NuRDPITerm(context);

    for (;;) {
        switch (context->token) {
            case RDPI_ADD:
                NuRDPIGetToken(context);
                value += NuRDPITerm(context);
                break;
            case RDPI_SUBTRACT:
                NuRDPIGetToken(context);
                value -= NuRDPITerm(context);
                break;
            case RDPI_AND:
                NuRDPIGetToken(context);
                value &= NuRDPITerm(context);
                break;
            case RDPI_OR:
                NuRDPIGetToken(context);
                value |= NuRDPITerm(context);
                break;
            case RDPI_XOR:
                NuRDPIGetToken(context);
                value ^= NuRDPITerm(context);
                break;
            case RDPI_LOGICAL_AND:
                NuRDPIGetToken(context);
                right = NuRDPITerm(context);
                value = value != 0 && right != 0;
                break;
            case RDPI_LOGICAL_OR:
                NuRDPIGetToken(context);
                right = NuRDPITerm(context);
                value = value != 0 || right != 0;
                break;
            default:
                return value;
        }
    }
}

void __cdecl NuFParGetPos(NUFPAR_PC *parser, NUFPARPOS_PC *position) {
    position->line_number = parser->line_number;
    position->line_position = parser->line_position;
    position->character_position = parser->character_position;
    position->buffer_start = parser->buffer_start;
    position->buffer_end = parser->buffer_end;
}

void __cdecl NuFParSetPos(NUFPAR_PC *parser, NUFPARPOS_PC *position) {
    parser->character_position = position->character_position;
    parser->line_position = position->line_position;

    if (parser->character_position < parser->buffer_start || parser->character_position > parser->buffer_end) {
        parser->buffer_end = parser->character_position - 1;
    }
}

i32 __cdecl NuFParPushCom(NUFPAR_PC *parser, NUFPCOMJMP *commands) {
    if (parser->command_position >= 7) {
        return -1;
    }

    parser->command_position++;
    parser->command_stack[parser->command_position] = commands;
    parser->command_stack2[parser->command_position] = NULL;
    return parser->command_position;
}

i32 __cdecl NuFParPushComCTX(NUFPAR_PC *parser, NUFPCOMJMPCTX *commands) {
    if (parser->command_position >= 7) {
        return -1;
    }

    parser->command_position++;
    parser->command_stack_ctx[parser->command_position] = commands;
    parser->command_stack2_ctx[parser->command_position] = NULL;
    return parser->command_position;
}

i32 __cdecl NuFParPushCom2(NUFPAR_PC *parser, NUFPCOMJMP *commands, NUFPCOMJMP *commands2) {
    if (parser->command_position >= 7) {
        return -1;
    }
    parser->command_position++;
    parser->command_stack[parser->command_position] = commands;
    parser->command_stack2[parser->command_position] = commands2;
    return parser->command_position;
}

i32 __cdecl NuFParPushComCTX2(NUFPAR_PC *parser, NUFPCOMJMPCTX *commands, NUFPCOMJMPCTX *commands2) {
    if (parser->command_position >= 7) {
        return -1;
    }

    parser->command_position++;
    parser->command_stack_ctx[parser->command_position] = commands;
    parser->command_stack2_ctx[parser->command_position] = commands2;
    return parser->command_position;
}

void __cdecl NuFParPopCom(NUFPAR_PC *parser) {
    if (parser->command_position >= 0) {
        parser->command_position--;
    }
}

i32 __cdecl NuFParInterpretWord(NUFPAR_PC *parser) {
    char buffer[64];
    i32 index;
    NUFPCOMJMP *commands;

    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }
    if (buffer[0] == '\0')
        return 0;
    if (buffer[0] == ';')
        return 0;

    if (parser->command_position >= 0) {
        commands = parser->command_stack[parser->command_position];
        for (index = 0; commands[index].keyword != NULL; index++) {
            if (NuStrICmp(commands[index].keyword, buffer) == 0) {
                commands[index].callback(parser);
                return 1;
            }
        }
        commands = parser->command_stack2[parser->command_position];
        if (commands != NULL) {
            for (index = 0; commands[index].keyword != NULL; index++) {
                if (NuStrICmp(commands[index].keyword, buffer) == 0) {
                    commands[index].callback(parser);
                    return 1;
                }
            }
        }
    }

    if (fnInterpreterError != NULL) {
        fnInterpreterError(parser);
    }
    return 0;
}

i32 __cdecl NuFParInterpretWordCTX(NUFPAR_PC *parser, void *context) {
    char buffer[64];
    i32 index;
    NUFPCOMJMPCTX *commands;

    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }
    if (buffer[0] != '\0' && buffer[0] != ';') {
        if (parser->command_position > -1) {
            commands = parser->command_stack_ctx[parser->command_position];
            for (index = 0; commands[index].keyword != NULL; index++) {
                if (NuStrICmp(commands[index].keyword, buffer) == 0) {
                    commands[index].callback(parser, context);
                    return 1;
                }
            }
            commands = parser->command_stack2_ctx[parser->command_position];
            if (commands != NULL) {
                for (index = 0; commands[index].keyword != NULL; index++) {
                    if (NuStrICmp(commands[index].keyword, buffer) == 0) {
                        commands[index].callback(parser, context);
                        return 1;
                    }
                }
            }
        }

        if (fnInterpreterError != NULL) {
            fnInterpreterError(parser);
        }
    }
    return 0;
}

/* Empty release diagnostics; the linker folds them into NuFileError. */
void __cdecl NuRDPFNumberErrorPc(void) {
}

void __cdecl NuRDPFVariableErrorPc(char *name) {
    (void)name;
}

static void NuRDPFGetToken(NURDPFCTX_PC *context) {
    char variable[256];
    i32 length;
    f32 multiplier;

    /* Increments the whitespace byte, not the input position. */
    for (;;) {
        if (context->input[context->position] == '\0') {
            context->token = RDPF_END;
            return;
        }
        if (context->input[context->position] == ' ') {
            context->input[context->position]++;
            continue;
        }
        if (context->input[context->position] == '\t') {
            context->input[context->position]++;
            continue;
        }
        break;
    }

    switch (context->input[context->position]) {
        case ';':
            context->token = RDPF_END;
            return;
        case '+':
            context->token = RDPF_ADD;
            context->position++;
            return;
        case '-':
            context->token = RDPF_SUBTRACT;
            context->position++;
            return;
        case '*':
            context->token = RDPF_MULTIPLY;
            context->position++;
            return;
        case '/':
            context->token = RDPF_DIVIDE;
            context->position++;
            return;
        case '(':
            context->token = RDPF_LEFT_PAREN;
            context->position++;
            return;
        case ')':
            context->token = RDPF_RIGHT_PAREN;
            context->position++;
            return;
        case '.':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            context->token = RDPF_NUMBER;
            context->value = 0.0f;
            multiplier = 0.0f;
            while ((context->input[context->position] >= '0' && context->input[context->position] <= '9') ||
                   context->input[context->position] == '.') {
                if (context->input[context->position] == '.') {
                    if (multiplier != 0.0f) {
                        NuRDPFNumberErrorPc();
                    }
                    multiplier = 0.1f;
                } else if (multiplier != 0.0f) {
                    context->value += (context->input[context->position] - '0') * multiplier;
                    multiplier /= 10.0;
                } else {
                    context->value *= 10.0;
                    context->value += context->input[context->position] - '0';
                }
                context->position++;
            }
            return;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
            length = 0;
            while (NuIsAlNum(context->input[context->position])) {
                variable[length++] = context->input[context->position++];
            }
            variable[length] = '\0';
            if (context->get_variable == NULL) {
                NuRDPFVariableErrorPc(variable);
            }
            context->value = 0.0f;
            context->token = RDPF_NUMBER;
            context->get_variable(variable, &context->value, NULL);
            return;
        default:
            return;
    }
}

static f32 NuRDPFPrimary(NURDPFCTX_PC *context) {
    f32 value;

    switch (context->token) {
        case RDPF_VARIABLE:
            NuFileError();
            /* fall through */
        case RDPF_NUMBER:
            NuRDPFGetToken(context);
            return context->value;
        case RDPF_SUBTRACT:
            NuRDPFGetToken(context);
            return -NuRDPFPrimary(context);
        case RDPF_LEFT_PAREN:
            NuRDPFGetToken(context);
            value = NuRDPFExpression(context);
            if (context->token != RDPF_RIGHT_PAREN) {
                NuFileError();
            }
            NuRDPFGetToken(context);
            return value;
        case RDPF_END:
            return 1.0f;
        default:
            NuFileError();
            return 0.0f;
    }
}

static f32 NuRDPFTerm(NURDPFCTX_PC *context) {
    f32 value = NuRDPFPrimary(context);

    for (;;) {
        switch (context->token) {
            case RDPF_MULTIPLY:
                NuRDPFGetToken(context);
                value *= NuRDPFPrimary(context);
                break;
            case RDPF_DIVIDE:
                NuRDPFGetToken(context);
                value /= NuRDPFPrimary(context);
                break;
            default:
                return value;
        }
    }
}

static f32 NuRDPFExpression(NURDPFCTX_PC *context) {
    f32 value = NuRDPFTerm(context);

    for (;;) {
        switch (context->token) {
            case RDPF_ADD:
                NuRDPFGetToken(context);
                value += NuRDPFTerm(context);
                break;
            case RDPF_SUBTRACT:
                NuRDPFGetToken(context);
                value -= NuRDPFTerm(context);
                break;
            default:
                return value;
        }
    }
}

NUFPCOMFN __cdecl NuFParSetInterpreterErrorHandler(NUFPCOMFN handler) {
    NUFPCOMFN previous = fnInterpreterError;

    fnInterpreterError = handler;
    return previous;
}

void __cdecl NuFParSuspend(NUFPAR_PC *parser) {
    NUFILE file_handle = parser->file_handle;

    if (file_handle >= 0x400 && file_handle < 0x800) {
        return;
    }

    if (parser->file_name[0] == '\0' || file_handle == 0) {
        NuFileError();
    }

    NuFileClose(file_handle);
    parser->file_handle = 0;
}

void __cdecl NuFParResume(NUFPAR_PC *parser) {
    NUFILE file_handle;

    if (parser->file_handle >= 0x400 && parser->file_handle < 0x800) {
        return;
    }

    if (parser->file_name[0] == '\0') {
        NuFileError();
    }

    file_handle = NuFileOpenDF(parser->file_name, 0, curr_dat, 0);
    parser->file_handle = file_handle;
    if (file_handle == 0) {
        NuFileError();
    }

    (void)NuFileSeek(file_handle, parser->character_position, 0);
    parser->buffer_end = parser->character_position - 1;
}

void __cdecl NuFParInit(i32 max_count, i32 buffer_size) {
    NuFParRecordBase = NuFParRecords;
    memset(NuFParRecordBase, 0, sizeof(NuFParRecords));
    NuFParBufferBase = &NuFParBuffers[0][0];
}

static NUFPAR_PC *NuAllocFPar(void) {
    i32 index;

    if (NuFParRecordBase == NULL) {
        NuFParRecordBase = NuFParRecords;
        memset(NuFParRecordBase, 0, sizeof(NuFParRecords));
        NuFParBufferBase = &NuFParBuffers[0][0];
    }

    for (index = 0; index < 4; index++) {
        NUFPAR_PC *parser = &NuFParRecordBase[index];

        if (parser->file_buffer == NULL) {
            memset(parser, 0, sizeof(*parser));
            parser->file_buffer = NuFParBufferBase + index * 0x1000;
            memset(parser->file_buffer, 0, 0x1000);
            return parser;
        }
    }

    return NULL;
}

void __cdecl NuFreeFPar(NUFPAR_PC *parser) {
    parser->file_buffer = NULL;
}

NUFPAR_PC *__cdecl NuFParOpen(NUFILE file_handle) {
    NUFPAR_PC *parser = NuAllocFPar();
    u16 first_word;

    if (parser != NULL) {
        parser->file_handle = file_handle;
        parser->buffer_end = -1;
        parser->line_number = -1;
        parser->command_position = -1;
        parser->separator_list = NULL;
        parser->separator_tokens = NULL;
        parser->line_buffer_ptr = parser->line_buffer;
        parser->word_buffer_ptr = parser->word_buffer;
        parser->line_buffer_size = 0x200;
        parser->word_buffer_size = 0x200;

        first_word = 0;
        NuFileRead(file_handle, &first_word, sizeof(first_word));
        parser->is_utf16 = 0;
        parser->is_utf8 = 0;

        if (first_word == 0xfffe) {
            parser->is_utf16 = 2;
        } else if (first_word == 0xfeff) {
            parser->is_utf16 = 1;
        } else if (first_word == 0xbbef) {
            if ((u8)NuFileReadChar(file_handle) == 0xbf) {
                parser->is_utf8 = 1;
            }
        } else if (first_word == 0xefbb) {
            if ((u8)NuFileReadChar(file_handle) == 0xbf) {
                parser->is_utf8 = 2;
            }
        }

        if (parser->is_utf16 == 0 && parser->is_utf8 == 0) {
            NuFileSeek(file_handle, 0, 0);
        }

        parser->file_size = (i32)NuFileOpenSize(file_handle);
    }
    return parser;
}

void __cdecl NuFParDestroy(NUFPAR_PC *parser) {
    NUFILE file_handle = parser->file_handle;

    NuFreeFPar(parser);
    NuFileClose(file_handle);
}

NUFPAR_PC *__cdecl NuFParCreate(char *file_name) {
    NUFILE file_handle;
    NUFPAR_PC *parser;

    if (NuFileExists(file_name)) {
        file_handle = NuFileOpenDF(file_name, 0, curr_dat, 0);
        if (file_handle != 0) {
            parser = NuFParOpen(file_handle);
            if (parser != NULL) {
                NuStrCpy(parser->file_name, file_name);
                return parser;
            }
            NuFileClose(file_handle);
        }
    }
    return NULL;
}

NUFPAR_PC *__cdecl NuFParCreateGivenFH(char *file_name, NUFILE file_handle) {
    NUFPAR_PC *parser;

    if (file_handle != 0) {
        parser = NuFParOpen(file_handle);
        if (parser != NULL) {
            NuStrCpy(parser->file_name, file_name);
            return parser;
        }
        NuFileClose(file_handle);
    }
    return NULL;
}

NUFPAR_PC *__cdecl NuFParCreateMem(char *name, char *buffer, i32 buffer_size) {
    NUFILE file_handle;
    NUFPAR_PC *parser;

    if (buffer_size != 0 && buffer != NULL) {
        file_handle = NuMemFileOpen(buffer, buffer_size, 0);
        if (file_handle != 0) {
            parser = NuFParOpen(file_handle);
            if (parser != NULL) {
                NuStrCpy(parser->file_name, name);
                return parser;
            }
            NuFileClose(file_handle);
        }
    }
    return NULL;
}

i32 __cdecl NuLinkedListCheck(NULISTHDR *list) {
    NULISTLNK *node;
    i32 count;
    NULISTLNK *previous = NULL;
    count = 0;
    node = list->head;
    while (node != NULL) {
        count++;
        previous = node;
        node = node->next;
    }
    return count;
}

void __cdecl NuLinkedListAppend(NULISTHDR *list, NULISTLNK *node) {
    node->next = NULL;
    node->prev = list->tail;
    if (list->tail != NULL) {
        list->tail->next = node;
    }
    list->tail = node;
    if (list->head == NULL) {
        list->head = node;
    }
}

void NuLinkedListInsert(NULISTHDR *list, NULISTLNK *node) {
    node->next = list->head;
    node->prev = NULL;

    if (list->head != NULL) {
        list->head->prev = node;
    }

    if (list->tail == NULL) {
        list->tail = node;
    }

    list->head = node;
}

void __cdecl NuLinkedListInsertBefore(NULISTHDR *list, NULISTLNK *position, NULISTLNK *node) {
    if (position != NULL) {
        node->next = position;
        node->prev = position->prev;
        if (position->prev != NULL) {
            position->prev->next = node;
        } else {
            list->head = node;
        }
        position->prev = node;
    } else {
        NuLinkedListInsert(list, node);
    }
}

void __cdecl NuLinkedListInsertAfter(NULISTHDR *list, NULISTLNK *position, NULISTLNK *node) {
    if (position != NULL) {
        node->prev = position;
        node->next = position->next;
        if (position->next != NULL) {
            position->next->prev = node;
        } else {
            list->tail = node;
        }
        position->next = node;
    } else {
        NuLinkedListAppend(list, node);
    }
}

void __cdecl NuLinkedListRemove(NULISTHDR *list, NULISTLNK *node) {
    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }
    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }
}

NULISTLNK *__cdecl NuLinkedListGetHead(NULISTHDR *list) {
    return list->head;
}

NULISTLNK *__cdecl NuLinkedListGetTail(NULISTHDR *list) {
    return list->tail;
}

NULISTLNK *__cdecl NuLinkedListGetNext(NULISTHDR *list, NULISTLNK *node) {
    return node != NULL ? node->next : list->head;
}

NULISTLNK *__cdecl NuLinkedListGetPrev(NULISTHDR *list, NULISTLNK *node) {
    return node != NULL ? node->prev : list->tail;
}

NUMEMBLK *__cdecl NuMemBlkCreate(i32 element_size, i32 count, u32 alignment_mask) {
    NUMEMBLKLINK *link;
    i32 i;
    u32 header_size;
    u32 stride;
    NUMEMBLK *pool;
    u32 data;
    u32 stride_words;
    if (element_size < sizeof(NUMEMBLKLINK))
        element_size = sizeof(NUMEMBLKLINK);
    header_size = (sizeof(NUMEMBLK) + alignment_mask) & ~alignment_mask;
    stride = (element_size + alignment_mask) & ~alignment_mask;
    pool = (NUMEMBLK *)NuAlloc(header_size + stride * count, "d:\\projects\\game_saga\\nu2api.saga\\nucore\\numemblk.c",
                               111);
    data = (u32)(pool + 1);
    data += alignment_mask;
    data &= ~alignment_mask;
    pool->free_list = (NUMEMBLKLINK *)data;
    pool->stride = stride;
    pool->capacity = count;
    pool->free_count = count;
    pool->flags = 0;
    stride_words = stride / sizeof(u32);
    link = pool->free_list;
    for (i = 0; i < count - 1; i++) {
        link->next = (NUMEMBLKLINK *)((u32 *)link + stride_words);
        link = link->next;
    }
    link->next = NULL;
    return pool;
}

NUMEMBLK *__cdecl NuMemBlkCreateEx(i32 element_size, i32 count, u32 alignment_mask, void *storage) {
    NUMEMBLKLINK *link;
    i32 i;
    u32 header_size;
    u32 stride;
    NUMEMBLK *pool;
    u32 data;
    u32 stride_words;
    if (element_size < sizeof(NUMEMBLKLINK))
        element_size = sizeof(NUMEMBLKLINK);
    header_size = (sizeof(NUMEMBLK) + alignment_mask) & ~alignment_mask;
    stride = (element_size + alignment_mask) & ~alignment_mask;
    pool = (NUMEMBLK *)storage;
    data = (u32)pool;
    data += (sizeof(NUMEMBLK) + alignment_mask) & ~alignment_mask;
    pool->stride = stride;
    pool->capacity = count;
    pool->free_count = count;
    pool->flags = 1;
    if (count != 0) {
        pool->free_list = (NUMEMBLKLINK *)data;
        stride_words = stride / sizeof(u32);
        link = pool->free_list;
        for (i = 0; i < count - 1; i++) {
            link->next = (NUMEMBLKLINK *)((u32 *)link + stride_words);
            link = link->next;
        }
        link->next = NULL;
    } else {
        pool->free_list = NULL;
    }
    return pool;
}

i32 __cdecl NuMemBlkSize(i32 element_size, i32 count, i32 alignment_mask) {
    i32 header_size = (16 + alignment_mask) & ~alignment_mask;
    i32 stride = (element_size + alignment_mask) & ~alignment_mask;
    i32 size = header_size + stride * count;
    return size;
}

void __cdecl NuMemBlkDestroy(NUMEMBLK *pool) {
    if (!(pool->flags & 1))
        NuFree(pool, "d:\\projects\\game_saga\\nu2api.saga\\nucore\\numemblk.c", 221);
}

void __cdecl NuMemBlkCheckFreeList(NUMEMBLK *pool) {
    NUMEMBLKLINK *link = pool->free_list;
    i32 count = 0;
    while (link != NULL) {
        link = link->next;
        count++;
    }
}

void *__cdecl NuMemBlkAlloc(NUMEMBLK *pool) {
    NUMEMBLKLINK *block;
    if ((block = pool->free_list) != NULL) {
        pool->free_list = pool->free_list->next;
        pool->free_count--;
        memset(block, -1, pool->stride);
    }
    return block;
}

void __cdecl NuMemBlkFree(NUMEMBLK *pool, void *block) {
    NUMEMBLKLINK *link = (NUMEMBLKLINK *)block;
    pool->free_count++;
    memset(block, -2, pool->stride);
    link->next = pool->free_list;
    pool->free_list = link;
}

static void NuAllocManagerUnlinkPc(NUALLOC_MANAGER_BLOCK_PC *block) {
    if (block->previous != NULL)
        block->previous->next = block->next;
    if (block->next != NULL)
        block->next->previous = block->previous;
}

static void NuAllocManagerSplitPc(NUALLOC_MANAGER_BLOCK_PC **allocated, NUALLOC_MANAGER_BLOCK_PC **remainder,
                                  NUALLOC_MANAGER_BLOCK_PC *block, u32 *size, u32 alignment) {
    u32 remainder_size = (block->size_flags & 0x7fffffffU) - *size - 16;
    *remainder = block;
    *allocated = (NUALLOC_MANAGER_BLOCK_PC *)((u32)block + remainder_size + 16);
    if (alignment > 0) {
        u32 payload = (u32)*allocated + 12;
        u32 aligned_payload = (payload + alignment - 1) & ~(alignment - 1);
        u32 adjustment = aligned_payload - payload;
        *allocated = (NUALLOC_MANAGER_BLOCK_PC *)((u32)*allocated + adjustment);
        *size -= adjustment;
        remainder_size += adjustment;
    }
    (*allocated)->size_flags = 0x80000000U | *size;
    (*allocated)->next = NULL;
    (*allocated)->previous = NULL;
    ((NUALLOC_MANAGER_TRAILER_PC *)((u32)*allocated + ((*allocated)->size_flags & 0x7fffffffU) +
                                    sizeof(NUALLOC_MANAGER_BLOCK_PC)))
        ->previous_block = *allocated;
    (*remainder)->size_flags = remainder_size;
    (*remainder)->next = NULL;
    (*remainder)->previous = NULL;
    ((NUALLOC_MANAGER_TRAILER_PC *)((u32)*remainder + ((*remainder)->size_flags & 0x7fffffffU) +
                                    sizeof(NUALLOC_MANAGER_BLOCK_PC)))
        ->previous_block = *remainder;
}

void *__cdecl NuAllocManagerAllocatePc(NUALLOC_MANAGER_PREFIX_PC *manager, u32 size, u32 alignment, u32 unused) {
    NUALLOC_MANAGER_BLOCK_PC *sentinel = (NUALLOC_MANAGER_BLOCK_PC *)((u32)manager->base + 16);
    NUALLOC_MANAGER_BLOCK_PC *block;
    NUALLOC_MANAGER_BLOCK_PC *allocated;
    NUALLOC_MANAGER_BLOCK_PC *remainder;
    (void)unused;
    if (size == 0)
        return NULL;
    size += alignment;
    block = sentinel;
    while (block != NULL) {
        if ((block->size_flags & 0x7fffffffU) > size + 16)
            break;
        block = block->next;
    }
    if (block == NULL)
        return NULL;
    if (block->previous != NULL)
        block->previous->next = block->next;
    if (block->next != NULL)
        block->next->previous = block->previous;
    NuAllocManagerSplitPc(&allocated, &remainder, block, &size, alignment);
    if (sentinel->next != NULL)
        sentinel->next->previous = remainder;
    remainder->next = sentinel->next;
    remainder->previous = sentinel;
    sentinel->next = remainder;
    manager->block_count += 1;
    manager->used_bytes += (allocated->size_flags & 0x7fffffffU) + 16;
    return (void *)((u32)allocated + 12);
}

void __cdecl NuAllocManagerFreePc(NUALLOC_MANAGER_PREFIX_PC *manager, void *payload) {
    void *base = manager->base;
    NUALLOC_MANAGER_BLOCK_PC *sentinel = (NUALLOC_MANAGER_BLOCK_PC *)((u32)base + 16);
    NUALLOC_MANAGER_BLOCK_PC *block;
    NUALLOC_MANAGER_BLOCK_PC *next;
    NUALLOC_MANAGER_BLOCK_PC *previous;
    if (payload == NULL || (u32)payload < (u32)base || (u32)payload > (u32)base + manager->size)
        return;
    block = (NUALLOC_MANAGER_BLOCK_PC *)((u32)payload - 12);
    block->size_flags &= 0x7fffffffU;
    manager->block_count -= 1;
    manager->used_bytes -= (block->size_flags & 0x7fffffffU) + 16;
    if (sentinel->next != NULL)
        sentinel->next->previous = block;
    block->previous = sentinel;
    block->next = sentinel->next;
    sentinel->next = block;
    next = (NUALLOC_MANAGER_BLOCK_PC *)((u32)block + (block->size_flags & 0x7fffffffU) + 16);
    if (!(next->size_flags & 0x80000000U)) {
        NuAllocManagerUnlinkPc(next);
        block->size_flags = (block->size_flags & 0x80000000U) |
                            ((next->size_flags & 0x7fffffffU) + (block->size_flags & 0x7fffffffU) + 16);
        ((NUALLOC_MANAGER_TRAILER_PC *)((u32)block + (block->size_flags & 0x7fffffffU) +
                                        sizeof(NUALLOC_MANAGER_BLOCK_PC)))
            ->previous_block = block;
    }
    previous = ((NUALLOC_MANAGER_TRAILER_PC *)block)[-1].previous_block;
    if (!(previous->size_flags & 0x80000000U)) {
        NuAllocManagerUnlinkPc(block);
        previous->size_flags = (previous->size_flags & 0x80000000U) |
                               ((block->size_flags & 0x7fffffffU) + (previous->size_flags & 0x7fffffffU) + 16);
        ((NUALLOC_MANAGER_TRAILER_PC *)((u32)previous + (previous->size_flags & 0x7fffffffU) +
                                        sizeof(NUALLOC_MANAGER_BLOCK_PC)))
            ->previous_block = previous;
    }
}

void *__cdecl NuAllocManagerAllocateAlignedPc(NUALLOC_MANAGER_PREFIX_PC *manager, u32 size, u32 alignment) {
    return NuAllocManagerAllocatePc(manager, size, alignment, 0);
}

/* Lists every tracked allocation of at least min_size bytes, then the running total. */

f32 __cdecl NuRDPFVar(char *input, i32(__cdecl *get_variable)(char *, f32 *, i32 *)) {
    NURDPFCTX_PC context;

    context.input = input;
    context.position = 0;
    context.get_variable = get_variable;
    NuRDPFGetToken(&context);
    return NuRDPFExpression(&context);
}

f32 __cdecl NuRDPF(char *input) {
    return NuRDPFVar(input, NULL);
}

/* Native unused string utilities; SOURCE-only proposal. */
i32 __cdecl NuStrCpyWC(char *dst, const char *src, const char *replacement) {
    i32 copied = 0;
    if (src == NULL) {
        *dst = '\0';
    } else {
        do {
            if (*src != '*') {
                *dst = *src;
                ++dst;
                ++copied;
            } else {
                const char *cursor = replacement;
                if (cursor != NULL) {
                    while (cursor != NULL && *cursor != '\0') {
                        *dst = *cursor;
                        ++dst;
                        ++cursor;
                        ++copied;
                    }
                }
            }
        } while (*src++ != '\0');
    }
    return copied;
}

i32 __cdecl NuStrICmpWC(char *pattern, char *text, char *wildcard) {
    i32 index;
    char a, b;
    i32 suffix_length, text_length, wildcard_length;
    if (pattern == NULL)
        return -1;
    if (text == NULL)
        return 1;
    while (*pattern != '\0' && *pattern != '*') {
        a = NuToUpper((u8)*pattern);
        b = NuToUpper((u8)*text);
        if (a > b)
            return 1;
        if (a < b)
            return -1;
        ++pattern;
        ++text;
    }
    if (*pattern == '*') {
        ++pattern;
        suffix_length = NuStrLen(pattern);
        text_length = NuStrLen(text);
        wildcard_length = text_length - suffix_length;
        if (wildcard_length < 0)
            return 1;
        if (wildcard != NULL) {
            for (index = 0; index < wildcard_length; ++index)
                wildcard[index] = text[index];
            wildcard[index] = '\0';
        }
        text += text_length - suffix_length;
    } else if (wildcard != NULL) {
        *wildcard = '\0';
    }
    while (*pattern != '\0') {
        a = NuToUpper((u8)*pattern);
        b = NuToUpper((u8)*text);
        if (a > b)
            return 1;
        if (a < b)
            return -1;
        ++pattern;
        ++text;
    }
    return 0;
}

void __cdecl NuStrCat(char *str, const char *ext) {
    while (*str != '\0') {
        str++;
    }

    if (ext != NULL) {
        do {
            *str = *ext;
            str++;
        } while (*(ext++) != '\0');
    }
}

i32 __cdecl NuStrNCat(char *str, const char *ext, i32 n) {
    i32 copied;

    while (*str != '\0') {
        str++;
    }
    copied = 0;

    if (ext != NULL) {
        do {
            if (n == 0) {
                break;
            }
            *str = *ext;
            str++;
            copied++;
            n--;
        } while (*(ext++) != '\0');
    }
    return copied;
}

i32 __cdecl NuStrCpy(char *dst, const char *src) {
    char *dst_start = dst;

    if (src != NULL) {
        while (*src != '\0') {
            *dst = *src;
            dst++;
            src++;
        }
    }

    *dst = '\0';
    return (i32)(dst - dst_start);
}

i32 __cdecl NuStrNCpy(char *dst, const char *src, i32 n) {
    i32 count = 0;

    if (src == NULL) {
        *dst = '\0';
        return 0;
    }

    do {
        n--;
        if (n <= 0) {
            *dst = '\0';
            count++;
            break;
        }

        *dst = *src;
        dst++;
        count++;
    } while (*(src++) != '\0');

    return count - 1;
}

char *__cdecl NuStrChr(char *src, char c) {
    while (*src != '\0') {
        if (*src == c) {
            return src;
        }
        src++;
    }
    return NULL;
}

char *__cdecl NuStrRChr(char *src, char c) {
    char *ptr = src;

    while (*ptr != '\0') {
        ptr++;
    }

    while (ptr >= src) {
        if (*ptr == c) {
            return ptr;
        }
        ptr--;
    }
    return NULL;
}

i32 __cdecl NuStrLen(const char *str) {
    i32 count = 0;

    while (*str != '\0') {
        count++;
        str++;
    }
    return count;
}

i32 __cdecl NuStrLenU(u8 *str) {
    i32 offset = 0;
    i32 characters = 0;

    while (str[offset] != 0) {
        characters++;
        offset++;
        while (str[offset] >= 0x80 && str[offset] <= 0xbf) {
            offset++;
        }
    }
    return characters;
}

i32 __cdecl NuStrFindPosU(u8 *text, i32 character_index) {
    i32 index = 0, count = 0;
    if (character_index == 0)
        return 0;
    if (text[0] == 0)
        return 0;
    do {
        ++index;
        ++count;
        while ((u8)(text[index] - 0x80) <= 0x3f)
            ++index;
    } while (count != character_index && text[index] != 0);
    return index;
}

char *__cdecl NuStrIStr(char *text, char *substring) {
    char *start;
    char *candidate;
    char *pattern;
    u8 text_upper;
    u8 pattern_upper;

    while (*text != '\0') {
        start = text;
        candidate = text;
        pattern = substring;
        while (*pattern != '\0') {
            if (*candidate == '\0') {
                break;
            }
            text_upper = (u8)*candidate;
            if (text_upper >= 'a' && text_upper <= 'z') {
                text_upper -= 0x20;
            } else if (text_upper >= 0xe0) {
                text_upper -= 0x20;
            }
            pattern_upper = (u8)*pattern;
            if (pattern_upper >= 'a' && pattern_upper <= 'z') {
                pattern_upper -= 0x20;
            } else if (pattern_upper >= 0xe0) {
                pattern_upper -= 0x20;
            }
            if (text_upper != pattern_upper) {
                break;
            }
            candidate++;
            pattern++;
        }
        if (*pattern == '\0') {
            return start;
        }
        text++;
    }
    return NULL;
}

i32 __cdecl NuStrCmp(const char *a, const char *b) {
    char a_cursor;
    char b_cursor;

    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return 1;
    }

    do {
        a_cursor = *a;
        b_cursor = *b;
        if (a_cursor > b_cursor) {
            return 1;
        }
        if (a_cursor < b_cursor) {
            return -1;
        }
        a++;
        b++;
    } while (a_cursor != '\0' && b_cursor != '\0');

    return 0;
}

i32 __cdecl NuStrICmp(const char *a, const char *b) {
    char a_cursor;
    char b_cursor;
    u8 a_upper;
    u8 b_upper;

    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return 1;
    }

    do {
        a_upper = (u8)*a;
        if (a_upper >= 'a' && a_upper <= 'z') {
            a_upper -= 0x20;
        } else if (a_upper >= 0xe0) {
            a_upper -= 0x20;
        }
        a_cursor = (char)a_upper;

        b_upper = (u8)*b;
        if (b_upper >= 'a' && b_upper <= 'z') {
            b_upper -= 0x20;
        } else if (b_upper >= 0xe0) {
            b_upper -= 0x20;
        }
        b_cursor = (char)b_upper;

        if (a_cursor > b_cursor) {
            return 1;
        }
        if (a_cursor < b_cursor) {
            return -1;
        }
        a++;
        b++;
    } while (a_cursor != '\0' && b_cursor != '\0');

    return 0;
}

i32 __cdecl NuStrNCmp(const char *a, const char *b, i32 n) {
    char a_cursor;
    char b_cursor;

    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return 1;
    }
    if (n == 0) {
        return 0;
    }
    if (n == -1) {
        n = NuStrLen(a);
    } else if (n == -2) {
        n = NuStrLen(b);
    }

    do {
        a_cursor = *a;
        b_cursor = *b;
        if (a_cursor > b_cursor) {
            return 1;
        }
        if (a_cursor < b_cursor) {
            return -1;
        }
        a++;
        b++;
        n--;
    } while (a_cursor != '\0' && b_cursor != '\0' && n != 0);

    return 0;
}

i32 __cdecl NuStrNICmp(const char *a, const char *b, i32 n) {
    char a_cursor;
    char b_cursor;
    u8 a_upper;
    u8 b_upper;

    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return 1;
    }
    if (n == 0) {
        return 0;
    }
    if (n == -1) {
        n = NuStrLen(a);
    } else if (n == -2) {
        n = NuStrLen(b);
    }

    do {
        a_upper = (u8)*a;
        if (a_upper >= 'a' && a_upper <= 'z') {
            a_upper -= 0x20;
        } else if (a_upper >= 0xe0) {
            a_upper -= 0x20;
        }
        a_cursor = (char)a_upper;

        b_upper = (u8)*b;
        if (b_upper >= 'a' && b_upper <= 'z') {
            b_upper -= 0x20;
        } else if (b_upper >= 0xe0) {
            b_upper -= 0x20;
        }
        b_cursor = (char)b_upper;

        if (a_cursor > b_cursor) {
            return 1;
        }
        if (a_cursor < b_cursor) {
            return -1;
        }
        a++;
        b++;
        n--;
    } while (a_cursor != '\0' && b_cursor != '\0' && n != 0);

    return 0;
}

void __cdecl NuStrUpr(char *dst, const char *src) {
    u8 value;

    for (; *src != '\0'; src++) {
        value = (u8)*src;
        if (value >= 'a' && value <= 'z') {
            value -= 0x20;
        } else if (value >= 0xe0) {
            value -= 0x20;
        }
        *dst = (char)value;
        dst++;
    }
    *dst = *src;
}

void __cdecl NuStrLwr(char *dst, const char *src) {
    u8 value;

    while (*src != '\0') {
        value = (u8)*src;
        if (value >= 'A' && value <= 'Z') {
            value += 0x20;
        } else if (value > 0xbf && value < 0xe0) {
            value += 0x20;
        }
        *dst = (char)value;
        dst++;
        src++;
    }
    *dst = *src;
}

i32 __cdecl NuHexStringToI(char *string) {
    i32 value = 0;
    u8 upper;
    i32 upper_value;
    u32 digit;

    for (; *string != '\0'; string++) {
        value <<= 4;
        upper = (u8)*string;
        if (upper >= 'a' && upper <= 'z') {
            upper -= 0x20;
        } else if (upper >= 0xe0) {
            upper -= 0x20;
        }

        upper_value = upper;
        digit = upper_value - '0';
        if (digit <= 9) {
            value |= digit;
        } else if ((u32)(upper_value - 'A') <= 5) {
            value |= upper_value - 0x37;
        } else {
            return 0;
        }
    }
    return value;
}

/* SOURCE-only R69 carry. Raw wide units and PC case ranges, not Unicode/locale
 * conversion. The character functions return NUWCHAR: only AX is the defined
 * return value; incidental high EAX bits are not a 32-bit integer contract.
 */
NUWCHAR __cdecl NuToUpperW(NUWCHAR character) {
    return NU_UTF16_UPPER(character);
}

NUWCHAR __cdecl NuToLowerW(NUWCHAR character) {
    if ((character >= 0x41 && character <= 0x5a) || (character >= 0xc0 && character <= 0xdf))
        character += 0x20;
    return character;
}

void __cdecl NuStrCatW(NUWCHAR *str, const NUWCHAR *ext) {
    while (*str != 0)
        ++str;
    if (ext != NULL) {
        do {
            *str = *ext;
            ++str;
        } while (*ext++ != 0);
    }
}

/* SOURCE proposal: ordinary wide-string counterparts in the existing nustring owner.
 * Work on raw 16-bit units, including Native's broad Latin-1 case conversion.
 * Counts preserve the PC's subtract-then-test behavior and 32-bit wrap; these
 * are not bounded libc replacements and do not add missing-pointer guards.
 */
i32 __cdecl NuStrNCatW(NUWCHAR *str, const NUWCHAR *ext, i32 n) {
    i32 copied;
    while (*str != 0)
        ++str;
    copied = 0;
    if (ext != NULL) {
        do {
            if (n == 0)
                break;
            *str = *ext;
            ++str;
            ++copied;
            n = (i32)((u32)n - 1u);
        } while (*ext++ != 0);
    }
    return copied;
}

i32 NuStrCpyW(u16 *dst, const u16 *src) {
    i32 count = 0;
    if (src == NULL) {
        *dst = 0;
    } else {
        do {
            *dst++ = *src;
            ++count;
        } while (*src++ != 0);
    }
    return count;
}

i32 __cdecl NuStrNCpyW(NUWCHAR *dst, const NUWCHAR *src, i32 n) {
    i32 copied = 0;
    if (src == NULL) {
        *dst = 0;
        return 0;
    }
    do {
        n = (i32)((u32)n - 1u);
        ++copied;
        if (n <= 0) {
            *dst = 0;
            break;
        }
        *dst = *src;
        ++dst;
    } while (*src++ != 0);
    return copied - 1;
}

NUWCHAR *__cdecl NuStrChrW(NUWCHAR *str, NUWCHAR character) {
    while (*str != 0) {
        if (*str == character)
            return str;
        ++str;
    }
    return NULL;
}

NUWCHAR *__cdecl NuStrRChrW(NUWCHAR *str, NUWCHAR character) {
    NUWCHAR *cursor = str;
    while (*cursor != 0)
        ++cursor;
    while (cursor >= str) {
        if (*cursor == character)
            return cursor;
        --cursor;
    }
    return NULL;
}

i32 NuStrLenW(const NUWCHAR *str) {
    i32 count = 0;

    while (*str != 0) {
        count++;
        str++;
    }

    return count;
}

NUWCHAR *__cdecl NuStrStrW(NUWCHAR *str, const NUWCHAR *sub) {
    while (*str != 0) {
        NUWCHAR *cursor = str;
        const NUWCHAR *match = sub;
        while (*match != 0) {
            if (*cursor == 0)
                break;
            if (*cursor != *match)
                break;
            ++cursor;
            ++match;
        }
        if (*match == 0)
            return str;
        ++str;
    }
    return NULL;
}

NUWCHAR *__cdecl NuStrIStrW(NUWCHAR *str, const NUWCHAR *sub) {
    while (*str != 0) {
        NUWCHAR *cursor = str;
        const NUWCHAR *match = sub;
        while (*match != 0) {
            NUWCHAR ca;
            NUWCHAR cb;
            if (*cursor == 0)
                break;
            ca = *cursor;
            ca = NU_UTF16_UPPER(ca);
            cb = *match;
            cb = NU_UTF16_UPPER(cb);
            if (ca != cb)
                break;
            ++cursor;
            ++match;
        }
        if (*match == 0)
            return str;
        ++str;
    }
    return NULL;
}

i32 __cdecl NuStrCmpW(const NUWCHAR *a, const NUWCHAR *b) {
    NUWCHAR ca;
    NUWCHAR cb;
    if (a == NULL)
        return -1;
    if (b == NULL)
        return 1;
    do {
        ca = *a;
        cb = *b;
        if (ca > cb)
            return 1;
        if (ca < cb)
            return -1;
        ++a;
        ++b;
    } while (ca != 0 && cb != 0);
    return 0;
}

i32 __cdecl NuStrICmpW(NUWCHAR *a, NUWCHAR *b) {
    NUWCHAR a_cursor;
    NUWCHAR b_cursor;
    if (a == NULL)
        return -1;
    if (b == NULL)
        return 1;
    do {
        a_cursor = *a;
        if ((u16)(a_cursor - 'a') <= 'z' - 'a' || (u16)(a_cursor - 0xe0) <= 0x1f)
            a_cursor -= 0x20;
        b_cursor = *b;
        if ((u16)(b_cursor - 'a') <= 'z' - 'a' || (u16)(b_cursor - 0xe0) <= 0x1f)
            b_cursor -= 0x20;
        if (a_cursor > b_cursor)
            return 1;
        if (a_cursor < b_cursor)
            return -1;
        ++a;
        ++b;
    } while (a_cursor != 0 && b_cursor != 0);
    return 0;
}

i32 __cdecl NuStrNCmpW(const NUWCHAR *a, const NUWCHAR *b, i32 n) {
    NUWCHAR ca;
    NUWCHAR cb;
    if (a == NULL)
        return -1;
    if (b == NULL)
        return 1;
    if (n == 0)
        return 0;
    if (n == -1)
        n = NuStrLenW(a);
    else if (n == -2)
        n = NuStrLenW(b);
    do {
        ca = *a;
        cb = *b;
        if (ca > cb)
            return 1;
        if (ca < cb)
            return -1;
        ++a;
        ++b;
        n = (i32)((u32)n - 1u);
    } while (ca != 0 && cb != 0 && n != 0);
    return 0;
}

i32 __cdecl NuStrNICmpW(const NUWCHAR *a, const NUWCHAR *b, i32 n) {
    NUWCHAR ca;
    NUWCHAR cb;
    if (a == NULL)
        return -1;
    if (b == NULL)
        return 1;
    if (n == 0)
        return 0;
    if (n == -1)
        n = NuStrLenW(a);
    else if (n == -2)
        n = NuStrLenW(b);
    do {
        ca = *a;
        ca = NU_UTF16_UPPER(ca);
        cb = *b;
        cb = NU_UTF16_UPPER(cb);
        if (ca > cb)
            return 1;
        if (ca < cb)
            return -1;
        ++a;
        ++b;
        n = (i32)((u32)n - 1u);
    } while (ca != 0 && cb != 0 && n != 0);
    return 0;
}

void __cdecl NuStrUprW(NUWCHAR *dst, const NUWCHAR *src) {
    while (*src != 0) {
        NUWCHAR character = *src;
        *dst = NU_UTF16_UPPER(character);
        ++dst;
        ++src;
    }
    *dst = *src;
}

void __cdecl NuStrLwrW(u16 *dst, const u16 *src) {
    while (*src != 0) {
        u16 value = *src;
        if ((u16)(value - 'A') <= 'Z' - 'A' || (u16)(value - 0xc0) <= 0x1f) {
            value += 0x20;
        }
        *dst++ = value;
        ++src;
    }
    *dst = *src;
}

i32 __cdecl NuVSPrintf(char *dest, const char *format, va_list args) {
    return vsprintf(dest, format, args);
}

i32 __cdecl NuSPrintf(char *dest, const char *format, ...) {
    va_list args;
    i32 length;
    va_start(args, format);
    vsprintf(dest, format, args);
    va_end(args);
    length = 0;
    while (dest[length] != '\0')
        length++;
    return length;
}

i32 NuSPrintfW(NUWCHAR *dest, NUWCHAR *format, ...) {
    // The original initializes this local but never reads it.
    i32 length = 0;
    va_list args;
    char ascii_format[256];
    char text[256];
    NuUnicodeToAscii(ascii_format, format);
    va_start(args, format);
    vsprintf(text, ascii_format, args);
    va_end(args);
    NuAsciiToUnicode(dest, text);
    return NuStrLenW(dest);
}

i32 __cdecl NuStringTok(char *str, ...) {
    char *token;
    i32 value;
    va_list args;
    va_start(args, str);
    do {
        token = va_arg(args, char *);
        value = va_arg(args, i32);
        if (token != NULL) {
            if (NuStrICmp(str, token) == 0)
                break;
        }
    } while (token != NULL);
    va_end(args);
    return value;
}

i32 NuIsAlNum(char value) {
    switch (value) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
            return 1;
    }
    return 0;
}

i32 __cdecl NuIsAl(char c) {
    switch (c) {
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case '_':
            return 1;
        default:
            return 0;
    }
}

char *__cdecl NuIToA(i32 value, char *buffer, i32 radix) {
    char reversed[33];
    char *digit = reversed;
    char *out = buffer;
    if (value < 0) {
        value = (i32)(0u - (u32)value);
        *out++ = '-';
    }
    do {
        *digit++ = (char)(value % radix + '0');
        value /= radix;
    } while (value != 0);
    while (digit != reversed)
        *out++ = *--digit;
    *out = '\0';
    return buffer;
}

f32 __cdecl NuAToF(char *string) {
    f32 dividend = 0.0f;
    f32 divisor = 1.0f;
    char character = *string++;

    if (character == '-') {
        divisor = -1.0f;
        character = *string++;
    }

    while (character >= '0' && character <= '9') {
        dividend *= 10.0;
        dividend += character - '0';
        character = *string++;
    }

    if (character == '.') {
        character = *string++;
        while (character >= '0' && character <= '9') {
            divisor *= 10.0;
            dividend *= 10.0;
            dividend += character - '0';
            character = *string++;
        }
    }

    return dividend / divisor;
}

i32 __cdecl NuAToI(char *string) {
    i32 value = 0;
    i32 sign = 0;
    char character = *string++;

    if (character == '-') {
        sign = -1;
        character = *string++;
    }

    while (character >= '0' && character <= '9') {
        value = (value << 3) + 2 * value;
        value = value + character - '0';
        character = *string++;
    }

    if (sign != 0) {
        return value * sign;
    }
    return value;
}

i32 __cdecl NuStrToL(char *str, char **end, i32 radix) {
    i32 value = 0;
    i32 sign = 0;
    char c = *str++;
    while (c == ' ' || c == '\t')
        c = *str++;
    if (c == '-') {
        sign = -1;
        c = *str++;
    } else if (c == '+')
        c = *str++;
    if (radix == 0) {
        radix = 10;
        if (c == '0' && (*str == 'x' || *str == 'X')) {
            radix = 16;
            ++str;
            c = *str;
        }
    }
    while (1) {
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'A' && c <= 'Z')
            c -= 'A' - 10;
        else if (c >= 'a' && c <= 'z')
            c -= 'a' - 10;
        else
            break;
        if (c >= radix)
            break;
        value = (i32)((u32)value * (u32)radix + (u32)c);
        c = *str++;
    }
    if (end != NULL)
        *end = str - 1;
    if (sign != 0)
        return (i32)((u32)value * (u32)sign);
    return value;
}

i32 __cdecl NuIsAlW(NUWCHAR c) {
    switch (c) {
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case '_':
            return 1;
        default:
            return 0;
    }
}

NUWCHAR *__cdecl NuIToAW(i32 value, NUWCHAR *buffer, i32 radix) {
    NUWCHAR reversed[33];
    NUWCHAR *digit = reversed;
    NUWCHAR *out = buffer;
    if (value < 0) {
        value = (i32)(0u - (u32)value);
        *out++ = '-';
    }
    do {
        *digit++ = value % radix + '0';
        value /= radix;
    } while (value != 0);
    while (digit != reversed)
        *out++ = *--digit;
    *out++ = 0;
    return buffer;
}

f32 __cdecl NuAToFW(const NUWCHAR *str) {
    f32 value = 0.0f;
    f32 divisor = 1.0f;
    NUWCHAR c = *str++;
    if (c == '-') {
        divisor = -1.0f;
        c = *str++;
    }
    while (c >= '0' && c <= '9') {
        value *= 10.0f;
        value += c - '0';
        c = *str++;
    }
    if (c == '.') {
        c = *str++;
        while (c >= '0' && c <= '9') {
            divisor *= 10.0f;
            value *= 10.0f;
            value += c - '0';
            c = *str++;
        }
    }
    return value / divisor;
}

i32 __cdecl NuAToIW(const NUWCHAR *str) {
    i32 value = 0;
    i32 sign = 0;
    NUWCHAR c = *str++;
    if (c == '-') {
        sign = -1;
        c = *str++;
    }
    while (c >= '0' && c <= '9') {
        value = (i32)((u32)value * 10u + (u32)c - (u32)'0');
        c = *str++;
    }
    if (sign != 0)
        return (i32)((u32)value * (u32)sign);
    return value;
}

i32 __cdecl NuStrToLW(NUWCHAR *str, NUWCHAR **end, i32 radix) {
    i32 value = 0;
    i32 sign = 0;
    NUWCHAR c = *str++;
    while (c == ' ' || c == '\t')
        c = *str++;
    if (c == '-') {
        sign = -1;
        c = *str++;
    } else if (c == '+')
        c = *str++;
    if (radix == 0) {
        radix = 10;
        if (c == '0' && (*str == 'x' || *str == 'X')) {
            radix = 16;
            ++str;
            c = *str;
        }
    }
    while (1) {
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'A' && c <= 'Z')
            c -= 'A' - 10;
        else if (c >= 'a' && c <= 'z')
            c -= 'a' - 10;
        else
            break;
        if (c >= radix)
            break;
        value = (i32)((u32)value * (u32)radix + (u32)c);
        c = *str++;
    }
    if (end != NULL)
        *end = str - 1;
    if (sign != 0)
        return (i32)((u32)value * (u32)sign);
    return value;
}

void __cdecl NuStrToLower(char *text) {
    u8 value;
    while (*text != 0) {
        value = (u8)*text;
        if ((u8)(value - 0x41) <= 0x19 || (u8)(value + 0x40) <= 0x1f)
            value = (u8)(value + 0x20);
        *text++ = (char)value;
    }
}

u8 __cdecl NuToUpper(u8 character) {
    if ((u8)(character - 'a') <= 'z' - 'a' || character >= 0xe0)
        character = (u8)(character - 0x20);
    return character;
}

char *__cdecl NuStrStr(char *text, const char *needle) {
    while (*text != '\0') {
        const char *part = needle;
        char *scan = text;
        while (*part != '\0' && *scan != '\0' && *scan == *part) {
            ++scan;
            ++part;
        }
        if (*part == '\0')
            return text;
        ++text;
    }
    return NULL;
}

u8 __cdecl NuToLower(u8 c) {
    if (c >= 'A' && c <= 'Z') {
        c += 0x20;
    } else if (c > 0xbf && c < 0xe0) {
        c += 0x20;
    }
    return c;
}

void __cdecl NuUnicodeToAscii(char *destination, u16 *source) {
    i32 index = 0;

    if (source == NULL || destination == NULL) {
        return;
    }

    *destination = '\0';
    for (; source[index] != 0; index++) {
        if ((source[index] & 0xff00) != 0) {
            switch (source[index]) {
                case 0x2018:
                    destination[index] = (char)0x91;
                    break;
                case 0x2019:
                    destination[index] = (char)0x92;
                    break;
                case 0x2013:
                    destination[index] = (char)0x96;
                    break;
                case 0x2026:
                    destination[index] = (char)0x85;
                    break;
                case 0x2122:
                    destination[index] = (char)0x99;
                    break;
                default:
                    destination[index] = '?';
                    break;
            }
        } else {
            destination[index] = (char)source[index];
        }
    }
    destination[index] = '\0';
}

void __cdecl NuUnicodeToUTF8(u8 *dst, u16 *src) {
    if (src == NULL) {
        return;
    }
    if (dst == NULL) {
        return;
    }

    *dst = '\0';
    for (; *src != 0; src++) {
        if (*src < 0x80) {
            *dst++ = (u8)*src;
        } else if (*src < 0x800) {
            *dst++ = (u8)((*src >> 6 & 0x1f) | 0xc0);
            *dst++ = (u8)((*src & 0x3f) | 0x80);
        } else {
            *dst++ = (u8)(((u32)*src >> 12 & 0xf) | 0xe0);
            *dst++ = (u8)((*src >> 6 & 0x3f) | 0x80);
            *dst++ = (u8)((*src & 0x3f) | 0x80);
        }
    }
    *dst = '\0';
}

void __cdecl NuAsciiToUnicode(u16 *dst, char *src) {
    i32 pos = 0;

    *dst = 0;
    if (src == NULL) {
        return;
    }
    if (dst == NULL) {
        return;
    }

    for (; src[pos] != '\0'; pos++) {
        dst[pos] = (u8)src[pos];
    }
    dst[pos] = '\0';
}

void __cdecl NuUTF8ToUnicode(u16 *dst, u8 *src) {
    if (src == NULL) {
        return;
    }
    if (dst == NULL) {
        return;
    }

    *dst = 0;
    while (*src != 0) {
        if (*src < 0x80) {
            *dst = *src;
            src++;
        } else if (*src < 0xe0) {
            *dst = (*src & 0x1f) << 6;
            src++;
            *dst |= *src & 0x3f;
            src++;
        } else if (*src < 0xf0) {
            *dst = (*src & 0x0f) << 12;
            src++;
            *dst |= (*src & 0x3f) << 6;
            src++;
            *dst |= *src & 0x3f;
            src++;
        } else {
            *dst = '?';
            src++;
        }
        dst++;
    }
    *dst = 0;
}

u8 *__cdecl NuUnicodeCharFromUTF8(u16 *dst, u8 *src) {
    u16 character[3];
    character[0] = *src++;
    if (character[0] < 0x80) {
        *dst = character[0];
    } else if ((character[0] & 0xe0) == 0xc0) {
        character[1] = *src++;
        *dst = ((character[0] & 0x1f) << 6) | (character[1] & 0x3f);
    } else if ((character[0] & 0xf0) == 0xe0) {
        character[1] = *src++;
        character[2] = *src++;
        *dst = (character[0] << 12) | ((character[1] & 0x3f) << 6) | (character[2] & 0x3f);
    }
    return src;
}

u8 *__cdecl NuUTF8CharFromUnicode(u8 *dst, u16 character) {
    if (character < 0x80) {
        *dst++ = (u8)character;
    } else if (character < 0x800) {
        *dst++ = (u8)((character >> 6) | 0xc0);
        *dst++ = (u8)((character & 0x3f) | 0x80);
    } else {
        *dst++ = (u8)((character >> 12) | 0xe0);
        *dst++ = (u8)(((character >> 6) & 0x3f) | 0x80);
        *dst++ = (u8)((character & 0x3f) | 0x80);
    }
    return dst;
}

i32 __cdecl GetLineW(u16 *line, NUFILE file) {
    i32 length = 0;
    i32 end_of_line;
    i32 has_text = 0;
    i32 quoted = 0;
    u16 character;

    for (;;) {
        character = NuFileReadWChar(file);
        switch (character) {
            case '\r':
                NuFileReadWChar(file);
                /* fall through */
            case '\n':
                if (length == 0 || has_text == 0) {
                    has_text = 0;
                    break;
                }
                /* fall through */
            case '\0':
                line[length] = 0;
                return length;
            case '"':
                quoted = 1 - quoted;
                has_text = 1;
                line[length++] = character;
                break;
            case ';':
                if (quoted == 0) {
                    if (length == 0 || has_text == 0) {
                        end_of_line = 0;
                        do {
                            character = NuFileReadWChar(file);
                            switch (character) {
                                case '\r':
                                    NuFileReadWChar(file);
                                    /* fall through */
                                case '\n':
                                case '\0':
                                    end_of_line = 1;
                                    break;
                                default:
                                    break;
                            }
                        } while (end_of_line == 0);
                        length = 0;
                        has_text = 0;
                        break;
                    }

                    for (;;) {
                        character = NuFileReadWChar(file);
                        switch (character) {
                            case '\r':
                                NuFileReadWChar(file);
                                /* fall through */
                            case '\n':
                            case '\0':
                                line[length] = 0;
                                return length;
                            default:
                                break;
                        }
                    }
                }
                /* fall through */
            default:
                has_text = 1;
                /* fall through */
            case ' ':
            case '\t':
                if (has_text != 0) {
                    line[length++] = character;
                }
                break;
        }
    }
}

void __cdecl NuStringTableSetFormat(i32 format) {
    nustring_format = (u32)format;
}

i32 __cdecl NuStringTableGetFormat(void) {
    return (i32)nustring_format;
}

i32 __cdecl NuStringTableLoadCSV(char *filepath, VARIPTR *buf, VARIPTR buf_end, char *label, char *language) {
    i32 parse_buf_size;
    NUSTRINGBANK *bank;
    u16 label_utf16[0x40];
    u16 lang_utf16[0x40];
    void *mem_buf;
    i32 bytes_read;
    char *word_buf;
    char *line_buf;
    char *tmp_buf;
    NUFILE mem_file;
    NUFPAR_PC *parser;
    NUSTRING *strings;
    i32 column;
    i32 label_column;
    i32 lang_column;
    i32 len;
    i32 i;
    i32 max_strings;
    i32 file_index;

    label_column = -1;
    lang_column = -1;
    i = 0;
    max_strings = 0;
    parse_buf_size = 0x8000;
    bank = &StringBank[CurrentStringBank];

    NuAsciiToUnicode(label_utf16, label);
    NuAsciiToUnicode(lang_utf16, language);
    if (bank->strings != NULL) {
        NuFileError();
    }
    bank->strings = NULL;
    bank->max_strings = 0;
    bank->string_count = 0;

    if (buf_end.addr - buf->addr < 0x14b000) {
        NuFileError();
    }

    mem_buf = (void *)(buf_end.addr - 0x100000);
    bytes_read = NuFileLoadBuffer(filepath, mem_buf, 0x100000);
    if ((i32)(0x100000 - bytes_read) < 0x18000) {
        NuFileError();
    }
    word_buf = (char *)((u32)mem_buf + bytes_read);
    line_buf = (char *)((u32)mem_buf + bytes_read + parse_buf_size);
    tmp_buf = (char *)((u32)mem_buf + bytes_read + parse_buf_size + parse_buf_size);

    mem_file = 0;
    if (bytes_read > 0) {
        for (file_index = 0; file_index < 20; file_index++) {
            if (pc_memfiles[file_index].used == 0) {
                pc_memfiles[file_index].buffer = (u8 *)mem_buf;
                pc_memfiles[file_index].end = (u8 *)mem_buf + bytes_read - 1;
                pc_memfiles[file_index].position = pc_memfiles[file_index].buffer;
                pc_memfiles[file_index].mode = 0;
                pc_memfiles[file_index].used = 1;
                mem_file = file_index + 0x400;
                break;
            }
        }
    }
    if (mem_file != 0) {
        parser = NuFParOpen(mem_file);
        if (parser != NULL) {
            parser->word_buffer_ptr = word_buf;
            parser->line_buffer_ptr = line_buf;
            parser->line_buffer_size = parse_buf_size;
            parser->word_buffer_size = parse_buf_size;
            while (NuFParGetLine(parser) != 0) {
                ++max_strings;
            }
            parser->file_buffer = NULL;
        }
        NuFileClose(mem_file);
    }

    strings = (NUSTRING *)((buf->addr + 3) & ~3U);
    buf->addr = (buf->addr + 3) & ~3U;
    buf->addr += max_strings * sizeof(NUSTRING);
    mem_file = 0;
    if (bytes_read > 0) {
        for (file_index = 0; file_index < 20; file_index++) {
            if (pc_memfiles[file_index].used == 0) {
                pc_memfiles[file_index].buffer = (u8 *)mem_buf;
                pc_memfiles[file_index].end = (u8 *)mem_buf + bytes_read - 1;
                pc_memfiles[file_index].position = pc_memfiles[file_index].buffer;
                pc_memfiles[file_index].mode = 0;
                pc_memfiles[file_index].used = 1;
                mem_file = file_index + 0x400;
                break;
            }
        }
    }
    if (mem_file != 0) {
        parser = NuFParOpen(mem_file);
        if (parser != NULL) {
            parser->word_buffer_ptr = word_buf;
            parser->line_buffer_ptr = line_buf;
            parser->line_buffer_size = parse_buf_size;
            parser->word_buffer_size = parse_buf_size;
            while (NuFParGetLine(parser) != 0) {
                column = 0;
                while (NuFParGetWord(parser) != 0) {
                    if (NuStrICmpW((NUWCHAR *)parser->word_buffer_ptr, label_utf16) == 0) {
                        label_column = column;
                    }
                    if (NuStrICmpW((NUWCHAR *)parser->word_buffer_ptr, lang_utf16) == 0) {
                        lang_column = column;
                    }
                    ++column;
                }
                if (label_column >= 0 && lang_column >= 0) {
                    break;
                }
            }

            if (label_column >= 0 && lang_column >= 0) {
                while (NuFParGetLine(parser) != 0) {
                    column = 0;
                    while (NuFParGetWord(parser) != 0) {
                        if (column == label_column) {
                            NU_WIDE_LENGTH(parser->word_buffer_ptr, len);
                            ++len;
                            strings[i].id = buf->char_ptr;
                            buf->addr += (u32)len;
                            NuUnicodeToAscii(strings[i].id, (u16 *)parser->word_buffer_ptr);
                        }
                        if (column == lang_column) {
                            if (nustring_format == 1) {
                                NU_WIDE_LENGTH(parser->word_buffer_ptr, len);
                                ++len;
                                strings[i].str_utf16 = (u16 *)((buf->addr + 1) & ~1U);
                                buf->addr = (buf->addr + 1) & ~1U;
                                buf->addr += (u32)len * 2;
                                NuStrCpyW(strings[i].str_utf16, (NUWCHAR *)parser->word_buffer_ptr);
                            } else {
                                NuUnicodeToUTF8((u8 *)tmp_buf, (u16 *)parser->word_buffer_ptr);
                                len = NuStrLen(tmp_buf) + 1;
                                strings[i].str_utf8 = (u8 *)((buf->addr + 1) & ~1U);
                                buf->addr = (buf->addr + 1) & ~1U;
                                buf->addr += (u32)len;
                                NuStrCpy(strings[i].str_utf8, tmp_buf);
                            }
                        }
                        ++column;
                        if (column > label_column && column > lang_column) {
                            if (strings[i].id != NULL && strings[i].str_utf16 != NULL) {
                                ++i;
                            } else {
                                strings[i].id = NULL;
                                strings[i].str_utf16 = NULL;
                            }
                            break;
                        }
                    }
                }
            }
            parser->file_buffer = NULL;
            bank->strings = strings;
            bank->string_count = i;
            bank->max_strings = max_strings;
        }
        NuFileClose(mem_file);
    }
    return bank->string_count;
}

i32 __cdecl NuStringTableLoadTXT(char *filepath, VARIPTR *buf, VARIPTR buf_end) {
    i32 i = 0;
    i32 max_strings = 0;
    NUSTRING *strings = NULL;
    NUSTRINGBANK *bank = &StringBank[CurrentStringBank];
    void *mem_buf = (void *)(buf_end.addr - 0x100000);
    i32 bytes_read;
    NUFILE mem_file;
    NUFPAR_PC *parser;
    i32 len;
    char *tmp_buf;
    i32 file_index;

    if (bank->strings != NULL) {
        NuFileError();
    }
    bank->strings = NULL;
    bank->max_strings = 0;
    bank->string_count = 0;
    bytes_read = NuFileLoadBuffer(filepath, mem_buf, 0x100000);
    if ((i32)(0x100000 - bytes_read) < 0x8000) {
        NuFileError();
    }
    tmp_buf = (char *)((u32)mem_buf + bytes_read);
    mem_file = 0;
    if (bytes_read > 0) {
        for (file_index = 0; file_index < 20; file_index++) {
            if (pc_memfiles[file_index].used == 0) {
                pc_memfiles[file_index].buffer = (u8 *)mem_buf;
                pc_memfiles[file_index].end = (u8 *)mem_buf + bytes_read - 1;
                pc_memfiles[file_index].position = pc_memfiles[file_index].buffer;
                pc_memfiles[file_index].mode = 0;
                pc_memfiles[file_index].used = 1;
                mem_file = file_index + 0x400;
                break;
            }
        }
    }
    if (mem_file != 0) {
        parser = NuFParOpen(mem_file);
        if (parser != NULL) {
            while (NuFParGetLine(parser) != 0) {
                ++max_strings;
            }
            parser->file_buffer = NULL;
        }
        NuFileClose(mem_file);
    }

    strings = (NUSTRING *)((buf->addr + 3) & ~3U);
    buf->addr = (buf->addr + 3) & ~3U;
    buf->addr += max_strings * sizeof(NUSTRING);
    mem_file = 0;
    if (bytes_read > 0) {
        for (file_index = 0; file_index < 20; file_index++) {
            if (pc_memfiles[file_index].used == 0) {
                pc_memfiles[file_index].buffer = (u8 *)mem_buf;
                pc_memfiles[file_index].end = (u8 *)mem_buf + bytes_read - 1;
                pc_memfiles[file_index].position = pc_memfiles[file_index].buffer;
                pc_memfiles[file_index].mode = 0;
                pc_memfiles[file_index].used = 1;
                mem_file = file_index + 0x400;
                break;
            }
        }
    }
    if (mem_file != 0) {
        parser = NuFParOpen(mem_file);
        if (parser != NULL) {
            while (NuFParGetLine(parser) != 0) {
                NuFParGetWord(parser);
                NuFParGetWord(parser);
                NU_WIDE_LENGTH(parser->word_buffer_ptr, len);
                ++len;
                strings[i].id = buf->char_ptr;
                buf->addr += (u32)len;
                NuUnicodeToAscii(strings[i].id, (u16 *)parser->word_buffer_ptr);
                NuFParGetWord(parser);
                NU_WIDE_LENGTH(parser->word_buffer_ptr, len);
                ++len;
                strings[i].str_utf16 = (u16 *)((buf->addr + 1) & ~1U);
                buf->addr = (buf->addr + 1) & ~1U;
                buf->addr += (u32)len * 2;
                NuStrCpyW(strings[i].str_utf16, (NUWCHAR *)parser->word_buffer_ptr);

                if (nustring_format == 1) {
                    NU_WIDE_LENGTH(parser->word_buffer_ptr, len);
                    ++len;
                    strings[i].str_utf16 = (u16 *)((buf->addr + 1) & ~1U);
                    buf->addr = (buf->addr + 1) & ~1U;
                    buf->addr += (u32)len * 2;
                    NuStrCpyW(strings[i].str_utf16, (NUWCHAR *)parser->word_buffer_ptr);
                } else {
                    NuUnicodeToUTF8((u8 *)tmp_buf, (u16 *)parser->word_buffer_ptr);
                    len = NuStrLen(tmp_buf) + 1;
                    strings[i].str_utf8 = (u8 *)((buf->addr + 1) & ~1U);
                    buf->addr = (buf->addr + 1) & ~1U;
                    buf->addr += (u32)len;
                    NuStrCpy(strings[i].str_utf8, tmp_buf);
                }
                ++i;
            }
            parser->file_buffer = NULL;
            bank->strings = strings;
            bank->string_count = i;
            bank->max_strings = max_strings;
        }
        NuFileClose(mem_file);
    }
    return bank->string_count;
}

void __cdecl NuStringTableSetBank(i32 bank) {
    if ((u32)bank <= 2) {
        CurrentStringBank = (i16)bank;
    }
}

i32 __cdecl NuStringTableLoad(char *filepath, VARIPTR *buf, VARIPTR buf_end) {
    char *extension = NuStrRChr(filepath, '.');

    if (extension != NULL && NuStrICmp(extension, ".csv") == 0) {
        return NuStringTableLoadCSV(filepath, buf, buf_end, "LABEL", "ENGLISH");
    }
    return NuStringTableLoadTXT(filepath, buf, buf_end);
}

void __cdecl NuStringTableUnload(void) {
    NUSTRINGBANK *bank = &StringBank[CurrentStringBank];
    bank->strings = NULL;
    bank->max_strings = 0;
    bank->string_count = 0;
}

i32 __cdecl NuStringTableGetIdByName(char *name) {
    i32 i;
    i32 j;

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < StringBank[i].string_count; ++j) {
            NUSTRING *string = &StringBank[i].strings[j];
            if (string->id != NULL && string->str_utf16 != NULL && NuStrICmp(string->id, name) == 0) {
                return j + (i << 24);
            }
        }
    }
    return -1;
}

u16 *__cdecl NuStringTableGetById(i32 id) {
    u32 bank = 0;

    if ((u32)id >= 0x1000000 && id != -1) {
        bank = (u32)id >> 24;
        id &= 0xffffff;
    }
    if (bank < 3 && id >= 0 && id < StringBank[bank].string_count)
        return StringBank[bank].strings[id].str_utf16;
    return NULL;
}

u16 *__cdecl NuStringTableGetByName(char *name) {
    i32 i;
    i32 j;

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < StringBank[i].string_count; ++j) {
            NUSTRING *string = &StringBank[i].strings[j];
            if (string->id != NULL && string->str_utf16 != NULL && NuStrICmp(string->id, name) == 0) {
                return StringBank[i].strings[j].str_utf16;
            }
        }
    }
    return (u16 *)NuBlankString;
}

void __cdecl NuStrFixExtPlatform(char *dst, char *src, char *ext, i32 dst_size, char *platform_string) {
    char *period;
    char *separator;
    char *windows_separator;
    char suffix[64];

    if (src == NULL || ext == NULL || dst == NULL) {
        return;
    }

    NuStrCpy(suffix, "PC");
    NuStrCat(suffix, ".");
    NuStrCat(suffix, ext);
    if (NuStrLen(src) > NuStrLen(suffix) &&
        NuStrNICmp(&src[NuStrLen(src) - NuStrLen(suffix)], suffix, NuStrLen(suffix)) == 0) {
        NuStrCpy(dst, src);
        return;
    }

    NuStrCpy(dst, src);
    period = NuStrRChr(dst, '.');
    separator = NuStrRChr(dst, '/');
    windows_separator = NuStrRChr(dst, '\\');
    if (windows_separator > separator) {
        separator = windows_separator;
    }

    if (separator >= period) {
        NuStrCat(dst, "_");
        if (platform_string != NULL) {
            NuStrCat(dst, platform_string);
        } else {
            NuStrCat(dst, "PC");
        }
        NuStrCat(dst, ".");
        NuStrCat(dst, ext);
    } else {
        *period = '\0';
        NuStrCat(period, "_");
        if (platform_string != NULL) {
            NuStrCat(period, platform_string);
        } else {
            NuStrCat(period, "PC");
        }
        NuStrCat(period, ".");
        NuStrCat(period, ext);
    }

    if (NuStrLen(dst) > dst_size - 1) {
        NuFileError();
    }
}

void __cdecl NuStrFixExt(char *dst, char *src, char *ext, i32 dst_size) {
    if (src == NULL || ext == NULL || dst == NULL) {
        return;
    }
    NuStrFixExtPlatform(dst, src, ext, dst_size, NULL);
}

void __cdecl NuStrGetFilenameNoExt(char *dst, char *src) {
    char *separator;
    char *backslash;
    i32 length;
    char *extension = NuStrRChr(src, '.');
    if (extension == NULL)
        extension = src + (NuStrLen(src) - 1);
    separator = NuStrRChr(src, '/');
    backslash = NuStrRChr(src, '\\');
    if (backslash > separator)
        separator = backslash;
    if (separator == NULL)
        separator = src;
    // Preserve the original's unconditional advance, even for a bare filename.
    ++separator;
    length = extension - separator + 1;
    NuStrNCpy(dst, separator, length);
}

void __cdecl NuStrGetPath(char *dst, char *src) {
    char *separator = NuStrRChr(src, '/');
    char *backslash = NuStrRChr(src, '\\');
    if (backslash > separator)
        separator = backslash;
    if (separator == NULL) {
        *dst = 0;
    } else {
        i32 length = separator - src + 1;
        NuStrNCpy(dst, src, length);
    }
}

void __cdecl NuStrGetExt(char *dst, char *src) {
    char *separator;
    char *backslash;
    char *extension = NuStrRChr(src, '.');
    separator = NuStrRChr(src, '/');
    backslash = NuStrRChr(src, '\\');
    if (backslash > separator)
        separator = backslash;
    if (extension == NULL || separator > extension) {
        *dst = 0;
    } else {
        char *end = src + NuStrLen(src);
        i32 length = end - extension;
        if (length == 0)
            *dst = 0;
        else
            NuStrNCpy(dst, extension + 1, length);
    }
}

void __cdecl NuStrSubstituteString(char *dst, char *src, char *search, char *replacement) {
    i32 matched = 0;
    while (*src != 0) {
        while (search[matched] != 0) {
            u8 source_character = (u8)src[matched];
            u8 search_character = (u8)search[matched];
            if ((u8)(source_character - 'A') <= 0x19 || (u8)(source_character + 0x40) <= 0x1f)
                source_character = (u8)(source_character + 0x20);
            if ((u8)(search_character - 'A') <= 0x19 || (u8)(search_character + 0x40) <= 0x1f)
                search_character = (u8)(search_character + 0x20);
            if (source_character != search_character) {
                matched = 0;
                break;
            }
            ++matched;
        }
        if (matched != 0) {
            src += matched;
            if (replacement != NULL) {
                matched = 0;
                while (replacement[matched] != 0) {
                    *dst = replacement[matched];
                    ++dst;
                    ++matched;
                }
            }
            matched = 0;
        } else {
            *dst = *src;
            ++dst;
            ++src;
        }
    }
    *dst = 0;
}

void __cdecl NuStringFilterLoad(char *path, VARIPTR *buf, VARIPTR buf_end) {
    i32 count;
    i32 i;
    i32 length;
    i32 j;
    void *mem_buf;
    i32 bytes_read;
    NUFILE file;
    NUFPAR_PC *parser;
    u16 word[128];
    i32 file_index;

    if (buf_end.addr - buf->addr < 0x100000) {
        NuFileError();
    }
    mem_buf = (void *)(buf_end.addr - 0x100000);
    bytes_read = NuFileLoadBuffer(path, mem_buf, 0x100000);
    count = 0;

    file = 0;
    if (bytes_read > 0) {
        for (file_index = 0; file_index < 20; file_index++) {
            if (pc_memfiles[file_index].used == 0) {
                pc_memfiles[file_index].buffer = (u8 *)mem_buf;
                pc_memfiles[file_index].end = (u8 *)mem_buf + bytes_read - 1;
                pc_memfiles[file_index].position = pc_memfiles[file_index].buffer;
                pc_memfiles[file_index].mode = 0;
                pc_memfiles[file_index].used = 1;
                file = file_index + 0x400;
                break;
            }
        }
    }
    if (file != 0) {
        parser = NuFParOpen(file);
        if (parser != NULL) {
            do {
                if (NuFParGetWord(parser) > 0) {
                    ++count;
                }
            } while (NuFParGetLine(parser) != 0);
            parser->file_buffer = NULL;
        }
        NuFileClose(file);
    }

    BadWords = (u16 **)((buf->addr + 3) & ~3U);
    buf->addr = (buf->addr + 3) & ~3U;
    buf->addr += (count + 1) * sizeof(u16 *);
    memset(BadWords, 0, (count + 1) * sizeof(u16 *));
    i = 0;

    file = 0;
    if (bytes_read > 0) {
        for (file_index = 0; file_index < 20; file_index++) {
            if (pc_memfiles[file_index].used == 0) {
                pc_memfiles[file_index].buffer = (u8 *)mem_buf;
                pc_memfiles[file_index].end = (u8 *)mem_buf + bytes_read - 1;
                pc_memfiles[file_index].position = pc_memfiles[file_index].buffer;
                pc_memfiles[file_index].mode = 0;
                pc_memfiles[file_index].used = 1;
                file = file_index + 0x400;
                break;
            }
        }
    }
    if (file != 0) {
        parser = NuFParOpen(file);
        if (parser != NULL) {
            do {
                if (NuFParGetWord(parser) > 0) {
                    NuUTF8ToUnicode(word, (u8 *)parser->word_buffer_ptr);
                    length = NuStrLenW(word);
                    if (length <= 3) {
                        for (j = length; j >= 0; --j) {
                            word[j + 1] = word[j];
                        }
                        word[0] = '!';
                        ++length;
                    }
                    ++length;
                    BadWords[i] = (u16 *)((buf->addr + 1) & ~1U);
                    buf->addr = (buf->addr + 1) & ~1U;
                    buf->addr += (u32)length * 2;
                    NuStrLwrW(BadWords[i], word);
                    ++i;
                }
            } while (NuFParGetLine(parser) != 0 && i < count);
            parser->file_buffer = NULL;
        }
        NuFileClose(file);
    }
    NumBadWords = i;
}

static i32 NuStringCharEquivalentPc(u16 a, u16 b) {
    i32 i = 0;
    if (a == b)
        return 1;
    while (bad_equiv[i][0] != 0) {
        if ((bad_equiv[i][1] == a && bad_equiv[i][0] == b) || (bad_equiv[i][1] == b && bad_equiv[i][0] == a))
            return 1;
        ++i;
    }
    return 0;
}

static i32 NuStringIsFluffPc(u16 character) {
    i32 i = 0;
    while (bad_fluff[i] != 0) {
        if (bad_fluff[i] == character)
            return 1;
        ++i;
    }
    return 0;
}

u16 *__cdecl NuStringBadSubString(const u16 *text, const u16 *word, i32 *length, i32 whole_word) {
    const u16 *cursor;
    const u16 *match;
    i32 boundary = 1;
    i32 equivalent;
    i32 is_fluff;

    while (*text != 0) {
        cursor = text;
        match = word;
        if (boundary || !whole_word) {
            while (*match != 0) {
                if (*cursor == 0) {
                    break;
                }
                equivalent = NuStringCharEquivalentPc(*cursor, *match);
                if (!equivalent) {
                    break;
                }
                ++cursor;
                while (*cursor != 0) {
                    is_fluff = NuStringIsFluffPc(*cursor);
                    if (!is_fluff && *cursor != *match) {
                        break;
                    }
                    ++cursor;
                }
                ++match;
            }
            if (*match == 0) {
                if (!whole_word || cursor[1] == 0) {
                    is_fluff = 1;
                } else {
                    is_fluff = NuStringIsFluffPc(cursor[1]);
                }
                if (is_fluff) {
                    *length = (i32)(cursor - text);
                    return (u16 *)text;
                }
            }
        }
        boundary = NuStringIsFluffPc(*text);
        ++text;
    }
    return NULL;
}

i32 __cdecl NuStringFilterBadWordsW(u16 *dest, u16 *source, u16 *replacement) {
    i32 i = 0;
    i32 changed = 0;
    i32 whole_word;
    u16 *word;
    u16 *match;
    i32 length;
    u16 lower[256];

    if (source != dest) {
        NuStrCpyW(dest, source);
    }
    NuStrLwrW(lower, dest);
    while (BadWords[i] != NULL) {
        if (BadWords[i][0] == '!') {
            word = BadWords[i] + 1;
            whole_word = 1;
        } else {
            word = BadWords[i];
            whole_word = 0;
        }
        match = NuStringBadSubString(lower, word, &length, whole_word);
        if (match != NULL) {
            NuStrCpyW(dest, replacement);
            changed = 1;
        }
        ++i;
    }
    return changed;
}

i32 __cdecl NuStringFilterBadWords(u8 *dest, u8 *source, u8 *replacement) {
    i32 changed;
    u16 text[128];
    u16 substitute[128];

    NuUTF8ToUnicode(text, source);
    NuUTF8ToUnicode(substitute, replacement);
    changed = NuStringFilterBadWordsW(text, text, substitute);
    NuUnicodeToUTF8(dest, text);
    return changed;
}

i32 __cdecl NuPPLoadBuffer(NUFILE file, void *buf, i32 buf_size) {
    i32 size;
    i32 loaded;
    u8 *source;
    struct NUPPHEADER_PC header;
    do {
        while (NuFileSeek(file, 0, 0) < 0) {
            NuFileRetry();
        }
        loaded = NuFileRead(file, &header, sizeof(header));
        if (loaded < 0) {
            NuFileRetry();
        }
    } while (loaded < 0);
    if (header.magic == 0x02434e52) {
        header.unpacked_size = NUPP_SWAP32(header.unpacked_size);
        header.packed_size = NUPP_SWAP32(header.packed_size) + 18;
        source = (u8 *)buf;
        source += header.unpacked_size + 256;
        source -= header.packed_size;
        do {
            NuFileRetry();
            while (NuFileSeek(file, 4, 0) < 0) {
                NuFileRetry();
            }
            NuFileRetry();
            loaded = NuFileRead(file, source, header.packed_size - 4);
            if (loaded < 0) {
                ((void(__cdecl *)(i32))NuFileRetry)(loaded);
            }
        } while (loaded < 0);
        NuPPUnpack(source - 4, buf);
        ((void(__cdecl *)(u32, u32, i32))NuFileRetry)(header.packed_size, header.unpacked_size, 0);
        size = header.unpacked_size;
    } else {
        size = (i32)NuFileOpenSize(file);
        if (size > buf_size) {
            ((void(__cdecl *)(i32, i32))NuFileError)(buf_size, size);
        }
        do {
            while (NuFileSeek(file, 0, 0) < 0) {
                NuFileRetry();
            }
            loaded = NuFileRead(file, buf, size);
            if (loaded < 0) {
                NuFileRetry();
            }
        } while (loaded < 0);
    }
    return size;
}

static void NuFileAddExtensionPC(const char *extension, i32 type, i32 platform) {
    char *ext;
    i32 len;
    PC_FILEEXTINFO *next;

    if (num_extensions >= 64) {
        NuFileError();
    }
    next = &extensions[num_extensions];
    len = 0;
    while (extension[len] != '\0') {
        ++len;
    }
    next->len = (char)len;
    if (len >= 13) {
        /* The retail no-op diagnostic accepts and ignores this stack argument. */
        ((void(__cdecl *)(i32))NuFileError)(13);
    }
    ext = next->extension;
    while (len != 0) {
        --len;
        *ext = extension[len];
        ++ext;
    }
    *ext = '\0';
    next->type = (char)type;
    next->platform = (char)platform;
    ++num_extensions;
}

f32 NuTimeGetFrameTime(void) {
    return nuapi.frame_time;
}

void __cdecl NuTimeGet(NuTime *time) {
    NuTimeGetTicksPS(&time->low, &time->high);
}

void __cdecl NuTimeSub(NuTime *time, NuTime *a, NuTime *b) {
    if (a->low >= b->low) {
        time->low = a->low - b->low;
        time->high = a->high - b->high;
    } else {
        time->low = a->low - b->low;
        time->high = a->high - 1 - b->high;
    }
}

f32 __cdecl NuTimeSeconds(NuTime *time) {
    u32 low;
    u32 high;
    f64 ticks;
    f64 ticks_per_second;

    NuTimeGetTicksPerSecondPS(&low, &high);
    ticks = time->low + time->high * NuTime_LowScale;
    ticks_per_second = low + high * NuTime_LowScale;
    return (f32)(ticks / ticks_per_second);
}

f32 __cdecl NuTimeMicroSeconds(NuTime *time) {
    u32 low;
    u32 high;
    f64 ticks;
    f64 ticks_per_second;

    NuTimeGetTicksPerSecondPS(&low, &high);
    ticks = time->low + time->high * NuTime_LowScale;
    ticks_per_second = low + high * NuTime_LowScale;
    return (f32)(ticks / ticks_per_second * NuTime_MicroScale);
}

f32 __cdecl NuTimeMilliSeconds(NuTime *time) {
    u32 low;
    u32 high;
    f64 ticks;
    f64 ticks_per_second;

    NuTimeGetTicksPerSecondPS(&low, &high);
    ticks = time->low + time->high * NuTime_LowScale;
    ticks_per_second = low + high * NuTime_LowScale;
    return (f32)(ticks / ticks_per_second * NuScale1000);
}

f32 __cdecl NuTimeScanlines(NuTime *time) {
    return (f32)(NuTimeSeconds(time) * NuTime_ScanlineScale * NuTime_RefreshRate);
}

void __cdecl NuCommandLine(i32 *__restrict count, char ***__restrict arguments) {
    // NuMain passes separate stack arguments, not these global cursors.
    argc = *count;
    argv = *arguments;
}

void __cdecl NuAPIInit(void) {
    memset(&nuapi, 0, sizeof(NUAPI));
    nuapi.fps = 60.0f;
    nuapi.video_mode = 2;
    nuapi.video_swap_mode = 5;
    nuapi.language = 1;
    nuapi.video_aspect = 0;
    nuapi.frame_end_state = 0;
    nuapi.pad_record.buf_size = NUAPI_PADREC_DEFAULT_BUFFERSIZE;
    nuapi.pad_record.end_record_buttons = 0;
    nuapi.pad_record.end_play_buttons = 0;
    NuWindInitialise((NUWIND *)nuapi.wind);
}

void __cdecl NuFrameBegin(void) {
    ++nuapi.frame_begin_count;
    NuPortalInit();
    NuWaterReset();
    NuMouseRead();
    NuKeyboardRead();
    NuOcclusionManagerBeginFrame();
}

void __cdecl NuApiPrintLinePc(u32 colour, const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    NuQFntPushPrintMode(2);
    NuQFntPushCoordinateSystem(1);
    NuQFntSet(system_qfont);
    NuQFntSetColour(system_qfont, colour);
    NuQFntSetScale(system_qfont, 0.7f, 0.7f);
    NuQFntPrintEx(system_qfont, 0x140, NuApiTextLinePc * 8, 0x10, buffer);
    NuQFntPopPrintMode();
    NuQFntPopCoordinateSystem();
    NuApiTextLinePc += 20;
}

/* Shows the pending error messages for up to five seconds, or until the jump
 * button is pressed after the first second. */
void __cdecl NuErrorSleep(void) {
    f32 milliseconds = 0.0f;
    i32 waiting = 1;
    i32 index;
    f32 y;
    char *message;
    FRAME_PAD_PC pad;
    NuTime start, now, elapsed, rate;
    memset(&pad, 0, sizeof(pad));
    NuTimeGetTicksPS(&start.low, &start.high);
    if ((i32)start.high < 0) {
        NuFileError();
    }
    while (milliseconds < 5000.0f && waiting != 0) {
        y = 500.0f;
        index = 0;
        NuTimeGetTicksPS(&now.low, &now.high);
        if ((i32)now.high < 0) {
            NuFileError();
        }
        NUFRAME_ELAPSED(elapsed, now, start);
        NuTimeGetTicksPerSecondPS(&rate.low, &rate.high);
        milliseconds =
            (f32)((elapsed.high * NuTime_LowScale + elapsed.low) / (rate.high * NuTime_LowScale + rate.low) * 1000.0);
        if (nuapi.frame_begin_count != 0) {
            NuFileError();
        }
        if (++nuapi.frame_begin_count < 1) {
            NuFileError();
        }
        NuPortalInit();
        NuWaterReset();
        NuMouseRead();
        NuKeyboardRead();
        NuOcclusionManagerBeginFrame();
        NuRndrBeginScene(0);
        NuRndrClear(0xb00, 0xff00ffff, 1.0f);
        NuQFntSet(system_qfont);
        NuQFntPushPrintMode(2);
        NuQFntPushCoordinateSystem(1);
        NuQFntSet(system_qfont);
        while (*(message = NuGetErrN(index++)) != '\0') {
            NuQFntSetColour(system_qfont, 0xff000000);
            NuQFntMove(system_qfont, 500.0f, y, 0.0f);
            NuQFntPrintV(system_qfont, message, NULL);
            y += 150.0f;
        }
        NuQFntPopCoordinateSystem();
        NuQFntPopPrintMode();
        NuRndrEndScene();
        NuPadRead(&pad);
        if ((pad.digital_buttons & 0x40) != 0 && milliseconds > 1000.0f)
            waiting = 0;
        NuFrameEnd();
    }
}

f32 __cdecl NuFrameEnd(void) {
    NuTime elapsed;
    NuTime now;
    f32 refresh_rate;
    f32 target_interval;
    f32 capped_rate;
    f32 capped_interval;
    i32 had_error = NuHasError();

    if (FrameStartupDelayPc > 0.0f) {
        FrameStartupDelayPc -= nuapi.frame_time;
        NuApiTextLinePc = 20;
        NuRndrBeginScene(1);
        if (PcFrameDelayMessagePc() != NULL) {
            NuApiPrintLinePc(0xffffffffu, PcFrameDelayMessagePc());
        }
        NuRndrEndScene();
    }
    NuMtlAnimate(nuapi.frame_time);
    NuTexAnimProcess(nuapi.frame_time);
    NuWindAnimate(nuapi.wind, nuapi.frame_time);
    NuFrameEndLeafPc();
    PCDisplayListCriticalSectionEnterPc();
    PCFrameCriticalSectionEnter_candidate();
    /* DispatchMessage may update the native focus and policy flags. */
    if (SoundVoiceEnabledGate == 0) {
        if (PcNoFocusPolicy != 0) {
            Sleep(1);
            NuWindowsProcessMessages();
        } else {
            NuSound3SuspendForFocusPc();
            while (SoundVoiceEnabledGate == 0) {
                Sleep(10);
                if (NuWindowsProcessMessages() == 0) {
                    break;
                }
            }
            FmvPlatformUpdatePc();
            NuTimeGetTicksPS(&nuapi.time.low, &nuapi.time.high);
            if ((i32)nuapi.time.high < 0) {
                NuFileError();
            }
            NuSound3ResumeAfterFocusPc();
        }
    } else {
        NuWindowsProcessMessages();
        FmvPlatformUpdatePc();
    }
    NuPadRecordEndFrame();
    NuTimeBarSetRender(-1);
    NuRndrSwapScreenEx(1, NuApiEndFrameCallbackPc);
    PCDisplayListCriticalSectionLeavePc();
    PCFrameCriticalSectionLeave_candidate();

    NuTimeGetTicksPS(&now.low, &now.high);
    if ((i32)now.high < 0) {
        NuFileError();
    }
    NUFRAME_ELAPSED(elapsed, now, nuapi.time);
    nuapi.frame_time = NuTimeSeconds(&elapsed);
    NuFrameAccumulatedTimePc += nuapi.frame_time;

    refresh_rate = 85.0 > (f32)PcGetRefreshRatePc() ? 85.0f : (f32)PcGetRefreshRatePc();
    target_interval = 1.0f / refresh_rate;
    capped_rate = 85.0 < (f32)PcGetRefreshRatePc() ? 85.0f : (f32)PcGetRefreshRatePc();
    capped_interval = 1.0f / capped_rate;
    FrameSleepMillisecondsPc = 20;
    if (!(nuapi.frame_time / capped_interval < 1.1f)) {
        FrameSleepMillisecondsPc = 1;
    }
    if (target_interval > nuapi.frame_time) {
        Sleep((DWORD)((target_interval - nuapi.frame_time) * 1000.0));
        NuTimeGetTicksPS(&now.low, &now.high);
        if ((i32)now.high < 0) {
            NuFileError();
        }
        NUFRAME_ELAPSED(elapsed, now, nuapi.time);
        nuapi.frame_time = NuTimeSeconds(&elapsed);
    }
    NuTimeGetTicksPS(&nuapi.time.low, &nuapi.time.high);
    if ((i32)nuapi.time.high < 0) {
        NuFileError();
    }
    NuTimeGetTicksPS(&nuapi.time2.low, &nuapi.time2.high);
    if ((i32)nuapi.time2.high < 0) {
        NuFileError();
    }
    bgprocFrameSleepPc(FrameSleepMillisecondsPc);
    NuPadUpdatePads();
    ++nuapi.frame_count;
    if (nuapi.forced_frame_time != 0.0f) {
        nuapi.frame_time = nuapi.forced_frame_time;
    }
    --nuapi.frame_begin_count;

    if (ShowingError == 0 && had_error != 0) {
        ShowingError = 1;
        NuErrorSleep();
        NuClearError();
        ShowingError = 0;
    }

    nuapi.frame_end_state = 0;
    return nuapi.frame_time;
}

void __cdecl NuApiSetEndFrameCallbackPc(void(__cdecl *callback)(void)) {
    NuApiEndFrameCallbackPc = callback;
}

void ParseCommandLine() {
    i32 argument_count = argc;
    if (argument_count > 0) {
        char **argument = argv;
        do {
            if (*argument == NULL)
                break;
            if (NuStrICmp(*argument, "PADRECORD") == 0) {
                ++argument;
                --argument_count;
                if (argument_count == 0)
                    NuFileError();
                nuapi.pad_record.filepath = *argument;
                nuapi.pad_record.mode = NUPAD_RECORD;
            } else if (NuStrICmp(*argument, "PADPLAY") == 0) {
                ++argument;
                --argument_count;
                if (argument_count == 0)
                    NuFileError();
                nuapi.pad_record.filepath = *argument;
                nuapi.pad_record.mode = NUPAD_PLAY;
            }
            --argument_count;
            ++argument;
        } while (argument_count > 0);
        argv = argument;
        argc = argument_count;
    }
}

static void DeadZoneValueXYProper(i32 *x_value, i32 *y_value, i32 deadzone) {
    i32 inverse;
    i32 cur_value;

    inverse = ANALOG_CENTER - deadzone;

    cur_value = *x_value;
    if (cur_value >= deadzone) {
        cur_value++;

        *x_value = ((cur_value - deadzone) * 0x7f) / inverse;
    } else if (cur_value < -deadzone) {
        *x_value = ((cur_value + deadzone) * 0x7f) / inverse;
    } else {
        *x_value = 0;
    }

    cur_value = *y_value;
    if (cur_value >= deadzone) {
        cur_value++;

        *y_value = ((cur_value - deadzone) * 0x7f) / inverse;
    } else if (cur_value < -deadzone) {
        *y_value = ((cur_value + deadzone) * 0x7f) / inverse;
    } else {
        *y_value = 0;
    }
}

static void DeadZoneValueXY(i32 *x_value, i32 *y_value, i32 deadzone) {
    f32 x_normal;
    f32 y_normal;
    f32 deadzone_normal;
    f32 magnitude;

    if (FramePadCorrectDeadZoningPc) {
        DeadZoneValueXYProper(x_value, y_value, deadzone);

        return;
    }

    x_normal = *x_value / 127.0f;
    y_normal = *y_value / 127.0f;
    deadzone_normal = deadzone / 127.0f;

    if (x_normal > 1.0f) {
        x_normal = 1.0f;
    }

    if (x_normal < -1.0f) {
        x_normal = -1.0f;
    }

    if (y_normal > 1.0f) {
        y_normal = 1.0f;
    }

    if (y_normal < -1.0f) {
        y_normal = -1.0f;
    }

    magnitude = NuFsqrt(x_normal * x_normal + y_normal * y_normal);

    if (magnitude < deadzone_normal) {
        *x_value = 0;
        *y_value = 0;
    }
}

FRAME_PAD_PC *NuPadOpen(i32 device, i32 slot) {
    FRAME_PAD_PC *pad = (FRAME_PAD_PC *)NuAlloc(sizeof(FRAME_PAD_PC) + 0x800,
                                                "d:\\projects\\game_saga\\nu2api.saga\\nucore\\nupad.cpp", 180);
    if (pad != NULL) {
        memset(pad, 0, sizeof(*pad));
        pad->device = device;
        pad->slot = slot;
        pad->stage = 0;
        pad->analog_right_x = pad->analog_right_y = pad->analog_left_x = pad->analog_left_y = 128;
    }
    NuPadOpenPS(pad);
    return pad;
}

static f32 CalcRamp(f32 ramp) {
    i32 i;
    if (!(ramp < 1.0f))
        ramp = 1.0f;
    for (i = 0; ramp > FramePadRampValuesPc[i].threshold; ++i) {
    }
    return FramePadRampValuesPc[i].value;
}

void NuPadProcessReadData(NUGENERICPAD *pad) {
    f32 ramp_rate_normal;

    pad->digital_buttons_pressed = pad->digital_buttons & ~pad->digital_buttons_prev;
    pad->digital_buttons_released = ~pad->digital_buttons & pad->digital_buttons_prev;

    pad->digital_buttons_prev = pad->digital_buttons;

    pad->digital_left_x = pad->analog_left_x - ANALOG_CENTER;
    pad->digital_left_y = ANALOG_CENTER - pad->analog_left_y;

    pad->digital_right_x = pad->analog_right_x - ANALOG_CENTER;
    pad->digital_right_y = ANALOG_CENTER - pad->analog_right_y;

    if (pad->deadzone != 0) {
        DeadZoneValueXY(&pad->digital_left_x, &pad->digital_left_y, pad->deadzone);
        DeadZoneValueXY(&pad->digital_right_x, &pad->digital_right_y, pad->deadzone);

        // Keep analog values consistent after deadzone modification.
        pad->analog_left_x = pad->digital_left_x + ANALOG_CENTER;
        pad->analog_left_y = ANALOG_CENTER - pad->digital_left_y;

        pad->analog_right_x = pad->digital_right_x + ANALOG_CENTER;
        pad->analog_right_y = ANALOG_CENTER - pad->digital_right_y;
    }

    pad->left_x_normal = (f32)pad->digital_left_x;
    pad->left_y_normal = (f32)pad->digital_left_y;

    pad->left_x_normal /= 128.0f;
    pad->left_y_normal /= 128.0f;

    if (pad->left_x_normal > 0.0f) {
        pad->left_x_normal = MAX(0.0f, pad->left_x_normal - pad->left_x_unknown2);
    }

    if (pad->left_x_normal < 0.0f) {
        pad->left_x_normal = MIN(0.0f, pad->left_x_normal + pad->left_x_unknown2);
    }

    if (pad->left_y_normal > 0.0f) {
        pad->left_y_normal = MAX(0.0f, pad->left_y_normal - pad->left_y_unknown2);
    }

    if (pad->left_y_normal < 0.0f) {
        pad->left_y_normal = MIN(0.0f, pad->left_y_normal + pad->left_y_unknown2);
    }

    pad->right_x_normal = (f32)pad->digital_right_x;
    pad->right_y_normal = (f32)pad->digital_right_y;

    pad->right_x_normal /= 128.0f;
    pad->right_y_normal /= 128.0f;

    if (pad->right_x_normal > 0.0f) {
        pad->right_x_normal = MAX(0.0f, pad->right_x_normal - pad->right_x_unknown2);
    }

    if (pad->right_x_normal < 0.0f) {
        pad->right_x_normal = MIN(0.0f, pad->right_x_normal + pad->right_x_unknown2);
    }

    if (pad->right_y_normal > 0.0f) {
        pad->right_y_normal = MAX(0.0f, pad->right_y_normal - pad->right_y_unknown2);
    }

    if (pad->right_y_normal < 0.0f) {
        pad->right_y_normal = MIN(0.0f, pad->right_y_normal + pad->right_y_unknown2);
    }

    if ((pad->analog_button_flags & 1) != 0) {
        // The right-hand side values never appear to be written. This may be a
        // platform difference.
        pad->analog_left_pad_left = pad->unknown_7c;
        pad->analog_left_pad_right = pad->unknown_7d;
        pad->analog_left_pad_up = pad->unknown_7e;
        pad->analog_left_pad_down = pad->unknown_7f;
    } else {
        ramp_rate_normal = FramePadRampRatePc / 30.0f;

        if ((pad->digital_buttons & PAD_BUTTON_UP) != 0) {
            pad->left_up_ramp = MIN(1.0f, pad->left_up_ramp + ramp_rate_normal);
        } else {
            pad->left_up_ramp = 0.0f;
        }

        if ((pad->digital_buttons & PAD_BUTTON_DOWN) != 0) {
            pad->left_down_ramp = MIN(1.0f, pad->left_down_ramp + ramp_rate_normal);
        } else {
            pad->left_down_ramp = 0.0f;
        }

        if ((pad->digital_buttons & PAD_BUTTON_LEFT) != 0) {
            pad->left_left_ramp = MIN(1.0f, pad->left_left_ramp + ramp_rate_normal);
        } else {
            pad->left_left_ramp = 0.0f;
        }

        if ((pad->digital_buttons & PAD_BUTTON_RIGHT) != 0) {
            pad->left_right_ramp = MIN(1.0f, pad->left_right_ramp + ramp_rate_normal);
        } else {
            pad->left_right_ramp = 0.0f;
        }

        pad->analog_left_pad_left = CalcRamp(pad->left_left_ramp) * 255.0f;
        pad->analog_left_pad_right = CalcRamp(pad->left_right_ramp) * 255.0f;
        pad->analog_left_pad_up = CalcRamp(pad->left_up_ramp) * 255.0f;
        pad->analog_left_pad_down = CalcRamp(pad->left_down_ramp) * 255.0f;
    }

    if ((pad->analog_button_flags & 2) != 0) {
        pad->unknown_82 = pad->unknown_80;
        pad->unknown_83 = pad->unknown_81;
    } else {
        pad->unknown_82 = 0x7f;
        pad->unknown_83 = 0x7f;
    }
}

void NuPadSetMaxGamePads(i32 count) {
    FramePadMaxGamePadsPc = count;
}

void __cdecl NuPadSetValid(i32 port, i32 valid) {
    g_nupadScannedPads[port].is_valid = (u8)valid;
}

// Uncompiled nupad.c append; Save133 belongs to peer material_review.
void NuPadUpdatePads(void) {
    i32 max_interface_devices;
    i32 max_game_pads;
    i32 i;
    i32 j;
    i32 has_active_pad;
    i32 selected = -1;

    max_interface_devices = NuPad_Interface_GetMaxDevices();
    max_game_pads = FramePadMaxGamePadsPc;

    for (i = 0; i < max_interface_devices; i++) {
        g_nupadScannedPads[i].is_valid = (u8)NuPadReadOutputsPc(
            i, &g_nupadScannedPads[i].analog_left_x, &g_nupadScannedPads[i].analog_left_y,
            &g_nupadScannedPads[i].analog_right_x, &g_nupadScannedPads[i].analog_right_y,
            &g_nupadScannedPads[i].analog_l1, &g_nupadScannedPads[i].analog_l2, &g_nupadScannedPads[i].analog_r1,
            &g_nupadScannedPads[i].analog_r2, &g_nupadScannedPads[i].digital_buttons, &g_nupadScannedPads[i].unknown_7c,
            &g_nupadScannedPads[i].analog_button_flags);

        NuPadRecordPlay(g_nupadScannedPads + i);

        if (g_nupadScannedPads[i].is_valid) {
            g_nupadScannedPads[i].deadzone = PcInputDeadzonePc(i);
        }

        NuPadProcessReadData(g_nupadScannedPads + i);
    }

    if (nuapi.pad_record.should_end == 1) {
        switch (nuapi.pad_record.mode) {
            case NUPAD_RECORD:
                NuPadRecordSave(nuapi.pad_record.filepath);
                break;
            case NUPAD_PLAY:
                nuapi.pad_record.mode = NUPAD_NORM;
                nuapi.pad_record.record.u8_ptr = nuapi.pad_record.buf_start;
                break;
        }
        nuapi.pad_record.should_end = 0;
    }

    for (i = 0; i < max_interface_devices; i++) {
        has_active_pad = 0;

        for (j = 0; j < max_game_pads; j++) {
            if (g_nupadMapping[j].is_active && i == g_nupadMapping[j].port) {
                has_active_pad = 1;
            }
        }

        if (has_active_pad) {
            continue;
        }

        g_nupadScannedPads[i].mapped_to_pad = -1;
    }

    if (!FramePadActivatedPc) {
        g_nupadMapping[0].port = -1;

        for (j = 0; j < max_interface_devices; j++) {
            if (g_nupadScannedPads[j].is_valid) {
                if (g_nupadMapping[0].port == -1) {
                    g_nupadMapping[0].port = j;
                    g_nupadScannedPads[j].mapped_to_pad = 0;
                }

                if (g_nupadScannedPads[j].digital_buttons != 0 ||
                    g_nupadScannedPads[j].analog_left_x != ANALOG_CENTER ||
                    g_nupadScannedPads[j].analog_left_y != ANALOG_CENTER ||
                    g_nupadScannedPads[j].analog_right_x != ANALOG_CENTER ||
                    g_nupadScannedPads[j].analog_right_y != ANALOG_CENTER) {
                    g_nupadMapping[0].port = j;
                    g_nupadScannedPads[j].mapped_to_pad = 0;

                    if ((g_nupadScannedPads[j].digital_buttons_pressed & 0x840) != 0) {
                        return;
                    }
                }
            }
        }
    }

    if (FramePadActivatedPc) {
        for (i = 0; i < max_game_pads; i++) {
            if (g_nupadMapping[i].is_active == 1 && !g_nupadScannedPads[g_nupadMapping[i].port].is_valid) {
                for (j = 0; j < max_interface_devices; j++) {
                    if (g_nupadScannedPads[j].is_valid &&
                        (g_nupadScannedPads[j].digital_buttons_pressed & 0x800) != 0 &&
                        (g_nupadScannedPads[j].mapped_to_pad == -1 ||
                         !g_nupadMapping[g_nupadScannedPads[j].mapped_to_pad].is_active)) {
                        g_nupadMapping[i].port = j;
                        g_nupadScannedPads[j].mapped_to_pad = i;
                        g_nupadScannedPads[j].digital_buttons_pressed = 0;

                        goto next_pad;
                    }
                }
            }

            if (!g_nupadMapping[i].is_active) {
                g_nupadMapping[i].port = -1;

                if (g_nupadMapping[0].is_active + g_nupadMapping[1].is_active == 0) {
                    if (i == 0) {
                        for (j = 0; j < max_interface_devices; j++) {
                            if (g_nupadScannedPads[j].mapped_to_pad != -1 &&
                                g_nupadMapping[g_nupadScannedPads[j].mapped_to_pad].is_active == 1) {
                                continue;
                            }

                            g_nupadScannedPads[j].mapped_to_pad = -1;
                        }
                    }
                } else {
                    for (j = 0; j < max_interface_devices; j++) {
                        if (g_nupadScannedPads[j].mapped_to_pad != -1 &&
                            g_nupadMapping[g_nupadScannedPads[j].mapped_to_pad].is_active == 1) {
                            continue;
                        }

                        g_nupadScannedPads[j].mapped_to_pad = -1;
                    }
                }

                for (j = 0; j < max_interface_devices; j++) {
                    if (g_nupadScannedPads[j].mapped_to_pad == -1 && g_nupadScannedPads[j].is_valid) {
                        g_nupadMapping[i].port = j;
                        g_nupadScannedPads[j].mapped_to_pad = i;
                        /* PC: only the last candidate keeps the player; release the previous one. */
                        if (selected >= 0) {
                            g_nupadScannedPads[selected].mapped_to_pad = -1;
                        }
                        selected = j;

                        if (g_nupadScannedPads[j].digital_buttons != 0 ||
                            g_nupadScannedPads[j].analog_left_x != ANALOG_CENTER ||
                            g_nupadScannedPads[j].analog_left_y != ANALOG_CENTER ||
                            g_nupadScannedPads[j].analog_right_x != ANALOG_CENTER ||
                            /* As on every platform, analog_right_x is tested twice. */
                            g_nupadScannedPads[j].analog_right_x != ANALOG_CENTER ||
                            (g_nupadScannedPads[j].digital_buttons_pressed & 0x840) != 0) {
                            break;
                        }
                    }
                }
            }
        next_pad:;
        }
    }
}

void __cdecl NuPadMapPlayerToPort(i32 player, i32 port) {
    i32 old_port;

    if (port >= 0) {
        NuKeyboardClearActiveDevicesPc();
        NuPadSetStatus(player, 0);

        old_port = g_nupadMapping[player].port;
        if (old_port >= 0) {
            g_nupadScannedPads[old_port].mapped_to_pad = -1;
        }
        g_nupadScannedPads[port].digital_buttons_pressed = 0;
        g_nupadMapping[player].port = port;
        g_nupadScannedPads[port].mapped_to_pad = player;

        NuPadSetStatus(player, 1);
        PlayerLastPort[player] = (i16)port;
        NuKeyboardApplyActionKeys_candidate(PlayerLastPort, 8);
    } else {
        NuPadSetStatus(player, 0);
    }
}

i32 __cdecl NuPadGetPort(i32 player) {
    return g_nupadMapping[player].is_active != 0 ? g_nupadMapping[player].port : -1;
}

i32 __cdecl NuPadGetPlayer(i32 port) {
    i32 player = -1;
    if (g_nupadScannedPads[port].is_valid) {
        player = g_nupadScannedPads[port].mapped_to_pad;
    }
    return player;
}

void NuPadInit(void) {
    i32 i;
    memset(g_nupadScannedPads, 0, sizeof(g_nupadScannedPads));
    for (i = 0; i < 16; ++i) {
        g_nupadScannedPads[i].mapped_to_pad = -1;
        NuPadInitPS(&g_nupadScannedPads[i]);
    }
    for (i = 0; i < 3; ++i) {
        g_nupadMapping[i].pad = -1;
        g_nupadMapping[i].port = -1;
        g_nupadMapping[i].is_active = 0;
    }
    FramePadActivatedPc = 0;
}

i32 NuPadRead(FRAME_PAD_PC *pad) {
    i32 device;

    if (g_nupadMapping[pad->device].port != -1 ||
        (FramePadDirectMappingPc && g_nupadScannedPads[pad->device].is_valid)) {
        if (FramePadDirectMappingPc != 0) {
            device = pad->device;
        } else {
            device = g_nupadMapping[pad->device].port;
        }

        pad->analog_left_x = g_nupadScannedPads[device].analog_left_x;
        pad->analog_left_y = g_nupadScannedPads[device].analog_left_y;
        pad->analog_right_x = g_nupadScannedPads[device].analog_right_x;
        pad->analog_right_y = g_nupadScannedPads[device].analog_right_y;

        pad->analog_l1 = g_nupadScannedPads[device].analog_l1;
        pad->analog_l2 = g_nupadScannedPads[device].analog_l2;
        pad->analog_r1 = g_nupadScannedPads[device].analog_r1;
        pad->analog_r2 = g_nupadScannedPads[device].analog_r2;

        pad->digital_buttons = g_nupadScannedPads[device].digital_buttons;
        pad->digital_buttons_pressed = g_nupadScannedPads[device].digital_buttons_pressed;
        pad->digital_buttons_released = g_nupadScannedPads[device].digital_buttons_released;
        pad->digital_buttons_prev = g_nupadScannedPads[device].digital_buttons_prev;

        pad->has_analog_buttons = g_nupadScannedPads[device].analog_button_flags;

        pad->deadzone = g_nupadScannedPads[device].deadzone;

        pad->digital_left_x = g_nupadScannedPads[device].digital_left_x;
        pad->digital_left_y = g_nupadScannedPads[device].digital_left_y;
        pad->digital_right_x = g_nupadScannedPads[device].digital_right_x;
        pad->digital_right_y = g_nupadScannedPads[device].digital_right_y;

        pad->left_x_normal = g_nupadScannedPads[device].left_x_normal;
        pad->left_y_normal = g_nupadScannedPads[device].left_y_normal;
        pad->left_x_unknown = g_nupadScannedPads[device].left_x_unknown;
        pad->left_y_unknown = g_nupadScannedPads[device].left_y_unknown;
        pad->left_x_unknown2 = g_nupadScannedPads[device].left_x_unknown2;
        pad->left_y_unknown2 = g_nupadScannedPads[device].left_y_unknown2;

        pad->right_x_unknown2 = g_nupadScannedPads[device].right_x_unknown2;
        pad->right_y_unknown2 = g_nupadScannedPads[device].right_y_unknown2;
        pad->right_x_normal = g_nupadScannedPads[device].right_x_normal;
        pad->right_y_normal = g_nupadScannedPads[device].right_y_normal;
        pad->right_x_unknown = g_nupadScannedPads[device].right_x_unknown;
        pad->right_y_unknown = g_nupadScannedPads[device].right_y_unknown;

        pad->analog_left_pad_left = g_nupadScannedPads[device].analog_left_pad_left;
        pad->analog_left_pad_right = g_nupadScannedPads[device].analog_left_pad_right;
        pad->analog_left_pad_up = g_nupadScannedPads[device].analog_left_pad_up;
        pad->analog_left_pad_down = g_nupadScannedPads[device].analog_left_pad_down;

        pad->unknown_b0 = g_nupadScannedPads[device].unknown_82;
        pad->unknown_b1 = g_nupadScannedPads[device].unknown_83;

        pad->is_valid = g_nupadScannedPads[device].is_valid;

        return pad->is_valid;
    }

    pad->is_valid = 0;
    pad->digital_buttons_prev = pad->digital_buttons_released = pad->digital_buttons_pressed = pad->digital_buttons = 0;

    return 0;
}

void NuPadRecordEndFrame() {
    FRAME_RECORD_CURSOR_PC *cursor;
    if (nuapi.pad_record.is_recording) {
        cursor = (FRAME_RECORD_CURSOR_PC *)&nuapi.pad_record.buf_start;
        nuapi.pad_record.is_recording = 0;
    }
    cursor = &nuapi.pad_record.record;
    switch (nuapi.pad_record.mode) {
        case NUPAD_PLAY:
            if (cursor->u8_ptr == nuapi.pad_record.buf_end) {
                nuapi.pad_record.mode = NUPAD_NORM;
            } else {
                nuapi.frame_time = *cursor->f32_ptr++;
            }
            break;
        case NUPAD_RECORD:
            *cursor->f32_ptr++ = nuapi.frame_time;
            if (cursor->u8_ptr > nuapi.pad_record.buf_end - 0x1840) {
                nuapi.pad_record.mode = NUPAD_NORM;
            }
            break;
        default:
            break;
    }
}

void NuPadResetState(i32 preserve_scanned_state) {
    FramePadActivatedPc = 0;
    FramePadProfilePortPc = -1;
    g_nupadMapping[0].port = -1;
    g_nupadMapping[1].port = -1;

    if (preserve_scanned_state == 0) {
        i32 i;
        for (i = 0; i <= 3; i++) {
            g_nupadScannedPads[i].digital_buttons_pressed = 0;
            g_nupadScannedPads[i].digital_buttons_prev = g_nupadScannedPads[i].digital_buttons_pressed;
            g_nupadScannedPads[i].digital_buttons = g_nupadScannedPads[i].digital_buttons_prev;
        }
    }
}

void NuPadSetStatus(i32 pad, i32 status) {
    i32 device = g_nupadMapping[pad].port;
    if (device >= 0) {
        if (status != 0) {
            g_nupadScannedPads[device].mapped_to_pad = pad;
        } else {
            g_nupadScannedPads[device].mapped_to_pad = -1;
        }
    }
    if (status == 1) {
        if (FramePadActivatedPc == 0)
            FramePadProfilePortPc = device;
        FramePadActivatedPc = 1;
    }
    g_nupadMapping[pad].is_active = status;
}

void NuPadRecordPlay(NUGENERICPAD *pad) {
    FRAME_RECORD_CURSOR_PC *record_ptr;

    if (pad->is_valid && nuapi.pad_record.mode == NUPAD_PLAY &&
        (pad->digital_buttons & nuapi.pad_record.end_play_buttons) != 0) {
        nuapi.pad_record.mode = NUPAD_NORM;
    }

    if (pad->is_valid && nuapi.pad_record.mode == NUPAD_RECORD &&
        (pad->digital_buttons & nuapi.pad_record.end_record_buttons) != 0) {
        nuapi.pad_record.should_end = 1;
    }

    record_ptr = &nuapi.pad_record.record;

    switch (nuapi.pad_record.mode) {
        case NUPAD_RECORD:
            *record_ptr->u32_ptr++ = pad->is_valid;

            if (pad->is_valid) {
                *record_ptr->u8_ptr++ = pad->analog_left_x;
                *record_ptr->u8_ptr++ = pad->analog_left_y;
                *record_ptr->u8_ptr++ = pad->analog_right_x;
                *record_ptr->u8_ptr++ = pad->analog_right_y;

                *record_ptr->u8_ptr++ = pad->analog_l1;
                *record_ptr->u8_ptr++ = pad->analog_l2;
                *record_ptr->u8_ptr++ = pad->analog_r1;
                *record_ptr->u8_ptr++ = pad->analog_r2;

                *record_ptr->u32_ptr++ = pad->digital_buttons;
            }
            break;
        case NUPAD_PLAY:
            pad->is_valid = *record_ptr->u32_ptr++;

            if (pad->is_valid) {
                pad->analog_left_x = *record_ptr->u8_ptr++;
                pad->analog_left_y = *record_ptr->u8_ptr++;
                pad->analog_right_x = *record_ptr->u8_ptr++;
                pad->analog_right_y = *record_ptr->u8_ptr++;

                pad->analog_l1 = *record_ptr->u8_ptr++;
                pad->analog_l2 = *record_ptr->u8_ptr++;
                pad->analog_r1 = *record_ptr->u8_ptr++;
                pad->analog_r2 = *record_ptr->u8_ptr++;

                pad->digital_buttons = *record_ptr->u32_ptr++;
            }
            break;
        default:
            break;
    }
}

/* Append to existing nupad.c; genuine NUAPI.pad_record layout is already asserted. */
void __cdecl NuPadRecordSave(char *filepath) {
    NUFILE file;
    u32 size;
    u8 *start;
    FRAME_PAD_RECORD_PC *record = &nuapi.pad_record;
    if (record->mode == NUPAD_RECORD && record->record.u8_ptr != record->buf_start) {
        if (filepath != NULL) {
            file = NuFileOpenDF(filepath, 1, curr_dat, 0);
            if (file != 0) {
                start = record->buf_start;
                size = (u32)(record->record.u8_ptr - start);
                NuFileWriteInt(file, (i32)size);
                NuFileWrite(file, start, (i32)size);
                NuFileWriteUnsignedInt(file, 0xe0fu);
                NuFileClose(file);
            }
        }
        record->should_end = 0;
        record->mode = NUPAD_NORM;
    }
}

void *__cdecl NuPtrBlockFix(void *block) {
    VARIPTR table;
    VARIPTR return_pointer;
    VARIPTR entry;
    i32 count;
    i32 index;

    return_pointer.void_ptr = block;
    return_pointer.addr += 4;
    table.void_ptr = block;
    table.addr = *(u32 *)table.void_ptr + table.addr;
    count = *(i32 *)table.void_ptr;
    table.addr += 4;
    for (index = 0; index < count; ++index) {
        entry.addr = *(u32 *)table.void_ptr + table.addr;
        if (*(u32 *)entry.void_ptr != 0)
            *(u32 *)entry.void_ptr += entry.addr;
        table.addr += 4;
    }
    return return_pointer.void_ptr;
}

i32 NuVideoGetMode(void) {
    return nuapi.video_mode;
}

i32 NuVideoGetAspect(void) {
    return nuapi.video_aspect;
}

void NuVideoSetMode(i32 mode) {
    nuapi.video_mode = mode;
    switch (mode) {
        case 3:
        case 4:
            nuapi.video_is_pal = 1;
            break;
        case 2:
        case 5:
        case 6:
            nuapi.video_is_pal = 0;
            break;
        default:
            break;
    }
}

void NuVideoSetResolution(i32 width, i32 height) {
    nuapi.screen_width = width;
    nuapi.screen_height = height;
}

i32 NuVideoSetSwapMode(i32 mode) {
    i32 previous_mode = nuapi.video_swap_mode;

    nuapi.video_swap_mode = mode;
    switch (mode) {
        case 1:
        case 2:
        case 5:
            if (nuapi.video_is_pal != 0) {
                NuVideoFrameRate = 50.0f;
            } else {
                NuVideoFrameRate = 60.0f;
            }
            break;
        case 3:
        case 4:
        case 6:
            if (nuapi.video_is_pal != 0) {
                NuVideoFrameRate = 25.0f;
            } else {
                NuVideoFrameRate = 30.0f;
            }
            break;
        default:
            break;
    }

    NuVideoSetSwapModePS(mode);
    return previous_mode;
}

i32 NuVideoGetSwapMode(void) {
    return nuapi.video_swap_mode;
}

void NuVideoRollingFrameRateReset(void) {
}

void NuVideoSetBrightness(f32 brightness) {
    nuapi.video_brightness = brightness;
    NuVideoSetBrightnessPS();
}

i32 NuLanguageGet(void) {
    return nuapi.language;
}

void NuLanguageSet(i32 language) {
    nuapi.language = language;
}

i32 NuRegionGet(void) {
    return nuapi.region;
}

void NuRegionSet(i32 region) {
    nuapi.region = region;
}

static i32 ishexnum(char value) {
    switch (value) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            return 1;
        default:
            return 0;
    }
}

static i32 hexcodetoint(char value) {
    switch (value) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return value - 0x30;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            return value - 0x57;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            return value - 0x37;
        default:
            return 0;
    }
}

i32 __cdecl NuRDPIVar(char *input, NURDPGETVARFN get_variable) {
    NURDPICTX_PC context;

    context.input = input;
    context.position = 0;
    context.get_variable = get_variable;
    NuRDPIGetToken(&context);
    return NuRDPIExpression(&context);
}

i32 __cdecl NuRDPI(char *input) {
    return NuRDPIVar(input, NULL);
}

void ImplodeError(char *msg, ...) {
    va_list args;
    va_start(args, msg);

    putc('\n', stderr);
    vfprintf(stderr, msg, args);
    putc('\n', stderr);

    NuFileError();
}

i32 __cdecl ExplodeBuffer(char *in_buf, char *out_buf) {
    i32 original_size;
    i32 compressed_size;
    if (gExplodeInitialised == 0)
        return 0;
    original_size = ExplodeBufferSize(in_buf);
    if (original_size != 0) {
        compressed_size = (i32)(((u32)(u8)in_buf[8] | ((u32)(u8)in_buf[9] << 8) | ((u32)(u8)in_buf[10] << 16) |
                                 ((u32)(u8)in_buf[11] << 24)) &
                                PCImplodeWordMask);
        return ExplodeBufferNoHeader(in_buf + 12, out_buf, compressed_size, original_size);
    }
    return 0;
}

unsigned char ImplodeGetByteFromMem() {
    return *implode_inbuffer++;
}

void ImplodeFillBuf(i32 count) {
    implode_bitbuf <<= count;

    while (ImplodeAvailableBitsPc < count) {
        implode_bitbuf |= ImplodeSubBitsPc << (count -= ImplodeAvailableBitsPc);

        if (implode_compsize != 0) {
            implode_compsize--;

            ImplodeSubBitsPc = ImplodeGetByteFromMem() & 0xff;
        } else {
            ImplodeSubBitsPc = 0;
        }

        ImplodeAvailableBitsPc = 8;
    }

    implode_bitbuf |= ImplodeSubBitsPc >> (ImplodeAvailableBitsPc -= count);
}

u32 ImplodeGetBits(i32 count) {
    u32 value;

    if (count == 0) {
        return 0;
    }

    value = implode_bitbuf >> (0x20u - count);

    ImplodeFillBuf(count);

    return value;
}

void ImplodeInitGetBits() {
    implode_bitbuf = 0;
    ImplodeSubBitsPc = 0;
    ImplodeAvailableBitsPc = 0;

    ImplodeFillBuf(0x20);
}

static void ImplodeDecodeRingPrivatePc(u32 size, unsigned char *buf) {
    static u32 ImplodeRingCursorPc = 0;

    u32 c;
    u32 n;

    n = 0;

    while (--ImplodePendingCopyPc >= 0) {
        buf[n] = buf[ImplodeRingCursorPc];
        ImplodeRingCursorPc = (ImplodeRingCursorPc + 1) & 0x1fff;

        if (++n == size) {
            return;
        }
    }

    while (1) {
        c = ImplodeDecodeC();

        if (c <= 255) {
            buf[n] = c;

            if (++n == size) {
                return;
            }
        } else {
            ImplodePendingCopyPc = c - 0xfd;
            ImplodeRingCursorPc = (n - ImplodeDecodeOffset() - 1) & 0x1fff;

            while (--ImplodePendingCopyPc >= 0) {
                buf[n] = buf[ImplodeRingCursorPc];
                ImplodeRingCursorPc = (ImplodeRingCursorPc + 1) & 0x1fff;

                if (++n == size) {
                    return;
                }
            }
        }
    }
}

i32 ExplodeBufferNoHeader(char *in_buf, char *out_buf, i32 compressed_size, i32 orig_size) {
    i32 total_read;
    u32 read_size;

    if (gExplodeInitialised == 0) {
        return 0;
    }

    implode_inbuffer = in_buf;
    implode_outbuffer = out_buf;
    implode_origsize = orig_size;
    implode_compsize = compressed_size;

    ImplodeHufDecodeStart();
    ImplodePendingCopyPc = 0;

    total_read = 0;
    while (implode_origsize != 0) {
        read_size = implode_origsize < 0x2000 ? implode_origsize : 0x2000;

        ImplodeDecodeRingPrivatePc(read_size, ImplodeRingBufferPc);

        memcpy(implode_outbuffer, ImplodeRingBufferPc, read_size);

        implode_outbuffer += read_size;
        implode_origsize -= read_size;
        total_read += read_size;
    }

    return total_read;
}

static void ImplodeReadOffsetLengthsPrivatePc(i32 table_size, i32 bit_count, i32 zero_value) {
    i32 n;
    i32 i;
    i32 value;
    u32 k;

    n = ImplodeGetBits(bit_count);
    if (n == 0) {
        value = ImplodeGetBits(bit_count);

        for (i = 0; i < table_size; i++) {
            ImplodeOffsetLengthsPc[i] = 0;
        }

        for (i = 0; i < 0x100; i++) {
            ImplodeOffsetTablePc[i] = value;
        }

        return;
    }

    i = 0;
    while (i < n) {
        value = implode_bitbuf >> 0x1d;

        if (value == 7) {
            for (k = 0x10000000; (implode_bitbuf & k) != 0; k >>= 1, value++) {
            }
        }

        ImplodeFillBuf(value >= 7 ? value - 3 : 3);

        ImplodeOffsetLengthsPc[i] = value;
        i++;

        if (i == zero_value) {
            value = ImplodeGetBits(2);

            while (--value >= 0) {
                ImplodeOffsetLengthsPc[i] = 0;
                i++;
            }
        }
    }

    for (; i < table_size; i++) {
        ImplodeOffsetLengthsPc[i] = 0;
    }

    ImplodeMakeTable(table_size, ImplodeOffsetLengthsPc, 8, ImplodeOffsetTablePc);
}

static void ImplodeReadLiteralLengthsPrivatePc() {
    i32 n;
    i32 i;
    i32 value;

    n = ImplodeGetBits(9);
    if (n == 0) {
        value = ImplodeGetBits(9);

        for (i = 0; i < sizeof(ImplodeLiteralLengthsPc); i++) {
            ImplodeLiteralLengthsPc[i] = 0;
        }

        for (i = 0; i < 0x1000; i++) {
            ImplodeLiteralTablePc[i] = value;
        }

        return;
    }

    i = 0;
    while (i < n) {
        SELECT(value, ImplodeOffsetTablePc, 0x18, 0x12, 0x800000, ImplodeOffsetLengthsPc);

        if (value <= 2) {
            if (value == 0) {
                value = 1;
            } else if (value == 1) {
                value = ImplodeGetBits(4) + 0x3;
            } else {
                value = ImplodeGetBits(9) + 0x14;
            }

            while (--value >= 0) {
                ImplodeLiteralLengthsPc[i] = 0;
                i++;
            }
        } else {
            ImplodeLiteralLengthsPc[i] = value - 2;
            i++;
        }
    }

    for (; i < sizeof(ImplodeLiteralLengthsPc); i++) {
        ImplodeLiteralLengthsPc[i] = 0;
    }

    ImplodeMakeTable(sizeof(ImplodeLiteralLengthsPc), ImplodeLiteralLengthsPc, 0xc, ImplodeLiteralTablePc);
}

i32 __cdecl NuDatGetFileInfo(NUDATHDR *header, char *name, i64 *position, i32 *length) {
    i32 index;
    i64 file_position;

    if (header == NULL) {
        return -1;
    }

    index = NuDatFileFindTree(header, name);
    if (index >= 0) {
        file_position = NuDatCalcPos(header, header->file_info[index].file_offset);
        if (nufile_lsn_allowed != 0 && header->unknown_b4 != 0) {
            if (position != NULL) {
                *position = header->unknown_b4 + (i32)(file_position / 0x800);
            }
            if (length != NULL) {
                *length = header->file_info[index].file_len;
            }
            return index;
        }
        if (position != NULL) {
            *position = file_position;
        }
        if (length != NULL) {
            *length = header->file_info[index].file_len;
        }
        return index;
    }
    return -1;
}

i32 __cdecl ExplodeBufferSize(char *header) {
    char magic[] = "LZ2K";
    i32 i;

    for (i = 0; i < 4; ++i) {
        if (*header++ != magic[i]) {
            return 0;
        }
    }
    return (i32)(((u32)(u8)header[0] | ((u32)(u8)header[1] << 8) | ((u32)(u8)header[2] << 16) |
                  ((u32)(u8)header[3] << 24)) &
                 PCImplodeWordMask);
}

i32 __cdecl ExplodeCompressedSize(char *header) {
    char magic[] = "LZ2K";
    i32 i;
    u32 size;

    for (i = 0; i < 4; ++i) {
        if (*header++ != magic[i]) {
            return 0;
        }
    }
    size = ((u32)(u8)header[4] | ((u32)(u8)header[5] << 8) | ((u32)(u8)header[6] << 16) | ((u32)(u8)header[7] << 24)) &
           PCImplodeWordMask;
    if (size != 0) {
        size += 12;
    }
    return (i32)size;
}

u32 ImplodeDecodeC() {
    u32 value;

    if (ImplodeBlockRemainingPc == 0) {
        ImplodeBlockRemainingPc = ImplodeGetBits(0x10);

        ImplodeReadOffsetLengthsPrivatePc(0x13, 5, 3);
        ImplodeReadLiteralLengthsPrivatePc();
        ImplodeReadOffsetLengthsPrivatePc(0xe, 4, -1);
    }

    ImplodeBlockRemainingPc--;

    SELECT(value, ImplodeLiteralTablePc, 0x14, sizeof(ImplodeLiteralLengthsPc) - 1, 0x80000, ImplodeLiteralLengthsPc);

    return value;
}

u32 ImplodeDecodeOffset() {
    u32 offset;

    SELECT(offset, ImplodeOffsetTablePc, 0x18, 0xd, 0x800000, ImplodeOffsetLengthsPc);

    if (offset != 0) {
        offset = (1u << (offset - 1)) + ImplodeGetBits(offset - 1);
    }

    return offset;
}

void ImplodeHufDecodeStart() {
    ImplodeInitGetBits();

    ImplodeBlockRemainingPc = 0;
}

void ImplodeMakeTable(i32 size, u8 *len, i32 unknown, u16 *table) {
    u16 symbol_count[17];
    u16 masks[17];
    u16 offsets[18];
    u32 symbol;
    i32 shift;
    u32 length;
    u32 i;
    u32 j;
    u32 k;
    u32 unknown2;
    u32 unknown3;
    u32 unknown4;
    u16 *cursor;

    for (i = 1; i < 0x11; i++) {
        symbol_count[i] = 0;
    }

    for (i = 0; i < size; i++) {
        symbol_count[len[i]]++;
    }

    offsets[1] = 0;

    for (i = 1; i < 0x11; i++) {
        offsets[i + 1] = offsets[i] + (symbol_count[i] << (0x10 - i));
    }

    if (offsets[0x11] != 0) {
        ImplodeError("Bad table");
    }

    shift = 0x10 - unknown;

    for (i = 1; i <= unknown; i++) {
        offsets[i] >>= shift;
        masks[i] = 1 << (unknown - i);
    }

    for (; i <= 16; i++) {
        masks[i] = 1 << (0x10 - i);
    }

    i = (i32)offsets[unknown + 1] >> shift;
    if (i != 0) {
        for (j = 1 << unknown; i != j; i++) {
            table[i] = 0;
        }
    }

    unknown2 = size;
    unknown3 = 1 << (0xf - unknown);

    for (symbol = 0; symbol < size; symbol++) {
        if ((length = len[symbol]) == 0) {
            continue;
        }

        unknown4 = offsets[length] + masks[length];
        if (unknown >= length) {
            for (i = offsets[length]; i < unknown4; i++) {
                table[i] = symbol;
            }
        } else {
            j = offsets[length];
            cursor = table + (j >> shift);

            for (k = length - unknown; k != 0; k--) {
                if (*cursor == 0) {
                    implode_left[unknown2] = 0;
                    implode_right[unknown2] = implode_left[unknown2];
                    *cursor = unknown2;

                    unknown2++;
                }

                if ((j & unknown3) != 0) {
                    cursor = implode_right + *cursor;
                } else {
                    cursor = implode_left + *cursor;
                }

                j <<= 1;
            }

            *cursor = symbol;
        }

        offsets[length] = unknown4;
    }
}
