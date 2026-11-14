#ifndef SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART
#define SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/PCInput.h, nu2api/numath/numtx.h, nu2api/nustencilshadows_PC.h,
 * nu2api/nuwin32/d3dCore.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/PCInput.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/d3dCore.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART == 1 && !defined(SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_STOP)
typedef struct PC_D3D9_DEVICE_PC PC_D3D9_DEVICE_PC;

typedef struct PC_D3D9_METHODS_PC {
    void *query_interface;
    void *add_ref;
    void *release;
    void *test_cooperative_level;
    u32(__stdcall *get_available_texture_mem)(PC_D3D9_DEVICE_PC *);
    void *evict_managed_resources;
    void *get_direct3d;
    HRESULT(__stdcall *get_device_caps)(PC_D3D9_DEVICE_PC *, struct GRAPHICS_CAPS_PC *);
    HRESULT(__stdcall *get_display_mode)(PC_D3D9_DEVICE_PC *, u32, void *);
    HRESULT(__stdcall *get_creation_parameters)(PC_D3D9_DEVICE_PC *, void *);
    HRESULT(__stdcall *set_cursor_properties)(PC_D3D9_DEVICE_PC *, u32, u32, PC_TEXTURE_RESOURCE_PC *);
    void(__stdcall *set_cursor_position)(PC_D3D9_DEVICE_PC *, i32, i32, DWORD);
    HRESULT(__stdcall *show_cursor)(PC_D3D9_DEVICE_PC *, BOOL);
    HRESULT(__stdcall *create_additional_swap_chain)(PC_D3D9_DEVICE_PC *, void *, void **);
    HRESULT(__stdcall *get_swap_chain)(PC_D3D9_DEVICE_PC *, u32, void **);
    u32(__stdcall *get_number_of_swap_chains)(PC_D3D9_DEVICE_PC *);
    void *reset;
    void *present;
    void *get_back_buffer;
    HRESULT(__stdcall *get_raster_status)(PC_D3D9_DEVICE_PC *, u32, void *);
    HRESULT(__stdcall *set_dialog_box_mode)(PC_D3D9_DEVICE_PC *, BOOL);
    void(__stdcall *set_gamma_ramp)(PC_D3D9_DEVICE_PC *, u32, u32, const void *);
    void(__stdcall *get_gamma_ramp)(PC_D3D9_DEVICE_PC *, u32, void *);
    void *create_texture;
    HRESULT(__stdcall *create_volume_texture)
    (PC_D3D9_DEVICE_PC *, u32, u32, u32, u32, u32, i32, i32, PC_TEXTURE_RESOURCE_PC **, HANDLE *);
    HRESULT(__stdcall *create_cube_texture)
    (PC_D3D9_DEVICE_PC *, u32, u32, u32, i32, i32, PC_TEXTURE_RESOURCE_PC **, HANDLE *);
    HRESULT(__stdcall *create_vertex_buffer)
    (PC_D3D9_DEVICE_PC *, u32, u32, u32, i32, PC_BUFFER_RESOURCE_PC **, HANDLE *);
    HRESULT(__stdcall *create_index_buffer)
    (PC_D3D9_DEVICE_PC *, u32, u32, i32, i32, PC_BUFFER_RESOURCE_PC **, HANDLE *);
    void *create_render_target;
    void *create_depth_stencil_surface;
    HRESULT(__stdcall *update_surface)
    (PC_D3D9_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC *, const RECT *, PC_TEXTURE_RESOURCE_PC *, const POINT *);
    HRESULT(__stdcall *update_texture)(PC_D3D9_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC *, PC_TEXTURE_RESOURCE_PC *);
    HRESULT(__stdcall *get_render_target_data)(PC_D3D9_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC *, PC_TEXTURE_RESOURCE_PC *);
    HRESULT(__stdcall *get_front_buffer_data)(PC_D3D9_DEVICE_PC *, u32, PC_TEXTURE_RESOURCE_PC *);
    HRESULT(__stdcall *stretch_rect)
    (PC_D3D9_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC *, const RECT *, PC_TEXTURE_RESOURCE_PC *, const RECT *, i32);
    HRESULT(__stdcall *color_fill)(PC_D3D9_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC *, const RECT *, u32);
    HRESULT(__stdcall *create_offscreen_plain_surface)
    (PC_D3D9_DEVICE_PC *, u32, u32, i32, i32, PC_TEXTURE_RESOURCE_PC **, HANDLE *);
    void *set_render_target;
    HRESULT(__stdcall *get_render_target)(PC_D3D9_DEVICE_PC *, u32, PC_TEXTURE_RESOURCE_PC **);
    HRESULT(__stdcall *set_depth_stencil_surface)(PC_D3D9_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC *);
    void *get_depth_stencil_surface;
    void *begin_scene;
    HRESULT(__stdcall *end_scene)(PC_D3D9_DEVICE_PC *);
    void *clear;
    HRESULT(__stdcall *set_transform)(PC_D3D9_DEVICE_PC *, i32, const NUMTX *);
    HRESULT(__stdcall *get_transform)(PC_D3D9_DEVICE_PC *, i32, NUMTX *);
    HRESULT(__stdcall *multiply_transform)(PC_D3D9_DEVICE_PC *, i32, const NUMTX *);
    HRESULT(__stdcall *set_viewport)(PC_D3D9_DEVICE_PC *, const PC_VIEWPORT_PC *);
    void *get_viewport;
    HRESULT(__stdcall *set_material)(PC_D3D9_DEVICE_PC *, const void *);
    HRESULT(__stdcall *get_material)(PC_D3D9_DEVICE_PC *, void *);
    HRESULT(__stdcall *set_light)(PC_D3D9_DEVICE_PC *, u32, const void *);
    HRESULT(__stdcall *get_light)(PC_D3D9_DEVICE_PC *, u32, void *);
    HRESULT(__stdcall *light_enable)(PC_D3D9_DEVICE_PC *, u32, BOOL);
    HRESULT(__stdcall *get_light_enable)(PC_D3D9_DEVICE_PC *, u32, BOOL *);
    HRESULT(__stdcall *set_clip_plane)(PC_D3D9_DEVICE_PC *, u32, const f32 *);
    HRESULT(__stdcall *get_clip_plane)(PC_D3D9_DEVICE_PC *, u32, f32 *);
    HRESULT(__stdcall *set_render_state)(PC_D3D9_DEVICE_PC *, i32, u32);
    HRESULT(__stdcall *get_render_state)(PC_D3D9_DEVICE_PC *, i32, u32 *);
    HRESULT(__stdcall *create_state_block)(PC_D3D9_DEVICE_PC *, i32, void **);
    void *begin_state_block;
    HRESULT(__stdcall *end_state_block)(PC_D3D9_DEVICE_PC *, void **);
    HRESULT(__stdcall *set_clip_status)(PC_D3D9_DEVICE_PC *, const void *);
    HRESULT(__stdcall *get_clip_status)(PC_D3D9_DEVICE_PC *, void *);
    HRESULT(__stdcall *get_texture)(PC_D3D9_DEVICE_PC *, u32, void **);
    HRESULT(__stdcall *set_texture)(PC_D3D9_DEVICE_PC *, u32, void *);
    HRESULT(__stdcall *get_texture_stage_state)(PC_D3D9_DEVICE_PC *, u32, i32, u32 *);
    HRESULT(__stdcall *set_texture_stage_state)(PC_D3D9_DEVICE_PC *, u32, i32, u32);
    HRESULT(__stdcall *get_sampler_state)(PC_D3D9_DEVICE_PC *, u32, i32, u32 *);
    HRESULT(__stdcall *set_sampler_state)(PC_D3D9_DEVICE_PC *, u32, i32, u32);
    HRESULT(__stdcall *validate_device)(PC_D3D9_DEVICE_PC *, u32 *);
    HRESULT(__stdcall *set_palette_entries)(PC_D3D9_DEVICE_PC *, u32, const PALETTEENTRY *);
    HRESULT(__stdcall *get_palette_entries)(PC_D3D9_DEVICE_PC *, u32, PALETTEENTRY *);
    HRESULT(__stdcall *set_current_texture_palette)(PC_D3D9_DEVICE_PC *, u32);
    HRESULT(__stdcall *get_current_texture_palette)(PC_D3D9_DEVICE_PC *, u32 *);
    HRESULT(__stdcall *set_scissor_rect)(PC_D3D9_DEVICE_PC *, const RECT *);
    HRESULT(__stdcall *get_scissor_rect)(PC_D3D9_DEVICE_PC *, RECT *);
    void *set_software_vertex_processing;
    BOOL(__stdcall *get_software_vertex_processing)(PC_D3D9_DEVICE_PC *);
    HRESULT(__stdcall *set_n_patch_mode)(PC_D3D9_DEVICE_PC *, f32);
    void *get_n_patch_mode;
    HRESULT(__stdcall *draw_primitive)(PC_D3D9_DEVICE_PC *, i32, u32, u32);
    HRESULT(__stdcall *draw_indexed_primitive)(PC_D3D9_DEVICE_PC *, i32, i32, u32, u32, u32, u32);
    void *draw_primitive_up;
    void *draw_indexed_primitive_up;
    HRESULT(__stdcall *process_vertices)
    (PC_D3D9_DEVICE_PC *, u32, u32, u32, PC_BUFFER_RESOURCE_PC *, PC_TEXTURE_RESOURCE_PC *, u32);
    HRESULT(__stdcall *create_vertex_declaration)
    (PC_D3D9_DEVICE_PC *, const PC_VERTEX_ELEMENT_PC *, PC_TEXTURE_RESOURCE_PC **);
    HRESULT(__stdcall *set_vertex_declaration)(PC_D3D9_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC *);
    HRESULT(__stdcall *get_vertex_declaration)(PC_D3D9_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC **);
    void *set_fvf;
    HRESULT(__stdcall *get_fvf)(PC_D3D9_DEVICE_PC *, u32 *);
    HRESULT(__stdcall *create_vertex_shader)(PC_D3D9_DEVICE_PC *, const DWORD *, PC_TEXTURE_RESOURCE_PC **);
    HRESULT(__stdcall *set_vertex_shader)(PC_D3D9_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC *);
    HRESULT(__stdcall *get_vertex_shader)(PC_D3D9_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC **);
    HRESULT(__stdcall *set_vertex_shader_constant_f)(PC_D3D9_DEVICE_PC *, UINT, const f32 *, UINT);
    HRESULT(__stdcall *get_vertex_shader_constant_f)(PC_D3D9_DEVICE_PC *, u32, f32 *, u32);
    HRESULT(__stdcall *set_vertex_shader_constant_i)(PC_D3D9_DEVICE_PC *, u32, const i32 *, u32);
    HRESULT(__stdcall *get_vertex_shader_constant_i)(PC_D3D9_DEVICE_PC *, u32, i32 *, u32);
    HRESULT(__stdcall *set_vertex_shader_constant_b)(PC_D3D9_DEVICE_PC *, u32, const BOOL *, u32);
    HRESULT(__stdcall *get_vertex_shader_constant_b)(PC_D3D9_DEVICE_PC *, u32, BOOL *, u32);
    HRESULT(__stdcall *set_stream_source)(PC_D3D9_DEVICE_PC *, u32, PC_BUFFER_RESOURCE_PC *, u32, u32);
    HRESULT(__stdcall *get_stream_source)(PC_D3D9_DEVICE_PC *, u32, PC_BUFFER_RESOURCE_PC **, u32 *, u32 *);
    HRESULT(__stdcall *set_stream_source_freq)(PC_D3D9_DEVICE_PC *, u32, u32);
    HRESULT(__stdcall *get_stream_source_freq)(PC_D3D9_DEVICE_PC *, u32, u32 *);
    HRESULT(__stdcall *set_indices)(PC_D3D9_DEVICE_PC *, PC_BUFFER_RESOURCE_PC *);
    HRESULT(__stdcall *get_indices)(PC_D3D9_DEVICE_PC *, PC_BUFFER_RESOURCE_PC **);
    HRESULT(__stdcall *create_pixel_shader)(PC_D3D9_DEVICE_PC *, const DWORD *, PC_TEXTURE_RESOURCE_PC **);
    HRESULT(__stdcall *set_pixel_shader)(PC_D3D9_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC *);
    HRESULT(__stdcall *get_pixel_shader)(PC_D3D9_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC **);
    HRESULT(__stdcall *set_pixel_shader_constant_f)(PC_D3D9_DEVICE_PC *, UINT, const f32 *, UINT);
    HRESULT(__stdcall *get_pixel_shader_constant_f)(PC_D3D9_DEVICE_PC *, u32, f32 *, u32);
    HRESULT(__stdcall *set_pixel_shader_constant_i)(PC_D3D9_DEVICE_PC *, u32, const i32 *, u32);
    HRESULT(__stdcall *get_pixel_shader_constant_i)(PC_D3D9_DEVICE_PC *, u32, i32 *, u32);
    HRESULT(__stdcall *set_pixel_shader_constant_b)(PC_D3D9_DEVICE_PC *, u32, const BOOL *, u32);
    HRESULT(__stdcall *get_pixel_shader_constant_b)(PC_D3D9_DEVICE_PC *, u32, BOOL *, u32);
    void *draw_rect_patch;
    void *draw_tri_patch;
    void *delete_patch;
    HRESULT(__stdcall *create_query)(PC_D3D9_DEVICE_PC *, i32, void **);
} PC_D3D9_METHODS_PC;

struct PC_D3D9_DEVICE_PC {
    const PC_D3D9_METHODS_PC *methods;
};
typedef struct GRAPHICS_FACTORY_PC GRAPHICS_FACTORY_PC;
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, create_vertex_buffer) == 0x68);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, create_index_buffer) == 0x6c);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_transform) == 0xb0);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_viewport) == 0xbc);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_render_state) == 0xe4);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_texture) == 0x104);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, stretch_rect) == 0x88);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_texture_stage_state) == 0x10c);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_sampler_state) == 0x114);
STATIC_ASSERT(offsetof(PC_D3D9_DEVICE_PC, methods) == 0);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, create_vertex_declaration) == 0x158);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, create_vertex_shader) == 0x16c);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, create_pixel_shader) == 0x1a8);

STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_vertex_shader_constant_f) == 0x178);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_pixel_shader_constant_f) == 0x1b4);

STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, draw_primitive) == 0x144);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, draw_indexed_primitive) == 0x148);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_stream_source) == 0x190);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_stream_source_freq) == 0x198);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_indices) == 0x1a0);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_vertex_declaration) == 0x15c);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_vertex_shader) == 0x170);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, set_pixel_shader) == 0x1ac);
STATIC_ASSERT(offsetof(PC_D3D9_METHODS_PC, create_query) == 0x1d8);
typedef struct PC_FMV_REALTIME_PC PC_FMV_REALTIME_PC;
#undef SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART
#define SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_STOP
#endif
#endif

#if SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART == 2 && !defined(SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_STOP)

typedef struct GRAPHICS_FACTORY_METHODS_PC {
    void *unknown_000[2];
    ULONG(__stdcall *release)(GRAPHICS_FACTORY_PC *);
    void *register_software;
    void *get_adapter_count;
    HRESULT(__stdcall *get_adapter_identifier)(GRAPHICS_FACTORY_PC *, u32, u32, GRAPHICS_ADAPTER_IDENTIFIER_PC *);
    u32(__stdcall *get_adapter_mode_count)(GRAPHICS_FACTORY_PC *, u32, i32);
    HRESULT(__stdcall *enum_adapter_modes)(GRAPHICS_FACTORY_PC *, u32, i32, u32, GRAPHICS_DISPLAY_MODE_PC *);
    HRESULT(__stdcall *get_adapter_display_mode)(GRAPHICS_FACTORY_PC *, u32, GRAPHICS_DISPLAY_MODE_PC *);
    HRESULT(__stdcall *check_device_type)(GRAPHICS_FACTORY_PC *, u32, i32, i32, i32, BOOL);
    HRESULT(__stdcall *check_device_format)(GRAPHICS_FACTORY_PC *, u32, i32, i32, u32, i32, i32);
    HRESULT(__stdcall *check_multisample)(GRAPHICS_FACTORY_PC *, u32, i32, i32, BOOL, i32, u32 *);
    void *unknown_030[2];
    HRESULT(__stdcall *get_device_caps)(GRAPHICS_FACTORY_PC *, u32, i32, GRAPHICS_CAPS_PC *);
    void *get_adapter_monitor;
    HRESULT(__stdcall *create_device)
    (GRAPHICS_FACTORY_PC *, u32, i32, HWND, u32, GRAPHICS_PRESENT_PARAMETERS_PC *, PC_D3D9_DEVICE_PC **);
} GRAPHICS_FACTORY_METHODS_PC;

struct GRAPHICS_FACTORY_PC {
    const GRAPHICS_FACTORY_METHODS_PC *methods;
};

STATIC_ASSERT(offsetof(GRAPHICS_FACTORY_METHODS_PC, release) == 8);
STATIC_ASSERT(offsetof(GRAPHICS_FACTORY_METHODS_PC, get_device_caps) == 0x38);
STATIC_ASSERT(offsetof(GRAPHICS_FACTORY_METHODS_PC, create_device) == 0x40);
STATIC_ASSERT(offsetof(GRAPHICS_FACTORY_METHODS_PC, get_adapter_identifier) == 0x14);
STATIC_ASSERT(offsetof(GRAPHICS_FACTORY_METHODS_PC, get_adapter_mode_count) == 0x18);
STATIC_ASSERT(offsetof(GRAPHICS_FACTORY_METHODS_PC, enum_adapter_modes) == 0x1c);
STATIC_ASSERT(offsetof(GRAPHICS_FACTORY_METHODS_PC, get_adapter_display_mode) == 0x20);
STATIC_ASSERT(offsetof(GRAPHICS_FACTORY_METHODS_PC, check_device_type) == 0x24);
STATIC_ASSERT(offsetof(GRAPHICS_FACTORY_METHODS_PC, check_multisample) == 0x2c);
STATIC_ASSERT(offsetof(GRAPHICS_FACTORY_PC, methods) == 0x0);
STATIC_ASSERT(sizeof(GRAPHICS_FACTORY_PC) == 4);
STATIC_ASSERT(sizeof(GRAPHICS_FACTORY_METHODS_PC) == 0x44);
#undef SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART
#define SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUFMV_PC_BINK_DSPLAY_BINK_H_STOP
