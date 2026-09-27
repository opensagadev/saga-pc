#include "saga.h"

struct SoundQueue;
#pragma bss_seg(".bss$music_man")
NuMusic music_man;
#pragma bss_seg()
#pragma bss_seg(".bss$the_music_player")
NuMusic *the_music_player;
#pragma bss_seg()
#pragma bss_seg(".bss$NuSound3StereoStreams")
SoundQueue *NuSound3StereoStreams[2];
#pragma bss_seg()

// Startup and main-loop state; defaults follow the PC executable.
i32 ChallengeMode;
i32 fade_draw_suppressed;
i32 GAMEDEMO;
i32 PLAYERCOUNT;
i32 Level;
i32 Area = -1;
i32 last_area;
i32 NextArea_FreePlay;
i32 FreePlay;
i32 makefreeplaymodellist;
i32 SuperStory;
i32 reset_area;
i32 PlayTrailer = -1;
i32 loadareacharacters_loadedlevel;
i32 level_already_loaded = -1;
i32 next_level;
volatile i32 abort_load;
i32 dagobah_training;
i32 party_cant_be_under_cover;
i32 ResetOldFStop;
i32 new_level_from_menu;
i32 reset_restart;
#pragma bss_seg(".bss$NoMoreLoads")
i32 no_more_loads;
#pragma bss_seg()
i32 BonusWinner = -1;
i32 BonusCoinTotal;
i32 NewMode;
i32 reset_load = 1;
i32 newlevel_resumecutaudio = 1;
i32 frameout;
i32 other_level = -1;
volatile i32 waiting_for_level;
i32 gone_through_door_to_new_level;
i32 waiting_for_new_level;
i32 screendump;
i32 MiniCutCam;
i32 pause_rndr_on;
i32 clear_screen_onstill = 1;
i32 HubMainRenderTimeHack;
i32 peak_poly_count;
i32 poly_count;
i32 CUTSTOPGAME;
i32 Paused;
#pragma bss_seg(".bss$NetPaused")
i32 NetPaused;
#pragma bss_seg()
i32 FinishLoop_On = 1;
i32 LevelChange;
i32 LevelChangesInArea;
i32 do_multiframe_update;
#pragma bss_seg(".bss$MainFrameCounters")
MAINFRAMECOUNTERS MainFrameCounters;
#pragma bss_seg()
i32 Arcade;
i32 hub_from_arcade = -1;
i32 UsePlayerList;
i32 Door_UseCutCam;

extern "C" {
    i32 SUPERBUFFERSIZE = 0x09600000;
    i32 StartupGameFlag0;
    i32 StartupGameFlag1 = 1;
    NUDATHDR *StartupDatList[7];
    i32 StartupDatCount;
    NUDATHDR *NuSoundDat;
    GAMEANIMPROGRESS gameanimsysprogress;
    VARIPTR original_permbuffer_base;
    char InitOnceCurrentDirectory[1];
#pragma const_seg(".rdata$InitOnceAllocSource")
    extern const char InitOnceAllocSource[15] = ".\\pcbatman.cpp";
#pragma const_seg()
#pragma const_seg(".rdata$InitOnceWindowTitle")
    extern const char InitOnceWindowTitle[36] = "LEGO\xae Star Wars\x99: The Complete Saga";
#pragma const_seg()
#pragma const_seg(".rdata$InitOnceHalf")
    extern const f32 InitOnceHalf = 0.5f;
#pragma const_seg()
    void *Game_NuPad_Store[2];
    void **Game_NuPad;
    void *NuTimeBar_SetList[16];
    i32 NuTimeBar_Initialised;

    // PC debris timing uses a single-precision 1/60-second step.
    f32 debris_time_increment = 1.0f / 60.0f;
    void *debris_cutoff_camera_vec;
    i32 debris_render_group;
    f32 FRAMETIME;
    f32 QFONTSCALEX = 1.0f;
    f32 STCOORDSCALE = 1.0f;
    VUFNT *QFont2D;
    VUFNT *QFont2DButtons;
    VUFNT *system_qfont;
    NUTEXDIM_PC **NuTextureDimensions;
    i32 NuPrimVertexCount;
    i32 *NuPrimVertexCountPointer;
    VARIPTR *NuPrimStreamBufferPointer;
    i32 NuPrimCurrentPrimitiveType;
    i32 NuPrimCSPos;
    f32 NuPrimXBias;
    f32 NuPrimYBias;
    f32 NuPrimXScale = 1.0f;
    f32 NuPrimYScale = 1.0f;
#pragma const_seg(".rdata$NuPrimPS2XScale")
    extern const f32 NuPrimPS2XScale = 0.003125f;
#pragma const_seg(".rdata$NuPrimPS2YScale")
    extern const f32 NuPrimPS2YScale = -1.0f / 112.0f;
#pragma const_seg(".rdata$NuPrimTwo")
    extern const f32 NuPrimTwo = 2.0f;
#pragma const_seg(".rdata$NuPrimMinusTwo")
    extern const f32 NuPrimMinusTwo = -2.0f;
#pragma const_seg()
    NUFPAR_PC NuFParRecords[4];
    char NuFParBuffers[4][0x1000];
    NUFPAR_PC *NuFParRecordBase;
    char *NuFParBufferBase;
    i32 file_criticalsection;
    PC_NUDATFILEINFO *unpack_file_info;
    NUDATOPENFILEINFO *unpack_file_odi;
    i64 unpack_file_pos;
    i32 decode_buffer_left;
    f32 nuqfnt_space_width;
    f32 qfnt_len_scale = 1.0f;
    f32 qfnt_height_scale = 1.0f;
    f32 qfnt_justify_squash = 1.0f;
    f32 qfnt_justify_stretch = 1.0f;
    u32 NuQFntMode;
    i32 NuQFntModeStackIndex;
#pragma bss_seg(".bss$NuQFntModeStack")
    u32 NuQFntModeStack[16];
#pragma bss_seg()
    i32 nuqfnt_cs_mode;
    f32 qfnt_offscale_x;
    f32 qfnt_offscale_y;
    f32 qfnt_rezscale_w = 1.0f;
    f32 qfnt_rezscale_h = 1.0f;
    extern const f32 nu_fsign_minus_one = -1.0f;
#pragma const_seg(".rdata$PCKeyboard2Name_candidate")
    extern const char PCKeyboard2Name_candidate[10] = "Keyboard2";
#pragma const_seg(".rdata$PCKeyboardMouse2Name_candidate")
    extern const char PCKeyboardMouse2Name_candidate[16] = "KeyboardMouse2";
#pragma const_seg(".rdata$PCKeyboard1Name_candidate")
    extern const char PCKeyboard1Name_candidate[10] = "Keyboard1";
#pragma const_seg(".rdata$PCKeyboardMouse1Name_candidate")
    extern const char PCKeyboardMouse1Name_candidate[15] = "KeyboardMouse1";
#pragma const_seg()
#pragma const_seg(".rdata$NuQFntHalfScale")
    extern const f64 NuQFntHalfScale = 0.5;
#pragma const_seg()
    GAMESAVE Game;
    OPTIONSSAVE TempOptions;
    PLAYERPROGRESS PlayerProgress[8];
    u32 InitOnceOptionInput0;
    u32 InitOnceOptionInput1;
    u32 InitOnceOptionInput2;
    i32 MenuLoadOccurred;
    i32 startnewgame_initiated;
    i32 startnewgame;
    i32 saveload_autosave = -1;
    i32 BGLOAD = 1;
    i32 PermDataLoaded = 1;
    i32 LOADEROFF;
    volatile i32 LoadPerm_LanguageSelect;
    NUMTL *legal_mtl;
    f32 DEFAULTFRAMETIME;
    i32 PAL;
    PART_EMIT_PC part_emits[40];
    i32 part_page_on[8];
    GAMEPAD_INIT_RECORD GamePadStorage[64];
    i32 bgproc_initialized;
    CRITICAL_SECTION bgproc_critical_section;
    HANDLE bgproc_work_event;
    HANDLE bgproc_resume_event;
    BGPROCINFO *bgproc_active;
    DWORD bgproc_thread_id;
    i32 bgproc_frozen;
    i32 bgproc_freeze;
    i32 ShowingError;
    i32 bHaveErr;
    HANDLE pc_file_handles[32] = {
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
    };
    FILEINFO file_info[33];
    FILEBUFF file_buff[4];
    i32 nufile_buffering_enabled;
    i32 datsys_offset;
    i32 file_time_count;
    NUFILE_DEVICE devices[16];
    i32 numdevices;
    NUFILE_DEVICE host_device = {
        0,
        0,
        0x0e,
        0,
        '\\',
        0x400,
        -1,
        -1,
        -1,
        {0},
        {0},
        4,
        6,
        {0},
        "Host pc filesystem",
        {0},
        {0},
        {0},
        DEV_FormatName,
        DEVHOST_Interrogate,
    };
    NUFILE_DEVICE *default_device = &host_device;
    i32 DontDoFileUpperCaseHack;
    NuKeyboardState_PC *nu_input_state;
    HWND PCInputWindow_candidate;
    HINSTANCE PCInputInstance_candidate;
    NuKeyboardOwner_PC nu_keyboard_owner;
    u8 nu_keyboard_pages[0x200];
    u8 nu_keyboard_page_index;
    i32 nu_keyboard_device_flags;
    i32 nu_keyboard_key_index;
    i32 nu_keyboard_key_count;
    i32 nu_keyboard_repeat_countdown;
    i32 nu_keyboard_previous_key_index;
    i32 nu_keyboard_no_key_state;
    i32 PCThreadCriticalSectionCount_candidate;
    CRITICAL_SECTION PCThreadCriticalSections_candidate[13];
    i32 PCFrameCriticalSectionIndex_candidate;
    NUALLOC_RECORD *NuAllocRecordHead;
    u32 NuAllocTrackedBytes;
    NUALLOC_POOL *NuAllocPool;
    u32 NuAllocRecordSerial;
    NUALLOC_ARENA *NuAllocArena;
    void *NuAllocManager;
    void *NuAllocPeakEnd0;
    void *NuAllocPeakEnd1;
    u32 NuAllocHeaderMetadata[3] = {0x424d454d, 0x45484b4c, 0x52454441};
    u32 NuAllocTailMetadata[3] = {0x424d454d, 0x41544b4c, 0x31304c49};
#pragma const_seg(".rdata$NuFileDiscSuffix")
    extern const char nufile_disc_suffix[3] = ";1";
#pragma const_seg()
    DAT_HANDLES_PC NuDatHandles;
    i32 finishloop_backdroponly;
#pragma const_seg(".rdata$StartupGameDat")
    extern const char StartupGameDat[9] = "GAME.DAT";
#pragma const_seg(".rdata$StartupEpisodeIDat")
    extern const char StartupEpisodeIDat[14] = "Episode_I.DAT";
#pragma const_seg(".rdata$StartupEpisodeIIDat")
    extern const char StartupEpisodeIIDat[15] = "Episode_II.DAT";
#pragma const_seg(".rdata$StartupEpisodeIIIDat")
    extern const char StartupEpisodeIIIDat[16] = "Episode_III.DAT";
#pragma const_seg(".rdata$StartupEpisodeIVDat")
    extern const char StartupEpisodeIVDat[15] = "Episode_IV.DAT";
#pragma const_seg(".rdata$StartupEpisodeVDat")
    extern const char StartupEpisodeVDat[14] = "Episode_V.DAT";
#pragma const_seg(".rdata$StartupEpisodeVIDat")
    extern const char StartupEpisodeVIDat[15] = "Episode_VI.DAT";
#pragma const_seg()
#pragma data_seg(".data$StartupDatNames")
    const char *StartupDatNames[7] = {
        StartupGameDat,      StartupEpisodeIDat, StartupEpisodeIIDat, StartupEpisodeIIIDat,
        StartupEpisodeIVDat, StartupEpisodeVDat, StartupEpisodeVIDat,
    };
#pragma data_seg()
#pragma const_seg(".rdata$EpisodeIDat")
    extern const char EpisodeIDat[14] = "Episode_I.dat";
#pragma const_seg(".rdata$EpisodeIIDat")
    extern const char EpisodeIIDat[15] = "Episode_II.dat";
#pragma const_seg(".rdata$EpisodeIIIDat")
    extern const char EpisodeIIIDat[16] = "Episode_III.dat";
#pragma const_seg(".rdata$EpisodeIVDat")
    extern const char EpisodeIVDat[15] = "Episode_IV.dat";
#pragma const_seg(".rdata$EpisodeVDat")
    extern const char EpisodeVDat[14] = "Episode_V.dat";
#pragma const_seg(".rdata$EpisodeVIDat")
    extern const char EpisodeVIDat[15] = "Episode_VI.dat";
#pragma const_seg()
#pragma const_seg(".rdata$bgproc_resume_event_name")
    extern const char bgproc_resume_event_name[9] = "BGFreeze";
#pragma const_seg(".rdata$bgproc_work_event_name")
    extern const char bgproc_work_event_name[11] = "BGWorkToDo";
#pragma const_seg()
    VUFNT *QFont3DZ;
    VUFNT *SmartTextFont;
    f32 INTROTEXT_Y = 0.175f;
    f32 INTROTEXT_SCALE = 0.79f;
    void (*Text_GameSetLanguageFn)(i32);
#pragma const_seg(".rdata$Text_TimeHMSC")
    extern const char Text_TimeHMSC[18] = "%i:%.2i:%.2i.%.2i";
#pragma const_seg(".rdata$Text_TimeHMS")
    extern const char Text_TimeHMS[13] = "%i:%.2i:%.2i";
#pragma const_seg(".rdata$Text_TimeMSC")
    extern const char Text_TimeMSC[13] = "%i:%.2i.%.2i";
#pragma const_seg(".rdata$Text_TimeMS")
    extern const char Text_TimeMS[8] = "%i:%.2i";
#pragma const_seg(".rdata$Text_TimeSC")
    extern const char Text_TimeSC[8] = "%i.%.2i";
#pragma const_seg(".rdata$Text_TimeS")
    extern const char Text_TimeS[3] = "%i";
#pragma const_seg()
    f32 nuspecial_const_alpha = 1.0f;
    NUVEC nuspecial_const_tint = {1.0f, 1.0f, 1.0f};
    NUVEC cam_axes = {1.0f, 1.0f, 1.0f};
    NUCAMERA global_camera;
    // clang-format off: LLVM 18 and newer format initializer macros differently.
#define IDENTITY_NUMTX {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
    // clang-format on
    NUMTX vmtx = IDENTITY_NUMTX;
    NUMTX pmtx = IDENTITY_NUMTX;
    NUMTX cmtx = IDENTITY_NUMTX;
    NUMTX smtx = IDENTITY_NUMTX;
    NUMTX vpmtx = IDENTITY_NUMTX;
    NUMTX vpc_sci_mtx = IDENTITY_NUMTX;
    NUMTX vpc_vport_mtx = IDENTITY_NUMTX;
    NUMTX pc_vport_mtx = IDENTITY_NUMTX;
    NUMTX vpsmtx = IDENTITY_NUMTX;
    NUMTX clip_planes = IDENTITY_NUMTX;
#undef IDENTITY_NUMTX
    i32 nuspecial_const_alpha_enabled;
    i32 nuspecial_const_tint_enabled;
    i32 nuspecial_draw_state;
#pragma bss_seg(".bss$NuTrigTable")
    f32 NuTrigTable[32768];
#pragma bss_seg()

// PC NuSound3 state and loop-hold conversion constants.
#pragma bss_seg(".bss$NuSound3Reverb")
    u8 NuSound3Reverb;
#pragma bss_seg()
    extern const f64 NuScale1000 = 1000.0;
    i32 NuSound3LoopHoldTime = 133;
    i32 NuSound3VolumeOffset = -300;
#pragma bss_seg(".bss$NuSound3VolumeTable")
    f32 NuSound3VolumeTable[0x4000];
#pragma bss_seg()
    PC_NUDATFILEINFO pc_dat_file_infos[20];
    PC_NUMEMFILE pc_memfiles[20];
    i32 old_line_pos;
#pragma bss_seg(".bss$ADataList")
    AREA_DATA_PC *ADataList;
#pragma bss_seg()
    AREA_DATA_PC *LastAData;
    AREA_DATA_PC *HUB_ADATA;
    i32 PlayerID[2] = {-1, -1};
    i16 PlayerList[8];
    APICHARACTERMODELLIST Hub_ModelList[6];
    LEVELDATA_PC *LDataList;
    i32 openlevels;
    u8 snakebodies[0x440];
#pragma data_seg(".data$gizaimessage_prefix")
    char gizaimessage_prefix[5] = "msg_";
#pragma data_seg()
#pragma const_seg(".rdata$GizAIMessage_NameFormat")
    extern const char GizAIMessage_NameFormat[3] = "%s";
#pragma const_seg(".rdata$GizAIMessage_PrefixedNameFormat")
    extern const char GizAIMessage_PrefixedNameFormat[5] = "%s%s";
#pragma const_seg()
    GIZAIMESSAGESYS *gizaimessagesys;
    f32 PanelProjectionX = 1222.0f;
    f32 PanelProjectionY = 1145.0f;
    f32 PanelWideWidth = 740.0f;
    f32 PanelWideHeight = 387.5f;
#pragma bss_seg(".bss$Panel3DMulX")
    f32 PANEL3DMULX;
#pragma bss_seg()
#pragma bss_seg(".bss$Panel3DMulY")
    f32 PANEL3DMULY;
#pragma bss_seg()
    LANGUAGEDATA Game_LanguageList[7] = {{1, 0}, {2, 0}, {4, 0}, {5, 0}, {3, 0}, {8, 0}, {-1, 0}};
    LANGUAGEDATA Text_LanguageList_Default[7] = {{1, 0}, {2, 0}, {4, 0}, {5, 0}, {3, 0}, {8, 0}, {-1, 0}};
    LANGUAGEDATA *Text_LanguageList = Text_LanguageList_Default;
    i32 LANGUAGECOUNT = 6;
    i32 Text_Language = 1;
    i32 INTROTEXT_ID = -1;
    void *Text_AppFont;
    char **TTab_Original;
    char **TTab;
    i32 Text_MaxStrings_Overall;
    u32 *Text_StringBits;
#pragma const_seg(".rdata$Text_ErrorText")
    extern const char Text_ErrorText[7] = "Err...";
#pragma const_seg()
#pragma const_seg(".rdata$Text_UnknownText")
    extern const char Text_UnknownText[2] = "?";
#pragma const_seg()
#pragma const_seg(".rdata$Text_WibbleCodeword")
    extern const char Text_WibbleCodeword[7] = "wibble";
#pragma const_seg()
#pragma const_seg(".rdata$Text_CrossCodeword")
    extern const char Text_CrossCodeword[6] = "cross";
#pragma const_seg()
    void (*buttonmapfn)(char *, char *);
    i32 smarttext_fwn;
#pragma const_seg(".rdata$Text_PathEnglishText")
    extern const char Text_PathEnglishText[8] = "english";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathFrenchText")
    extern const char Text_PathFrenchText[7] = "french";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathGermanText")
    extern const char Text_PathGermanText[7] = "german";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathItalianText")
    extern const char Text_PathItalianText[8] = "italian";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathSpanishText")
    extern const char Text_PathSpanishText[8] = "spanish";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathDanishText")
    extern const char Text_PathDanishText[7] = "danish";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathJapaneseText")
    extern const char Text_PathJapaneseText[9] = "japanese";
#pragma const_seg()
#pragma const_seg(".rdata$Text_EnglishName")
    extern const char Text_EnglishName[8] = "English";
#pragma const_seg()
#pragma const_seg(".rdata$Text_FrenchName")
    extern const char Text_FrenchName[10] = "Fran\xc3\xa7"
                                            "ais";
#pragma const_seg()
#pragma const_seg(".rdata$Text_GermanName")
    extern const char Text_GermanName[8] = "Deutsch";
#pragma const_seg()
#pragma const_seg(".rdata$Text_ItalianName")
    extern const char Text_ItalianName[9] = "Italiano";
#pragma const_seg()
#pragma const_seg(".rdata$Text_SpanishName")
    extern const char Text_SpanishName[9] = "Espa\xc3\xb1"
                                            "ol";
#pragma const_seg()
#pragma const_seg(".rdata$Text_DanishName")
    extern const char Text_DanishName[6] = "Dansk";
#pragma const_seg()
    char *Text_ErrorString = const_cast<char *>(Text_ErrorText);
    char *txt_NULL = const_cast<char *>(Text_UnknownText);
    char *txt_UNKNOWN = const_cast<char *>(Text_UnknownText);
    char *Text_PathEnglish = const_cast<char *>(Text_PathEnglishText);
    char *Text_PathFrench = const_cast<char *>(Text_PathFrenchText);
    char *Text_PathGerman = const_cast<char *>(Text_PathGermanText);
    char *Text_PathItalian = const_cast<char *>(Text_PathItalianText);
    char *Text_PathSpanish = const_cast<char *>(Text_PathSpanishText);
    char *Text_PathDanish = const_cast<char *>(Text_PathDanishText);
    char *Text_PathJapanese = const_cast<char *>(Text_PathJapaneseText);
    char *apitxt_ENGLISH = const_cast<char *>(Text_EnglishName);
    char *apitxt_FRENCH = const_cast<char *>(Text_FrenchName);
    char *apitxt_GERMAN = const_cast<char *>(Text_GermanName);
    char *apitxt_ITALIAN = const_cast<char *>(Text_ItalianName);
    char *apitxt_SPANISH = const_cast<char *>(Text_SpanishName);
    char *apitxt_DANISH = const_cast<char *>(Text_DanishName);
#pragma bss_seg(".bss$tNULL")
    i16 tNULL;
#pragma bss_seg()
#pragma bss_seg(".bss$tUNKNOWN")
    i16 tUNKNOWN;
#pragma bss_seg()
#pragma bss_seg(".bss$tENGLISH")
    i16 tENGLISH;
#pragma bss_seg()
#pragma bss_seg(".bss$tFRENCH")
    i16 tFRENCH;
#pragma bss_seg()
#pragma bss_seg(".bss$tGERMAN")
    i16 tGERMAN;
#pragma bss_seg()
#pragma bss_seg(".bss$tITALIAN")
    i16 tITALIAN;
#pragma bss_seg()
#pragma bss_seg(".bss$tSPANISH")
    i16 tSPANISH;
#pragma bss_seg()
#pragma bss_seg(".bss$tDANISH")
    i16 tDANISH;
#pragma bss_seg()

// NuWaterReset writes these standalone four-byte PC globals.
#pragma bss_seg(".bss$NuWaterResetState")
    i32 nu_water_reset_state;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWaterResetStatus")
    i32 nu_water_reset_status;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTimeTickBase")
    i64 NuTime_TickBase;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTimePerformanceFrequency")
    i64 NuTime_PerformanceFrequency;
#pragma bss_seg()
#pragma const_seg(".rdata$NuTimeLowScale")
    extern const f64 NuTime_LowScale = 4294967295.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeMicroScale")
    extern const f64 NuTime_MicroScale = 1000000.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeScanlineScale")
    extern const f64 NuTime_ScanlineScale = 272.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeRefreshRate")
    extern const f64 NuTime_RefreshRate = 60.0;
#pragma const_seg()
    void (*nuapi_endframe_callbackfn)(void);
    u8 fstackmem[NU_PORTAL_STACK_SIZE];
    VARIPTR fstack;
    VARIPTR fstack_top;
    VARIPTR fstack_limit;
    i32 DebrisCutSceneMode;
    i32 part_raycasts_enabled;
    f32 NuVideoFrameRate;
    OCCLUSIONMANAGER g_OcclusionManager = {0, 1, {0}, {{0}}, 0, 0, 0.3f, -1.0f, 0, 0, {0}};

    // STUBBED: reserved PC menu callbacks remain unavailable.
    MENUFNINFO MenuInfo[100] = {
        {1000, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1001, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1002, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1004, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1003, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1005, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1006, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1007, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1008, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1009, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1010, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 0, 0}, {1011, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1012, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1013, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1014, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1015, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1016, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1017, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1018, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1019, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 0, 0},
        {1020, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1021, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1022, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1023, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1024, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1025, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1026, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1027, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1028, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1029, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1030, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
    };
    MENU GameMenu[10];
    i32 MenuSFX = -1;
    i32 GameMenuLevel = -1;
    i32 MenuLanguages = 1;
    i32 MenusUsed = 31;
    // STUBBED: PC defaults are draw callbacks at 0x5100f0 and 0x5103e0.
    void (*drawslotinfofn)(f32, f32, i32, i32);
    void (*drawslotsfn)(MENU *, f32);
    char MenuHeader[64];
    i32 header_r;
    i32 header_g;
    i32 header_b;
    i32 MenuFadeEnabled;
    i32 MenuDrawDropShadows;
    NUMTL *MenuFadeMtl;
#pragma bss_seg(".bss$MENUHEADERR")
    u8 MENUHEADERR;
#pragma bss_seg()
#pragma data_seg(".data$MENUHEADERG")
    u8 MENUHEADERG = 0x7f;
#pragma data_seg(".data$MENUHEADERB")
    u8 MENUHEADERB = 0xff;
#pragma data_seg()
#pragma const_seg(".rdata$Menu_UsedFormat")
    extern const char Menu_UsedFormat[15] = "Menus used: %d";
#pragma const_seg()
#pragma const_seg(".rdata$Menu_OptionsVolumeFormat")
    extern const char Menu_OptionsVolumeFormat[10] = "%s: %i/%i";
#pragma const_seg(".rdata$Menu_ColonSeparator")
    extern const char Menu_ColonSeparator[3] = ": ";
#pragma const_seg()
#pragma const_seg(".rdata$nu_wind_phase_scale")
    extern const f64 nu_wind_phase_scale = 1.0 / 256.0;
#pragma const_seg(".rdata$nu_wind_scaled_time_multiplier")
    extern const f64 nu_wind_scaled_time_multiplier = 5.0;
#pragma const_seg()
#pragma const_seg(".rdata$PCKeyboardDefaultValue_candidate")
    extern const f32 PCKeyboardDefaultValue_candidate = 0.1f;
#pragma const_seg()
    f32 dme_sy = 1.0f;
    f32 MASTERVOLUME = 1.0f;
    f32 default_targetmomf = 4.0f;
    f32 default_ownermomf = 5.0f;
    f32 default_max_cable_length = 15.0f;
    f32 default_tow_length = 6.0f;
    f32 targetmomf = 4.0f;
    f32 ownermomf = 5.0f;
    f32 max_cable_length = 15.0f;
    f32 tow_length = 6.0f;
    AREA_DATA_PC *GUNSHIP_ADATA;
#pragma bss_seg(".bss$cables")
    u8 cables[0xf60];
#pragma bss_seg()
    i32 menu_flash;
#pragma bss_seg(".bss$tAUDIOVOLUME")
    i16 tAUDIOVOLUME;
#pragma bss_seg(".bss$tMUSIC")
    i16 tMUSIC;
#pragma bss_seg(".bss$tON")
    i16 tON;
#pragma bss_seg(".bss$tOFF")
    i16 tOFF;
#pragma bss_seg(".bss$tACCEPT")
    i16 tACCEPT;
#pragma bss_seg(".bss$tBACK")
    i16 tBACK;
#pragma bss_seg()
    OPTIONSSAVE *Game_OptionsSave;
    f32 opts_sfx_wait = 1.5f;
    i32 opts_sfx_i;
    LEVELDATA_PC *TITLES_LDATA;

    // STUBBED: PC callback bodies are not reconstructed; NULL slots keep them inactive.
    // IDs, remembered cursors, and prompt flags use the 23-record PC table.
    MENUFNINFO GameMenuInfo[23] = {
        {0, MenuEnterTitles, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1, MenuEnterNewGame, NULL, NULL, NULL, -1, -1, 0, 1, 0, 0},
        {2, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {3, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {4, MenuEnterOptions, MenuDrawOptions, MenuUpdateOptions, MenuExitOptions, 0, 0, 0, 1, 1, 0},
        {5, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0},
        {6, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {7, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0},
        {8, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {9, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {10, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {11, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {12, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {13, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {14, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {15, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {16, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {17, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {18, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {19, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {20, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {21, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {22, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0},
    };

    i32 DebrisGlassInitialized = 0;
    void *DebrisGlassCopyMaterial = NULL;
    f32 DebrisGlassU1 = 0.0f;
#pragma bss_seg(".bss$DebrisGlassV1")
    f32 DebrisGlassV1;
#pragma bss_seg()
#pragma bss_seg(".bss$DebrisGlassTextureWidth")
    i32 DebrisGlassTextureWidth;
#pragma bss_seg()
#pragma bss_seg(".bss$DebrisGlassTextureHeight")
    i32 DebrisGlassTextureHeight;
#pragma bss_seg()

    extern const f32 DebrisGlassMaterialOpacity = 0.999f;
}
void *pNuCam;
i32 app_tbgameset;
i32 app_tbplayerset;
i32 app_tbaiset;
i32 app_tbdrawset;
NUAPI nuapi;

i32 GAMERAND = 0x1f3ad27f;
u32 NuWindQS = 0x1365;
u32 partseed = 0x5c0999;
u32 debrisseed = 0x5c0999;

i32 argc;
char **argv;
NUDATHDR *curr_dat;
MSG win_message;

i32 permbuffer_size;
i32 CHARACTERBUFFERSIZE = 0x02800000;
i32 EDITBUFFERENDSIZE;
VARIPTR permbuffer_base;
VARIPTR permbuffer_ptr;
VARIPTR permbuffer_end;
VARIPTR characterbuffer_base;
#pragma bss_seg(".bss$CharacterBufferPtr")
VARIPTR characterbuffer_ptr;
#pragma bss_seg()
VARIPTR characterbuffer_end;
VARIPTR superbuffer_base;
VARIPTR superbuffer_ptr;
VARIPTR superbuffer_end;
VARIPTR editbuffer_end;

WORLDINFO WorldInfo[2] = {0};
#pragma data_seg(".data$WORLD")
WORLDINFO *WORLD = &WorldInfo[0];
#pragma data_seg()
#pragma data_seg(".data$LWorld")
WORLDINFO *LWORLD = &WorldInfo[0];
#pragma data_seg()

#pragma data_seg(".data$WaitingForCharacter")
i32 waiting_for_character = -1;
#pragma data_seg()

// CRC-16/IBM lookup values stored as 32-bit words.
u32 crctab[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1,
    0xc481, 0x0440, 0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40, 0x0a00, 0xcac1, 0xcb81, 0x0b40,
    0xc901, 0x09c0, 0x0880, 0xc841, 0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40, 0x1e00, 0xdec1,
    0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41, 0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040, 0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1,
    0xf281, 0x3240, 0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441, 0x3c00, 0xfcc1, 0xfd81, 0x3d40,
    0xff01, 0x3fc0, 0x3e80, 0xfe41, 0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840, 0x2800, 0xe8c1,
    0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41, 0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640, 0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0,
    0x2080, 0xe041, 0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240, 0x6600, 0xa6c1, 0xa781, 0x6740,
    0xa501, 0x65c0, 0x6480, 0xa441, 0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 0xaa01, 0x6ac0,
    0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840, 0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40, 0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1,
    0xb681, 0x7640, 0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041, 0x5000, 0x90c1, 0x9181, 0x5140,
    0x9301, 0x53c0, 0x5280, 0x9241, 0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440, 0x9c01, 0x5cc0,
    0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40, 0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40, 0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0,
    0x4c80, 0x8c41, 0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 0x8201, 0x42c0, 0x4380, 0x8341,
    0x4100, 0x81c1, 0x8081, 0x4040,
};

// PC walks 8-byte entries to a zero key.
NUKEYCHAR NuKeyChar[65] = {
    {0x2, 0x31, 0x21, {0, 0}},  {0x3, 0x32, 0x22, {0, 0}},  {0x4, 0x33, 0xa3, {0, 0}},  {0x5, 0x34, 0x24, {0, 0}},
    {0x6, 0x35, 0x25, {0, 0}},  {0x7, 0x36, 0x5e, {0, 0}},  {0x8, 0x37, 0x26, {0, 0}},  {0x9, 0x38, 0x2a, {0, 0}},
    {0xa, 0x39, 0x28, {0, 0}},  {0xb, 0x30, 0x29, {0, 0}},  {0xc, 0x2d, 0x5f, {0, 0}},  {0xd, 0x3d, 0x2b, {0, 0}},
    {0xf, 0x09, 0x09, {0, 0}},  {0x10, 0x71, 0x51, {0, 0}}, {0x11, 0x77, 0x57, {0, 0}}, {0x12, 0x65, 0x45, {0, 0}},
    {0x13, 0x72, 0x52, {0, 0}}, {0x14, 0x74, 0x54, {0, 0}}, {0x15, 0x79, 0x59, {0, 0}}, {0x16, 0x75, 0x55, {0, 0}},
    {0x17, 0x69, 0x49, {0, 0}}, {0x18, 0x6f, 0x4f, {0, 0}}, {0x19, 0x70, 0x50, {0, 0}}, {0x1a, 0x5b, 0x7b, {0, 0}},
    {0x1b, 0x5d, 0x7d, {0, 0}}, {0x1e, 0x61, 0x41, {0, 0}}, {0x1f, 0x73, 0x53, {0, 0}}, {0x20, 0x64, 0x44, {0, 0}},
    {0x21, 0x66, 0x46, {0, 0}}, {0x22, 0x67, 0x47, {0, 0}}, {0x23, 0x68, 0x48, {0, 0}}, {0x24, 0x6a, 0x4a, {0, 0}},
    {0x25, 0x6b, 0x4b, {0, 0}}, {0x26, 0x6c, 0x4c, {0, 0}}, {0x27, 0x3b, 0x3a, {0, 0}}, {0x28, 0x27, 0x40, {0, 0}},
    {0x2b, 0x5c, 0x3f, {0, 0}}, {0x2c, 0x7a, 0x5a, {0, 0}}, {0x2d, 0x78, 0x58, {0, 0}}, {0x2e, 0x63, 0x43, {0, 0}},
    {0x2f, 0x76, 0x56, {0, 0}}, {0x30, 0x62, 0x42, {0, 0}}, {0x31, 0x6e, 0x4e, {0, 0}}, {0x32, 0x6d, 0x4d, {0, 0}},
    {0x33, 0x2c, 0x3c, {0, 0}}, {0x34, 0x2e, 0x3e, {0, 0}}, {0x35, 0x2f, 0x00, {0, 0}}, {0x37, 0x2a, 0x2a, {0, 0}},
    {0x39, 0x20, 0x20, {0, 0}}, {0x47, 0x37, 0x37, {0, 0}}, {0x48, 0x38, 0x38, {0, 0}}, {0x49, 0x39, 0x39, {0, 0}},
    {0x4a, 0x2d, 0x2d, {0, 0}}, {0x4b, 0x34, 0x34, {0, 0}}, {0x4c, 0x35, 0x35, {0, 0}}, {0x4d, 0x36, 0x36, {0, 0}},
    {0x4e, 0x2b, 0x2b, {0, 0}}, {0x4f, 0x31, 0x31, {0, 0}}, {0x50, 0x32, 0x32, {0, 0}}, {0x51, 0x33, 0x33, {0, 0}},
    {0x52, 0x30, 0x30, {0, 0}}, {0x53, 0x2e, 0x2e, {0, 0}}, {0x8d, 0x3d, 0x3d, {0, 0}}, {0xb5, 0x2f, 0x2f, {0, 0}},
    {0x0, 0x00, 0x00, {0, 0}},
};

// PC reads pairs of 16-bit code points, ending with a zero pair.
u16 bad_equiv[8][2] = {{'a', '@'}, {'a', '4'}, {'e', '3'}, {'i', '1'}, {'l', '1'}, {'o', '0'}, {'t', '7'}, {0, 0}};

i32 SkinFlipTab[8] = {0, 1, 2, 3, 0, 2, 1, 3};

// Both PC indices are masked to two bits.
i32 TrooperStepFrames[4] = {1, 2, 1, 0};
f32 trooperspeedtab[4] = {0.3f, 0.4f, 0.5f, 0.6f};

// Four explicit PC scalar accesses establish each array's extent.
f32 vehicle_timebase_dist[4] = {50.0f, 25.0f, 0.0f, 0.0f};
i32 vehicle_timebase_nframes[4] = {4, 2, 0, 0};

// PC checks each byte index is less than 16.
const u8 KeyStructSizes[16] = {3, 4, 4, 3, 4, 3, 4, 8, 4, 8, 4, 0, 0, 0, 0, 0};

// Each of the 12 cube edges has two vertex indices and two incident faces.
i32 cubeEdgeIndices[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
                              {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};
i32 cubeEdgeTopologies[12][2] = {{3, 0}, {4, 0}, {1, 0}, {5, 0}, {2, 3}, {2, 4},
                                 {2, 1}, {2, 5}, {5, 3}, {3, 4}, {4, 1}, {1, 5}};

// 513 angular samples including both endpoints.
u16 ang[513] = {
    0x0000, 0x0014, 0x0029, 0x003d, 0x0051, 0x0066, 0x007a, 0x008f, 0x00a3, 0x00b7, 0x00cc, 0x00e0, 0x00f4, 0x0109,
    0x011d, 0x0131, 0x0146, 0x015a, 0x016f, 0x0183, 0x0197, 0x01ac, 0x01c0, 0x01d4, 0x01e9, 0x01fd, 0x0211, 0x0226,
    0x023a, 0x024e, 0x0262, 0x0277, 0x028b, 0x029f, 0x02b4, 0x02c8, 0x02dc, 0x02f0, 0x0305, 0x0319, 0x032d, 0x0341,
    0x0356, 0x036a, 0x037e, 0x0392, 0x03a7, 0x03bb, 0x03cf, 0x03e3, 0x03f7, 0x040c, 0x0420, 0x0434, 0x0448, 0x045c,
    0x0470, 0x0484, 0x0499, 0x04ad, 0x04c1, 0x04d5, 0x04e9, 0x04fd, 0x0511, 0x0525, 0x0539, 0x054d, 0x0561, 0x0575,
    0x0589, 0x059d, 0x05b1, 0x05c5, 0x05d9, 0x05ed, 0x0601, 0x0615, 0x0629, 0x063d, 0x0651, 0x0665, 0x0678, 0x068c,
    0x06a0, 0x06b4, 0x06c8, 0x06dc, 0x06ef, 0x0703, 0x0717, 0x072b, 0x073e, 0x0752, 0x0766, 0x077a, 0x078d, 0x07a1,
    0x07b5, 0x07c8, 0x07dc, 0x07ef, 0x0803, 0x0817, 0x082a, 0x083e, 0x0851, 0x0865, 0x0878, 0x088c, 0x089f, 0x08b3,
    0x08c6, 0x08da, 0x08ed, 0x0901, 0x0914, 0x0927, 0x093b, 0x094e, 0x0961, 0x0975, 0x0988, 0x099b, 0x09ae, 0x09c2,
    0x09d5, 0x09e8, 0x09fb, 0x0a0e, 0x0a22, 0x0a35, 0x0a48, 0x0a5b, 0x0a6e, 0x0a81, 0x0a94, 0x0aa7, 0x0aba, 0x0acd,
    0x0ae0, 0x0af3, 0x0b06, 0x0b19, 0x0b2c, 0x0b3f, 0x0b51, 0x0b64, 0x0b77, 0x0b8a, 0x0b9d, 0x0baf, 0x0bc2, 0x0bd5,
    0x0be7, 0x0bfa, 0x0c0d, 0x0c1f, 0x0c32, 0x0c45, 0x0c57, 0x0c6a, 0x0c7c, 0x0c8f, 0x0ca1, 0x0cb4, 0x0cc6, 0x0cd9,
    0x0ceb, 0x0cfd, 0x0d10, 0x0d22, 0x0d34, 0x0d47, 0x0d59, 0x0d6b, 0x0d7d, 0x0d90, 0x0da2, 0x0db4, 0x0dc6, 0x0dd8,
    0x0dea, 0x0dfc, 0x0e0f, 0x0e21, 0x0e33, 0x0e45, 0x0e56, 0x0e68, 0x0e7a, 0x0e8c, 0x0e9e, 0x0eb0, 0x0ec2, 0x0ed4,
    0x0ee5, 0x0ef7, 0x0f09, 0x0f1b, 0x0f2c, 0x0f3e, 0x0f50, 0x0f61, 0x0f73, 0x0f84, 0x0f96, 0x0fa7, 0x0fb9, 0x0fca,
    0x0fdc, 0x0fed, 0x0fff, 0x1010, 0x1021, 0x1033, 0x1044, 0x1055, 0x1067, 0x1078, 0x1089, 0x109a, 0x10ab, 0x10bc,
    0x10ce, 0x10df, 0x10f0, 0x1101, 0x1112, 0x1123, 0x1134, 0x1145, 0x1156, 0x1166, 0x1177, 0x1188, 0x1199, 0x11aa,
    0x11bb, 0x11cb, 0x11dc, 0x11ed, 0x11fd, 0x120e, 0x121f, 0x122f, 0x1240, 0x1250, 0x1261, 0x1271, 0x1282, 0x1292,
    0x12a3, 0x12b3, 0x12c3, 0x12d4, 0x12e4, 0x12f4, 0x1305, 0x1315, 0x1325, 0x1335, 0x1345, 0x1355, 0x1366, 0x1376,
    0x1386, 0x1396, 0x13a6, 0x13b6, 0x13c6, 0x13d6, 0x13e6, 0x13f5, 0x1405, 0x1415, 0x1425, 0x1435, 0x1444, 0x1454,
    0x1464, 0x1473, 0x1483, 0x1493, 0x14a2, 0x14b2, 0x14c1, 0x14d1, 0x14e0, 0x14f0, 0x14ff, 0x150f, 0x151e, 0x152d,
    0x153d, 0x154c, 0x155b, 0x156b, 0x157a, 0x1589, 0x1598, 0x15a7, 0x15b7, 0x15c6, 0x15d5, 0x15e4, 0x15f3, 0x1602,
    0x1611, 0x1620, 0x162f, 0x163e, 0x164c, 0x165b, 0x166a, 0x1679, 0x1688, 0x1696, 0x16a5, 0x16b4, 0x16c2, 0x16d1,
    0x16e0, 0x16ee, 0x16fd, 0x170b, 0x171a, 0x1728, 0x1737, 0x1745, 0x1754, 0x1762, 0x1770, 0x177f, 0x178d, 0x179b,
    0x17aa, 0x17b8, 0x17c6, 0x17d4, 0x17e2, 0x17f0, 0x17fe, 0x180d, 0x181b, 0x1829, 0x1837, 0x1845, 0x1853, 0x1860,
    0x186e, 0x187c, 0x188a, 0x1898, 0x18a6, 0x18b3, 0x18c1, 0x18cf, 0x18dd, 0x18ea, 0x18f8, 0x1906, 0x1913, 0x1921,
    0x192e, 0x193c, 0x1949, 0x1957, 0x1964, 0x1972, 0x197f, 0x198c, 0x199a, 0x19a7, 0x19b4, 0x19c2, 0x19cf, 0x19dc,
    0x19e9, 0x19f6, 0x1a04, 0x1a11, 0x1a1e, 0x1a2b, 0x1a38, 0x1a45, 0x1a52, 0x1a5f, 0x1a6c, 0x1a79, 0x1a86, 0x1a93,
    0x1a9f, 0x1aac, 0x1ab9, 0x1ac6, 0x1ad3, 0x1adf, 0x1aec, 0x1af9, 0x1b05, 0x1b12, 0x1b1f, 0x1b2b, 0x1b38, 0x1b44,
    0x1b51, 0x1b5d, 0x1b6a, 0x1b76, 0x1b83, 0x1b8f, 0x1b9c, 0x1ba8, 0x1bb4, 0x1bc1, 0x1bcd, 0x1bd9, 0x1be5, 0x1bf2,
    0x1bfe, 0x1c0a, 0x1c16, 0x1c22, 0x1c2e, 0x1c3a, 0x1c46, 0x1c52, 0x1c5e, 0x1c6a, 0x1c76, 0x1c82, 0x1c8e, 0x1c9a,
    0x1ca6, 0x1cb2, 0x1cbe, 0x1cc9, 0x1cd5, 0x1ce1, 0x1ced, 0x1cf8, 0x1d04, 0x1d10, 0x1d1b, 0x1d27, 0x1d33, 0x1d3e,
    0x1d4a, 0x1d55, 0x1d61, 0x1d6c, 0x1d78, 0x1d83, 0x1d8e, 0x1d9a, 0x1da5, 0x1db1, 0x1dbc, 0x1dc7, 0x1dd3, 0x1dde,
    0x1de9, 0x1df4, 0x1dff, 0x1e0b, 0x1e16, 0x1e21, 0x1e2c, 0x1e37, 0x1e42, 0x1e4d, 0x1e58, 0x1e63, 0x1e6e, 0x1e79,
    0x1e84, 0x1e8f, 0x1e9a, 0x1ea5, 0x1eb0, 0x1eba, 0x1ec5, 0x1ed0, 0x1edb, 0x1ee6, 0x1ef0, 0x1efb, 0x1f06, 0x1f10,
    0x1f1b, 0x1f26, 0x1f30, 0x1f3b, 0x1f45, 0x1f50, 0x1f5a, 0x1f65, 0x1f6f, 0x1f7a, 0x1f84, 0x1f8f, 0x1f99, 0x1fa4,
    0x1fae, 0x1fb8, 0x1fc3, 0x1fcd, 0x1fd7, 0x1fe1, 0x1fec, 0x1ff6, 0x2000};
