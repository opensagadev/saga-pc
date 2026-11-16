#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nuhspecial.h"

struct CHARACTER_MODEL_LAYERS;
struct CUSTOMISER_LAYERS;
struct NUGSCN_s;
struct WORLDINFO_s;
struct apicharactermodellist_s;
struct custom_category_config_s;
struct custom_piece_config_s;
struct custom_save_config_s;
struct gameobject_s;
struct nufpar_s;

typedef struct CUSTOMPIECE_LAYERS { // size 0x28
    u8 unknown_00[0x12];            // 0x00
    u8 layer_flags;                 // 0x12
    u8 unknown_13[0x15];            // 0x13
} CUSTOMPIECE_LAYERS;

typedef struct customiser_config_s {                // size 0xc98
    struct custom_category_config_s *categories[9]; // 0x000
    struct custom_piece_config_s *piece_sets[9];    // 0x024
    i32 piece_counts[9];                            // 0x048
    i16 character_ids[2];                           // 0x06c
    u8 unknown_70[0x100];                           // 0x070
    i32 (*piece_available)(CUSTOMPIECE_LAYERS *);   // 0x170
    struct custom_save_config_s *save;              // 0x174
    u8 unknown_178[0xd8];                           // 0x178
    i8 locator_indices[9];                          // 0x250
    u8 unknown_259[3];                              // 0x259
    i8 layer_indices[9];                            // 0x25c
    u8 unknown_265[0x803];                          // 0x265
    u16 *animation_ids_to_load;                     // 0xa68
    u8 unknown_a6c[0x1bc];                          // 0xa6c
    u16 default_pieces[2][10];                      // 0xc28
    f32 locator_x_offsets[9];                       // 0xc50
    f32 locator_y_offsets[9];                       // 0xc74
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

typedef struct KILLPARTS_ACCESSORY { // size 0x20
    void *scene;                     // 0x00
    nuhspecial_s special;            // 0x04
    u8 unknown_10[0x10];             // 0x10
} KILLPARTS_ACCESSORY;

#ifdef __cplusplus
extern "C" {
#endif
    extern KILLPARTS_ACCESSORY KillPartsAccessories[2][9];
    void Customiser_DumpAll(void *, struct WORLDINFO_s *);
    void Customiser_SaveModelTextureIDs(void *customiser, struct CHARACTER_MODEL_LAYERS *model);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void Customiser_SetAnimsToLoad(struct CUSTOMISER_LAYERS *, i32);
    void Customiser_ResetModelTextureIDs(struct CUSTOMISER_LAYERS *);

    void Customiser_InitNames(struct CUSTOMISER_LAYERS *);
    void Customiser_AddPartAccessories(struct CUSTOMISER_LAYERS *, struct gameobject_s *, i32, i32, f32);

    struct CUSTOMISER_LAYERS *Customiser_Configure(char *, VARIPTR *, VARIPTR *, i32, i32, i32 (*)(CUSTOMPIECE_LAYERS *),
                                                   void (*)(CUSTOMPIECE_LAYERS *, struct nufpar_s *), i32 (*)(char *), void *, i16 *);
    void Customiser_CopyDefaultPiecesToSave(struct CUSTOMISER_LAYERS *, void *);
    void Customiser_LoadAccessories(struct CUSTOMISER_LAYERS *, struct apicharactermodellist_s *);
    void Customiser_Set100PercentPieces(struct CUSTOMISER_LAYERS *);
    CUSTOMPIECE_LAYERS *Customiser_FindPieceByName(struct CUSTOMISER_LAYERS *, char *, i32 *, i32 *);
    void Customiser_LoadAll(struct CUSTOMISER_LAYERS *, struct WORLDINFO_s *);
    void Customiser_TransformToPanel(struct CUSTOMISER_LAYERS *);
    i32 Customiser_NextPieceLeft(struct CUSTOMISER_LAYERS *, i32, i32, i32, i32);
    void CustomiserLoadTrace(char *path);
    i32 Customiser_GetIcon(struct CUSTOMISER_LAYERS *input, const void *save_input, i32 unused);
    i32 Customiser_NextPieceRight(struct CUSTOMISER_LAYERS *input, i32 index, i32 count, i32 unused, i32 category);
    void CustomiserDrawAccessories(struct CUSTOMISER_LAYERS *, struct gameobject_s *, NUMTX *);
#ifdef __cplusplus
}
#endif
