#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/animpacket.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/numtx.h"

struct CHARACTER_MODEL_LAYERS;
struct customiser_config_s;
struct NUGSCN_s;
struct WORLDINFO_s;
struct apicharactermodellist_s;
struct custom_category_config_s;
struct custom_piece_config_s;
struct gameobject_s;
struct nufpar_s;

/* One player's customiser selection (GAMESAVE.customiser_start[2]); CUSTOMISER_CONFIG.save points at both. */
typedef struct GAME_CUSTOMISER_START { // size 0x38
    u16 pieces[9];                     // 0x00
    u8 reserved_12[2];                 // 0x12
    char name[32];                     // 0x14
    u8 use_saved_name;                 // 0x34
    u8 reserved_35[3];                 // 0x35
} GAME_CUSTOMISER_START;

/* Customiser animation packets are allocated with a 0x48 stride: an ANIMPACKET and four more bytes. */
typedef struct customiser_anim_packet_s { // size 0x48
    ANIMPACKET packet;                    // 0x00
    u8 reserved_44[4];                     // 0x44
} CUSTOMISER_ANIM_PACKET;

typedef struct customiser_config_s {                        // size 0xc98
    struct custom_category_config_s *categories[9];         // 0x000
    struct custom_piece_config_s *piece_sets[9];            // 0x024
    i32 piece_counts[9];                                    // 0x048
    i16 character_ids[2];                                   // 0x06c
    char display_names[2][128];                             // 0x070
    i32 (*piece_available)(struct custom_piece_config_s *); // 0x170
    GAME_CUSTOMISER_START *save;                            // 0x174
    CUSTOMISER_ANIM_PACKET animation_packets[2];            // 0x178
    i32 model_texture_ids[18];                              // 0x208
    i8 locator_indices[9];                                  // 0x250
    u8 reserved_259[3];                                      // 0x259
    i8 layer_indices[9];                                    // 0x25c
    u8 reserved_265[3];                                      // 0x265
    NUMTX joint_matrices[2][16];                            // 0x268
    u16 *animation_ids_to_load;                             // 0xa68
    u8 animation_active[2];                                 // 0xa6c
    u8 animation_state[2];                                  // 0xa6e
    f32 animation_values[2];                                // 0xa70
    u8 reserved_a78[0x1b0];                                  // 0xa78
    u16 default_pieces[2][10];                              // 0xc28
    f32 locator_x_offsets[9];                               // 0xc50
    f32 locator_y_offsets[9];                               // 0xc74
} CUSTOMISER_CONFIG;

typedef struct custom_accessory_resource_s {        // size 0x20
    struct NUGSCN_s *scene;                         // 0x00
    nuhspecial_s special;                           // 0x04
    i32 original_texture_id;                        // 0x10
    i32 texture_id;                                 // 0x14
    i32 material_index;                             // 0x18
    struct CHARACTER_MODEL_LAYERS *character_model; // 0x1c
} CUSTOM_ACCESSORY_RESOURCE;

#ifdef __cplusplus
extern "C" {
#endif
    void Customiser_Dump(void);
    void Customiser_DumpAccessories(void *customiser);
    void Customiser_RestoreModelTextureIDs(void *customiser);
    extern i32 customiser_quit;
    extern i32 customiser_quit;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern CUSTOM_ACCESSORY_RESOURCE KillPartsAccessories[2][9];
    void Customiser_DumpAll(void *, struct WORLDINFO_s *);
    void Customiser_SaveModelTextureIDs(void *customiser, struct CHARACTER_MODEL_LAYERS *model);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void Customiser_SetAnimsToLoad(struct customiser_config_s *, i32);
    void Customiser_ResetModelTextureIDs(struct customiser_config_s *);

    void Customiser_InitNames(struct customiser_config_s *);
    void Customiser_AddPartAccessories(struct customiser_config_s *, struct gameobject_s *, i32, i32, f32);

    struct customiser_config_s *Customiser_Configure(char *, VARIPTR *, VARIPTR *, i32, i32, i32 (*)(struct custom_piece_config_s *),
                                                     void (*)(struct custom_piece_config_s *, struct nufpar_s *), i32 (*)(char *), void *,
                                                     i16 *);
    void Customiser_CopyDefaultPiecesToSave(struct customiser_config_s *, void *);
    void Customiser_LoadAccessories(struct customiser_config_s *, struct apicharactermodellist_s *);
    void Customiser_Set100PercentPieces(struct customiser_config_s *);
    struct custom_piece_config_s *Customiser_FindPieceByName(struct customiser_config_s *, char *, i32 *, i32 *);
    void Customiser_LoadAll(struct customiser_config_s *, struct WORLDINFO_s *);
    void Customiser_TransformToPanel(struct customiser_config_s *);
    i32 Customiser_NextPieceLeft(struct customiser_config_s *, i32, i32, i32, i32);
    void CustomiserLoadTrace(char *path);
    i32 Customiser_GetIcon(CUSTOMISER_CONFIG *customiser, const GAME_CUSTOMISER_START *save, i32 unused);
    i32 Customiser_NextPieceRight(CUSTOMISER_CONFIG *customiser, i32 index, i32 count, i32 unused, i32 category);
    void CustomiserDrawAccessories(CUSTOMISER_CONFIG *, struct gameobject_s *, NUMTX *);
#ifdef __cplusplus
}
#endif
