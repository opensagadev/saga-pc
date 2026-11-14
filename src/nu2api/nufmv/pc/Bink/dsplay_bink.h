/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART
#define SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART == 1 && !defined(SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_STOP)

struct GRAPHICS_ADAPTER_IDENTIFIER;
struct GRAPHICS_CAPS_PC;
struct GRAPHICS_DISPLAY_MODE_PC;
struct GRAPHICS_PRESENT_PARAMETERS_PC;
struct PC_BUFFER_RESOURCE_PC;
struct PC_TEXTURE_RESOURCE_PC;
struct PC_VERTEX_ELEMENT_PC;
struct PC_VIEWPORT_PC;
struct numtx_s;

typedef struct PC_D3D9_DEVICE_PC PC_D3D9_DEVICE_PC;

typedef struct PC_D3D9_METHODS_PC {                                                      // size 0x1dc
    void *query_interface;                                                               // 0x000
    void *add_ref;                                                                       // 0x004
    void *release;                                                                       // 0x008
    void *test_cooperative_level;                                                        // 0x00c
    u32(__stdcall *get_available_texture_mem)(PC_D3D9_DEVICE_PC *);                      // 0x010
    void *evict_managed_resources;                                                       // 0x014
    void *get_direct3d;                                                                  // 0x018
    HRESULT(__stdcall *get_device_caps)(PC_D3D9_DEVICE_PC *, struct GRAPHICS_CAPS_PC *); // 0x01c
    HRESULT(__stdcall *get_display_mode)(PC_D3D9_DEVICE_PC *, u32, void *);              // 0x020
    HRESULT(__stdcall *get_creation_parameters)(PC_D3D9_DEVICE_PC *, void *);            // 0x024
    HRESULT(__stdcall *set_cursor_properties)(PC_D3D9_DEVICE_PC *, u32, u32, struct PC_TEXTURE_RESOURCE_PC *); // 0x028
    void(__stdcall *set_cursor_position)(PC_D3D9_DEVICE_PC *, i32, i32, DWORD);                                // 0x02c
    HRESULT(__stdcall *show_cursor)(PC_D3D9_DEVICE_PC *, BOOL);                                                // 0x030
    HRESULT(__stdcall *create_additional_swap_chain)(PC_D3D9_DEVICE_PC *, void *, void **);                    // 0x034
    HRESULT(__stdcall *get_swap_chain)(PC_D3D9_DEVICE_PC *, u32, void **);                                     // 0x038
    u32(__stdcall *get_number_of_swap_chains)(PC_D3D9_DEVICE_PC *);                                            // 0x03c
    void *reset;                                                                                               // 0x040
    void *present;                                                                                             // 0x044
    void *get_back_buffer;                                                                                     // 0x048
    HRESULT(__stdcall *get_raster_status)(PC_D3D9_DEVICE_PC *, u32, void *);                                   // 0x04c
    HRESULT(__stdcall *set_dialog_box_mode)(PC_D3D9_DEVICE_PC *, BOOL);                                        // 0x050
    void(__stdcall *set_gamma_ramp)(PC_D3D9_DEVICE_PC *, u32, u32, const void *);                              // 0x054
    void(__stdcall *get_gamma_ramp)(PC_D3D9_DEVICE_PC *, u32, void *);                                         // 0x058
    void *create_texture;                                                                                      // 0x05c
    HRESULT(__stdcall *create_volume_texture)                                                                  // 0x060
    (PC_D3D9_DEVICE_PC *, u32, u32, u32, u32, u32, i32, i32, struct PC_TEXTURE_RESOURCE_PC **, HANDLE *);
    HRESULT(__stdcall *create_cube_texture) // 0x064
    (PC_D3D9_DEVICE_PC *, u32, u32, u32, i32, i32, struct PC_TEXTURE_RESOURCE_PC **, HANDLE *);
    HRESULT(__stdcall *create_vertex_buffer) // 0x068
    (PC_D3D9_DEVICE_PC *, u32, u32, u32, i32, struct PC_BUFFER_RESOURCE_PC **, HANDLE *);
    HRESULT(__stdcall *create_index_buffer) // 0x06c
    (PC_D3D9_DEVICE_PC *, u32, u32, i32, i32, struct PC_BUFFER_RESOURCE_PC **, HANDLE *);
    void *create_render_target;         // 0x070
    void *create_depth_stencil_surface; // 0x074
    HRESULT(__stdcall *update_surface)  // 0x078
    (PC_D3D9_DEVICE_PC *, struct PC_TEXTURE_RESOURCE_PC *, const RECT *, struct PC_TEXTURE_RESOURCE_PC *,
     const POINT *);
    HRESULT(__stdcall *update_texture) // 0x07c
    (PC_D3D9_DEVICE_PC *, struct PC_TEXTURE_RESOURCE_PC *, struct PC_TEXTURE_RESOURCE_PC *);
    HRESULT(__stdcall *get_render_target_data) // 0x080
    (PC_D3D9_DEVICE_PC *, struct PC_TEXTURE_RESOURCE_PC *, struct PC_TEXTURE_RESOURCE_PC *);
    HRESULT(__stdcall *get_front_buffer_data)(PC_D3D9_DEVICE_PC *, u32, struct PC_TEXTURE_RESOURCE_PC *); // 0x084
    HRESULT(__stdcall *stretch_rect)                                                                      // 0x088
    (PC_D3D9_DEVICE_PC *, struct PC_TEXTURE_RESOURCE_PC *, const RECT *, struct PC_TEXTURE_RESOURCE_PC *, const RECT *,
     i32);
    HRESULT(__stdcall *color_fill)(PC_D3D9_DEVICE_PC *, struct PC_TEXTURE_RESOURCE_PC *, const RECT *, u32); // 0x08c
    HRESULT(__stdcall *create_offscreen_plain_surface)                                                       // 0x090
    (PC_D3D9_DEVICE_PC *, u32, u32, i32, i32, struct PC_TEXTURE_RESOURCE_PC **, HANDLE *);
    void *set_render_target;                                                                             // 0x094
    HRESULT(__stdcall *get_render_target)(PC_D3D9_DEVICE_PC *, u32, struct PC_TEXTURE_RESOURCE_PC **);   // 0x098
    HRESULT(__stdcall *set_depth_stencil_surface)(PC_D3D9_DEVICE_PC *, struct PC_TEXTURE_RESOURCE_PC *); // 0x09c
    void *get_depth_stencil_surface;                                                                     // 0x0a0
    void *begin_scene;                                                                                   // 0x0a4
    HRESULT(__stdcall *end_scene)(PC_D3D9_DEVICE_PC *);                                                  // 0x0a8
    void *clear;                                                                                         // 0x0ac
    HRESULT(__stdcall *set_transform)(PC_D3D9_DEVICE_PC *, i32, const struct numtx_s *);                 // 0x0b0
    HRESULT(__stdcall *get_transform)(PC_D3D9_DEVICE_PC *, i32, struct numtx_s *);                       // 0x0b4
    HRESULT(__stdcall *multiply_transform)(PC_D3D9_DEVICE_PC *, i32, const struct numtx_s *);            // 0x0b8
    HRESULT(__stdcall *set_viewport)(PC_D3D9_DEVICE_PC *, const struct PC_VIEWPORT_PC *);                // 0x0bc
    void *get_viewport;                                                                                  // 0x0c0
    HRESULT(__stdcall *set_material)(PC_D3D9_DEVICE_PC *, const void *);                                 // 0x0c4
    HRESULT(__stdcall *get_material)(PC_D3D9_DEVICE_PC *, void *);                                       // 0x0c8
    HRESULT(__stdcall *set_light)(PC_D3D9_DEVICE_PC *, u32, const void *);                               // 0x0cc
    HRESULT(__stdcall *get_light)(PC_D3D9_DEVICE_PC *, u32, void *);                                     // 0x0d0
    HRESULT(__stdcall *light_enable)(PC_D3D9_DEVICE_PC *, u32, BOOL);                                    // 0x0d4
    HRESULT(__stdcall *get_light_enable)(PC_D3D9_DEVICE_PC *, u32, BOOL *);                              // 0x0d8
    HRESULT(__stdcall *set_clip_plane)(PC_D3D9_DEVICE_PC *, u32, const f32 *);                           // 0x0dc
    HRESULT(__stdcall *get_clip_plane)(PC_D3D9_DEVICE_PC *, u32, f32 *);                                 // 0x0e0
    HRESULT(__stdcall *set_render_state)(PC_D3D9_DEVICE_PC *, i32, u32);                                 // 0x0e4
    HRESULT(__stdcall *get_render_state)(PC_D3D9_DEVICE_PC *, i32, u32 *);                               // 0x0e8
    HRESULT(__stdcall *create_state_block)(PC_D3D9_DEVICE_PC *, i32, void **);                           // 0x0ec
    void *begin_state_block;                                                                             // 0x0f0
    HRESULT(__stdcall *end_state_block)(PC_D3D9_DEVICE_PC *, void **);                                   // 0x0f4
    HRESULT(__stdcall *set_clip_status)(PC_D3D9_DEVICE_PC *, const void *);                              // 0x0f8
    HRESULT(__stdcall *get_clip_status)(PC_D3D9_DEVICE_PC *, void *);                                    // 0x0fc
    HRESULT(__stdcall *get_texture)(PC_D3D9_DEVICE_PC *, u32, void **);                                  // 0x100
    HRESULT(__stdcall *set_texture)(PC_D3D9_DEVICE_PC *, u32, void *);                                   // 0x104
    HRESULT(__stdcall *get_texture_stage_state)(PC_D3D9_DEVICE_PC *, u32, i32, u32 *);                   // 0x108
    HRESULT(__stdcall *set_texture_stage_state)(PC_D3D9_DEVICE_PC *, u32, i32, u32);                     // 0x10c
    HRESULT(__stdcall *get_sampler_state)(PC_D3D9_DEVICE_PC *, u32, i32, u32 *);                         // 0x110
    HRESULT(__stdcall *set_sampler_state)(PC_D3D9_DEVICE_PC *, u32, i32, u32);                           // 0x114
    HRESULT(__stdcall *validate_device)(PC_D3D9_DEVICE_PC *, u32 *);                                     // 0x118
    HRESULT(__stdcall *set_palette_entries)(PC_D3D9_DEVICE_PC *, u32, const PALETTEENTRY *);             // 0x11c
    HRESULT(__stdcall *get_palette_entries)(PC_D3D9_DEVICE_PC *, u32, PALETTEENTRY *);                   // 0x120
    HRESULT(__stdcall *set_current_texture_palette)(PC_D3D9_DEVICE_PC *, u32);                           // 0x124
    HRESULT(__stdcall *get_current_texture_palette)(PC_D3D9_DEVICE_PC *, u32 *);                         // 0x128
    HRESULT(__stdcall *set_scissor_rect)(PC_D3D9_DEVICE_PC *, const RECT *);                             // 0x12c
    HRESULT(__stdcall *get_scissor_rect)(PC_D3D9_DEVICE_PC *, RECT *);                                   // 0x130
    void *set_software_vertex_processing;                                                                // 0x134
    BOOL(__stdcall *get_software_vertex_processing)(PC_D3D9_DEVICE_PC *);                                // 0x138
    HRESULT(__stdcall *set_n_patch_mode)(PC_D3D9_DEVICE_PC *, f32);                                      // 0x13c
    void *get_n_patch_mode;                                                                              // 0x140
    HRESULT(__stdcall *draw_primitive)(PC_D3D9_DEVICE_PC *, i32, u32, u32);                              // 0x144
    HRESULT(__stdcall *draw_indexed_primitive)(PC_D3D9_DEVICE_PC *, i32, i32, u32, u32, u32, u32);       // 0x148
    void *draw_primitive_up;                                                                             // 0x14c
    void *draw_indexed_primitive_up;                                                                     // 0x150
    HRESULT(__stdcall *process_vertices)                                                                 // 0x154
    (PC_D3D9_DEVICE_PC *, u32, u32, u32, struct PC_BUFFER_RESOURCE_PC *, struct PC_TEXTURE_RESOURCE_PC *, u32);
    HRESULT(__stdcall *create_vertex_declaration) // 0x158
    (PC_D3D9_DEVICE_PC *, const struct PC_VERTEX_ELEMENT_PC *, struct PC_TEXTURE_RESOURCE_PC **);
    HRESULT(__stdcall *set_vertex_declaration)(PC_D3D9_DEVICE_PC *, struct PC_TEXTURE_RESOURCE_PC *);  // 0x15c
    HRESULT(__stdcall *get_vertex_declaration)(PC_D3D9_DEVICE_PC *, struct PC_TEXTURE_RESOURCE_PC **); // 0x160
    void *set_fvf;                                                                                     // 0x164
    HRESULT(__stdcall *get_fvf)(PC_D3D9_DEVICE_PC *, u32 *);                                           // 0x168
    HRESULT(__stdcall *create_vertex_shader)                                                           // 0x16c
    (PC_D3D9_DEVICE_PC *, const DWORD *, struct PC_TEXTURE_RESOURCE_PC **);
    HRESULT(__stdcall *set_vertex_shader)(PC_D3D9_DEVICE_PC *, struct PC_TEXTURE_RESOURCE_PC *);               // 0x170
    HRESULT(__stdcall *get_vertex_shader)(PC_D3D9_DEVICE_PC *, struct PC_TEXTURE_RESOURCE_PC **);              // 0x174
    HRESULT(__stdcall *set_vertex_shader_constant_f)(PC_D3D9_DEVICE_PC *, UINT, const f32 *, UINT);            // 0x178
    HRESULT(__stdcall *get_vertex_shader_constant_f)(PC_D3D9_DEVICE_PC *, u32, f32 *, u32);                    // 0x17c
    HRESULT(__stdcall *set_vertex_shader_constant_i)(PC_D3D9_DEVICE_PC *, u32, const i32 *, u32);              // 0x180
    HRESULT(__stdcall *get_vertex_shader_constant_i)(PC_D3D9_DEVICE_PC *, u32, i32 *, u32);                    // 0x184
    HRESULT(__stdcall *set_vertex_shader_constant_b)(PC_D3D9_DEVICE_PC *, u32, const BOOL *, u32);             // 0x188
    HRESULT(__stdcall *get_vertex_shader_constant_b)(PC_D3D9_DEVICE_PC *, u32, BOOL *, u32);                   // 0x18c
    HRESULT(__stdcall *set_stream_source)(PC_D3D9_DEVICE_PC *, u32, struct PC_BUFFER_RESOURCE_PC *, u32, u32); // 0x190
    HRESULT(__stdcall *get_stream_source)                                                                      // 0x194
    (PC_D3D9_DEVICE_PC *, u32, struct PC_BUFFER_RESOURCE_PC **, u32 *, u32 *);
    HRESULT(__stdcall *set_stream_source_freq)(PC_D3D9_DEVICE_PC *, u32, u32);             // 0x198
    HRESULT(__stdcall *get_stream_source_freq)(PC_D3D9_DEVICE_PC *, u32, u32 *);           // 0x19c
    HRESULT(__stdcall *set_indices)(PC_D3D9_DEVICE_PC *, struct PC_BUFFER_RESOURCE_PC *);  // 0x1a0
    HRESULT(__stdcall *get_indices)(PC_D3D9_DEVICE_PC *, struct PC_BUFFER_RESOURCE_PC **); // 0x1a4
    HRESULT(__stdcall *create_pixel_shader)                                                // 0x1a8
    (PC_D3D9_DEVICE_PC *, const DWORD *, struct PC_TEXTURE_RESOURCE_PC **);
    HRESULT(__stdcall *set_pixel_shader)(PC_D3D9_DEVICE_PC *, struct PC_TEXTURE_RESOURCE_PC *);    // 0x1ac
    HRESULT(__stdcall *get_pixel_shader)(PC_D3D9_DEVICE_PC *, struct PC_TEXTURE_RESOURCE_PC **);   // 0x1b0
    HRESULT(__stdcall *set_pixel_shader_constant_f)(PC_D3D9_DEVICE_PC *, UINT, const f32 *, UINT); // 0x1b4
    HRESULT(__stdcall *get_pixel_shader_constant_f)(PC_D3D9_DEVICE_PC *, u32, f32 *, u32);         // 0x1b8
    HRESULT(__stdcall *set_pixel_shader_constant_i)(PC_D3D9_DEVICE_PC *, u32, const i32 *, u32);   // 0x1bc
    HRESULT(__stdcall *get_pixel_shader_constant_i)(PC_D3D9_DEVICE_PC *, u32, i32 *, u32);         // 0x1c0
    HRESULT(__stdcall *set_pixel_shader_constant_b)(PC_D3D9_DEVICE_PC *, u32, const BOOL *, u32);  // 0x1c4
    HRESULT(__stdcall *get_pixel_shader_constant_b)(PC_D3D9_DEVICE_PC *, u32, BOOL *, u32);        // 0x1c8
    void *draw_rect_patch;                                                                         // 0x1cc
    void *draw_tri_patch;                                                                          // 0x1d0
    void *delete_patch;                                                                            // 0x1d4
    HRESULT(__stdcall *create_query)(PC_D3D9_DEVICE_PC *, i32, void **);                           // 0x1d8
} PC_D3D9_METHODS_PC;

struct PC_D3D9_DEVICE_PC {             // size 0x4
    const PC_D3D9_METHODS_PC *methods; // 0x0
};
typedef struct GRAPHICS_FACTORY_PC GRAPHICS_FACTORY_PC;

typedef struct PC_FMV_REALTIME_PC PC_FMV_REALTIME_PC;
#undef SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART
#define SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_STOP
#endif
#endif

#if SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART == 2 && !defined(SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_STOP)

typedef struct GRAPHICS_FACTORY_METHODS {             // size 0x44
    void *unknown_000[2];                             // 0x00
    ULONG(__stdcall *release)(GRAPHICS_FACTORY_PC *); // 0x08
    void *register_software;                          // 0x0c
    void *get_adapter_count;                          // 0x10
    HRESULT(__stdcall *get_adapter_identifier)        // 0x14
    (GRAPHICS_FACTORY_PC *, u32, u32, struct GRAPHICS_ADAPTER_IDENTIFIER *);
    u32(__stdcall *get_adapter_mode_count)(GRAPHICS_FACTORY_PC *, u32, i32); // 0x18
    HRESULT(__stdcall *enum_adapter_modes)                                   // 0x1c
    (GRAPHICS_FACTORY_PC *, u32, i32, u32, struct GRAPHICS_DISPLAY_MODE_PC *);
    HRESULT(__stdcall *get_adapter_display_mode)(GRAPHICS_FACTORY_PC *, u32, struct GRAPHICS_DISPLAY_MODE_PC *); // 0x20
    HRESULT(__stdcall *check_device_type)(GRAPHICS_FACTORY_PC *, u32, i32, i32, i32, BOOL);                      // 0x24
    HRESULT(__stdcall *check_device_format)(GRAPHICS_FACTORY_PC *, u32, i32, i32, u32, i32, i32);                // 0x28
    HRESULT(__stdcall *check_multisample)(GRAPHICS_FACTORY_PC *, u32, i32, i32, BOOL, i32, u32 *);               // 0x2c
    void *unknown_030[2];                                                                                        // 0x30
    HRESULT(__stdcall *get_device_caps)(GRAPHICS_FACTORY_PC *, u32, i32, struct GRAPHICS_CAPS_PC *);             // 0x38
    void *get_adapter_monitor;                                                                                   // 0x3c
    HRESULT(__stdcall *create_device)                                                                            // 0x40
    (GRAPHICS_FACTORY_PC *, u32, i32, HWND, u32, struct GRAPHICS_PRESENT_PARAMETERS_PC *, PC_D3D9_DEVICE_PC **);
} GRAPHICS_FACTORY_METHODS;

struct GRAPHICS_FACTORY_PC {                 // size 0x4
    const GRAPHICS_FACTORY_METHODS *methods; // 0x0
};

#undef SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART
#define SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_STOP
