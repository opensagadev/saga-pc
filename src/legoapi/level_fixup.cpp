#include "saga.h"

/* Original level.cpp FixUpLevels family; PC-specific startup gates verified. */
extern "C" {
    extern LEVELDATA_PC *WorldArchiveOneLevelsPc[11];
    extern LEVELDATA_PC *WorldArchiveThreeLevelsPc[4];
    extern LEVELDATA_PC *TITLES_LDATA;
    extern LEVELDATA_PC *STATUS_LDATA;
    extern LEVELDATA_PC *CREDITS_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b2c4;
    extern LEVELDATA_PC *UpdateSockets_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b2cc = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b2d0 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b2d4 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b2d8 = NULL;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b2dc;
    extern LEVELDATA_PC *HUB_LDATA;
    extern LEVELDATA_PC *SPEEDERCHASEA_LDATA;
    extern LEVELDATA_PC *NEGOTIATIONSA_LDATA;
    extern LEVELDATA_PC *NEGOTIATIONSB_LDATA;
    extern LEVELDATA_PC *NEGOTIATIONSC_LDATA;
    extern LEVELDATA_PC *GUNGAN_A_LDATA;
    extern LEVELDATA_PC *RESCUEC_LDATA;
    extern LEVELDATA_PC *PODRACEB_LDATA;
    extern LEVELDATA_PC *PODRACEA_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b0b4;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b0b8;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b0bc;
    extern LEVELDATA_PC *PODSPRINTA_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b0d4 = NULL;
    extern LEVELDATA_PC *RETAKEB_LDATA;
    extern LEVELDATA_PC *RETAKED_LDATA;
    extern LEVELDATA_PC *RETAKEE_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b0d0 = NULL;
    extern LEVELDATA_PC *MAULA_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b0dc = NULL;
    extern LEVELDATA_PC *MAULD_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b0e4 = NULL;
    extern LEVELDATA_PC *MAULF_LDATA;
    extern LEVELDATA_PC *JEDI_B_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b0f0;
    extern LEVELDATA_PC *GUNSHIPA_LDATA;
    extern LEVELDATA_PC *GUNSHIPB_LDATA;
    extern LEVELDATA_PC *BONUS_GUNSHIPA_LDATA;
    extern LEVELDATA_PC *BONUS_GUNSHIPB_LDATA;
    extern LEVELDATA_PC *CollisionVehicleExcludedLevelPc;
    static LEVELDATA_PC *LevelFixupSlotPc0087b108 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b10c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b110 = NULL;
    extern LEVELDATA_PC *BOUNTYHUNTERPURSUITE_LDATA;
    extern LEVELDATA_PC *DOGFIGHTA_LDATA;
    extern LEVELDATA_PC *FACTORYB_LDATA;
    extern LEVELDATA_PC *FACTORYD_LDATA;
    extern LEVELDATA_PC *FACTORYF_LDATA;
    extern LEVELDATA_PC *FACTORYG_LDATA;
    extern LEVELDATA_PC *DOOKUC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b140;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b12c;
    extern LEVELDATA_PC *KAMINOC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b138;
    extern LEVELDATA_PC *KAMINOE_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b164;
    extern LEVELDATA_PC *CRUISERC_LDATA;
    extern LEVELDATA_PC *CRUISERD_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b170;
    extern LEVELDATA_PC *CRUISERG_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b144 = NULL;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b148;
    static LEVELDATA_PC *LevelFixupSlotPc0087b14c = NULL;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b150;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b154;
    static LEVELDATA_PC *LevelFixupSlotPc0087b158 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b15c = NULL;
    extern LEVELDATA_PC *KASHYYYKD_LDATA;
    extern LEVELDATA_PC *VADERA_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b17c;
    extern LEVELDATA_PC *VADERC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b194;
    extern LEVELDATA_PC *ObstacleReturnLevelPc;
    static LEVELDATA_PC *LevelFixupSlotPc0087b19c = NULL;
    extern LEVELDATA_PC *MOSEISLEYA_LDATA;
    extern LEVELDATA_PC *MOSEISLEYB_LDATA;
    extern LEVELDATA_PC *MOSEISLEYC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1bc;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1c0;
    extern LEVELDATA_PC *TATOOINEA_LDATA;
    extern LEVELDATA_PC *TATOOINEB_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1a8;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1ac;
    extern LEVELDATA_PC *DEATHSTARRESCUEA_LDATA;
    extern LEVELDATA_PC *DEATHSTARRESCUEB_LDATA;
    extern LEVELDATA_PC *DEATHSTARRESCUEC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1d0;
    extern LEVELDATA_PC *DEATHSTARRESCUEE_LDATA;
    extern LEVELDATA_PC *DEATHSTARESCAPEA_LDATA;
    extern LEVELDATA_PC *DEATHSTARESCAPEB_LDATA;
    extern LEVELDATA_PC *DEATHSTARESCAPEC_LDATA;
    extern LEVELDATA_PC *DEATHSTARESCAPED_LDATA;
    extern LEVELDATA_PC *DEATHSTARBATTLEA_LDATA;
    extern LEVELDATA_PC *DEATHSTARBATTLEB_LDATA;
    extern LEVELDATA_PC *DEATHSTARBATTLEC_LDATA;
    extern LEVELDATA_PC *DEATHSTARBATTLED_LDATA;
    extern LEVELDATA_PC *HOTHBATTLEA_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b200;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b204;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b208;
    static LEVELDATA_PC *LevelFixupSlotPc0087b20c = NULL;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b210;
    extern LEVELDATA_PC *HOTHESCAPEA_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b218;
    extern LEVELDATA_PC *HOTHESCAPEC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b220;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b224;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b228;
    extern LEVELDATA_PC *ASTEROIDCHASEC_LDATA;
    extern LEVELDATA_PC *ASTEROIDCHASED_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b234;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b258;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b264;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b268;
    static LEVELDATA_PC *LevelFixupSlotPc0087b25c = NULL;
    extern LEVELDATA_PC *DAGOBAHC_LDATA;
    extern LEVELDATA_PC *CLOUDCITYTRAPA_LDATA;
    extern LEVELDATA_PC *CLOUDCITYTRAPB_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b24c = NULL;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b250;
    extern LEVELDATA_PC *CLOUDCITYESCAPEA_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b23c = NULL;
    extern LEVELDATA_PC *CLOUDCITYESCAPEC_LDATA;
    extern LEVELDATA_PC *JABBASPALACEA_LDATA;
    extern LEVELDATA_PC *JABBASPALACEB_LDATA;
    extern LEVELDATA_PC *JABBASPALACED_LDATA;
    extern LEVELDATA_PC *JABBASPALACEE_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b2b0;
    extern LEVELDATA_PC *ENDORBATTLEA_LDATA;
    extern LEVELDATA_PC *ENDORBATTLEB_LDATA;
    extern LEVELDATA_PC *ENDORBATTLEC_LDATA;
    extern LEVELDATA_PC *ENDORBATTLED_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEA_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEB_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLED_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEE_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEF_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEG_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b284;
    extern LEVELDATA_PC *EMPERORFIGHTA_LDATA;
    extern LEVELDATA_PC *SARLACCPITA_LDATA;
    extern LEVELDATA_PC *SARLACCPITB_LDATA;
    extern LEVELDATA_PC *SARLACCPITC_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b298 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b29c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087af58 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087af5c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087af60 = NULL;
}
STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) == 0x68);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_index) == 0x94);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_tracks) == 0x118);
STATIC_ASSERT(sizeof(LEVELDATA_PC) == 0x130);
extern "C" void __cdecl FixUpLevels(struct level_fixup_pc_s *fixup) {
    LEVELDATA_PC *level;
    i32 index;
    Levels_FixUp(fixup);
    level = Level_FindByName("titles", NULL); /* Native call 0x48da62. */
    TITLES_LDATA = level;                     /* Native store 0x48da6c. */
    if (level != NULL) {
        STUBBED_NAMED(FixUpLevels_Callback_489b60); /* Native 0x48da7c: callbacks[1] = entry 0x489b60. */
        STUBBED_NAMED(FixUpLevels_Callback_489ca0); /* Native 0x48da89: callbacks[3] = entry 0x489ca0. */
        STUBBED_NAMED(FixUpLevels_Callback_489da0); /* Native 0x48da96: callbacks[5] = entry 0x489da0. */
        TITLES_LDATA->flags &= ~0xau;
        TITLES_LDATA->music_index = GetMusicIndex("titles", MusicInfo, -1);
        TITLES_LDATA->music_tracks[0] = TITLES_LDATA->music_tracks[1] = music_man.GetTrackHandle(1, "titles");
        TITLES_LDATA->music_tracks[2] = TITLES_LDATA->music_tracks[3] = music_man.GetTrackHandle(2, "titles");
        TITLES_LDATA->music_tracks[4] = TITLES_LDATA->music_tracks[5] = music_man.GetTrackHandle(0x20, "titles");
    }
    level = Level_FindByName("status", NULL); /* Native call 0x48db58. */
    STATUS_LDATA = level;                     /* Native store 0x48db62. */
    if (level != NULL) {
        STUBBED_NAMED(FixUpLevels_Callback_486c60); /* Native 0x48db69: callbacks[3] = entry 0x486c60. */
        STUBBED_NAMED(FixUpLevels_Callback_486200); /* Native 0x48db76: callbacks[6] = entry 0x486200. */
        STATUS_LDATA->flags |= 0x400;
        STATUS_LDATA->flags &= ~0xau;
    }
    level = Level_FindByName("credits", NULL); /* Native call 0x48db9b. */
    CREDITS_LDATA = level;                     /* Native store 0x48dba5. */
    if (level != NULL) {
        level->flags = 0;                           /* Native48dbac. */
        STUBBED_NAMED(FixUpLevels_Callback_59cd50); /* Native 0x48dbb8: callbacks[0] = entry 0x59cd50. */
        STUBBED_NAMED(FixUpLevels_Callback_48d6b0); /* Native 0x48dbc5: callbacks[1] = entry 0x48d6b0. */
        STUBBED_NAMED(FixUpLevels_Callback_48d7d0); /* Native 0x48dbd2: callbacks[3] = entry 0x48d7d0. */
        STUBBED_NAMED(FixUpLevels_Callback_48d830); /* Native 0x48dbde: callbacks[5] = entry 0x48d830. */
        STUBBED_NAMED(FixUpLevels_Callback_59d450); /* Native 0x48dbeb: callbacks[6] = entry 0x59d450. */
    }
    if (StartupGameFlag0 == 0) {
        if (StartupGameFlag1 != 0) {
            level = Level_FindByName("losttemple_a", NULL); /* Native call 0x48dc16. */
            LevelFixupSlotPc0087b2c4 = level;               /* Native store 0x48dc22. */
            level = Level_FindByName("losttemple_c", NULL); /* Native call 0x48dc27. */
            UpdateSockets_LDATA = level;                    /* Native store 0x48dc31. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_481920); /* Native 0x48dc38: callbacks[2] = entry 0x481920. */
                STUBBED_NAMED(FixUpLevels_Callback_481950); /* Native 0x48dc45: callbacks[3] = entry 0x481950. */
            }
            level = Level_FindByName("losttemple_d", NULL);         /* Native call 0x48dc53. */
            LevelFixupSlotPc0087b2cc = level;                       /* Native store 0x48dc5f. */
            level = Level_FindByName("losttemple_e", NULL);         /* Native call 0x48dc64. */
            LevelFixupSlotPc0087b2d0 = level;                       /* Native store 0x48dc70. */
            level = Level_FindByName("losttemple_f", NULL);         /* Native call 0x48dc75. */
            LevelFixupSlotPc0087b2d4 = level;                       /* Native store 0x48dc81. */
            level = Level_FindByName("losttemple_outrowin", NULL);  /* Native call 0x48dc86. */
            LevelFixupSlotPc0087b2d8 = level;                       /* Native store 0x48dc92. */
            level = Level_FindByName("losttemple_outrolose", NULL); /* Native call 0x48dc97. */
            LevelFixupSlotPc0087b2dc = level;                       /* Native store 0x48dca3. */
            level = Level_FindByName("map", NULL);                  /* Native call 0x48dca8. */
            HUB_LDATA = level;                                      /* Native store 0x48dcb2. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4778d0); /* Native 0x48dcb9: callbacks[0] = entry 0x4778d0. */
                STUBBED_NAMED(FixUpLevels_Callback_47cab0); /* Native 0x48dcc5: callbacks[1] = entry 0x47cab0. */
                STUBBED_NAMED(FixUpLevels_Callback_479b30); /* Native 0x48dcd2: callbacks[2] = entry 0x479b30. */
                STUBBED_NAMED(FixUpLevels_Callback_47cdd0); /* Native 0x48dcdf: callbacks[3] = entry 0x47cdd0. */
                STUBBED_NAMED(FixUpLevels_Callback_479ed0); /* Native 0x48dceb: callbacks[5] = entry 0x479ed0. */
                STUBBED_NAMED(FixUpLevels_Callback_47dce0); /* Native 0x48dcf8: callbacks[6] = entry 0x47dce0. */
            }
            level = Level_FindByName("speederchase_a", NULL); /* Native call 0x48dd09. */
            SPEEDERCHASEA_LDATA = level;                      /* Native store 0x48dd13. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4c8c40); /* Native 0x48dd1a: callbacks[1] = entry 0x4c8c40. */
                STUBBED_NAMED(FixUpLevels_Callback_4c8d90); /* Native 0x48dd27: callbacks[2] = entry 0x4c8d90. */
                STUBBED_NAMED(FixUpLevels_Callback_4c8e40); /* Native 0x48dd33: callbacks[3] = entry 0x4c8e40. */
                STUBBED_NAMED(FixUpLevels_Callback_4c9080); /* Native 0x48dd40: callbacks[6] = entry 0x4c9080. */
            }
            level = Level_FindByName("negotiations_a", NULL); /* Native call 0x48dd51. */
            NEGOTIATIONSA_LDATA = level;                      /* Native store 0x48dd5d. */
            level = Level_FindByName("negotiations_b", NULL); /* Native call 0x48dd62. */
            NEGOTIATIONSB_LDATA = level;                      /* Native store 0x48dd6e. */
            level = Level_FindByName("negotiations_c", NULL); /* Native call 0x48dd73. */
            NEGOTIATIONSC_LDATA = level;                      /* Native store 0x48dd7f. */
            level = Level_FindByName("gungan_a", NULL);       /* Native call 0x48dd84. */
            GUNGAN_A_LDATA = level;                           /* Native store 0x48dd8e. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4d82f0); /* Native 0x48dd95: callbacks[1] = entry 0x4d82f0. */
                STUBBED_NAMED(FixUpLevels_Callback_4d83e0); /* Native 0x48dda2: callbacks[3] = entry 0x4d83e0. */
            }
            level = Level_FindByName("rescue_c", NULL);         /* Native call 0x48ddb1. */
            RESCUEC_LDATA = level;                              /* Native store 0x48ddbd. */
            level = Level_FindByName("PodRace_Arrival1", NULL); /* Native call 0x48ddc2. */
            WorldArchiveOneLevelsPc[0] = level;                 /* Native store 0x48ddce. */
            level = Level_FindByName("PodRace_Arrival2", NULL); /* Native call 0x48ddd3. */
            WorldArchiveOneLevelsPc[1] = level;                 /* Native store 0x48dddf. */
            level = Level_FindByName("PodRace_Arrival3", NULL); /* Native call 0x48dde4. */
            WorldArchiveOneLevelsPc[2] = level;                 /* Native store 0x48ddf0. */
            level = Level_FindByName("PodRace_Arrival4", NULL); /* Native call 0x48ddf5. */
            WorldArchiveOneLevelsPc[3] = level;                 /* Native store 0x48de01. */
            level = Level_FindByName("PodRace_Intro", NULL);    /* Native call 0x48de06. */
            WorldArchiveOneLevelsPc[4] = level;                 /* Native store 0x48de12. */
            level = Level_FindByName("PodRace_B", NULL);        /* Native call 0x48de17. */
            WorldArchiveOneLevelsPc[5] = level;                 /* Native store 0x48de23. */
            level = Level_FindByName("PodRace_C", NULL);        /* Native call 0x48de28. */
            WorldArchiveOneLevelsPc[6] = level;                 /* Native store 0x48de37. */
            level = Level_FindByName("PodRace_A", NULL);        /* Native call 0x48de3c. */
            WorldArchiveOneLevelsPc[7] = level;                 /* Native store 0x48de48. */
            level = Level_FindByName("PodRace_Outro1", NULL);   /* Native call 0x48de4d. */
            WorldArchiveOneLevelsPc[8] = level;                 /* Native store 0x48de59. */
            level = Level_FindByName("PodRace_Outro2", NULL);   /* Native call 0x48de5e. */
            WorldArchiveOneLevelsPc[9] = level;                 /* Native store 0x48de6a. */
            level = Level_FindByName("PodRace_Status", NULL);   /* Native call 0x48de6f. */
            WorldArchiveOneLevelsPc[10] = level;                /* Native store 0x48de7b. */
            level = Level_FindByName("ANewHope_Intro", NULL);   /* Native call 0x48de80. */
            WorldArchiveThreeLevelsPc[0] = level;               /* Native store 0x48de8c. */
            level = Level_FindByName("ANewHope_A", NULL);       /* Native call 0x48de91. */
            WorldArchiveThreeLevelsPc[1] = level;               /* Native store 0x48de9d. */
            level = Level_FindByName("ANewHope_B", NULL);       /* Native call 0x48dea2. */
            WorldArchiveThreeLevelsPc[2] = level;               /* Native store 0x48deae. */
            level = Level_FindByName("ANewHope_Status", NULL);  /* Native call 0x48deb3. */
            WorldArchiveThreeLevelsPc[3] = level;               /* Native store 0x48dec2. */
            level = Level_FindByName("podrace_b", NULL);        /* Native call 0x48dec7. */
            PODRACEB_LDATA = level;                             /* Native store 0x48ded1. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4dd580); /* Native 0x48dee2: callbacks[1] = entry 0x4dd580. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd5c0); /* Native 0x48deee: callbacks[2] = entry 0x4dd5c0. */
                STUBBED_NAMED(FixUpLevels_Callback_4dae30); /* Native 0x48defb: callbacks[3] = entry 0x4dae30. */
            }
            level = Level_FindByName("podrace_a", NULL); /* Native call 0x48df1d. */
            PODRACEA_LDATA = level;                      /* Native store 0x48df27. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4dd4e0); /* Native 0x48df2e: callbacks[1] = entry 0x4dd4e0. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd4f0); /* Native 0x48df3b: callbacks[2] = entry 0x4dd4f0. */
                STUBBED_NAMED(FixUpLevels_Callback_4daab0); /* Native 0x48df48: callbacks[3] = entry 0x4daab0. */
                STUBBED_NAMED(FixUpLevels_Callback_4daaf0); /* Native 0x48df54: callbacks[4] = entry 0x4daaf0. */
                STUBBED_NAMED(FixUpLevels_Callback_4dab40); /* Native 0x48df61: callbacks[5] = entry 0x4dab40. */
            }
            level = Level_FindByName("podrace_c", NULL); /* Native call 0x48df7b. */
            LevelFixupSlotPc0087b0b4 = level;            /* Native store 0x48df85. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4dd650); /* Native 0x48df8c: callbacks[1] = entry 0x4dd650. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd770); /* Native 0x48df98: callbacks[2] = entry 0x4dd770. */
                STUBBED_NAMED(FixUpLevels_Callback_4db080); /* Native 0x48dfa5: callbacks[3] = entry 0x4db080. */
            }
            level = Level_FindByName("PodRace_Outro1", NULL); /* Native call 0x48dfc7. */
            LevelFixupSlotPc0087b0b8 = level;                 /* Native store 0x48dfd3. */
            level = Level_FindByName("podrace_status", NULL); /* Native call 0x48dfd8. */
            LevelFixupSlotPc0087b0bc = level;                 /* Native store 0x48dfe4. */
            level = Level_FindByName("PodSprint_A", NULL);    /* Native call 0x48dfe9. */
            PODSPRINTA_LDATA = level;                         /* Native store 0x48dff3. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4dbce0); /* Native 0x48dffa: callbacks[1] = entry 0x4dbce0. */
                STUBBED_NAMED(FixUpLevels_Callback_4dbe20); /* Native 0x48e007: callbacks[2] = entry 0x4dbe20. */
                STUBBED_NAMED(FixUpLevels_Callback_4dc640); /* Native 0x48e014: callbacks[3] = entry 0x4dc640. */
                STUBBED_NAMED(FixUpLevels_Callback_4dc810); /* Native 0x48e020: callbacks[6] = entry 0x4dc810. */
            }
            level = Level_FindByName("AnakinsFlight_B", NULL); /* Native call 0x48e031. */
            LevelFixupSlotPc0087b0d4 = level;                  /* Native store 0x48e03b. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4dd2f0); /* Native 0x48e042: callbacks[1] = entry 0x4dd2f0. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd380); /* Native 0x48e04f: callbacks[3] = entry 0x4dd380. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd390); /* Native 0x48e05c: callbacks[5] = entry 0x4dd390. */
            }
            level = Level_FindByName("retake_b", NULL); /* Native call 0x48e06a. */
            RETAKEB_LDATA = level;                      /* Native store 0x48e076. */
            level = Level_FindByName("retake_d", NULL); /* Native call 0x48e07b. */
            RETAKED_LDATA = level;                      /* Native store 0x48e087. */
            level = Level_FindByName("retake_e", NULL); /* Native call 0x48e08c. */
            RETAKEE_LDATA = level;                      /* Native store 0x48e098. */
            level = Level_FindByName("retake_g", NULL); /* Native call 0x48e09d. */
            LevelFixupSlotPc0087b0d0 = level;           /* Native store 0x48e0a7. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4dceb0); /* Native 0x48e0ae: callbacks[1] = entry 0x4dceb0. */
                STUBBED_NAMED(FixUpLevels_Callback_4dcef0); /* Native 0x48e0ba: callbacks[2] = entry 0x4dcef0. */
                STUBBED_NAMED(FixUpLevels_Callback_4dcf00); /* Native 0x48e0c7: callbacks[3] = entry 0x4dcf00. */
                STUBBED_NAMED(FixUpLevels_Callback_4dcf70); /* Native 0x48e0d4: callbacks[6] = entry 0x4dcf70. */
            }
            level = Level_FindByName("Maul_A", NULL); /* Native call 0x48e0e5. */
            MAULA_LDATA = level;                      /* Native store 0x48e0ef. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4dd070); /* Native 0x48e0f6: callbacks[1] = entry 0x4dd070. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd110); /* Native 0x48e102: callbacks[2] = entry 0x4dd110. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd160); /* Native 0x48e10f: callbacks[3] = entry 0x4dd160. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd170); /* Native 0x48e11c: callbacks[6] = entry 0x4dd170. */
            }
            level = Level_FindByName("Maul_B", NULL); /* Native call 0x48e12d. */
            LevelFixupSlotPc0087b0dc = level;         /* Native store 0x48e139. */
            level = Level_FindByName("Maul_D", NULL); /* Native call 0x48e13e. */
            MAULD_LDATA = level;                      /* Native store 0x48e148. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4dd1d0); /* Native 0x48e14f: callbacks[1] = entry 0x4dd1d0. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd1e0); /* Native 0x48e15b: callbacks[3] = entry 0x4dd1e0. */
            }
            level = Level_FindByName("Maul_E", NULL); /* Native call 0x48e169. */
            LevelFixupSlotPc0087b0e4 = level;         /* Native store 0x48e173. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4dd1f0); /* Native 0x48e17a: callbacks[1] = entry 0x4dd1f0. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd200); /* Native 0x48e187: callbacks[3] = entry 0x4dd200. */
            }
            level = Level_FindByName("Maul_F", NULL); /* Native call 0x48e195. */
            MAULF_LDATA = level;                      /* Native store 0x48e19f. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4dd210); /* Native 0x48e1a6: callbacks[1] = entry 0x4dd210. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd280); /* Native 0x48e1b3: callbacks[2] = entry 0x4dd280. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd2a0); /* Native 0x48e1bf: callbacks[3] = entry 0x4dd2a0. */
                STUBBED_NAMED(FixUpLevels_Callback_4dd2b0); /* Native 0x48e1cc: callbacks[6] = entry 0x4dd2b0. */
            }
            level = Level_FindByName("Jedi_B", NULL); /* Native call 0x48e1dd. */
            JEDI_B_LDATA = level;                     /* Native store 0x48e1e7. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4de360); /* Native 0x48e1ee: callbacks[1] = entry 0x4de360. */
                STUBBED_NAMED(FixUpLevels_Callback_4de740); /* Native 0x48e1fb: callbacks[2] = entry 0x4de740. */
                STUBBED_NAMED(FixUpLevels_Callback_4df120); /* Native 0x48e207: callbacks[3] = entry 0x4df120. */
                STUBBED_NAMED(FixUpLevels_Callback_4df9b0); /* Native 0x48e214: callbacks[6] = entry 0x4df9b0. */
            }
            level = Level_FindByName("Jedi_Outro", NULL); /* Native call 0x48e225. */
            LevelFixupSlotPc0087b0f0 = level;             /* Native store 0x48e231. */
            level = Level_FindByName("gunship_a", NULL);  /* Native call 0x48e236. */
            GUNSHIPA_LDATA = level;                       /* Native store 0x48e240. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e23e0); /* Native 0x48e247: callbacks[1] = entry 0x4e23e0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e24d0); /* Native 0x48e254: callbacks[3] = entry 0x4e24d0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e24e0); /* Native 0x48e260: callbacks[5] = entry 0x4e24e0. */
            }
            level = Level_FindByName("gunship_b", NULL); /* Native call 0x48e26e. */
            GUNSHIPB_LDATA = level;                      /* Native store 0x48e278. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e24f0); /* Native 0x48e27f: callbacks[2] = entry 0x4e24f0. */
            }
            level = Level_FindByName("bonus_gunship_a", NULL); /* Native call 0x48e28d. */
            BONUS_GUNSHIPA_LDATA = level;                      /* Native store 0x48e297. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e2740); /* Native 0x48e29e: callbacks[2] = entry 0x4e2740. */
                STUBBED_NAMED(FixUpLevels_Callback_4e2770); /* Native 0x48e2ab: callbacks[3] = entry 0x4e2770. */
            }
            level = Level_FindByName("bonus_gunship_b", NULL); /* Native call 0x48e2b9. */
            BONUS_GUNSHIPB_LDATA = level;                      /* Native store 0x48e2c3. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e2860); /* Native 0x48e2ca: callbacks[1] = entry 0x4e2860. */
                STUBBED_NAMED(FixUpLevels_Callback_4e2890); /* Native 0x48e2d7: callbacks[2] = entry 0x4e2890. */
                STUBBED_NAMED(FixUpLevels_Callback_4e28b0); /* Native 0x48e2e3: callbacks[3] = entry 0x4e28b0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e2ae0); /* Native 0x48e2f0: callbacks[6] = entry 0x4e2ae0. */
            }
            level = Level_FindByName("pursuit_a", NULL); /* Native call 0x48e301. */
            CollisionVehicleExcludedLevelPc = level;     /* Native store 0x48e30b. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4bc4b0); /* Native 0x48e312: callbacks[1] = entry 0x4bc4b0. */
                STUBBED_NAMED(FixUpLevels_Callback_4bc4c0); /* Native 0x48e31f: callbacks[2] = entry 0x4bc4c0. */
                STUBBED_NAMED(FixUpLevels_Callback_4bc4f0); /* Native 0x48e32b: callbacks[3] = entry 0x4bc4f0. */
            }
            level = Level_FindByName("pursuit_b", NULL); /* Native call 0x48e339. */
            LevelFixupSlotPc0087b108 = level;            /* Native store 0x48e343. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4bc500); /* Native 0x48e34a: callbacks[1] = entry 0x4bc500. */
                STUBBED_NAMED(FixUpLevels_Callback_4bc510); /* Native 0x48e357: callbacks[2] = entry 0x4bc510. */
                STUBBED_NAMED(FixUpLevels_Callback_4bc620); /* Native 0x48e364: callbacks[3] = entry 0x4bc620. */
            }
            level = Level_FindByName("pursuit_c", NULL); /* Native call 0x48e372. */
            LevelFixupSlotPc0087b10c = level;            /* Native store 0x48e37c. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4bc640); /* Native 0x48e383: callbacks[1] = entry 0x4bc640. */
                STUBBED_NAMED(FixUpLevels_Callback_4bc650); /* Native 0x48e38f: callbacks[2] = entry 0x4bc650. */
                STUBBED_NAMED(FixUpLevels_Callback_4bc750); /* Native 0x48e39c: callbacks[3] = entry 0x4bc750. */
            }
            level = Level_FindByName("pursuit_d", NULL); /* Native call 0x48e3aa. */
            LevelFixupSlotPc0087b110 = level;            /* Native store 0x48e3b4. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4bc800); /* Native 0x48e3bb: callbacks[1] = entry 0x4bc800. */
                STUBBED_NAMED(FixUpLevels_Callback_4bc810); /* Native 0x48e3c8: callbacks[2] = entry 0x4bc810. */
                STUBBED_NAMED(FixUpLevels_Callback_4bc990); /* Native 0x48e3d4: callbacks[3] = entry 0x4bc990. */
            }
            level = Level_FindByName("pursuit_e", NULL);  /* Native call 0x48e3e2. */
            BOUNTYHUNTERPURSUITE_LDATA = level;           /* Native store 0x48e3ee. */
            level = Level_FindByName("dogfight_a", NULL); /* Native call 0x48e3f3. */
            DOGFIGHTA_LDATA = level;                      /* Native store 0x48e3fd. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_413a20); /* Native 0x48e404: callbacks[1] = entry 0x413a20. */
                STUBBED_NAMED(FixUpLevels_Callback_4124f0); /* Native 0x48e411: callbacks[2] = entry 0x4124f0. */
                STUBBED_NAMED(FixUpLevels_Callback_413aa0); /* Native 0x48e41e: callbacks[3] = entry 0x413aa0. */
                STUBBED_NAMED(FixUpLevels_Callback_413b10); /* Native 0x48e42a: callbacks[5] = entry 0x413b10. */
            }
            level = Level_FindByName("Factory_B", NULL); /* Native call 0x48e438. */
            FACTORYB_LDATA = level;                      /* Native store 0x48e442. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e2c60); /* Native 0x48e449: callbacks[1] = entry 0x4e2c60. */
                STUBBED_NAMED(FixUpLevels_Callback_4dfc90); /* Native 0x48e456: callbacks[2] = entry 0x4dfc90. */
                STUBBED_NAMED(FixUpLevels_Callback_4dfd00); /* Native 0x48e463: callbacks[3] = entry 0x4dfd00. */
                STUBBED_NAMED(FixUpLevels_Callback_4dfdf0); /* Native 0x48e46f: callbacks[5] = entry 0x4dfdf0. */
            }
            level = Level_FindByName("Factory_D", NULL); /* Native call 0x48e47d. */
            FACTORYD_LDATA = level;                      /* Native store 0x48e489. */
            level = Level_FindByName("Factory_F", NULL); /* Native call 0x48e48e. */
            FACTORYF_LDATA = level;                      /* Native store 0x48e49a. */
            level = Level_FindByName("Factory_G", NULL); /* Native call 0x48e49f. */
            FACTORYG_LDATA = level;                      /* Native store 0x48e4a9. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4dff40); /* Native 0x48e4b0: callbacks[1] = entry 0x4dff40. */
                STUBBED_NAMED(FixUpLevels_Callback_4dfff0); /* Native 0x48e4bd: callbacks[3] = entry 0x4dfff0. */
            }
            level = Level_FindByName("Dooku_C", NULL); /* Native call 0x48e4cb. */
            DOOKUC_LDATA = level;                      /* Native store 0x48e4d5. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4dfe00); /* Native 0x48e4dc: callbacks[2] = entry 0x4dfe00. */
                STUBBED_NAMED(FixUpLevels_Callback_4dfe80); /* Native 0x48e4e9: callbacks[3] = entry 0x4dfe80. */
                STUBBED_NAMED(FixUpLevels_Callback_4dfef0); /* Native 0x48e4f5: callbacks[6] = entry 0x4dfef0. */
            }
            level = Level_FindByName("Dooku_Outro", NULL); /* Native call 0x48e506. */
            LevelFixupSlotPc0087b140 = level;              /* Native store 0x48e512. */
            level = Level_FindByName("Kamino_A", NULL);    /* Native call 0x48e517. */
            LevelFixupSlotPc0087b12c = level;              /* Native store 0x48e521. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e0180); /* Native 0x48e528: callbacks[4] = entry 0x4e0180. */
            }
            level = Level_FindByName("Kamino_C", NULL); /* Native call 0x48e536. */
            KAMINOC_LDATA = level;                      /* Native store 0x48e540. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e06e0); /* Native 0x48e547: callbacks[2] = entry 0x4e06e0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e01b0); /* Native 0x48e554: callbacks[1] = entry 0x4e01b0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e0b50); /* Native 0x48e561: callbacks[3] = entry 0x4e0b50. */
            }
            level = Level_FindByName("Kamino_Outro1", NULL); /* Native call 0x48e56f. */
            LevelFixupSlotPc0087b138 = level;                /* Native store 0x48e579. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e23b0); /* Native 0x48e580: callbacks[1] = entry 0x4e23b0. */
            }
            level = Level_FindByName("Kamino_A", NULL); /* Native call 0x48e58e. */
            LevelFixupSlotPc0087b12c = level;           /* Native store 0x48e59a. */
            level = Level_FindByName("Kamino_C", NULL); /* Native call 0x48e59f. */
            KAMINOC_LDATA = level;                      /* Native store 0x48e5ab. */
            level = Level_FindByName("Kamino_E", NULL); /* Native call 0x48e5b0. */
            KAMINOE_LDATA = level;                      /* Native store 0x48e5ba. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e1260); /* Native 0x48e5c1: callbacks[2] = entry 0x4e1260. */
                STUBBED_NAMED(FixUpLevels_Callback_4e1220); /* Native 0x48e5cd: callbacks[1] = entry 0x4e1220. */
                STUBBED_NAMED(FixUpLevels_Callback_4e1f40); /* Native 0x48e5da: callbacks[4] = entry 0x4e1f40. */
                STUBBED_NAMED(FixUpLevels_Callback_4e1f70); /* Native 0x48e5e7: callbacks[3] = entry 0x4e1f70. */
                STUBBED_NAMED(FixUpLevels_Callback_4e2280); /* Native 0x48e5f3: callbacks[5] = entry 0x4e2280. */
            }
            level = Level_FindByName("Dogfight_A", NULL); /* Native call 0x48e601. */
            DOGFIGHTA_LDATA = level;                      /* Native store 0x48e60d. */
            level = Level_FindByName("Cruiser_A", NULL);  /* Native call 0x48e612. */
            LevelFixupSlotPc0087b164 = level;             /* Native store 0x48e61c. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e2cf0); /* Native 0x48e623: callbacks[1] = entry 0x4e2cf0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e2d50); /* Native 0x48e630: callbacks[3] = entry 0x4e2d50. */
            }
            level = Level_FindByName("Cruiser_C", NULL); /* Native call 0x48e63e. */
            CRUISERC_LDATA = level;                      /* Native store 0x48e648. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e2db0); /* Native 0x48e64f: callbacks[2] = entry 0x4e2db0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e2de0); /* Native 0x48e65c: callbacks[3] = entry 0x4e2de0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e2f40); /* Native 0x48e668: callbacks[6] = entry 0x4e2f40. */
            }
            level = Level_FindByName("Cruiser_D", NULL); /* Native call 0x48e679. */
            CRUISERD_LDATA = level;                      /* Native store 0x48e683. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e2f80); /* Native 0x48e68a: callbacks[1] = entry 0x4e2f80. */
                STUBBED_NAMED(FixUpLevels_Callback_4e3040); /* Native 0x48e697: callbacks[2] = entry 0x4e3040. */
                STUBBED_NAMED(FixUpLevels_Callback_4e3120); /* Native 0x48e6a4: callbacks[3] = entry 0x4e3120. */
            }
            level = Level_FindByName("Cruiser_E", NULL);  /* Native call 0x48e6b2. */
            LevelFixupSlotPc0087b170 = level;             /* Native store 0x48e6be. */
            level = Level_FindByName("cruiser_g", NULL);  /* Native call 0x48e6c3. */
            CRUISERG_LDATA = level;                       /* Native store 0x48e6cf. */
            level = Level_FindByName("Grievous_A", NULL); /* Native call 0x48e6d4. */
            LevelFixupSlotPc0087b144 = level;             /* Native store 0x48e6de. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e3330); /* Native 0x48e6e5: callbacks[1] = entry 0x4e3330. */
                STUBBED_NAMED(FixUpLevels_Callback_4e3340); /* Native 0x48e6f1: callbacks[2] = entry 0x4e3340. */
                STUBBED_NAMED(FixUpLevels_Callback_4e3380); /* Native 0x48e6fe: callbacks[3] = entry 0x4e3380. */
            }
            level = Level_FindByName("temple_b", NULL); /* Native call 0x48e70c. */
            LevelFixupSlotPc0087b148 = level;           /* Native store 0x48e718. */
            level = Level_FindByName("temple_c", NULL); /* Native call 0x48e71d. */
            LevelFixupSlotPc0087b14c = level;           /* Native store 0x48e727. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e3c30); /* Native 0x48e72e: callbacks[4] = entry 0x4e3c30. */
            }
            level = Level_FindByName("temple_status", NULL); /* Native call 0x48e73c. */
            LevelFixupSlotPc0087b150 = level;                /* Native store 0x48e748. */
            level = Level_FindByName("Kashyyyk_A", NULL);    /* Native call 0x48e74d. */
            LevelFixupSlotPc0087b154 = level;                /* Native store 0x48e757. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e33e0); /* Native 0x48e75e: callbacks[1] = entry 0x4e33e0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e33f0); /* Native 0x48e76b: callbacks[2] = entry 0x4e33f0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e3430); /* Native 0x48e777: callbacks[3] = entry 0x4e3430. */
                STUBBED_NAMED(FixUpLevels_Callback_4e3440); /* Native 0x48e784: callbacks[6] = entry 0x4e3440. */
            }
            level = Level_FindByName("Kashyyyk_B", NULL); /* Native call 0x48e795. */
            LevelFixupSlotPc0087b158 = level;             /* Native store 0x48e79f. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e3540); /* Native 0x48e7a6: callbacks[1] = entry 0x4e3540. */
                STUBBED_NAMED(FixUpLevels_Callback_4e3550); /* Native 0x48e7b3: callbacks[2] = entry 0x4e3550. */
                STUBBED_NAMED(FixUpLevels_Callback_4e3560); /* Native 0x48e7bf: callbacks[3] = entry 0x4e3560. */
            }
            level = Level_FindByName("Kashyyyk_C", NULL); /* Native call 0x48e7cd. */
            LevelFixupSlotPc0087b15c = level;             /* Native store 0x48e7d7. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e3570); /* Native 0x48e7de: callbacks[1] = entry 0x4e3570. */
                STUBBED_NAMED(FixUpLevels_Callback_4e3580); /* Native 0x48e7eb: callbacks[3] = entry 0x4e3580. */
            }
            level = Level_FindByName("Kashyyyk_D", NULL); /* Native call 0x48e7f9. */
            KASHYYYKD_LDATA = level;                      /* Native store 0x48e803. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e3590); /* Native 0x48e80a: callbacks[1] = entry 0x4e3590. */
                STUBBED_NAMED(FixUpLevels_Callback_4e35d0); /* Native 0x48e817: callbacks[2] = entry 0x4e35d0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e3990); /* Native 0x48e823: callbacks[3] = entry 0x4e3990. */
            }
            level = Level_FindByName("vader_a", NULL); /* Native call 0x48e831. */
            VADERA_LDATA = level;                      /* Native store 0x48e83b. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e3db0); /* Native 0x48e842: callbacks[1] = entry 0x4e3db0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e3e50); /* Native 0x48e84f: callbacks[2] = entry 0x4e3e50. */
                STUBBED_NAMED(FixUpLevels_Callback_4e3ea0); /* Native 0x48e85c: callbacks[3] = entry 0x4e3ea0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e41e0); /* Native 0x48e868: callbacks[6] = entry 0x4e41e0. */
            }
            level = Level_FindByName("vader_b", NULL); /* Native call 0x48e879. */
            LevelFixupSlotPc0087b17c = level;          /* Native store 0x48e883. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e4260); /* Native 0x48e88a: callbacks[1] = entry 0x4e4260. */
                STUBBED_NAMED(FixUpLevels_Callback_4e4270); /* Native 0x48e897: callbacks[2] = entry 0x4e4270. */
                STUBBED_NAMED(FixUpLevels_Callback_4e42a0); /* Native 0x48e8a4: callbacks[3] = entry 0x4e42a0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e4340); /* Native 0x48e8b0: callbacks[6] = entry 0x4e4340. */
            }
            level = Level_FindByName("vader_c", NULL); /* Native call 0x48e8c1. */
            VADERC_LDATA = level;                      /* Native store 0x48e8cb. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e4350); /* Native 0x48e8d2: callbacks[1] = entry 0x4e4350. */
                STUBBED_NAMED(FixUpLevels_Callback_4e4470); /* Native 0x48e8df: callbacks[2] = entry 0x4e4470. */
                STUBBED_NAMED(FixUpLevels_Callback_4e4480); /* Native 0x48e8ec: callbacks[3] = entry 0x4e4480. */
                STUBBED_NAMED(FixUpLevels_Callback_4e49c0); /* Native 0x48e8f8: callbacks[6] = entry 0x4e49c0. */
            }
            level = Level_FindByName("BlockadeRunner_B", NULL); /* Native call 0x48e909. */
            LevelFixupSlotPc0087b194 = level;                   /* Native store 0x48e913. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e4b40); /* Native 0x48e91a: callbacks[1] = entry 0x4e4b40. */
                STUBBED_NAMED(FixUpLevels_Callback_4e4b60); /* Native 0x48e927: callbacks[3] = entry 0x4e4b60. */
            }
            level = Level_FindByName("BlockadeRunner_C", NULL); /* Native call 0x48e935. */
            ObstacleReturnLevelPc = level;                      /* Native store 0x48e93f. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e4b90); /* Native 0x48e946: callbacks[1] = entry 0x4e4b90. */
            }
            level = Level_FindByName("BlockadeRunner_D", NULL); /* Native call 0x48e954. */
            LevelFixupSlotPc0087b19c = level;                   /* Native store 0x48e95e. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e4df0); /* Native 0x48e965: callbacks[2] = entry 0x4e4df0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e4e40); /* Native 0x48e972: callbacks[3] = entry 0x4e4e40. */
            }
            level = Level_FindByName("moseisley_a", NULL); /* Native call 0x48e980. */
            MOSEISLEYA_LDATA = level;                      /* Native store 0x48e98c. */
            level = Level_FindByName("moseisley_b", NULL); /* Native call 0x48e991. */
            MOSEISLEYB_LDATA = level;                      /* Native store 0x48e99b. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e4a40); /* Native 0x48e9a2: callbacks[1] = entry 0x4e4a40. */
            }
            level = Level_FindByName("moseisley_c", NULL); /* Native call 0x48e9b0. */
            MOSEISLEYC_LDATA = level;                      /* Native store 0x48e9bc. */
            level = Level_FindByName("moseisley_d", NULL); /* Native call 0x48e9c1. */
            LevelFixupSlotPc0087b1bc = level;              /* Native store 0x48e9cb. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e4ab0); /* Native 0x48e9d2: callbacks[4] = entry 0x4e4ab0. */
            }
            level = Level_FindByName("moseisley_e", NULL); /* Native call 0x48e9e0. */
            LevelFixupSlotPc0087b1c0 = level;              /* Native store 0x48e9ea. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e4ad0); /* Native 0x48e9f1: callbacks[1] = entry 0x4e4ad0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e4b00); /* Native 0x48e9fd: callbacks[2] = entry 0x4e4b00. */
                STUBBED_NAMED(FixUpLevels_Callback_4e4b10); /* Native 0x48ea0a: callbacks[3] = entry 0x4e4b10. */
            }
            level = Level_FindByName("tatooine_a", NULL);        /* Native call 0x48ea18. */
            TATOOINEA_LDATA = level;                             /* Native store 0x48ea24. */
            level = Level_FindByName("tatooine_b", NULL);        /* Native call 0x48ea29. */
            TATOOINEB_LDATA = level;                             /* Native store 0x48ea35. */
            level = Level_FindByName("tatooine_d", NULL);        /* Native call 0x48ea3a. */
            LevelFixupSlotPc0087b1a8 = level;                    /* Native store 0x48ea46. */
            level = Level_FindByName("tatooine_e", NULL);        /* Native call 0x48ea4b. */
            LevelFixupSlotPc0087b1ac = level;                    /* Native store 0x48ea57. */
            level = Level_FindByName("deathstarrescue_a", NULL); /* Native call 0x48ea5c. */
            DEATHSTARRESCUEA_LDATA = level;                      /* Native store 0x48ea68. */
            level = Level_FindByName("deathstarrescue_b", NULL); /* Native call 0x48ea6d. */
            DEATHSTARRESCUEB_LDATA = level;                      /* Native store 0x48ea77. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e4f80); /* Native 0x48ea7e: callbacks[1] = entry 0x4e4f80. */
                STUBBED_NAMED(FixUpLevels_Callback_4e5060); /* Native 0x48ea8b: callbacks[3] = entry 0x4e5060. */
                STUBBED_NAMED(FixUpLevels_Callback_4e5130); /* Native 0x48ea97: callbacks[4] = entry 0x4e5130. */
            }
            level = Level_FindByName("deathstarrescue_c", NULL); /* Native call 0x48eaa5. */
            DEATHSTARRESCUEC_LDATA = level;                      /* Native store 0x48eaaf. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e5150); /* Native 0x48eab6: callbacks[4] = entry 0x4e5150. */
            }
            level = Level_FindByName("deathstarrescue_d", NULL); /* Native call 0x48eac4. */
            LevelFixupSlotPc0087b1d0 = level;                    /* Native store 0x48ead0. */
            level = Level_FindByName("deathstarrescue_e", NULL); /* Native call 0x48ead5. */
            DEATHSTARRESCUEE_LDATA = level;                      /* Native store 0x48eae1. */
            level = Level_FindByName("deathstarescape_a", NULL); /* Native call 0x48eae6. */
            DEATHSTARESCAPEA_LDATA = level;                      /* Native store 0x48eaf0. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e5460); /* Native 0x48eaf7: callbacks[3] = entry 0x4e5460. */
            }
            level = Level_FindByName("deathstarescape_b", NULL); /* Native call 0x48eb05. */
            DEATHSTARESCAPEB_LDATA = level;                      /* Native store 0x48eb0f. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e5170); /* Native 0x48eb16: callbacks[1] = entry 0x4e5170. */
                STUBBED_NAMED(FixUpLevels_Callback_4e5240); /* Native 0x48eb23: callbacks[3] = entry 0x4e5240. */
                STUBBED_NAMED(FixUpLevels_Callback_4e5380); /* Native 0x48eb30: callbacks[4] = entry 0x4e5380. */
                STUBBED_NAMED(FixUpLevels_Callback_4e53e0); /* Native 0x48eb3c: callbacks[5] = entry 0x4e53e0. */
            }
            level = Level_FindByName("deathstarescape_c", NULL); /* Native call 0x48eb4a. */
            DEATHSTARESCAPEC_LDATA = level;                      /* Native store 0x48eb54. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e54d0); /* Native 0x48eb5b: callbacks[1] = entry 0x4e54d0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e5540); /* Native 0x48eb68: callbacks[3] = entry 0x4e5540. */
                STUBBED_NAMED(FixUpLevels_Callback_4e5520); /* Native 0x48eb75: callbacks[2] = entry 0x4e5520. */
            }
            level = Level_FindByName("deathstarescape_d", NULL); /* Native call 0x48eb83. */
            DEATHSTARESCAPED_LDATA = level;                      /* Native store 0x48eb8d. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e55c0); /* Native 0x48eb94: callbacks[3] = entry 0x4e55c0. */
            }
            level = Level_FindByName("deathstarbattle_a", NULL); /* Native call 0x48eba2. */
            DEATHSTARBATTLEA_LDATA = level;                      /* Native store 0x48ebae. */
            level = Level_FindByName("deathstarbattle_b", NULL); /* Native call 0x48ebb3. */
            DEATHSTARBATTLEB_LDATA = level;                      /* Native store 0x48ebbf. */
            level = Level_FindByName("deathstarbattle_c", NULL); /* Native call 0x48ebc4. */
            DEATHSTARBATTLEC_LDATA = level;                      /* Native store 0x48ebce. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e5630); /* Native 0x48ebd5: callbacks[4] = entry 0x4e5630. */
            }
            level = Level_FindByName("deathstarbattle_d", NULL); /* Native call 0x48ebe3. */
            DEATHSTARBATTLED_LDATA = level;                      /* Native store 0x48ebed. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e5b30); /* Native 0x48ebf4: callbacks[1] = entry 0x4e5b30. */
                STUBBED_NAMED(FixUpLevels_Callback_4e5ba0); /* Native 0x48ec00: callbacks[3] = entry 0x4e5ba0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e5ea0); /* Native 0x48ec0d: callbacks[5] = entry 0x4e5ea0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e5b40); /* Native 0x48ec1a: callbacks[2] = entry 0x4e5b40. */
            }
            level = Level_FindByName("hothbattle_a", NULL); /* Native call 0x48ec28. */
            HOTHBATTLEA_LDATA = level;                      /* Native store 0x48ec32. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e88c0); /* Native 0x48ec39: callbacks[1] = entry 0x4e88c0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e89f0); /* Native 0x48ec45: callbacks[2] = entry 0x4e89f0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8a40); /* Native 0x48ec52: callbacks[3] = entry 0x4e8a40. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8a50); /* Native 0x48ec5f: callbacks[5] = entry 0x4e8a50. */
            }
            level = Level_FindByName("hothbattle_b", NULL); /* Native call 0x48ec6d. */
            LevelFixupSlotPc0087b200 = level;               /* Native store 0x48ec79. */
            level = Level_FindByName("hothbattle_c", NULL); /* Native call 0x48ec7e. */
            LevelFixupSlotPc0087b204 = level;               /* Native store 0x48ec88. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e8ad0); /* Native 0x48ec8f: callbacks[1] = entry 0x4e8ad0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8c50); /* Native 0x48ec9b: callbacks[2] = entry 0x4e8c50. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8cd0); /* Native 0x48eca8: callbacks[3] = entry 0x4e8cd0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8d10); /* Native 0x48ecb5: callbacks[5] = entry 0x4e8d10. */
            }
            level = Level_FindByName("hothbattle_d", NULL); /* Native call 0x48ecc3. */
            LevelFixupSlotPc0087b208 = level;               /* Native store 0x48eccf. */
            level = Level_FindByName("hothbattle_e", NULL); /* Native call 0x48ecd4. */
            LevelFixupSlotPc0087b20c = level;               /* Native store 0x48ecde. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e9800); /* Native 0x48ece5: callbacks[1] = entry 0x4e9800. */
                STUBBED_NAMED(FixUpLevels_Callback_4e9ef0); /* Native 0x48ecf1: callbacks[3] = entry 0x4e9ef0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8d40); /* Native 0x48ecfe: callbacks[5] = entry 0x4e8d40. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8d50); /* Native 0x48ed0b: callbacks[6] = entry 0x4e8d50. */
            }
            level = Level_FindByName("HothBattle_Outro", NULL); /* Native call 0x48ed1c. */
            LevelFixupSlotPc0087b210 = level;                   /* Native store 0x48ed28. */
            level = Level_FindByName("hothescape_a", NULL);     /* Native call 0x48ed2d. */
            HOTHESCAPEA_LDATA = level;                          /* Native store 0x48ed37. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e6360); /* Native 0x48ed3e: callbacks[1] = entry 0x4e6360. */
                STUBBED_NAMED(FixUpLevels_Callback_4e6390); /* Native 0x48ed4a: callbacks[3] = entry 0x4e6390. */
                STUBBED_NAMED(FixUpLevels_Callback_4e6380); /* Native 0x48ed57: callbacks[2] = entry 0x4e6380. */
            }
            level = Level_FindByName("hothescape_b", NULL); /* Native call 0x48ed65. */
            LevelFixupSlotPc0087b218 = level;               /* Native store 0x48ed6f. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e63b0); /* Native 0x48ed76: callbacks[1] = entry 0x4e63b0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e6470); /* Native 0x48ed83: callbacks[3] = entry 0x4e6470. */
                STUBBED_NAMED(FixUpLevels_Callback_4e6410); /* Native 0x48ed8f: callbacks[2] = entry 0x4e6410. */
            }
            level = Level_FindByName("hothescape_c", NULL); /* Native call 0x48ed9d. */
            HOTHESCAPEC_LDATA = level;                      /* Native store 0x48eda7. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e64d0); /* Native 0x48edae: callbacks[1] = entry 0x4e64d0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e6500); /* Native 0x48edbb: callbacks[3] = entry 0x4e6500. */
                STUBBED_NAMED(FixUpLevels_Callback_4e6520); /* Native 0x48edc8: callbacks[4] = entry 0x4e6520. */
                STUBBED_NAMED(FixUpLevels_Callback_4e64f0); /* Native 0x48edd4: callbacks[2] = entry 0x4e64f0. */
            }
            level = Level_FindByName("hothescape_d", NULL); /* Native call 0x48ede2. */
            LevelFixupSlotPc0087b220 = level;               /* Native store 0x48edec. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e6540); /* Native 0x48edf3: callbacks[1] = entry 0x4e6540. */
                STUBBED_NAMED(FixUpLevels_Callback_4e6570); /* Native 0x48ee00: callbacks[3] = entry 0x4e6570. */
                STUBBED_NAMED(FixUpLevels_Callback_4e6560); /* Native 0x48ee0d: callbacks[2] = entry 0x4e6560. */
            }
            level = Level_FindByName("AsteroidChase_A", NULL); /* Native call 0x48ee1b. */
            LevelFixupSlotPc0087b224 = level;                  /* Native store 0x48ee25. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e6f40); /* Native 0x48ee2c: callbacks[1] = entry 0x4e6f40. */
                STUBBED_NAMED(FixUpLevels_Callback_4e6f50); /* Native 0x48ee38: callbacks[2] = entry 0x4e6f50. */
                STUBBED_NAMED(FixUpLevels_Callback_4e6f60); /* Native 0x48ee45: callbacks[3] = entry 0x4e6f60. */
            }
            level = Level_FindByName("AsteroidChase_B", NULL); /* Native call 0x48ee53. */
            LevelFixupSlotPc0087b228 = level;                  /* Native store 0x48ee5d. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e6f70); /* Native 0x48ee64: callbacks[1] = entry 0x4e6f70. */
                STUBBED_NAMED(FixUpLevels_Callback_4e7080); /* Native 0x48ee71: callbacks[2] = entry 0x4e7080. */
                STUBBED_NAMED(FixUpLevels_Callback_4e7090); /* Native 0x48ee7d: callbacks[3] = entry 0x4e7090. */
            }
            level = Level_FindByName("AsteroidChase_C", NULL); /* Native call 0x48ee8b. */
            ASTEROIDCHASEC_LDATA = level;                      /* Native store 0x48ee95. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e7260); /* Native 0x48ee9c: callbacks[1] = entry 0x4e7260. */
                STUBBED_NAMED(FixUpLevels_Callback_4e73c0); /* Native 0x48eea9: callbacks[2] = entry 0x4e73c0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e73d0); /* Native 0x48eeb6: callbacks[3] = entry 0x4e73d0. */
            }
            level = Level_FindByName("AsteroidChase_D", NULL); /* Native call 0x48eec4. */
            ASTEROIDCHASED_LDATA = level;                      /* Native store 0x48eece. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e7630); /* Native 0x48eed5: callbacks[1] = entry 0x4e7630. */
                STUBBED_NAMED(FixUpLevels_Callback_4e77e0); /* Native 0x48eee1: callbacks[3] = entry 0x4e77e0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e7c60); /* Native 0x48eeee: callbacks[5] = entry 0x4e7c60. */
            }
            level = Level_FindByName("AsteroidChase_Midtro", NULL); /* Native call 0x48eefc. */
            LevelFixupSlotPc0087b234 = level;                       /* Native store 0x48ef08. */
            level = Level_FindByName("dagobah_a", NULL);            /* Native call 0x48ef0d. */
            LevelFixupSlotPc0087b258 = level;                       /* Native store 0x48ef19. */
            level = Level_FindByName("dagobah_d", NULL);            /* Native call 0x48ef1e. */
            LevelFixupSlotPc0087b264 = level;                       /* Native store 0x48ef2a. */
            level = Level_FindByName("dagobah_e", NULL);            /* Native call 0x48ef2f. */
            LevelFixupSlotPc0087b268 = level;                       /* Native store 0x48ef3b. */
            level = Level_FindByName("dagobah_e", NULL);            /* Native call 0x48ef40. */
            LevelFixupSlotPc0087b268 = level;                       /* Native store 0x48ef4a. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e84c0); /* Native 0x48ef51: callbacks[1] = entry 0x4e84c0. */
            }
            level = Level_FindByName("dagobah_b", NULL); /* Native call 0x48ef5f. */
            LevelFixupSlotPc0087b25c = level;            /* Native store 0x48ef69. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e8410); /* Native 0x48ef70: callbacks[1] = entry 0x4e8410. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8420); /* Native 0x48ef7d: callbacks[2] = entry 0x4e8420. */
            }
            level = Level_FindByName("dagobah_c", NULL); /* Native call 0x48ef8b. */
            DAGOBAHC_LDATA = level;                      /* Native store 0x48ef95. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e85b0); /* Native 0x48ef9c: callbacks[1] = entry 0x4e85b0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e85f0); /* Native 0x48efa8: callbacks[6] = entry 0x4e85f0. */
            }
            level = Level_FindByName("CloudCityTrap_A", NULL); /* Native call 0x48efb9. */
            CLOUDCITYTRAPA_LDATA = level;                      /* Native store 0x48efc3. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e7c90); /* Native 0x48efca: callbacks[1] = entry 0x4e7c90. */
                STUBBED_NAMED(FixUpLevels_Callback_4e7d10); /* Native 0x48efd7: callbacks[3] = entry 0x4e7d10. */
                STUBBED_NAMED(FixUpLevels_Callback_4e7cf0); /* Native 0x48efe4: callbacks[2] = entry 0x4e7cf0. */
            }
            level = Level_FindByName("CloudCityTrap_B", NULL); /* Native call 0x48eff2. */
            CLOUDCITYTRAPB_LDATA = level;                      /* Native store 0x48effc. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e7dc0); /* Native 0x48f003: callbacks[1] = entry 0x4e7dc0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e7e10); /* Native 0x48f00f: callbacks[3] = entry 0x4e7e10. */
            }
            level = Level_FindByName("CloudCityTrap_C", NULL); /* Native call 0x48f01d. */
            LevelFixupSlotPc0087b24c = level;                  /* Native store 0x48f027. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e7ed0); /* Native 0x48f02e: callbacks[2] = entry 0x4e7ed0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e7f90); /* Native 0x48f03b: callbacks[3] = entry 0x4e7f90. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8040); /* Native 0x48f048: callbacks[6] = entry 0x4e8040. */
            }
            level = Level_FindByName("CloudCityTrap_Outro", NULL); /* Native call 0x48f059. */
            LevelFixupSlotPc0087b250 = level;                      /* Native store 0x48f065. */
            level = Level_FindByName("cloudcityescape_a", NULL);   /* Native call 0x48f06a. */
            CLOUDCITYESCAPEA_LDATA = level;                        /* Native store 0x48f074. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e8080); /* Native 0x48f07b: callbacks[1] = entry 0x4e8080. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8100); /* Native 0x48f087: callbacks[2] = entry 0x4e8100. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8160); /* Native 0x48f094: callbacks[3] = entry 0x4e8160. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8240); /* Native 0x48f0a1: callbacks[6] = entry 0x4e8240. */
            }
            level = Level_FindByName("cloudcityescape_b", NULL); /* Native call 0x48f0b2. */
            LevelFixupSlotPc0087b23c = level;                    /* Native store 0x48f0be. */
            level = Level_FindByName("cloudcityescape_c", NULL); /* Native call 0x48f0c3. */
            CLOUDCITYESCAPEC_LDATA = level;                      /* Native store 0x48f0cd. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e82b0); /* Native 0x48f0d4: callbacks[1] = entry 0x4e82b0. */
                STUBBED_NAMED(FixUpLevels_Callback_4e8310); /* Native 0x48f0e0: callbacks[3] = entry 0x4e8310. */
            }
            level = Level_FindByName("jabbaspalace_a", NULL); /* Native call 0x48f0ee. */
            JABBASPALACEA_LDATA = level;                      /* Native store 0x48f0f8. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4ebba0); /* Native 0x48f0ff: callbacks[1] = entry 0x4ebba0. */
                STUBBED_NAMED(FixUpLevels_Callback_4ebbc0); /* Native 0x48f10c: callbacks[2] = entry 0x4ebbc0. */
                STUBBED_NAMED(FixUpLevels_Callback_4ebbe0); /* Native 0x48f119: callbacks[3] = entry 0x4ebbe0. */
            }
            level = Level_FindByName("jabbaspalace_b", NULL); /* Native call 0x48f127. */
            JABBASPALACEB_LDATA = level;                      /* Native store 0x48f131. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4ebc50); /* Native 0x48f138: callbacks[1] = entry 0x4ebc50. */
                STUBBED_NAMED(FixUpLevels_Callback_4ebc80); /* Native 0x48f144: callbacks[2] = entry 0x4ebc80. */
            }
            level = Level_FindByName("jabbaspalace_d", NULL); /* Native call 0x48f152. */
            JABBASPALACED_LDATA = level;                      /* Native store 0x48f15c. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4ebca0); /* Native 0x48f163: callbacks[2] = entry 0x4ebca0. */
            }
            level = Level_FindByName("jabbaspalace_e", NULL); /* Native call 0x48f171. */
            JABBASPALACEE_LDATA = level;                      /* Native store 0x48f17b. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4ebcc0); /* Native 0x48f182: callbacks[1] = entry 0x4ebcc0. */
                STUBBED_NAMED(FixUpLevels_Callback_4ebd10); /* Native 0x48f18f: callbacks[2] = entry 0x4ebd10. */
                STUBBED_NAMED(FixUpLevels_Callback_4ebda0); /* Native 0x48f19c: callbacks[3] = entry 0x4ebda0. */
                STUBBED_NAMED(FixUpLevels_Callback_4ec2f0); /* Native 0x48f1a8: callbacks[6] = entry 0x4ec2f0. */
            }
            level = Level_FindByName("jabbaspalace_Outro", NULL); /* Native call 0x48f1b9. */
            LevelFixupSlotPc0087b2b0 = level;                     /* Native store 0x48f1c5. */
            level = Level_FindByName("endorbattle_a", NULL);      /* Native call 0x48f1ca. */
            ENDORBATTLEA_LDATA = level;                           /* Native store 0x48f1d4. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4ee380); /* Native 0x48f1db: callbacks[1] = entry 0x4ee380. */
                STUBBED_NAMED(FixUpLevels_Callback_4ee3a0); /* Native 0x48f1e8: callbacks[3] = entry 0x4ee3a0. */
            }
            level = Level_FindByName("endorbattle_b", NULL);      /* Native call 0x48f1f6. */
            ENDORBATTLEB_LDATA = level;                           /* Native store 0x48f202. */
            level = Level_FindByName("endorbattle_c", NULL);      /* Native call 0x48f207. */
            ENDORBATTLEC_LDATA = level;                           /* Native store 0x48f213. */
            level = Level_FindByName("endorbattle_d", NULL);      /* Native call 0x48f218. */
            ENDORBATTLED_LDATA = level;                           /* Native store 0x48f224. */
            level = Level_FindByName("DeathStar2Battle_A", NULL); /* Native call 0x48f229. */
            DEATHSTAR2BATTLEA_LDATA = level;                      /* Native store 0x48f233. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e9f30); /* Native 0x48f23a: callbacks[4] = entry 0x4e9f30. */
            }
            level = Level_FindByName("DeathStar2Battle_B", NULL); /* Native call 0x48f248. */
            DEATHSTAR2BATTLEB_LDATA = level;                      /* Native store 0x48f254. */
            level = Level_FindByName("DeathStar2Battle_D", NULL); /* Native call 0x48f259. */
            DEATHSTAR2BATTLED_LDATA = level;                      /* Native store 0x48f263. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4e9f50); /* Native 0x48f26a: callbacks[1] = entry 0x4e9f50. */
                STUBBED_NAMED(FixUpLevels_Callback_4ea110); /* Native 0x48f277: callbacks[3] = entry 0x4ea110. */
            }
            level = Level_FindByName("DeathStar2Battle_E", NULL); /* Native call 0x48f285. */
            DEATHSTAR2BATTLEE_LDATA = level;                      /* Native store 0x48f28f. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4ee430); /* Native 0x48f2a0: callbacks[1] = entry 0x4ee430. */
            }
            level = Level_FindByName("DeathStar2Battle_F", NULL); /* Native call 0x48f2bf. */
            DEATHSTAR2BATTLEF_LDATA = level;                      /* Native store 0x48f2c9. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4ee430); /* Native 0x48f2d0: callbacks[1] = entry 0x4ee430. */
            }
            level = Level_FindByName("DeathStar2Battle_G", NULL); /* Native call 0x48f2ef. */
            DEATHSTAR2BATTLEG_LDATA = level;                      /* Native store 0x48f2f9. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4ee430); /* Native 0x48f300: callbacks[1] = entry 0x4ee430. */
            }
            level = Level_FindByName("DeathStar2Battle_Midtro", NULL); /* Native call 0x48f320. */
            LevelFixupSlotPc0087b284 = level;                          /* Native store 0x48f32c. */
            level = Level_FindByName("EmperorFight_A", NULL);          /* Native call 0x48f331. */
            EMPERORFIGHTA_LDATA = level;                               /* Native store 0x48f33b. */
            if (level != NULL) {
                level->callbacks[1] = (void *)EmperorFightA_Init;  /* Native 0x48f343. */
                level->callbacks[2] = (void *)EmperorFightA_Reset; /* Native 0x48f34f. */
                STUBBED_NAMED(FixUpLevels_Callback_4eaf30);        /* Native 0x48f35c: callbacks[3] = entry 0x4eaf30. */
                STUBBED_NAMED(FixUpLevels_Callback_4ebb60);        /* Native 0x48f369: callbacks[6] = entry 0x4ebb60. */
            }
            level = Level_FindByName("SarlaccPit_A", NULL); /* Native call 0x48f37a. */
            SARLACCPITA_LDATA = level;                      /* Native store 0x48f384. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4ec330); /* Native 0x48f38b: callbacks[2] = entry 0x4ec330. */
                STUBBED_NAMED(FixUpLevels_Callback_4ec390); /* Native 0x48f397: callbacks[5] = entry 0x4ec390. */
            }
            level = Level_FindByName("SarlaccPit_B", NULL); /* Native call 0x48f3a5. */
            SARLACCPITB_LDATA = level;                      /* Native store 0x48f3af. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4ec7e0); /* Native 0x48f3b6: callbacks[1] = entry 0x4ec7e0. */
                STUBBED_NAMED(FixUpLevels_Callback_4ecb50); /* Native 0x48f3c3: callbacks[2] = entry 0x4ecb50. */
                STUBBED_NAMED(FixUpLevels_Callback_4ece70); /* Native 0x48f3d0: callbacks[3] = entry 0x4ece70. */
            }
            level = Level_FindByName("SarlaccPit_C", NULL); /* Native call 0x48f3de. */
            SARLACCPITC_LDATA = level;                      /* Native store 0x48f3e8. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4ed6c0);       /* Native 0x48f3ef: callbacks[1] = entry 0x4ed6c0. */
                level->callbacks[2] = (void *)SarlaccPitC_Reset;  /* Native 0x48f3fb. */
                level->callbacks[3] = (void *)SarlaccPitC_Update; /* Native 0x48f408. */
            }
            level = Level_FindByName("lego_city", NULL); /* Native call 0x48f416. */
            LevelFixupSlotPc0087b298 = level;            /* Native store 0x48f420. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4edbb0); /* Native 0x48f427: callbacks[2] = entry 0x4edbb0. */
                STUBBED_NAMED(FixUpLevels_Callback_4edc10); /* Native 0x48f434: callbacks[3] = entry 0x4edc10. */
            }
            level = Level_FindByName("new_town", NULL); /* Native call 0x48f442. */
            LevelFixupSlotPc0087b29c = level;           /* Native store 0x48f44c. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4edfb0); /* Native 0x48f453: callbacks[1] = entry 0x4edfb0. */
                STUBBED_NAMED(FixUpLevels_Callback_4edfe0); /* Native 0x48f45f: callbacks[2] = entry 0x4edfe0. */
                STUBBED_NAMED(FixUpLevels_Callback_4ee0a0); /* Native 0x48f46c: callbacks[3] = entry 0x4ee0a0. */
            }
        } else {
            level = Level_FindByName("batcave_a", NULL);       /* Native call 0x48f47a. */
            LevelFixupSlotPc0087af58 = level;                  /* Native store 0x48f486. */
            HUB_LDATA = level;                                 /* Native store 0x48f48b. */
            level = Level_FindByName("rooftops_a", NULL);      /* Native call 0x48f490. */
            LevelFixupSlotPc0087af5c = level;                  /* Native store 0x48f49c. */
            level = Level_FindByName("fairgroundbad_a", NULL); /* Native call 0x48f4a1. */
            LevelFixupSlotPc0087af60 = level;                  /* Native store 0x48f4ab. */
            if (level != NULL) {
                STUBBED_NAMED(FixUpLevels_Callback_4ee970); /* Native 0x48f4b2: callbacks[1] = entry 0x4ee970. */
                STUBBED_NAMED(FixUpLevels_Callback_4ee990); /* Native 0x48f4bf: callbacks[3] = entry 0x4ee990. */
                STUBBED_NAMED(FixUpLevels_Callback_4eeae0); /* Native 0x48f4cb: callbacks[5] = entry 0x4eeae0. */
            }
        }
    }
    for (index = 0, level = LDataList; index < LEVELCOUNT; ++index, ++level) {
        if ((level->flags & 0x400) != 0 || level == STATUS_LDATA) {
            STUBBED_NAMED(FixUpLevels_UpdateStatusScreen); /* Native48f4fd: callback[3] = entry486c60. */
            STUBBED_NAMED(FixUpLevels_DrawStatusScreen);   /* Native48f500: callback[6] = entry486200. */
        }
    }
}
