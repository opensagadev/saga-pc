#include <stdio.h>
#include "saga.h"
#include <string.h>
#include <math.h>

typedef struct visibility_scene_reader_pc_s {
    u8 unknown_00[0x1c];
    i32 instance_count;
    u8 unknown_20[0x100];
    VISIBILITY_RESULT_READER_PC result;
} VISIBILITY_SCENE_READER_PC;
typedef struct visibility_occlusion_grid_pc_s {
    u32 unknown_00;
    i32 width;
    f32 min_x, max_x, min_z, max_z;
    f32 scale_x, scale_z;
    i32 mode;
    u32 output_size;
    u32 unknown_28;
    u8 **cells;
} VISIBILITY_OCCLUSION_GRID_PC;
STATIC_ASSERT(offsetof(VISIBILITY_SCENE_READER_PC, instance_count) == 0x1c);
STATIC_ASSERT(offsetof(VISIBILITY_SCENE_READER_PC, result) == 0x120);
STATIC_ASSERT(sizeof(VISIBILITY_OCCLUSION_GRID_PC) == 0x30);
STATIC_ASSERT(offsetof(VISIBILITY_OCCLUSION_GRID_PC, min_x) == 8);
STATIC_ASSERT(offsetof(VISIBILITY_OCCLUSION_GRID_PC, scale_x) == 0x18);
STATIC_ASSERT(offsetof(VISIBILITY_OCCLUSION_GRID_PC, mode) == 0x20);
STATIC_ASSERT(offsetof(VISIBILITY_OCCLUSION_GRID_PC, output_size) == 0x24);
STATIC_ASSERT(offsetof(VISIBILITY_OCCLUSION_GRID_PC, cells) == 0x2c);
extern i32 VisibilityEvaluateEnabledPc;
extern i32 VisibilityOcclusionEnabledPc;
extern i32 VisibilityInstanceTreeEnabledPc;
extern i32 VisibilityCameraLockPc;
extern i32 VisibilityLoadedOcclusionPc;
extern i32 VisibilityUsingOcclusionPc;
extern i32 VisibilityCurrentViewBoxPc;
/* Unsized reference only: no inferred allocation or emitted backing. */

/* Genuine target5d8fb0 body remains held; this declaration supplies no source. */
void __cdecl NuVisiInstTree(VISIBILITY_RESULT_READER_PC *, NUGSCN *);

void __cdecl NuCameraLock(i32 lock) {
    if (lock != NuPortalCameraSelectPc && NuPortalCameraSelectPc == 0)
        NuPortalRestoreCameraPc = global_camera;
    if (lock != 0) {
        NuPortalDebugCameraPc = global_camera;
        NuCameraSet(&NuPortalRestoreCameraPc);
    }
    NuPortalCameraSelectPc = lock;
}

static i32 VisibilityFloorPc(f32 value) {
    i32 integer = (i32)value;
    return (f32)integer > value ? integer - 1 : integer;
}

void __cdecl NuVisiOcclusion(VISIBILITY_RESULT_READER_PC *result) {
    VISIBILITY_OCCLUSION_GRID_PC *grid = (VISIBILITY_OCCLUSION_GRID_PC *)result->occlusion_data;
    f32 x = global_camera.mtx.m30;
    f32 z;
    f32 grid_x, grid_z;
    i32 column, row, cell_index;
    u8 *volatile *cell_slot;
    u8 *cell, *output, *end, *input;
    if (x < grid->min_x || x > grid->max_x)
        return;
    z = global_camera.mtx.m32;
    if (z < grid->min_z || z > grid->max_z)
        return;
    grid_z = (z - grid->min_z) * grid->scale_z;
    row = VisibilityFloorPc(grid_z);
    grid_x = (x - grid->min_x) * grid->scale_x;
    column = VisibilityFloorPc(grid_x);
    cell_index = row * grid->width + column;
    cell_slot = grid->cells + cell_index;
    cell = *cell_slot;
    result->instance_tree_bits = cell;
    VisibilityUsingOcclusionPc = cell != NULL;
    if (grid->mode != 1 || cell == NULL)
        return;
    output = VISIBILITY_OCCLUSION_OUTPUT_PC;
    result->instance_tree_bits = output;
    if (cell_index == VisibilityCurrentViewBoxPc)
        return;
    VisibilityCurrentViewBoxPc = cell_index;
    end = output + grid->output_size;
    input = *cell_slot;
    while (output < end) {
        u8 command = *input++;
        u32 length = command >> 2;
        u8 kind = command & 3;
        if (kind > 1) {
            PCByteCopy_candidate(output, input, length);
            input += length;
        } else {
            memset(output, (u8)-kind, length);
        }
        output += length;
    }
}

void NuHtmlBitmap(char *filename, i32 width, i32 height, char *caption, char *source) {
    char tag[256];
    NuHtmlWrite("<P>");
    if (height != 0 && width != 0) {
        sprintf(tag, "<IMG ALIGN=\"left\" HEIGHT=\"%d\" WIDTH=\"%d\" SRC=\"%s\">", height, width, filename);
    } else {
        sprintf(tag, "<IMG ALIGN=\"left\" SRC=\"%s\">", filename);
    }
    NuHtmlWrite(tag);
    if (caption != NULL) {
        NuHtmlWrite(caption);
    }
    NuHtmlWrite("<BR CLEAR=\"all\"> <P>&nbsp;<P>");
    if (!(source != NULL && NuStrCmp(filename, source) != 0)) {
        return;
    }
    NuFileCopy(filename, source);
}

void NuHtmlTitle(char *title) {
    NuHtmlWrite("<HR><Center><B>");
    NuHtmlWrite(title);
    NuHtmlWrite("</B></Center><HR>");
    NuHtmlWrite("<P><P>");
}

VISIBILITY_RESULT_READER_PC *__cdecl NuVisiEvaluate(NUGSCN *scene, void *context) {
    VISIBILITY_SCENE_READER_PC *view = (VISIBILITY_SCENE_READER_PC *)scene;
    VISIBILITY_RESULT_READER_PC *result;
    if (context == NULL || VisibilityEvaluateEnabledPc == 0)
        return NULL;
    NuCameraLock(VisibilityCameraLockPc);
    result = &view->result;
    result->context = NULL;
    result->instance_tree_bits = NULL;
    result->instance_count = result->source_scene != NULL
                                 ? ((VISIBILITY_SCENE_READER_PC *)result->source_scene)->instance_count
                                 : view->instance_count;
    result->flags &= ~0x15u;
    VisibilityLoadedOcclusionPc = 1;
    if (VisibilityOcclusionEnabledPc != 0 && result->occlusion_data != NULL)
        NuVisiOcclusion(result);
    if (result->portal_marker != NULL && PCPortalsEnabled != 0) {
        NuPortalVisibility(scene);
    } else if (result->instance_tree != NULL && VisibilityInstanceTreeEnabledPc != 0) {
        result->context = context;
        NuVisiInstTree(result, scene);
        result->flags |= 1;
    }
    if (NuPortalCameraSelectPc == 1)
        NuCameraSet(&NuPortalDebugCameraPc);
    return result;
}

typedef struct visibility_tree_node_pc_s {
    u16 bounds_index;
    u16 subtree_instance_count;
    u16 child_count;
    u16 child_first;
    u16 instance_count;
    u16 instance_first;
} VISIBILITY_TREE_NODE_PC;
typedef struct visibility_tree_bounds_pc_s {
    NUVEC center;
    u8 unknown_0c[4];
    NUVEC extent;
    u8 unknown_1c[4];
} VISIBILITY_TREE_BOUNDS_PC;
typedef struct visibility_tree_reader_pc_s {
    u8 unknown_00[8];
    u16 *instance_ids;
    VISIBILITY_TREE_BOUNDS_PC *instance_bounds;
    u8 unknown_10[4];
    VISIBILITY_TREE_NODE_PC *nodes;
    VISIBILITY_TREE_BOUNDS_PC *node_bounds;
    u16 root_count;
    u8 unknown_1e[2];
    u16 *root_indices;
    f32 *root_far_distance;
} VISIBILITY_TREE_READER_PC;
typedef struct visibility_extent_scratch_pc_s {
    NUVEC xyz;
    f32 unknown_w;
} VISIBILITY_EXTENT_SCRATCH_PC;
STATIC_ASSERT(sizeof(VISIBILITY_TREE_NODE_PC) == 12);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_NODE_PC, subtree_instance_count) == 2);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_NODE_PC, child_count) == 4);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_NODE_PC, child_first) == 6);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_NODE_PC, instance_count) == 8);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_NODE_PC, instance_first) == 10);
STATIC_ASSERT(sizeof(VISIBILITY_TREE_BOUNDS_PC) == 32);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_BOUNDS_PC, extent) == 16);
STATIC_ASSERT(sizeof(VISIBILITY_TREE_READER_PC) == 40);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_READER_PC, instance_ids) == 8);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_READER_PC, instance_bounds) == 12);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_READER_PC, nodes) == 20);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_READER_PC, node_bounds) == 24);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_READER_PC, root_count) == 28);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_READER_PC, root_indices) == 32);
STATIC_ASSERT(offsetof(VISIBILITY_TREE_READER_PC, root_far_distance) == 36);
STATIC_ASSERT(sizeof(VISIBILITY_EXTENT_SCRATCH_PC) == 16);
STATIC_ASSERT(offsetof(VISIBILITY_EXTENT_SCRATCH_PC, unknown_w) == 12);
extern i32 VisibilityTreeInitialisedPc;
extern VISIBILITY_RESULT_READER_PC *VisibilityCurrentResultPc;
extern NUMTX VisibilityZeroMatrixTemplatePc;
extern NUMTX VisibilityPrimaryPlaneMatrixPc;
extern NUMTX VisibilityAlternatePlaneMatrixPc;
extern NUMTX VisibilityPrimaryWorldMatrixPc;
extern NUMTX VisibilityAlternateWorldMatrixPc;
extern NUMTX VisibilityPrimaryAbsoluteMatrixPc;
extern NUMTX VisibilityAlternateAbsoluteMatrixPc;
extern NUPLANE VisibilityDepthPlanePc;
extern NUPLANE VisibilityAbsoluteDepthPlanePc;

extern f32 VisibilityAlternateHeightPc;

/* Native21 copies the complete64-byte zero template; no invented scale helper. */
__declspec(noinline) void __cdecl VisibilityZeroMatrixPc(NUMTX *matrix) {
    *matrix = VisibilityZeroMatrixTemplatePc;
}

static void VisibilityAbsoluteFourPc(NUVEC4 *destination, NUVEC4 *source) {
    destination->x = (f32)fabs(source->x);
    destination->y = (f32)fabs(source->y);
    destination->z = (f32)fabs(source->z);
    destination->w = (f32)fabs(source->w);
}

static void VisibilityTransformFourPc(NUMTX *matrix, NUVEC *point, NUVEC4 *out) {
    out->x = matrix->m10 * point->y + point->x * matrix->m00 + matrix->m20 * point->z + matrix->m30;
    out->y = matrix->m01 * point->x + matrix->m11 * point->y + matrix->m21 * point->z + matrix->m31;
    out->z = matrix->m02 * point->x + matrix->m12 * point->y + matrix->m22 * point->z + matrix->m32;
    out->w = matrix->m03 * point->x + matrix->m13 * point->y + matrix->m23 * point->z + matrix->m33;
}

static void VisibilityTreeInitialisePc(void) {
    f32 near_distance = global_camera.near_clip;
    f32 near_squared, horizontal, vertical, squared, inverse_horizontal, inverse_vertical;
    VisibilityTreeInitialisedPc = 1;
    near_squared = near_distance * near_distance;
    horizontal = zx * near_distance;
    vertical = near_distance * zy;
    squared = horizontal * horizontal + near_squared;
    inverse_horizontal = 1.0f / NuFsqrt(squared);
    squared = vertical * vertical + near_squared;
    inverse_vertical = 1.0f / NuFsqrt(squared);
    VisibilityZeroMatrixPc(&VisibilityPrimaryPlaneMatrixPc);
    VisibilityPrimaryPlaneMatrixPc.m00 = -near_distance * inverse_horizontal;
    VisibilityPrimaryPlaneMatrixPc.m02 = -horizontal * inverse_horizontal;
    VisibilityPrimaryPlaneMatrixPc.m01 = -VisibilityPrimaryPlaneMatrixPc.m00;
    VisibilityPrimaryPlaneMatrixPc.m03 = VisibilityPrimaryPlaneMatrixPc.m02;
    VisibilityPrimaryPlaneMatrixPc.m20 = near_distance * inverse_vertical;
    VisibilityPrimaryPlaneMatrixPc.m22 = -vertical * inverse_vertical;
    VisibilityPrimaryPlaneMatrixPc.m21 = -VisibilityPrimaryPlaneMatrixPc.m20;
    VisibilityPrimaryPlaneMatrixPc.m23 = VisibilityPrimaryPlaneMatrixPc.m22;
    horizontal = VISIBILITY_ALTERNATE_WIDTH_PC * near_distance;
    vertical = near_distance * VisibilityAlternateHeightPc;
    squared = horizontal * horizontal + near_squared;
    inverse_horizontal = 1.0f / NuFsqrt(squared);
    squared = vertical * vertical + near_squared;
    inverse_vertical = 1.0f / NuFsqrt(squared);
    VisibilityZeroMatrixPc(&VisibilityAlternatePlaneMatrixPc);
    VisibilityAlternatePlaneMatrixPc.m00 = -near_distance * inverse_horizontal;
    VisibilityAlternatePlaneMatrixPc.m02 = -horizontal * inverse_horizontal;
    VisibilityAlternatePlaneMatrixPc.m01 = -VisibilityAlternatePlaneMatrixPc.m00;
    VisibilityAlternatePlaneMatrixPc.m03 = VisibilityAlternatePlaneMatrixPc.m02;
    VisibilityAlternatePlaneMatrixPc.m20 = near_distance * inverse_vertical;
    VisibilityAlternatePlaneMatrixPc.m22 = -vertical * inverse_vertical;
    VisibilityAlternatePlaneMatrixPc.m21 = -VisibilityAlternatePlaneMatrixPc.m20;
    VisibilityAlternatePlaneMatrixPc.m23 = VisibilityAlternatePlaneMatrixPc.m22;
}

static void VisibilityTreePreparePc(void) {
    f32 distance;
    NuMtxMulH(&VisibilityPrimaryWorldMatrixPc, &vmtx, &VisibilityPrimaryPlaneMatrixPc);
    NuMtxMulH(&VisibilityAlternateWorldMatrixPc, &vmtx, &VisibilityAlternatePlaneMatrixPc);
    VisibilityDepthPlanePc.a = global_camera.mtx.m20;
    VisibilityDepthPlanePc.b = global_camera.mtx.m21;
    VisibilityDepthPlanePc.c = global_camera.mtx.m22;
    distance = global_camera.mtx.m21 * global_camera.mtx.m31 + global_camera.mtx.m20 * global_camera.mtx.m30 +
               global_camera.mtx.m22 * global_camera.mtx.m32;
    VisibilityDepthPlanePc.d = -distance;
    VisibilityAbsoluteDepthPlanePc.a = (f32)fabs(VisibilityDepthPlanePc.a);
    VisibilityAbsoluteDepthPlanePc.b = (f32)fabs(VisibilityDepthPlanePc.b);
    VisibilityAbsoluteDepthPlanePc.c = (f32)fabs(VisibilityDepthPlanePc.c);
    VisibilityAbsoluteDepthPlanePc.d = 0.0f;
    VisibilityAbsoluteFourPc((NUVEC4 *)&VisibilityPrimaryAbsoluteMatrixPc.m00,
                             (NUVEC4 *)&VisibilityPrimaryWorldMatrixPc.m00);
    VisibilityAbsoluteFourPc((NUVEC4 *)&VisibilityPrimaryAbsoluteMatrixPc.m10,
                             (NUVEC4 *)&VisibilityPrimaryWorldMatrixPc.m10);
    VisibilityAbsoluteFourPc((NUVEC4 *)&VisibilityPrimaryAbsoluteMatrixPc.m20,
                             (NUVEC4 *)&VisibilityPrimaryWorldMatrixPc.m20);
    VisibilityPrimaryAbsoluteMatrixPc.m30 = 0.0f;
    VisibilityPrimaryAbsoluteMatrixPc.m31 = 0.0f;
    VisibilityPrimaryAbsoluteMatrixPc.m32 = 0.0f;
    VisibilityPrimaryAbsoluteMatrixPc.m33 = 0.0f;
    VisibilityAbsoluteFourPc((NUVEC4 *)&VisibilityAlternateAbsoluteMatrixPc.m00,
                             (NUVEC4 *)&VisibilityAlternateWorldMatrixPc.m00);
    VisibilityAbsoluteFourPc((NUVEC4 *)&VisibilityAlternateAbsoluteMatrixPc.m10,
                             (NUVEC4 *)&VisibilityAlternateWorldMatrixPc.m10);
    VisibilityAbsoluteFourPc((NUVEC4 *)&VisibilityAlternateAbsoluteMatrixPc.m20,
                             (NUVEC4 *)&VisibilityAlternateWorldMatrixPc.m20);
    VisibilityAlternateAbsoluteMatrixPc.m30 = 0.0f;
    VisibilityAlternateAbsoluteMatrixPc.m31 = 0.0f;
    VisibilityAlternateAbsoluteMatrixPc.m32 = 0.0f;
    VisibilityAlternateAbsoluteMatrixPc.m33 = 0.0f;
}

/* Held original defect: native three-component extent writer leaves the fourth
 * local component untouched before the four-column clipping comparisons.
 * No zero initialization or substitute four-component provider is invented. */
static i32 VisibilityTreeClipNodePc(NUVEC *center, NUVEC *extent, f32 far_distance, i32 instance) {
    NUVEC4 projected;
    VISIBILITY_EXTENT_SCRATCH_PC projected_extent;
    f32 radius, distance, far_side, near_side;
    radius = extent->y * VisibilityAbsoluteDepthPlanePc.b + extent->x * VisibilityAbsoluteDepthPlanePc.a +
             extent->z * VisibilityAbsoluteDepthPlanePc.c;
    distance = center->y * VisibilityDepthPlanePc.b + center->x * VisibilityDepthPlanePc.a +
               center->z * VisibilityDepthPlanePc.c;
    distance += VisibilityDepthPlanePc.d;
    far_side = distance - far_distance;
    near_side = distance - global_camera.near_clip;
    if (radius < far_side || -radius > near_side)
        return 0;
    VisibilityTransformFourPc(&VisibilityPrimaryWorldMatrixPc, center, &projected);
    NuVecMtxTransform(&projected_extent.xyz, extent, &VisibilityPrimaryAbsoluteMatrixPc);
    if (projected_extent.xyz.x < projected.x || projected_extent.xyz.y < projected.y ||
        projected_extent.xyz.z < projected.z || projected_extent.unknown_w < projected.w)
        return 0;
    if (-projected_extent.xyz.x < projected.x || -projected_extent.xyz.y < projected.y ||
        -projected_extent.xyz.z < projected.z || -projected_extent.unknown_w < projected.w) {
        if (instance == 0)
            return 2;
        VisibilityTransformFourPc(&VisibilityAlternateWorldMatrixPc, center, &projected);
        NuVecMtxTransform(&projected_extent.xyz, extent, &VisibilityAlternateAbsoluteMatrixPc);
        if (projected_extent.xyz.x < projected.x || projected_extent.xyz.y < projected.y ||
            projected_extent.xyz.z < projected.z || projected_extent.unknown_w < projected.w)
            return 0;
        if (-projected_extent.xyz.x < projected.x || -projected_extent.xyz.y < projected.y ||
            -projected_extent.xyz.z < projected.z || -projected_extent.unknown_w < projected.w)
            return 2;
        return 1;
    }
    if (instance != 0)
        return 1;
    if (-radius < far_side)
        return 2;
    return radius > near_side ? 2 : 1;
}

static void VisibilityTreeWalkPc(i32 root_index, VISIBILITY_TREE_READER_PC *tree) {
    VISIBILITY_TREE_NODE_PC *work[96];
    VISIBILITY_TREE_NODE_PC **cursor = work;
    f32 far_distance = tree->root_far_distance[root_index];
    *cursor++ = &tree->nodes[tree->root_indices[root_index]];
    if (far_distance == 0.0f)
        far_distance = global_camera.far_clip;
    do {
        VISIBILITY_TREE_NODE_PC *node = *--cursor;
        VISIBILITY_TREE_BOUNDS_PC *bounds = &tree->node_bounds[node->bounds_index];
        i32 clip = VisibilityTreeClipNodePc(&bounds->center, &bounds->extent, far_distance, 0);
        if (clip == 1) {
            u16 *id = tree->instance_ids + node->instance_first;
            u16 *end = id + node->subtree_instance_count;
            while (id != end) {
                u32 index = *id++;
                ((u8 *)VisibilityCurrentResultPc->context)[index >> 2] |= (u8)(1 << ((index & 3) * 2));
            }
        } else if (clip != 0) {
            VISIBILITY_TREE_NODE_PC *child = tree->nodes + node->child_first;
            VISIBILITY_TREE_NODE_PC *child_end = child + node->child_count;
            u16 *id;
            u16 *end;
            VISIBILITY_TREE_BOUNDS_PC *instance_bounds;
            while (child != child_end)
                *cursor++ = child++;
            id = tree->instance_ids + node->instance_first;
            end = id + node->instance_count;
            instance_bounds = tree->instance_bounds + node->instance_first;
            while (id != end) {
                u32 index = *id++;
                i32 instance_clip =
                    VisibilityTreeClipNodePc(&instance_bounds->center, &instance_bounds->extent, far_distance, 1);
                ((u8 *)VisibilityCurrentResultPc->context)[index >> 2] |= (u8)(instance_clip << ((index & 3) * 2));
                ++instance_bounds;
            }
        }
    } while (cursor != work);
}

void __cdecl NuVisiInstTree(VISIBILITY_RESULT_READER_PC *result, NUGSCN *scene) {
    i32 enabled = result->instance_tree != NULL && VisibilityInstanceTreeEnabledPc != 0;
    i32 root;
    VISIBILITY_TREE_READER_PC *tree;
    (void)scene;
    result->flags = (result->flags & ~0x10u) | (enabled ? 0x10u : 0);
    if ((result->flags & 0x10u) == 0)
        return;
    VisibilityCurrentResultPc = result;
    if (VisibilityTreeInitialisedPc == 0)
        VisibilityTreeInitialisePc();
    VisibilityTreePreparePc();
    tree = (VISIBILITY_TREE_READER_PC *)result->instance_tree;
    for (root = (i32)tree->root_count - 1; root >= 0; --root)
        VisibilityTreeWalkPc(root, tree);
}
