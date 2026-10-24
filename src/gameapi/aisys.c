#define _CRT_SECURE_NO_DEPRECATE // NOLINT(readability-identifier-naming): VC8 CRT selector.
#include "saga.h"
#include <stdio.h>
#include <float.h>
#include <math.h>

extern void __cdecl EdFileWriteUnsignedChar(u8);
extern const char GameAIEveryonePc[];

typedef union AI_ACTION_FLAGS_EF4_PC {
    u32 word;
    struct {
        u32 unused0 : 25;
        u32 no_idle_speed : 1;
        u32 unused26 : 6;
    } bits;
} AI_ACTION_FLAGS_EF4_PC;
STATIC_ASSERT(sizeof(AI_ACTION_FLAGS_EF4_PC) == 4);

enum {
    AIPATHINFO_FLAG_ON_PATH_LOCAL = 0x01,
    AIPATHINFO_FLAG_ROUTE_CHECKED_LOCAL = 0x04,
    AIPATH_CONNECTION_FLAG_RESELECT_ROUTE_LOCAL = 0x98000000u
};

i32 __cdecl AISysSetLevelPath(AISYS_PC *system, char *name) {
    AIPATH_PC *path = NULL;
    i32 index;

    if (system == NULL || system->path_sys == NULL) {
        return 0;
    }
    if (name == NULL) {
        path = system->path_sys->paths[0];
    } else {
        for (index = 0; index < system->path_sys->path_count; ++index) {
            if (NuStrICmp(system->path_sys->paths[index]->name, name) == 0) {
                path = system->path_sys->paths[index];
                break;
            }
        }
    }
    if (path == NULL || system->path_sys->active_path == path) {
        return 0;
    }
    system->path_sys->active_path = path;
    return 1;
}

AILOCATOR_PC *__cdecl AIPathFindLocator(AISYS_PC *system, char *name) {
    i32 index;
    if (system != NULL) {
        for (index = 0; index < system->locator_count; ++index) {
            if (NuStrICmp(system->locators[index].name, name) == 0) {
                return &system->locators[index];
            }
        }
    }
    return NULL;
}

AILOCATORSET_PC *__cdecl AIPathFindLocatorSet(AISYS_PC *system, char *name) {
    i32 index;
    if (system != NULL) {
        for (index = 0; index < system->locator_set_count; ++index) {
            if (NuStrICmp(system->locator_sets[index].name, name) == 0) {
                return &system->locator_sets[index];
            }
        }
    }
    return NULL;
}

AIAREA_PC *__cdecl AISysFindArea(AISYS_PC *system, char *name) {
    i32 index;
    if (system != NULL) {
        for (index = 0; index < system->area_count; ++index) {
            if (NuStrICmp(name, system->areas[index].name) == 0) {
                return &system->areas[index];
            }
        }
    }
    return NULL;
}

AIPATH_PC *__cdecl AISysFindPath(AISYS_PC *system, char *name) {
    i32 index;
    if (system != NULL && system->path_sys != NULL) {
        for (index = 0; index < system->path_sys->path_count; ++index) {
            AIPATH_PC *path = system->path_sys->paths[index];
            if (NuStrICmp(path->name, name) == 0) {
                return path;
            }
        }
    }
    return NULL;
}

__declspec(noinline) AIPATHNODE_PC *__cdecl AIPathFindNode(AISYS_PC *system, AIPATH_PC *path, char *name) {
    i32 index;
    if (path == NULL) {
        if (system == NULL || system->path_sys == NULL || system->path_sys->path_count == 0) {
            return NULL;
        }
        path = system->path_sys->active_path;
        if (path == NULL) {
            return NULL;
        }
    }
    for (index = 0; index < path->node_count; ++index) {
        if (path->nodes[index].name != NULL && NuStrICmp(path->nodes[index].name, name) == 0) {
            return &path->nodes[index];
        }
    }
    return NULL;
}

__declspec(noinline) void *__cdecl AIPAthFindPathCnx(AISYS_PC *system, AIPATH_PC *path, char *from_name, char *to_name,
                                                     i32 *direction) {
    AIPATHNODE_PC *from;
    AIPATHNODE_PC *to;
    i32 to_index;
    i32 index;
    if (path == NULL) {
        if (system == NULL || system->path_sys == NULL || system->path_sys->path_count == 0) {
            return NULL;
        }
        path = system->path_sys->active_path;
    }
    if (path == NULL) {
        return NULL;
    }
    from = AIPathFindNode(system, path, from_name);
    to = AIPathFindNode(system, path, to_name);
    if (to == NULL || from == NULL || from == to) {
        return NULL;
    }
    to_index = (i32)(to - path->nodes);
    for (index = 0; index < from->connection_count; ++index) {
        AIPATHCNX_PC *connection = from->connections[index];
        if (connection->node_indices[0] == to_index) {
            *direction = 1;
            return connection;
        }
        if (connection->node_indices[1] == to_index) {
            *direction = 0;
            return connection;
        }
    }
    return NULL;
}

AIANTINODE_PC *__cdecl AIAntinodeCreate(NUVEC *position, f32 radius) {
    i32 index;
    for (index = 0; index < 64; ++index) {
        AIANTINODE_PC *antinode = &dynamic_antinodes[index];
        if (antinode->enabled == 0) {
            memset(antinode, 0, sizeof(*antinode));
            antinode->enabled = 1;
            antinode->game_flags |= 1;
            antinode->position = *position;
            antinode->radius = radius;
            antinode->min_y = antinode->position.y - default_path_heighttol;
            antinode->max_y = antinode->position.y + default_path_heighttol;
            return antinode;
        }
    }
    return NULL;
}

AIANTINODE_PC *__cdecl AIAntinodeCreateSingleFrame(NUVEC *position, f32 radius) {
    i32 index;
    for (index = 0; index < 64; ++index) {
        AIANTINODE_PC *antinode = &dynamic_antinodes[index];
        if (antinode->enabled == 0) {
            memset(antinode, 0, sizeof(*antinode));
            antinode->enabled = 1;
            antinode->game_flags |= 5;
            antinode->position = *position;
            antinode->radius = radius;
            antinode->min_y = antinode->position.y - default_path_heighttol;
            antinode->max_y = antinode->position.y + default_path_heighttol;
            return antinode;
        }
    }
    return NULL;
}

void __cdecl AIAntinodeDestroy(AIANTINODE_PC *antinode) {
    if ((antinode->game_flags & 1) != 0) {
        memset(antinode, 0, sizeof(*antinode));
    }
}

void __cdecl AIAntinodeCullSingleFrame(void) {
    i32 index;
    for (index = 0; index < 64; ++index) {
        AIANTINODE_PC *antinode = &dynamic_antinodes[index];
        if (antinode->enabled != 0 && (antinode->game_flags & 4) != 0) {
            memset(antinode, 0, sizeof(*antinode));
        }
    }
}

void __cdecl AIAntinodeMove(AIANTINODE_PC *antinode, NUVEC *position, f32 radius, f32 below, f32 above) {
    if (antinode != NULL) {
        antinode->position = *position;
        antinode->radius = radius;
        antinode->min_y = antinode->position.y - below;
        antinode->max_y = antinode->position.y + above;
    }
}

void __cdecl InitFn_APIOBJECTFromObjIDFn(APIOBJECT_FROM_ID_PC callback) {
    APIOBJECTFromObjIDFn = callback;
}

__declspec(noinline) void __cdecl AILocatorSet_CheckLocatorsStillAssigned(AISYS_PC *system,
                                                                          AILOCATORSET_PC *locator_set) {
    i32 index;
    if (locator_set == NULL || APIOBJECTFromObjIDFn == NULL) {
        return;
    }
    for (index = 0; index < locator_set->locator_count; ++index) {
        u8 assignment = locator_set->assigned[index];
        u8 locator_index = 0xff;
        APIOBJECT_PC *object;
        if (assignment == 0x80 || assignment == 0xff) {
            continue;
        }
        object = APIOBJECTFromObjIDFn(assignment);
        if (object != NULL && object->ai != NULL && object->ai->locator != NULL) {
            locator_index = (u8)(object->ai->locator - system->locators);
        }
        if (locator_index != locator_set->locator_entries[index]) {
            locator_set->assigned[index] = 0xff;
        }
    }
}

void __cdecl AILocatorSet_AssignNearestLocator(AISYS_PC *system, AILOCATORSET_PC *locator_set, APIOBJECT_PC *object,
                                               f32 max_range, NUVEC *position, NUVEC *second_position,
                                               f32 off_screen_radius, i32 ignore_assigned) {
    if (object != NULL && locator_set != NULL && object->ai != NULL && position != NULL) {
        f32 nearest_distance = max_range > 0.0f ? max_range * max_range : FLT_MAX;
        NUVEC difference;
        i32 nearest_index = -1;
        i32 index;
        if (ignore_assigned != 0) {
            AILocatorSet_CheckLocatorsStillAssigned(system, locator_set);
        }
        for (index = 0; index < locator_set->locator_count; ++index) {
            AILOCATOR_PC *locator;
            f32 first_distance;
            if (ignore_assigned != 0 && locator_set->assigned[index] != 0xff) {
                continue;
            }
            locator = &system->locators[locator_set->locator_entries[index]];
            if (off_screen_radius != 0.0f &&
                NuCameraClipTestSphere(&locator->position, off_screen_radius, &NuMtxSetIdentityTemplate) == 0) {
                continue;
            }
            first_distance = NuVecDistSqr(position, &locator->position, &difference);
            if (first_distance < nearest_distance) {
                nearest_distance = first_distance;
                nearest_index = index;
            }
            if (second_position != NULL) {
                f32 second_distance = NuVecDistSqr(second_position, &locator->position, &difference);
                if (second_distance < nearest_distance) {
                    nearest_distance = second_distance;
                    nearest_index = index;
                }
            }
        }
        if (nearest_index != -1) {
            object->ai->locator = &system->locators[locator_set->locator_entries[nearest_index]];
            locator_set->assigned[nearest_index] = object->object_id;
        }
    }
}

void __cdecl AILocatorSet_AssignFurthestLocator(AISYS_PC *system, AILOCATORSET_PC *locator_set, APIOBJECT_PC *object,
                                                f32 max_range, NUVEC *position, NUVEC *second_position,
                                                f32 off_screen_radius, i32 ignore_assigned) {
    if (object != NULL && locator_set != NULL && object->ai != NULL) {
        NUVEC difference;
        f32 furthest_distance = 0.0f;
        i32 furthest_index = -1;
        i32 index;
        if (ignore_assigned != 0) {
            AILocatorSet_CheckLocatorsStillAssigned(system, locator_set);
        }
        for (index = 0; index < locator_set->locator_count; ++index) {
            AILOCATOR_PC *locator;
            f32 distance;
            if (ignore_assigned != 0 && locator_set->assigned[index] != 0xff) {
                continue;
            }
            locator = &system->locators[locator_set->locator_entries[index]];
            if (off_screen_radius != 0.0f &&
                NuCameraClipTestSphere(&locator->position, off_screen_radius, &NuMtxSetIdentityTemplate) == 0) {
                continue;
            }
            distance = NuVecDistSqr(position, &locator->position, &difference);
            if (distance > furthest_distance) {
                if (second_position != NULL) {
                    f32 second_distance = NuVecDistSqr(second_position, &locator->position, &difference);
                    if (!(second_distance > furthest_distance)) {
                        continue;
                    }
                    if (second_distance < distance) {
                        distance = second_distance;
                    }
                }
                if (max_range == 0.0f || max_range * max_range > distance) {
                    furthest_distance = distance;
                    furthest_index = index;
                }
            }
        }
        if (furthest_index != -1) {
            object->ai->locator = &system->locators[locator_set->locator_entries[furthest_index]];
            locator_set->assigned[furthest_index] = object->object_id;
        }
    }
}

void __cdecl AILocatorSet_AssignRandomLocator(AISYS_PC *system, AILOCATORSET_PC *locator_set, APIOBJECT_PC *object,
                                              f32 max_range, NUVEC *position, f32 off_screen_radius,
                                              i32 ignore_assigned) {
    f32 max_distance;
    NUVEC difference;
    i32 candidate_count = 0;
    i32 selected_candidate;
    i32 candidate_index = 0;
    i32 index;
    if (object == NULL || locator_set == NULL || object->ai == NULL) {
        return;
    }
    max_distance = max_range > 0.0f ? max_range * max_range : FLT_MAX;
    if (ignore_assigned != 0) {
        AILocatorSet_CheckLocatorsStillAssigned(system, locator_set);
    }
    for (index = 0; index < locator_set->locator_count; ++index) {
        AILOCATOR_PC *locator;
        if (ignore_assigned != 0 && locator_set->assigned[index] != 0xff) {
            continue;
        }
        locator = &system->locators[locator_set->locator_entries[index]];
        if (off_screen_radius != 0.0f &&
            NuCameraClipTestSphere(&locator->position, off_screen_radius, &NuMtxSetIdentityTemplate) == 0) {
            continue;
        }
        if (NuVecDistSqr(position, &locator->position, &difference) <= max_distance) {
            ++candidate_count;
        }
    }
    if (candidate_count == 0) {
        return;
    }
    selected_candidate = NuRandInt() % candidate_count;
    for (index = 0; index < locator_set->locator_count; ++index) {
        AILOCATOR_PC *locator;
        if (ignore_assigned != 0 && locator_set->assigned[index] != 0xff) {
            continue;
        }
        locator = &system->locators[locator_set->locator_entries[index]];
        if (NuVecDistSqr(&object->position, &locator->position, &difference) <= max_distance) {
            if (candidate_index == selected_candidate) {
                object->ai->locator = locator;
                locator_set->assigned[index] = object->object_id;
                break;
            }
            ++candidate_index;
        }
    }
}

void DrawAreaBox(NUVEC *position, NUVEC *size, i32 rotation, i32 colour) {
    i32 i;
    f32 local_x[4] = {-size->x, -size->x, size->x, size->x};
    f32 rotated_x[4];
    f32 local_z[4] = {-size->z, size->z, size->z, -size->z};
    f32 rotated_z[4];
    const f32 cosine = NU_COS_LUT(rotation);
    const f32 sine = NU_SIN_LUT(rotation);
    for (i = 0; i < 4; ++i) {
        rotated_x[i] = cosine * local_x[i] + sine * local_z[i];
        rotated_z[i] = cosine * local_z[i] - sine * local_x[i];
    }
#define DRAW_AREA_BOX_EDGE(a, b, y0, y1)                                                                               \
    AiRndrLine3dDbg(position->x + rotated_x[a], y0, position->z + rotated_z[a], position->x + rotated_x[b], y1,        \
                    position->z + rotated_z[b], colour)
    DRAW_AREA_BOX_EDGE(0, 1, position->y, position->y);
    DRAW_AREA_BOX_EDGE(1, 2, position->y, position->y);
    DRAW_AREA_BOX_EDGE(2, 3, position->y, position->y);
    DRAW_AREA_BOX_EDGE(3, 0, position->y, position->y);
    DRAW_AREA_BOX_EDGE(0, 0, position->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(1, 1, position->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(2, 2, position->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(3, 3, position->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(0, 1, position->y + size->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(1, 2, position->y + size->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(2, 3, position->y + size->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(3, 0, position->y + size->y, position->y + size->y);
#undef DRAW_AREA_BOX_EDGE
}

void locatorEditorSaveData(AIPATHSYS_PC *path_system) {
    i32 locator_count = 0;
    EDLOCATOR_s *locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators);
    while (locator != NULL) {
        locator->runtime_index = 0xff;
        if (locator->path != NULL) {
            locator->runtime_index = locator_count++;
        }
        locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link);
    }
    EdFileWriteInt(locator_count);
    locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators);
    while (locator != NULL) {
        if (locator->path != NULL) {
            i32 connection_index;
            i32 connection_direction;
            struct AIPATH_PC *path;
            i32 index;
            EdFileWrite(locator->name, 16);
            EdFileWriteFloat(locator->position.x);
            EdFileWriteFloat(locator->position.y);
            EdFileWriteFloat(locator->position.z);
            EdFileWriteShort(locator->direction);
            EdFileWriteChar(locator->path->draw_index);
            connection_index = 0;
            connection_direction = 0;
            path = path_system->paths[locator->path->draw_index];
            for (index = 0; index < path->connection_count; ++index) {
                AIPATHCNX_PC *connection = &path->connections[index];
                i32 first = locator->first_node->index;
                i32 second = locator->second_node->index;
                if (connection->node_indices[0] == first && connection->node_indices[1] == second) {
                    i32 angle;
                    i32 magnitude;
                    connection_index = index;
                    angle = locator->path_angle;
                    magnitude = angle < 0 ? -angle : angle;
                    if (magnitude > 0x3fff) {
                        connection_direction = 1;
                    } else {
                        connection_direction = 0;
                    }
                    break;
                }
                if (connection->node_indices[0] == second && connection->node_indices[1] == first) {
                    i32 angle;
                    i32 magnitude;
                    connection_index = index;
                    angle = locator->path_angle;
                    magnitude = angle < 0 ? -angle : angle;
                    if (magnitude > 0x3fff) {
                        connection_direction = 0;
                    } else {
                        connection_direction = 1;
                    }
                    break;
                }
            }
            EdFileWriteChar(connection_direction);
            EdFileWriteShort(connection_index);
            EdFileWriteFloat(locator->path_fraction);
            EdFileWriteFloat(locator->path_width);
            if (aidata_version > 14) {
                EdFileWriteInt(locator->path_angle);
            }
        }
        locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link);
    }
    if (aidata_version > 17) {
        i32 set_count = 0;
        EDLOCATORSET_s *set = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->locator_sets);
        while (set != NULL) {
            ++set_count;
            set = (EDLOCATORSET_s *)NuLinkedListGetNext(&aieditor->locator_sets, &set->link);
        }
        EdFileWriteInt(set_count);
        set = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->locator_sets);
        while (set != NULL) {
            i32 member_count = 0;
            i32 index = 0;
            while (1) {
                EDLOCATOR_s *member = set->locators[index];
                if (member == NULL) {
                    break;
                }
                if (member->runtime_index != 0xff) {
                    ++member_count;
                }
                ++index;
                if (index == 64) {
                    break;
                }
            }
            EdFileWrite(set->name, 16);
            EdFileWriteInt(member_count);
            if (member_count != 0) {
                index = 0;
                while (1) {
                    EDLOCATOR_s *member = set->locators[index];
                    if (member == NULL) {
                        break;
                    }
                    if (member->runtime_index != 0xff) {
                        EdFileWriteChar(member->runtime_index);
                    }
                    ++index;
                    if (index == 64) {
                        break;
                    }
                }
            }
            set = (EDLOCATORSET_s *)NuLinkedListGetNext(&aieditor->locator_sets, &set->link);
        }
    }
}

extern void __cdecl AiRndrLine3d(NURND_VERTEX3D_PC *, NUMTL *, NUMTX *);

void __cdecl DrawLocator(NUVEC *position, f32 width, i32 direction, i32 colour) {
    NUVEC offset;
    NUVEC centre;
    NUVEC arrow;
    NURND_VERTEX3D_PC vertices[2];
    vertices[0].colour = colour;
    vertices[1].colour = colour;
    centre = *position;
    centre.y += 0.25f;
    offset.x = width;
    offset.y = 0.0f;
    offset.z = 0.0f;
    NuVecRotateY(&offset, &offset, direction);
    NuVecAdd(&vertices[0].position, &centre, &offset);
    NuVecSub(&vertices[1].position, &centre, &offset);
    AiRndrLine3d(vertices, NULL, NULL);
    vertices[0].position.x = centre.x;
    vertices[0].position.y = centre.y - width;
    vertices[0].position.z = centre.z;
    vertices[1].position.x = centre.x;
    vertices[1].position.y = centre.y + width;
    vertices[1].position.z = centre.z;
    AiRndrLine3d(vertices, NULL, NULL);
    offset.x = 0.0f;
    offset.y = 0.0f;
    offset.z = width;
    NuVecRotateY(&offset, &offset, direction);
    NuVecAdd(&vertices[0].position, &centre, &offset);
    NuVecSub(&vertices[1].position, &centre, &offset);
    AiRndrLine3d(vertices, NULL, NULL);
    arrow.x = -offset.z * (f64)0.2f;
    arrow.y = 0.0f;
    arrow.z = offset.x * (f64)0.2f;
    NuVecAdd(&vertices[1].position, &centre, &arrow);
    AiRndrLine3d(vertices, NULL, NULL);
    NuVecSub(&vertices[1].position, &centre, &arrow);
    AiRndrLine3d(vertices, NULL, NULL);
}

void locatorEditorDrawLocators(void) {
    AIEDITOR_RENDER_STATE *const *state = &aieditor;
    EDLOCATOR_s *locator = (EDLOCATOR_s *)NuLinkedListGetHead(&(*state)->locators);
    while (locator != NULL) {
        locator->drawn = 0;
        locator = (EDLOCATOR_s *)NuLinkedListGetNext(&(*state)->locators, &locator->link);
    }
    if ((*state)->current_locator_set != NULL) {
        i32 index;
        for (index = 0; index < 64; ++index) {
            i32 colour;
            locator = (*state)->current_locator_set->locators[index];
            if (locator == NULL)
                break;

            if (locator == (*state)->current_locator) {
                colour = locator == (*state)->nearest_locator ? 0xff0000ff : 0x800000ff;
            } else {
                colour = locator == (*state)->nearest_locator ? -1 : 0x32323232;
            }
            DrawLocator(&locator->position, aiEditor_LocatorWidth, locator->direction, colour);
            locator->drawn = 1;
        }
    }
    locator = (EDLOCATOR_s *)NuLinkedListGetHead(&(*state)->locators);
    while (locator != NULL) {
        if (!locator->drawn) {
            i32 colour;
            if ((*state)->current_locator_set != NULL) {
                if (locator == (*state)->current_locator) {
                    colour = locator == (*state)->nearest_locator ? 0x50 : 0x28;
                } else {
                    colour = locator == (*state)->nearest_locator ? 0x64646464 : 0x32323232;
                }
            } else if (locator == (*state)->current_locator) {
                colour = locator == (*state)->nearest_locator ? 0xff0000ff : 0x800000ff;
            } else {
                colour = locator == (*state)->nearest_locator ? -1 : 0x32323232;
            }
            DrawLocator(&locator->position, aiEditor_LocatorWidth, locator->direction, colour);
            locator->drawn = 1;
        }
        locator = (EDLOCATOR_s *)NuLinkedListGetNext(&(*state)->locators, &locator->link);
    }
}

void pathEditor_UpdateNodesOnPlatforms(void) {
    EDAIPATH_s *path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths);
    while (path != NULL) {
        EDAIPATHNODE_s *node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&path->nodes);
        while (node != NULL) {
            if (NuSpecialExistsFn(&node->special)) {
                NuVecMtxTransform(&node->position, &node->special_position, NuSpecialGetDrawMtx(&node->special));
            }
            node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&path->nodes, &node->link);
        }
        path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path->link);
    }
}

static i32 TestPointPathCheck(NUVEC *point, EDAIPATHNODE_s *first, EDAIPATHNODE_s *second, f32 *fraction, f32 *width,
                              i32 *angle, f32 tolerance) {
    NUVEC horizontal;
    f32 distance;
    NUVEC direction;
    NUVEC relative;
    f32 along;
    NUVEC nearest;
    f32 radius;
    NUVEC radial;
    f32 distance_squared;
    AIPATH_PC path;
    AIPATHCNX_PC connection;
    AIPATHNODE_PC nodes[2];
    AIPATHINFO_PC info;
    memset(&path, 0, sizeof(path));
    memset(&connection, 0, sizeof(connection));
    memset(nodes, 0, sizeof(nodes));
    memset(&info, 0, sizeof(info));
    nodes[0].position = first->position;
    nodes[0].radius = first->radius + tolerance;
    nodes[0].min_height = first->position.y + first->lower_height - tolerance;
    nodes[0].max_height = first->position.y + first->upper_height + tolerance;
    nodes[1].position = second->position;
    nodes[1].radius = second->radius + tolerance;
    nodes[1].min_height = second->position.y + second->lower_height - tolerance;
    nodes[1].max_height = second->position.y + second->upper_height + tolerance;
    path.nodes = nodes;
    path.connections = &connection;
    path.node_count = 2;
    path.connection_count = 1;
    connection.node_indices[0] = 1;
    horizontal = horizontal;
    distance = NuVecXZDist(&first->position, &second->position, &horizontal);
    connection.horizontal_distance = distance != 0.0f ? distance : 0.0001f;
    connection.rotation = (i16)(NuAtan2(horizontal.x, horizontal.z) * 10430.378f);
    if (!WithinConnection(NULL, point, &path, &connection, 1, NULL, 0xff, 0xff, &info, 0.0f, 0)) {
        return 0;
    }

    direction = direction;
    NuVecSub(&direction, &second->position, &first->position);
    distance = NuVecMag(&direction);
    NuVecScale(&direction, &direction, 1.0f / distance);
    relative = relative;
    NuVecSub(&relative, point, &first->position);
    along = NuVecDot(&direction, &relative);
    *fraction = along / distance;
    nearest = nearest;

    if (*fraction <= 0.0f) {
        nearest = first->position;
        radius = first->radius;
    } else if (*fraction >= 1.0f) {
        nearest = second->position;
        radius = second->radius;
    } else {
        NuVecScale(&nearest, &direction, along);
        NuVecAdd(&nearest, &nearest, &first->position);
        radius = second->radius * *fraction + first->radius * (1.0f - *fraction);
    }
    radial = radial;
    distance_squared = NuVecXZDistSqr(&nearest, point, &radial);
    radius += tolerance;
    *angle = (i32)(NuAtan2(direction.x, direction.z) * 10430.378f);
    NuVecRotateY(&relative, &relative, -*angle);
    if (*fraction <= 0.0f) {
        f32 beyond = -*fraction * distance;
        *width = distance_squared - beyond * beyond;
        *width = NuFsqrt(*width) / radius;
    } else if (*fraction >= 1.0f) {
        f32 beyond = (*fraction - 1.0f) * distance;
        *width = distance_squared - beyond * beyond;
        *width = NuFsqrt(*width) / radius;
    } else {
        *width = NuFsqrt(distance_squared) / radius;
    }
    if (relative.x < 0.0f) {
        *width = -*width;
    }
    return 1;
}

void antinodeEditor_UpdateAntiNodesOnPlatforms(void) {
    struct EDANTINODE_s *node;
    NULISTHDR *list = ((NULISTHDR *)(((u8 *)(aieditor)) + 0x42e94));
    NUVEC rotated;
    NUVEC forward = {0.0f, 0.0f, 1.0f};
    for (node = ((EDANTINODE_s *)(NuLinkedListGetHead(list))); node != NULL;
         node = ((EDANTINODE_s *)(NuLinkedListGetNext(((NULISTHDR *)(((u8 *)(aieditor)) + 0x42e94)), &node->link)))) {
        struct numtx_s *matrix;
        if (!NuSpecialExistsFn(&node->special))
            continue;
        matrix = NuSpecialGetDrawMtx(&node->special);
        NuVecMtxTransform(&node->position, &node->special_position, matrix);
        NuVecMtxRotate(&rotated, &forward, matrix);
        node->flags = NuAtan2D(rotated.x, rotated.z);
        node->flags = NuAngAdd(node->flags, node->rotation_offset);
    }
}

void antinodeEditorSaveData(void) {
    struct nulisthdr_s *list;
    struct EDANTINODE_s *head;
    i32 count;
    struct EDANTINODE_s *node;
    if (aidata_version <= 12)
        return;
    list = ((NULISTHDR *)(((u8 *)(aieditor)) + 0x42e94));
    head = ((EDANTINODE_s *)(NuLinkedListGetHead(list)));
    if (head == NULL) {
        EdFileWriteInt(0);
        return;
    }
    count = 0;
    for (node = head; node != NULL;
         node = ((EDANTINODE_s *)(NuLinkedListGetNext(((NULISTHDR *)(((u8 *)(aieditor)) + 0x42e94)), &node->link))))
        ++count;
    EdFileWriteInt(count);
    for (node = ((EDANTINODE_s *)(NuLinkedListGetHead(((NULISTHDR *)(((u8 *)(aieditor)) + 0x42e94))))); node != NULL;
         node = ((EDANTINODE_s *)(NuLinkedListGetNext(((NULISTHDR *)(((u8 *)(aieditor)) + 0x42e94)), &node->link)))) {
        char *name;
        if (node->type == 2)
            node->radius = NuFsqrt(node->base_radius * node->base_radius + node->base_height * node->base_height);
        else if (node->type == 1)
            node->radius = node->base_radius > node->base_height ? node->base_radius : node->base_height;
        EdFileWriteFloat(node->position.x);
        EdFileWriteFloat(node->position.y);
        EdFileWriteFloat(node->position.z);
        EdFileWriteFloat(node->radius);
        EdFileWriteFloat(node->position.y + node->lower_height);
        EdFileWriteFloat(node->position.y + node->upper_height);
        if (aidata_version > 14) {
            EdFileWriteInt(node->flags);
            EdFileWriteFloat(node->base_radius);
            EdFileWriteFloat(node->base_height);
            EdFileWriteChar(node->type);
        } else {
            EdFileWriteChar(0);
        }
        EdFileWriteChar(0);
        EdFileWriteChar(0);
        EdFileWriteChar(node->game_flags);
        name = NuSpecialExistsFn(&node->special) ? NuSpecialGetName(&node->special) : NULL;
        if (name == NULL) {
            EdFileWriteChar(0);
        } else {
            i32 length = strlen(name) + 1;
            EdFileWriteChar(length);
            EdFileWrite(name, length);
            EdFileWriteFloat(node->special_position.x);
            EdFileWriteFloat(node->special_position.y);
            EdFileWriteFloat(node->special_position.z);
            if (aidata_version > 14)
                EdFileWriteInt(node->rotation_offset);
        }
    }
}

void __cdecl AISysCharacterSetPath(AIPACKET_PC *packet, AIPATH_PC *path) {
    AIPACKET_PC *path_packet = packet;
    u8 route;

    if (path_packet->path_info.path == path) {
        return;
    }

    path_packet->path_info.connection = NULL;
    path_packet->path_info.direction = 0;
    path_packet->path_info.path_index = 0xff;
    path_packet->path_info.game_flags = 0;
    path_packet->path_info.next_check = 0;
    path_packet->path_info.flags = 0;
    path_packet->path_info.padding = 0;
    path_packet->path_info.distance = 0.0f;
    path_packet->path_info.width = 0.0f;
    path_packet->unknown_124 = 0xffff;
    path_packet->available_routes = 0;
    path_packet->current_route = 0xff;
    path_packet->next_route = 0;
    path_packet->path_info.path = path;
    path_packet->current_path_node = NULL;

    if (path == NULL) {
        return;
    }

    for (route = 0; route < path->route_count; ++route) {
        AIPATHROUTE_PC *route_info = &path->routes[route];
        u64 route_mask = (u64)1 << route;

        if ((route_info->character_mask[0] & path_packet->route_mask_low) != 0 ||
            (route_info->character_mask[1] & path_packet->route_mask_high) != 0) {
            path_packet->available_routes = (u16)(path_packet->available_routes | (u16)route_mask);
        }
    }
}

void __cdecl AISysFindRoute(AIPACKET_PC *packet) {
    AIPACKET_PC *path_packet = packet;
    AIPATH_PC *path = (AIPATH_PC *)path_packet->path_info.path;
    AIPATHCNX_PC *connection = (AIPATHCNX_PC *)path_packet->path_info.connection;
    u8 route_count = path->route_count;
    u16 valid_routes;
    u8 route;
    u8 first_route;

    if (route_count == 0 || connection == NULL) {
        return;
    }

    valid_routes = (u16)(path_packet->available_routes & connection->route_mask);
    if (valid_routes == 0) {
        return;
    }

    route = path_packet->next_route;
    if (route >= route_count) {
        route = 0;
    }
    first_route = route;
    do {
        u64 route_mask = (u64)1 << route;
        if ((valid_routes & (u16)route_mask) != 0) {
            path_packet->current_route = route;
            path_packet->next_route = (u8)(route + 1);
            return;
        }
        ++route;
        if (route >= route_count) {
            route = 0;
        }
    } while (route != first_route);
}

__declspec(noinline) void __cdecl AISysCharacterSetPathCnx(AIPACKET_PC *packet, NUVEC *position,
                                                           AIPATHCNX_PC *connection, i32 direction) {
    AIPACKET_PC *path_packet = packet;
    AIPATH_PC *path;
    u32 connection_flags;

    if (connection == NULL || path_packet->owner == NULL) {
        path_packet->path_info.connection = connection;
        path_packet->current_route = 0xff;
        path_packet->next_route = 0;
        return;
    }

    if (path_packet->path_info.connection == connection && path_packet->path_info.direction == direction &&
        (path_packet->path_info.flags & (AIPATHINFO_FLAG_ON_PATH_LOCAL | AIPATHINFO_FLAG_ROUTE_CHECKED_LOCAL)) !=
            AIPATHINFO_FLAG_ON_PATH_LOCAL) {
        return;
    }

    connection_flags = connection->traversal_flags[direction];
    if (connection_flags != 0) {
        if ((connection_flags & AIPATH_CONNECTION_FLAG_RESELECT_ROUTE_LOCAL) != 0) {
            AISysFindRoute(packet);
            return;
        }
        if ((path_packet->capabilities & connection_flags) == 0) {
            return;
        }
    }

    if (path_packet->path_info.connection != connection || path_packet->path_info.direction != direction) {
        AIPATHNODE_PC *node;
        NUVEC delta;
        NUVEC rotated;

        path_packet->path_info.direction = (u8)direction;
        path_packet->path_update_flag = 0;
        path_packet->path_info.connection = connection;
        path = (AIPATH_PC *)path_packet->path_info.path;
        node = &path->nodes[connection->node_indices[0]];
        delta.x = position->x - node->position.x;
        delta.y = position->y - node->position.y;
        delta.z = position->z - node->position.z;
        NuVecRotateY(&rotated, &delta, -(NUANG)connection->rotation);
        path_packet->path_info.distance = rotated.z / connection->horizontal_distance;
        path_packet->path_info.width = rotated.x;
        path_packet->path_info.flags &= (u8)~AIPATHINFO_FLAG_ROUTE_CHECKED_LOCAL;
    }

    if ((path_packet->path_info.flags & (AIPATHINFO_FLAG_ON_PATH_LOCAL | AIPATHINFO_FLAG_ROUTE_CHECKED_LOCAL)) ==
        AIPATHINFO_FLAG_ON_PATH_LOCAL) {
        if (path_packet->current_route != 0xff) {
            u64 route_mask = (u64)1 << path_packet->current_route;
            if ((connection->route_mask & (u16)route_mask) == 0) {
                path_packet->current_route = 0xff;
            }
        }
        if (path_packet->current_route == 0xff) {
            AISysFindRoute(packet);
        }
        path_packet->path_info.flags |= AIPATHINFO_FLAG_ROUTE_CHECKED_LOCAL;
    }
}

void __cdecl AIPathNodeUpdatePos(void *context, AIPATH_PC *path, AIPATHNODE_PC *node) {
    i32 node_index;
    u8 node_bit;
    u8 *updated_byte;
    i32 connection_index;

    if (node->has_special == 0) {
        return;
    }

    node_index = (i32)(node - path->nodes);
    updated_byte = &path->updated_node_bits[node_index >> 3];
    node_bit = (u8)(1u << (node_index & 7));
    if ((*updated_byte & node_bit) != 0) {
        return;
    }
    *updated_byte |= node_bit;

    if ((node->runtime_flags & 0x04) != 0) {
        if (NuSpecialGetVisibilityFn(&node->special_handle) == 0) {
            return;
        }

        node->runtime_flags &= (u8)~0x04;
        for (connection_index = 0; connection_index < node->connection_count; ++connection_index) {
            AIPATHCNX_PC *connection = node->connections[connection_index];
            u8 other_index = connection->node_indices[0];
            AIPATHNODE_PC *other;

            if (other_index == (u8)node_index) {
                other_index = connection->node_indices[1];
            }
            other = &path->nodes[other_index];
            if ((other->runtime_flags & 0x04) == 0) {
                connection->traversal_flags[0] &= 0xefffffffu;
                connection->traversal_flags[1] &= 0xefffffffu;
            }
        }
    } else if (NuSpecialGetVisibilityFn(&node->special_handle) == 0 &&
               (AISysSpecialNodeCallback == NULL || AISysSpecialNodeCallback(context, &node->special_handle) == 0)) {
        node->runtime_flags |= 0x04;
        for (connection_index = 0; connection_index < node->connection_count; ++connection_index) {
            AIPATHCNX_PC *connection = node->connections[connection_index];
            connection->traversal_flags[0] |= 0x10000000u;
            connection->traversal_flags[1] |= 0x10000000u;
        }
        return;
    }

    {
        NUVEC old_position = node->position;
        NUMTX *draw_matrix = NuSpecialGetDrawMtx(&node->special_handle);
        NuVecMtxTransform(&node->position, &node->special_position, draw_matrix);
        node->min_height = node->position.y + node->min_height_offset;
        node->max_height = node->position.y + node->max_height_offset;

        for (connection_index = 0; connection_index < node->connection_count; ++connection_index) {
            AIPATHCNX_PC *connection = node->connections[connection_index];
            u8 other_index = connection->node_indices[0];
            AIPATHNODE_PC *other;
            NUVEC delta;

            if (other_index == (u8)node_index) {
                other_index = connection->node_indices[1];
            }
            other = &path->nodes[other_index];

            if (connection->node_indices[0] == (u8)node_index)
                connection->horizontal_distance = NuVecXZDist(&other->position, &node->position, &delta);
            else
                connection->horizontal_distance = NuVecXZDist(&node->position, &other->position, &delta);
            connection->rotation = (i16)(NuAtan2(delta.x, delta.z) * AIPathAngularScale);

            if (connection->max_horizontal_distance != 0.0f) {
                if (connection->horizontal_distance >
                    node->radius + connection->max_horizontal_distance + other->radius) {
                    connection->traversal_flags[0] |= 0x08000000u;
                    connection->traversal_flags[1] |= 0x08000000u;
                } else {
                    connection->traversal_flags[0] &= 0xf7ffffffu;
                    connection->traversal_flags[1] &= 0xf7ffffffu;
                }
            }
        }

        if (old_position.x == node->position.x && old_position.y == node->position.y &&
            old_position.z == node->position.z) {
            NUINSTANIM_TIME_VIEW *animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&node->special_handle);
            if (animation == NULL || (animation->flags & 1) == 0 || animation->tfactor == 0.0f) {
                node->runtime_flags &= (u8)~0x02;
                return;
            }
        }
        node->runtime_flags |= 0x02;
    }
}

__declspec(noinline) i32 __cdecl WithinConnection(void *context, NUVEC *position, AIPATH_PC *path,
                                                  AIPATHCNX_PC *connection, i32 reserved_check,
                                                  AIPATHCNX_PC *previous_connection, i32 route_index,
                                                  i32 reserved_ground, AIPATHINFO_PC *path_info, f32 radius,
                                                  i32 update_once) {
    AIPATHNODE_PC *first;
    AIPATHNODE_PC *second;
    f32 first_radius;
    f32 second_radius;
    i32 narrow = 0;
    NUVEC delta;
    NUVEC local;

    (void)reserved_ground;
    (void)reserved_check;

    if (update_once != 0) {
        if (connection->last_search_checksum == path->search_checksum) {
            return 0;
        }
        connection->last_search_checksum = path->search_checksum;
    }

    if (route_index != 0xff && previous_connection != NULL &&
        ((u64)connection->route_mask & ((u64)1 << route_index)) == 0) {
        AIPATHNODE_PC *shared = NULL;
        u8 first_index = connection->node_indices[0];
        u8 second_index = connection->node_indices[1];

        if (previous_connection->node_indices[0] == first_index) {
            shared = &path->nodes[first_index];
        } else if (previous_connection->node_indices[0] == second_index) {
            shared = &path->nodes[second_index];
        } else if (previous_connection->node_indices[1] == first_index) {
            shared = &path->nodes[first_index];
        } else if (previous_connection->node_indices[1] == second_index) {
            shared = &path->nodes[second_index];
        } else {
            return 0;
        }

        if ((shared->route_boundary_mask & (u16)(1u << route_index)) == 0) {
            return 0;
        }
    }

    first = &path->nodes[connection->node_indices[0]];
    second = &path->nodes[connection->node_indices[1]];
    first_radius = first->radius;
    second_radius = second->radius;
    if (first_radius > radius + AIPathConnectionMargin) {
        first_radius -= radius;
    } else {
        narrow = 1;
    }
    if (second_radius > radius + AIPathConnectionMargin) {
        second_radius -= radius;
    } else {
        narrow = 1;
    }

    if (first->has_special != 0 && (path->updated_node_bits[connection->node_indices[0] >> 3] &
                                    (u8)(1u << (connection->node_indices[0] & 7))) == 0) {
        AIPathNodeUpdatePos(context, path, first);
    }
    if (second->has_special != 0 && (path->updated_node_bits[connection->node_indices[1] >> 3] &
                                     (u8)(1u << (connection->node_indices[1] & 7))) == 0) {
        AIPathNodeUpdatePos(context, path, second);
    }

    delta.x = position->x - first->position.x;
    delta.y = 0.0f;
    delta.z = position->z - first->position.z;
    NuVecRotateY(&local, &delta, -connection->rotation);

    if ((second_radius - first_radius < 0.0f ? first_radius - second_radius : second_radius - first_radius) >
        connection->horizontal_distance) {
        if (second_radius > first_radius) {
            first = second;
            first_radius = second_radius;
        }
        if (first->min_height > position->y || position->y > first->max_height) {
            return 0;
        }
        delta.x = position->x - first->position.x;
        delta.y = 0.0f;
        delta.z = position->z - first->position.z;
        if (first_radius * first_radius < delta.x * delta.x + delta.z * delta.z) {
            return 0;
        }
    } else {
        f32 abs_local_x = local.x < 0.0f ? -local.x : local.x;
        f32 radius_delta = second_radius - first_radius;
        i32 intersection_angle;
        NUVEC wall;

        if (abs_local_x > first_radius && abs_local_x > second_radius) {
            return 0;
        }
        if (local.z < -first_radius || local.z > connection->horizontal_distance + second_radius) {
            return 0;
        }

        intersection_angle = PCPathArcSine(radius_delta / connection->horizontal_distance);
        wall = local;
        if (wall.x < 0.0f) {
            wall.x = -wall.x;
        }
        NuVecRotateY(&wall, &wall, -intersection_angle);

        if (wall.z < 0.0f) {
            if (first->min_height > position->y || position->y > first->max_height) {
                return 0;
            }
            if (first_radius * first_radius < NuVecXZDistSqr(position, &first->position, &delta)) {
                return 0;
            }
        } else {
            u32 trig_index = (u32)(intersection_angle + 0x4000) >> 1;
            f32 end_circle = NuTrigTable[trig_index & 0x7fff] * connection->horizontal_distance;

            if (wall.z > end_circle) {
                if (second->min_height > position->y || position->y > second->max_height) {
                    return 0;
                }
                if (second_radius * second_radius < NuVecXZDistSqr(position, &second->position, &delta)) {
                    return 0;
                }
            } else {
                f32 minimum;
                f32 maximum;

                if (first_radius > local.z) {
                    minimum = first->min_height;
                    maximum = first->max_height;
                } else if (local.z > connection->horizontal_distance - second_radius) {
                    minimum = second->min_height;
                    maximum = second->max_height;
                } else {
                    f32 fraction =
                        (local.z - first_radius) / (connection->horizontal_distance - (first_radius + second_radius));
                    f32 inverse = 1.0f - fraction;
                    minimum = second->min_height * fraction + first->min_height * inverse;
                    maximum = second->max_height * fraction + first->max_height * inverse;
                }
                if (minimum > position->y || position->y > maximum || !(first_radius > wall.x)) {
                    return 0;
                }
            }
        }
    }

    if (path_info != NULL) {
        path_info->direction = 0;
        path_info->connection = connection;
        path_info->flags = (u8)((path_info->flags & (u8)~0x08) | 0x01 | (narrow << 3));
        path_info->distance = local.z / connection->horizontal_distance;
        path_info->width = local.x;
    }
    return 1;
}

i32 Action_CameraCut(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                     i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC target_position;
    NUVEC camera_position;
    NUVEC camera_delta;
    f32 range;
    i32 rotation_x;
    i32 rotation_y;
    char *target_locator;
    char *target_character;
    char *target_object;
    char *camera_locator;
    i32 follow_target;
    i32 borders;
    i32 use_current_camera;
    f32 start;
    f32 end;
    f32 blend_in;
    f32 blend_out;
    char *value;
    NUVEC *target;
    AILOCATOR_PC *locator;
    GAMEOBJECT *object;
    NUSPECIALHANDLE special;
    NUMTX *matrix;
    i32 index;

    target_position.x = 1000000000.0f;
    target_position.y = 1000000000.0f;
    target_position.z = 1000000000.0f;
    camera_position.x = 1000000000.0f;
    camera_position.y = 1000000000.0f;
    camera_position.z = 1000000000.0f;
    camera_delta.x = 0.0f;
    camera_delta.y = 0.0f;
    camera_delta.z = 0.0f;
    if (first_time == 0 || param_count <= 0)
        return 1;

    range = 1.0f;
    rotation_x = 0;
    rotation_y = 0;
    target_locator = NULL;
    target_character = NULL;
    target_object = NULL;
    camera_locator = NULL;
    follow_target = 0;
    borders = 1;
    use_current_camera = 0;
    start = 0.0f;
    end = 1000000000.0f;
    blend_in = 0.0f;
    blend_out = 0.0f;
    for (index = 0; index < param_count; ++index) {
        if (NuStrICmp((char *)AITransferLiteral757a68, params[index]) == 0) {
            GameCameraMakeMiniCut2(NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0);
            return 1;
        } else if (NuStrICmp((char *)AICameraLiteral758090, params[index]) == 0) {
            use_current_camera = 1;
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758084)) != NULL) {
            start = AIParamToFloat(processor, value + 11);
        } else if (NuStrICmp((char *)AICameraLiteral758070, params[index]) == 0) {
            end = 1000000000.0f;
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758064)) != NULL) {
            end = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758054)) != NULL) {
            blend_in = AIParamToFloat(processor, value + 14);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758044)) != NULL) {
            blend_out = AIParamToFloat(processor, value + 15);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758034)) != NULL) {
            target_locator = value + 12;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757874)) != NULL) {
            target_character = value + 14;
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758028)) != NULL) {
            target_object = value + 8;
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758018)) != NULL) {
            camera_locator = value + 12;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757724)) != NULL) {
            range = AIParamToFloat(processor, value + 6);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758010)) != NULL) {
            rotation_y = (i32)AIParamToFloat(processor, value + 5) * (65536 / 360.0f);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758008)) != NULL) {
            rotation_x = -(i32)AIParamToFloat(processor, value + 5) * (65536 / 360.0f);
        } else if (NuStrICmp(params[index], (char *)AICameraLiteral757ffc) == 0) {
            follow_target = 1;
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757ff0)) != NULL) {
            camera_delta.x = AIParamToFloat(processor, value + 10);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fe4)) != NULL) {
            camera_delta.y = AIParamToFloat(processor, value + 10);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fd8)) != NULL) {
            camera_delta.z = AIParamToFloat(processor, value + 10);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fcc)) != NULL) {
            camera_position.x = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fc0)) != NULL) {
            camera_position.y = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fb4)) != NULL) {
            camera_position.z = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fa8)) != NULL) {
            target_position.x = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757f9c)) != NULL) {
            target_position.y = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757f90)) != NULL) {
            target_position.z = AIParamToFloat(processor, value + 9);
        } else if (NuStrICmp((char *)AICameraLiteral757f84, params[index]) == 0) {
            borders = 0;
        }
    }

    if (target_locator != NULL) {
        locator = AIPathFindLocator(system, target_locator);
        if (locator == NULL)
            return 1;
        target = &locator->position;
    } else if (target_character != NULL) {
        if (packet != NULL && packet->owner != NULL &&
            NuStrICmp(target_character, (char *)AITransferLiteral757f7c) == 0)
            object = packet->owner->objptr;
        else
            object = GetNamedGameObject(system, target_character);
        if (object == NULL)
            return 1;
        target = &object->collision_position;
    } else if (target_object != NULL) {
        if (NuSpecialFind(WORLD->current_gscn, &special, target_object, 1) == 0)
            return 1;
        target = NuSpecialGetDrawPos(&special);
        if (target == NULL)
            return 1;
    } else {
        if (target_position.x == 1000000000.0f || target_position.y == 1000000000.0f ||
            target_position.z == 1000000000.0f)
            return 1;
        target = &target_position;
    }

    if (use_current_camera != 0) {
        matrix = NuCameraGetMtx();
        if (matrix != NULL) {
            camera_position.x = matrix->m30;
            camera_position.y = matrix->m31;
            camera_position.z = matrix->m32;
            NuVecAdd(&camera_position, &camera_position, &camera_delta);
            goto camera_ready;
        }
    } else {
        if (camera_position.x != 1000000000.0f && camera_position.y != 1000000000.0f &&
            camera_position.z != 1000000000.0f)
            goto camera_ready;
        if (camera_locator != NULL) {
            locator = AIPathFindLocator(system, camera_locator);
            if (locator != NULL) {
                camera_position = locator->position;
                goto camera_ready;
            }
        }
    }
    camera_position.x = 0.0f;
    camera_position.y = 0.0f;
    camera_position.z = range;
    NuVecRotateX(&camera_position, &camera_position, rotation_x);
    NuVecRotateY(&camera_position, &camera_position, rotation_y);
    NuVecAdd(&camera_position, &camera_position, target);
camera_ready:
    GameCameraMakeMiniCut2(&camera_position, target, 0, start, end, blend_in, blend_out, follow_target, 0, borders);
    return 1;
}

i32 Action_DynamicCameraCut(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC target_position;
    NUVEC camera_position;
    NUVEC camera_offset;
    u32 flags;
    f32 range;
    i32 rotation_x;
    i32 rotation_y;
    i32 rotation_z;
    f32 start_time;
    f32 blend_in_time;
    f32 end_time;
    f32 blend_out_time;
    f32 blend_time;
    f32 hold_time;
    f32 max_time;
    i32 borders;
    i32 use_current_position;
    i8 mode;
    i8 interpolation;
    NUGSPLINE *spline;
    char *target_locator;
    char *target_character;
    char *target_object;
    char *camera_locator;
    char *value;
    NUVEC *target;
    AILOCATOR_PC *locator;
    GAMEOBJECT *object;
    NUSPECIALHANDLE special;
    i32 have_camera_position;
    NUMTX *matrix;
    i32 index;

    target_position.x = 1000000000.0f;
    target_position.y = 1000000000.0f;
    target_position.z = 1000000000.0f;
    camera_position.x = 1000000000.0f;
    camera_position.y = 1000000000.0f;
    camera_position.z = 1000000000.0f;
    camera_offset.x = 0.0f;
    camera_offset.y = 0.0f;
    camera_offset.z = 0.0f;
    if (first_time != 0) {
        flags = 0;
        range = 1.0f;
        rotation_x = 0;
        rotation_y = 0;
        rotation_z = 0;
        start_time = 0.0f;
        blend_in_time = 0.0f;
        end_time = 1000000000.0f;
        blend_out_time = 0.0f;
        blend_time = 0.0f;
        hold_time = 0.0f;
        max_time = 0.0f;
        borders = 1;
        use_current_position = 0;
        mode = -1;
        interpolation = -1;
        spline = NULL;
        target_locator = NULL;
        target_character = NULL;
        target_object = NULL;
        camera_locator = NULL;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp((char *)AICameraLiteral758090, params[index]) == 0) {
                flags |= 0x100;
                use_current_position = 1;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758160)) != NULL) {
                flags |= 0x2000;
                spline = NuSplineFind(WORLD->current_gscn, value + 17);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral75814c)) != NULL) {
                flags |= 0x4000;
                range = AIParamToFloat(processor, value + 17);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758084)) != NULL) {
                flags |= 0x200;
                start_time = AIParamToFloat(processor, value + 11);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758140)) != NULL) {
                flags |= 0x800;
                blend_time = AIParamToFloat(processor, value + 11);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758054)) != NULL) {
                blend_in_time = AIParamToFloat(processor, value + 14);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758044)) != NULL) {
                blend_out_time = AIParamToFloat(processor, value + 15);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758064)) != NULL) {
                flags |= 0x400;
                end_time = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758134)) != NULL) {
                max_time = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758128)) != NULL) {
                flags |= 0x1000;
                hold_time = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758034)) != NULL) {
                target_locator = value + 12;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757874)) != NULL) {
                target_character = value + 14;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758028)) != NULL) {
                target_object = value + 8;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758018)) != NULL) {
                camera_locator = value + 12;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757724)) != NULL) {
                flags |= 1;
                range = AIParamToFloat(processor, value + 6);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral75811c)) != NULL) {
                flags |= 0x20;
                rotation_y = (i32)((i32)AIParamToFloat(processor, value + 10) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758010)) != NULL) {
                flags |= 0x4;
                rotation_y = (i32)((i32)AIParamToFloat(processor, value + 5) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758110)) != NULL) {
                flags |= 0x10;
                rotation_x = (i32)((i32)AIParamToFloat(processor, value + 10) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758008)) != NULL) {
                flags |= 0x2;
                rotation_x = (i32)(-(i32)AIParamToFloat(processor, value + 5) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758104)) != NULL) {
                flags |= 0x40;
                rotation_z = (i32)((i32)AIParamToFloat(processor, value + 10) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral7580fc)) != NULL) {
                flags |= 0x8;
                rotation_z = (i32)(-(i32)AIParamToFloat(processor, value + 5) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fcc)) != NULL) {
                camera_position.x = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fc0)) != NULL) {
                camera_position.y = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fb4)) != NULL) {
                camera_position.z = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fa8)) != NULL) {
                target_position.x = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757f9c)) != NULL) {
                target_position.y = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757f90)) != NULL) {
                target_position.z = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757ff0)) != NULL) {
                camera_offset.x = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fe4)) != NULL) {
                camera_offset.y = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fd8)) != NULL) {
                camera_offset.z = AIParamToFloat(processor, value + 10);
            } else if (NuStrICmp((char *)AICameraLiteral757f84, params[index]) == 0) {
                borders = 0;
            } else if (NuStrICmp((char *)AICameraLiteral7580f0, params[index]) == 0) {
                mode = 0;
            } else if (NuStrICmp((char *)AICameraLiteral7580e4, params[index]) == 0) {
                mode = 1;
            } else if (NuStrICmp((char *)AICameraLiteral7580dc, params[index]) == 0) {
                interpolation = 0;
            } else if (NuStrICmp((char *)AICameraLiteral7580cc, params[index]) == 0) {
                interpolation = 2;
            } else if (NuStrICmp((char *)AICameraLiteral7580c0, params[index]) == 0) {
                interpolation = 1;
            } else if (NuStrICmp((char *)AICameraLiteral7580b0, params[index]) == 0) {
                interpolation = 4;
            } else if (NuStrICmp((char *)AICameraLiteral7580a4, params[index]) == 0) {
                interpolation = 3;
            }
        }
        target = NULL;
        if (target_locator != NULL) {
            locator = AIPathFindLocator(system, target_locator);
            if (locator != NULL)
                target = &locator->position;
        } else if (target_character != NULL) {
            if (packet != NULL && packet->owner != NULL &&
                NuStrICmp(target_character, (char *)AITransferLiteral757f7c) == 0)
                object = packet->owner->objptr;
            else
                object = GetNamedGameObject(system, target_character);
            if (object != NULL)
                target = &object->collision_position;
        } else if (target_object != NULL) {
            if (NuSpecialFind(WORLD->current_gscn, &special, target_object, 1) != 0)
                target = NuSpecialGetDrawPos(&special);
        } else if (target_position.x != 1000000000.0f && target_position.y != 1000000000.0f &&
                   target_position.z != 1000000000.0f) {
            target = &target_position;
        }
        if (target != NULL)
            flags |= 0x80;

        have_camera_position = 0;
        if (use_current_position != 0) {
            matrix = NuCameraGetMtx();
            if (matrix != NULL) {
                camera_position.x = matrix->m30;
                camera_position.y = matrix->m31;
                camera_position.z = matrix->m32;
                have_camera_position = 1;
            }
        } else if (camera_position.x != 1000000000.0f && camera_position.y != 1000000000.0f &&
                   camera_position.z != 1000000000.0f) {
            have_camera_position = 1;
        } else if (camera_locator != NULL) {
            locator = AIPathFindLocator(system, camera_locator);
            if (locator != NULL) {
                camera_position = locator->position;
                have_camera_position = 1;
            }
        }
        if (spline != NULL) {
            PointAlongSpline(spline, 0.0f, &camera_position, NULL, NULL, 0);
            if (mode == -1)
                mode = 2;
        } else if (have_camera_position != 0) {
            flags |= 0x100;
            NuVecAdd(&camera_position, &camera_position, &camera_offset);
            if (mode == -1)
                mode = 1;
        } else {
            if (target != NULL) {
                camera_position.x = 0.0f;
                camera_position.y = 0.0f;
                camera_position.z = range;
                NuVecRotateX(&camera_position, &camera_position, rotation_x);
                NuVecRotateY(&camera_position, &camera_position, rotation_y);
                NuVecAdd(&camera_position, &camera_position, target);
            }
            if (mode == -1)
                mode = 0;
        }
        GameCameraMakeMiniCut3(flags, range, rotation_x, rotation_y, rotation_z, target, 0, &camera_position,
                               start_time, blend_in_time, end_time, blend_out_time, blend_time, hold_time, max_time,
                               borders, spline, mode, interpolation);
    }
    return 1;
}

i32 Action_EndCameraCut(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                        i32 param_count, i32 first_time, f32 elapsed) {
    char *value;
    i32 i;

    if (first_time != 0 && MiniCutCam != 0) {
        for (i = 0; i < param_count; ++i) {
            value = NuStrIStr(params[i], (char *)AICameraLiteral758064);
            if (value != NULL) {
                ObstacleCamEnd = AIParamToFloat(processor, value + 9);
                ObstacleCamEnd += ObstacleCamTime;
            } else {
                value = NuStrIStr(params[i], (char *)AICameraLiteral758044);
                if (value != NULL) {
                    ObstacleCamBlendOutTime = AIParamToFloat(processor, value + 15);
                }
            }
        }
    }
    return 1;
}

f32 __cdecl AIPathNodeDistanceToPathNode(AIPATH_PC *path, i32 start_node, i32 destination_node, i32 route_index,
                                         u32 excluded_route_mask) {
    AIPATHNODE_PC *nodes;
    AIPATHNODE_PC *node;
    AIPATHNODE_PC *destination;
    AIPATHCNX_PC *previous_connection = NULL;
    AIPATHROUTE_PC *route = NULL;
    f32 *cached_distance = NULL;
    f32 distance = 0.0f;
    i32 node_index;

    if (path->route_matrix == NULL) {
        return FLT_MAX;
    }
    if (start_node == destination_node) {
        return 0.0f;
    }
    nodes = path->nodes;
    node = &nodes[start_node];
    if (excluded_route_mask == 0 && route_index == 0xff) {
        if (node->distance_cache_nodes[0] == destination_node) {
            return node->distance_cache[0];
        }
        if (node->distance_cache_nodes[1] == destination_node) {
            return node->distance_cache[1];
        }
        node->distance_cache_nodes[1] = node->distance_cache_nodes[0];
        node->distance_cache_nodes[0] = (u8)destination_node;
        nodes = path->nodes;
        node->distance_cache[1] = node->distance_cache[0];
        cached_distance = &node->distance_cache[0];
        *cached_distance = 0.0f;
    }
    node_index = (i32)(node - nodes);
    if (route_index != 0xff) {
        route = &path->routes[route_index];
        if (((u64)node->route_membership_mask & ((u64)1 << route_index)) == 0) {
            return FLT_MAX;
        }
    }

    destination = &nodes[destination_node];
    while (node != destination) {
        AIPATHCNX_PC *connection;
        i32 connection_index = 0xff;
        if (route != NULL) {
            if (((u64)destination->route_membership_mask & ((u64)1 << route_index)) != 0) {
                u8 from = route->node_routes[node_index];
                if (from < route->route_count && route->node_routes[destination_node] < route->route_count) {
                    connection_index = route->route_nodes[from][route->node_routes[destination_node]];
                } else {
                    STUBBED_NAMED("PC path route-map diagnostic at 0x00512860");
                    return FLT_MAX;
                }
            } else {
                f32 nearest = FLT_MAX;
                i32 index;
                for (index = 0; index < route->exit_node_count; ++index) {
                    f32 first =
                        route->exit_nodes[index] == start_node
                            ? 0.0f
                            : AIPathNodeDistanceToPathNode(path, start_node, route->exit_nodes[index], route_index, 0);
                    f32 second = route->exit_nodes[index] == destination_node
                                     ? 0.0f
                                     : AIPathNodeDistanceToPathNode(path, route->exit_nodes[index], destination_node,
                                                                    0xff, (u32)((u64)1 << route_index));
                    f32 candidate = first != FLT_MAX && second != FLT_MAX ? second + first : FLT_MAX;
                    if (candidate < nearest) {
                        nearest = candidate;
                    }
                }
                if (nearest == FLT_MAX) {
                    if (cached_distance != NULL) {
                        *cached_distance = FLT_MAX;
                    }
                    return FLT_MAX;
                }
                distance += nearest;
                break;
            }
        } else {
            connection_index = path->route_matrix[node_index][destination_node];
        }
        if (connection_index == 0xff) {
            if (cached_distance != NULL) {
                *cached_distance = FLT_MAX;
            }
            return FLT_MAX;
        }
        connection = node->connections[connection_index];
        if (connection == NULL) {
            return FLT_MAX;
        }
        if ((connection->route_mask & excluded_route_mask) != 0) {
            if (cached_distance != NULL) {
                *cached_distance = FLT_MAX;
            }
            return FLT_MAX;
        }
        if (connection == previous_connection) {
            break;
        }
        node_index =
            connection->node_indices[0] == node_index ? connection->node_indices[1] : connection->node_indices[0];
        node = &nodes[node_index];
        distance += connection->distance;
        previous_connection = connection;
    }
    if (cached_distance != NULL) {
        *cached_distance = distance;
    }
    return distance;
}

static void AISysResetPathSearchConnectionChecks(AIPATH_PC *path) {
    AIPATHNODE_PC *node;
    i32 connection_index;
    if (path == NULL || path->nodes == NULL || path->node_count == 0) {
        return;
    }
    ++path->search_checksum;
    if (path->search_checksum == 0) {
        path->search_checksum = 1;
    }
    ++path->search_reset_node;
    if (path->search_reset_node >= path->node_count) {
        path->search_reset_node = 0;
    }
    node = &path->nodes[path->search_reset_node];
    for (connection_index = 0; connection_index < node->connection_count; ++connection_index) {
        node->connections[connection_index]->last_search_checksum = 0;
    }
}

__declspec(noinline) i32 __cdecl AIPathCheckExtents(AIPATH_PC *path, NUVEC *position) {
    if (path->bounds_min.x > position->x || path->bounds_min.y > position->y || path->bounds_min.z > position->z ||
        position->x > path->bounds_max.x || position->y > path->bounds_max.y) {
        return 0;
    }
    return position->z <= path->bounds_max.z;
}

void __cdecl AISysGetPathPosEx(AISYS_PC *system, NUVEC *position, AIPATHINFO_PC *info, AIPATH_PC *path, i32 checks,
                               i32 search_all_paths, f32 *nearest_distance_squared) {
    AIPATHCNX_PC *current;
    AIPATHNODE_PC *node;
    i32 index;
    info->flags_word &= 0xfffe;
    if (system == NULL || system->path_sys == NULL || system->path_sys->path_count == 0) {
        return;
    }
    if (search_all_paths != 0) {
        path = info->path;
        if (path == NULL) {
            path = system->path_sys->paths[0];
            info->connection = NULL;
            info->path = path;
            info->next_check = 0;
        }
    } else {
        if (path == NULL) {
            path = system->path_sys->active_path;
        }
        if (info->path != path) {
            info->connection = NULL;
            info->path = path;
            info->next_check = 0;
        }
    }
    AISysResetPathSearchConnectionChecks(path);
    current = info->connection;
    if (current != NULL) {
        if (WithinConnection(system, position, path, current, 1, NULL, 0xff, 0xff, info, 0.0f, 1)) {
            return;
        }
        node = &info->path->nodes[current->node_indices[info->direction]];
        for (index = 0; index < node->connection_count; ++index) {
            AIPATHCNX_PC *candidate = node->connections[index];
            if (candidate != current &&
                WithinConnection(system, position, path, candidate, 1, NULL, 0xff, 0xff, info, 0.0f, 1)) {
                return;
            }
        }
        node = &info->path->nodes[current->node_indices[info->direction == 0]];
        for (index = 0; index < node->connection_count; ++index) {
            AIPATHCNX_PC *candidate = node->connections[index];
            if (candidate != current &&
                WithinConnection(system, position, path, candidate, 1, NULL, 0xff, 0xff, info, 0.0f, 1)) {
                return;
            }
        }
    }
    if (search_all_paths == 0) {
        i32 remaining = info->path->node_count < checks ? info->path->node_count : checks;
        while (remaining != 0) {
            node = &info->path->nodes[info->next_check];
            for (index = 0; index < node->connection_count; ++index) {
                if (WithinConnection(system, position, path, node->connections[index], 1, NULL, 0xff, 0xff, info, 0.0f,
                                     1)) {
                    return;
                }
            }
            info->next_check = (info->next_check + 1) % info->path->node_count;
            --remaining;
        }
    } else {
        AIPATH_PC *saved_path = info->path;
        AIPATHCNX_PC *saved_connection = info->connection;
        u8 saved_direction = info->direction;
        u8 start_path;
        u16 start_node;
        i32 inside_extents = 0;
        if (info->path_index == 0xff) {
            info->path_index = saved_path->index;
        }
        if (info->path_index >= system->path_sys->path_count) {
            info->path_index = 0;
        }
        info->path = system->path_sys->paths[info->path_index];
        if (info->next_check >= info->path->node_count) {
            info->next_check = 0;
        }
        AISysResetPathSearchConnectionChecks(info->path);
        start_path = info->path_index;
        start_node = info->next_check;
        while (checks != 0) {
            if (inside_extents == 0) {
                inside_extents = AIPathCheckExtents(info->path, position);
            }
            if (inside_extents != 0) {
                node = &info->path->nodes[info->next_check];
                for (index = 0; index < node->connection_count; ++index) {
                    AIPATHCNX_PC *candidate = node->connections[index];
                    if (WithinConnection(system, position, info->path, candidate, 1, NULL, 0xff, 0xff, info, 0.0f, 1)) {
                        return;
                    }
                    if (nearest_distance_squared != NULL) {
                        AIPATHCNX_PC *connection = node->connections[index];
                        f32 distance = NuLineToPointDistSqrEx(&info->path->nodes[connection->node_indices[0]].position,
                                                              &info->path->nodes[connection->node_indices[1]].position,
                                                              position, NULL);
                        if (distance < *nearest_distance_squared) {
                            *nearest_distance_squared = distance;
                            saved_connection = candidate;
                            saved_path = info->path;
                        }
                    }
                }
                ++info->next_check;
            }
            if (inside_extents == 0 || info->next_check >= info->path->node_count) {
                info->next_check = 0;
                ++info->path_index;
                if (info->path_index >= system->path_sys->path_count) {
                    info->path_index = 0;
                }
                info->path = system->path_sys->paths[info->path_index];
                info->connection = NULL;
                AISysResetPathSearchConnectionChecks(info->path);
                inside_extents = 0;
            }
            --checks;
            if (info->path_index == start_path && info->next_check == start_node) {
                break;
            }
        }
        info->path = saved_path;
        info->connection = saved_connection;
        info->direction = saved_direction;
    }
}

void __cdecl AISysGetPathPos(AISYS_PC *system, NUVEC *position, AIPATHINFO_PC *info, AIPATH_PC *path, i32 checks) {
    AISysGetPathPosEx(system, position, info, path, checks, 0, NULL);
}

void __cdecl AICircle(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    f32 radius = packet->movement_parameter < packet->fallback_stopping_distance ? packet->fallback_stopping_distance
                                                                                 : packet->movement_parameter;
    NUVEC difference;
    NUVEC circle_position;
    i32 angle;
    (void)system;
    (void)checks;
    NuVecSub(&difference, &object->position, &packet->fallback_destination);
    angle = NuAtan2D(difference.x, difference.z);
    difference.x = 0.0f;
    difference.z = radius;
    NuVecRotateY(&difference, &difference, angle);
    NuVecAdd(&circle_position, &packet->fallback_destination, &difference);
    if ((packet->flags & 0x200) != 0) {
        angle = NuAngSub(angle, 0x4000);
    } else {
        angle = NuAngAdd(angle, 0x4000);
    }
    difference.x = 0.0f;
    difference.y = 0.0f;
    difference.z = 0.5f;
    NuVecRotateY(&difference, &difference, angle);
    NuVecAdd(&packet->movement_destination, &circle_position, &difference);
    packet->flags |= 0x400;
    packet->movement_stopping_distance = 0.0f;
}

void __cdecl AIMoveDirectlyToDestination(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    (void)system;
    (void)checks;
    if (packet->movement_parameter > 0.0f) {
        NUVEC delta;
        f32 distance_squared = NuVecDistSqr(&packet->fallback_destination, &object->position, &delta);
        if (packet->movement_parameter * packet->movement_parameter > distance_squared) {
            packet->movement_destination = object->position;
            packet->movement_stopping_distance = 0.0f;
            return;
        }
    }
    packet->movement_destination = packet->fallback_destination;
    packet->movement_stopping_distance = 0.0f;
}

void LocaledbitsDrawSolidCircleXY(NUVEC *centre, f32 radius, f32 lower_y, f32 upper_y, u32 colour, i32 unused5,
                                  i32 segments) {
    i32 i;
    f32 previous_x = centre->x;
    f32 previous_z = centre->z + radius;
    for (i = 1; i <= segments; ++i) {
        i32 angle = (i * 65536) / segments;
        f32 next_x = centre->x + radius * NU_SIN_LUT(angle);
        f32 next_z = centre->z + NU_COS_LUT(angle) * radius;
        AiRndrLine3dDbg(previous_x, lower_y, previous_z, next_x, lower_y, next_z, colour);
        AiRndrLine3dDbg(previous_x, upper_y, previous_z, next_x, upper_y, next_z, colour);
        AiRndrLine3dDbg(next_x, lower_y, next_z, next_x, upper_y, next_z, colour);
        previous_x = next_x;
        previous_z = next_z;
    }
}

void LocaledbitsDrawSolidEllipseXY(NUVEC *centre, f32 radius_x, f32 radius_z, i32 rotation, f32 lower_y, f32 upper_y,
                                   u32 colour, i32 unused7, i32 segments) {
    i32 i;
    NURND_VERTEX3D_PC vertices[2];
    NURND_VERTEX3D_PC *first = &(vertices[0]);
    NURND_VERTEX3D_PC *second = &(vertices[1]);
    (*second).position.x = 0.0f;
    (*second).position.y = 0.0f;
    (*second).position.z = radius_z;
    NuVecRotateY(&(*second).position, &(*second).position, rotation);
    NuVecAdd(&(*second).position, &(*second).position, centre);
    (*first).colour = colour;
    (*second).colour = colour;
    for (i = 1; i <= segments; ++i) {
        i32 angle;
        (*first).position = (*second).position;
        (*second).position = *centre;
        angle = (i * 65536) / segments;
        {
            NUVEC offset = {radius_x * NU_SIN_LUT(angle), 0.0f, radius_z * NU_COS_LUT(angle)};
            NuVecRotateY(&offset, &offset, rotation);
            NuVecAdd(&(*second).position, centre, &offset);
            (*first).position.y = lower_y;
            (*second).position.y = lower_y;
            AiRndrLine3dDbg((*first).position.x, (*first).position.y, (*first).position.z, (*second).position.x,
                            (*second).position.y, (*second).position.z, colour);
            (*first).position.y = upper_y;
            (*second).position.y = upper_y;
            AiRndrLine3dDbg((*first).position.x, (*first).position.y, (*first).position.z, (*second).position.x,
                            (*second).position.y, (*second).position.z, colour);
            (*first).position = (*second).position;
            (*first).position.y = lower_y;
            AiRndrLine3dDbg((*first).position.x, (*first).position.y, (*first).position.z, (*second).position.x,
                            (*second).position.y, (*second).position.z, colour);
        }
    }
}

void LocaledbitsDrawCircleXY(NUVEC *centre, f32 radius, u32 colour, i32 unused3, i32 segments) {
    i32 i;
    NUVEC previous = {centre->x, centre->y, centre->z + radius};
    for (i = 1; i <= segments; ++i) {
        i32 angle = (i * 65536) / segments;
        NUVEC next = {centre->x + NU_SIN_LUT(angle) * radius, centre->y, centre->z + NU_COS_LUT(angle) * radius};
        AiRndrLine3dDbg(previous.x, previous.y, previous.z, next.x, next.y, next.z, colour);
        previous = next;
    }
}

void __cdecl AISysProcess(AISYS_PC *system, APIOBJECT_PC *player_1, APIOBJECT_PC *player_2) {
    i32 index;
    if (system == NULL) {
        return;
    }
    system->player_1 = player_1;
    system->player_2 = player_2;
    for (index = 0; index < system->antinode_count + 64; ++index) {
        i32 antinode_count = system->antinode_count;
        AIANTINODE_PC *antinode =
            index < antinode_count ? &system->antinodes[index] : &dynamic_antinodes[index - antinode_count];
        NUSPECIALHANDLE *special;
        NUMTX *draw_matrix;
        if (antinode->has_special == 0) {
            continue;
        }
        special = &antinode->special_handle;
        if (AISysSpecialNodeCallback != NULL && NuSpecialGetVisibilityFn(special) == 0) {
            AISysSpecialNodeCallback(system, special);
        }
        if (NuSpecialGetVisibilityFn(special) == 0) {
            continue;
        }
        draw_matrix = NuSpecialGetDrawMtx(special);
        NuVecMtxTransform(&antinode->position, &antinode->special_position, draw_matrix);
        antinode->min_y = antinode->min_y_offset + antinode->position.y;
        antinode->max_y = antinode->position.y + antinode->max_y_offset;
        if (antinode->type != 0) {
            NUVEC forward = {0.0f, 0.0f, 1.0f};
            NUVEC rotated;
            NuVecMtxRotate(&rotated, &forward, draw_matrix);
            antinode->rotation = NuAngAdd(NuAtan2D(rotated.x, rotated.z), antinode->rotation_offset);
        }
    }
    for (index = 0; index < system->locator_count; ++index) {
        AILOCATOR_PC *locator = &system->locators[index];
        AIPATHNODE_PC *start = &locator->path_info.path->nodes[locator->path_info.connection->node_indices[0]];
        AIPATHNODE_PC *end = &locator->path_info.path->nodes[locator->path_info.connection->node_indices[1]];
        NUVEC delta;
        NUVEC direction;
        NUVEC offset;
        f32 position_on_connection;
        f32 radius;
        f32 direction_x;
        i32 path_angle;
        if ((start->runtime_flags & 0x02) == 0 && (end->runtime_flags & 0x02) == 0) {
            continue;
        }
        NuVecSub(&delta, &end->position, &start->position);
        NuVecNorm(&direction, &delta);
        position_on_connection = locator->path_info.distance;
        if (position_on_connection > 1.0f) {
            radius = end->radius;
        } else if (position_on_connection < 0.0f) {
            radius = start->radius;
        } else {
            radius = end->radius * position_on_connection + (1.0f - position_on_connection) * start->radius;
        }
        direction_x = direction.x;
        direction.x = direction.z * radius;
        direction.z = -direction_x * radius;
        locator->position = start->position;
        NuVecScale(&offset, &delta, position_on_connection);
        NuVecAdd(&locator->position, &locator->position, &offset);
        NuVecScale(&offset, &direction, locator->path_info.width);
        NuVecAdd(&locator->position, &locator->position, &offset);
        path_angle = (i32)(NuAtan2(delta.x, delta.z) * AIPathAngularScale);
        locator->rotation = NuAngAdd(path_angle, locator->rotation_offset);
    }
    for (index = 0; index < 16; ++index) {
        AIGROUP_PC *group = &system->groups[index];
        if ((group->flags & 1) != 0) {
            group->flags = (group->flags & ~2u) | ((group->member_is_alive == 0) << 1);
        }
    }
}

static __declspec(noinline) f32 AIPathSafeDivide(f32 numerator, f32 denominator) {
    if (numerator == 0.0f || denominator == 0.0f)
        return 0.0f;
    return numerator / denominator;
}

static u32 CharacterTestPathCnx(AISYS_PC *system, APIOBJECT_PC *object, AIPACKET_PC *packet, AIPATHCNX_PC *connection,
                                i32 direction, f32 *nearest_distance) {
    AIPATH_PC *path;
    u32 flags;
    AIPATHCNX_PC *current;
    AIPATHNODE_PC *shared;
    AIPATHNODE_PC *first;
    AIPATHNODE_PC *second;
    f32 first_radius;
    i32 narrow;
    f32 second_radius;
    NUVEC delta;
    NUVEC local;
    NUVEC radial;
    f32 height_distance;
    f32 distance;
    f32 clearance;
    i32 node_index;
    i32 angle;
    NUVEC wall;
    f32 length;
    f32 denominator;
    f32 numerator;
    f32 fraction;
    f32 inverse;
    f32 height;
    f32 minimum;
    f32 maximum;

    path = packet->path_info.path;
    if (connection->last_search_checksum == path->search_checksum || path->nodes == NULL) {
        return 0;
    }
    connection->last_search_checksum = path->search_checksum;
    if (object->character_data != NULL) {
        flags = connection->traversal_flags[0] | connection->traversal_flags[1];
        if (flags != 0) {
            if (direction != -1) {
                flags = connection->traversal_flags[direction];
            }
            if (flags != 0 && ((packet->capabilities & flags) == 0 || (flags & 0x98000000) != 0)) {
                return 0;
            }
        }
    }
    if ((object->flags_low & 0x80) == 0 && packet->current_route != 0xff &&
        ((u64)connection->route_mask & ((u64)1 << packet->current_route)) == 0) {
        current = packet->path_info.connection;
        if (current == NULL || current == connection) {
            return 0;
        }
        shared = NULL;
        if (current->node_indices[0] == connection->node_indices[0]) {
            shared = &path->nodes[connection->node_indices[0]];
        } else if (current->node_indices[0] == connection->node_indices[1]) {
            shared = &path->nodes[current->node_indices[0]];
        } else if (current->node_indices[1] == connection->node_indices[0]) {
            shared = &path->nodes[connection->node_indices[0]];
        } else if (current->node_indices[1] == connection->node_indices[1]) {
            shared = &path->nodes[connection->node_indices[1]];
        }
        if (shared == NULL || ((u64)shared->route_boundary_mask & ((u64)1 << packet->current_route)) == 0) {
            return 0;
        }
    }
    if (direction == -1) {
        direction = 0;
    }
    first = &path->nodes[connection->node_indices[0]];
    second = &path->nodes[connection->node_indices[1]];
    first_radius = first->radius;
    narrow = 0;
    if (first_radius > object->collision_radius + AIPathConnectionMargin) {
        first_radius -= object->collision_radius;
    } else {
        narrow = 1;
    }
    second_radius = second->radius;
    if (second_radius > object->collision_radius + AIPathConnectionMargin) {
        second_radius -= object->collision_radius;
    } else {
        narrow = 1;
    }
    if (first->has_special != 0 &&
        (path->updated_node_bits[connection->node_indices[0] >> 3] & (1 << (connection->node_indices[0] & 7))) == 0) {
        AIPathNodeUpdatePos(system, path, first);
    }
    if (second->has_special != 0 &&
        (path->updated_node_bits[connection->node_indices[1] >> 3] & (1 << (connection->node_indices[1] & 7))) == 0) {
        AIPathNodeUpdatePos(system, path, second);
    }

    delta.x = object->position.x - first->position.x;
    delta.z = object->position.z - first->position.z;

    NuVecRotateY(&local, &delta, -connection->rotation);

    if ((f32)fabs(second_radius - first_radius) > connection->horizontal_distance) {
        if (second_radius > first_radius) {
            first_radius = second_radius;
            first = second;
        }
        height_distance = (f32)fabs(first->position.y - packet->terrain_origin.y);
        distance = NuVecXZDist(&object->position, &first->position, &radial);

        if (first->min_height > packet->terrain_origin.y || packet->terrain_origin.y > first->max_height) {
            clearance = distance - first_radius;
            clearance = height_distance > clearance ? height_distance : clearance;
        } else if (first_radius >= distance) {
            packet->path_info.flags_word = (u16)(((packet->path_info.flags_word | 1) & ~8) | (narrow << 3));
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->last_path_position = object->position;
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            node_index = first - path->nodes;
            path->inside_node_bits[node_index / 8] |= 1 << (node_index % 8);
            packet->inside_path_node = node_index;
            if (second_radius * second_radius >= NuVecXZDistSqr(&object->position, &second->position, &radial)) {
                node_index = second - path->nodes;
                path->inside_node_bits[node_index / 8] |= 1 << (node_index % 8);
                packet->inside_path_node = node_index;
            }
            return 1;
        } else {
            clearance = distance - first_radius;
        }
        if (*nearest_distance > clearance) {
            *nearest_distance = clearance;
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            if (first_radius < distance) {
                packet->last_path_position.x = first_radius * radial.x / distance + first->position.x;
                packet->last_path_position.z = first_radius * radial.z / distance + first->position.z;
            } else {
                packet->last_path_position.x = object->position.x;
                packet->last_path_position.z = object->position.z;
            }
            packet->last_path_position.y = first->position.y;
            packet->inside_path_node = -1;
        }
        return 0;
    }

    angle = PCPathArcSine((second_radius - first_radius) / connection->horizontal_distance);
    wall = local;
    if (wall.x < 0.0f) {
        wall.x = -wall.x;
    }
    NuVecRotateY(&wall, &wall, -angle);
    length = NuTrigTable[((u32)(angle + 0x4000) >> 1) & 0x7fff] * connection->horizontal_distance;
    if (wall.z < 0.0f) {
        height_distance = (f32)fabs(first->position.y - packet->terrain_origin.y);
        distance = NuVecXZDist(&object->position, &first->position, &radial);

        if (first->min_height > packet->terrain_origin.y || packet->terrain_origin.y > first->max_height) {
            clearance = distance - first_radius;
            clearance = height_distance > clearance ? height_distance : clearance;
        } else if (first_radius >= distance) {
            packet->path_info.flags_word = (u16)(((packet->path_info.flags_word | 1) & ~8) | (narrow << 3));
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->last_path_position = object->position;
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            node_index = connection->node_indices[0];
            path->inside_node_bits[node_index >> 3] |= 1 << (node_index & 7);
            packet->inside_path_node = connection->node_indices[0];
            return 1;
        } else {
            clearance = distance - first_radius;
        }
        if (*nearest_distance > clearance) {
            *nearest_distance = clearance;
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            if (first_radius < distance) {
                packet->last_path_position.x = first_radius * radial.x / distance + first->position.x;
                packet->last_path_position.z = first_radius * radial.z / distance + first->position.z;
            } else {
                packet->last_path_position.x = object->position.x;
                packet->last_path_position.z = object->position.z;
            }
            packet->last_path_position.y = first->position.y;
            packet->inside_path_node = -1;
        }
    }
    if (wall.z > length) {
        height_distance = (f32)fabs(second->position.y - packet->terrain_origin.y);
        distance = NuVecXZDist(&object->position, &second->position, &radial);

        if (second->min_height > packet->terrain_origin.y || packet->terrain_origin.y > second->max_height) {
            clearance = distance - second_radius;
            clearance = height_distance > clearance ? height_distance : clearance;
        } else if (second_radius >= distance) {
            packet->path_info.flags_word = (u16)(((packet->path_info.flags_word | 1) & ~8) | (narrow << 3));
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->last_path_position = object->position;
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            node_index = connection->node_indices[1];
            path->inside_node_bits[node_index >> 3] |= 1 << (node_index & 7);
            packet->inside_path_node = connection->node_indices[1];
            return 1;
        } else {
            clearance = distance - second_radius;
        }
        if (*nearest_distance > clearance) {
            *nearest_distance = clearance;
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            if (second_radius < distance) {
                packet->last_path_position.x = second_radius * radial.x / distance + second->position.x;
                packet->last_path_position.z = second_radius * radial.z / distance + second->position.z;
            } else {
                packet->last_path_position.x = object->position.x;
                packet->last_path_position.z = object->position.z;
            }
            packet->last_path_position.y = second->position.y;
            packet->inside_path_node = -1;
        }
    } else if (!(wall.z < 0.0f)) {

        if (first_radius > local.z) {
            height = first->position.y;
            minimum = first->min_height;
            maximum = first->max_height;
            if (local.z > connection->horizontal_distance - second_radius) {
                minimum = second->min_height < minimum ? second->min_height : minimum;
                maximum = second->max_height > maximum ? second->max_height : maximum;
            }
        } else if (local.z > connection->horizontal_distance - second_radius) {
            height = second->position.y;
            minimum = second->min_height;
            maximum = second->max_height;
        } else {
            denominator = connection->horizontal_distance - (first_radius + second_radius);
            numerator = local.z - first_radius;
            fraction = AIPathSafeDivide(numerator, denominator);
            inverse = 1.0f - fraction;
            height = fraction * second->position.y + first->position.y * inverse;
            minimum = second->min_height * fraction + first->min_height * inverse;
            maximum = fraction * second->max_height + first->max_height * inverse;
        }
        height_distance = (f32)fabs(height - packet->terrain_origin.y);

        if (height_distance > 0.0f && (minimum > packet->terrain_origin.y || packet->terrain_origin.y > maximum)) {
            clearance = wall.x - first_radius;
            clearance = height_distance > clearance ? height_distance : clearance;
        } else if (first_radius > wall.x) {
            packet->path_info.flags_word = (u16)(((packet->path_info.flags_word | 1) & ~8) | (narrow << 3));
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            packet->last_path_position = object->position;
            packet->inside_path_node = -1;
            if (first_radius > local.z) {
                if (first_radius * first_radius >= NuVecXZDistSqr(&object->position, &first->position, &radial)) {
                    node_index = connection->node_indices[0];
                    path->inside_node_bits[node_index >> 3] |= 1 << (node_index & 7);
                    packet->inside_path_node = connection->node_indices[0];
                }
            } else if (local.z > length - second_radius) {
                if (second_radius * second_radius >= NuVecXZDistSqr(&object->position, &second->position, &radial)) {
                    node_index = connection->node_indices[1];
                    path->inside_node_bits[node_index >> 3] |= 1 << (node_index & 7);
                    packet->inside_path_node = connection->node_indices[1];
                }
            }
            return 1;
        } else {
            clearance = wall.x - first_radius;
        }
        if (*nearest_distance > clearance) {
            *nearest_distance = clearance;
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            if (first_radius > wall.x) {
                packet->last_path_position.x = object->position.x;
                packet->last_path_position.z = object->position.z;
            } else {
                wall.x = first_radius;
                NuVecRotateY(&wall, &wall, angle);
                if (local.x < 0.0f) {
                    wall.x = -wall.x;
                }
                NuVecRotateY(&wall, &wall, connection->rotation);
                packet->last_path_position.x = first->position.x + wall.x;
                packet->last_path_position.z = first->position.z + wall.z;
            }
            packet->last_path_position.y = first->position.y;
            packet->inside_path_node = -1;
        }
    }
    return 0;
}

void __cdecl AISysGetCharacterPathPos(AISYS_PC *system, APIOBJECT_PC *object, AIPACKET_PC *packet, i32 checks,
                                      i32 ground) {
    f32 nearest_distance_squared;
    u16 previous_flags;
    AIPATH_PC *path;
    AIPATHCNX_PC *current_connection;
    i32 current_direction;
    i32 check_adjacent_connections;
    AIPATHNODE_PC *first_node;
    AIPATHCNX_PC *candidate;
    i32 other_direction;
    AIPATHNODE_PC *second_node;
    i32 remaining_checks;
    AIPATHNODE_PC *node;
    AIPATH_PC *original_path;
    AIPATHCNX_PC *original_connection;
    u8 original_direction;
    i32 position_inside_path_bounds;
    i32 connection_index;

    nearest_distance_squared = 3.402823466e+38f;

    previous_flags = packet->path_info.flags_word;
    packet->path_info.flags_word = (previous_flags & (u8) ~(1 | 2 | 8)) | ((previous_flags & 1) << 1);

    if (system == NULL || system->path_sys == NULL || system->path_sys->path_count == 0) {
        AISysCharacterSetPath(packet, NULL);
        packet->path_info.connection = NULL;
        packet->current_route = 0xff;
        packet->next_route = 0;
        return;
    }

    path = packet->path_info.path;
    if (path == NULL) {
        path = (packet->navigation_flags & 2) != 0 ? system->path_sys->paths[0] : system->path_sys->active_path;
        AISysCharacterSetPath(packet, path);
        packet->path_info.connection = NULL;
        packet->current_route = 0xff;
        packet->next_route = 0;
        packet->path_info.next_check = 0;
        packet->path_info.path_index = 0;
    }
    path = packet->path_info.path;
    AISysResetPathSearchConnectionChecks(path);

    current_connection = packet->path_info.connection;
    if (current_connection != NULL) {
        current_direction = packet->path_info.direction;
        if (CharacterTestPathCnx(system, object, packet, current_connection, current_direction,
                                 &nearest_distance_squared) != 0) {
            return;
        }

        check_adjacent_connections = (object->flags_low & 0x80) != 0 ||
                                     (packet->path_target == NULL && packet->path_update_flag == 0 && ground != 0);
        if (!check_adjacent_connections) {
            return;
        }

        path = packet->path_info.path;
        first_node = &path->nodes[current_connection->node_indices[current_direction]];
        for (connection_index = 0; connection_index < first_node->connection_count; ++connection_index) {
            candidate = first_node->connections[connection_index];
            if (candidate != current_connection &&
                CharacterTestPathCnx(system, object, packet, candidate,
                                     candidate->node_indices[0] != current_connection->node_indices[current_direction],
                                     &nearest_distance_squared) != 0)
                return;
        }

        path = packet->path_info.path;
        other_direction = current_direction == 0;
        second_node = &path->nodes[current_connection->node_indices[other_direction]];
        for (connection_index = 0; connection_index < second_node->connection_count; ++connection_index) {
            candidate = second_node->connections[connection_index];
            if (candidate != current_connection &&
                CharacterTestPathCnx(system, object, packet, candidate,
                                     candidate->node_indices[0] != current_connection->node_indices[other_direction],
                                     &nearest_distance_squared) != 0)
                return;
        }
    }

    path = packet->path_info.path;
    if ((packet->navigation_flags & 2) == 0) {
        remaining_checks = path->node_count < checks ? path->node_count : checks;
        while (remaining_checks-- > 0) {
            node = &path->nodes[packet->path_info.next_check];
            for (connection_index = 0; connection_index < node->connection_count; ++connection_index) {
                if (CharacterTestPathCnx(system, object, packet, node->connections[connection_index], -1,
                                         &nearest_distance_squared) != 0) {
                    return;
                }
            }
            path = packet->path_info.path;
            packet->path_info.next_check = (packet->path_info.next_check + 1) % path->node_count;
        }
        return;
    }

    original_path = packet->path_info.path;
    original_connection = packet->path_info.connection;
    original_direction = packet->path_info.direction;

    if (packet->path_info.path_index == 0xff) {
        packet->path_info.path_index = original_path->index;
    }
    if (packet->path_info.path_index >= system->path_sys->path_count) {
        packet->path_info.path_index = 0;
    }

    path = system->path_sys->paths[packet->path_info.path_index];
    packet->path_info.path = path;
    if (packet->path_info.next_check >= path->node_count) {
        packet->path_info.next_check = 0;
    }
    AISysResetPathSearchConnectionChecks(path);

    position_inside_path_bounds = 0;
    for (remaining_checks = checks; remaining_checks > 0; --remaining_checks) {
        if (!position_inside_path_bounds) {
            position_inside_path_bounds = AIPathCheckExtents(packet->path_info.path, &packet->owner->position);
        }

        if (position_inside_path_bounds) {
            path = packet->path_info.path;
            node = &path->nodes[packet->path_info.next_check];
            for (connection_index = 0; connection_index < node->connection_count; ++connection_index) {
                if (CharacterTestPathCnx(system, object, packet, node->connections[connection_index], -1,
                                         &nearest_distance_squared) != 0) {
                    return;
                }
            }

            ++packet->path_info.next_check;
            path = packet->path_info.path;
            if (packet->path_info.next_check < path->node_count) {
                continue;
            }
        }

        packet->path_info.next_check = 0;
        ++packet->path_info.path_index;
        if (packet->path_info.path_index >= system->path_sys->path_count) {
            packet->path_info.path_index = 0;
        }

        path = system->path_sys->paths[packet->path_info.path_index];
        packet->path_info.path = path;
        packet->path_info.connection = NULL;
        packet->current_route = 0xff;
        packet->next_route = 0;
        AISysResetPathSearchConnectionChecks(packet->path_info.path);
        position_inside_path_bounds = 0;
    }

    packet->path_info.path = original_path;
    packet->path_info.connection = original_connection;
    packet->path_info.direction = original_direction;
}

void __cdecl AISysUpdateCharacterPathPos(AISYS_PC *system, APIOBJECT_PC *object, AIPACKET_PC *packet, i32 ground,
                                         f32 elapsed) {
    i32 should_find_path =
        (packet->flags & 0x02000000) == 0 &&
        (((object->flags_low & 4) == 0 || (packet->path_info.flags & 1) == 0) || object->supporting_platform_id != -1);
    if (should_find_path) {
        if (packet->path_info.path == NULL) {
            packet->last_path_position = object->position;
        } else {
            AISysGetCharacterPathPos(system, object, packet,
                                     (object->flags_low & 0x80000) == 0 ? AIPathDefaultChecks : 0, ground);
        }
        packet->flags &= ~0x02000000u;
        if ((packet->path_info.flags & 1) == 0) {
            packet->time_off_path += elapsed;
        } else {
            packet->time_off_path = 0.0f;
        }
    } else {
        AIPATH_PC *path = packet->path_info.path;
        if (path != NULL && packet->path_info.connection != NULL) {
            u8 node_index = packet->path_info.connection->node_indices[0];
            AIPATHNODE_PC *first = &path->nodes[node_index];
            AIPATHNODE_PC *second = &path->nodes[packet->path_info.connection->node_indices[1]];
            if (first->has_special != 0 && (path->updated_node_bits[node_index / 8] & (1u << (node_index % 8))) == 0) {
                AIPathNodeUpdatePos(system, path, first);
            }
            if (second->has_special != 0) {
                node_index = packet->path_info.connection->node_indices[1];
                path = packet->path_info.path;
                if ((path->updated_node_bits[node_index / 8] & (1u << (node_index % 8))) == 0) {
                    AIPathNodeUpdatePos(system, path, second);
                }
            }
        }
        path = packet->path_info.path;
        if (packet->inside_path_node != -1 && path != NULL) {
            i16 node_index = packet->inside_path_node;
            path->inside_node_bits[node_index / 8] |= (u8)(1u << (node_index % 8));
        }
        packet->flags &= ~0x02000000u;
        packet->time_off_path = 0.0f;
    }
}

static i32 RowMoveWander(AIGROUP_PC *group, AIROW_PC *row, AIROW_PC *previous, APIOBJECT_PC *leader) {
    i32 i;
    NUVEC offset;
    NUVEC rotated;
    NUVEC centre;
    i32 node_index;
    i32 first;
    u8 route;
    i32 direction;
    i32 current_angle;
    i32 next_angle;
    f32 distance;
    i32 angle;
    i32 difference;
    i32 step;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *node;
    AIPATHCNX_PC *next;
    AIPATHNODE_PC *other;
    AIPATHNODE_PC *nodes;
    APIOBJECT_PC *object;
    connection = row->path_info.connection;
    if (connection == NULL)
        return 0;
    node_index = connection->node_indices[row->path_info.direction == 0];
    if (row->next_connection == NULL) {
        if (previous != NULL) {
            row->next_connection = previous->path_info.connection == connection ? previous->next_connection
                                                                                : previous->path_info.connection;
            if (row->next_connection == NULL)
                return 0;
        } else {
            node = &row->path_info.path->nodes[node_index];
            first = NuRandInt() % node->connection_count;
            for (i = 0; i < node->connection_count; ++i) {
                row->next_connection = node->connections[(first + i) % node->connection_count];
                next = row->next_connection;
                if (next != connection) {
                    route = leader->ai->current_route;
                    if (route == 0xff || (((u64)1 << route) & next->route_mask) != 0 ||
                        (((u64)1 << route) & node->route_boundary_mask) != 0) {
                        direction = node_index != next->node_indices[0];
                        other = &row->path_info.path->nodes[next->node_indices[direction ^ 1]];
                        if (other->radius > group->radius && next->traversal_flags[direction] == 0)
                            break;
                    }
                }
                row->next_connection = NULL;
            }
            if (row->next_connection == NULL)
                row->next_connection = connection;
        }
        next = row->next_connection;
        if (next->traversal_flags[node_index != next->node_indices[0]] == 0) {
            current_angle = connection->rotation;
            if (row->path_info.direction != 0)
                current_angle = NuAngAdd(0x8000, current_angle);
            if (connection->node_indices[row->path_info.direction == 0] == next->node_indices[0]) {
                row->next_direction = 0;
                next_angle = next->rotation;
            } else {
                row->next_direction = 1;
                next_angle = NuAngAdd(0x8000, next->rotation);
            }
            row->flags = (row->flags & ~1u) | (NuAngSub(next_angle, current_angle) < 0);
        }
    }
    offset.x = (row->flags & 1u) ? -group->radius : group->radius;
    offset.y = 0.0f;
    offset.z = 0.0f;
    NuVecRotateY(&rotated, &offset, row->rotation);
    NuVecAdd(&centre, &rotated, &row->position);
    if (!(row->flags & 2u)) {
        if (row->path_info.connection != row->next_connection) {
            nodes = row->path_info.path->nodes;
            distance = NuInfiniteLineToPointDistSqr(&nodes[row->next_connection->node_indices[0]].position,
                                                    &nodes[row->next_connection->node_indices[1]].position, &centre);
            if (group->radius * group->radius >= distance)
                row->flags |= 2u;
        } else {
            node = &row->path_info.path->nodes[row->path_info.connection->node_indices[row->path_info.direction == 0]];
            distance = NuVecXZDistSqr(&row->position, &node->position, &rotated);
            if (node->radius_squared > distance) {
                for (i = 0; i < group->row_count; ++i) {
                    group->rows[i].next_connection = NULL;
                    group->rows[i].rotation = NuAngAdd(group->rows[i].rotation, 0x8000);
                    group->rows[i].path_info.direction = group->rows[i].path_info.direction == 0;
                }
                group->leader = NULL;
                group->flags ^= 4u;
                if ((group->flags & 4u)) {
                    for (i = group->member_count - 1; i != -1; --i) {
                        object = group->members[i];
                        if (object != NULL && (object->flags_low & 0x1001) == 0x1001 &&
                            object->leader_selection_state == 0) {
                            group->leader = object;
                            break;
                        }
                    }
                } else {
                    for (i = 0; i < group->member_count; ++i) {
                        object = group->members[i];
                        if (object->owner != NULL && (object->flags_low & 0x1001) == 0x1001 &&
                            object->leader_selection_state == 0) {
                            group->leader = object;
                            break;
                        }
                    }
                }
                return 1;
            }
        }
    }
    if ((row->flags & 2u)) {
        angle = row->next_connection->rotation;
        if (row->next_direction != 0)
            angle = NuAngAdd(angle, 0x8000);
        difference = NuAngSub(angle, row->rotation);
        step = group->rotation_speed;
        if (difference <= step) {
            step = -step;
            if (difference >= step) {
                row->flags &= ~2u;
                row->path_info.connection = row->next_connection;
                row->next_connection = NULL;
                row->path_info.direction = row->next_direction;
                step = difference;
            }
        }
        row->rotation = NuAngAdd(step, row->rotation);
        NuVecRotateY(&rotated, &offset, row->rotation);
        NuVecSub(&row->position, &centre, &rotated);
        return 0;
    }
    if (!(group->flags & 0x10u)) {
        angle = row->path_info.connection->rotation;
        if (row->path_info.direction != 0)
            angle = NuAngAdd(angle, 0x8000);
        difference = NuAngSub(angle, row->rotation);
        step = group->rotation_speed;
        if (difference <= step) {
            step = -step;
            if (difference >= step)
                step = difference;
        }
        row->rotation = NuAngAdd(step, row->rotation);
        rotated.x = rotated.y = rotated.z = 0.0f;
        if (GetCharacterGoalSpeedFn != NULL)
            rotated.z = GetCharacterGoalSpeedFn(leader);
        NuVecRotateY(&rotated, &rotated, row->rotation);
        NuVecAdd(&row->position, &row->position, &rotated);
        row->position.y = leader->position.y;
    }
    return 0;
}
static i32 RowMoveTowards(AIGROUP_PC *group, AIROW_PC *row, AIROW_PC *previous, APIOBJECT_PC *leader) {
    i32 i;
    i32 node_index;
    i32 direction_node;
    i32 connection_index;
    i32 reverse;
    i32 angle;
    i32 difference;
    i32 speed;
    f32 distance;
    NUVEC offset;
    NUVEC rotated;
    NUVEC centre;
    AIPACKET_PC *ai;
    AIPATHCNX_PC *connection = row->path_info.connection;
    AIPATHNODE_PC *nodes;
    AIPATHNODE_PC *node;
    AIPATHNODE_PC *node1;
    APIOBJECT_PC *object;
    NUVEC *position;
    if (connection == NULL)
        return 0;
    node_index = connection->node_indices[row->path_info.direction == 0];
    direction_node = node_index;
    if (row->next_connection == NULL) {
        if (previous != NULL) {
            row->next_connection = row->path_info.connection == previous->path_info.connection
                                       ? previous->next_connection
                                       : previous->path_info.connection;
        } else if (group->leader != NULL) {
            ai = group->leader->ai;
            if (ai->script_destination_path_info.path == row->path_info.path) {
                direction_node = ai->script_destination_path_info.distance > 0.5f
                                     ? ai->script_destination_path_info.connection->node_indices[1]
                                     : ai->script_destination_path_info.connection->node_indices[0];
                connection_index = row->path_info.path->route_matrix[node_index][direction_node];
                if (connection_index != 0xff) {
                    row->next_connection = row->path_info.path->nodes[node_index].connections[connection_index];
                    reverse = row->next_connection->node_indices[0] != node_index;
                    if (row->next_connection->traversal_flags[reverse] != 0)
                        row->next_connection = NULL;
                }
            }
        }
        if (row->next_connection != NULL) {
            reverse = row->next_connection->node_indices[0] != direction_node;
            if (row->next_connection->traversal_flags[reverse] == 0) {
                if (row->path_info.direction != 0)
                    angle = NuAngAdd(0x8000, row->path_info.connection->rotation);
                else
                    angle = row->path_info.connection->rotation;
                if (row->path_info.connection->node_indices[row->path_info.direction == 0] ==
                    row->next_connection->node_indices[0]) {
                    row->next_direction = 0;
                    difference = row->next_connection->rotation;
                } else {
                    row->next_direction = 1;
                    difference = NuAngAdd(0x8000, row->next_connection->rotation);
                }
                if (NuAngSub(difference, angle) < 0)
                    row->flags |= 1;
                else
                    row->flags &= ~1;
            }
        }
    }
    if (row->next_connection != NULL) {
        offset.z = offset.y = 0.0f;
        offset.x = (row->flags & 1) != 0 ? -group->radius : group->radius;
        NuVecRotateY(&rotated, &offset, row->rotation);
        position = &row->position;
        NuVecAdd(&centre, &rotated, position);
        if (!row->is_turning) {
            if (row->path_info.connection == row->next_connection) {
                node =
                    &row->path_info.path->nodes[row->path_info.connection->node_indices[row->path_info.direction == 0]];
                distance = NuVecXZDistSqr(position, &node->position, &rotated);
                if (distance < node->radius_squared) {
                    for (i = 0; i < group->row_count; ++i) {
                        group->rows[i].next_connection = NULL;
                        group->rows[i].rotation = NuAngAdd(group->rows[i].rotation, 0x8000);
                        group->rows[i].path_info.direction = group->rows[i].path_info.direction == 0;
                    }
                    group->is_reversed = !group->is_reversed;
                    group->leader = NULL;
                    if (group->is_reversed) {
                        for (i = group->member_count - 1; i >= 0; --i) {
                            object = group->members[i];
                            if (object != NULL && (object->flags_low & 1) != 0 && (object->flags_low & 0x1000) != 0 &&
                                object->leader_selection_state == 0) {
                                group->leader = group->members[i];
                                break;
                            }
                        }
                    } else {
                        for (i = 0; i < group->member_count; ++i) {
                            object = group->members[i];
                            if (object->owner != NULL && (object->flags_low & 1) != 0 &&
                                (object->flags_low & 0x1000) != 0 && object->leader_selection_state == 0) {
                                group->leader = group->members[i];
                                break;
                            }
                        }
                    }
                    return 1;
                }
            } else {
                nodes = row->path_info.path->nodes;
                node = &nodes[row->next_connection->node_indices[0]];
                node1 = &nodes[row->next_connection->node_indices[1]];
                distance = NuInfiniteLineToPointDistSqr(&node->position, &node1->position, &centre);
                if (distance <= group->radius * group->radius)
                    row->is_turning = 1;
            }
        }
        if (row->is_turning) {
            angle = row->next_connection->rotation;
            if (row->next_direction != 0)
                angle = NuAngAdd(angle, 0x8000);
            difference = NuAngSub(angle, row->rotation);
            speed = group->rotation_speed;
            if (difference > speed) {
                difference = speed;
            } else if (difference < -speed) {
                difference = -speed;
            } else {
                row->is_turning = 0;
                row->path_info.connection = row->next_connection;
                row->path_info.direction = row->next_direction;
                row->next_connection = NULL;
            }
            row->rotation = NuAngAdd(difference, row->rotation);
            NuVecRotateY(&rotated, &offset, row->rotation);
            NuVecSub(position, &centre, &rotated);
            return 0;
        }
        if (!group->is_row_turning) {
            angle = row->path_info.connection->rotation;
            if (row->path_info.direction != 0)
                angle = NuAngAdd(angle, 0x8000);
            difference = NuAngSub(angle, row->rotation);
            speed = group->rotation_speed;
            if (difference > speed)
                difference = speed;
            else if (difference < -speed)
                difference = -speed;
            row->rotation = NuAngAdd(difference, row->rotation);
            rotated.z = rotated.y = rotated.x = 0.0f;
            if (GetCharacterGoalSpeedFn != NULL)
                rotated.z = GetCharacterGoalSpeedFn(leader);
            NuVecRotateY(&rotated, &rotated, row->rotation);
            NuVecAdd(position, position, &rotated);
            row->position.y = leader->position.y;
        }
    } else {
        if (row->path_info.direction != 0) {
            if (row->path_info.distance < 0.0f)
                return 1;
        } else if (row->path_info.distance > 1.0f) {
            return 1;
        }
        angle = row->path_info.connection->rotation;
        if (row->path_info.direction != 0)
            angle = NuAngAdd(angle, 0x8000);
        difference = NuAngSub(angle, row->rotation);
        speed = group->rotation_speed;
        if (difference > speed)
            difference = speed;
        else if (difference < -speed)
            difference = -speed;
        row->rotation = NuAngAdd(difference, row->rotation);
        rotated.z = rotated.y = rotated.x = 0.0f;
        if (GetCharacterGoalSpeedFn != NULL)
            rotated.z = GetCharacterGoalSpeedFn(leader);
        NuVecRotateY(&rotated, &rotated, row->rotation);
        NuVecAdd(&row->position, &row->position, &rotated);
        row->position.y = leader->position.y;
    }
    return 0;
}
static void FormationMove(AIGROUP_PC *group, i32 (*move)(AIGROUP_PC *, AIROW_PC *, AIROW_PC *, APIOBJECT_PC *)) {
    i32 i;
    i32 turning;
    AIROW_PC *previous;
    AIROW_PC *row;
    turning = 0;
    if (move != NULL && (group->flags & 8u)) {
        previous = NULL;
        if ((group->flags & 4u)) {
            for (i = group->row_count - 1; i != -1; --i) {
                row = &group->rows[i];
                if (row->is_alive) {
                    if (move(group, row, previous, group->leader) != 0)
                        break;
                    if ((row->flags & 2u))
                        turning = 1;
                    previous = row;
                }
            }
        } else {
            for (i = 0; i < group->row_count; ++i) {
                row = &group->rows[i];
                if (row->is_alive) {
                    if (move(group, row, previous, group->leader) != 0)
                        break;
                    if ((row->flags & 2u))
                        turning = 1;
                    previous = row;
                }
            }
        }
    }
    group->flags = (group->flags & ~0x10u) | ((u32)turning << 4);
}
void __cdecl AIMoveInstruction(AIPACKET_PC *packet, NUVEC *destination, f32 stopping_distance, AIPATHINFO_PC *path_info,
                               i32 mode, f32 movement_parameter) {
    AIGROUP_PC *group;
    group = packet->group;
    if (group != NULL && (group->flags & 8u)) {
        switch (mode) {
            case 4:
                if (group->leader == (APIOBJECT_PC *)packet->owner)
                    FormationMove(group, RowMoveWander);
                AIFormationFollow(packet);
                return;
            case 5:
                mode = 1;
                break;
            case 1:
                if (group->leader == (APIOBJECT_PC *)packet->owner)
                    FormationMove(group, RowMoveTowards);
                AIFormationFollow(packet);
                return;
        }
    }
    if (destination != NULL) {
        packet->fallback_destination = *destination;
    }
    if (path_info != NULL) {
        packet->fallback_path_info = *path_info;
    }
    packet->fallback_stopping_distance = stopping_distance;
    packet->flags = (packet->flags & ~7u) | (mode & 7);
    packet->movement_parameter = movement_parameter;
}
void __cdecl AIFormationFollow(AIPACKET_PC *packet) {
    NUVEC offset;
    u8 column;
    f32 column_offset;
    NUVEC destination;
    AIGROUP_PC *group;
    AIROW_PC *row;
    group = packet->group;
    if (packet->group_row < group->row_count) {
        row = &group->rows[packet->group_row];
        column = packet->group_column;
        if ((packet->flags & 0x01000000u) != 0) {
            offset.x = (group->count_across & 1) != 0 ? 0.0f : -(0.5f * group->x_spacing);
        } else {
            column_offset = ((column + 1) / 2) * group->x_spacing;
            offset.x = (column & 1) != 0 ? -column_offset : column_offset;
        }
        if ((group->flags & 4u)) {
            offset.x = -offset.x;
        }
        offset.y = 0.0f;
        offset.z = 0.0f;
        NuVecRotateY(&offset, &offset, row->rotation);
        NuVecAdd(&destination, &offset, &row->position);
        packet->flags |= 8u;
        packet->fallback_destination = destination;
        packet->fallback_path_info = row->path_info;
        packet->fallback_stopping_distance = 0.0f;
        packet->movement_parameter = 0.0f;
        packet->flags = (packet->flags & ~7u) | ((group != NULL && (group->flags & 8u)) ? 1u : 5u);
        packet->script_action_position.x = 0.0f;
        packet->script_action_position.y = 0.0f;
        packet->script_action_position.z = 100.0f;
        NuVecRotateY(&packet->script_action_position, &packet->script_action_position, row->rotation);
        NuVecAdd(&packet->script_action_position, &packet->script_action_position, &row->position);
        packet->movement_look_target = &packet->script_action_position;
    }
}
void __cdecl AddToAIGroup(AIGROUP_PC *group, APIOBJECT_PC *object) {
    u8 member_index;
    i32 count_across;
    i32 row_count;
    if (object == NULL || group == NULL || group->member_count >= 16) {
        return;
    }
    member_index = group->member_count;
    count_across = group->count_across;
    row_count = (i32)(AIPathSafeDivide((f32)(member_index + 1), (f32)(count_across)));
    if ((member_index + 1) % group->count_across != 0)
        ++row_count;
    if (row_count > 4) {
        return;
    }
    if (member_index == 0) {
        group->leader = object;
        if (group->x_spacing > 0.0f) {
            group->radius = ((f32)(count_across)-1.0f) * group->x_spacing * 0.5f;
            group->rotation_speed =
                (i32)(AIPathAngularScale *
                      AIPathSafeDivide(group->max_speed, (f32)(group->radius * NuTime_RefreshRate)));
        } else {
            group->radius = 0.0f;
            group->rotation_speed = 100;
        }
    }
    group->row_count = (u8)(row_count);
    object->ai->group = group;
    object->ai->group_member_index = group->member_count;
    object->ai->group_row =
        (i32)(AIPathSafeDivide((f32)((i32)(group->member_count)), (f32)((i32)(group->count_across))));
    object->ai->group_column = group->member_count % group->count_across;
    group->members[group->member_count] = object;
    ++group->member_count;
}

void __cdecl InitFn_GetCharacterGoalSpeedFn(AI_CHARACTER_GOAL_SPEED_PC callback) {
    GetCharacterGoalSpeedFn = callback;
}

void InitFn_FindAlternativeSpecialObjectFn(AIFINDALTERNATIVESPECIALOBJECT *function) {
    AISysSpecialNodeCallback = (AISYS_SPECIAL_NODE_CALLBACK_PC)function;
}

void AIRetreatFromDestination2(AISYS_PC *, AIPACKET_PC *, APIOBJECT_PC *, i32);
static u32 CalculateRightIntersection(APIOBJECT_PC *object, AIPATHCNX_PC *first_connection,
                                      AIPATHCNX_PC *second_connection, i32 first_angle, i32 second_angle,
                                      AIPATHNODE_PC *before, AIPATHNODE_PC *shared, AIPATHNODE_PC *after,
                                      NUVEC *result) {
    f32 clearance;
    f32 before_radius;
    f32 shared_radius;
    f32 after_radius;
    NUVEC first_start;
    NUVEC first_end;
    i32 angle;
    NUVEC second_start;
    NUVEC second_end;
    NUVEC first_origin;
    NUVEC first_direction;
    NUVEC second_origin;
    NUVEC second_direction;
    f32 first_parameter;
    f32 second_parameter;

    clearance = object->collision_radius + AIPathConnectionMargin;
    before_radius = before->radius - clearance;
    if (before_radius < 0.0f) {
        before_radius = 0.0f;
    }
    shared_radius = shared->radius - clearance;
    if (shared_radius < 0.0f) {
        shared_radius = 0.0f;
    }
    after_radius = after->radius - clearance;
    if (after_radius < 0.0f) {
        after_radius = 0.0f;
    }
    first_start.x = before_radius;
    first_start.y = 0.0f;
    first_start.z = 0.0f;
    first_end.x = before_radius;
    first_end.y = 0.0f;
    first_end.z = first_connection->horizontal_distance;
    angle =
        NuAngAdd(first_angle, PCPathArcSine((shared_radius - before_radius) / first_connection->horizontal_distance));
    NuVecRotateY(&first_start, &first_start, angle);
    NuVecRotateY(&first_end, &first_end, angle);
    NuVecAdd(&first_start, &first_start, &before->position);
    NuVecAdd(&first_end, &first_end, &before->position);

    second_start.x = -after_radius;
    second_start.y = 0.0f;
    second_start.z = 0.0f;
    second_end.x = -after_radius;
    second_end.y = 0.0f;
    second_end.z = second_connection->horizontal_distance;
    angle =
        NuAngSub(second_angle, PCPathArcSine((shared_radius - after_radius) / second_connection->horizontal_distance));
    NuVecRotateY(&second_start, &second_start, angle);
    NuVecRotateY(&second_end, &second_end, angle);
    NuVecAdd(&second_start, &second_start, &after->position);
    NuVecAdd(&second_end, &second_end, &after->position);

    first_origin = first_start;

    NuVecSub(&first_direction, &first_end, &first_origin);
    second_origin = second_start;

    NuVecSub(&second_direction, &second_end, &second_origin);

    if (NuLineLineIntersect(&first_origin, &first_direction, &second_origin, &second_direction, &first_parameter,
                            &second_parameter) != 0 &&
        first_parameter >= 0.0f && second_parameter >= 0.0f && first_parameter <= 1.0f && second_parameter <= 1.0f) {
        first_origin.y = shared->position.y * first_parameter + before->position.y * (1.0f - first_parameter);
        NuVecScale(&first_direction, &first_direction, first_parameter);
        NuVecAdd(result, &first_origin, &first_direction);
        return 1;
    }
    return 0;
}

static u32 CalculateIntersection(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object,
                                 AIPATHCNX_PC *current_connection, AIPATHCNX_PC *target_connection) {
    AIPATH_PC *path;
    AIPATHNODE_PC *before;
    AIPATHNODE_PC *shared;
    AIPATHNODE_PC *after;
    i32 first_angle;
    i32 second_angle;
    u32 right;
    u32 left;

    if (packet->intersection_connection != current_connection ||
        packet->intersection_target_connection != target_connection) {
        packet->flags &= ~(((0x20u | 0x40u)) << 0);
        packet->intersection_connection = current_connection;
        packet->intersection_target_connection = target_connection;

        path = packet->path_info.path;

        if (current_connection->node_indices[0] == target_connection->node_indices[0]) {
            before = &path->nodes[current_connection->node_indices[1]];
            shared = &path->nodes[current_connection->node_indices[0]];
            after = &path->nodes[target_connection->node_indices[1]];
            first_angle = NuAngAdd(current_connection->rotation, 0x8000);
            second_angle = NuAngAdd(target_connection->rotation, 0x8000);
        } else if (current_connection->node_indices[0] == target_connection->node_indices[1]) {
            before = &path->nodes[current_connection->node_indices[1]];
            shared = &path->nodes[current_connection->node_indices[0]];
            after = &path->nodes[target_connection->node_indices[0]];
            first_angle = NuAngAdd(current_connection->rotation, 0x8000);
            second_angle = target_connection->rotation;
        } else if (current_connection->node_indices[1] == target_connection->node_indices[0]) {
            before = &path->nodes[current_connection->node_indices[0]];
            shared = &path->nodes[current_connection->node_indices[1]];
            after = &path->nodes[target_connection->node_indices[1]];
            first_angle = current_connection->rotation;
            second_angle = NuAngAdd(target_connection->rotation, 0x8000);
        } else if (current_connection->node_indices[1] == target_connection->node_indices[1]) {
            before = &path->nodes[current_connection->node_indices[0]];
            shared = &path->nodes[current_connection->node_indices[1]];
            after = &path->nodes[target_connection->node_indices[0]];
            first_angle = current_connection->rotation;
            second_angle = target_connection->rotation;
        } else {
            return 0;
        }
        if (WithinConnection(system, &after->position, path, current_connection, 1, current_connection,
                             packet->current_route, object->object_id, NULL, 0.0f, 1) != 0) {
            return 0;
        }
        right = CalculateRightIntersection(object, current_connection, target_connection, first_angle, second_angle,
                                           before, shared, after, &packet->right_diversion);
        packet->flags = (packet->flags & ~0x20u) | ((right & 1) << 5);
        left = CalculateRightIntersection(object, target_connection, current_connection, second_angle, first_angle,
                                          after, shared, before, &packet->left_diversion);
        packet->flags = (packet->flags & ~0x40u) | ((left & 1) << 6);
    }
    return ((packet->flags & 0x00000020u) != 0) | ((packet->flags & 0x00000040u) != 0);
}

static void AIMoveFindDivertNode(AISYS_PC *system, AIPATH_PC *path, AIPACKET_PC *packet, NUVEC *destination) {
    AIPATHNODELINK_PC *links;
    f32 nearest;
    u8 initial_index;
    i32 index;
    f32 distance;
    f32 nearest_2;
    i32 index_2;
    f32 distance_2;

    if (path->special_route_count != 0 && (packet->navigation_flags & 4u) != 0) {
        links = path->special_routes;
        if (packet->divert_node_index >= path->node_count) {
            packet->divert_node_index = 0xff;
        }
        if (packet->divert_search_index >= path->special_route_count) {
            packet->divert_search_index = 0;
        }
        nearest = FLT_MAX;
        if (packet->divert_node_index < path->node_count) {
            nearest = NuVecDistSqr(&path->nodes[packet->divert_node_index].position, destination, NULL);
        }
        initial_index = packet->divert_search_index;
        for (index = 0; index < 4; ++index) {
            if (packet->divert_search_index != packet->divert_node_index) {
                distance = NuVecDistSqr(&path->nodes[links[packet->divert_search_index].node_index].position,
                                        destination, NULL);
                if (distance < nearest) {
                    nearest = distance;
                    packet->divert_node_index = links[packet->divert_search_index].node_index;
                }
            }
            ++packet->divert_search_index;
            if (packet->divert_search_index > path->special_route_count) {
                packet->divert_search_index = 0;
            }
            if (packet->divert_search_index == initial_index) {
                break;
            }
        }
    } else {
        if (packet->divert_node_index >= path->node_count) {
            packet->divert_node_index = 0xff;
        }
        if (packet->divert_search_index >= path->node_count) {
            packet->divert_search_index = 0;
        }
        nearest_2 = FLT_MAX;
        if (packet->divert_node_index < path->node_count) {
            nearest_2 = NuVecDistSqr(&path->nodes[packet->divert_node_index].position, destination, NULL);
        }
        for (index_2 = 0; index_2 < 4; ++index_2) {
            distance_2 = NuVecDistSqr(&path->nodes[packet->divert_search_index].position, destination, NULL);
            if (distance_2 < nearest_2) {
                nearest_2 = distance_2;
                packet->divert_node_index = packet->divert_search_index;
            }
            ++packet->divert_search_index;
            if (packet->divert_search_index > path->node_count) {
                packet->divert_search_index = 0;
            }
        }
    }
}

static i32 AIMoveApplyDivertNode(AIPACKET_PC *packet) {
    AIPATHNODE_PC *node;
    f32 radius;

    node = &packet->path_info.path->nodes[packet->divert_node_index];

    if (node->connection_count == 0) {
        return 0;
    }
    packet->fallback_destination = node->position;
    packet->fallback_stopping_distance = 0.0f;
    radius = node->radius - 1.0f;
    packet->movement_parameter = radius > 1.0f ? radius : 1.0f;
    memset(&packet->fallback_path_info, 0, sizeof(packet->fallback_path_info));
    packet->fallback_path_info.path = packet->path_info.path;
    packet->fallback_path_info.connection = node->connections[0];
    packet->fallback_path_info.direction = node->connections[0]->node_indices[0] == packet->divert_node_index;
    packet->fallback_path_info.distance =
        node->connections[0]->node_indices[0] == packet->divert_node_index ? 0.0f : 1.0f;
    return 1;
}
static i32 AIMoveAdjustDestinationPath(AISYS_PC *system, AIPACKET_PC *packet) {

    if (packet->path_info.path == packet->fallback_path_info.path && packet->fallback_path_info.connection != NULL) {
        return 0;
    }
    AIMoveFindDivertNode(system, packet->path_info.path, packet, &packet->fallback_destination);
    if (packet->divert_node_index >= packet->path_info.path->node_count) {
        return -1;
    }
    return AIMoveApplyDivertNode(packet) != 0 ? 1 : -1;
}

static i32 AIMoveFindNearestSpecialRouteNode(AIPATH_PC *path, NUVEC *destination, i32 *nearest_node, f32 *distance) {
    i32 node;
    f32 nearest;
    i32 index;
    u8 candidate_node;
    f32 candidate;

    node = -1;
    nearest = FLT_MAX;

    for (index = 0; index < path->special_route_count; ++index) {
        candidate_node = path->special_routes[index].node_index;
        candidate = NuVecDistSqr(&path->nodes[candidate_node].position, destination, NULL);
        if (candidate < nearest) {
            nearest = candidate;
            node = candidate_node;
        }
    }
    if (node < 0) {
        return 0;
    }
    *nearest_node = node;
    *distance = nearest;
    return 1;
}
static i32 AIMoveChooseExitNodePath(AISYS_PC *system, AIPACKET_PC *packet) {
    AIPATHNODE_PC *first;
    AIPATHNODE_PC *second;
    f64 first_distance;
    f32 second_distance;
    AIPATHNODE_PC *node;
    AIPATHSPECIALROUTE_PC *route;
    i32 index;
    f32 nearest;
    AIPATH_PC *selected;
    i32 nearest_node;
    f32 nearest_node_distance;

    first = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[0]];
    second = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[1]];
    first_distance = (f32)(NuVecDistSqr(&packet->owner->position, &first->position, NULL) - first->radius_squared);
    second_distance = NuVecDistSqr(&packet->owner->position, &second->position, NULL) - second->radius_squared;
    node = first_distance < second_distance ? first : second;

    selected = NULL;

    if (node->special_route_index >= packet->path_info.path->special_route_count) {
        return 0;
    }
    route =
        &system->path_sys
             ->special_routes[packet->path_info.path->special_routes[node->special_route_index].special_route_index];
    for (index = 0; index < route->path_count; ++index) {
        if (route->paths[index] == packet->fallback_path_info.path) {
            AISysCharacterSetPath(packet, packet->fallback_path_info.path);
            return 1;
        }
    }
    nearest = FLT_MAX;
    for (index = 0; index < route->path_count; ++index) {
        /* PC ignores success. With no nearest node the output is stale or uninitialized. */
        AIMoveFindNearestSpecialRouteNode(route->paths[index], &packet->fallback_destination, &nearest_node,
                                          &nearest_node_distance);
        if (nearest_node_distance < nearest) {
            nearest = nearest_node_distance;
            selected = route->paths[index];
        }
    }
    if (selected != NULL) {
        AISysCharacterSetPath(packet, selected);
        return 1;
    }
    return 0;
}

static AIPATHCNX_PC *GetNextConnection(const AIPACKET_PC *packet, i32 *direction) {
    AIPATH_PC *path;
    i32 node_index;
    AIPATHNODE_PC *node;
    u32 destination;
    AIPATHCNX_PC *connection;
    i32 connection_index;
    u8 route_index;
    AIPATHROUTE_PC *route;
    i32 next_node;
    f32 nearest;
    i32 index;
    f32 first;
    f32 second;
    f32 distance;

    if (packet->goal_path_node == NULL || packet->path_info.connection == NULL) {
        return NULL;
    }
    path = packet->path_info.path;
    node_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
    node = &path->nodes[node_index];
    destination = packet->goal_path_node - path->nodes;
    if (destination > 0xff) {
        return NULL;
    }
    connection = NULL;
    connection_index = path->route_matrix[node_index][destination];
    if (connection_index < node->connection_count) {
        connection = node->connections[connection_index];
        *direction = connection->node_indices[0] != node_index;
    }
    route_index = packet->current_route;
    if (route_index == 0xff) {
        return connection;
    }
    if ((((u64)1 << route_index) & node->route_membership_mask) == 0) {
        return NULL;
    }
    if ((((u64)1 << route_index) & node->route_boundary_mask) != 0) {
        if (connection == NULL) {
            return NULL;
        }
        if ((((u64)1 << route_index) & connection->route_mask) == 0) {
            return connection;
        }
    }
    route = &path->routes[route_index];
    next_node = destination;
    if ((((u64)1 << route_index) & path->nodes[destination].route_membership_mask) == 0) {
        if (route->exit_node_count == 0) {
            return NULL;
        }
        nearest = FLT_MAX;
        next_node = 0;
        for (index = 0; index < route->exit_node_count; ++index) {
            first = AIPathNodeDistanceToPathNode(packet->path_info.path, node_index, route->exit_nodes[index],
                                                 packet->current_route, 0);
            second = AIPathNodeDistanceToPathNode(packet->path_info.path, route->exit_nodes[index], destination, 0xff,
                                                  (u32)((u64)(1) << packet->current_route));
            distance = first != FLT_MAX && second != FLT_MAX ? first + second : FLT_MAX;
            if (distance < nearest) {
                nearest = distance;
                next_node = route->exit_nodes[index];
            }
        }
        if (nearest == FLT_MAX) {
            return NULL;
        }
    }
    connection_index = route->route_nodes[route->node_routes[node_index]][route->node_routes[next_node]];
    if (connection_index >= node->connection_count) {
        return NULL;
    }
    connection = node->connections[connection_index];
    *direction = connection->node_indices[0] != node_index;
    return connection;
}

void AIMoveToDestination(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    i32 direction;
    NUVEC saved_destination;
    f32 saved_stopping_distance;
    f32 saved_parameter;
    AIPATHINFO_PC saved_path_info;
    AIPATH_PC *path;
    i32 diverted;
    AIPATHCNX_PC *connection;
    AIPATHCNX_PC *destination_connection;
    NUVEC difference;
    f32 distance_squared;
    AIPATHCNX_PC *target;
    AIPATHCNX_PC *current;
    i32 goal_index;
    f32 current_distance;
    f32 endpoint_distances[2];
    f32 best_distance;
    u32 attempted_routes;
    f32 destination_distances[2];
    i32 end;
    AIPATHNODE_PC *node;
    f32 candidate;
    f32 candidate_2;
    f32 candidate_3;
    f32 candidate_4;
    AIPATHCNX_PC *next;
    u32 flags;
    AIPATHCNX_PC *current_2;
    u32 flags_2;
    AIPATHNODE_PC *shared;
    AIPATHCNX_PC *next_2;
    NUVEC target_2;
    i32 use_left;
    NUVEC offset;
    i32 angle;
    i32 angle_2;
    NUVEC offset_2;

    direction = 0;
    saved_destination = packet->fallback_destination;
    saved_stopping_distance = packet->fallback_stopping_distance;
    saved_parameter = packet->movement_parameter;
    saved_path_info = packet->fallback_path_info;
    path = packet->path_info.path;
    diverted = AIMoveAdjustDestinationPath(system, packet);
    if (diverted < 0) {
        packet->movement_destination = packet->owner->position;
        packet->movement_stopping_distance = 0.0f;
        return;
    }
    path = packet->path_info.path;
    connection = packet->path_info.connection;
    destination_connection = packet->fallback_path_info.connection;

    distance_squared = NuVecDistSqr(&packet->fallback_destination, &object->position, &difference);
    if ((object->supporting_platform_id == -1 || (path->nodes[connection->node_indices[0]].has_special != 0 &&
                                                  path->nodes[connection->node_indices[1]].has_special != 0)) &&
        packet->movement_parameter != 0.0f && difference.y < AIMovementInstructionPositiveThreshold &&
        difference.y > AIMovementInstructionNegativeThreshold &&
        distance_squared < packet->movement_parameter * packet->movement_parameter) {
        if (diverted == 0) {
            packet->movement_destination = object->position;
            packet->movement_stopping_distance = 0.0f;
            return;
        }
        if (0 != ((packet->navigation_flags & 4))) {
            packet->fallback_destination = saved_destination;
            packet->fallback_stopping_distance = saved_stopping_distance;
            packet->movement_parameter = saved_parameter;
            packet->fallback_path_info = saved_path_info;
            AIMoveChooseExitNodePath(system, packet);
            AIMoveAdjustDestinationPath(system, packet);
            path = packet->path_info.path;
            connection = packet->path_info.connection;
            destination_connection = packet->fallback_path_info.connection;
        }
    }
    if (connection == NULL || path == NULL || destination_connection == NULL) {
        packet->movement_destination = packet->owner->position;
        packet->movement_stopping_distance = 0.0f;
        return;
    }
    if (packet->path_info.connection != packet->fallback_path_info.connection) {
        if (WithinConnection(system, &packet->terrain_origin, packet->path_info.path,
                             packet->fallback_path_info.connection, checks, packet->path_info.connection,
                             packet->current_route, object->object_id, NULL, object->collision_radius, 0) != 0) {
            target = packet->fallback_path_info.connection;
            current = packet->path_info.connection;
            direction = target->node_indices[0] != current->node_indices[0] &&
                        target->node_indices[0] != current->node_indices[1];
            AISysCharacterSetPathCnx(packet, &object->position, target, direction);
        }
        if (packet->path_info.connection != packet->fallback_path_info.connection) {
            direction = packet->path_info.direction;
            goal_index = destination_connection->node_indices[0];
            current_distance = packet->path_info.distance;
            if (current_distance > 1.0f) {
                current_distance = 1.0f;
            } else if (current_distance < 0.0f) {
                current_distance = 0.0f;
            }
            endpoint_distances[0] = (f32)fabs(current_distance * connection->distance);
            endpoint_distances[1] = (f32)fabs((1.0f - current_distance) * connection->distance);
            if (packet->current_route != 0xff) {
                packet->flags |= 0x00100000u;
            }
            best_distance = FLT_MAX;
            attempted_routes = 0;
            for (;;) {
                destination_distances[0] =
                    (f32)fabs(packet->fallback_path_info.distance * destination_connection->distance);
                destination_distances[1] =
                    (f32)fabs((1.0f - packet->fallback_path_info.distance) * destination_connection->distance);
                if (packet->current_route != 0xff &&
                    (((u64)1 << packet->current_route) & (destination_connection->route_mask)) == 0) {
                    for (end = 0; 2 > end; ++end) {
                        node = &path->nodes[destination_connection->node_indices[end]];
                        if (0 != (((u64)1 << packet->current_route) &
                                  (node->route_membership_mask & ~node->route_boundary_mask))) {
                            destination_distances[end] = FLT_MAX;
                        }
                    }
                }
                if ((connection->traversal_flags[packet->path_info.direction] & 0x40000000) == 0) {
                    {
                        candidate = FLT_MAX;
                        if (destination_distances[0] != FLT_MAX &&
                            endpoint_distances[0 == packet->path_info.direction] != FLT_MAX) {
                            candidate = AIPathNodeDistanceToPathNode(
                                packet->path_info.path, connection->node_indices[packet->path_info.direction == 0],
                                destination_connection->node_indices[0], packet->current_route, 0);
                            if (candidate != FLT_MAX) {
                                candidate +=
                                    endpoint_distances[packet->path_info.direction == 0] + destination_distances[0];
                            }
                        }
                        if (candidate < best_distance) {
                            best_distance = candidate;
                            direction = packet->path_info.direction;
                            goal_index = destination_connection->node_indices[0];
                            packet->flags &= ~0x00100000u;
                        }
                    }
                    {
                        candidate_2 = FLT_MAX;
                        if (destination_distances[1] != FLT_MAX &&
                            endpoint_distances[packet->path_info.direction == 0] != FLT_MAX) {
                            candidate_2 = AIPathNodeDistanceToPathNode(
                                packet->path_info.path, connection->node_indices[packet->path_info.direction == 0],
                                destination_connection->node_indices[1], packet->current_route, 0);
                            if (candidate_2 != FLT_MAX) {
                                candidate_2 +=
                                    endpoint_distances[packet->path_info.direction == 0] + destination_distances[1];
                            }
                        }
                        if (candidate_2 < best_distance) {
                            best_distance = candidate_2;
                            goal_index = destination_connection->node_indices[1];
                            direction = packet->path_info.direction;
                            packet->flags &= ~0x00100000u;
                        }
                    }
                }
                if ((connection->traversal_flags[0 == packet->path_info.direction] & 0x40000000) == 0) {
                    {
                        candidate_3 = FLT_MAX;
                        if (destination_distances[0] != FLT_MAX &&
                            endpoint_distances[packet->path_info.direction] != FLT_MAX) {
                            candidate_3 = AIPathNodeDistanceToPathNode(
                                packet->path_info.path, connection->node_indices[packet->path_info.direction],
                                destination_connection->node_indices[0], packet->current_route, 0);
                            if (candidate_3 != FLT_MAX) {
                                candidate_3 +=
                                    destination_distances[0] + endpoint_distances[packet->path_info.direction];
                            }
                        }
                        if (candidate_3 < best_distance) {
                            best_distance = candidate_3;
                            direction = packet->path_info.direction == 0;
                            goal_index = destination_connection->node_indices[0];
                            packet->flags &= ~0x00100000u;
                        }
                    }
                    {
                        candidate_4 = FLT_MAX;
                        if (destination_distances[1] != FLT_MAX &&
                            endpoint_distances[packet->path_info.direction] != FLT_MAX) {
                            candidate_4 = AIPathNodeDistanceToPathNode(
                                packet->path_info.path, connection->node_indices[packet->path_info.direction],
                                destination_connection->node_indices[1], packet->current_route, 0);
                            if (candidate_4 != FLT_MAX) {
                                candidate_4 = candidate_4 + (destination_distances[1] +
                                                             endpoint_distances[packet->path_info.direction]);
                            }
                        }
                        if (candidate_4 < best_distance) {
                            best_distance = candidate_4;
                            direction = packet->path_info.direction == 0;
                            goal_index = destination_connection->node_indices[1];
                            packet->flags &= ~0x00100000u;
                        }
                    }
                }
                if ((packet->flags & 0x00100000u) == 0) {
                    break;
                }
                if (packet->current_route != 0xff) {
                    attempted_routes |= 1u << packet->current_route;
                }
                AISysFindRoute(packet);
                if (((attempted_routes >> packet->current_route) & 1) != 0) {
                    return;
                }
            }
            packet->goal_path_node = &path->nodes[goal_index];
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            next = GetNextConnection(packet, &direction);
            if (next != NULL) {
                while (WithinConnection(system, &packet->terrain_origin, packet->path_info.path, next, checks,
                                        packet->path_info.connection, packet->current_route, object->object_id, NULL,
                                        object->collision_radius, 0) != 0) {
                    if (connection == destination_connection) {
                        break;
                    }
                    AISysCharacterSetPathCnx(packet, &object->position, next, direction);
                    if (packet->path_info.connection != next || packet->path_info.direction != direction) {
                        connection = next;
                        break;
                    }
                    connection = next;
                    next = GetNextConnection(packet, &direction);
                    if (next == connection || next == NULL) {
                        break;
                    }
                }
            } else {
                flags = destination_connection->traversal_flags[0] | destination_connection->traversal_flags[1];
                if ((flags & 0xd8000000) == 0 &&
                    ((flags & 0xdfffffff) == 0 || (packet->capabilities & (flags & 0xdfffffff)) != 0) &&
                    WithinConnection(system, &packet->terrain_origin, packet->path_info.path, destination_connection,
                                     checks, packet->path_info.connection, packet->current_route, object->object_id,
                                     NULL, object->collision_radius, 0) != 0) {
                    AISysCharacterSetPathCnx(packet, &object->position, destination_connection, 0);
                }
            }
        }
    }
    if (PreparingForSpecialMoveFn != NULL) {
        if (connection != destination_connection) {
            packet->flags |= 0x20000000u;
        }
        if (PreparingForSpecialMoveFn(packet, object, checks) != 0) {
            packet->flags |= 0x00080000u;
            packet->flags |= 0x40000000u;
            return;
        }
    }
    current_2 = packet->path_info.connection;
    flags_2 = current_2->traversal_flags[packet->path_info.direction];
    if ((flags_2 & 0xd8000000) != 0) {
        packet->movement_destination = path->nodes[current_2->node_indices[packet->path_info.direction]].position;
        packet->flags |= 0x00400000u;
        packet->movement_stopping_distance = 0.0f;
        packet->frame_state = flags_2;
        return;
    }
    if (current_2 == destination_connection &&
        (packet->current_route == 0xff ||
         (((u64)1 << packet->current_route) & (destination_connection->route_mask)) != 0)) {
        if ((packet->path_info.flags & 1u) == 0 || packet->movement_parameter == 0.0f) {
            packet->movement_stopping_distance = packet->fallback_stopping_distance;
            packet->movement_destination = packet->fallback_destination;
            return;
        }
        if (distance_squared < packet->movement_parameter * packet->movement_parameter) {
            if (diverted == 0) {
                packet->movement_destination = object->position;
                packet->movement_stopping_distance = 0.0f;
                return;
            }
            if ((packet->navigation_flags & 4) != 0) {
                packet->fallback_destination = saved_destination;
                packet->fallback_stopping_distance = saved_stopping_distance;
                packet->movement_parameter = saved_parameter;
                packet->fallback_path_info = saved_path_info;
                AIMoveChooseExitNodePath(system, packet);
                AIMoveAdjustDestinationPath(system, packet);
            }
        }
        NuVecScale(&difference, &difference, packet->movement_parameter / NuFsqrt(distance_squared));
        packet->movement_destination.x = packet->fallback_destination.x - difference.x;
        packet->movement_destination.y = packet->fallback_destination.y;
        packet->movement_destination.z = packet->fallback_destination.z - difference.z;
        packet->movement_stopping_distance =
            packet->fallback_stopping_distance < packet->movement_parameter ? 0.0f : packet->fallback_stopping_distance;
        return;
    }
    shared = NULL;
    if (destination_connection->node_indices[0] == current_2->node_indices[0]) {
        shared = &path->nodes[destination_connection->node_indices[0]];
        direction = 0;
    } else if (current_2->node_indices[0] == destination_connection->node_indices[1]) {
        shared = &path->nodes[destination_connection->node_indices[1]];
        direction = 1;
    } else if (current_2->node_indices[1] == destination_connection->node_indices[0]) {
        shared = &path->nodes[destination_connection->node_indices[0]];
        direction = 0;
    } else if (current_2->node_indices[1] == destination_connection->node_indices[1]) {
        shared = &path->nodes[destination_connection->node_indices[1]];
        direction = 1;
    }

    if (shared != NULL && (packet->current_route == 0xff ||
                           (((u64)1 << packet->current_route) & (destination_connection->route_mask)) != 0 ||
                           (((u64)1 << packet->current_route) & (shared->route_boundary_mask)) != 0)) {
        next_2 = destination_connection;
    } else {
        next_2 = GetNextConnection(packet, &direction);
    }
    if (next_2 != NULL) {
        flags_2 = next_2->traversal_flags[direction];
        if ((flags_2 & 0xdfffffff) != 0) {
            if ((flags_2 & 0xd8000000) != 0) {
                packet->movement_destination = object->position;
                packet->flags |= 0x00400000u;
                packet->movement_stopping_distance = 0.0f;
                packet->frame_state = flags_2;
                return;
            }
            if ((packet->capabilities & flags_2) != 0) {
                packet->navigation_flags |= 1;
            } else {
                packet->flags |= 0x00600000u;
                packet->frame_state = next_2->traversal_flags[direction];
                next_2 = NULL;
            }
        }
        if (next_2 != NULL) {

            if (next_2 == destination_connection) {
                target_2 = packet->fallback_destination;
            } else {
                target_2 = path->nodes[next_2->node_indices[direction]].position;
                packet->flags |= 0x00080000u;
            }
            if (WithinConnection(system, &target_2, packet->path_info.path, current_2, checks,
                                 packet->path_info.connection, packet->current_route, object->object_id, NULL,
                                 object->collision_radius, 0) == 0) {
                if (CalculateIntersection(system, packet, object, current_2, next_2) != 0) {
                    use_left = 0;

                    if ((packet->flags & 0x00000040u) != 0) {
                        NuVecSub(&offset, &packet->left_diversion, &object->position);
                        angle = NuAtan2D(offset.x, offset.z);
                        NuVecSub(&offset, &target_2, &object->position);
                        if (NuAngSub(NuAtan2D(offset.x, offset.z), angle) < 0) {
                            target_2 = packet->left_diversion;
                            packet->flags |= 0x00080000u;
                            use_left = 1;
                        }
                    }
                    if (!use_left && (packet->flags & 0x00000020u) != 0) {
                        NuVecSub(&offset, &packet->right_diversion, &object->position);
                        angle_2 = NuAtan2D(offset.x, offset.z);
                        NuVecSub(&offset, &target_2, &object->position);
                        if (NuAngSub(NuAtan2D(offset.x, offset.z), angle_2) > 0) {
                            target_2 = packet->right_diversion;
                            packet->flags |= 0x00080000u;
                        }
                    }
                } else {
                    target_2 = path->nodes[next_2->node_indices[direction]].position;
                }
            }
            packet->movement_stopping_distance = 0.0f;
            packet->movement_destination = target_2;
            return;
        }
    }
    packet->movement_destination =
        path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]].position;
    packet->movement_stopping_distance = 0.0f;

    if (NuVecDistSqr(&packet->movement_destination, &object->position, &offset_2) < 1.0f) {
        packet->movement_destination = object->position;
    }
    packet->flags |= 0x00400000u;
}

static void AIRetreatFromDestinationPath(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    NUVEC difference;
    AIPATHCNX_PC *connection;
    AIPATH_PC *path;
    AIPATHCNX_PC *destination_connection;
    f32 parameter;
    f32 remaining_parameter;
    f32 destination_parameter;
    f32 destination_a;
    f32 destination_b;
    f32 length;
    AIPATHNODE_PC *node;
    f32 best_distance;
    i32 direction;
    f32 partial_distance;
    f32 candidate;
    f32 partial_distance_2;
    f32 candidate_2;
    u8 end_node;
    AIPATHNODE_PC *node_2;
    i32 start;
    i32 index;
    AIPATHCNX_PC *target;
    u8 old_direction;
    AIPATHNODE_PC *node_3;
    i32 index_2;
    AIPATHCNX_PC *target_2;
    i32 angle;
    i32 angle_2;

    connection = packet->path_info.connection;
    if (connection == NULL || packet->fallback_path_info.connection == NULL ||
        packet->path_info.path != packet->fallback_path_info.path) {
        return;
    }
    path = packet->path_info.path;
    destination_connection = packet->fallback_path_info.connection;
    if (connection == destination_connection) {
        AISysCharacterSetPathCnx(packet, &object->position, connection,
                                 packet->fallback_path_info.distance > packet->path_info.distance);
    } else {
        parameter = packet->path_info.distance;
        if (parameter > 1.0f) {
            parameter = 1.0f;
        } else if (parameter < 0.0f) {
            parameter = 0.0f;
        }
        remaining_parameter = 1.0f - parameter;
        destination_parameter = packet->fallback_path_info.distance;
        destination_a = (f32)fabs(destination_parameter * destination_connection->distance);
        destination_b = (f32)fabs((1.0f - destination_parameter) * destination_connection->distance);
        length = connection->distance;
        if (packet->current_route != 0xff &&
            (((u64)1 << packet->current_route) & (destination_connection->route_mask)) == 0) {
            node = &path->nodes[destination_connection->node_indices[0]];
            if (((u64)1 << packet->current_route) & (node->route_membership_mask & ~node->route_boundary_mask)) {
                destination_a = FLT_MAX;
            }
            node = &path->nodes[destination_connection->node_indices[1]];
            if (((u64)1 << packet->current_route) & (node->route_membership_mask & ~node->route_boundary_mask)) {
                destination_b = FLT_MAX;
            }
        }

        best_distance = 0.0f;
        direction = 0;
        if ((connection->traversal_flags[1] & 0x40000000u) == 0) {
            partial_distance = (f32)fabs(parameter * length);
            candidate =
                AIPathNodeDistanceToPathNode(path, connection->node_indices[0], destination_connection->node_indices[0],
                                             packet->current_route, 0) +
                partial_distance + destination_a;
            if (candidate > best_distance) {
                best_distance = candidate;
                direction = 1;
            }
            candidate = AIPathNodeDistanceToPathNode(
                            packet->path_info.path, packet->path_info.connection->node_indices[0],
                            packet->fallback_path_info.connection->node_indices[1], packet->current_route, 0) +
                        partial_distance + destination_b;
            if (candidate > best_distance) {
                best_distance = candidate;
                direction = 1;
            }
            connection = packet->path_info.connection;
        }
        if ((connection->traversal_flags[0] & 0x40000000u) == 0) {
            partial_distance_2 = (f32)fabs(length * remaining_parameter);
            candidate_2 = AIPathNodeDistanceToPathNode(packet->path_info.path, connection->node_indices[1],
                                                       packet->fallback_path_info.connection->node_indices[0],
                                                       packet->current_route, 0) +
                          partial_distance_2 + destination_a;
            if (candidate_2 > best_distance) {
                best_distance = candidate_2;
                direction = 0;
            }
            candidate_2 = AIPathNodeDistanceToPathNode(
                              packet->path_info.path, packet->path_info.connection->node_indices[1],
                              packet->fallback_path_info.connection->node_indices[1], packet->current_route, 0) +
                          partial_distance_2 + destination_b;
            if (candidate_2 > best_distance) {
                direction = 0;
            }
            connection = packet->path_info.connection;
        }
        AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
    }

    connection = packet->path_info.connection;
    end_node = connection->node_indices[packet->path_info.direction == 0];
    packet->goal_path_node = &path->nodes[end_node];
    if (packet->movement_target != NULL && packet->movement_target->node_indices[0] != end_node &&
        packet->movement_target->node_indices[1] != end_node) {
        packet->movement_target = NULL;
    }
    if (packet->movement_target == NULL) {
        node_2 = &packet->path_info.path->nodes[end_node];
        start = NuRandInt() % node_2->connection_count;
        connection = packet->path_info.connection;
        for (index = 0; index < node_2->connection_count; ++index) {
            packet->movement_target = node_2->connections[(index + start) % node_2->connection_count];
            target = packet->movement_target;
            if (target != connection &&
                (packet->current_route == 0xff || (((u64)1 << packet->current_route) & (target->route_mask)) != 0 ||
                 (((u64)1 << packet->current_route) & (node_2->route_boundary_mask)) != 0) &&
                target->traversal_flags[target->node_indices[0] != end_node] == 0) {
                break;
            }
            packet->movement_target = NULL;
        }
    }
    if (packet->movement_target == NULL) {
        old_direction = packet->path_info.direction;
        end_node = connection->node_indices[old_direction];
        node_3 = &path->nodes[end_node];
        for (index_2 = 0; index_2 < node_3->connection_count; ++index_2) {
            packet->movement_target = node_3->connections[index_2];
            target_2 = packet->movement_target;
            if (target_2 != connection && target_2->traversal_flags[target_2->node_indices[0] != end_node] == 0) {
                AISysCharacterSetPathCnx(packet, &object->position, connection, old_direction == 0);
                connection = packet->path_info.connection;
                end_node = connection->node_indices[packet->path_info.direction == 0];
                packet->goal_path_node = &path->nodes[end_node];
                break;
            }
            packet->movement_target = NULL;
        }
    }
    if (packet->movement_target == NULL) {
        packet->movement_destination = packet->goal_path_node->position;
        packet->movement_stopping_distance = 0.0f;
        return;
    }
    if (WithinConnection(system, &packet->terrain_origin, packet->path_info.path, packet->movement_target, checks,
                         connection, packet->current_route, object->object_id, NULL, object->collision_radius,
                         0) != 0) {
        AISysCharacterSetPathCnx(packet, &object->position, packet->movement_target,
                                 packet->movement_target->node_indices[0] != end_node);
        packet->goal_path_node =
            &path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        packet->movement_destination = packet->goal_path_node->position;
        packet->movement_target = NULL;
        packet->movement_stopping_distance = 0.0f;
        return;
    }

    packet->movement_destination =
        packet->path_info.path->nodes[packet->movement_target->node_indices[packet->movement_target_direction == 0]]
            .position;
    packet->movement_stopping_distance = 0.0f;
    if (CalculateIntersection(system, packet, object, packet->path_info.connection, packet->movement_target) == 0) {
        return;
    }
    if ((packet->flags & 0x00000040u) != 0) {
        NuVecSub(&difference, &packet->left_diversion, &object->position);
        angle = NuAtan2D(difference.x, difference.z);
        NuVecSub(&difference, &packet->movement_destination, &object->position);
        if (NuAngSub(NuAtan2D(difference.x, difference.z), angle) < 0) {
            packet->movement_destination = packet->left_diversion;
            return;
        }
    }
    if ((packet->flags & 0x00000020u) != 0) {
        NuVecSub(&difference, &packet->right_diversion, &object->position);
        angle_2 = NuAtan2D(difference.x, difference.z);
        NuVecSub(&difference, &packet->movement_destination, &object->position);
        if (NuAngSub(NuAtan2D(difference.x, difference.z), angle_2) > 0) {
            packet->movement_destination = packet->right_diversion;
        }
    }
}
void AIRetreatFromDestination(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    NUVEC difference;
    f32 distance;

    if (new_retreat != 0) {
        AIRetreatFromDestination2(system, packet, object, checks);
        return;
    }
    if ((packet->path_info.flags & 1u) != 0) {
        distance = NuVecXZDist(&object->position, &packet->fallback_destination, &difference);
        if (distance != 0.0f) {
            NuVecScale(&difference, &difference, packet->movement_parameter / distance);
        } else {
            difference.x = packet->movement_parameter;
            difference.y = 0.0f;
            difference.z = 0.0f;
        }
        NuVecAdd(&packet->movement_destination, &object->position, &difference);
        packet->movement_stopping_distance = 0.0f;
    } else {
        AIRetreatFromDestinationPath(system, packet, object, checks);
    }
}

void AIRetreatFromDestination2(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    NUVEC difference;
    f32 distance;
    NUVEC position;
    NUVEC path_position;
    AIPATHINFO_PC path_info;

    distance = NuVecXZDist(&object->position, &packet->fallback_destination, &difference);
    if (distance != 0.0f) {
        NuVecScale(&difference, &difference, packet->movement_parameter / distance);
    } else {
        difference.x = packet->movement_parameter;
        difference.y = 0.0f;
        difference.z = 0.0f;
    }
    NuVecAdd(&packet->movement_destination, &packet->fallback_destination, &difference);
    packet->movement_stopping_distance = 0.0f;
    distance = NuVecXZDist(&packet->movement_destination, &object->position, &difference);
    if (distance > ai_moveradius) {
        NuVecScale(&difference, &difference, ai_moveradius / distance);
        NuVecAdd(&packet->movement_destination, &object->position, &difference);
    }

    position = object->position;
    path_position = packet->last_path_position;
    path_info = packet->path_info;
    object->position = packet->movement_destination;
    AISysGetCharacterPathPos(system, object, packet, 0, 1);
    packet->movement_destination = packet->last_path_position;
    packet->path_info = path_info;
    object->position = position;
    packet->last_path_position = path_position;
}

void AIMoveToDestinationAvoidingCamera(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    NUVEC destination;

    destination = packet->fallback_destination;
    AIMoveToDestination(system, packet, object, checks);
    packet->fallback_destination = destination;
}

void AIWander(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    NUVEC difference;
    i32 node_index;
    u8 node_index_2;
    AIPATHNODE_PC *node;
    i32 start;
    i32 index;
    AIPATHCNX_PC *target;
    AIPATHNODE_PC *node_2;
    f32 distance;
    i32 angle;
    i32 angle_2;

    if (packet->movement_target != NULL) {
        node_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
        if (packet->movement_target->node_indices[0] != node_index &&
            packet->movement_target->node_indices[1] != node_index) {
            packet->movement_target = NULL;
        }
    }
    if (packet->movement_target == NULL) {
        node_index_2 = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
        node = &packet->path_info.path->nodes[node_index_2];
        start = NuRandInt() % node->connection_count;
        for (index = 0; index < node->connection_count; ++index) {
            packet->movement_target = node->connections[(index + start) % node->connection_count];
            target = packet->movement_target;
            if (target != packet->path_info.connection) {
                if (packet->current_route == 0xff || (((u64)1 << packet->current_route) & (target->route_mask)) != 0 ||
                    (((u64)1 << packet->current_route) & (node->route_boundary_mask)) != 0) {
                    packet->movement_target_direction = node_index_2 != target->node_indices[0];
                    if (target->traversal_flags[packet->movement_target_direction] == 0) {
                        break;
                    }
                }
            }
            packet->movement_target = NULL;
        }
    }
    if (packet->movement_target == NULL) {
        node_2 = &packet->path_info.path
                      ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        distance = NuVecXZDistSqr(&object->position, &node_2->position, &difference);
        if (distance < node_2->radius_squared) {
            AISysCharacterSetPathCnx(packet, &object->position, packet->path_info.connection,
                                     packet->path_info.direction == 0);
            node_2 = &packet->path_info.path
                          ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            packet->goal_path_node = node_2;
        }
        packet->movement_destination = node_2->position;
        packet->movement_stopping_distance = 0.0f;
        return;
    }

    packet->flags |= 0x00080000u;
    if (WithinConnection(system, &packet->terrain_origin, packet->path_info.path, packet->movement_target, checks,
                         packet->path_info.connection, packet->current_route, object->object_id, NULL,
                         object->collision_radius, 0) == 0) {
        packet->movement_destination =
            packet->path_info.path->nodes[packet->movement_target->node_indices[packet->movement_target_direction == 0]]
                .position;
        packet->movement_stopping_distance = 0.0f;
        if (CalculateIntersection(system, packet, object, packet->path_info.connection, packet->movement_target) != 0) {
            if ((packet->flags & 0x00000040u) != 0) {
                NuVecSub(&difference, &packet->left_diversion, &object->position);
                angle = NuAtan2D(difference.x, difference.z);
                NuVecSub(&difference, &packet->movement_destination, &object->position);
                if (NuAngSub(NuAtan2D(difference.x, difference.z), angle) < 0) {
                    packet->movement_destination = packet->left_diversion;
                    return;
                }
            }
            if ((packet->flags & 0x00000020u) != 0) {
                NuVecSub(&difference, &packet->right_diversion, &object->position);
                angle_2 = NuAtan2D(difference.x, difference.z);
                NuVecSub(&difference, &packet->movement_destination, &object->position);
                if (NuAngSub(NuAtan2D(difference.x, difference.z), angle_2) > 0) {
                    packet->movement_destination = packet->right_diversion;
                }
            }
        }
    } else {
        AISysCharacterSetPathCnx(packet, &object->position, packet->movement_target, packet->movement_target_direction);
        packet->goal_path_node =
            &packet->path_info.path
                 ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        packet->movement_destination = packet->goal_path_node->position;
        packet->movement_stopping_distance = 0.0f;
        packet->movement_target = NULL;
    }
}

void __cdecl AISysCharacterMovement(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    u32 mode;

    packet->flags &= ~0x20000000u;
    if (MidSpecialMoveFn != NULL && MidSpecialMoveFn(system, packet, object) != 0) {
        packet->flags |= 0x80080000u;
        return;
    }
    packet->movement_destination = object->position;
    packet->movement_stopping_distance = 0.0f;
    if (packet->path_info.path != NULL && packet->path_info.connection != NULL) {
        switch (packet->flags & 7) {
            case 1:
                AIMoveToDestination(system, packet, object, checks);
                break;
            case 2:
                AIRetreatFromDestination(system, packet, object, checks);
                break;
            case 3:
                AICircle(system, packet, object, checks);
                break;
            case 4:
                AIWander(system, packet, object, checks);
                break;
            case 6:
                AIMoveToDestinationAvoidingCamera(system, packet, object, checks);
                break;
            case 7:
                AIMoveDirectlyToDestination(system, packet, object, checks);
                break;
        }
    } else {
        mode = packet->flags & 7;
        if (mode == 1 || mode == 7 || mode == 6) {
            packet->movement_stopping_distance = packet->fallback_stopping_distance;
            packet->movement_destination = packet->fallback_destination;
        }
    }
    packet->flags &= ~7u;
}

static void pathEditor_PathNodeMoved(EDAIPATHNODE_s *node) {
    if (!(node->flags & 0x80) && NuSpecialExistsFn(&aieditor->cursor_platform)) {
        node->platform = aieditor->cursor_platform;
        NuVecInvMtxTransform(&node->platform_position, &node->position,
                             NuSpecialGetDrawMtx(&aieditor->cursor_platform));
    } else {
        node->special.scene = NULL;
        node->special.special = NULL;
        node->special.display_special = NULL;
    }
    creatureEditor_PathNodeMoved(node);
    locatorEditor_PathNodeMoved(node);
    if (AIPathNodeDeletedFn != NULL)
        AIPathNodeDeletedFn(aieditor->current_path->current_node);
    if (node->shared_node != NULL) {
        struct EDAIPATH_s *path;
        for (path = ((EDAIPATH_s *)(NuLinkedListGetHead(&aieditor->paths))); path != NULL;
             path = ((EDAIPATH_s *)(NuLinkedListGetNext(&aieditor->paths, &path->link)))) {
            struct EDAIPATHNODE_s *other;
            for (other = ((EDAIPATHNODE_s *)(NuLinkedListGetHead(&path->nodes))); other != NULL;
                 other = ((EDAIPATHNODE_s *)(NuLinkedListGetNext(&path->nodes, &other->link)))) {
                if (other != node && other->shared_node == node->shared_node) {
                    other->radius = node->radius;
                    other->height_min = node->height_min;
                    other->height_max = node->height_max;
                    other->position = node->position;
                    other->platform = node->platform;
                    other->platform_position = node->platform_position;
                    creatureEditor_PathNodeMoved(other);
                    locatorEditor_PathNodeMoved(other);
                    break;
                }
            }
        }
    }
}

void __cdecl AISysProcessCharacter(AISYS_PC *system, APIOBJECT_PC *object, AIPACKET_PC *packet, i32 ground, f32 elapsed,
                                   i32 use_three_dimensions, i32 process_ai) {
    AISCRIPTPROCESS_PC *primary = &packet->script_processor;
    AISCRIPTPROCESS_PC *processor = packet->alternate_processor;
    f32 clearance;
    NUVEC delta;
    f32 distance;
    f32 stopping_clearance;
    f32 scale;
    if (processor == NULL) {
        processor = primary;
    }
    if ((packet->flags & 0x00010000u) == 0) {
        AISTATE_PC *saved_state = primary->state;
        AISTATE_PC *initial_state =
            primary->script == NULL ? NULL : AIStateFind(AICharacterInitialiseName, primary->script);
        if (initial_state != NULL) {
            AIScriptSetState(primary, initial_state);
            AIScriptProcess(system, object, packet, primary, 0.0f);
            AIScriptSetState(primary, saved_state);
        }
        packet->flags |= 0x00010000u;
    }
    if (process_ai != 0) {
        AISysUpdateCharacterPathPos(system, object, packet, ground, elapsed);
        packet->flags &= 0xfff7fbf7u;
        packet->movement_look_target = NULL;
        if ((object->flags_low & 0x180u) == 0x80u) {
            packet->path_update_flag = 0;
            packet->movement_destination = object->position;
            packet->path_target = NULL;
            packet->flags &= 0x3f0fffffu;
            packet->navigation_flags &= ~1u;
        } else {
            u32 source;
            if (AIRespawnOnPathFn != NULL && AIRespawnOnPathFn(object) != 0) {
                return;
            }
            NuSpecialClear(&object->collision_special);
            object->flags_low &= ~0x00040000u;
            if (system != NULL && processor->script != NULL) {
                AIScriptProcess(system, object, packet, processor, elapsed);
            }
            packet->flags &= 0x3f0fffffu;
            packet->navigation_flags &= ~1u;
            packet->frame_state = 0;
            if ((packet->flags & 0x10000000u) != 0) {
                source = (packet->flags >> 26) & 3;
                if (source == 1) {
                    if (packet->creature_index != 0xff) {
                        AICREATURE_PC *creature = &system->creatures[packet->creature_index];
                        AIMoveInstruction(packet, &creature->position, 0.0f, &creature->path_info, 1, 0.0f);
                    }
                } else if (source == 2) {
                    if (packet->locator != NULL) {
                        AIMoveInstruction(packet, &packet->locator->position, 0.0f, &packet->locator->path_info, 1,
                                          0.0f);
                    }
                } else {
                    packet->flags &= ~0x10000000u;
                }
            }
            AISysCharacterMovement(system, packet, object, ground);
            source = (packet->flags >> 26) & 3;
            if (source != 0 && packet->path_target == NULL && packet->movement_target_radius > 0.0f &&
                (object->motion_flags & 0x400u) != 0) {
                NUVEC *source_position = NULL;
                if (source == 1 && packet->creature_index != 0xff) {
                    source_position = &system->creatures[packet->creature_index].position;
                } else if (source == 2 && packet->locator != NULL) {
                    source_position = &packet->locator->position;
                }
                if (source_position != NULL) {
                    NUVEC source_delta;
                    f32 distance_squared = NuVecXZDistSqr(source_position, &object->position, &source_delta);
                    f32 radius = packet->movement_target_radius;
                    if ((packet->flags & 0x10000000u) != 0) {
                        if (distance_squared < radius * radius) {
                            packet->flags &= ~0x10000000u;
                        }
                    } else if (distance_squared > radius * radius) {
                        f32 release_radius = (f32)(radius + 0.5);
                        if (distance_squared > release_radius * release_radius) {
                            packet->flags |= 0x10000000u;
                        } else {
                            NUVEC movement_delta;
                            movement_delta.x = packet->movement_destination.x - object->position.x;
                            movement_delta.y = 0.0f;
                            movement_delta.z = packet->movement_destination.z - object->position.z;
                            if (NuVecDot(&movement_delta, &source_delta) < 0.0f) {
                                packet->movement_destination = object->position;
                                packet->movement_stopping_distance = 0.0f;
                            }
                        }
                    }
                }
            }
        }
    }
    clearance = packet->movement_stopping_distance != 0.0f ? packet->mover_height : 0.0f;
    if (use_three_dimensions != 0) {
        distance = NuVecDist(&packet->movement_destination, &object->position, &delta);
        if (distance > packet->mover_height + clearance + packet->movement_stopping_distance) {
            scale = distance != 0.0f && packet->mover_height != 0.0f ? packet->mover_height / distance : 0.0f;
            NuVecScale(&delta, &delta, scale);
            NuVecAdd(&packet->movement_position, &object->position, &delta);
        } else {
            stopping_clearance = clearance + packet->movement_stopping_distance;
            scale = distance != 0.0f && stopping_clearance != 0.0f ? stopping_clearance / distance : 0.0f;
            NuVecScale(&delta, &delta, scale);
            NuVecSub(&packet->movement_position, &packet->movement_destination, &delta);
        }
    } else {
        distance = NuVecXZDist(&packet->movement_destination, &object->position, &delta);
        if (distance > packet->mover_height + clearance + packet->movement_stopping_distance) {
            scale = distance != 0.0f && packet->mover_height != 0.0f ? packet->mover_height / distance : 0.0f;
            NuVecScale(&delta, &delta, scale);
            NuVecAdd(&packet->movement_position, &object->position, &delta);
        } else {
            stopping_clearance = clearance + packet->movement_stopping_distance;
            scale = distance != 0.0f && stopping_clearance != 0.0f ? stopping_clearance / distance : 0.0f;
            NuVecScale(&delta, &delta, scale);
            NuVecSub(&packet->movement_position, &packet->movement_destination, &delta);
        }
    }
}

i32 __cdecl Action_SetState(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 count, i32 first_time, f32 elapsed) {
    i32 i;
    if (first_time == 0 || count == 0)
        return 0;
    processor->next_state = AIStateFind(params[0], processor->script);
    processor->flags &= ~4u;
    for (i = 1; i < count; i++) {
        if (NuStrICmp(params[i], "KeepBlockedMessages") == 0)
            processor->flags |= 4;
    }
    if (processor->next_state == NULL) {
        if (AIActionDiagnosticMode == 1)
            AIScriptMissingStateDiagnosticAlternate(params[0], processor->script->name, processor->state->name);
        else
            AIScriptMissingStateDiagnostic(params[0], processor->script->name, processor->state->name);
    }
    return 0;
}

i32 __cdecl Action_ResetTimer(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 count, i32 first_time, f32 elapsed) {
    f32 minimum;
    f32 maximum;
    f32 exact;
    f64 first_term;
    f32 random;
    char *value;
    i32 i;
    if (first_time == 0)
        return 1;
    minimum = maximum = exact = 0.0f;
    for (i = 0; i < count; i++) {
        value = NuStrIStr(params[i], (char *)AIActionMinTimeName);
        if (value != NULL)
            minimum = AIParamToFloatEx(packet, processor, value + 8);
        else if ((value = NuStrIStr(params[i], (char *)AIActionMaxTimeName)) != NULL)
            maximum = AIParamToFloatEx(packet, processor, value + 8);
        else if ((value = NuStrIStr(params[i], (char *)AIActionTimeName)) != NULL)
            exact = AIParamToFloatEx(packet, processor, value + 5);
    }
    if (minimum != 0.0f || maximum != 0.0f) {
        first_term = (1.0 - NuRandFloat()) * minimum;
        random = NuRandFloat();
        processor->script_timer = (f32)(random * maximum + first_term);
    } else
        processor->script_timer = exact;
    return 1;
}

i32 __cdecl Action_Idle(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params, i32 count,
                        i32 first_time, f32 elapsed) {
    f32 minimum;
    f32 maximum;
    i32 frames;
    i32 i;
    char *value;
    minimum = maximum = 0.0f;
    frames = 0;
    if (first_time != 0) {
        for (i = 0; i < count; i++) {
            value = NuStrIStr(params[i], (char *)AIActionMinTimeName);
            if (value != NULL)
                minimum = AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionMinTimeName) + 1);
            else if ((value = NuStrIStr(params[i], (char *)AIActionMaxTimeName)) != NULL)
                maximum = AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionMaxTimeName) + 1);
            else if ((value = NuStrIStr(params[i], (char *)AIActionFramesName)) != NULL)
                frames = (i32)AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionFramesName) + 1);
            else
                processor->action_timer = AIParamToFloatEx(packet, processor, params[i]);
        }
        if (frames != 0) {
            processor->action_data_1 = (u8)(frames < 0 ? 0 : (frames > 255 ? 255 : frames));
        } else if (processor->action_timer == 0.0f && maximum > minimum) {
            processor->action_timer = NuRandFloat() * (maximum - minimum) + minimum;
        }
        return 0;
    }
    if (processor->action_data_1 != 0) {
        processor->action_data_1--;
        return processor->action_data_1 == 0;
    }
    if (processor->action_timer > 0.0f) {
        processor->action_timer -= elapsed;
        if (processor->action_timer <= 0.0f) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl Action_SetParam(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 count, i32 first_time, f32 elapsed) {
    AICREATURE_PC *creature;
    i32 i;
    i32 index;
    u64 flag;
    char *value;
    f32 result;
    f64 previous;
    if (packet != NULL && processor != NULL && processor->script != NULL) {
        creature = packet->creature_index == 0xff ? NULL : &system->creatures[packet->creature_index];
        for (i = 0; i < count - 1; i++) {
            for (index = 0; index < 4; index++) {
                if (NuStrICmp(params[i], processor->script->params[index].name) == 0)
                    break;
            }
            if (index < 4) {
                flag = (u64)2 << index;
                i++;
                if (NuStrICmp(params[i], GizmoDefaultOutputName) == 0) {
                    if (creature != NULL && ((u64)(i64)creature->flags & flag) != 0)
                        processor->params[index] = creature->script_params[index];
                    else
                        processor->params[index] = processor->script->params[index].default_value;
                } else if ((value = NuStrIStr(params[i], (char *)AIActionIncrementName)) != NULL) {
                    result =
                        AIParamToFloatEx(packet, &packet->script_processor, value + NuStrLen(AIActionIncrementName));
                    processor->params[index] += result;
                } else if ((value = NuStrIStr(params[i], (char *)AIActionDecrementName)) != NULL) {
                    previous = processor->params[index];
                    result =
                        AIParamToFloatEx(packet, &packet->script_processor, value + NuStrLen(AIActionDecrementName));
                    processor->params[index] = (f32)(previous - result);
                } else
                    processor->params[index] = AIParamToFloatEx(packet, &packet->script_processor, params[i]);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetReturnToState(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    AISTATE_PC *state;
    i32 i;
    char *value;
    if (first_time != 0 && processor != NULL) {
        state = processor->state;
        for (i = 0; i < count; i++) {
            value = NuStrIStr(params[i], (char *)AIActionStateName);
            if (value != NULL) {
                state = AIStateFind(value + 6, processor->script);
                if (state == NULL) {
                    if (AIActionDiagnosticMode == 0)
                        AIScriptMissingStateDiagnostic(params[0], processor->script->name, processor->state->name);
                    else
                        AIScriptMissingStateDiagnosticAlternate(params[0], processor->script->name,
                                                                processor->state->name);
                }
            }
        }
        processor->return_to_state = state;
    }
    return 1;
}

i32 __cdecl Action_ReturnToState(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 count, i32 first_time, f32 elapsed) {
    if (processor != NULL && processor->return_to_state != NULL) {
        processor->next_state = processor->return_to_state;
        processor->return_to_state = NULL;
    }
    return 1;
}

void __cdecl InitFn_GameParamToFloat(AI_GAME_PARAM_TO_FLOAT_PC callback) {
    GameParamToFloatFn = callback;
}

i32 __cdecl Action_SetCircleDirection(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;

    if (packet != NULL && packet->owner != NULL && first_time != 0 && param_count != 0) {
        for (i = 0; i < param_count; i++) {
            if (NuStrICmp(params[i], AIActionClockwiseName) == 0) {
                packet->flags |= 0x200u;
            } else if (NuStrICmp(params[i], AIActionAnticlockwiseName) == 0) {
                packet->flags &= ~0x200u;
            } else if (NuStrICmp(params[i], AIActionReverseName) == 0) {
                packet->flags ^= 0x200u;
            } else if (NuStrICmp(params[i], AIActionRandomName) == 0) {
                if (NuRandFloat() > 0.5f) {
                    packet->flags |= 0x200u;
                } else {
                    packet->flags &= ~0x200u;
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_FacePlayer(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_count, i32 first_time, f32 elapsed) {
    f32 min_time;
    f32 max_time;
    i32 index;
    char *value;
    f32 remaining_time;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        min_time = 0.0f;
        max_time = 0.0f;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionMinTimeName);
            if (value != NULL) {
                min_time = AIParamToFloatEx(packet, processor, value + 8);
            } else if ((value = NuStrIStr(params[index], (char *)AIActionMaxTimeName)) != NULL) {
                max_time = AIParamToFloatEx(packet, processor, value + 8);
            } else {
                processor->action_timer = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
        if (processor->action_timer == 0.0f && min_time < max_time) {
            processor->action_timer = NuRandFloat() * (max_time - min_time) + min_time;
        }
    }
    remaining_time = processor->action_timer;
    if (sys->player_1 != NULL) {
        packet->movement_look_target = &sys->player_1->position;
    }
    if (remaining_time > 0.0f) {
        remaining_time -= elapsed;
        processor->action_timer = remaining_time;
        if (remaining_time <= 0.0f) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl Action_FaceOpponent(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_count, i32 first_time, f32 elapsed) {
    f32 min_time;
    f32 max_time;
    i32 index;
    char *value;
    APIOBJECT_PC *opponent;
    NUVEC direction;

    (void)sys;
    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        min_time = 0.0f;
        max_time = 0.0f;
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIActionMinTimeName);
            if (value != NULL) {
                min_time = AIParamToFloatEx(packet, processor, value + 8);
            } else if ((value = NuStrIStr(params[index], (char *)AIActionMaxTimeName)) != NULL) {
                max_time = AIParamToFloatEx(packet, processor, value + 8);
            } else if ((value = NuStrIStr(params[index], (char *)AIActionFaceOffsetName)) != NULL) {
                processor->action_data_4 = AIParamToFloatEx(packet, processor, value + 11);
            } else if (NuStrICmp(params[index], AIActionNearestOpponentName) == 0) {
                processor->action_data_1 = 1;
            } else {
                processor->action_timer = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
        if (processor->action_timer == 0.0f && min_time < max_time) {
            processor->action_timer = NuRandFloat() * (max_time - min_time) + min_time;
        }
    }

    opponent =
        processor->action_data_1 != 0 ? ((APIOBJECT_PC *)packet->nearest_opponent) : ((APIOBJECT_PC *)packet->opponent);
    if (opponent != NULL && opponent->ai != NULL) {
        if (processor->action_data_4 == 0.0f) {
            packet->movement_look_target = &opponent->position;
        } else {
            direction.x = opponent->position.z - packet->owner->position.z;
            direction.y = 0.0f;
            direction.z = packet->owner->position.x - opponent->position.x;
            NuVecNorm(&direction, &direction);
            processor->action_pos.x = opponent->position.x + direction.x * processor->action_data_4;
            processor->action_pos.y = opponent->position.y;
            processor->action_pos.z = opponent->position.z + direction.z * processor->action_data_4;
            packet->movement_look_target = &processor->action_pos;
        }
    }
    if (processor->action_timer > 0.0f) {
        processor->action_timer -= elapsed;
        if (processor->action_timer <= 0.0f) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl Action_ShadowTerrain(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;

    if (packet != NULL && packet->owner != NULL && first_time != 0) {
        packet->owner->flags_1fc |= 8;
        for (i = 0; i < param_count; i++) {
            if (NuStrICmp(params[i], AIScriptFalse) == 0) {
                packet->owner->flags_1fc &= ~8;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetMoveRadius(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {

    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (packet != NULL && packet->owner != NULL && param_5 != 0) {
        packet->mover_height =
            ((APIOBJECT_PC *)packet->owner)->collision_radius + ((APIOBJECT_PC *)packet->owner)->collision_radius;
        if (param_4 != 0 && NuStrICmp(params[0], GizmoDefaultOutputName) != 0) {
            packet->mover_height = AIParamToFloatEx(packet, processor, params[0]);
        }
    }
    return 1;
}

static i32 Action_OverrideAnimation(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                    char **params, i32 param_count, i32 first_time, f32 unused6) {
    i16 from;
    i16 to;
    i32 index;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL || first_time == 0) {
        return 1;
    }

    from = -1;
    to = -1;
    for (index = 0; index < param_count; ++index) {
        char *value;
        if (NuStrICmp(params[index], "from=All") == 0) {
            from = ((i16)(apicharsys->model_id_capacity));
            continue;
        }

        value = NuStrIStr(params[index], "from=");
        if (value != NULL) {
            from = ((i16)(FindAnimIX(packet->owner->character_data, value + 5)));
            continue;
        }

        value = NuStrIStr(params[index], "to=");
        if (value != NULL) {
            to = ((i16)(FindAnimIX(packet->owner->character_data, value + 3)));
            continue;
        }

        if (processor != NULL) {
            processor->action_timer = AIParamToFloatEx(packet, processor, params[0]);
        }
    }

    if (to == -1) {
        from = -1;
    }
    packet->animation_override_from = from;
    packet->animation_override_to = to;
    return 1;
}

i32 __cdecl Action_ResetToOrigin(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    NUVEC *origin;

    (void)processor;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL) {
        return 1;
    }
    origin = GetOriginPositionFn != NULL ? GetOriginPositionFn(sys, packet) : NULL;
    if (origin != NULL) {
        packet->owner->position = *origin;
    } else if (packet->owner != NULL && (packet->owner->flags_1f8 & 0x400) != 0 && packet->creature_index != 0xff) {
        packet->owner->position = sys->creatures[packet->creature_index].position;
    }
    return 1;
}

i32 __cdecl Action_SetIgnoreAntinodes(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    APIOBJECT_PC *object;
    i32 ignore;
    i32 i;
    char *value;

    if (first_time != 0) {
        object = packet != NULL ? (APIOBJECT_PC *)(packet->owner) : NULL;
        ignore = 1;
        for (i = 0; i < param_count; i++) {
            value = NuStrIStr(params[i], (char *)AreaConfigKeyword_character);
            if (value != NULL) {
                if (GetNamedGameObjectCallback != NULL) {
                    object = GetNamedGameObjectCallback(sys, value + 10);
                }
            } else if (NuStrICmp(AIActionFalseName, params[0]) == 0) {
                // The original tests the first parameter, even on later iterations.
                ignore = 0;
            }
        }
        if (object != NULL) {
            object->flags_low = (object->flags_low & ~0x20000u) | (ignore != 0 ? 0x20000u : 0);
        }
    }
    return 1;
}

i32 __cdecl Action_SetViewDistance(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 count, i32 first_time, f32 elapsed) {
    APIOBJECT_PC *object;

    if (packet != NULL && packet->owner != NULL && first_time != 0) {
        object = (APIOBJECT_PC *)packet->owner;
        if (packet->creature_index != 0xff)
            object->view_distance = system->creatures[packet->creature_index].view_distance;
        else if (GetViewRangeFn != NULL)
            object->view_distance = GetViewRangeFn(object->character_model->model_id);
        else
            object->view_distance = 1.0f;
        if (count != 0 && NuStrICmp(params[0], GizmoDefaultOutputName) != 0)
            object->view_distance = AIParamToFloatEx(packet, processor, params[0]);
    }
    return 1;
}

i32 __cdecl Action_SetHearDistance(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 count, i32 first_time, f32 elapsed) {
    APIOBJECT_PC *object;

    if (packet != NULL && packet->owner != NULL && first_time != 0) {
        object = (APIOBJECT_PC *)packet->owner;
        if (packet->creature_index != 0xff)
            object->hear_distance = system->creatures[packet->creature_index].hear_distance;
        else if (GetHearDistanceFn != NULL)
            object->hear_distance = GetHearDistanceFn(object->character_model->model_id);
        else
            object->hear_distance = 1.0f;
        if (count != 0 && NuStrICmp(params[0], GizmoDefaultOutputName) != 0)
            object->hear_distance = AIParamToFloatEx(packet, processor, params[0]);
    }
    return 1;
}

i32 __cdecl Action_SetMaxViewHeight(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    APIOBJECT_PC *object;

    if (packet != NULL && packet->owner != NULL && first_time != 0) {
        object = (APIOBJECT_PC *)packet->owner;
        if (packet->creature_index != 0xff)
            object->max_view_height = system->creatures[packet->creature_index].max_view_height;
        else if (GetMaxViewHeightFn != NULL)
            object->max_view_height = GetMaxViewHeightFn(object->character_model->model_id);
        else
            object->max_view_height = 1.0f;
        if (count != 0 && NuStrICmp(params[0], GizmoDefaultOutputName) != 0)
            object->max_view_height = AIParamToFloatEx(packet, processor, params[0]);
    }
    return 1;
}

i32 __cdecl Action_SetMinViewHeight(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    APIOBJECT_PC *object;

    if (packet != NULL && packet->owner != NULL && first_time != 0) {
        object = (APIOBJECT_PC *)packet->owner;
        if (packet->creature_index != 0xff)
            object->min_view_height = system->creatures[packet->creature_index].min_view_height;
        else if (GetMinViewHeightFn != NULL)
            object->min_view_height = GetMinViewHeightFn(object->character_model->model_id);
        else
            object->min_view_height = 1.0f;
        if (count != 0 && NuStrICmp(params[0], GizmoDefaultOutputName) != 0)
            object->min_view_height = AIParamToFloatEx(packet, processor, params[0]);
    }
    return 1;
}

i32 __cdecl Action_IgnoreWallSplines(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                     char **params, i32 count, i32 first_time, f32 elapsed) {
    i32 i;

    if (packet != NULL && first_time != 0) {
        packet->flags |= 0x80u;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], AIScriptFalse) == 0)
                packet->flags &= ~0x80u;
        }
    }
    return 1;
}

i32 __cdecl Action_CheckWallSplines(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    i32 i;

    if (packet != NULL && first_time != 0) {
        packet->flags |= 0x100u;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], AIScriptFalse) == 0)
                packet->flags &= ~0x100u;
        }
        if ((packet->flags & 0x100u) != 0)
            packet->flags |= 0x80u;
    }
    return 1;
}

i32 __cdecl Action_Circle(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                          i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    i32 i;
    char *value;
    NUVEC *position;
    AIPATHINFO_PC *path;
    AILOCATOR_PC *locator;
    AICREATURE_PC *creature;

    if (packet == NULL || sys == NULL || sys->player_1 == NULL) {
        return 1;
    }
    if (first_time != 0) {
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) != 0) {
                continue;
            }

            if (NuStrICmp(params[i], AIPathAnticlockwiseName) == 0) {
                packet->flags &= ~0x200u;
            } else if (NuStrICmp(params[i], AIPathClockwiseName) == 0) {
                packet->flags |= 0x200u;
            } else if (NuStrICmp(params[i], AIPathReverseName) == 0) {
                packet->flags ^= 0x200u;
            } else if (NuStrICmp(params[i], AIPathFacingName) == 0) {
                processor->action_data_1 |= 1;
            } else if (NuStrICmp(params[i], AIPathCurrentDistanceName) == 0) {
                processor->action_data_1 |= 4;
            } else if ((value = NuStrIStr(params[i], (char *)AIPathLocatorName)) != NULL) {
                processor->action_data_3 = AIPathFindLocator(sys, value + 8);
                if (processor->action_data_3 != NULL) {
                    processor->action_data_1 |= 0x10;
                }
            } else if (NuStrIStr(params[i], (char *)AIPathCurrentPositionName) != NULL) {
                processor->action_data_1 |= 8;
                processor->action_pos = packet->terrain_origin;
            } else if (NuStrICmp(params[i], AIPathOriginName) == 0) {
                if (packet->creature_index != 0xff) {
                    processor->action_data_1 |= 0x20;
                }
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
        if ((processor->action_data_1 & 0x38) == 0) {
            processor->action_data_1 |= 8;
            processor->action_pos = packet->terrain_origin;
        }
    }
    position = NULL;
    path = NULL;
    if ((processor->action_data_1 & 8) != 0) {
        position = &processor->action_pos;
        path = &packet->path_info;
    } else if ((processor->action_data_1 & 0x10) != 0) {
        locator = (AILOCATOR_PC *)(processor->action_data_3);
        position = &locator->position;
        path = &locator->path_info;
    } else if ((processor->action_data_1 & 0x20) != 0) {
        creature = &sys->creatures[packet->creature_index];
        path = &creature->path_info;
        position = GetOriginPositionFn != NULL ? GetOriginPositionFn(sys, packet) : NULL;
        if (position == NULL) {
            position = &creature->position;
        }
    }
    if (position != NULL && path != NULL) {
        if ((processor->action_data_1 & 4) != 0) {
            packet->movement_instruction_parameter = NuVecXZDist(&packet->terrain_origin, position, &difference);
        }
        AIMoveInstruction(packet, position, 0.0f, path, 3, packet->movement_instruction_parameter);
        if ((processor->action_data_1 & 1) != 0) {
            packet->movement_look_target = position;
        }
    }
    return 0;
}

i32 __cdecl Action_CirclePlayer(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    i32 i;
    AIPACKET_PC *target;
    NUVEC *position;

    if (packet == NULL || sys == NULL || sys->player_1 == NULL) {
        return 1;
    }
    if (first_time != 0) {
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[i], AIPathAnticlockwiseName) == 0) {
                packet->flags &= ~0x200u;
            } else if (NuStrICmp(params[i], AIPathClockwiseName) == 0) {
                packet->flags |= 0x200u;
            } else if (NuStrICmp(params[i], AIPathReverseName) == 0) {
                packet->flags ^= 0x200u;
            } else if (NuStrICmp(params[i], AIPathFacingName) == 0) {
                processor->action_data_1 |= 1;
            } else if (NuStrICmp(params[i], AIPathCanGoOffPathName) == 0) {
                processor->action_data_1 |= 2;
            } else if (NuStrICmp(params[i], AIPathCurrentDistanceName) == 0) {
                processor->action_data_1 |= 4;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (sys->player_1 != NULL) {
        if ((processor->action_data_1 & 4) != 0) {
            packet->movement_instruction_parameter =
                NuVecXZDist(&packet->terrain_origin, &sys->player_1->ai->last_path_position, &difference);
        }
        target = sys->player_1->ai;
        position = (processor->action_data_1 & 2) != 0 ? &target->terrain_origin : &target->last_path_position;
        AIMoveInstruction(packet, position, target->mover_height, &target->path_info, 3,
                          packet->movement_instruction_parameter);
        if ((processor->action_data_1 & 1) != 0) {
            packet->movement_look_target = position;
        }
    }
    return 0;
}

i32 __cdecl Action_CircleOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    i32 i;
    AIPACKET_PC *target;
    NUVEC *position;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[i], AIPathAnticlockwiseName) == 0) {
                packet->flags &= ~0x200u;
            } else if (NuStrICmp(params[i], AIPathClockwiseName) == 0) {
                packet->flags |= 0x200u;
            } else if (NuStrICmp(params[i], AIPathReverseName) == 0) {
                packet->flags ^= 0x200u;
            } else if (NuStrICmp(params[i], AIPathRandomName) == 0) {
                if (NuRandFloat() < 0.5f) {
                    packet->flags &= ~0x200u;
                } else {
                    packet->flags |= 0x200u;
                }
            } else if (NuStrICmp(params[i], AIPathFacingName) == 0) {
                processor->action_data_1 |= 1;
            } else if (NuStrICmp(params[i], AIPathCanGoOffPathName) == 0) {
                processor->action_data_1 |= 2;
            } else if (NuStrICmp(params[i], AIPathCurrentDistanceName) == 0) {
                processor->action_data_1 |= 4;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (((APIOBJECT_PC *)packet->opponent) != NULL && ((APIOBJECT_PC *)packet->opponent)->ai != NULL) {
        if ((processor->action_data_1 & 4) != 0) {
            packet->movement_instruction_parameter = NuVecXZDist(
                &packet->terrain_origin, &((APIOBJECT_PC *)packet->opponent)->ai->last_path_position, &difference);
        }
        target = ((APIOBJECT_PC *)packet->opponent)->ai;
        position = (processor->action_data_1 & 2) != 0 ? &target->terrain_origin : &target->last_path_position;
        AIMoveInstruction(packet, position, target->mover_height, &target->path_info, 3,
                          packet->movement_instruction_parameter);
        if ((processor->action_data_1 & 1) != 0) {
            packet->movement_look_target = position;
        }
    }
    return 0;
}

i32 __cdecl Action_GoToNode(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    i32 index;
    AIPATHNODE_PC *node;
    i32 node_index;
    AIPATHNODE_PC *node_2;
    f32 distance_squared;

    (void)elapsed;

    if (packet == NULL || packet->owner == NULL || packet->path_info.path == NULL ||
        packet->path_info.connection == NULL) {
        return 1;
    }

    if (first_time != 0) {
        if (param_count == 0) {
            return 0;
        }
        for (index = 1; index < param_count; ++index) {
            if (AIActionParseSpeedFn == NULL || AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) == 0) {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
        processor->action_data_3 = AIPathFindNode(sys, packet->path_info.path, params[0]);
        node = (AIPATHNODE_PC *)(processor->action_data_3);
        if (node != NULL && node->connection_count != 0) {
            node_index = node - packet->path_info.path->nodes;
            processor->path_info.path = packet->path_info.path;
            processor->path_info.connection = node->connections[0];
            processor->path_info.distance =
                node_index == processor->path_info.connection->node_indices[0] ? 0.0f : 1.0f;
            processor->path_info.direction = 0;
            processor->path_info.flags_word |= 1;
            processor->path_info.width = 0.0f;
            AIMoveInstruction(packet, &node->position, 0.0f, &processor->path_info, 1,
                              packet->movement_instruction_parameter);
        } else {
            return 1;
        }
        return 0;
    }

    node_2 = (AIPATHNODE_PC *)(processor->action_data_3);
    if (node_2 == NULL) {
        return 1;
    }
    distance_squared = NuVecXZDistSqr(&packet->terrain_origin, &node_2->position, &difference);
    AIMoveInstruction(packet, &node_2->position, 0.0f, &processor->path_info, 1,
                      packet->movement_instruction_parameter);
    return distance_squared < node_2->radius_squared;
}

i32 __cdecl Action_FollowPath(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_count, i32 is_first_time, f32 elapsed) {
    f32 completion_time;
    f32 minimum_time;
    f32 maximum_time;
    i32 index;
    char *value;
    f64 first_term;
    f32 random;

    (void)sys;

    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL) {
        return 1;
    }
    if (packet->path_info.path == NULL || packet->path_info.connection == NULL) {
        return 0;
    }

    completion_time = 0.0f;
    if (is_first_time != 0) {
        packet->movement_target = NULL;

        minimum_time = 0.0f;
        maximum_time = 0.0f;
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }

            value = NuStrIStr(params[index], (char *)AIActionMinTimeName);
            if (value != NULL) {
                minimum_time = AIParamToFloatEx(packet, processor, value + 8);
                continue;
            }

            value = NuStrIStr(params[index], (char *)AIActionMaxTimeName);
            if (value != NULL) {
                maximum_time = AIParamToFloatEx(packet, processor, value + 8);
                continue;
            }

            processor->action_timer = AIParamToFloatEx(packet, processor, params[index]);
        }

        if (maximum_time > minimum_time) {
            first_term = (1.0 - NuRandFloat()) * minimum_time;
            random = NuRandFloat();
            processor->action_timer = (f32)(random * maximum_time + first_term);
        }
    }

    AIMoveInstruction(packet, NULL, 0.0f, NULL, 4, packet->movement_instruction_parameter);

    if (!(processor->action_timer > completion_time)) {
        return 0;
    }

    processor->action_timer -= elapsed;
    return completion_time >= processor->action_timer;
}

i32 __cdecl Action_MoveAwayFromNode(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    i32 i;
    AIPATHNODE_PC *node;
    i32 index;
    AIPATHNODE_PC *node_2;
    f32 distance;

    if (packet == NULL || packet->owner == NULL || packet->path_info.path == NULL ||
        packet->path_info.connection == NULL) {
        return 1;
    }
    if (first_time != 0) {
        if (param_count == 0) {
            return 0;
        }
        for (i = 1; i < param_count; i++) {
            if (AIActionParseSpeedFn == NULL || AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) == 0) {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
        node = AIPathFindNode(sys, packet->path_info.path, params[0]);
        processor->action_data_3 = node;
        if (node == NULL || node->connection_count == 0) {
            return 1;
        }
        index = node - packet->path_info.path->nodes;
        AISysCharacterSetPath(packet, packet->path_info.path);
        processor->path_info.connection = node->connections[0];
        processor->path_info.distance = index == processor->path_info.connection->node_indices[0] ? 0.0f : 1.0f;
        processor->path_info.width = 0.0f;
        processor->path_info.direction = 0;
        processor->path_info.flags_word |= 1;
        AIMoveInstruction(packet, &node->position, 0.0f, &processor->path_info, 2,
                          packet->movement_instruction_parameter);
        return 0;
    }
    node_2 = (AIPATHNODE_PC *)(processor->action_data_3);
    if (node_2 != NULL) {
        distance = NuVecXZDistSqr(&packet->terrain_origin, &node_2->position, &difference);
        if (!(node_2->radius_squared > distance)) {
            AIMoveInstruction(packet, &node_2->position, 0.0f, &processor->path_info, 2,
                              packet->movement_instruction_parameter);
            return 0;
        }
    }
    return 1;
}

i32 __cdecl Action_GoToNodeRandom(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    i32 selected;
    AIPATHNODE_PC *node;
    i32 node_index;
    AIPATHNODE_PC *node_2;
    f32 distance_squared;

    (void)elapsed;

    if (packet == NULL || packet->owner == NULL || packet->path_info.path == NULL ||
        packet->path_info.connection == NULL) {
        return 1;
    }
    if (first_time != 0) {
        if (param_count == 0) {
            return 0;
        }
        selected = (i32)(NuRandFloat() * param_count);
        if (selected >= param_count) {
            selected = param_count - 1;
        }
        processor->action_data_3 = AIPathFindNode(sys, packet->path_info.path, params[selected]);
        node = (AIPATHNODE_PC *)(processor->action_data_3);
        if (node != NULL && node->connection_count != 0) {
            node_index = node - packet->path_info.path->nodes;
            processor->path_info.path = packet->path_info.path;
            processor->path_info.connection = node->connections[0];
            processor->path_info.distance =
                node_index == processor->path_info.connection->node_indices[0] ? 0.0f : 1.0f;
            processor->path_info.direction = 0;
            processor->path_info.flags_word |= 1;
            processor->path_info.width = 0.0f;
            AIMoveInstruction(packet, &node->position, 0.0f, &processor->path_info, 1,
                              packet->movement_instruction_parameter);
        } else {
            return 1;
        }
        return 0;
    }
    node_2 = (AIPATHNODE_PC *)(processor->action_data_3);
    if (node_2 == NULL) {
        return 1;
    }
    distance_squared = NuVecXZDistSqr(&packet->terrain_origin, &node_2->position, &difference);
    AIMoveInstruction(packet, &node_2->position, 0.0f, &processor->path_info, 1,
                      packet->movement_instruction_parameter);
    return distance_squared < node_2->radius_squared;
}

i32 __cdecl Action_MoveAwayFromOpponent(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    AIPACKET_PC *target;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[i], AIPathFaceName) == 0) {
                processor->action_data_1 = 1;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (((APIOBJECT_PC *)packet->opponent) != NULL && ((APIOBJECT_PC *)packet->opponent)->ai != NULL) {
        target = ((APIOBJECT_PC *)packet->opponent)->ai;
        AIMoveInstruction(packet, &target->last_path_position, target->mover_height, &target->path_info, 2,
                          packet->movement_instruction_parameter);
        if (processor->action_data_1 != 0) {
            packet->movement_look_target = &((APIOBJECT_PC *)packet->opponent)->position;
        }
    }
    return 0;
}

i32 __cdecl Action_MoveAwayFromPlayer(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    AIPACKET_PC *target;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[i], AIPathFaceName) == 0) {
                processor->action_data_1 = 1;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (sys->player_1 != NULL) {
        target = sys->player_1->ai;
        AIMoveInstruction(packet, &target->last_path_position, target->mover_height, &target->path_info, 2,
                          packet->movement_instruction_parameter);
        if (processor->action_data_1 != 0) {
            packet->movement_look_target = &sys->player_1->position;
        }
    }
    return 0;
}

i32 __cdecl Action_MoveAwayFromPlayer2(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                       i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    AIPACKET_PC *target;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[i], AIPathFaceName) == 0) {
                processor->action_data_1 = 1;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (sys->player_2 != NULL) {
        target = sys->player_2->ai;
        AIMoveInstruction(packet, &target->last_path_position, target->mover_height, &target->path_info, 2,
                          packet->movement_instruction_parameter);
        if (processor->action_data_1 != 0) {
            packet->movement_look_target = &sys->player_2->position;
        }
    }
    return 0;
}

i32 __cdecl Action_GoToOrigin(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    AICREATURE_PC *creature;
    NUVEC *origin;
    f32 min_time;
    f32 max_time;
    i32 index;
    char *value;
    f32 distance_squared;
    f32 range;
    f32 remaining_time;

    if (packet == NULL || packet->owner == NULL || packet->path_info.path == NULL ||
        packet->path_info.connection == NULL || (packet->owner->flags_1f8 & 0x400) == 0 ||
        packet->creature_index == 0xff) {
        return 1;
    }
    creature = &sys->creatures[packet->creature_index];
    origin = GetOriginPositionFn != NULL ? GetOriginPositionFn(sys, packet) : NULL;
    if (origin == NULL) {
        origin = &creature->position;
    }

    if (first_time != 0) {
        packet->movement_instruction_parameter = 0.2f;
        min_time = 0.0f;
        max_time = 0.0f;
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIPathWaitTimeName);
            if (value != NULL) {
                processor->action_timer = AIParamToFloatEx(packet, processor, value + NuStrLen(AIPathWaitTimeName) + 1);
            } else if ((value = NuStrIStr(params[index], (char *)AIActionMinTimeName)) != NULL) {
                min_time = AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionMinTimeName) + 1);
            } else if ((value = NuStrIStr(params[index], (char *)AIActionMaxTimeName)) != NULL) {
                max_time = AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionMaxTimeName) + 1);
            } else if (NuStrICmp(params[index], AIPathXZRangeCheckName) == 0) {
                processor->action_data_2 = 1;
            } else if ((value = NuStrIStr(params[index], (char *)AIPathGoalRangeName)) != NULL) {
                packet->movement_instruction_parameter =
                    AIParamToFloatEx(packet, processor, value + NuStrLen(AIPathGoalRangeName) + 1);
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
        if (processor->action_timer == 0.0f) {
            if (min_time < max_time) {
                processor->action_timer = NuRandFloat() * (max_time - min_time) + min_time;
            } else {
                processor->action_timer = 0.01f;
            }
        }
        AIMoveInstruction(packet, origin, 0.0f, &creature->path_info, 1, packet->movement_instruction_parameter);
        processor->action_pos.x = 0.0f;
        processor->action_pos.y = 0.0f;
        processor->action_pos.z = 1.0f;
        NuVecRotateY(&processor->action_pos, &processor->action_pos, creature->y_rotation);
        NuVecAdd(&processor->action_pos, &processor->action_pos, origin);
        return 0;
    }

    AIMoveInstruction(packet, origin, 0.0f, &creature->path_info, 1, packet->movement_instruction_parameter);
    distance_squared = processor->action_data_2 != 0 ? NuVecXZDistSqr(&packet->terrain_origin, origin, &difference)
                                                     : NuVecDistSqr(&packet->terrain_origin, origin, &difference);
    range = packet->movement_instruction_parameter + ai_moveradius;
    range = elapsed * packet->owner->horizontal_velocity_magnitude + range;
    if (distance_squared < range * range) {
        remaining_time = processor->action_timer;
        packet->movement_look_target = &processor->action_pos;
        if (!(remaining_time > 0.0f)) {
            return 1;
        }
        remaining_time -= elapsed;
        processor->action_timer = remaining_time;
        if (remaining_time < 0.0f) {
            processor->action_timer = 0.0f;
        }
    }
    return 0;
}

__declspec(noinline) void __cdecl FollowAPIObject(APIOBJECT_PC *object, APIOBJECT_PC *target, i32 flags,
                                                  f32 movement_parameter) {
    AIPACKET_PC *packet;
    AIPACKET_PC *target_packet;
    AIPATH_PC *path;
    NUVEC *destination;
    i32 movement_mode;
    i32 use_path;
    u8 object_node;
    u8 target_node;
    f32 stopping_distance;
    packet = object->ai;
    target_packet = target->ai;
    if ((flags & 1) != 0) {
        use_path = 0;
        path = packet->path_info.path;
        if (path != NULL && path == target_packet->path_info.path) {
            if (((target_packet->path_info.flags & 1) != 0 ||
                 packet->path_info.connection != target_packet->path_info.connection) &&
                packet->path_info.connection != NULL && target_packet->path_info.connection != NULL &&
                (packet->flags & 0x100000u) == 0) {
                object_node = packet->path_info.connection->node_indices[0];
                target_node = target_packet->path_info.connection->node_indices[0];
                if (path->route_matrix[object_node][target_node] != 0xff || object_node == target_node)
                    use_path = 1;
            }
        }
        if (use_path != 0) {
            destination = &target_packet->last_path_position;
            movement_mode = 1;
        } else {
            destination = &target_packet->terrain_origin;
            movement_mode = 7;
        }
    } else {
        destination = &target_packet->last_path_position;
        movement_mode = 1;
    }
    stopping_distance = (flags & 2) != 0 ? 0.0f : target_packet->mover_height;
    AIMoveInstruction(packet, destination, stopping_distance, &target_packet->path_info, movement_mode,
                      movement_parameter);
}

__declspec(noinline) void __cdecl AIPathCnxSetTemporaryBlock(AIPATH_PC *path, char *from_name, char *to_name,
                                                             i32 blocked) {
    AIPATHNODE_PC *from;
    AIPATHNODE_PC *to;
    AIPATHCNX_PC *connection;
    i32 to_index;
    i32 index;
    i32 direction;
    if (from_name == NULL || path == NULL || to_name == NULL)
        return;
    from = AIPathFindNode(NULL, path, from_name);
    to = AIPathFindNode(NULL, path, to_name);
    if (to == NULL || from == NULL)
        return;
    to_index = to - path->nodes;
    for (index = 0; index < from->connection_count; index++) {
        connection = from->connections[index];
        if (connection->node_indices[0] == to_index)
            direction = 1;
        else if (connection->node_indices[1] == to_index)
            direction = 0;
        else
            continue;
        if (blocked != 0)
            connection->traversal_flags[direction] |= 0x80000000u;
        else
            connection->traversal_flags[direction] &= 0x7fffffffu;
        return;
    }
}

void __cdecl InitFn_AIBigJumpToDestination(AI_BIG_JUMP_TO_DESTINATION_PC callback) {
    AIBigJumpToDestinationFn = callback;
}

i32 __cdecl Action_FaceLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    char *value;
    AILOCATOR_PC *locator;

    (void)elapsed;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL ||
        packet->path_info.path == NULL || packet->path_info.connection == NULL) {
        return 1;
    }
    if (first_time != 0) {
        processor->action_data_3 = processor->locator;
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            value = NuStrIStr(params[index], "name");
            if (value != NULL) {
                processor->action_data_3 = AIPathFindLocator(sys, value + NuStrLen("name") + 1);
                ++index;
            }
        }
    }
    locator = (AILOCATOR_PC *)(processor->action_data_3);
    if (locator != NULL) {
        packet->movement_look_target = &locator->position;
        return 1;
    }
    return 0;
}

i32 __cdecl Action_SetRespawnLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    AILOCATOR_PC *locators[32];
    i32 count;
    i32 i;
    char *value;

    if (packet != NULL && packet->owner != NULL && ((APIOBJECT_PC *)packet->owner)->owner != NULL && first_time != 0) {
        packet->respawn_locator = processor->locator;
        count = 0;
        for (i = 0; i < param_count; i++) {
            value = NuStrIStr(params[i], "locator");
            if (value != NULL) {
                if (count < 32) {
                    locators[count] = AIPathFindLocator(sys, value + 8);
                    if (locators[count] != NULL) {
                        count++;
                    }
                }
            }
        }
        if (count != 0) {
            packet->respawn_locator = locators[NuRand(NULL) % count];
        }
    }
    return 1;
}

i32 __cdecl Action_BlockPath(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    char *from;
    char *to;
    i32 both_ways;
    i32 blocked;
    i32 index;
    char *value;

    (void)processor;
    (void)packet;
    (void)param_6;
    if (sys != NULL && sys->path_sys != NULL && sys->path_sys->path_count != 0 && param_5 != 0 && param_4 > 0) {
        from = NULL;
        to = NULL;
        both_ways = 0;
        blocked = 1;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionFromName);
            if (value != NULL) {
                from = value + 5;
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIActionToName);
            if (value != NULL) {
                to = value + 3;
            } else if (NuStrICmp(params[index], AIActionBothWaysName) == 0) {
                both_ways = 1;
            } else if (NuStrICmp(params[index], AIActionFalseName) == 0) {
                blocked = 0;
            }
        }
        if (to != NULL && from != NULL) {
            AIPathCnxSetTemporaryBlock(sys->path_sys->active_path, from, to, blocked);
            if (both_ways) {
                AIPathCnxSetTemporaryBlock(sys->path_sys->active_path, to, from, blocked);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_PathConnectionMaxLength(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                           char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 direction;
    char *from;
    char *to;
    f32 length;
    i32 i;
    char *value;
    AIPATHCNX_PC *connection;

    direction = 0;
    if (sys != NULL && first_time != 0) {
        from = NULL;
        to = NULL;
        length = 0.0f;
        for (i = 0; i < param_count; i++) {
            value = NuStrIStr(params[i], (char *)AIActionFromName);
            if (value != NULL) {
                from = value + 5;
            } else if ((value = NuStrIStr(params[i], (char *)AIActionToName)) != NULL) {
                to = value + 3;
            } else if ((value = NuStrIStr(params[i], (char *)AIActionLengthName)) != NULL) {
                length = AIParamToFloatEx(packet, processor, value + 7);
            }
        }
        if (to != NULL && from != NULL) {
            connection = (AIPATHCNX_PC *)(AIPAthFindPathCnx(sys, sys->path_sys->active_path, from, to, &direction));
            if (connection != NULL) {
                connection->max_horizontal_distance = length;
                if (length != 0.0f &&
                    connection->horizontal_distance > length + sys->path_sys->active_path->nodes[0].radius +
                                                          sys->path_sys->active_path->nodes[0].radius) {
                    connection->traversal_flags[0] |= 0x08000000;
                    connection->traversal_flags[1] |= 0x08000000;
                } else {
                    connection->traversal_flags[0] &= ~0x08000000;
                    connection->traversal_flags[1] &= ~0x08000000;
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_PathConnectionObstacle(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 direction;
    char *from;
    char *to;
    i32 open;
    i32 i;
    char *value;
    AIPATHCNX_PC *connection;

    direction = 0;
    if (sys != NULL && first_time != 0) {
        from = NULL;
        to = NULL;
        open = 0;
        for (i = 0; i < param_count; i++) {
            value = NuStrIStr(params[i], (char *)AIActionFromName);
            if (value != NULL) {
                from = value + 5;
            } else if ((value = NuStrIStr(params[i], (char *)AIActionToName)) != NULL) {
                to = value + 3;
            } else if (NuStrICmp(params[i], AIActionOpenName) == 0) {
                open = 1;
            }
        }
        if (to != NULL && from != NULL) {
            connection = (AIPATHCNX_PC *)(AIPAthFindPathCnx(sys, NULL, from, to, &direction));
            if (connection != NULL && (connection->traversal_flags[0] & 0x20000000) != 0) {
                connection->open = open;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetInterrupt(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_count, i32 is_first_time, f32 elapsed) {
    u8 priority;
    u8 id;
    char *state_name;
    f32 time;
    char *value;
    i32 i;

    if (is_first_time && processor != NULL && param_count > 0) {
        priority = 0;
        id = 0;
        state_name = NULL;
        time = 0.0f;

        for (i = 0; i < param_count; i++) {
            if ((value = NuStrIStr(params[i], "priority")) != NULL) {
                priority = (i32)(AIParamToFloatEx(packet, processor, value + 9));
            } else if ((value = NuStrIStr(params[i], (char *)AIActionIDName)) != NULL) {
                id = (i32)(AIParamToFloatEx(packet, processor, value + 3));
            } else if ((value = NuStrIStr(params[i], (char *)AIActionStateName)) != NULL) {
                state_name = value + 6;
            } else if ((value = NuStrIStr(params[i], (char *)AIActionTimeName)) != NULL) {
                time = AIParamToFloatEx(packet, processor, value + 5);
            }
        }
        if (state_name != NULL) {
            AIScriptSetInterrupt(processor, priority, id, state_name, time);
        }
    }
    return 1;
}

i32 __cdecl Action_ClearInterrupt(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_count, i32 is_first_time, f32 elapsed) {
    char *state_name;
    i32 i;
    char *value;

    if (is_first_time && processor != NULL && param_count > 0) {
        state_name = NULL;
        for (i = 0; i < param_count; i++) {
            value = NuStrIStr(params[i], (char *)AIActionStateName);
            if (value != NULL) {
                state_name = value + 6;
            }
        }
        if (state_name != NULL) {
            if (processor->interrupt_state != NULL && NuStrCmp(state_name, processor->interrupt_state->name) == 0) {
                processor->interrupt_priority = 0;
                processor->interrupt_id = 0;
                processor->interrupt_timer = 0.0f;
                processor->interrupt_state = NULL;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_NotifyStateChange(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                     char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 enabled;
    i32 i;

    enabled = 1;
    for (i = 0; i < param_count; i++) {
        if (NuStrICmp(params[i], AIScriptFalse) == 0) {
            enabled = 0;
        }
    }
    if (enabled) {
        AiSysStateDebugee = processor;
    } else {
        AiSysStateDebugee = NULL;
    }
    return 1;
}

i32 __cdecl Action_FollowOpponent(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *target;

    if (packet == NULL) {
        return 1;
    }

    if (first_time != 0) {
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[index], AIActionIgnoreRadiusName) == 0) {
                processor->action_data_1 |= 2;
            } else if (NuStrICmp(params[index], AIPathCanGoOffPathName) == 0) {
                processor->action_data_1 |= 1;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
    }

    target = (APIOBJECT_PC *)(packet->opponent);
    if (target != NULL && target->ai != NULL) {
        FollowAPIObject(((APIOBJECT_PC *)packet->owner), target, processor->action_data_1,
                        packet->movement_instruction_parameter);
    }
    return 0;
}

i32 __cdecl Action_RetreatFromOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    AIPACKET_PC *target;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        packet->movement_instruction_parameter = 1.0f;
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn == NULL || AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) == 0) {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (packet->opponent != NULL && ((APIOBJECT_PC *)packet->opponent)->ai != NULL) {
        target = ((APIOBJECT_PC *)packet->opponent)->ai;
        AIMoveInstruction(packet, &target->last_path_position, target->mover_height, &target->path_info, 2,
                          packet->movement_instruction_parameter);
    }
    return 0;
}

i32 __cdecl Action_RetreatFromNearestOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                              char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    AIPACKET_PC *target;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        packet->movement_instruction_parameter = 1.0f;
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn == NULL || AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) == 0) {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (packet->nearest_opponent != NULL) {
        target = ((APIOBJECT_PC *)packet->nearest_opponent)->ai;
        AIMoveInstruction(packet, &target->last_path_position, target->mover_height, &target->path_info, 2,
                          packet->movement_instruction_parameter);
    }
    return 0;
}

i32 __cdecl Action_GoToLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_count, i32 is_first_time, f32 elapsed) {
    AILOCATOR_PC *locator;
    NUVEC distance_vector;
    f32 distance_squared;
    f32 reach_distance;
    i32 random_locator_count;
    i32 use_personal_name;
    i32 use_indexed_name;
    f32 minimum_time;
    f32 maximum_time;
    char locator_name[64];
    i32 index;
    AILOCATOR_PC *locator_2;
    char *value;
    AILOCATOR_PC *locator_3;

    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL ||
        packet->path_info.path == NULL || packet->path_info.connection == NULL) {
        return 1;
    }

    if (is_first_time == 0) {
        locator = (AILOCATOR_PC *)(processor->action_data_3);
        if (locator == NULL) {
            return 1;
        }

        if ((processor->action_data_1 & 8u) == 0) {
            AIMoveInstruction(packet, &locator->position, 0.0f, &locator->path_info, 1,
                              packet->movement_instruction_parameter);
        } else {
            AIMoveInstruction(packet, &locator->position, 0.0f, &locator->path_info, 7,
                              packet->movement_instruction_parameter);
        }

        if ((processor->action_data_1 & 4u) != 0 && packet->opponent != NULL) {
            packet->movement_look_target = &packet->opponent->position;
        }
        if ((processor->action_data_1 & 1u) != 0 && ((APIOBJECT_PC *)packet->owner)->grounded == 0) {
            return 0;
        }

        distance_squared = (processor->action_data_1 & 2u) != 0
                               ? NuVecXZDistSqr(&packet->terrain_origin, &locator->position, &distance_vector)
                               : NuVecDistSqr(&packet->terrain_origin, &locator->position, &distance_vector);
        reach_distance = packet->movement_instruction_parameter + ai_moveradius;
        reach_distance = elapsed * packet->owner->horizontal_velocity_magnitude + reach_distance;
        if (reach_distance * reach_distance > distance_squared) {
            if ((processor->action_data_1 & 4u) == 0 || packet->opponent == NULL) {
                packet->movement_look_target = &processor->action_pos;
            }
            if (!(processor->action_timer > 0.0f)) {
                return 1;
            }
            processor->action_timer -= elapsed;
            if (processor->action_timer < 0.0f) {
                processor->action_timer = 0.0f;
            }
            return 0;
        } else {
            if ((packet->flags & 0x400000u) != 0 && (processor->action_data_1 & 16u) != 0 &&
                AIBigJumpToDestinationFn != NULL) {
                return AIBigJumpToDestinationFn(((APIOBJECT_PC *)packet->owner), &locator->position) != 0;
            }
            return 0;
        }
    }

    processor->action_data_1 = 0;
    processor->action_data_3 = processor->locator;

    random_locator_count = 0;
    use_personal_name = 0;
    use_indexed_name = 0;
    minimum_time = 0.0f;
    maximum_time = 0.0f;

    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }

            if (NuStrIStr(params[index], "name") != NULL || NuStrIStr(params[index], "teleport") != NULL) {
                ++index;
                if (index < param_count) {
                    if (use_indexed_name != 0 && ((APIOBJECT_PC *)packet->owner)->locator_name_index != -1) {
                        sprintf(locator_name, AILocatorIndexedFormat, params[index],
                                ((APIOBJECT_PC *)packet->owner)->locator_name_index);
                    } else if (use_personal_name != 0 && ((APIOBJECT_PC *)packet->owner)->character_data != NULL) {
                        sprintf(locator_name, AILocatorPersonalFormat, params[index],
                                ((APIOBJECT_PC *)packet->owner)->character_data->name);
                    } else if (random_locator_count != 0) {
                        sprintf(locator_name, AILocatorIndexedFormat, params[index],
                                NuRand(NULL) % random_locator_count);
                    } else {
                        sprintf(locator_name, params[index]);
                    }

                    processor->action_data_3 = AIPathFindLocator(sys, locator_name);
                }
                if (NuStrIStr(params[index - 1], "teleport") != NULL) {
                    locator_2 = (AILOCATOR_PC *)(processor->action_data_3);
                    if (locator_2 == NULL) {
                        return 1;
                    }
                    packet->owner->position = locator_2->position;
                    return 1;
                }
                continue;
            }

            if (NuStrIStr(params[index], (char *)AIActionPersonalName) != NULL) {
                use_personal_name = 1;
                continue;
            }
            if (NuStrIStr(params[index], (char *)AIActionIndexedName) != NULL) {
                use_indexed_name = 1;
                continue;
            }

            value = NuStrIStr(params[index], "random");
            if (value != NULL) {
                random_locator_count = AIParamToFloatEx(packet, processor, value + NuStrLen("random") + 1);
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIPathWaitTimeName);
            if (value != NULL) {
                processor->action_timer = AIParamToFloatEx(packet, processor, value + NuStrLen(AIPathWaitTimeName) + 1);
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIActionMinTimeName);
            if (value != NULL) {
                minimum_time = AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionMinTimeName) + 1);
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIActionMaxTimeName);
            if (value != NULL) {
                maximum_time = AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionMaxTimeName) + 1);
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIPathGoalRangeName);
            if (value != NULL) {
                packet->movement_instruction_parameter =
                    AIParamToFloatEx(packet, processor, value + NuStrLen(AIPathGoalRangeName) + 1);
                continue;
            }

            if (NuStrICmp(params[index], "on_ground") == 0) {
                processor->action_data_1 |= 1u;
            } else if (NuStrICmp(params[index], AIPathXZRangeCheckName) == 0) {
                processor->action_data_1 |= 2u;
            } else if (NuStrICmp(params[index], AIPathFaceOpponentName) == 0) {
                processor->action_data_1 |= 4u;
            } else if (NuStrICmp(params[index], AIPathIgnorePathName) == 0) {
                processor->action_data_1 |= 8u;
            } else if (NuStrICmp(params[index], AIPathMustReachDestinationName) == 0) {
                processor->action_data_1 |= 16u;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
    }

    if (processor->action_timer == 0.0f) {
        if (maximum_time > minimum_time) {
            processor->action_timer = NuRandFloat() * (maximum_time - minimum_time) + minimum_time;
        } else {
            processor->action_timer = 0.01f;
        }
    }

    locator_3 = (AILOCATOR_PC *)(processor->action_data_3);
    if (locator_3 == NULL) {
        return 1;
    }

    if ((processor->action_data_1 & 8u) != 0) {
        AIMoveInstruction(packet, &locator_3->position, 0.0f, &locator_3->path_info, 7,
                          packet->movement_instruction_parameter);
    } else {
        AIMoveInstruction(packet, &locator_3->position, 0.0f, &locator_3->path_info, 1,
                          packet->movement_instruction_parameter);
    }

    processor->action_pos.x = 0.0f;
    processor->action_pos.y = 0.0f;
    processor->action_pos.z = 1.0f;
    NuVecRotateY(&processor->action_pos, &processor->action_pos, locator_3->rotation);
    NuVecAdd(&processor->action_pos, &processor->action_pos, &locator_3->position);
    return 0;
}

i32 __cdecl Action_SetLocator(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 count, i32 first_time, f32 elapsed) {
    char *name;
    char *value;
    char buffer[64];
    i32 personal;
    i32 indexed;
    i32 random_count;
    i32 index;
    APIOBJECT_PC *object;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL ||
        packet->path_info.path == NULL || packet->path_info.connection == NULL || first_time == 0)
        return 1;
    processor->locator = NULL;
    name = NULL;
    personal = 0;
    indexed = 0;
    random_count = 0;
    for (index = 0; index < count; index++) {
        value = NuStrIStr(params[index], "name");
        if (value != NULL)
            name = value + 5;
        else if (NuStrIStr(params[index], (char *)AIActionPersonalName) != NULL)
            personal = 1;
        else if (NuStrIStr(params[index], (char *)AIActionIndexedName) != NULL)
            indexed = 1;
        else {
            value = NuStrIStr(params[index], "random");
            if (value != NULL)
                random_count = (i32)AIParamToFloatEx(packet, &packet->script_processor, value + 7);
        }
    }
    if (name != NULL) {
        object = (APIOBJECT_PC *)packet->owner;
        if (indexed != 0 && object->locator_name_index != -1)
            sprintf(buffer, AILocatorIndexedFormat, name, object->locator_name_index);
        else if (personal != 0 && object->character_data != NULL)
            sprintf(buffer, AILocatorPersonalFormat, name, object->character_data->name);
        else if (random_count != 0)
            sprintf(buffer, AILocatorIndexedFormat, name, NuRand(NULL) % random_count);
        else
            sprintf(buffer, name);
        processor->locator = AIPathFindLocator(system, buffer);
    }
    return 1;
}

i32 __cdecl Action_FollowPlayer(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_count, i32 first_time, f32 elapsed) {
    if (packet == NULL) {
        return 1;
    }

    if (first_time != 0) {
        i32 index;
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[index], AIActionIgnoreRadiusName) == 0) {
                processor->action_data_1 |= 2;
            } else if (NuStrICmp(params[index], AIPathCanGoOffPathName) == 0) {
                processor->action_data_1 |= 1;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
    }

    if (sys->player_1 != NULL && sys->player_1->ai != NULL) {
        FollowAPIObject(((APIOBJECT_PC *)packet->owner), sys->player_1, processor->action_data_1,
                        packet->movement_instruction_parameter);
    }
    return 0;
}

i32 __cdecl Action_CircleLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
    AILOCATOR_PC *locator;
    i32 index;
    char *value;
    (void)elapsed;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL || packet->path_info.path == NULL ||
        packet->path_info.connection == NULL) {
        return 1;
    }
    if (first_time != 0) {
        processor->action_data_1 = 0;
        processor->action_data_3 = processor->locator;
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value == NULL) {
                value = NuStrIStr(params[index], (char *)AIActionCircleTeleportName);
            }
            if (value != NULL) {
                processor->action_data_3 =
                    AIPathFindLocator(sys, value + NuStrLen((char *)AIScriptActionNameArgPrefix));
            } else if ((value = NuStrIStr(params[index], (char *)AIPathGoalRangeName)) != NULL) {
                packet->movement_instruction_parameter =
                    AIParamToFloatEx(packet, processor, value + NuStrLen((char *)AIPathGoalRangeName) + 1);
            } else if (NuStrICmp(params[index], (char *)AIActionCircleReverseName) == 0) {
                packet->flags ^= 0x200;
            } else if (NuStrICmp(params[index], (char *)AIActionCircleAnticlockwiseName) == 0) {
                packet->flags &= ~0x200u;
            } else if (NuStrICmp(params[index], (char *)AIActionCircleClockwiseName) == 0) {
                packet->flags |= 0x200;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
    }
    locator = (AILOCATOR_PC *)processor->action_data_3;
    if (locator != NULL) {
        AIMoveInstruction(packet, &locator->position, packet->movement_instruction_parameter, &locator->path_info, 3,
                          packet->movement_instruction_parameter);
    }
    return locator == NULL;
}

i32 __cdecl Action_GizmoActivate(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2, char **params,
                                 i32 count, i32 first_time, f32 unused6) {
    void *gizmo = NULL;
    i32 enabled = 1;
    i32 index;
    char *value;
    if (first_time != 0 && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL)
                gizmo = GizmoFindByName(WORLD->gizmo_sys, -1, value + NuStrLen(AIScriptActionNameArgPrefix));
            else if (NuStrICmp(params[index], AIActionFalseName) == 0)
                enabled = 0;
        }
        if (gizmo != NULL)
            GizmoActivate(WORLD->gizmo_sys, gizmo, enabled, 1);
    }
    return 1;
}

i32 __cdecl Action_GizmoSetVisibility(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2,
                                      char **params, i32 count, i32 first_time, f32 unused6) {
    void *gizmo = NULL;
    i32 enabled = 1;
    i32 index;
    char *value;
    if (first_time != 0 && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL)
                gizmo = GizmoFindByName(WORLD->gizmo_sys, -1, value + NuStrLen(AIScriptActionNameArgPrefix));
            else if (NuStrICmp(params[index], AIActionFalseName) == 0)
                enabled = 0;
        }
        if (gizmo != NULL)
            GizmoSetVisibility(WORLD->gizmo_sys, gizmo, enabled, 1);
    }
    return 1;
}

i32 __cdecl Action_GoToOriginalPath(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    GAMEOBJECT *object;
    AIPATH_PC *original_path;
    AIPATH_PC *current_path;
    AIPATHNODE_PC *node;
    AIPATHNODE_PC *target_node;
    AIPATHCNX_PC *connection;
    AIPATHINFO_PC *move_path;
    i32 index;
    i32 node_index;
    f32 distance;
    (void)elapsed;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }
    object = packet->owner->objptr;
    if (sys == NULL || sys->path_sys == NULL || (object->flags_1f8 & 0x400) == 0 ||
        object->ai_packet.creature_index == 0xff) {
        return 0;
    }
    original_path = sys->creatures[packet->creature_index].path_info.path;
    current_path = packet->path_info.path;
    if (first_time != 0) {
        if (current_path == original_path) {
            return 1;
        }
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL) {
                AIActionParseSpeedFn(params[index], &packet->goal_speed_mode);
            }
        }
        current_path = packet->path_info.path;
        packet->goal_path_node = NULL;
        if (current_path == NULL) {
            return 0;
        }
        if (sys->path_sys->active_path != current_path || original_path == NULL || (original_path->flags & 2) == 0 ||
            original_path->node_count == 0) {
            return 0;
        }
        node = original_path->nodes;
        node_index = 0;
        while (node_index < original_path->node_count && (node->runtime_flags & 1) == 0) {
            ++node_index;
            ++node;
        }
        if (node_index >= original_path->node_count) {
            return 1;
        }
        processor->action_data_3 = node;
        AISysCharacterSetPath(packet, current_path);
        connection = &sys->path_sys->active_path->connections[(u16)node->path_flags];
        move_path = &processor->path_info;
        target_node = &move_path->path->nodes[connection->node_indices[0]];
        move_path->connection = connection;
        move_path->direction = 0;
        move_path->flags |= AIPATHINFO_FLAG_ON_PATH_LOCAL;
        difference.x = node->position.x - target_node->position.x;
        difference.z = node->position.z - target_node->position.z;
        NuVecRotateY(&difference, &difference, -connection->rotation);
        move_path->distance = difference.z / connection->max_horizontal_distance;
        move_path->width = difference.x;
        AIMoveInstruction(packet, &node->position, 0.0f, move_path, 1, packet->movement_instruction_parameter);
        return 0;
    }
    node = (AIPATHNODE_PC *)processor->action_data_3;
    if (node == NULL) {
        return 0;
    }
    distance = NuVecXZDistSqr(&packet->terrain_origin, &node->position, &difference);
    if (distance < node->radius_squared) {
        memset(&packet->path_info, 0, sizeof(packet->path_info));
        AISysCharacterSetPath(packet, original_path);
        AISysCharacterSetPathCnx(packet, &packet->owner->position, packet->fallback_path_info.connection, 0);
        return 1;
    }
    AIMoveInstruction(packet, &node->position, 0.0f, &processor->path_info, 1, packet->movement_instruction_parameter);
    return 0;
}

static i32 Action_SetAnimation(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL && param_5 != 0 && param_4 == 1) {
        GAMEOBJECT *object = packet->owner->objptr;
        const i32 animation = FindAnimIX(object->character_data, params[0]);
        if (animation != -1) {
            ResetAnimPacket(&object->anim_packet, animation);
        }
    }
    return 1;
}

i32 __cdecl Action_BigJump(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    NUVEC destination;
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    i32 to_origin;
    i32 check_terrain;
    f32 jump_factor;
    i32 slam;
    char *value;
    AICREATURE_PC *creature;
    NUVEC *jump_destination;

    (void)param_6;
    destination.x = 1.0e9f;
    destination.y = 1.0e9f;
    destination.z = 1.0e9f;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }

    object = packet->owner->objptr;
    if (param_5 != 0) {
        target = NULL;
        to_origin = 0;
        check_terrain = 0;
        jump_factor = 1.0f;
        slam = 0;

        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral757874);
            if (value != NULL) {
                target = GetNamedGameObject(sys, value + 14);
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral75786c) == 0) {
                slam = 1;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral757860) == 0) {
                to_origin = 1;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757834)) != NULL) {
                jump_factor = AIParamToFloat(processor, value + 12);
                if (jump_factor < 0.0f) {
                    jump_factor = 0.0f;
                }
            } else if ((value = NuStrIStr(params[index], (char *)XArgPrefix)) != NULL) {
                destination.x = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)YArgPrefix)) != NULL) {
                destination.y = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)ZArgPrefix)) != NULL) {
                destination.z = AIParamToFloat(processor, value + 2);
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral757844) == 0) {
                check_terrain = 1;
            }
        }

        if (to_origin != 0 && (object->flags_1f8 & 0x400) != 0 && object->ai_packet.creature_index != 0xff) {
            creature = &sys->creatures[object->ai_packet.creature_index];
            StartBigJump(object, &creature->position, slam, jump_factor, 1.0f, 0);
            object->ai_packet.path_info = creature->path_info;
            object->ai_packet.last_path_position = creature->position;
        } else if (target != NULL) {
            StartBigJump(object, &target->last_safe_position, slam, jump_factor, 1.0f, 0);
            object->ai_packet.path_info = target->ai_packet.path_info;
            object->ai_packet.last_path_position = target->ai_packet.last_path_position;
        } else {
            jump_destination = &destination;
            if (destination.x == 1.0e9 && destination.y == 1.0e9 && destination.z == 1.0e9) {
                jump_destination = &object->collision_position;
            }
            StartBigJump(object, jump_destination, slam, jump_factor, 1.0f, check_terrain);
        }
    }
    return object->character_context != 0x1f;
}

i32 __cdecl Action_SetCurrentSpeed(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 num_params, i32 first_time, f32 elapsed) {
    i32 i;
    GAMEOBJECT *object;
    i32 speed_mode;
    f32 speed;
    char *value;

    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (!first_time)
        return 1;
    speed_mode = -1;
    speed = 0.0f;
    for (i = 0; i < num_params; i++) {
        value = NuStrIStr(params[i], (char *)AIGameCharacterName);
        if (value != NULL)
            object = GetNamedGameObject(sys, value + 10);
        else if (NuStrICmp((char *)AITransferLiteral757bb4, params[i]) == 0)
            speed_mode = 2;
        else if (NuStrICmp((char *)AITransferLiteral757ba8, params[i]) == 0)
            speed_mode = 1;
        else if (NuStrICmp((char *)AITransferLiteral757b9c, params[i]) == 0)
            speed_mode = 0;
        else
            speed = AIParamToFloat(processor, params[i]);
    }
    if (object != NULL) {
        if (speed_mode == 2)
            speed = ((PLAYERCHARACTERCONFIG_PC *)object->character_data->game_character)->tiptoe_speed;
        else if (speed_mode == 1)
            speed = ((PLAYERCHARACTERCONFIG_PC *)object->character_data->game_character)->walk_speed;
        else if (speed_mode == 0) {
            speed = ((PLAYERCHARACTERCONFIG_PC *)object->character_data->game_character)->run_speed;
            object->field_dc8 = 1.0f;
        }
        object->velocity.x = 0.0f;
        object->velocity.y = 0.0f;
        object->velocity.z = speed;
        NuVecRotateY(&object->velocity, &object->velocity, object->facing_angle);
    }
    return 1;
}

i32 __cdecl Action_SetGravityHeight(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    f32 minimum;
    f32 maximum;
    char *value;
    f32 random;

    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (object == NULL || first_time == 0) {
        return 1;
    }
    object->portal_flags &= 0x7fffffffu;
    object->hover_height_override = 1.0e9f;

    minimum = 1.0e9f;
    maximum = 1.0e9f;
    for (index = 0; index < param_count; ++index) {
        if (NuStrICmp(params[index], (char *)AIScaleOverrideResetName) == 0) {
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIControlMinimumName);
        if (value != NULL) {
            minimum = AIParamToFloat(processor, value + 4);
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIControlMaximumName);
        if (value != NULL) {
            maximum = AIParamToFloat(processor, value + 4);
        } else {
            object->hover_height_override = AIParamToFloat(processor, params[index]);
        }
    }
    if (minimum != 1.0e9 && maximum != 1.0e9) {
        random = NuRandFloat();
        object->hover_height_override = maximum * random + (1.0f - random) * minimum;
    }
    return 1;
}

i32 __cdecl Action_SnapWeaponOut(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 enabled;
    i32 keep_out;
    char *value;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    enabled = 1;
    keep_out = -1;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            object = GetNamedGameObject(sys, value + 10);
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757ec4)) != NULL) {
            keep_out = NuStrICmp(value + 9, (char *)AITransferLiteral757ebc) == 0;
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            enabled = 0;
        }
    }

    if (object != NULL) {
        if (enabled != 0) {
            object->flags_e20 |= 0x10000u;
            object->weapon_scale = 1.0f;
            if (keep_out != -1) {
                object->portal_flags = (u32)((object->portal_flags & ~0x10u) | (keep_out ? 0x10u : 0));
            }
        } else {
            object->weapon_scale = 0.0f;
            object->flags_e20 &= (u32)(~0x10000u);
            object->portal_flags &= (u32)(~0x10u);
        }
    }
    return 1;
}

i32 __cdecl Action_DontAvoidCharacter(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    GAMEOBJECT *dont_avoid;
    i32 enabled;
    char *value;

    if (first_time) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        if (param_count > 0) {
            dont_avoid = NULL;
            enabled = 1;
            for (index = 0; index < param_count; ++index) {
                value = NuStrIStr(params[index], (char *)AIGameCharacterName);
                if (value != NULL) {
                    object = GetNamedGameObject(system, value + 10);
                    continue;
                }
                value = NuStrIStr(params[index], (char *)AITransferLiteral757a5c);
                if (value != NULL) {
                    dont_avoid = GetNamedGameObject(system, value + 11);
                    continue;
                }
                if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                    enabled = 0;
            }
            if (object != NULL && dont_avoid != NULL)
                object->ai_packet.dont_avoid_character = enabled ? dont_avoid : NULL;
        }
    }
    return 1;
}

i32 __cdecl Action_MoveAwayFromLastAttacker(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                            char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    GAMEOBJECT *attacker;

    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }
    object = packet->owner->objptr;

    if (first_time != 0) {
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[index], (char *)AIPathFaceName) == 0) {
                processor->action_data_1 = 1;
            } else {
                packet->movement_instruction_parameter = AIParamToFloat(processor, params[index]);
            }
        }
    }

    attacker = object->last_attacker;
    if (attacker != NULL) {
        AIMoveInstruction(packet, &attacker->ai_packet.last_path_position, attacker->ai_packet.mover_height,
                          &attacker->ai_packet.path_info, 2, packet->movement_instruction_parameter);
        if (processor->action_data_1 != 0) {
            packet->movement_look_target = &object->last_attacker->position;
        }
    }
    return 0;
}

i32 __cdecl Action_CannotDropIn(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 enabled;
    char *value;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        enabled = 1;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                object = GetNamedGameObject(sys, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757ee0)) != NULL) {
                GetNamedGameObject(sys, value + 7);
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
        if (object != NULL) {
            object->portal_flags = (object->portal_flags & ~0x4000000u) | ((enabled & 1) << 26);
        }
    }
    return 1;
}

i32 __cdecl Action_SetSpline(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    NUGSPLINE *spline;
    i32 looping;
    char *value;
    SPLINEPOS *position;

    (void)sys;
    (void)processor;
    (void)param_6;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (object == NULL || param_5 == 0) {
        return 1;
    }

    spline = NULL;
    looping = 0;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral757a84);
        if (value != NULL) {
            spline = NuSplineFind(WORLD->current_gscn, value + 7);
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral757a7c) == 0) {
            looping = 1;
        }
    }

    position = &object->movement_spline_position;
    memset(position, 0, sizeof(*position));
    if (spline != NULL) {
        InitSplinePosition(position, spline, 0.0f, looping);
    }
    return 1;
}

i32 __cdecl Action_CameraShake(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    f32 amount;
    f32 duration;
    f32 speed;
    char *value;

    (void)sys;
    (void)packet;
    (void)param_6;
    if (param_5 != 0) {
        amount = 1.0f;
        duration = 0.0f;
        speed = 1.0f;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral757cd8);
            if (value != NULL) {
                amount = AIParamToFloat(processor, value + 4);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757cd0)) != NULL) {
                duration = AIParamToFloat(processor, value + 5);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757cc8)) != NULL) {
                speed = AIParamToFloat(processor, value + 6);
            }
        }
        GameCam_NewShake(GameCam, amount, duration, speed);
    }
    return 1;
}

i32 Action_ResetGameCamera(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    if (param_5 != 0) {
        GameCam_Reset(GameCam);
    }
    return 1;
}

i32 Action_PlayCutScene(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2, char **params,
                        i32 param_count, i32 first_time, float unused6) {
    if (first_time != 0) {
        i32 i;
        char *name = NULL;
        for (i = 0; i < param_count; ++i) {
            char *value = NuStrIStr(params[i], "name");
            if (value != NULL) {
                name = value + 5;
            }
        }
        if (name != NULL) {
            NewCutScene(NULL, WORLD->cutscene_sys, name, 0);
        }
    }
    return 1;
}

i32 __cdecl Action_SetZeroAcceleration(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    i32 enabled;

    if (first_time) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        enabled = 1;
        for (index = 0; index < param_count; ++index) {
            if (NuStrIStr(params[index], (char *)ActionSnapPlayer1Name) != NULL ||
                NuStrIStr(params[index], (char *)GameAIPlayerName) != NULL)
                object = player;
            else if (NuStrIStr(params[index], (char *)ActionSnapPlayer2Name) != NULL)
                object = player2;
            else if (NuStrIStr(params[index], (char *)AIActionFalseName) != NULL)
                enabled = 0;
        }
        if (object != NULL)
            object->ai_packet.flags = (object->ai_packet.flags & ~0x40000u) | ((enabled & 1) << 18);
    }
    return 1;
}

i32 __cdecl Action_FaceCharacter(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    char *value;
    GAMEOBJECT *target;

    if (first_time != 0) {
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                processor->action_data_3 = GetNamedGameObject(sys, value + 10);
            } else {
                processor->action_timer = AIParamToFloat(processor, params[index]);
            }
        }
    }
    target = (GAMEOBJECT *)(processor->action_data_3);
    if (packet != NULL && target != NULL) {
        packet->movement_look_target = &target->position;
    }
    if (processor->action_timer > 0.0f) {
        processor->action_timer -= elapsed;
        if (processor->action_timer <= 0.0f) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl Action_SetPath(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    AIPATHSYS_PC *path_system;
    GAMEOBJECT *object;
    AIPATH_PC *path;
    char *value;
    AIPATH_PC *candidate;
    i32 index, path_index;
    (void)param_6;
    if (param_5 == 0 || sys == NULL || sys->path_sys == NULL) {
        return 1;
    }

    path_system = sys->path_sys;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    path = NULL;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            object = GetNamedGameObject(sys, value + 10);
            continue;
        }
        if (NuStrIStr(params[index], (char *)AITransferLiteral757e70) != NULL) {
            path = sys->path_sys->active_path;
            continue;
        }
        value = NuStrIStr(params[index], (char *)AITransferLiteral757e68);
        if (value != NULL) {
            for (path_index = 0; path_index < path_system->path_count; ++path_index) {
                candidate = path_system->paths[path_index];
                if (NuStrICmp(candidate->name, value + 5) == 0) {
                    path = path_system->paths[path_index];
                    break;
                }
            }
        }
    }
    if (object != NULL && path != NULL) {
        AISysCharacterSetPath(&object->ai_packet, path);
        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
    }
    return 1;
}

i32 __cdecl Action_SetInvulnerable(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    i32 types[10];
    i32 type_count;
    i32 still_take_hit;
    GAMEOBJECT *object;
    i32 enabled;
    char *value;
    i32 local_type;
    i32 global_type;
    i32 object_count;
    GAMEOBJECT *candidate;
    i32 index, object_index, type_index;
    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    type_count = 0;
    still_take_hit = 0;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    enabled = 1;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral75772c);
        if (value != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                local_type = (u8)(LevelCharacterTypeIDFn(value + 5));
                if (local_type != 0xff) {
                    global_type = LevelCharacterGlobalIDFn(local_type);
                    if (global_type != -1 && type_count < 10) {
                        types[type_count++] = global_type;
                    }
                }
            }
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            object = GetNamedGameObject(sys, value + 10);
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            enabled = 0;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral757a44) == 0) {
            still_take_hit = 1;
        }
    }

    if (type_count != 0) {
        object_count = HIGHGAMEOBJECT;
        candidate = Obj;

        for (object_index = 0; object_index < object_count; ++object_index, ++candidate) {
            if ((candidate->flags_1fc_word & (1u | 0x1000u)) != (1u | 0x1000u) || (candidate->flags_1f8 & 0x400) == 0) {
                continue;
            }
            if (enabled) {
                for (type_index = 0; type_index < type_count; ++type_index) {
                    if ((i16)candidate->id == types[type_index]) {
                        candidate->flags_ef0 =
                            (candidate->flags_ef0 & ~0x1000u) | 0x400000u | ((still_take_hit & 1) << 12);
                    }
                }
            } else {
                for (type_index = 0; type_index < type_count; ++type_index) {
                    if ((i16)candidate->id == types[type_index]) {
                        candidate->flags_ef0 &= ~0x400000u;
                    }
                }
            }
        }
    } else if (object != NULL && (object->flags_1f8 & 0x400) != 0) {
        if (enabled) {

            object->flags_ef0 = (object->flags_ef0 & ~0x1000u) | 0x400000u | ((still_take_hit & 1) << 12);
        } else {
            object->flags_ef0 &= ~0x400000u;
        }
    }
    return 1;
}

typedef struct GIZSPECIAL_PARTIAL_PC {
    u8 unknown_00[0x20];
    GAMEANIMSET *anim_set;
} GIZSPECIAL_PARTIAL_PC;
STATIC_ASSERT(offsetof(GIZSPECIAL_PARTIAL_PC, anim_set) == 0x20);
i32 __cdecl Action_SetSide(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    char *value;
    GAMEOBJECT *object;
    i32 side;
    f32 range_squared;
    i32 type_ids[10];
    i32 type_count;
    u8 local_type;
    i32 global_type;
    f32 range;
    u32 old_state;
    u32 new_state;
    i32 type;
    u32 model_flags;
    f32 distance_squared;
    GAMEOBJECT *candidate;
    i32 type_matches;
    NUVEC offset;

    i32 index, type_index;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object =
        packet != NULL && packet->owner != NULL && (packet->owner->flags_1fc & 1u) != 0 ? packet->owner->objptr : NULL;
    side = 0;
    range_squared = 0.0f;

    type_count = 0;

    for (index = 0; index < param_4; ++index) {
        if (NuStrICmp(params[index], (char *)AITransferLiteral75773c) == 0) {
            side = 0;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral7576a8) == 0 ||
                   NuStrICmp(params[index], (char *)AITransferLiteral7576b0) == 0) {
            side = -1;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral7576b8) == 0 ||
                   NuStrICmp(params[index], (char *)AITransferLiteral7576c0) == 0) {
            side = 1;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral75768c) == 0 ||
                   NuStrICmp(params[index], (char *)AITransferLiteral75769c) == 0) {
            side = 2;
        } else if (NuStrICmp(params[index], (char *)GizmoDefaultOutputName) == 0) {
            side = 3;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral75772c)) != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                local_type = (u8)(LevelCharacterTypeIDFn(value + 5));
                if (local_type != 0xff) {
                    global_type = LevelCharacterGlobalIDFn(local_type);
                    if (global_type != -1 && type_count < 10) {
                        type_ids[type_count++] = global_type;
                    }
                }
            }
        } else if ((value = NuStrIStr(params[index], (char *)AIGameCharacterName)) != NULL) {
            object = GetNamedGameObject(sys, value + 10);
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757724)) != NULL) {
            range = AIParamToFloat(processor, value + 6);
            range_squared = range * range;
        }
    }

    if (type_count == 0 && !(range_squared > 0.0f)) {
        if (object != NULL) {

            old_state = object->flags_1f8;
            new_state = old_state & 0xfffefffau;
            if (side == 1) {
                new_state |= 1;
            } else if (side == 0) {
                new_state |= 4;
            } else if (side == 2) {
                new_state |= 0x10000;
            } else if (side == 3 && (old_state & 0x400u) != 0 && object->ai_packet.creature_index != 0xff) {
                type = sys->creatures[object->ai_packet.creature_index].type;
                {
                    model_flags = apicharsys->char_data[type].model_flags;
                    if ((model_flags & 0x200) != 0) {
                        new_state |= 4;
                    } else if ((model_flags & 4) != 0) {
                        new_state |= 1;
                    }
                }
            }
            object->flags_1f8 = new_state;

            object->ai_packet.capabilities &= ~((u32)(LEGO_AIPATHCNX_FORGOODIES) | (u32)(LEGO_AIPATHCNX_FORBADDIES));
            if ((new_state & 1) != 0) {
                object->ai_packet.capabilities |= LEGO_AIPATHCNX_FORBADDIES;
            } else if ((new_state & 4) == 0) {
                object->ai_packet.capabilities |= LEGO_AIPATHCNX_FORGOODIES;
            }

            if ((old_state & new_state & 0x10005) == 0) {
                object->ai_packet.flags &= ~0x800u;
                object->ai_packet.opponent = NULL;
                object->field_ec0 = 0;
                object->field_ec4 = 0.0f;
            }
        }
        return 1;
    }

    distance_squared = 0.0f;
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        candidate = &Obj[index];
        if ((candidate->flags_1fc_word & (1u | 0x1000u)) != (1u | 0x1000u) || (candidate->flags_1f8 & 0x400u) == 0) {
            continue;
        }

        type_matches = 0;
        for (type_index = 0; type_index < type_count; ++type_index) {
            type_matches |= (i16)candidate->id == type_ids[type_index];
        }
        if (range_squared > 0.0f && object != NULL) {

            NuVecSub(&offset, &object->position, &candidate->position);
            distance_squared = offset.x * offset.x + offset.y * offset.y + offset.z * offset.z;
        }
        if ((type_matches && (range_squared == 0.0f || distance_squared < range_squared)) ||
            (type_count == 0 && distance_squared < range_squared)) {
            if (candidate != NULL) {

                old_state = candidate->flags_1f8;
                new_state = old_state & 0xfffefffau;
                if (side == 1) {
                    new_state |= 1;
                } else if (side == 0) {
                    new_state |= 4;
                } else if (side == 2) {
                    new_state |= 0x10000;
                } else if (side == 3 && (old_state & 0x400u) != 0 && candidate->ai_packet.creature_index != 0xff) {
                    type = sys->creatures[candidate->ai_packet.creature_index].type;
                    {
                        model_flags = apicharsys->char_data[type].model_flags;
                        if ((model_flags & 0x200) != 0) {
                            new_state |= 4;
                        } else if ((model_flags & 4) != 0) {
                            new_state |= 1;
                        }
                    }
                }
                candidate->flags_1f8 = new_state;

                candidate->ai_packet.capabilities &=
                    ~((u32)(LEGO_AIPATHCNX_FORGOODIES) | (u32)(LEGO_AIPATHCNX_FORBADDIES));
                if ((new_state & 1) != 0) {
                    candidate->ai_packet.capabilities |= LEGO_AIPATHCNX_FORBADDIES;
                } else if ((new_state & 4) == 0) {
                    candidate->ai_packet.capabilities |= LEGO_AIPATHCNX_FORGOODIES;
                }

                if ((old_state & new_state & 0x10005) == 0) {
                    candidate->ai_packet.flags &= ~0x800u;
                    candidate->ai_packet.opponent = NULL;
                    candidate->field_ec0 = 0;
                    candidate->field_ec4 = 0.0f;
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetTakeOverTarget(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                     char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    char *type_name;
    GAMEOBJECT *target;
    f32 nearest;
    i32 check_takeover;
    char *value;
    i32 type;
    GAMEOBJECT *candidate;
    NUVEC difference;
    f32 distance;
    GAMEOBJECT *previous;

    if (!first_time)
        return 1;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    type_name = NULL;
    target = NULL;
    nearest = 1000000000.0f;
    check_takeover = 0;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral7576f8);
        if (value != NULL)
            type_name = value + 5;
        else if ((value = NuStrIStr(params[index], (char *)AICombatOpponentName)) != NULL)
            target = GetNamedGameObject(system, value + 9);
        else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral7576e0)) != NULL) {
            nearest = AIParamToFloat(processor, value + 9);
            nearest *= nearest;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral7576d0) != 0)
            check_takeover = 1;
    }
    if (target == NULL && type_name == NULL)
        return 1;
    type = -1;
    for (index = 0; index < CHARCOUNT && type == -1; ++index) {
        if (NuStrICmp(CDataList[index].name, type_name) == 0)
            type = index;
    }
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        candidate = &Obj[index];
        if ((candidate->flags_1fc_word & 0x1001) != 0x1001 || (i16)candidate->id != type)
            continue;
        if (candidate->takeover_target != NULL && candidate->takeover_target != object)
            continue;
        if (candidate->takeover_source != NULL &&
            (candidate->takeover_source->character_context == 0x3b || candidate->character_context != 0x3b))
            continue;

        NuVecSub(&difference, &candidate->collision_position, &packet->owner->collision_position);
        distance = NuVecMagSqr(&difference);
        if (!(distance < nearest))
            continue;
        nearest = distance;
        target = candidate;
    }
    if (target == NULL) {
        object->takeover_target = NULL;
        return 1;
    }
    if (check_takeover &&
        (target->takeover_source != NULL || !(target->character_data->game_character->flags_090 & 0x40) ||
         (object->character_data->model_flags & 0x10)))
        return 1;
    previous = object->takeover_target;
    if (previous != NULL && previous->takeover_target != NULL && previous->takeover_target == object)
        previous->takeover_target = NULL;
    object->takeover_target = target;
    target->takeover_target = object;
    return 1;
}

i32 __cdecl Action_SetStateArea(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    char *state_name;
    i32 types[10];
    i32 type_count;
    f32 range_squared;
    char *value;
    f32 range;
    i32 local_type;
    i32 global_type;
    NUVEC origin;
    GAMEOBJECT *object;
    NUVEC difference;
    f32 distance_squared;
    i32 matching_type;
    AIPACKET_PC *object_packet;
    i32 index, object_index, type_index;
    (void)sys;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    state_name = NULL;

    type_count = 0;
    range_squared = 0.0f;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral757724);
        if (value != NULL) {
            range = AIParamToFloat(processor, value + 6);
            range_squared = range * range;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral75772c)) != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                local_type = (u8)(LevelCharacterTypeIDFn(value + 5));
                if (local_type != 0xff) {
                    global_type = LevelCharacterGlobalIDFn(local_type);
                    if (global_type != -1 && type_count < 10) {
                        types[type_count++] = global_type;
                    }
                }
            }
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757744)) != NULL) {
            state_name = value + 6;
        }
    }

    if (state_name == NULL || (type_count == 0 && !(range_squared > 0.0f))) {
        processor->next_state = AIStateFind(state_name, processor->script);
        return 1;
    }

    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        origin = packet->owner->objptr->collision_position;
    }

    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index) {
        object = &Obj[object_index];
        if ((object->flags_1fc_word & (1u | 0x1000u)) != (1u | 0x1000u) || (object->flags_1f8 & 0x400) == 0) {
            continue;
        }

        NuVecSub(&difference, &origin, &object->position);
        distance_squared = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;

        matching_type = type_count == 0;
        for (type_index = 0; type_index < type_count; ++type_index) {
            if ((i16)object->id == types[type_index]) {
                matching_type = 1;
            }
        }
        if (matching_type != 0 && range_squared > distance_squared) {
            object_packet = ((APIOBJECT_PC *)object)->ai;
            object_packet->script_processor.next_state =
                AIStateFind(state_name, object_packet->script_processor.script);
        }
    }
    return 1;
}

i32 __cdecl Action_SnapToOrigin(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 check_terrain;
    char *value;
    AICREATURE_PC *creature;
    u16 rotation;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    check_terrain = 1;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
        if (value != NULL) {
            object = GetNamedGameObject(sys, value + 10);
        } else if (NuStrIStr(params[index], (char *)ActionSnapDontCheckTerrainName) != NULL) {
            check_terrain = 0;
        }
    }

    if (object == NULL || (object->flags_1f8 & 0x400) == 0 || object->ai_packet.creature_index == 0xff) {
        return 1;
    }

    creature = &sys->creatures[object->ai_packet.creature_index];
    rotation = (u16)(creature->y_rotation);
    object->field_0x276 = rotation;
    object->facing_angle = rotation;
    object->movement_facing_angle = rotation;
    object->ai_packet.path_info.flags_word &= ~1u;
    object->position = creature->position;
    object->initial_position = object->position;
    object->collision_position = object->position;
    plr_lastpos = object->position;
    object->start_position = object->position;
    object->respawn_position = object->position;
    object->last_safe_position = object->position;
    object->ai_update_position = object->position;
    object->velocity = v000;
    InitSurfaceInfo(object);
    if (check_terrain != 0) {
        SetObjOnSurface(object, 0);
    }
    return 1;
}

i32 __cdecl Action_GameDontPush(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 dont_push;
    i32 types[10];
    i32 type_count;
    i32 type_index;
    i32 object_index;
    GAMEOBJECT *candidate;
    char *value;
    i32 local_type;
    i32 global_type;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    dont_push = 1;

    type_count = 0;

    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral75772c);
        if (value != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                local_type = (u32)LevelCharacterTypeIDFn(value + 5);
                if (local_type != 0xff) {
                    global_type = LevelCharacterGlobalIDFn(local_type);
                    if (global_type != -1 && type_count < 10)
                        types[type_count++] = global_type;
                }
            }
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            object = GetNamedGameObject(sys, value + NuStrLen((char *)AIGameCharacterName));
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            dont_push = 0;
        }
    }
    if (type_count != 0) {
        for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index) {
            candidate = &Obj[object_index];
            if ((candidate->flags_1fc_word & 0x1001u) != 0x1001u || (candidate->flags_1f8 & 0x400u) == 0)
                continue;
            for (type_index = 0; type_index < type_count; ++type_index) {
                if ((i16)candidate->id == types[type_index])
                    candidate->flags_1fc_word = (candidate->flags_1fc_word & ~2u) | ((dont_push & 1) << 1);
            }
        }
    } else if (object != NULL) {
        object->flags_1fc_word = (object->flags_1fc_word & (u32)(~2u)) | (u32)(dont_push ? 2 : 0);
    }
    return 1;
}

i32 __cdecl Action_SetAIOverrideControl(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *object;
    i32 enabled;
    char *value;
    GAMEOBJECT *character;

    enabled = 1;
    if (!first_time) {
        object = processor->action_data_3;
    } else {
        object = packet != NULL && packet->owner != NULL ? (APIOBJECT_PC *)packet->owner : NULL;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
            if (value != NULL) {
                if (GetNamedGameObjectCallback != NULL)
                    object = GetNamedGameObjectCallback(system, value + 10);
            } else if (NuStrICmp((char *)AIActionFalseName, params[index]) == 0) {
                enabled = 0;
            }
        }
        processor->action_data_3 = object;
    }
    if (object != NULL) {
        if (enabled) {
            character = (GAMEOBJECT *)object->owner;
            if (character != NULL) {
                {
                    character->field_1010 = 0.0f;
                    character->death_timer = 0;
                    if (character->dead != 0) {
                        character->dead = 0;
                        character->current_hp = character->field_107e;
                        character->shield_hitpoints = character->character_data->game_character->shield_hitpoints;
                        character->saved_hitpoints = 4;
                    }
                }
                if (character->takeover_source != NULL) {
                    character->takeover_source->field_1010 = 0.0f;
                    character->takeover_source->death_timer = 0;
                    if (character->takeover_source->dead != 0) {
                        character->takeover_source->dead = 0;
                        character->takeover_source->current_hp = character->takeover_source->field_107e;
                        character->takeover_source->shield_hitpoints =
                            character->takeover_source->character_data->game_character->shield_hitpoints;
                        character->takeover_source->saved_hitpoints = 4;
                    }
                }
            }
        }
        object->flags_low = (object->flags_low & ~0x100u) | ((enabled & 1) << 8);
    }
    return 1;
}

i32 __cdecl Action_PlayGizSpecial(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GIZSPECIAL_PARTIAL_PC *special;
    i32 backwards;
    i32 snap;
    i32 restart;
    char *value;
    GIZMO_PC *gizmo;

    (void)sys;
    (void)processor;
    (void)packet;
    (void)param_6;
    if (param_5 == 0 || param_4 <= 0) {
        return 1;
    }

    special = NULL;
    backwards = 0;
    snap = 0;
    restart = 0;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, gizspecial_gizmotype_id, value + 5);
            if (gizmo != NULL) {
                special = (GIZSPECIAL_PARTIAL_PC *)(gizmo->object);
            }
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral757ea8) == 0) {
            backwards = 1;
        } else if (NuStrICmp(params[index], (char *)GizmoSnapOptionNamePc) == 0) {
            snap = 1;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral757eb4) == 0) {
            restart = 1;
        }
    }

    if (special == NULL) {
        return 1;
    }
    if (backwards != 0) {
        if (snap != 0) {
            GameAnimSet_JumpToStart(special->anim_set);
        } else {
            if (restart != 0) {
                GameAnimSet_JumpToEnd(special->anim_set);
            }
            GameAnimSet_Play(special->anim_set, -1.0f, 1);
        }
    } else if (snap != 0) {
        GameAnimSet_JumpToEnd(special->anim_set);
    } else {
        if (restart != 0) {
            GameAnimSet_JumpToStart(special->anim_set);
        }
        GameAnimSet_Play(special->anim_set, 1.0f, 1);
    }
    return 1;
}

i32 Action_SetObstacleToEnd(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)param_6;
    if (param_5 != 0 && param_4 != 0) {
        i32 index;
        GIZOBSTACLE *obstacle = NULL;
        for (index = 0; index < param_4; ++index) {
            char *value = NuStrIStr(params[index], "name=");
            if (value != NULL) {
                obstacle = GizObstacle_FindByName(WORLD->giz_obstacle_sys, value + 5);
            }
        }
        if (obstacle != NULL) {
            GizObstacle_JumpToEnd(obstacle);
        }
    }
    return 1;
}

i32 __cdecl Action_SetLastAttacker(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *victim;
    GAMEOBJECT *attacker;
    char *value;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    victim = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    attacker = NULL;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral757a20);
        if (value != NULL) {
            victim = GetNamedGameObject(sys, value + NuStrLen((char *)AITransferLiteral757a20));
            continue;
        }
        if (NuStrICmp(params[index], (char *)AITransferLiteral7583d0) == 0) {
            if (packet != NULL && packet->opponent != NULL) {
                attacker = (GAMEOBJECT *)((APIOBJECT_PC *)packet->opponent)->owner;
            }
            continue;
        }
        value = NuStrIStr(params[index], (char *)AITransferLiteral7583c4);
        if (value != NULL) {
            attacker = GetNamedGameObject(sys, value + 9);
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral7583b4) == 0) {
            attacker = (GAMEOBJECT *)((APIOBJECT_PC *)sys->player_1)->owner;
        }
    }
    if (victim != NULL && attacker != NULL) {
        victim->last_attacker = attacker;
    }
    return 1;
}

i32 __cdecl Action_SetUseOneAtOnce(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    i32 enabled;
    GAMEOBJECT *object;
    i32 index;
    char *name;
    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        enabled = 1;
        object = packet != NULL ? packet->owner->objptr : NULL;

        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
            if (name != NULL) {
                name += 10;
                if (NuStrICmp(name, (char *)AITransferLiteral757f7c) == 0 && packet != NULL)
                    object = packet->owner->objptr;
                else
                    object = GetNamedGameObject(WORLD->ai_sys, name);
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                enabled = 0;
        }
        if (object != NULL) {
            object->flags_ef4 = (object->flags_ef4 & ~0x2000u) | ((enabled & 1) << 13);
        }
    }
    return 1;
}

i32 __cdecl Action_SetAO_MaxAttackers(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2,
                                      char **params, i32 count, i32 first_time, f32 unused6) {
    i32 index;
    char *value;
    if (first_time != 0) {
        if (count == 0)
            return 1;
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIExpressionMaxName);
            if (value != NULL)
                oneAtOnce_SetNumAttackers((i32)AIParamToFloat(processor, value + 4));
        }
    }
    return 1;
}

i32 __cdecl Action_SetAO_AttackersPerRow(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2,
                                         char **params, i32 count, i32 first_time, f32 unused6) {
    i32 index;
    char *value;
    if (first_time != 0) {
        if (count == 0)
            return 1;
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionNumName);
            if (value != NULL)
                oneAtOnce_SetAttackersPerRow((i32)AIParamToFloat(processor, value + 4));
        }
    }
    return 1;
}

i32 __cdecl Action_SetAO_RowDist(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2, char **params,
                                 i32 count, i32 first_time, f32 unused6) {
    i32 index;
    char *value;
    if (first_time != 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionDistName);
            if (value != NULL)
                oneAtOnce_SetDistPerRow(AIParamToFloat(processor, value + 5));
        }
    }
    return 1;
}

i32 __cdecl Action_SetAO_InitRowDist(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2,
                                     char **params, i32 count, i32 first_time, f32 unused6) {
    i32 index;
    char *value;
    if (first_time != 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionDistName);
            if (value != NULL)
                oneAtOnce_SetInitDistPerRow(AIParamToFloat(processor, value + 5));
        }
    }
    return 1;
}

static i32 Action_SetTechnoComplete(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **params,
                                    i32 param_count, i32 first_time, f32 unused6) {
    GAMEOBJECT *object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (first_time) {
        i32 index;
        TECHNO_s *techno = NULL;
        i32 complete = 1;
        for (index = 0; index < param_count; ++index) {
            char *value = NuStrIStr(params[index], "techno");
            if (value != NULL) {
                char *name = value + NuStrLen("techno") + 1;
                i32 type = GizmoGetTypeIDByName(WORLD->gizmo_sys, "Techno");
                GIZMO_PC *gizmo = GizmoFindByName(WORLD->gizmo_sys, type, name);
                if (gizmo != NULL && gizmo->object != NULL)
                    techno = ((TECHNO_s *)(gizmo->object));
            } else if (NuStrICmp(params[index], "FALSE") == 0) {
                complete = 0;
            } else if ((value = NuStrIStr(params[index], "controlling")) != NULL) {
                object = GetNamedGameObject(WORLD->ai_sys, value + NuStrLen("controlling") + 1);
            }
        }
        if (object != NULL)
            techno = Technos_FindControllingTechno(object);
        if (techno != NULL)
            techno->complete = complete;
    }
    return 1;
}

i32 __cdecl Action_NoIdleSpeed(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 enabled = 1;
    char *name;

    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (name != NULL) {
                {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, name + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                }
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
        if (object != NULL) {
            ((AI_ACTION_FLAGS_EF4_PC *)&object->flags_ef4)->bits.no_idle_speed = enabled;
        }
    }
    return 1;
}

i32 __cdecl Action_CatchUpForbidden(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 forbidden;
    char *parameter;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        forbidden = 1;
        if (param_4 != 0) {
            for (index = 0; index < param_4; ++index) {
                parameter = NuStrIStr(params[index], (char *)AIGameCharacterName);
                if (parameter != NULL) {
                    {
                        lookup =
                            GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, parameter + 10) : NULL;
                        object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                    }
                } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                    forbidden = 0;
                }
            }
        }
        if (object != NULL) {
            object->portal_flags = (u32)((object->portal_flags & ~0x2000000u) | ((forbidden & 1) << 25));
        }
    }
    return 1;
}

i32 __cdecl Action_KeepWeaponOut(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 keep_out;
    char *value;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    keep_out = 1;
    for (index = 0; index < param_4; ++index) {
        if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            keep_out = 0;
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL
                             ? GetNamedGameObjectCallback(sys, value + NuStrLen((char *)AIGameCharacterName))
                             : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        }
    }
    if (object != NULL) {
        object->portal_flags = (object->portal_flags & (u32)(~0x10u)) | (u32)(keep_out ? 0x10u : 0);
    }
    return 1;
}

i32 __cdecl Action_NotWithParty(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 enabled;
    char *name;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        enabled = 1;
        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (name != NULL) {
                {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, name + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                }
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
        if (object != NULL) {
            object->flags_ef0 = (u32)((object->flags_ef0 & ~0x1000000u) | (enabled ? 0x1000000u : 0u));
        }
    }
    return 1;
}

i32 __cdecl Action_CanAttack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 enabled;
    char *name;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        enabled = 1;
        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (name != NULL) {
                {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, name + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                }
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
        if (object != NULL) {
            object->portal_flags = (u32)((object->portal_flags & ~0x200u) | (enabled ? 0x200u : 0u));
        }
    }
    return 1;
}

i32 __cdecl Action_SetDontMove(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 enabled;
    char *name;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        enabled = 1;
        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
            if (name != NULL) {
                {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, name + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                }
            } else if (NuStrICmp((char *)AIActionFalseName, params[0]) == 0) {
                // This action's FALSE toggle uses the first parameter, not the current one.
                enabled = 0;
            }
        }
        if (object != NULL) {
            object->flags_ef0 = (u32)((object->flags_ef0 & ~0x10u) | (enabled ? 0x10u : 0u));
        }
    }
    return 1;
}

i32 __cdecl Action_DontSetStoppedFlag(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 enabled;
    char *value;

    if (first_time) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        enabled = 1;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
            if (value != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else if (NuStrICmp((char *)AIActionFalseName, params[0]) == 0)
                enabled = 0;
        }
        if (object != NULL)
            object->flags_ef0 = (object->flags_ef0 & ~0x20u) | ((enabled & 1) << 5);
    }
    return 1;
}

i32 __cdecl Action_Respawnable(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags = (object->portal_flags & ~0x200000u) | 0x100000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIPathOriginName) == 0) {
                object->portal_flags |= 0x200000u;
            } else if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= ~0x100000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_UseTimeBasedUpdate(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    i32 disabled;
    i32 enabled;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
        disabled = 1;
        if (first_time != 0 && param_count > 0) {
            enabled = 0;
            for (index = 0; index != param_count; ++index) {
                if (NuStrICmp(params[index], (char *)AITransferLiteral757ebc) == 0)
                    enabled = 1;
                else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                    enabled = 0;
            }
            disabled = (enabled ^ 1) & 1;
        }
        object->flags_ef4 = (object->flags_ef4 & ~0x20) | (disabled << 5);
    }
    return 1;
}

i32 __cdecl Action_CanOpenDoors(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->flags_1044 |= 1;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757ebc) == 0) {
                object->flags_1044 |= 1u;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_1044 &= ~1u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanShootOffScreen(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time != 0) {
        object->flags_1044 |= 4u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757ebc) == 0) {
                object->flags_1044 |= 4u;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_1044 &= ~4u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetCanTakeOver(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (param_5 != 0 && packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        object = packet->owner->objptr;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp((char *)AITransferLiteral758430, params[index]) == 0) {
                object->flags_ef0 |= 0x40000000u;
            } else if (NuStrIStr(params[index], (char *)AIScriptFalse) != NULL) {
                object->flags_ef0 &= ~0x40000000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_ImmuneToKillTerrain(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                       i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->portal_flags |= 0x40000u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0)
                object->portal_flags &= ~0x40000u;
        }
    }
    return 1;
}

i32 __cdecl Action_ImmuneToBolts(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 0x80000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= (u32)(~0x80000u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetBoss(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 0x8000000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= (u32)(~0x8000000u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_UpdateSockPos(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 0x4000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= ~0x4000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_ApplyGravity(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags &= (u32)(~0x80000000u);
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->portal_flags |= 0x80000000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CannotBeForcedBack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->flags_ef0 |= 0x10000000u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef0 &= ~0x10000000u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanTurn(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->flags_ef0 |= 0x20000000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef0 &= (u32)(~0x20000000u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_PrefersPlayers(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_5;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->portal_flags |= 0x40000000u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->portal_flags &= (u32)(~0x40000000u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanShootObstructions(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->portal_flags |= 0x10000000u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->portal_flags &= ~0x10000000u;
        }
    }
    return 1;
}

i32 __cdecl Action_UseBigJumpToJump(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_5;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef4 |= 0x40000000u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef4 &= ~0x40000000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_IgnoreTurnAroundSpline(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef4 |= 0x80000000u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef4 &= ~0x80000000u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanPullLevers(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef0 |= 0x800000u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef0 &= ~0x800000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_AlwaysBackFlip(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_5;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->portal_flags |= 0x100u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->portal_flags &= (u32)(~0x100u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_DeflectPlayersPart(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL || !first_time)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef0 |= 0x100u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef0 &= ~0x100u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanHelpWithTriggers(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                       i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->flags_ef4 |= 0x40000u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef4 &= ~0x40000u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanCollideWithObjects(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                         char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->flags_ef4 &= ~0x80000u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757ebc) == 0)
                object->flags_ef4 |= 0x80000u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanDefend(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 2u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0)
                object->portal_flags &= ~2u;
        }
    }
    return 1;
}

i32 __cdecl Action_DontAimAt(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->flags_ef4 |= 1;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->flags_ef4 &= (u32)(~1u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanUseWeapon(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 8;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= (u32)(~8u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_IgnoreShoveSystem(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->flags_ef0 |= 1;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef0 &= ~1;
        }
    }
    return 1;
}

i32 __cdecl Action_CannotBeSeen(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->flags_ef0 |= 2;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef0 &= (u32)(~2u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetBoltsDontGetDeflectedBack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                                char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef0 |= 8u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef0 &= ~8u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanMoveWhenDeactivated(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef8_word |= 1;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef8_word &= ~1;
        }
    }
    return 1;
}

i32 __cdecl Action_DontAttack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef8_word |= 2u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef8_word &= ~2u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanHitForceObjects(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->portal_flags |= 0x40;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->portal_flags &= ~0x40;
        }
    }
    return 1;
}

i32 __cdecl Action_PlayerSpeederHack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef8_word |= 8u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef8_word &= (u32)(~8u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_UseWeapon(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    object = packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL ? packet->owner->objptr : NULL;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (object != NULL) {
        object->portal_flags |= 0x20000000u;
    }
    return 1;
}

extern const char AITransferLiteral75782c[5];
extern const char AITransferLiteral7583e4[12];
extern const char AITransferLiteral7583f0[8];
extern const char AIActionFalseName[6];
extern const char AIScriptActionNameArgPrefix[6];
extern const char AreaConfigKeyword_character[10];
extern const char AITransferLiteral757a38[12];
extern const char AIGameCharacterName[11];
extern const char AIScriptFalse[6];
extern const char AITransferLiteral757a70[11];
extern const char GizmoDefaultOutputName[8];
typedef struct AITURRET_PARTIAL_PC {
    u8 unknown_00[0xe8];
    GAMEOBJECT *controller;
} AITURRET_PARTIAL_PC;
typedef struct AIOBSTACLE_PARTIAL_PC {
    u8 unknown_00[0x9c];
    u32 runtime_flags;
} AIOBSTACLE_PARTIAL_PC;
STATIC_ASSERT(offsetof(AITURRET_PARTIAL_PC, controller) == 0xe8);
STATIC_ASSERT(offsetof(AIOBSTACLE_PARTIAL_PC, runtime_flags) == 0x9c);
extern i32 turret_gizmotype_id;
extern i32 obstacle_gizmotype_id;
extern i32 blowup_gizmotype_id;
void __cdecl ReleaseTakeOver(GAMEOBJECT *, i32);
void __cdecl PartKill_ForceThrow(PART_PC *, i32);
i32 __cdecl Action_SetLevelPath(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    char *path_name;
    char *value;
    GAMEOBJECT *object;
    AIPACKET_PC *object_packet;
    (void)processor;
    (void)packet;
    (void)param_6;
    if (param_5 == 0 || sys->path_sys == NULL) {
        return 1;
    }

    path_name = NULL;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)((char *)AITransferLiteral75782c));
        if (value != NULL) {
            path_name = value + 5;
        }
    }

    if (AISysSetLevelPath(sys, path_name) == 0) {
        return 1;
    }

    for (index = 0; index < 8; ++index) {
        object = Player[index];
        if (object == NULL || (object->flags_1fc_word & (1u | 0x1000u)) != (1u | 0x1000u)) {
            continue;
        }
        object_packet = &object->ai_packet;
        AISysCharacterSetPath(object_packet, sys->path_sys->active_path);
        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, object_packet, 0xff, (i8)(object->field_281));
    }
    return 1;
}

i32 Action_SetHint(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params, i32 param_4,
                   i32 param_5, f32 param_6) {
    i32 hint_id = -1;
    i32 ignore_done = 0;
    i32 index;
    char *value;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionIDName);
            if (value != NULL)
                hint_id = (i32)AIParamToFloat(processor, value + 3);
            else if (NuStrIStr(params[index], (char *)AIActionIgnoreDoneFlagNamePc) != NULL)
                ignore_done = 1;
        }
        Hint_SetHintFromId(hint_id, 0, ignore_done);
    }
    return 1;
}

i32 Action_SetHintComplete(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    i32 hint_id = -1;
    i32 index;
    char *value;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionIDName);
            if (value != NULL)
                hint_id = (i32)AIParamToFloat(processor, value + 3);
        }
        Hint_SetComplete(hint_id);
    }
    return 1;
}

i32 Action_CancelHint(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params, i32 param_4,
                      i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    Hint_CancelCurrent();
    return 1;
}

i32 __cdecl Action_LinkTurretToController(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *lookup;
    AITURRET_PARTIAL_PC *turret;
    GAMEOBJECT *controller;
    char *value;
    GIZMO_PC *gizmo;

    if (first_time == 0) {
        return 1;
    }
    turret = NULL;
    controller = NULL;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral7583f0);
        if (value != NULL) {
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, turret_gizmotype_id, value + 7);
            if (gizmo != NULL && gizmo->object != NULL) {
                turret = (AITURRET_PARTIAL_PC *)(gizmo->object);
            }
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral7583e4)) != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 11) : NULL;
                controller = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        }
    }
    if (turret != NULL) {
        turret->controller = controller;
    }
    return 1;
}

i32 __cdecl Action_SetHoverPhase(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)param_6;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (object == NULL || (object->character_data->game_character)->hover_fall_mode != 2) {
        return 1;
    }

    if (param_5 != 0) {
        processor->action_data_1 = 1;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                processor->action_data_1 = 0;
            }
        }
    }

    if (processor->action_data_1 != 0) {
        if (object->jump_variant_state == 1) {
            return 1;
        }
    } else if (object->jump_variant_state != 1) {
        return 1;
    }
    object->pad->buttons_pressed |= GAMEPAD_JUMP;
    return 0;
}

i32 __cdecl Action_WalkBackwards(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *opponent;
    GAMEOBJECT *owner;
    (void)sys;
    (void)param_6;
    if (packet == NULL) {
        return 1;
    }
    if (param_5 != 0) {
        packet->movement_instruction_parameter = 1.0f;
        for (index = 0; index < param_4; ++index) {
            if (AIActionParseSpeedFn == NULL || AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) == 0) {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
    }

    opponent = (APIOBJECT_PC *)packet->opponent;
    if (opponent != NULL && opponent->ai != NULL) {
        owner = packet->owner->objptr;
        AIMoveInstruction(packet, &opponent->ai->last_path_position, opponent->ai->mover_height,
                          &opponent->ai->path_info, 2, packet->movement_instruction_parameter);
        owner->flags_ef0 |= 0x8000u;
        owner->ai_packet.goal_speed_mode = 1;
    }
    return 0;
}

void __cdecl FindForcePushTarget(GAMEOBJECT *object, i32 activate, i32 target_filter);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_target) == 0x790);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_d20) == 0xd20);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEPAD_PC, runtime_flags) == 0x5a);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_data_1) == 0x68);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_data_3) == 0x6c);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_data_4) == 0x70);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_timer) == 0x9c);

i32 __cdecl Action_ForcePush(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *action_object;
    i32 index;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }
    object = packet->owner->objptr;
    if (first_time != 0) {
        processor->action_data_3 = NULL;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], "target=opponent") == 0) {
                processor->action_data_3 = packet->opponent;
                if (processor->action_data_3 == NULL) {
                    return 1;
                }
            } else {
                char *value = NuStrIStr(params[index], "target=");
                if (value != NULL) {
                    processor->action_data_3 = GetNamedGameObject(system, value + 7);
                    if (processor->action_data_3 == NULL) {
                        return 1;
                    }
                } else if (NuStrICmp(params[index], "forcepush_highlight_only") == 0) {
                    processor->action_data_1 = 1;
                }
            }
        }
        if (object->character_context == 0x1b && processor->action_data_1 == 0 && object->force_target != NULL &&
            processor->action_data_3 == NULL) {
            object->pad->runtime_flags |= 0x10;
            object->field_d20 = (u32)object->force_target;
            object->flags_e20 = (object->flags_e20 & ~0x400u) | ((processor->action_data_1 << 10) & 0x400u);
            object->flags_e20 &= ~0x20000u;
            object->character_context = -1;
            FindForcePushTarget(object, 0, 0);
        }
    }
    if (CharCategory_IsCategory(object, 0) == 0) {
        if (FreePlay != 0) {
            f32 timer = processor->action_timer - elapsed;
            processor->action_timer = timer;
            if (timer < 0.0f) {
                processor->action_timer = 0.5f;
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
        }
        return 0;
    }
    object->pad->runtime_flags |= 0x10;
    object->field_d20 = (u32)processor->action_data_3;
    object->flags_e20 = (object->flags_e20 & ~0x400u) | ((processor->action_data_1 << 10) & 0x400u);
    memcpy(&action_object, &processor->action_data_4, sizeof(action_object));
    if (action_object != NULL) {
        return action_object != object->force_target;
    }
    if (object->character_context == 0x1b && object->force_target != NULL) {
        memcpy(&processor->action_data_4, &object->force_target, sizeof(object->force_target));
    }
    return 0;
}

i32 __cdecl Action_ForceLightning(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    struct gameobject_s *object;
    NUVEC target;
    NUVEC direction;
    NUVEC origins[2];
    i32 origin_index;
    (void)param_6;
    object = NULL;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
    }

    if (param_5 != 0) {
        if (param_4 > 0) {
            i32 index;
            for (index = 0; index < param_4; ++index) {
                char *value = NuStrIStr(params[index], "locator=");
                if (value != NULL) {
                    AILOCATOR_PC *locator = AIPathFindLocator(sys, value + 8);
                    if (locator != NULL) {
                        processor->action_pos = locator->position;
                        processor->action_data_1 = 1;
                    }
                } else {
                    value = NuStrIStr(params[index], "randxz=");
                    if (value != NULL) {
                        processor->action_data_4 = AIParamToFloat(processor, value + 7);
                    }
                }
            }
        }
    }

    if (object == NULL || processor->action_data_1 == 0) {
        return 0;
    }

    ForceLightning_Origin(object, &origins[0], &origins[1]);

    target = processor->action_pos;
    if (processor->action_data_4 > 0.0f) {
        f64 range = processor->action_data_4;
        target.x += range - NuRandFloat() * (range + range);
        range = processor->action_data_4;
        target.z += range - NuRandFloat() * (range + range);
    }

    for (origin_index = 0; origin_index < 2; ++origin_index) {
        NUVEC *origin = &origins[origin_index];
        if (origin->y != 1000000000.0) {
            f32 distance = NuVecDist(&target, origin, &direction);
            NuLgtLaser(lightning_type, lightning_sizew[0], lightning_sizel[0], lightning_sizewab[0], origin, &direction,
                       lightning_col[0], lightning_endw[0], distance);
            PlaySfx("ForceLightningLp", &processor->action_pos);
        }
    }
    packet->movement_look_target = &processor->action_pos;
    return 0;
}

i32 __cdecl Action_AlwaysTriggerObstacle(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                         char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    AIOBSTACLE_PARTIAL_PC *obstacle;
    i32 enabled;
    char *value;
    GIZMO_PC *gizmo;
    if (first_time == 0 || param_count == 0) {
        return 1;
    }
    obstacle = NULL;
    enabled = 1;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, value + 5);
            if (gizmo != NULL) {
                obstacle = (AIOBSTACLE_PARTIAL_PC *)(gizmo->object);
            }
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            enabled = 0;
        }
    }
    if (obstacle != NULL) {
        obstacle->runtime_flags = (u32)((obstacle->runtime_flags & ~4u) | ((enabled & 1) << 2));
    }
    return 1;
}

i32 __cdecl Action_CanTriggerObstacle(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    AIOBSTACLE_PARTIAL_PC *obstacle;
    i32 blocked;
    char *value;
    GIZMO_PC *gizmo;
    if (first_time == 0 || param_count == 0) {
        return 1;
    }
    obstacle = NULL;
    blocked = 0;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, value + 5);
            if (gizmo != NULL) {
                obstacle = (AIOBSTACLE_PARTIAL_PC *)(gizmo->object);
            }
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            blocked = 1;
        }
    }
    if (obstacle != NULL) {
        obstacle->runtime_flags = (u32)((obstacle->runtime_flags & ~8u) | ((blocked & 1) << 3));
    }
    return 1;
}

i32 __cdecl Action_SetLastSafePathPos(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *object;
    char *value;
    if (first_time == 0) {
        return 1;
    }
    object = packet != NULL && packet->owner != NULL ? (APIOBJECT_PC *)packet->owner : NULL;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
        if (value != NULL) {
            if (GetNamedGameObjectCallback != NULL)
                object = GetNamedGameObjectCallback(system, value + 10);
        } else {
            NuStrICmp((char *)AIActionFalseName, params[index]);
        }
    }
    if (object != NULL) {
        object->respawn_position = object->position;
        object->last_safe_position = object->position;
    }
    return 1;
}

i32 __cdecl Action_PressSpecialButton(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    i32 result;
    if (packet == NULL || packet->owner == NULL)
        return 1;
    object = packet->owner->objptr;
    if (object == NULL)
        return 1;
    if (first_time) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757a38) == 0)
                processor->action_data_1 = 1;
        }
    }
    object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
    result = 1;
    if (processor->action_data_1 != 0) {
        object->pad->buttons_held |= GAMEPAD_SPECIAL;
        result = 0;
    }
    return result;
}

i32 __cdecl Action_ReleaseTakeOver(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    char *value;

    if (param_5 != 0) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                }
            }
        }
        if (object != NULL && object->takeover_source != NULL) {
            ReleaseTakeOver(object, 0);
        }
    }
    return 1;
}

i32 __cdecl Action_TriggerBlowUp(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GIZMOBLOWUP_PC *blowup;
    char *value;
    GIZMO_PC *gizmo;
    if (param_5 != 0 && param_4 > 0) {
        blowup = NULL;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, value + 5);
                if (gizmo != NULL) {
                    blowup = (GIZMOBLOWUP_PC *)(gizmo->object);
                }
            }
        }
        if (blowup != NULL && (blowup->status_flags & 1) == 0) {
            GizmoBlowupBlowup(blowup, 1, -1, 1, NULL, 1);
        }
    }
    return 1;
}

i32 __cdecl Action_DontRaycastLOS(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    (void)sys;
    (void)processor;
    (void)packet;
    (void)param_6;
    if (param_5 != 0 && WORLD->object_sys != NULL) {

        WORLD->object_sys->flags_word |= 1u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                WORLD->object_sys->flags_word &= ~1u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetLocatorSet(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    char *value;
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                processor->locator_set = AIPathFindLocatorSet(WORLD->ai_sys, value + 5);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetControlSystem(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL && param_5 != 0) {
        object = packet->owner->objptr;
        object->ai_packet.movement_stopped = 0;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757a70) == 0) {
                object->ai_packet.movement_stopped = 1;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetWalkSpeed(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 num_params, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->walk_speed_override = 1000000000.0f;
        if (num_params != 0 && NuStrICmp(params[0], (char *)GizmoDefaultOutputName) != 0)
            object->walk_speed_override = AIParamToFloat(processor, params[0]);
    }
    return 1;
}

i32 __cdecl Action_PartyCanBeUnderCover(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    if (first_time) {
        party_cant_be_under_cover = 0;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                party_cant_be_under_cover = 1;
        }
    }
    return 1;
}

i32 __cdecl Action_DisableNarrowSocks(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 disabled;
    disabled = 1;
    if (param_count != 0 && params != NULL && params[0] != NULL && NuStrICmp((char *)AIActionFalseName, params[0]) == 0)
        disabled = 0;
    disable_narrow_socks = disabled;
    return 1;
}

i32 __cdecl Action_PressActionButton(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
        if (object != NULL) {
            object->pad->buttons_pressed |= GAMEPAD_ACTION;
            object->portal_flags |= 0x400u;
            object->portal_flags |= 0x20;
        }
    }
    return 1;
}

i32 __cdecl Action_ClearTakeOverTarget(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                       i32 param_count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    if (first_time) {
        object = packet->owner->objptr;
        if (object != NULL) {
            target = object->takeover_target;
            if (target != NULL) {
                object->takeover_target = NULL;
                target->takeover_target = NULL;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_PressTagButton(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    object = packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL ? packet->owner->objptr : NULL;
    if (object != NULL) {
        object->pad->buttons_pressed |= GAMEPAD_TAG;
    }
    return 1;
}

i32 __cdecl Action_PressJumpButton(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    object = packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL ? packet->owner->objptr : NULL;
    if (object != NULL) {
        object->pad->buttons_pressed |= GAMEPAD_JUMP;
    }
    return 1;
}

i32 __cdecl Action_Explode(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {

    (void)sys;
    (void)processor;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (packet != NULL && packet->owner != NULL) {
        Detonate(&packet->owner->position, 0);
    }
    return 1;
}

i32 __cdecl Action_BreakFormation(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {

    if (packet != NULL && packet->group != NULL) {
        packet->group->flags &= ~8u;
    }
    return 1;
}

i32 __cdecl Action_FormationMove(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {

    (void)sys;
    (void)processor;
    if (packet != NULL && packet->group != NULL) {
        packet->group->flags |= 8u;
    }
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 1;
}

extern const char AIGameCharacterName[11];
extern const char AIGameSetName[5];
extern const char AILocatorIndexedFormat[6];
extern const char AIGameNearestName[8];
extern const char AIScriptActionNameArgPrefix[6];
extern const char AIActionPersonalName[9];
extern const char AITransferLiteral7577dc[8];
extern const char AIActionIndexedName[8];
extern const char AILocatorPersonalFormat[6];
extern const char ActionSnapDontCheckTerrainName[19];
extern const char AITransferLiteral75782c[5];
extern const char AITransferLiteral757824[7];
extern const char AreaConfigKeyword_character[10];
extern const char AITransferLiteral75781c[8];
extern const char AITransferLiteral757834[13];
extern const char AITransferLiteral757884[12];
extern const char AITransferLiteral757a68[6];
extern const char AITransferLiteral757c10[13];
extern const char AITransferLiteral757c20[12];
extern const char GizmoDefaultOutputName[8];
extern const char AIActionFalseName[6];
extern const char AIPathLocatorName[9];
extern const char AITransferLiteral758194[11];
extern const char AITransferLiteral758188[11];
extern const char AITransferLiteral7581a0[8];
extern const char AITransferLiteral758174[19];
extern const char AITransferLiteral7581d4[10];
extern const char AITransferLiteral7581ec[9];
extern const char AITransferLiteral7581e0[9];
extern const char AITransferLiteral757cd0[6];
i32 __cdecl Action_DeActivate(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 creature_set;
    char *value;
    i32 parsed_set;
    GAMEOBJECT *candidate;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    creature_set = 0;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIGameSetName);
        if (value != NULL) {
            parsed_set = (i32)(AIParamToFloat(processor, value + 4));
            creature_set = (u32)(parsed_set) < 17 ? parsed_set : 0;
        }
    }

    if (creature_set != 0) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc_word & (1u | 0x1000u)) == (1u | 0x1000u) &&
                candidate->ai_packet.creature_set == creature_set) {
                DeactivateGameObject(candidate);
            }
        }
    } else if (object != NULL) {
        DeactivateGameObject(object);
    }
    return 1;
}

i32 __cdecl Action_GameSetLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *target;
    char *locator_name;
    i32 personal;
    i32 indexed;
    i32 nearest;
    i32 random_count;
    char *value;
    NUVEC difference;
    AILOCATOR_PC *locator;
    f32 best_distance;
    char numbered_name[64];
    AILOCATOR_PC *candidate;
    f32 distance;
    char resolved_name[64];
    GAMEOBJECT *object;
    (void)param_6;
    if (param_5 == 0 || param_4 == 0) {
        return 1;
    }

    target = packet != NULL ? (APIOBJECT_PC *)packet->owner : NULL;
    locator_name = NULL;
    personal = 0;
    indexed = 0;
    nearest = 0;
    random_count = 0;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            locator_name = value + 5;
        } else if (NuStrICmp(params[index], (char *)AIActionPersonalName) == 0) {
            personal = 1;
        } else if (NuStrICmp(params[index], (char *)AIActionIndexedName) == 0) {
            indexed = 1;
        } else if (NuStrICmp(params[index], (char *)AIGameNearestName) == 0) {
            nearest = 1;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral7577dc)) != NULL) {
            random_count = (i32)(AIParamToFloat(processor, value + 7));
        } else if ((value = NuStrIStr(params[index], (char *)AIGameCharacterName)) != NULL &&
                   GetNamedGameObjectCallback != NULL) {
            target = GetNamedGameObjectCallback(sys, value + 10);
        }
    }

    locator = NULL;
    if (locator_name != NULL && nearest) {
        best_distance = 1.0e9f;

        for (index = 0;; ++index) {
            sprintf(numbered_name, (char *)AILocatorIndexedFormat, locator_name, index);
            candidate = AIPathFindLocator(sys, numbered_name);
            if (candidate == NULL) {
                break;
            }
            distance = NuVecDistSqr(&packet->terrain_origin, &candidate->position, &difference);
            if (distance < best_distance) {
                best_distance = distance;
                locator = candidate;
            }
        }
    } else if (locator_name != NULL) {

        object = packet->owner->objptr;
        if (indexed && ((APIOBJECT_PC *)packet->owner)->locator_name_index != -1) {
            sprintf(resolved_name, (char *)AILocatorIndexedFormat, locator_name,
                    ((APIOBJECT_PC *)packet->owner)->locator_name_index);
        } else if (personal && ((APIOBJECT_PC *)packet->owner)->character_data != NULL) {
            sprintf(resolved_name, (char *)AILocatorPersonalFormat, locator_name,
                    ((APIOBJECT_PC *)packet->owner)->character_data->name);
        } else if (random_count != 0) {
            sprintf(resolved_name, (char *)AILocatorIndexedFormat, locator_name, NuRand(0) % random_count);
        } else {
            sprintf(resolved_name, locator_name);
        }
        locator = AIPathFindLocator(sys, resolved_name);
    }
    if (target != NULL) {
        target->ai->locator = locator;
    }
    return 1;
}

i32 __cdecl Action_SnapToLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    char *name;
    i32 personal;
    i32 indexed;
    i32 random_count;
    i32 check_terrain;
    char *value;
    i32 player;
    AILOCATOR_PC *locator;
    char locator_name[64];
    NUVEC position;

    if (first_time != 0) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        name = NULL;
        personal = 0;
        indexed = 0;
        random_count = 0;
        check_terrain = 1;
        for (i = 0; i < param_count; ++i) {

            if ((value = NuStrIStr(params[i], (char *)AITransferLiteral75782c)) != NULL)
                name = value + 5;
            else if (NuStrIStr(params[i], (char *)AIActionPersonalName) != NULL)
                personal = 1;
            else if (NuStrIStr(params[i], (char *)AIActionIndexedName) != NULL)
                indexed = 1;
            else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757824)) != NULL)
                random_count = (i32)AIParamToFloat(processor, value + 9);
            else if ((value = NuStrIStr(params[i], (char *)AreaConfigKeyword_character)) != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral75781c)) != NULL) {
                player = (i32)AIParamToFloat(processor, value + 7) - 1;
                if ((u32)player <= 1)
                    object = Player[player];
            } else if (NuStrIStr(params[i], (char *)ActionSnapDontCheckTerrainName) != NULL)
                check_terrain = 0;
        }
        if (object == NULL && packet != NULL && packet->owner != NULL)
            object = packet->owner->objptr;
        if (object != NULL) {
            locator = object->ai_packet.locator;
            if (name != NULL) {

                if (indexed && (i8)object->field_280 != -1)
                    sprintf(locator_name, (char *)AILocatorIndexedFormat, name, (i8)object->field_280);
                else if (personal && object->character_data != NULL)
                    sprintf(locator_name, (char *)AILocatorPersonalFormat, name, object->character_data->name);
                else if (random_count != 0)
                    sprintf(locator_name, (char *)AILocatorIndexedFormat, name, NuRand(NULL) % random_count);
                else
                    sprintf(locator_name, name);
                locator = AIPathFindLocator(sys, locator_name);
            }
            if (locator != NULL) {
                position = locator->position;
                object->position = position;
                object->field_0x276 = locator->rotation;
                object->facing_angle = locator->rotation;
                object->movement_facing_angle = locator->rotation;
                object->ai_packet.path_info = locator->path_info;
                object->initial_position = position;
                object->collision_position = position;
                plr_lastpos = position;
                object->start_position = position;
                object->respawn_position = position;
                object->last_safe_position = position;
                object->ai_update_position = position;
                object->velocity = v000;
                InitSurfaceInfo(object);
                if (check_terrain != 0)
                    SetObjOnSurface(object, 0);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_BigJumpToLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 personal;
    i32 indexed;
    i32 random_count;
    char *name;
    f32 jump_factor;
    char *value;
    char locator_name[64];
    AILOCATOR_PC *locator;
    (void)param_6;

    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (object == NULL)
        return 1;
    if (param_5 != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
            object = packet->owner->objptr;
        }
        processor->action_data_3 = processor->locator;

        personal = 0;
        indexed = 0;
        random_count = 0;
        name = NULL;
        jump_factor = 1.0f;

        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)((char *)AITransferLiteral75782c));
            if (value != NULL) {
                name = value + 5;
            } else if (NuStrIStr(params[index], (char *)((char *)AIActionPersonalName)) != NULL) {
                personal = 1;
            } else if (NuStrIStr(params[index], (char *)((char *)AIActionIndexedName)) != NULL) {
                indexed = 1;
            } else if ((value = NuStrIStr(params[index], (char *)((char *)AITransferLiteral757824))) != NULL) {
                random_count = (i32)(AIParamToFloat(processor, value + 9));
            } else if ((value = NuStrIStr(params[index], (char *)((char *)AITransferLiteral757834))) != NULL) {
                jump_factor = AIParamToFloat(processor, value + 12);
                if (jump_factor < 0.0f) {
                    jump_factor = 0.0f;
                }
            }
        }

        if (object == NULL) {
            return 1;
        }

        if (name != NULL) {

            if (indexed != 0 && (i8)(object->field_280) != -1) {
                sprintf(locator_name, (char *)AILocatorIndexedFormat, name, (i8)(object->field_280));
            } else if (personal != 0 && object->character_data != NULL) {
                sprintf(locator_name, (char *)AILocatorPersonalFormat, name, object->character_data->name);
            } else if (random_count != 0) {
                sprintf(locator_name, (char *)AILocatorIndexedFormat, name, NuRand(NULL) % random_count);
            } else {
                sprintf(locator_name, name);
            }
            processor->action_data_3 = AIPathFindLocator(sys, locator_name);
        }

        locator = (AILOCATOR_PC *)(processor->action_data_3);
        if (locator != NULL) {
            StartBigJump(object, &locator->position, 0, jump_factor, 1.0f, 0);
            object->ai_packet.path_info = locator->path_info;
            object->ai_packet.last_path_position = locator->position;
        }
    }
    return object->character_context != 0x1f;
}

i32 __cdecl Action_SetDoomedEscapeLocator(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    char *name;
    i32 personal;
    i32 indexed;
    i32 random_count;
    i32 take_damage;
    char *value;
    char locator_name[64];

    if (first_time) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        if (param_count != 0) {
            name = NULL;
            personal = 0;
            indexed = 0;
            random_count = 0;
            take_damage = 0;
            for (index = 0; index < param_count; ++index) {
                value = NuStrIStr(params[index], (char *)AIGameCharacterName);
                if (value != NULL) {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral75782c)) != NULL)
                    name = value + 5;
                else if (NuStrIStr(params[index], (char *)AIActionPersonalName) != NULL)
                    personal = 1;
                else if (NuStrIStr(params[index], (char *)AIActionIndexedName) != NULL)
                    indexed = 1;
                else if (NuStrIStr(params[index], (char *)AITransferLiteral757884) != NULL)
                    take_damage = 1;
                else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757824)) != NULL)
                    random_count = (i32)(AIParamToFloat(processor, value + 9));
            }
            if (object != NULL) {
                object->flags_ef0 &= ~0x800u;
                object->doomed_escape_locator = NULL;
                if (name != NULL) {

                    if (indexed && (i8)(object->field_280) != -1)
                        sprintf(locator_name, (char *)AILocatorIndexedFormat, name, (i8)(object->field_280));
                    else if (personal && object->character_data != NULL)
                        sprintf(locator_name, (char *)AILocatorPersonalFormat, name, object->character_data->name);
                    else if (random_count != 0)
                        sprintf(locator_name, (char *)AILocatorIndexedFormat, name, NuRand(NULL) % random_count);
                    else
                        sprintf(locator_name, name);
                    object->doomed_escape_locator = AIPathFindLocator(system, locator_name);
                    if (object->doomed_escape_locator != NULL)
                        object->flags_ef0 = (object->flags_ef0 & ~0x800u) | (take_damage << 11);
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_AddToSet(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 creature_set;
    (void)sys;
    (void)param_6;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (param_5 != 0 && object != NULL && param_4 > 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757a68) == 0) {
                object->ai_packet.creature_set = 0;
                continue;
            }
            creature_set = (i32)(AIParamToFloat(processor, params[index]));
            if ((u32)(creature_set - 1) < 16) {
                object->ai_packet.creature_set = (u8)(creature_set);
                ++aicreature_sets_alive[creature_set - 1];
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetHitPoints(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 num_params, i32 first_time, f32 elapsed) {
    i32 i;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 hitpoints;
    i32 set_max;
    char *value;
    GIZAIMESSAGE *message;

    if (!first_time)
        return 1;
    object = NULL;
    if (packet != NULL && packet->owner != NULL)
        object = packet->owner->objptr;
    hitpoints = -1;
    set_max = 1;
    for (i = 0; i < num_params; i++) {
        value = NuStrIStr(params[i], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        } else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757c20)) != NULL) {
            message = CheckGizAIMessage(gizaimessagesys, value + 11, NULL);
            if (message != NULL)
                hitpoints = (i32)message->value;
        } else if (NuStrICmp(params[i], (char *)AITransferLiteral757c10) == 0) {
            set_max = 0;
        } else if (NuStrICmp(params[i], (char *)GizmoDefaultOutputName) != 0) {
            hitpoints = (i32)AIParamToFloat(processor, params[i]);
        }
    }
    if (object != NULL) {
        if (hitpoints == -1)
            hitpoints = ((PLAYERCHARACTERCONFIG_PC *)object->character_data->game_character)->hitpoints;
        if (set_max)
            object->field_107e = (u8)hitpoints;
        object->current_hp = (i8)hitpoints;
    }
    return 1;
}

i32 __cdecl Action_SetShieldHitPoints(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 hit_points;
    char *value;

    if (first_time != 0) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        hit_points = -1;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else
                hit_points = (i32)(AIParamToFloat(processor, params[index]));
        }
        if (object != NULL) {
            if (hit_points == -1)
                hit_points = object->character_data->game_character->shield_hitpoints;
            object->shield_hitpoints = (u8)(hit_points);
        }
    }
    return 1;
}

i32 __cdecl Action_SetScriptParam(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2, char **params,
                                  i32 count, i32 first_time, f32 unused6) {
    f32 amount = 0.0f;
    i32 operation = 0;
    i32 param_index = -1;
    i32 index;
    i32 candidate;
    char *value;
    if (first_time != 0 && count != 0 && processor->script != NULL) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                value += 5;
                for (candidate = 0; candidate < 4; ++candidate) {
                    if (NuStrICmp(processor->script->params[candidate].name, value) == 0) {
                        param_index = candidate;
                        break;
                    }
                }
            } else if ((value = NuStrIStr(params[index], "ix=")) != NULL) {
                param_index = (i32)AIParamToFloat(processor, value + 3);
                if (param_index >= 4)
                    param_index = -1;
            } else if ((value = NuStrIStr(params[index], (char *)AIAnimSpeedValueName)) != NULL)
                amount = AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], "increment=")) != NULL) {
                amount = AIParamToFloat(processor, value + 10);
                operation = 1;
            } else if ((value = NuStrIStr(params[index], "decrement=")) != NULL) {
                amount = AIParamToFloat(processor, value + 10);
                operation = -1;
            }
        }
        if (param_index >= 0) {
            switch (operation) {
                case -1:
                    processor->params[param_index] -= amount;
                    break;
                case 0:
                    processor->params[param_index] = amount;
                    break;
                case 1:
                    processor->params[param_index] = processor->params[param_index] + amount;
                    break;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CopyMessage(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2, char **params,
                               i32 count, i32 first_time, f32 unused6) {
    char *from = NULL;
    char *to = NULL;
    i32 index;
    char *value;
    GIZAIMESSAGE *source;
    GIZAIMESSAGE *destination;
    if (first_time != 0 && gizaimessagesys != NULL && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], "from=");
            if (value != NULL)
                from = value + 5;
            else if ((value = NuStrIStr(params[index], "to=")) != NULL)
                to = value + 3;
        }
        if (from != NULL && to != NULL) {
            source = CheckGizAIMessage(gizaimessagesys, from, NULL);
            destination = CheckGizAIMessage(gizaimessagesys, to, NULL);
            if (source != NULL && destination != NULL)
                destination->value = source->value;
        }
    }
    return 1;
}

i32 __cdecl Action_SetMessage(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2, char **params,
                              i32 count, i32 first_time, f32 unused6) {
    char *name = NULL;
    i32 operation = 0;
    f32 amount = 0.0f;
    i32 index;
    char *value;
    GIZAIMESSAGE *message;
    if (first_time != 0 && gizaimessagesys != NULL && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL)
                name = value + 5;
            else if ((value = NuStrIStr(params[index], (char *)AIAnimSpeedValueName)) != NULL)
                amount = AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], "increment=")) != NULL) {
                amount = AIParamToFloat(processor, value + 10);
                operation = 1;
            } else if ((value = NuStrIStr(params[index], "decrement=")) != NULL) {
                amount = AIParamToFloat(processor, value + 10);
                operation = -1;
            }
        }
        if (name != NULL) {
            message = CheckGizAIMessage(gizaimessagesys, name, NULL);
            if (message != NULL) {
                switch (operation) {
                    case -1:
                        message->value -= amount;
                        break;
                    case 0:
                        message->value = amount;
                        break;
                    case 1:
                        message->value = message->value + amount;
                        break;
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetForceBack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    NUVEC *position;
    f32 radius;
    i32 type;
    i32 enabled;
    char *value;
    AILOCATOR_PC *locator;

    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    position = NULL;
    radius = 1.5f;
    type = 0;
    enabled = 1;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        } else if ((value = NuStrIStr(params[index], (char *)AIPathLocatorName)) != NULL) {
            locator = AIPathFindLocator(sys, value + 8);
            position = locator != NULL ? &locator->position : NULL;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral7581a0)) != NULL) {
            radius = AIParamToFloat(processor, value + 7);
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            enabled = 0;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral758194) == 0) {
            type = 1;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral758188) == 0) {
            type = 2;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral758174) == 0) {
            type = 3;
        }
    }

    if (enabled != 0) {
        if (object != NULL || position != NULL) {
            SetForceBack(object, position, radius, type);
        }
    } else {
        ResetForceBack();
    }
    return 1;
}

i32 __cdecl Action_SpinOnSpot(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    f32 zero;
    u16 angle;
    f32 min_time;
    f32 max_time;
    char *value;
    (void)sys;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }

    object = packet->owner->objptr;
    zero = 0.0f;

    if (param_5 != 0) {
        min_time = 0.0f;
        max_time = 0.0f;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral7581ec);
            if (value != NULL) {
                min_time = AIParamToFloatEx(packet, processor, value + 8);
            } else {
                value = NuStrIStr(params[index], (char *)AITransferLiteral7581e0);
                if (value != NULL) {
                    max_time = AIParamToFloatEx(packet, processor, value + 8);
                } else {
                    value = NuStrIStr(params[index], (char *)AITransferLiteral757cd0);
                    if (value != NULL) {
                        processor->action_timer = AIParamToFloatEx(packet, processor, value + 5);
                    } else {
                        value = NuStrIStr(params[index], (char *)AITransferLiteral7581d4);
                        if (value != NULL) {
                            processor->action_data_4 =
                                (f32)((i32)(AIParamToFloatEx(packet, processor, value + 9) * 182.04444885253906));
                        }
                    }
                }
            }
        }
        if (min_time != max_time) {
            processor->action_timer = NuRandFloat() * (max_time - min_time) + min_time;
        }
        angle = object->field_0x276;
    } else {
        angle = processor->action_data_6;
    }

    angle += (i32)(param_6 * processor->action_data_4);
    processor->action_data_6 = angle;
    processor->action_pos.x = 0.0f;
    processor->action_pos.y = 0.0f;
    processor->action_pos.z = 1.0f;
    NuVecRotateY(&processor->action_pos, &processor->action_pos, angle);
    NuVecAdd(&processor->action_pos, &processor->action_pos, &object->collision_position);
    packet->movement_look_target = &processor->action_pos;

    if (processor->action_timer > zero) {
        processor->action_timer -= param_6;
        if (zero >= processor->action_timer) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

extern const char AITransferLiteral757a8c[7];
extern const char AITransferLiteral757a94[9];
extern const char AITransferLiteral757a84[8];
extern const char ZArgPrefix[3];
extern const char AITransferLiteral75782c[5];
extern const char AIGameCharacterName[11];
extern const char YArgPrefix[3];
extern const char AITransferLiteral757c5c[4];
extern const char XArgPrefix[3];
extern const char AITransferLiteral757c64[4];
extern const char AITransferLiteral757c60[4];
extern const char AITransferLiteral757f28[8];
extern const char AITransferLiteral75781c[8];
extern const char AIGameOpponentName[9];
extern const char AITransferLiteral7583f8[8];
i32 __cdecl Action_SelectRandomSpline(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    NUGSPLINE *splines[32];
    NUGSPLINE *unused_splines[32];
    i32 count;
    i32 unused;
    char *value;
    NUGSPLINE *spline;
    GAMEOBJECT *object;
    i32 unused_count;
    if (first_time) {

        count = 0;
        unused = 0;
        script_spline_selected = NULL;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral757a94);
            if (value != NULL) {
                count += NuSplineFindAllBeg(WORLD->current_gscn, value + 8, &splines[count], 32 - count);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757a84)) != NULL) {
                spline = NuSplineFind(WORLD->current_gscn, value + 7);
                if (spline != NULL && count < 32)
                    splines[count++] = spline;
            } else if (NuStrIStr(params[index], (char *)AITransferLiteral757a8c) != NULL) {
                unused = 1;
            }
        }
        if (count != 0) {
            if (unused) {
                for (index = 0; index < HIGHGAMEOBJECT; ++index) {
                    object = &Obj[index];
                    if ((object->flags_1fc_word & 0x1001) == 0x1001 && object->dead == 0 &&
                        object->movement_spline_position.spline != NULL)
                        object->movement_spline_position.spline->length |= 0x8000;
                }
                unused_count = 0;
                for (index = 0; index < count; ++index) {
                    if (splines[index]->length >= 0)
                        unused_splines[unused_count++] = splines[index];
                }
                for (index = 0; index < HIGHGAMEOBJECT; ++index) {
                    object = &Obj[index];
                    if ((object->flags_1fc_word & 0x1001) == 0x1001 && object->dead == 0 &&
                        object->movement_spline_position.spline != NULL)
                        object->movement_spline_position.spline->length &= 0x7fff;
                }
                if (unused_count != 0)
                    script_spline_selected = unused_splines[qrand() / (0xffff / unused_count + 1)];
            } else {
                script_spline_selected = splines[qrand() / (0xffff / count + 1)];
            }
        }
    }
    return 1;
}

extern const char RaceOpponentDistancePrefix[9];
i32 Action_CreateSplineCreatures(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 num_params, i32 first_time, f32 unused6) {
    u16 angle;
    u16 pitch;
    i32 model_count;
    i32 spline_count;
    i32 use_selected;
    i32 min_group_size;
    i32 max_group_size;
    i32 group_size;
    f32 min_distance;
    f32 max_distance;
    f32 distance;
    i32 relative_to_player;
    i32 creature_set;
    i32 looping;
    struct AILOCATOR_PC *relative_locator;
    struct gameobject_s *rider;
    i32 i;
    struct nugspline_s *spline;
    i32 model;
    i16 models[10];
    struct nugspline_s *splines[32];
    char script_name[64] = "default";
    NUVEC spline_offset = {0.0f, 0.0f, 0.0f};
    if (!first_time || num_params <= 0)
        return 1;

    model_count = 0, spline_count = 0, use_selected = 0;
    min_group_size = -1, max_group_size = -1, group_size = 1;
    min_distance = 1000000000.0f, max_distance = 1000000000.0f, distance = 0.0f;
    relative_to_player = 0, creature_set = 0, looping = 0;
    relative_locator = NULL;
    rider = NULL;
    for (i = 0; i < num_params; i++) {
        char *value = NuStrIStr(params[i], "mingroupsize");
        if (value != NULL)
            min_group_size = (i32)AIParamToFloat(processor, value + 13);
        else if ((value = NuStrIStr(params[i], "maxgroupsize")) != NULL)
            max_group_size = (i32)AIParamToFloat(processor, value + 13);
        else if ((value = NuStrIStr(params[i], "groupsize")) != NULL)
            group_size = (i32)AIParamToFloat(processor, value + 10);
        else if ((value = NuStrIStr(params[i], "type")) != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                u8 type = LevelCharacterTypeIDFn(value + 5);
                if (type != 0xff) {
                    i16 model = LevelCharacterGlobalIDFn(type);
                    if (model != -1 && model_count < 10)
                        models[model_count++] = model;
                }
            }
        } else if ((value = NuStrIStr(params[i], "script")) != NULL)
            NuStrCpy(script_name, value + 7);
        else if ((value = NuStrIStr(params[i], "mindistance")) != NULL)
            min_distance = AIParamToFloat(processor, value + 12);
        else if ((value = NuStrIStr(params[i], "maxdistance")) != NULL)
            max_distance = AIParamToFloat(processor, value + 12);
        else if ((value = NuStrIStr(params[i], (char *)RaceOpponentDistancePrefix)) != NULL)
            distance = AIParamToFloat(processor, value + 9);
        else if (script_spline_selected != NULL && NuStrICmp("use_selected_spline", params[i]) == 0)
            use_selected = 1;
        else if ((value = NuStrIStr(params[i], "splines=")) != NULL)
            spline_count +=
                NuSplineFindAllBeg(WORLD->current_gscn, value + 8, &splines[spline_count], 32 - spline_count);
        else if (NuStrIStr(params[i], "spline=myspline") != NULL) {
            if (processor->myspline != NULL)
                splines[spline_count++] = processor->myspline;
        } else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757a84)) != NULL) {
            NUGSPLINE *spline = NuSplineFind(WORLD->current_gscn, value + 7);
            if (spline != NULL && spline_count < 32)
                splines[spline_count++] = spline;
        } else if ((value = NuStrIStr(params[i], "x_offset=")) != NULL)
            spline_offset.x = AIParamToFloat(processor, value + 9);
        else if ((value = NuStrIStr(params[i], "y_offset=")) != NULL)
            spline_offset.y = AIParamToFloat(processor, value + 9);
        else if ((value = NuStrIStr(params[i], "z_offset=")) != NULL)
            spline_offset.z = AIParamToFloat(processor, value + 9);
        else if (NuStrICmp(params[i], "looping") == 0)
            looping = 1;
        else if (NuStrIStr(params[i], "addtoset=myset") != NULL)
            creature_set = processor->creature_set;
        else if ((value = NuStrIStr(params[i], "addtoset=")) != NULL)
            creature_set = (i32)AIParamToFloat(processor, value + 9);
        else if (NuStrICmp(params[i], "relative_to_player") == 0)
            relative_to_player = 1;
        else if ((value = NuStrIStr(params[i], "relative_to_locator=")) != NULL)
            relative_locator = AIPathFindLocator(system, value + 20);
        else if (NuStrIStr(params[i], "ridden_by=myself") != NULL) {
            if (packet != NULL && packet->owner != NULL)
                rider = ((GAMEOBJECT *)packet->owner)->objptr;
        } else if ((value = NuStrIStr(params[i], "ridden_by=")) != NULL)
            rider = GetNamedGameObject(system, value + 10);
    }
    if (min_group_size < max_group_size && min_group_size >= 0)
        group_size = NuRand(NULL) % (max_group_size + 1 - min_group_size) + min_group_size;
    if (min_distance != 1000000000.0f && max_distance != 1000000000.0f) {
        f32 fraction = NuRandFloat();
        distance = max_distance * fraction + min_distance * (1.0f - fraction);
    }
    if ((spline_count | use_selected) == 0 || group_size <= 0 || model_count == 0)
        return 1;
    spline = use_selected ? script_spline_selected : splines[qrand() / (65535 / spline_count + 1)];
    model = models[NuRand(NULL) % model_count];
    for (i = 0; i < group_size; i++) {
        GAMEOBJECT *object = AddDynamicCreature(model, NULL, 0, script_name, NULL, NULL, 0, spline, &spline_offset,
                                                looping, creature_set);
        if (object == NULL)
            continue;
        if (relative_to_player || relative_locator != NULL) {
            SPLINEPOS position;
            i16 start = -1, end = -1;
            if (WORLD->current_level == PODSPRINTA_LDATA)
                PodSprint_GetIAlongVals(spline, &start, &end);
            if (relative_to_player) {
                GetNearestSplinePos(&player->collision_position, &position, spline, looping, start, end);
                object->movement_spline_position = position;
            } else if (relative_locator != NULL) {
                GetNearestSplinePos(&relative_locator->position, &position, spline, looping, start, end);
                object->movement_spline_position = position;
            }
        }
        if (distance != 0.0f) {
            NUVEC position;
            NUVEC offset;
            MoveSplinePosition(&object->movement_spline_position, distance);
            position = position, offset = offset;
            PointAlongSpline(object->movement_spline_position.spline, object->movement_spline_position.along, &position,
                             &angle, &pitch, object->movement_spline_position.looping);
            object->movement_facing_angle = angle;
            object->facing_angle = angle;
            object->field_0x276 = angle;
            object->pitch_angle = -pitch;
            if (object->movement_spline_offset.x != 0.0f || object->movement_spline_offset.y != 0.0f ||
                object->movement_spline_offset.z != 0.0f) {
                NuVecRotateX(&offset, &object->movement_spline_offset, object->pitch_angle);
                NuVecRotateY(&offset, &offset, object->field_0x276);
                NuVecAdd(&position, &position, &offset);
            }
            object->position = position;
            object->initial_position = position;
            object->collision_position = position;
            plr_lastpos = position;
            object->start_position = position;
            GameObjectOrigin(object);
            object->last_safe_position = object->position;
            object->ai_update_position = object->position;
        }
        if (rider != NULL)
            TakeOverGameObject(rider, object, 0, 1);
    }
    return 1;
}

i32 __cdecl Action_AddPartDebris(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 num_params, i32 first_time, f32 elapsed) {
    i32 i;
    NUVEC offset;
    NUVEC position;
    i32 type;
    char *value;
    GAMEOBJECT *object;
    type = -1;
    offset.x = 0.0f;
    offset.y = 0.0f;
    offset.z = 0.0f;
    if (first_time) {
        // The original requires coordinates or a character parameter to supply this position.

        for (i = 0; i < num_params; i++) {
            value = NuStrIStr(params[i], (char *)AITransferLiteral75782c);
            if (value != NULL)
                type = PARTLookupType(value + 5);
            else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757c64)) != NULL)
                offset.x = AIParamToFloat(processor, value + 3);
            else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757c60)) != NULL)
                offset.y = AIParamToFloat(processor, value + 3);
            else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757c5c)) != NULL)
                offset.z = AIParamToFloat(processor, value + 3);
            else if ((value = NuStrIStr(params[i], (char *)XArgPrefix)) != NULL)
                position.x = AIParamToFloat(processor, value + 2);
            else if ((value = NuStrIStr(params[i], (char *)YArgPrefix)) != NULL)
                position.y = AIParamToFloat(processor, value + 2);
            else if ((value = NuStrIStr(params[i], (char *)ZArgPrefix)) != NULL)
                position.z = AIParamToFloat(processor, value + 2);
            else if ((value = NuStrIStr(params[i], (char *)AIGameCharacterName)) != NULL) {
                object = GetNamedGameObject(sys, value + 10);
                if (object != NULL)
                    position = object->collision_position;
            }
        }
        if (type != -1) {
            NuVecAdd(&position, &position, &offset);
            AddFiniteShotPART(type, &position, 1);
        }
    }
    return 1;
}

void GuidedMissile_Kill(PART_PC *part, i32 reason) {
    AddGameDebris(WORLD->game_debris_sys, 0x7d, &part->position);
    AddGameDebris(WORLD->game_debris_sys, 0x21, &part->position);
    GameCam_Judder(GameCam, 0.2f, 0, NULL);
    Game_KillPart(part, reason);
    NewRumbleAllPlayers(0.7f, 0.1f, 0, 0);
}

i32 __cdecl Action_TakeOver(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    i32 release;
    char *value;
    i32 player_index;
    PLAYERCHARACTERCONFIG_PC *target_config;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = NULL;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
    }
    target = object != NULL ? object->takeover_target : NULL;
    release = 0;

    if (param_4 > 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                }
            } else {
                value = NuStrIStr(params[index], (char *)AITransferLiteral75781c);
                if (value != NULL) {
                    player_index = (i32)(AIParamToFloat(processor, value + 7)) - 1;
                    if ((u32)(player_index) < 2) {
                        object = Player[player_index];
                    }
                } else {
                    value = NuStrIStr(params[index], (char *)AITransferLiteral757f28);
                    if (value != NULL) {
                        {
                            lookup =
                                GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 7) : NULL;
                            target = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                        }
                    } else if (NuStrIStr(params[index], (char *)AIGameOpponentName) != NULL) {
                        if (packet->opponent != NULL) {
                            target = (GAMEOBJECT *)((APIOBJECT_PC *)packet->opponent)->owner;
                        }
                    } else if (NuStrIStr(params[index], (char *)AITransferLiteral7583f8) != NULL) {
                        release = 1;
                    }
                }
            }
        }
    }

    if (release != 0) {
        if (object != NULL) {
            ReleaseTakeOver(object, 0);
        }
        return 1;
    }
    if (target == NULL || object == NULL) {
        return 1;
    }

    if ((target->character_data->model_flags & 0x40000000) != 0 &&
        (target->character_context == 0x3e || target->character_context == 0x17)) {
        return 0;
    }
    target_config = target->character_data->game_character;
    if (target->takeover_source == NULL && (target_config->flags_090 & 0x40) != 0 &&
        (object->character_data->model_flags & 0x10) == 0) {
        TakeOverGameObject(object, target, 1, 0);
    }
    return 1;
}

i32 __cdecl Action_AddDebris(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    i32 handle;
    NUVEC position;
    GAMEOBJECT *object;
    i32 debris_type;
    char *value;
    NUVEC *debris_position;

    (void)param_6;

    handle = -1;
    position.x = AIPositionUnsetPc;
    position.y = AIPositionUnsetPc;
    position.z = AIPositionUnsetPc;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;

    if (param_5 != 0) {
        debris_type = 33;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                }
            } else if ((value = NuStrIStr(params[index], (char *)XArgPrefix)) != NULL) {
                position.x = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)YArgPrefix)) != NULL) {
                position.y = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)ZArgPrefix)) != NULL) {
                position.z = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral75772c)) != NULL) {
                debris_type = FindGameDebris((APIDEBRISSYS_PC *)(perm_debrissys), value + 5);
            }
        }

        if (position.x == AIPositionUnsetPc && position.y == AIPositionUnsetPc && position.z == AIPositionUnsetPc) {
            if (object == NULL)
                return 1;
            debris_position = &object->collision_position;
        } else {
            debris_position = &position;
        }
        AddFiniteShotDebrisEffect(&handle, WORLD->game_debris_sys->entries[debris_type].effect, debris_position, 1);
    }
    return 1;
}

i32 Action_JudderGameCamera(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)packet;
    (void)param_6;
    if (param_5 != 0) {
        i32 index;
        i32 axis = 0;
        f32 time = 0.1f;
        for (index = 0; index < param_4; ++index) {
            if (NuStrIStr(params[index], "axis=x") != NULL) {
                axis = 0;
            } else if (NuStrIStr(params[index], "axis=y") != NULL) {
                axis = 1;
            } else if (NuStrIStr(params[index], "axis=z") != NULL) {
                axis = 2;
            } else {
                char *parameter = NuStrIStr(params[index], "time");
                if (parameter != NULL) {
                    time = AIParamToFloat(processor, parameter + 5);
                }
            }
        }
        if (axis == 2 && qrand() < 0x8000) {
            time = -time;
        }
        GameCam_Judder(GameCam, time, axis, NULL);
    }
    return 1;
}

void __cdecl DestroyAIGroup(AIGROUP_PC *group) {
    if (group != NULL)
        memset(group, 0, sizeof(*group));
}

typedef struct INTERACTION_PACKET_VIEW_PC {
    u8 unknown[0x18c];
    f32 antinode_timer;
} INTERACTION_PACKET_VIEW_PC;
STATIC_ASSERT(offsetof(INTERACTION_PACKET_VIEW_PC, antinode_timer) == 0x18c);
typedef struct INTERACTION_OBJECT_VIEW_PC {
    u8 unknown[0x292];
    u16 priority;
    u16 resolved_priority;
    u8 unknown296[2];
    APIOBJECT_PC *collision_link;
} INTERACTION_OBJECT_VIEW_PC;
STATIC_ASSERT(offsetof(INTERACTION_OBJECT_VIEW_PC, priority) == 0x292);
STATIC_ASSERT(offsetof(INTERACTION_OBJECT_VIEW_PC, resolved_priority) == 0x294);
STATIC_ASSERT(offsetof(INTERACTION_OBJECT_VIEW_PC, collision_link) == 0x298);
typedef struct INTERACTION_ANTINODE_VIEW_PC {
    u8 unknown[0x50];
    u64 excluded_character_types;
} INTERACTION_ANTINODE_VIEW_PC;
STATIC_ASSERT(offsetof(INTERACTION_ANTINODE_VIEW_PC, excluded_character_types) == 0x50);
#define INTERACTION_PACKET(p) ((INTERACTION_PACKET_VIEW_PC *)(p))
#define INTERACTION_OBJECT(p) ((INTERACTION_OBJECT_VIEW_PC *)(p))
#define INTERACTION_GEOMETRY(p) ((GAMEOBJECT *)(p))
extern f32 antinode_time;
typedef void(__cdecl *CHECK_ANTINODE_FN_PC)(APIOBJECT_PC *, AIANTINODE_PC *, NUVEC *, f32);
/* The genuine callback definitions and initializer remain in the original aisys owner. */
extern CHECK_ANTINODE_FN_PC checkantinodefns[3];

typedef struct INTERACTION_ANTINODE_SHAPE_VIEW_PC {
    u8 unknown[0x40];
    f32 base_radius;
    f32 base_height;
} INTERACTION_ANTINODE_SHAPE_VIEW_PC;
STATIC_ASSERT(offsetof(INTERACTION_ANTINODE_SHAPE_VIEW_PC, base_radius) == 0x40);
STATIC_ASSERT(offsetof(INTERACTION_ANTINODE_SHAPE_VIEW_PC, base_height) == 0x44);
#define INTERACTION_ANTINODE_SHAPE(a) ((INTERACTION_ANTINODE_SHAPE_VIEW_PC *)(a))
#define INTERACTION_SIN(a) NuTrigTable[((a) >> 1) & 0x7fff]
#define INTERACTION_COS(a) NuTrigTable[(((a) + 0x4000) >> 1) & 0x7fff]
extern f32 antinode_reverse_time;
static void AISysCheckAntinode_Rectangle(APIOBJECT_PC *object, AIANTINODE_PC *antinode, NUVEC *difference, f32 radius) {
    NUVEC local_object;
    NUVEC local_destination;
    i32 object_angle;
    i32 turn;
    i32 turn_offset;
    i32 corner_angle;
    i32 angle;
    f32 tangent_x;
    f32 tangent_z;
    i32 absolute_turn;
    f32 scale;
    difference->x = object->ai->movement_position.x - antinode->position.x;
    if (difference->x > radius || difference->x < -radius) {
        return;
    }
    difference->z = object->ai->movement_position.z - antinode->position.z;
    if (difference->z > radius || difference->z < -radius ||
        INTERACTION_GEOMETRY(object)->collision_min.y > antinode->max_y ||
        antinode->min_y > INTERACTION_GEOMETRY(object)->collision_max.y) {
        return;
    }
    NuVecRotateY(difference, difference, -antinode->rotation);
    if (fabsf(difference->x) < INTERACTION_ANTINODE_SHAPE(antinode)->base_radius + object->ai->mover_height &&
        fabsf(difference->z) < INTERACTION_ANTINODE_SHAPE(antinode)->base_height + object->ai->mover_height) {

        difference->x = object->position.x - antinode->position.x;
        difference->y = 0.0f;
        difference->z = object->position.z - antinode->position.z;
        NuVecRotateY(&local_object, difference, -antinode->rotation);
        object_angle = NuAtan2D(local_object.x, local_object.z);
        difference->x = object->ai->movement_destination.x - antinode->position.x;
        difference->y = 0.0f;
        difference->z = object->ai->movement_destination.z - antinode->position.z;
        NuVecRotateY(&local_destination, difference, -antinode->rotation);
        turn = NuAngSub(NuAtan2D(local_destination.x, local_destination.z), object_angle);
        turn_offset = 0;
        if (INTERACTION_PACKET(object->ai)->antinode_timer > 0.0f) {
            if (((object->ai->path_info.flags & 3) == 2 || INTERACTION_GEOMETRY(object)->respawn_timer > 1.0f) &&
                INTERACTION_PACKET(object->ai)->antinode_timer < antinode_time) {
                object->ai->flags ^= 0x8000;
                INTERACTION_PACKET(object->ai)->antinode_timer = antinode_time + antinode_reverse_time;
            }
            if ((object->ai->flags & 0x8000)) {
                if (turn < 0) {
                    turn_offset = 0x10000;
                }
            } else if (turn > 0) {
                turn_offset = -0x10000;
            }
        } else if (turn > 0) {
            object->ai->flags |= 0x8000;
        } else {
            object->ai->flags &= ~0x8000u;
        }
        if ((antinode->game_flags & 2) == 0 && INTERACTION_PACKET(object->ai)->antinode_timer < antinode_time) {
            INTERACTION_PACKET(object->ai)->antinode_timer = antinode_time;
        }
        corner_angle = NuAtan2D(INTERACTION_ANTINODE_SHAPE(antinode)->base_radius,
                                INTERACTION_ANTINODE_SHAPE(antinode)->base_height);
        angle = NuAngAdd(object_angle, 0);

        if (angle > 0x8000 - corner_angle) {
            object->ai->movement_position.x =
                (INTERACTION_ANTINODE_SHAPE(antinode)->base_radius + object->ai->mover_height) * INTERACTION_SIN(angle);
            object->ai->movement_position.z =
                -(object->ai->mover_height + INTERACTION_ANTINODE_SHAPE(antinode)->base_height);
            tangent_x = -1.0f;
            tangent_z = 0.0f;
        } else if (angle > corner_angle) {
            object->ai->movement_position.x =
                INTERACTION_ANTINODE_SHAPE(antinode)->base_radius + object->ai->mover_height;
            object->ai->movement_position.z =
                (object->ai->mover_height + INTERACTION_ANTINODE_SHAPE(antinode)->base_height) * INTERACTION_COS(angle);
            tangent_x = 0.0f;
            tangent_z = -1.0f;
        } else if (angle > -corner_angle) {
            object->ai->movement_position.x =
                (INTERACTION_ANTINODE_SHAPE(antinode)->base_radius + object->ai->mover_height) * INTERACTION_SIN(angle);
            object->ai->movement_position.z =
                object->ai->mover_height + INTERACTION_ANTINODE_SHAPE(antinode)->base_height;
            tangent_x = 1.0f;
            tangent_z = 0.0f;
        } else if (angle > corner_angle - 0x8000) {
            object->ai->movement_position.x =
                -(INTERACTION_ANTINODE_SHAPE(antinode)->base_radius + object->ai->mover_height);
            object->ai->movement_position.z =
                (object->ai->mover_height + INTERACTION_ANTINODE_SHAPE(antinode)->base_height) * INTERACTION_COS(angle);
            tangent_x = 0.0f;
            tangent_z = 1.0f;
        } else {
            object->ai->movement_position.x =
                (INTERACTION_ANTINODE_SHAPE(antinode)->base_radius + object->ai->mover_height) * INTERACTION_SIN(angle);
            object->ai->movement_position.z =
                -(object->ai->mover_height + INTERACTION_ANTINODE_SHAPE(antinode)->base_height);
            tangent_x = -1.0f;
            tangent_z = 0.0f;
        }
        if (turn + turn_offset < 0) {
            tangent_x = -tangent_x;
            tangent_z = -tangent_z;
        }
        absolute_turn = turn < 0 ? -turn : turn;
        if (absolute_turn < 1820) {
            scale = (f32)(absolute_turn) / 1820.0f;
            tangent_x *= scale;
            tangent_z *= scale;
        }
        object->ai->movement_position.y = 0.0f;
        object->ai->movement_position.x += tangent_x;
        object->ai->movement_position.z += tangent_z;
        NuVecRotateY(&object->ai->movement_position, &object->ai->movement_position, antinode->rotation);
        NuVecAdd(&object->ai->movement_position, &object->ai->movement_position, &antinode->position);
        object->flags_low |= 0x40000;
    }
}

static void AISysCheckAntinode_Ellipse(APIOBJECT_PC *object, AIANTINODE_PC *antinode, NUVEC *difference, f32 radius) {
    f32 distance_squared;
    f32 aspect;
    NUVEC local_position;
    NUVEC boundary;
    i32 angle;
    NUVEC local_object;
    NUVEC object_boundary;
    NUVEC local_destination;
    i32 object_angle;
    i32 turn;
    f32 tangent_x;
    f32 tangent_z;
    i32 absolute_turn;
    f32 scale;
    distance_squared = difference->x * difference->x + difference->z * difference->z;
    if (distance_squared < radius * radius) {
        aspect = INTERACTION_ANTINODE_SHAPE(antinode)->base_height / INTERACTION_ANTINODE_SHAPE(antinode)->base_radius;

        difference->y = 0.0f;
        NuVecRotateY(&local_position, difference, -antinode->rotation);
        angle = NuAtan2D(aspect * local_position.x, local_position.z);
        boundary.x = INTERACTION_SIN(angle) * INTERACTION_ANTINODE_SHAPE(antinode)->base_radius;
        boundary.y = 0.0f;
        boundary.z = INTERACTION_COS(angle) * INTERACTION_ANTINODE_SHAPE(antinode)->base_height;
        NuVecNorm(difference, &boundary);
        NuVecScale(difference, difference, object->ai->mover_height);
        NuVecAdd(&boundary, &boundary, difference);
        if (distance_squared < boundary.x * boundary.x + boundary.z * boundary.z) {

            difference->x = object->position.x - antinode->position.x;
            difference->y = 0.0f;
            difference->z = object->position.z - antinode->position.z;
            NuVecRotateY(&local_object, difference, -antinode->rotation);
            object_angle = NuAtan2D(aspect * local_object.x, local_object.z);
            object_boundary.x = INTERACTION_SIN(object_angle) * INTERACTION_ANTINODE_SHAPE(antinode)->base_radius;
            object_boundary.y = 0.0f;
            object_boundary.z = INTERACTION_COS(object_angle) * INTERACTION_ANTINODE_SHAPE(antinode)->base_height;
            NuVecNorm(difference, &object_boundary);
            NuVecScale(difference, difference, object->ai->mover_height);
            NuVecAdd(&object_boundary, &object_boundary, difference);
            difference->x = object->ai->movement_destination.x - antinode->position.x;
            difference->y = 0.0f;
            difference->z = object->ai->movement_destination.z - antinode->position.z;
            NuVecRotateY(&local_destination, difference, -antinode->rotation);
            turn = NuAngSub(NuAtan2D(aspect * local_destination.x, local_destination.z), object_angle);
            if (INTERACTION_PACKET(object->ai)->antinode_timer > 0.0f) {
                if (((object->ai->path_info.flags & 3) == 2 || INTERACTION_GEOMETRY(object)->respawn_timer > 1.0f) &&
                    INTERACTION_PACKET(object->ai)->antinode_timer < antinode_time) {
                    object->ai->flags ^= 0x8000;
                    INTERACTION_PACKET(object->ai)->antinode_timer = antinode_time + antinode_reverse_time;
                }
                if ((object->ai->flags & 0x8000)) {
                    if (turn < 0) {
                        turn += 0x10000;
                    }
                } else if (turn > 0) {
                    turn -= 0x10000;
                }
            } else if (turn > 0) {
                object->ai->flags |= 0x8000;
            } else {
                object->ai->flags &= ~0x8000u;
            }
            if ((antinode->game_flags & 2) == 0 && INTERACTION_PACKET(object->ai)->antinode_timer < antinode_time) {
                INTERACTION_PACKET(object->ai)->antinode_timer = antinode_time;
            }
            tangent_x = INTERACTION_COS(object_angle) * INTERACTION_ANTINODE_SHAPE(antinode)->base_radius;
            tangent_z = -INTERACTION_SIN(object_angle) * INTERACTION_ANTINODE_SHAPE(antinode)->base_height;
            if (turn < 0) {
                tangent_x = -tangent_x;
                tangent_z = -tangent_z;
            }
            absolute_turn = turn < 0 ? -turn : turn;
            if (absolute_turn < 1820) {
                scale = (f32)(absolute_turn) / 1820.0f;
                tangent_x *= scale;
                tangent_z *= scale;
            }
            object->ai->movement_position.y = 0.0f;
            object->ai->movement_position.x = object_boundary.x + tangent_x;
            object->ai->movement_position.z = object_boundary.z + tangent_z;
            NuVecRotateY(&object->ai->movement_position, &object->ai->movement_position, antinode->rotation);
            NuVecAdd(&object->ai->movement_position, &object->ai->movement_position, &antinode->position);
            object->flags_low |= 0x40000;
        }
    }
}

static void AISysCheckAntinode_Circle(APIOBJECT_PC *object, AIANTINODE_PC *antinode, NUVEC *difference, f32 radius) {
    i32 angle;
    i32 turn;
    f32 turn_fraction;
    i32 max_turn;
    f32 squared_distance;
    squared_distance = difference->x * difference->x + difference->z * difference->z;
    if (squared_distance < radius * radius) {
        difference->x = object->position.x - antinode->position.x;
        difference->z = object->position.z - antinode->position.z;
        angle = NuAtan2D(difference->x, difference->z);
        difference->x = object->ai->movement_destination.x - antinode->position.x;
        difference->z = object->ai->movement_destination.z - antinode->position.z;
        turn = NuAngSub(NuAtan2D(difference->x, difference->z), angle);
        if (INTERACTION_PACKET(object->ai)->antinode_timer > 0.0f) {
            if (((object->ai->path_info.flags & 3) == 2 || INTERACTION_GEOMETRY(object)->respawn_timer > 1.0f) &&
                INTERACTION_PACKET(object->ai)->antinode_timer < antinode_time) {
                object->ai->flags ^= 0x8000;
                INTERACTION_PACKET(object->ai)->antinode_timer = antinode_time + antinode_reverse_time;
            }
            if ((object->ai->flags & 0x8000)) {
                if (turn < 0) {
                    turn += 0x10000;
                }
            } else if (turn > 0) {
                turn -= 0x10000;
            }
        } else {
            if (turn > 0) {
                object->ai->flags |= 0x8000;
            } else {
                object->ai->flags &= ~0x8000u;
            }
        }
        if ((antinode->game_flags & 2) == 0 && INTERACTION_PACKET(object->ai)->antinode_timer < antinode_time) {
            INTERACTION_PACKET(object->ai)->antinode_timer = antinode_time;
        }
        turn_fraction = ai_moveradius / radius;
        max_turn = (i32)((f64)turn_fraction * AIPathAngularScale);
        if (turn > max_turn) {
            turn = max_turn;
        } else if (turn < -max_turn) {
            turn = -max_turn;
        }
        angle = NuAngAdd(angle, turn);
        difference->x = 0.0f;
        difference->y = 0.0f;
        difference->z = radius;
        NuVecRotateY(difference, difference, angle);
        object->ai->movement_position.x = difference->x + antinode->position.x;
        object->ai->movement_position.y = object->ai->movement_destination.y;
        object->ai->movement_position.z = difference->z + antinode->position.z;
        object->flags_low |= 0x40000;
    }
}

CHECK_ANTINODE_FN_PC checkantinodefns[3] = {AISysCheckAntinode_Circle, AISysCheckAntinode_Ellipse,
                                            AISysCheckAntinode_Rectangle};

void __cdecl AISysCreatureAntinodeInteraction(AISYS_PC *system, i32 object_count, APIOBJECT_PC **objects, i32 *unused) {
    AIANTINODE_PC *active[192];
    i32 count, index, obstacle;
    AIANTINODE_PC *antinode;
    APIOBJECT_PC *object;
    NUVEC difference;
    f32 radius;
    if (system != NULL) {
        count = 0;
        antinode = system->antinodes;
        for (index = 0; index < system->antinode_count && count < 192; ++index, ++antinode) {
            if (antinode->enabled && (!antinode->has_special || NuSpecialGetVisibilityFn(&antinode->special_handle)))
                active[count++] = antinode;
        }
        antinode = dynamic_antinodes;
        for (index = 0; index < 64 && count < 192; ++index, ++antinode) {
            if (antinode->enabled && (!antinode->has_special || NuSpecialGetVisibilityFn(&antinode->special_handle)))
                active[count++] = antinode;
        }
        if (count != 0) {
            for (index = 0; index < object_count; ++index) {
                object = objects[index];
                {
                    u32 flags = object->flags_low;
                    if (flags & 0x20000)
                        continue;
                    if (flags & 0x80)
                        continue;
                }
                for (obstacle = 0; obstacle < count; ++obstacle) {
                    antinode = active[obstacle];
                    if (NuSpecialExistsFn(&object->collision_special) &&
                        NuSpecialCompare(&object->collision_special, &antinode->special_handle))
                        continue;
                    if (((u64)1 << object->object_id) &
                        ((INTERACTION_ANTINODE_VIEW_PC *)antinode)->excluded_character_types)
                        continue;
                    radius = object->ai->mover_height + antinode->radius;
                    difference.x = object->ai->movement_position.x - antinode->position.x;
                    if (difference.x > radius || difference.x < -radius)
                        continue;
                    difference.z = object->ai->movement_position.z - antinode->position.z;
                    if (difference.z > radius || difference.z < -radius ||
                        INTERACTION_GEOMETRY(object)->collision_min.y > antinode->max_y)
                        continue;
                    /* Native unordered min-height comparison continues to callback. */
                    if (!(antinode->min_y > INTERACTION_GEOMETRY(object)->collision_max.y))
                        checkantinodefns[antinode->type](object, antinode, &difference, radius);
                }
            }
        }
    }
    AIAntinodeCullSingleFrame();
}

void __cdecl AISysCreatureInteraction2D(AISYS_PC *system, i32 object_count, APIOBJECT_PC **objects, i32 *immovable,
                                        f32 delta_time) {
    i32 index, first_index, second_index, first_fixed, second_fixed;
    APIOBJECT_PC *first, *second;
    AIPACKET_PC *first_packet, *second_packet;
    NUVEC difference;
    f32 radius, distance, squared_distance, overlap, scale;
    u16 first_priority, second_priority;
    for (index = 0; index < object_count; ++index) {
        APIOBJECT_PC *timer_object = objects[index];
        INTERACTION_PACKET(timer_object->ai)->antinode_timer -= delta_time;
        if (INTERACTION_PACKET(timer_object->ai)->antinode_timer < 0.0f)
            INTERACTION_PACKET(timer_object->ai)->antinode_timer = 0.0f;
    }
    for (first_index = 0; first_index < object_count - 1; ++first_index) {
        first = objects[first_index];
        for (second_index = first_index + 1; second_index < object_count; ++second_index) {
            second = objects[second_index];
            first_packet = first->ai;
            second_packet = second->ai;
            radius = second_packet->mover_height + first_packet->mover_height;
            difference.x = first_packet->movement_position.x - second_packet->movement_position.x;
            if (difference.x > radius || difference.x < -radius)
                continue;
            difference.z = first_packet->movement_position.z - second_packet->movement_position.z;
            if (difference.z > radius || difference.z < -radius)
                continue;
            if (INTERACTION_GEOMETRY(first)->collision_min.y > INTERACTION_GEOMETRY(second)->collision_max.y ||
                INTERACTION_GEOMETRY(second)->collision_min.y > INTERACTION_GEOMETRY(first)->collision_max.y)
                continue;
            if (INTERACTION_OBJECT(first)->collision_link == second ||
                INTERACTION_OBJECT(second)->collision_link == first)
                continue;
            first_fixed = immovable != NULL ? immovable[first_index] : 0;
            second_fixed = immovable != NULL ? immovable[second_index] : 0;
            if (first_packet->dont_avoid_character == (GAMEOBJECT *)second)
                first_fixed = 1;
            if (second_packet->dont_avoid_character == (GAMEOBJECT *)first)
                second_fixed = 1;
            if (first->flags_low & 0x80)
                first_fixed = 1;
            if (second->flags_low & 0x80)
                second_fixed = 1;
            first_priority = INTERACTION_OBJECT(first)->priority;
            second_priority = INTERACTION_OBJECT(second)->priority;
            if (first_priority > second_priority) {
                first_fixed = 1;
                if (second_fixed)
                    continue;
            } else if (first_priority < second_priority) {
                second_fixed = 1;
                if (first_fixed)
                    continue;
            } else {
                continue;
            }
            squared_distance = difference.x * difference.x + difference.z * difference.z;
            distance = NuFsqrt(squared_distance);
            if (!(distance < radius))
                continue;
            difference.y = 0.0f;
            overlap = radius - distance;
            if (first_fixed) {
                if (first_priority > INTERACTION_OBJECT(second)->resolved_priority) {
                    scale = AIPathSafeDivide(overlap, distance);
                    NuVecScale(&difference, &difference, scale);
                    NuVecSub(&second->ai->movement_position, &second->ai->movement_position, &difference);
                    INTERACTION_OBJECT(second)->resolved_priority = INTERACTION_OBJECT(first)->priority;
                }
            } else if (second_fixed) {
                if (second_priority > INTERACTION_OBJECT(first)->resolved_priority) {
                    scale = AIPathSafeDivide(overlap, distance);
                    NuVecScale(&difference, &difference, scale);
                    NuVecAdd(&first->ai->movement_position, &first->ai->movement_position, &difference);
                    INTERACTION_OBJECT(first)->resolved_priority = INTERACTION_OBJECT(second)->priority;
                }
            } else {
                AIScriptMissingStateDiagnostic();
                scale = AIPathSafeDivide(overlap, distance);
                NuVecScale(&difference, &difference, scale);
                NuVecSub(&second->ai->movement_position, &second->ai->movement_position, &difference);
            }
            first_packet = first->ai;
            if (INTERACTION_PACKET(first_packet)->antinode_timer > antinode_time &&
                INTERACTION_PACKET(second_packet = second->ai)->antinode_timer > antinode_time) {
                if (INTERACTION_PACKET(first_packet)->antinode_timer >
                    INTERACTION_PACKET(second_packet)->antinode_timer) {
                    INTERACTION_PACKET(second_packet)->antinode_timer =
                        INTERACTION_PACKET(first_packet)->antinode_timer;
                    first_packet = first->ai;
                    second_packet = second->ai;
                    second_packet->flags ^= (first_packet->flags ^ second_packet->flags) & 0x8000;
                } else {
                    INTERACTION_PACKET(first_packet)->antinode_timer =
                        INTERACTION_PACKET(second_packet)->antinode_timer;
                    first_packet = first->ai;
                    second_packet = second->ai;
                    first_packet->flags ^= (second_packet->flags ^ first_packet->flags) & 0x8000;
                }
            } else if (INTERACTION_PACKET(second_packet = second->ai)->antinode_timer > antinode_time) {
                INTERACTION_PACKET(first_packet)->antinode_timer = INTERACTION_PACKET(second_packet)->antinode_timer;
                first_packet = first->ai;
                second_packet = second->ai;
                first_packet->flags ^= (second_packet->flags ^ first_packet->flags) & 0x8000;
            } else if (INTERACTION_PACKET(first_packet)->antinode_timer > 0.0f) {
                INTERACTION_PACKET(second_packet)->antinode_timer = INTERACTION_PACKET(first_packet)->antinode_timer;
                first_packet = first->ai;
                second_packet = second->ai;
                second_packet->flags ^= (first_packet->flags ^ second_packet->flags) & 0x8000;
            }
            if ((first->motion_flags ^ second->motion_flags) & 1) {
                first->ai->flags |= 0x4000;
                second->ai->flags |= 0x4000;
            }
        }
    }
    AISysCreatureAntinodeInteraction(system, object_count, objects, immovable);
}

#include "saga.h"
#include <stdio.h>
#include <string.h>

typedef struct ai_area_load_pc_s {
    char name[16];
    NUVEC minimum;
    NUVEC maximum;
    u16 flags;
    u8 unknown_2a;
    u8 game_flags;
    u8 unknown_2c[12];
    AISYS_PC *system;
    u8 unknown_3c[4];
} AI_AREA_LOAD_PC;
STATIC_ASSERT(sizeof(AI_AREA_LOAD_PC) == 0x40);
STATIC_ASSERT(offsetof(AI_AREA_LOAD_PC, minimum) == 0x10);
STATIC_ASSERT(offsetof(AI_AREA_LOAD_PC, maximum) == 0x1c);
STATIC_ASSERT(offsetof(AI_AREA_LOAD_PC, flags) == 0x28);
STATIC_ASSERT(offsetof(AI_AREA_LOAD_PC, game_flags) == 0x2b);
STATIC_ASSERT(offsetof(AI_AREA_LOAD_PC, system) == 0x38);

typedef struct ai_creature_load_pc_s {
    char name[16];
    char script_name[16];
    NUVEC position;
    i32 y_rotation;
    AIPATHINFO_PC path_info;
    i32 flags;
    u8 set;
    u8 unknown_4d;
    i16 type;
    u8 count;
    u8 count_across;
    u8 unknown_52[6];
    u32 active_mask;
    f32 x_spacing;
    f32 z_spacing;
    f32 script_params[4];
    AIAREA_PC *activate_area;
    AIAREA_PC *area;
    AILOCATOR_PC *locator;
    AILOCATOR_PC *respawn_locator;
    u8 activation_difficulty;
    i8 min_respawn_count;
    i8 max_respawn_count;
    u8 activation_type;
    f32 min_respawn_time;
    f32 max_respawn_time;
    f32 start_stagger;
    f32 view_distance;
    f32 hear_distance;
    f32 max_view_height;
    f32 min_view_height;
} AI_CREATURE_LOAD_PC;
STATIC_ASSERT(sizeof(AI_CREATURE_LOAD_PC) == 0xa4);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, script_name) == 0x10);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, position) == 0x20);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, y_rotation) == 0x2c);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, path_info) == 0x30);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, flags) == 0x48);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, set) == 0x4c);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, type) == 0x4e);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, count) == 0x50);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, count_across) == 0x51);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, active_mask) == 0x58);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, x_spacing) == 0x5c);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, z_spacing) == 0x60);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, script_params) == 0x64);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, activate_area) == 0x74);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, area) == 0x78);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, locator) == 0x7c);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, respawn_locator) == 0x80);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, activation_difficulty) == 0x84);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, min_respawn_time) == 0x88);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, start_stagger) == 0x90);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, view_distance) == 0x94);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, min_view_height) == 0xa0);

typedef struct ai_antinode_load_pc_s {
    NUVEC position;
    f32 radius;
    f32 min_y;
    f32 min_y_offset;
    f32 max_y;
    f32 max_y_offset;
    NUSPECIALHANDLE special_handle;
    NUVEC special_position;
    i32 flags;
    i32 rotation_offset;
    f32 base_radius;
    f32 base_height;
    u8 enabled;
    u8 game_flags;
    u8 type;
    u8 has_special;
    u8 unknown_4c[12];
} AI_ANTINODE_LOAD_PC;
STATIC_ASSERT(sizeof(AI_ANTINODE_LOAD_PC) == 0x58);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, flags) == 0x38);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, base_radius) == 0x40);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, base_height) == 0x44);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, enabled) == 0x48);

typedef struct ai_system_load_pc_s {
    void *storage;
    VARIPTR storage_end;
    VARIPTR storage_cursor;
    i32 storage_size;
    AIPATHSYS_PC *path_sys;
    i32 creature_count;
    AI_CREATURE_LOAD_PC *creatures;
    NULISTHDR scripts;
    i32 locator_count;
    AILOCATOR_PC *locators;
    i32 locator_set_count;
    AILOCATORSET_PC *locator_sets;
    i32 area_count;
    AI_AREA_LOAD_PC *areas;
    AIGROUP_PC groups[16];
    i32 antinode_count;
    AI_ANTINODE_LOAD_PC *antinodes;
    u8 unknown_1384[8];
    APIOBJECT_PC *player_1;
    APIOBJECT_PC *player_2;
    NUGSCN *scene;
} AI_SYSTEM_LOAD_PC;
STATIC_ASSERT(sizeof(AI_SYSTEM_LOAD_PC) == 0x1398);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, storage_end) == 4);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, storage_cursor) == 8);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, storage_size) == 0xc);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, path_sys) == 0x10);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, creature_count) == 0x14);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, creatures) == 0x18);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, scripts) == 0x1c);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, locators) == 0x28);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, locator_sets) == 0x30);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, areas) == 0x38);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, antinode_count) == 0x137c);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, antinodes) == 0x1380);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, scene) == 0x1394);

STATIC_ASSERT(offsetof(AI_AREA_LOAD_PC, name) == 0x0);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, name) == 0x0);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, min_respawn_count) == 0x85);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, max_respawn_count) == 0x86);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, activation_type) == 0x87);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, max_respawn_time) == 0x8c);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, hear_distance) == 0x98);
STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, max_view_height) == 0x9c);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, position) == 0x0);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, radius) == 0xc);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, min_y) == 0x10);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, min_y_offset) == 0x14);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, max_y) == 0x18);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, max_y_offset) == 0x1c);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, special_handle) == 0x20);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, special_position) == 0x2c);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, rotation_offset) == 0x3c);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, game_flags) == 0x49);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, type) == 0x4a);
STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, has_special) == 0x4b);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, storage) == 0x0);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, locator_count) == 0x24);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, locator_set_count) == 0x2c);
STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, area_count) == 0x34);

STATIC_ASSERT(offsetof(AIPATHCNX_PC, traversal_flags) == 0x0);
STATIC_ASSERT(offsetof(AIPATHCNX_PC, original_traversal_flags) == 0x8);
STATIC_ASSERT(offsetof(AIPATHCNX_PC, distance) == 0x18);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, name) == 0x0);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, radius) == 0x10);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, radius_squared) == 0x14);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, min_height) == 0x18);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, min_height_offset) == 0x1c);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, max_height) == 0x20);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, max_height_offset) == 0x24);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, connection_count) == 0x28);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, flags) == 0x29);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, has_special) == 0x2a);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, runtime_flags) == 0x2b);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, path_flags) == 0x2c);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, distance_cache_nodes) == 0x2e);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, special_route_index) == 0x30);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, special_position) == 0x4c);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, route_boundary_mask) == 0x5a);
STATIC_ASSERT(offsetof(AIPATHROUTE_PC, name) == 0x0);
STATIC_ASSERT(offsetof(AIPATHROUTE_PC, node_routes) == 0x4);
STATIC_ASSERT(offsetof(AIPATHROUTE_PC, node_directions) == 0x8);
STATIC_ASSERT(offsetof(AIPATHROUTE_PC, exit_nodes) == 0xc);
STATIC_ASSERT(offsetof(AIPATHROUTE_PC, route_count) == 0x10);
STATIC_ASSERT(offsetof(AIPATHROUTE_PC, exit_node_count) == 0x11);
STATIC_ASSERT(offsetof(AIPATHROUTE_PC, route_nodes) == 0x14);
STATIC_ASSERT(offsetof(AIPATHNODELINK_PC, node_index) == 0x0);
STATIC_ASSERT(offsetof(AIPATH_PC, name) == 0x0);
STATIC_ASSERT(offsetof(AIPATH_PC, node_count) == 0x10);
STATIC_ASSERT(offsetof(AIPATH_PC, flags) == 0x11);
STATIC_ASSERT(offsetof(AIPATHSPECIALROUTE_PC, path_count) == 0x0);

extern char *AiLevelPathName;
extern i32 ai_usepackfile;
extern u8 AiLoadActivationDifficultyPc;
extern i8 AiLoadMinimumRespawnCountPc, AiLoadMaximumRespawnCountPc;
extern f32 AiLoadMinimumRespawnTimePc, AiLoadMaximumRespawnTimePc;
static f32 AiLoadStartStaggerPc = 0.0f;
extern i32(__cdecl *GlobalCharacterTypeIDFn)(char *);
extern void(__cdecl *GameAILoadFn)(AISYS_PC *, i32, NUGSCN *, VARIPTR *, VARIPTR *);
void __cdecl AIPathCalcExtents(AIPATH_PC *path);

typedef struct ai_path_system_load_pc_s {
    u8 path_count;
    u8 unknown_01;
    u16 special_route_count;
    AIPATH_PC **paths;
    AIPATH_PC *active_path;
    AIPATHSPECIALROUTE_PC *special_routes;
} AI_PATH_SYSTEM_LOAD_PC;
STATIC_ASSERT(sizeof(AI_PATH_SYSTEM_LOAD_PC) == 0x10);
STATIC_ASSERT(offsetof(AI_PATH_SYSTEM_LOAD_PC, path_count) == 0);
STATIC_ASSERT(offsetof(AI_PATH_SYSTEM_LOAD_PC, special_routes) == 0xc);
STATIC_ASSERT(offsetof(AI_PATH_SYSTEM_LOAD_PC, special_route_count) == 2);
STATIC_ASSERT(offsetof(AI_PATH_SYSTEM_LOAD_PC, paths) == 4);
STATIC_ASSERT(sizeof(AIPATH_PC) == 0xa8);
STATIC_ASSERT(offsetof(AIPATH_PC, connection_count) == 0x12);
STATIC_ASSERT(offsetof(AIPATH_PC, index) == 0x15);
STATIC_ASSERT(offsetof(AIPATH_PC, connections) == 0x80);
STATIC_ASSERT(offsetof(AIPATH_PC, route_matrix) == 0x84);
STATIC_ASSERT(offsetof(AIPATH_PC, bounds_min) == 0x90);
STATIC_ASSERT(offsetof(AIPATH_PC, bounds_max) == 0x9c);

void __cdecl pathEditorSaveData(AIPATHSYS_PC *path_system) {
    AI_PATH_SYSTEM_LOAD_PC *system = (AI_PATH_SYSTEM_LOAD_PC *)path_system;
    i32 path_index;
    i32 index;
    i32 row;
    i32 character;
    i32 character_count;
    u32 length;
    char *text;
    char *end;
    AIPATH_PC *path;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *node;
    AIPATHROUTE_PC *route;
    AIPATHNODELINK_PC *link;
    AIPATHSPECIALROUTE_PC *group;
    u32 count_mask_low;
    u64 bit;

    EdFileWriteInt(system->path_count);
    for (path_index = 0; path_index < system->path_count; ++path_index) {
        path = system->paths[path_index];
        EdFileWrite(path->name, 16);
        EdFileWriteChar(path->node_count);
        EdFileWriteChar(path->flags);
        EdFileWriteShort((u16)path->connection_count);
        for (index = 0; index < path->connection_count; ++index) {
            connection = &path->connections[index];
            EdFileWriteChar(connection->node_indices[0]);
            EdFileWriteChar(connection->node_indices[1]);
            if (aidata_version > 11) {
                EdFileWriteInt((i32)connection->traversal_flags[0]);
                EdFileWriteInt((i32)connection->traversal_flags[1]);
            } else if (aidata_version > 8) {
                EdFileWriteShort((u16)connection->traversal_flags[0]);
                EdFileWriteShort((u16)connection->traversal_flags[1]);
            } else {
                EdFileWriteChar((u8)connection->traversal_flags[0]);
                EdFileWriteChar((u8)connection->traversal_flags[1]);
            }
            EdFileWriteShort((u16)connection->rotation);
            EdFileWriteShort((u16)connection->route_mask);
            EdFileWriteFloat(connection->distance);
            EdFileWriteFloat(connection->horizontal_distance);
        }
        for (index = 0; index < path->node_count; ++index) {
            node = &path->nodes[index];
            text = node->name;
            length = 0;
            if (text != NULL) {
                end = text;
                while (*end != 0)
                    ++end;
                length = (u32)(end - text) + 1;
            }
            EdFileWriteInt((i32)length);
            if (length != 0)
                EdFileWrite(node->name, (i32)length);
            EdFileWriteFloat(node->position.x);
            EdFileWriteFloat(node->position.y);
            EdFileWriteFloat(node->position.z);
            EdFileWriteFloat(node->radius);
            if (aidata_version > 7) {
                EdFileWriteFloat(node->min_height);
                EdFileWriteFloat(node->max_height);
            }
            EdFileWriteChar(node->connection_count);
            EdFileWriteChar(node->flags);
            EdFileWriteChar(0);
            EdFileWriteChar(node->runtime_flags);
            EdFileWriteShort((u16)node->path_flags);
            EdFileWriteChar(aidata_version >= 19 ? node->special_route_index : 0);
            if (NuSpecialExistsFn(&node->special_handle) != 0 &&
                (text = NuSpecialGetName(&node->special_handle)) != NULL) {
                end = text;
                while (*end != 0)
                    ++end;
                length = (u32)(end - text) + 1;
                EdFileWriteChar(length);
                EdFileWrite(text, (i32)length);
                EdFileWriteFloat(node->special_position.x);
                EdFileWriteFloat(node->special_position.y);
                EdFileWriteFloat(node->special_position.z);
            } else {
                EdFileWriteChar(0);
            }
            for (row = 0; row < node->connection_count; ++row)
                EdFileWriteShort((u16)(node->connections[row] - path->connections));
            if ((node->connection_count & 1) != 0)
                EdFileWriteShort(0);
            EdFileWriteShort((u16)node->route_membership_mask);
            EdFileWriteShort((u16)node->route_boundary_mask);
        }
        for (row = 0; row < path->node_count; ++row)
            EdFileWrite(path->route_matrix[row], path->node_count);
        EdFileWriteChar(path->route_count);
        for (index = 0; index < path->route_count; ++index) {
            route = &path->routes[index];
            text = route->name;
            end = text;
            while (*end != 0)
                ++end;
            length = (u32)(end - text) + 1;
            EdFileWriteChar(length);
            EdFileWrite(route->name, (i32)length);
            EdFileWriteChar(route->route_count);
            EdFileWriteChar(route->exit_node_count);
            EdFileWriteChar(0);
            EdFileWriteChar(0);
            if (route->route_count != 0) {
                EdFileWrite(route->node_routes, path->node_count);
                EdFileWrite(route->node_directions, route->route_count);
                for (row = 0; row < route->route_count; ++row)
                    EdFileWrite(route->route_nodes[row], route->route_count);
                if (route->exit_node_count != 0)
                    EdFileWrite(route->exit_nodes, route->exit_node_count);
            }
            if (SpecialRouteCharacterNameFn != NULL) {
                count_mask_low = route->character_mask[2];
                character_count = 0;
                for (character = 0; character < 64; ++character) {
                    bit = (u64)1 << character;
                    if ((((u32)bit & count_mask_low) | ((u32)(bit >> 32) & route->character_mask[3])) != 0)
                        ++character_count;
                }
                EdFileWriteChar(character_count);
                for (character = 0; character < 63; ++character) {
                    bit = (u64)1 << character;
                    if ((((u32)bit & route->character_mask[2]) | ((u32)(bit >> 32) & route->character_mask[3])) != 0) {
                        text = SpecialRouteCharacterNameFn((u8)character);
                        if (text != NULL) {
                            end = text;
                            while (*end != 0)
                                ++end;
                            length = (u32)(end - text) + 1;
                            EdFileWriteChar(length);
                            EdFileWrite(text, (i32)length);
                        } else {
                            EdFileWriteChar(0);
                        }
                    }
                }
                if ((route->character_mask[3] & 0x80000000u) != 0) {
                    EdFileWriteChar(9);
                    EdFileWrite((void *)GameAIEveryonePc, 9);
                }
            } else {
                EdFileWriteChar(0);
            }
        }
        if (aidata_version >= 19) {
            EdFileWriteChar(path->special_route_count);
            link = path->special_routes;
            for (index = 0; index < path->special_route_count; ++index, ++link) {
                EdFileWriteUnsignedChar(link->node_index);
                EdFileWriteShort((u16)link->special_route_index);
            }
        }
    }
    if (aidata_version >= 19) {
        EdFileWriteShort((u16)system->special_route_count);
        group = system->special_routes;
        for (index = 0; index < system->special_route_count; ++index, ++group) {
            EdFileWriteChar(group->path_count);
            for (row = 0; row < group->path_count; ++row)
                EdFileWriteChar(group->paths[row]->index);
        }
    }
}

// Native inlined AISysBufferAlloc blocks retain the failure diagnostic and
// publication after the allocation memset. Size arithmetic remains unsigned32.
#define PATH_LOAD_ALLOCATE(type, destination, input_cursor, input_end, input_size)                                     \
    do {                                                                                                               \
        u32 allocation_size = (input_size);                                                                            \
        VARIPTR *allocation_cursor = (input_cursor);                                                                   \
        VARIPTR *allocation_end = (input_end);                                                                         \
        void *allocation_result = NULL;                                                                                \
        if (allocation_cursor == NULL || allocation_end == NULL ||                                                     \
            allocation_cursor->addr + allocation_size >= allocation_end->addr) {                                       \
            AIScriptMissingStateDiagnostic();                                                                          \
        } else {                                                                                                       \
            u32 allocation_address = (allocation_cursor->addr + 15) & ~15u;                                            \
            allocation_cursor->addr = allocation_address + allocation_size;                                            \
            allocation_result = (void *)allocation_address;                                                            \
            memset(allocation_result, 0, allocation_size);                                                             \
        }                                                                                                              \
        destination = (type)allocation_result;                                                                         \
    } while (0)

static AIPATHSYS_PC *AISysLoadPathsPc(VARIPTR *cursor, i32 version, NUGSCN *scene, VARIPTR *end) {
    i32 count = EdFileReadInt();
    AI_PATH_SYSTEM_LOAD_PC *system;
    i32 path_index;
    i32 index;
    i32 route_index;
    i32 row;
    i32 length;
    i32 character_count;
    char special_name[256];
    if (count == 0)
        return NULL;
    PATH_LOAD_ALLOCATE(AI_PATH_SYSTEM_LOAD_PC *, system, cursor, end, sizeof(*system));
    if (system == NULL)
        return NULL;
    system->path_count = (u8)count;
    system->paths = (AIPATH_PC **)AISysBufferAlloc(cursor, end, system->path_count * sizeof(AIPATH_PC *));
    for (path_index = 0; path_index < system->path_count; ++path_index) {
        AIPATH_PC *path;
        PATH_LOAD_ALLOCATE(AIPATH_PC *, system->paths[path_index], cursor, end, sizeof(AIPATH_PC));
        path = system->paths[path_index];
        EdFileRead(path->name, 16);
        path->node_count = (u8)EdFileReadChar();
        path->flags = (u8)EdFileReadChar();
        path->index = (u8)path_index;
        path->connection_count = (u16)(version == 1 ? (i16)(i8)EdFileReadChar() : EdFileReadShort());
        if (path->connection_count != 0) {
            PATH_LOAD_ALLOCATE(AIPATHCNX_PC *, path->connections, cursor, end,
                               path->connection_count * sizeof(AIPATHCNX_PC));
            for (index = 0; index < path->connection_count; ++index) {
                AIPATHCNX_PC *connection = &path->connections[index];
                connection->node_indices[0] = (u8)EdFileReadChar();
                connection->node_indices[1] = (u8)EdFileReadChar();
                if (version > 11) {
                    connection->traversal_flags[0] = EdFileReadInt();
                    connection->traversal_flags[1] = EdFileReadInt();
                } else if (version > 8) {
                    connection->traversal_flags[0] = (i16)EdFileReadShort();
                    connection->traversal_flags[1] = (i16)EdFileReadShort();
                } else {
                    connection->traversal_flags[0] = (i8)EdFileReadChar();
                    connection->traversal_flags[1] = (i8)EdFileReadChar();
                }
                connection->original_traversal_flags[0] = connection->traversal_flags[0];
                connection->original_traversal_flags[1] = connection->traversal_flags[1];
                connection->rotation = EdFileReadShort();
                connection->route_mask = (u16)EdFileReadShort();
                connection->distance = EdFileReadFloat();
                connection->horizontal_distance = EdFileReadFloat();
                if (connection->horizontal_distance == 0.0f)
                    connection->horizontal_distance = 0.00001f;
            }
        }
        if (version == 1)
            EdFileReadChar();
        if (path->node_count != 0) {
            PATH_LOAD_ALLOCATE(AIPATHNODE_PC *, path->nodes, cursor, end, path->node_count * sizeof(AIPATHNODE_PC));
            for (index = 0; index < path->node_count; ++index) {
                AIPATHNODE_PC *node = &path->nodes[index];
                length = EdFileReadInt();
                if (length != 0) {
                    PATH_LOAD_ALLOCATE(char *, node->name, cursor, end, length + 1);
                    EdFileRead(node->name, length);
                }
                node->position.x = EdFileReadFloat();
                node->position.y = EdFileReadFloat();
                node->position.z = EdFileReadFloat();
                node->radius = EdFileReadFloat();
                node->radius_squared = node->radius * node->radius;
                if (version > 7) {
                    node->min_height = EdFileReadFloat();
                    node->max_height = EdFileReadFloat();
                } else {
                    node->min_height = node->position.y - default_path_heighttol;
                    node->max_height = node->position.y + default_path_heighttol;
                }
                node->min_height_offset = node->min_height - node->position.y;
                node->max_height_offset = node->max_height - node->position.y;
                node->connection_count = (u8)EdFileReadChar();
                node->flags = (u8)EdFileReadChar();
                EdFileReadChar();
                node->runtime_flags = (u8)EdFileReadChar() & 0xf9;
                node->path_flags = EdFileReadShort();
                if (version >= 19)
                    node->special_route_index = (u8)EdFileReadChar();
                else {
                    EdFileReadChar();
                    node->special_route_index = 0xff;
                }
                length = (i8)EdFileReadChar();
                if (length != 0) {
                    EdFileRead(special_name, length);
                    node->has_special = (u8)(NuSpecialFind(scene, &node->special_handle, special_name, 1) != 0);
                    node->special_position.x = EdFileReadFloat();
                    node->special_position.y = EdFileReadFloat();
                    node->special_position.z = EdFileReadFloat();
                }
                if (path->connections != NULL && node->connection_count != 0) {
                    PATH_LOAD_ALLOCATE(AIPATHCNX_PC **, node->connections, cursor, end,
                                       node->connection_count * sizeof(AIPATHCNX_PC *));
                    for (row = 0; row < node->connection_count; ++row)
                        node->connections[row] = &path->connections[(i16)EdFileReadShort()];
                    if ((node->connection_count & 1) != 0)
                        EdFileReadShort();
                }
                if (version >= 5) {
                    node->route_membership_mask = (u16)EdFileReadShort();
                    node->route_boundary_mask = (u16)EdFileReadShort();
                }
                node->distance_cache_nodes[0] = 0xff;
                node->distance_cache_nodes[1] = 0xff;
            }
            AIPathCalcExtents(path);
        }
        PATH_LOAD_ALLOCATE(u8 **, path->route_matrix, cursor, end, path->node_count * sizeof(u8 *));
        for (index = 0; index < path->node_count; ++index) {
            PATH_LOAD_ALLOCATE(u8 *, path->route_matrix[index], cursor, end, path->node_count);
            EdFileRead(path->route_matrix[index], path->node_count);
        }
        if (version >= 5) {
            path->route_count = (u8)EdFileReadChar();
            if (path->route_count != 0) {
                PATH_LOAD_ALLOCATE(AIPATHROUTE_PC *, path->routes, cursor, end,
                                   path->route_count * sizeof(AIPATHROUTE_PC));
                for (route_index = 0; route_index < path->route_count; ++route_index) {
                    AIPATHROUTE_PC *route = &path->routes[route_index];
                    length = (i8)EdFileReadChar();
                    if (length != 0) {
                        PATH_LOAD_ALLOCATE(char *, route->name, cursor, end, length);
                        EdFileRead(route->name, length);
                        route->route_count = (u8)EdFileReadChar();
                        route->exit_node_count = (u8)EdFileReadChar();
                        EdFileReadChar();
                        EdFileReadChar();
                        if (path->node_count != 0 && route->route_count != 0) {
                            PATH_LOAD_ALLOCATE(u8 *, route->node_routes, cursor, end, path->node_count);
                            EdFileRead(route->node_routes, path->node_count);
                            PATH_LOAD_ALLOCATE(u8 *, route->node_directions, cursor, end, route->route_count);
                            EdFileRead(route->node_directions, route->route_count);
                            PATH_LOAD_ALLOCATE(u8 **, route->route_nodes, cursor, end,
                                               route->route_count * sizeof(u8 *));
                            for (row = 0; row < route->route_count; ++row) {
                                PATH_LOAD_ALLOCATE(u8 *, route->route_nodes[row], cursor, end, route->route_count);
                                EdFileRead(route->route_nodes[row], route->route_count);
                            }
                            if (route->exit_node_count != 0) {
                                PATH_LOAD_ALLOCATE(u8 *, route->exit_nodes, cursor, end, route->exit_node_count);
                                EdFileRead(route->exit_nodes, route->exit_node_count);
                            }
                        }
                    }
                    character_count = (i8)EdFileReadChar();
                    for (row = 0; row < character_count; ++row) {
                        length = (i8)EdFileReadChar();
                        EdFileRead(special_name, length);
                        if (SpecialRouteCharacterTypeIDFn != NULL) {
                            u8 type = (u8)SpecialRouteCharacterTypeIDFn(special_name);
                            if (type < 64) {
                                u64 mask = (u64)1 << type;
                                route->character_mask[0] |= (u32)mask;
                                route->character_mask[1] |= (u32)(mask >> 32);
                                route->character_mask[2] |= (u32)mask;
                                route->character_mask[3] |= (u32)(mask >> 32);
                            } else {
                                route->character_mask[0] = 0xffffffff;
                                route->character_mask[1] = 0xffffffff;
                                route->character_mask[3] |= 0x80000000;
                            }
                        }
                    }
                }
            }
        }
        if (version >= 19) {
            path->special_route_count = (u8)EdFileReadChar();
            if (path->special_route_count != 0) {
                AIPATHNODELINK_PC *link;
                PATH_LOAD_ALLOCATE(AIPATHNODELINK_PC *, path->special_routes, cursor, end,
                                   path->special_route_count * sizeof(AIPATHNODELINK_PC));
                link = path->special_routes;
                for (index = 0; index < path->special_route_count; ++index, ++link) {
                    link->node_index = EdFileReadUnsignedChar();
                    link->special_route_index = EdFileReadShort();
                }
            }
        }
    }
    if (version >= 19) {
        system->special_route_count = (u16)EdFileReadShort();
        if (system->special_route_count != 0) {
            AIPATHSPECIALROUTE_PC *route;
            system->special_routes = (AIPATHSPECIALROUTE_PC *)AISysBufferAlloc(
                cursor, end, system->special_route_count * sizeof(AIPATHSPECIALROUTE_PC));
            route = system->special_routes;
            for (index = 0; index < system->special_route_count; ++index, ++route) {
                route->path_count = (u8)EdFileReadChar();
                if (route->path_count != 0) {
                    PATH_LOAD_ALLOCATE(AIPATH_PC **, route->paths, cursor, end,
                                       route->path_count * sizeof(AIPATH_PC *));
                    for (row = 0; row < route->path_count; ++row)
                        route->paths[row] = system->paths[(i8)EdFileReadChar()];
                }
            }
        }
    }
    return (AIPATHSYS_PC *)system;
}

static AISYS_PC *AISysLoadPrivatePc(NUGSCN *scene, char *directory, void *pak, VARIPTR *buffer, VARIPTR *buffer_end,
                                    i32 storage_size, char *name, char *parameter, char *load_directory) {
    char ai2_path[256];
    char member_path[32];
    char script_path[128];
    char character_name[32];
    char reference_name[16];
    char special_name[256];
    AI_SYSTEM_LOAD_PC *system;
    i32 is_open;
    i32 version;
    i32 index;
    i32 search;
    sprintf(ai2_path, "%sLevels\\%s\\%s\\%s.ai2", AiLevelPathName, load_directory, parameter, name);
    PATH_LOAD_ALLOCATE(AI_SYSTEM_LOAD_PC *, system, buffer, buffer_end, sizeof(AI_SYSTEM_LOAD_PC));
    if (system == NULL)
        return NULL;
    memset(system, 0, sizeof(*system));
    system->storage_size = storage_size;
    system->storage = AISysBufferAlloc(buffer, buffer_end, storage_size);
    system->storage_end = *buffer;
    memset(system->storage, 0, storage_size);
    system->storage_cursor.void_ptr = system->storage;
    EdFileSetMedia(1);
    is_open = 0;
    if (pak != NULL) {
        sprintf(member_path, "%s.ai2", name);
        EdFileSetPakFile(pak);
        is_open = EdFileOpen(member_path, 0);
    }
    if (is_open == 0) {
        EdFileSetPakFile(NULL);
        is_open = EdFileOpen(ai2_path, 0);
    }
    if (is_open != 0) {
        version = EdFileReadInt();
        system->scene = scene;
        system->path_sys = AISysLoadPathsPc(&system->storage_cursor, version, scene, &system->storage_end);
        if (version >= 4) {
            system->area_count = EdFileReadInt();
            if (system->area_count != 0) {
                system->areas = (AI_AREA_LOAD_PC *)AISysBufferAlloc(&system->storage_cursor, &system->storage_end,
                                                                    system->area_count * sizeof(AI_AREA_LOAD_PC));
                for (index = 0; index < system->area_count; ++index) {
                    AI_AREA_LOAD_PC *area = &system->areas[index];
                    EdFileRead(area->name, 16);
                    area->minimum.x = EdFileReadFloat();
                    area->minimum.y = EdFileReadFloat();
                    area->minimum.z = EdFileReadFloat();
                    area->maximum.x = EdFileReadFloat();
                    area->maximum.y = EdFileReadFloat();
                    area->maximum.z = EdFileReadFloat();
                    area->flags = EdFileReadShort();
                    area->system = (AISYS_PC *)system;
                    if (version > 19)
                        area->game_flags = (u8)EdFileReadChar();
                    else
                        EdFileReadChar();
                    EdFileReadChar();
                }
            }
        }
        if (system->path_sys != NULL) {
            if (version >= 6) {
                system->locator_count = EdFileReadInt();
                if (system->locator_count != 0) {
                    system->locators = (AILOCATOR_PC *)AISysBufferAlloc(&system->storage_cursor, &system->storage_end,
                                                                        system->locator_count * sizeof(AILOCATOR_PC));
                    for (index = 0; index < system->locator_count; ++index) {
                        AILOCATOR_PC *locator = &system->locators[index];
                        EdFileRead(locator->name, 16);
                        locator->position.x = EdFileReadFloat();
                        locator->position.y = EdFileReadFloat();
                        locator->position.z = EdFileReadFloat();
                        locator->rotation = (i16)EdFileReadShort();
                        locator->path_info.path = system->path_sys->paths[(u8)EdFileReadChar()];
                        locator->path_info.direction = (u8)EdFileReadChar();
                        locator->path_info.connection = &locator->path_info.path->connections[(u16)EdFileReadShort()];
                        locator->path_info.distance = EdFileReadFloat();
                        locator->path_info.width = EdFileReadFloat();
                        if (version > 14)
                            locator->rotation_offset = EdFileReadInt();
                    }
                }
                if (version > 17) {
                    system->locator_set_count = EdFileReadInt();
                    if (system->locator_set_count != 0) {
                        system->locator_sets =
                            (AILOCATORSET_PC *)AISysBufferAlloc(&system->storage_cursor, &system->storage_end,
                                                                system->locator_set_count * sizeof(AILOCATORSET_PC));
                        for (index = 0; index < system->locator_set_count; ++index) {
                            AILOCATORSET_PC *set = &system->locator_sets[index];
                            EdFileRead(set->name, 16);
                            set->locator_count = (i8)EdFileReadInt();
                            set->locator_entries = (u8 *)AISysBufferAlloc(&system->storage_cursor, &system->storage_end,
                                                                          set->locator_count * sizeof(AILOCATORSET_PC));
                            for (search = 0; search < set->locator_count; ++search)
                                set->locator_entries[search] = (u8)EdFileReadChar();
                            PATH_LOAD_ALLOCATE(u8 *, set->assigned, &system->storage_cursor, &system->storage_end,
                                               set->locator_count);
                            memset(set->assigned, 0, set->locator_count);
                        }
                    }
                }
            }
            system->creature_count = EdFileReadInt();
            if (system->creature_count != 0) {
                system->creatures =
                    (AI_CREATURE_LOAD_PC *)AISysBufferAlloc(&system->storage_cursor, &system->storage_end,
                                                            system->creature_count * sizeof(AI_CREATURE_LOAD_PC));
                for (index = 0; index < system->creature_count; ++index) {
                    AI_CREATURE_LOAD_PC *creature = &system->creatures[index];
                    EdFileRead(creature->name, 16);
                    EdFileRead(creature->script_name, 16);
                    if (strlen(creature->script_name) == 0)
                        strcpy(creature->script_name, "default");
                    EdFileRead(character_name, version > 13 ? 32 : 16);
                    if (GlobalCharacterTypeIDFn != NULL)
                        creature->type = (i16)GlobalCharacterTypeIDFn(character_name);
                    creature->position.x = EdFileReadFloat();
                    creature->position.y = EdFileReadFloat();
                    creature->position.z = EdFileReadFloat();
                    creature->y_rotation = (i16)EdFileReadShort();
                    if (version > 15)
                        creature->set = (u8)EdFileReadChar();
                    creature->count = (u8)EdFileReadChar();
                    creature->count_across = (u8)EdFileReadChar();
                    creature->active_mask = EdFileReadInt();
                    creature->x_spacing = EdFileReadFloat();
                    creature->z_spacing = EdFileReadFloat();
                    creature->flags = EdFileReadInt();
                    creature->path_info.path = system->path_sys->paths[(u8)EdFileReadChar()];
                    creature->path_info.direction = (u8)EdFileReadChar();
                    creature->path_info.connection = &creature->path_info.path->connections[(u16)EdFileReadShort()];
                    if (version > 2) {
                        for (search = 0; search < 4; ++search)
                            creature->script_params[search] = EdFileReadFloat();
                    }
                    if (version >= 4 && EdFileReadInt() != 0) {
                        EdFileRead(reference_name, 16);
                        for (search = 0; search < system->area_count; ++search) {
                            if (NuStrICmp(reference_name, system->areas[search].name) == 0) {
                                creature->area = (AIAREA_PC *)&system->areas[search];
                                break;
                            }
                        }
                    }
                    if (version >= 6 && EdFileReadInt() != 0) {
                        EdFileRead(reference_name, 16);
                        for (search = 0; search < system->locator_count; ++search) {
                            if (NuStrICmp(reference_name, system->locators[search].name) == 0) {
                                creature->locator = &system->locators[search];
                                break;
                            }
                        }
                    }
                    if (version > 16 && EdFileReadInt() != 0) {
                        EdFileRead(reference_name, 16);
                        for (search = 0; search < system->locator_count; ++search) {
                            if (NuStrICmp(reference_name, system->locators[search].name) == 0) {
                                creature->respawn_locator = &system->locators[search];
                                break;
                            }
                        }
                    }
                    if (version > 7) {
                        creature->activation_difficulty = (u8)EdFileReadChar();
                        creature->min_respawn_count = EdFileReadChar();
                        creature->max_respawn_count = EdFileReadChar();
                        creature->activation_type = (u8)EdFileReadChar();
                        creature->min_respawn_time = EdFileReadFloat();
                        creature->max_respawn_time = EdFileReadFloat();
                    } else {
                        creature->activation_difficulty = AiLoadActivationDifficultyPc;
                        creature->min_respawn_count = AiLoadMinimumRespawnCountPc;
                        creature->max_respawn_count = AiLoadMaximumRespawnCountPc;
                        creature->min_respawn_time = AiLoadMinimumRespawnTimePc;
                        creature->max_respawn_time = AiLoadMaximumRespawnTimePc;
                    }
                    creature->start_stagger = version > 9 ? EdFileReadFloat() : AiLoadStartStaggerPc;
                    if (creature->activation_type == 1) {
                        EdFileRead(reference_name, 16);
                        for (search = 0; search < system->area_count; ++search) {
                            if (NuStrICmp(reference_name, system->areas[search].name) == 0) {
                                creature->activate_area = (AIAREA_PC *)&system->areas[search];
                                break;
                            }
                        }
                    }
                    if (version > 10) {
                        creature->view_distance = EdFileReadFloat();
                        creature->hear_distance = EdFileReadFloat();
                        creature->max_view_height = EdFileReadFloat();
                        creature->min_view_height = EdFileReadFloat();
                        EdFileReadInt();
                        if (GetViewRangeFn != NULL && creature->view_distance == 0.0f)
                            creature->view_distance = GetViewRangeFn(creature->type);
                        if (GetHearDistanceFn != NULL && creature->hear_distance == 0.0f)
                            creature->hear_distance = GetHearDistanceFn(creature->type);
                        if (GetMaxViewHeightFn != NULL && creature->max_view_height == 0.0f)
                            creature->max_view_height = GetMaxViewHeightFn(creature->type);
                        if (GetMinViewHeightFn != NULL && creature->min_view_height == 0.0f)
                            creature->min_view_height = GetMinViewHeightFn(creature->type);
                    } else {
                        creature->view_distance = GetViewRangeFn != NULL ? GetViewRangeFn(creature->type) : 1.0f;
                        creature->hear_distance = GetHearDistanceFn != NULL ? GetHearDistanceFn(creature->type) : 1.0f;
                        creature->max_view_height =
                            GetMaxViewHeightFn != NULL ? GetMaxViewHeightFn(creature->type) : 1.0f;
                        creature->min_view_height =
                            GetMinViewHeightFn != NULL ? GetMinViewHeightFn(creature->type) : 1.0f;
                    }
                }
            }
            if (version > 12) {
                system->antinode_count = EdFileReadInt();
                if (system->antinode_count != 0) {
                    PATH_LOAD_ALLOCATE(AI_ANTINODE_LOAD_PC *, system->antinodes, &system->storage_cursor,
                                       &system->storage_end, system->antinode_count * sizeof(AI_ANTINODE_LOAD_PC));
                    for (index = 0; index < system->antinode_count; ++index) {
                        AI_ANTINODE_LOAD_PC *antinode = &system->antinodes[index];
                        i32 length;
                        antinode->enabled = 1;
                        antinode->position.x = EdFileReadFloat();
                        antinode->position.y = EdFileReadFloat();
                        antinode->position.z = EdFileReadFloat();
                        antinode->radius = EdFileReadFloat();
                        antinode->base_radius = antinode->radius;
                        antinode->base_height = antinode->radius;
                        antinode->min_y = EdFileReadFloat();
                        antinode->max_y = EdFileReadFloat();
                        antinode->min_y_offset = antinode->min_y - antinode->position.y;
                        antinode->max_y_offset = antinode->max_y - antinode->position.y;
                        if (version > 14) {
                            antinode->flags = EdFileReadInt();
                            antinode->base_radius = EdFileReadFloat();
                            antinode->base_height = EdFileReadFloat();
                            antinode->type = (u8)EdFileReadChar();
                        } else {
                            EdFileReadChar();
                        }
                        EdFileReadChar();
                        EdFileReadChar();
                        antinode->game_flags = (u8)EdFileReadChar();
                        length = (i8)EdFileReadChar();
                        if (length != 0) {
                            EdFileRead(special_name, length);
                            antinode->has_special =
                                (u8)(NuSpecialFind(scene, &antinode->special_handle, special_name, 1) != 0);
                            antinode->special_position.x = EdFileReadFloat();
                            antinode->special_position.y = EdFileReadFloat();
                            antinode->special_position.z = EdFileReadFloat();
                            if (version > 14)
                                antinode->rotation_offset = EdFileReadInt();
                        }
                    }
                }
            }
            if (version > 6 && GameAILoadFn != NULL)
                GameAILoadFn((AISYS_PC *)system, version, scene, buffer, buffer_end);
        }
        EdFileClose();
    }
    sprintf(script_path, "%sLevels\\%s\\%s", AiLevelPathName, directory, parameter);
    AIScriptLoadAllPakFile(pak, script_path, buffer, buffer_end, (AISYS_PC *)system);
    AISysSetLevelPath((AISYS_PC *)system, NULL);
    return (AISYS_PC *)system;
}

#undef PATH_LOAD_ALLOCATE

AISYS_PC *__cdecl AISysLoadEx(VARIPTR *buffer, VARIPTR *buffer_end, i32 storage_size, NUGSCN *scene, char *directory,
                              char *name, char *parameter, char *load_directory) {
    VARIPTR pak_start = *buffer_end;
    void *pak = NULL;
    char path[128];
    if (ai_usepackfile != 0) {
        i32 pak_size;
        sprintf(path, "%sLevels\\%s\\%s\\ai.pak", AiLevelPathName, directory, parameter);
        pak_size = (i32)NuFileSize(path);
        if (pak_size > 0) {
            pak_start.addr = buffer_end->addr - ((pak_size + 16) & ~15u);
            pak = NuFilePakLoad(path, &pak_start, *buffer_end, 16);
        }
    }
    return AISysLoadPrivatePc(scene, directory, pak, buffer, buffer_end, storage_size, name, parameter, load_directory);
}

AISYS_PC *__cdecl AISysLoad(VARIPTR *buffer, VARIPTR *buffer_end, i32 storage_size, NUGSCN *scene, char *directory,
                            char *name, char *parameter) {
    return AISysLoadEx(buffer, buffer_end, storage_size, scene, directory, name, parameter, directory);
}

/* Original AISYS PC callbacks; bounded native evidence in ../native. */

typedef struct ai_context_reset_view_pc_s {
    u8 unknown_000[0x6c4];
    u8 reset_contexts;
} AI_CONTEXT_RESET_VIEW_PC;
STATIC_ASSERT(offsetof(AI_CONTEXT_RESET_VIEW_PC, reset_contexts) == 0x6c4);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1f8) == 0x1f8);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_set) == 0x378);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1010) == 0x1010);
STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);

i32 __cdecl Action_ResetContext(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    APIOBJECT_PC *found;
    char *value;
    i32 index;
    (void)processor;
    (void)delta;
    if (!first_time)
        return 1;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character=");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL) {
        Player_ClearContext(object, 1);
        Player_ResetContexts(&((AI_CONTEXT_RESET_VIEW_PC *)object)->reset_contexts);
    }
    return 1;
}

i32 __cdecl Action_CanBeCarried(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    APIOBJECT_PC *found;
    char *value;
    i32 index, enabled = 1;
    (void)processor;
    (void)delta;
    if (!first_time)
        return 1;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    for (index = 0; index < count; ++index) {
        if (NuStrICmp(params[index], "FALSE") == 0)
            enabled = 0;
        else if ((value = NuStrIStr(params[index], "character=")) != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL) {
        if (object->character_context == 0x3c || object->takeover_source != NULL)
            Player_ClearContext(object, 1);
        object->flags_ef4 = (object->flags_ef4 & ~2u) | ((u32)(enabled & 1) << 1);
    }
    return 1;
}

i32 __cdecl Action_SetFormationCommander(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet,
                                         char **unused3, i32 unused4, i32 unused5, f32 unused6) {
    GAMEOBJECT *object;
    AIGROUP_PC *group;
    i32 commander_id;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL || packet->group == NULL)
        return 1;
    object = packet->owner->objptr;
    group = packet->group;
    if (packet->group_member_index != group->member_count - 1)
        return 1;
    if (group->count_across != 1 && group->member_count % group->count_across != 1)
        return 1;
    packet->flags |= 0x1000000;
    if ((i16)object->id == id_BATTLEDROID || (i16)object->id == id_BATTLEDROIDSECURITY ||
        (i16)object->id == id_BATTLEDROIDGEONOSIAN)
        commander_id = id_BATTLEDROIDCOMMANDER;
    else if ((i16)object->id == id_CLONEEP3)
        commander_id = id_CLONEEP3SAND;
    else
        return 1;
    NewPlayerCharacter(object, commander_id, (i16)object->id, 1);
    object->current_hp = object->character_data->game_character->hitpoints;
    object->field_107e = object->character_data->game_character->hitpoints;
    return 1;
}

i32 __cdecl Action_AddMiscPickups(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 unused6) {
    i32 value = 0;
    i32 torpedo = 0;
    GAMEOBJECT *object = NULL;
    APIOBJECT_PC *named;
    NUVEC *position;
    i32 index;
    char *parameter;
    if (first_time != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < count; ++index) {
            parameter = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (parameter != NULL) {
                named = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, parameter + 10) : NULL;
                object = named != NULL ? named->owner : NULL;
            } else if ((parameter = NuStrIStr(params[index], (char *)AIAnimSpeedValueName)) != NULL)
                value = (i32)AIParamToFloat(processor, parameter + 6);
            else if ((parameter = NuStrIStr(params[index], (char *)AIActionTorpedoPrefixPc)) != NULL)
                torpedo = (i32)AIParamToFloat(processor, parameter + 8);
        }
        if (object != NULL) {
            position = &object->collision_position;
            if (position != NULL && (value != 0 || torpedo != 0))
                AddMiscPickups(position, -1, value, torpedo);
        }
    }
    return 1;
}

i32 __cdecl Action_CreateRider(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **params,
                               i32 count, i32 first_time, f32 unused6) {
    GAMEOBJECT *vehicle = NULL;
    i32 model = -1;
    char *script = NULL;
    i32 index;
    char *value;
    GAMEOBJECT *rider;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        vehicle = packet->owner->objptr;
    if (first_time != 0 && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral75772c);
            if (value != NULL) {
                if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                    model = (u8)LevelCharacterTypeIDFn(value + 5);
                    if (model != 0xff)
                        model = LevelCharacterGlobalIDFn(model);
                }
            } else if ((value = NuStrIStr(params[index], "script")) != NULL)
                script = value + 7;
        }
        if (model != -1 && vehicle->rider == NULL) {
            rider =
                AddDynamicCreature(model, &vehicle->collision_position, 0,
                                   script != NULL ? script : GizmoDefaultOutputName, NULL, NULL, 0, NULL, NULL, 0, 0);
            if (rider != NULL)
                TakeOverGameObject(rider, vehicle, 0, 1);
        }
    }
    return 1;
}

i32 __cdecl Action_TakeDamage(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *hitter = NULL, *candidate;
    APIOBJECT_PC *found, *opponent;
    char *value;
    i32 index, damage = 1, creature_set = 0;
    (void)delta;
    if (!first_time)
        return 1;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character=");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if ((value = NuStrIStr(params[index], "hitter=")) != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 7) : NULL;
            hitter = found != NULL ? found->owner : NULL;
        } else if ((value = NuStrIStr(params[index], "damage=")) != NULL) {
            damage = (i32)AIParamToFloat(processor, value + 7);
        } else if (NuStrIStr(params[index], "opponent") != NULL) {
            if (packet != NULL && packet->opponent != NULL) {
                opponent = (APIOBJECT_PC *)packet->opponent;
                if (opponent->owner != NULL) {
                    object = opponent->owner;
                    if (packet->owner != NULL && packet->owner->objptr != NULL)
                        hitter = packet->owner->objptr;
                }
            }
        } else if ((value = NuStrIStr(params[index], "set=")) != NULL) {
            creature_set = (i32)AIParamToFloat(processor, value + 4);
            if (creature_set < 0 || creature_set > 16)
                creature_set = 0;
        }
    }
    if (creature_set != 0) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc & 1) != 0 && candidate->ai_packet.creature_set == creature_set)
                ObjHitObj(hitter, candidate, damage, 0, 0, 1);
        }
    } else if (object != NULL) {
        ObjHitObj(hitter, object, damage, 0, 0, 1);
    }
    return 1;
}

i32 __cdecl Action_Kill(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params, i32 count,
                        i32 first_time, f32 delta) {
    GAMEOBJECT *object, *candidate, *excluded = NULL;
    APIOBJECT_PC *found;
    char *value;
    i32 index, creature_set = 0, all_ai = 0, check_dead = 0, parts = 0, debris = 0, respawn = 0, origin = 0;
    (void)delta;
    if (!first_time)
        return 1;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if (NuStrIStr(params[index], "opponent") != NULL) {
            if (packet->opponent != NULL)
                object = ((APIOBJECT_PC *)packet->opponent)->owner;
        } else if (NuStrICmp(params[index], "respawn") == 0) {
            respawn = 1;
        } else if (NuStrICmp(params[index], "respawn_at_origin") == 0) {
            respawn = 1;
            origin = 1;
        } else if ((value = NuStrIStr(params[index], "all_ai_except")) != NULL) {
            excluded = GetNamedGameObject(system, value + 14);
            all_ai = 1;
        } else if (NuStrICmp(params[index], "all_ai") == 0) {
            all_ai = 1;
        } else if (NuStrICmp(params[index], "check_if_dead") == 0) {
            check_dead = 1;
        } else if (NuStrICmp(params[index], "debris") == 0) {
            debris = 1;
        } else if (NuStrICmp(params[index], "parts_on") == 0) {
            parts = 1;
        } else if ((value = NuStrIStr(params[index], "set=")) != NULL) {
            creature_set = (i32)AIParamToFloat(processor, value + 4);
            if (creature_set < 0 || creature_set > 16)
                creature_set = 0;
        }
    }
    if (all_ai) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc_word & 0x1001) != 0x1001 || !(candidate->flags_1f8 & 0x400) ||
                candidate == excluded)
                continue;
            if (check_dead && (candidate->dead != 0 || !(candidate->field_1010 <= 0.0f)))
                continue;
            candidate->portal_flags =
                (candidate->portal_flags & ~0x00300000u) | ((u32)((respawn & 1) | ((origin & 1) << 1)) << 20);
            if (parts)
                KillParts(candidate, -1, -1, 1, 0.0f, 0, NULL);
            KillGameObject(candidate, debris ? 2 : 4, 0);
        }
    } else if (creature_set != 0) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc_word & 0x1001) != 0x1001 || candidate->ai_packet.creature_set != creature_set)
                continue;
            if (check_dead && (candidate->dead != 0 || !(candidate->field_1010 <= 0.0f)))
                continue;
            candidate->portal_flags =
                (candidate->portal_flags & ~0x00300000u) | ((u32)((respawn & 1) | ((origin & 1) << 1)) << 20);
            if (parts)
                KillParts(candidate, -1, -1, 1, 0.0f, 0, NULL);
            KillGameObject(candidate, debris ? 2 : 4, 0);
        }
    } else if (object != NULL && (!check_dead || (object->dead == 0 && object->field_1010 <= 0.0f))) {
        object->portal_flags =
            (object->portal_flags & ~0x00300000u) | ((u32)((respawn & 1) | ((origin & 1) << 1)) << 20);
        if (parts)
            KillParts(object, -1, -1, 1, 0.0f, 0, NULL);
        KillGameObject(object, debris ? 2 : 4, 0);
    }
    return 1;
}

void __cdecl GizObstacle_JumpToStart(GIZOBSTACLE *);
void __cdecl GizObstacle_JumpToEnd(GIZOBSTACLE *);
void __cdecl GizObstacle_PlayForwards(GIZOBSTACLE *);
void __cdecl GizObstacle_PlayBackwards(GIZOBSTACLE *);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_index) == 0x400);
i32 __cdecl Action_Activate(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *candidate;
    APIOBJECT_PC *found;
    i32 index, creature_set = 0;
    char *value;
    (void)delta;
    if (!first_time)
        return 1;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character=");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if ((value = NuStrIStr(params[index], "set=")) != NULL) {
            creature_set = (i32)AIParamToFloat(processor, value + 4);
            if (creature_set < 0 || creature_set > 16)
                creature_set = 0;
        }
    }
    if (creature_set != 0) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc & 1) && candidate->ai_packet.creature_set == creature_set) {
                if (candidate->ai_packet.creature_index == 0xff) {
                    candidate->flags_1fc_word |= 0x1000;
                    AIScriptSetBaseScriptStateByName(&candidate->ai_packet.script_processor, "Base");
                } else
                    ResetAICreature(candidate, system);
                ++aicreature_sets_alive[creature_set - 1];
            }
        }
    } else if (object != NULL) {
        if (object->ai_packet.creature_index == 0xff) {
            object->flags_1fc_word |= 0x1000;
            AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, "Base");
        } else
            ResetAICreature(object, system);
        if (object->ai_packet.creature_set != 0 && object->ai_packet.creature_set <= 16)
            ++aicreature_sets_alive[object->ai_packet.creature_set - 1];
    }
    return 1;
}

i32 __cdecl Action_RemoveThrownForceObjects(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                            char **params, i32 count, i32 first_time, f32 delta) {
    PART_PC *part = Part;
    i32 index;
    (void)system;
    (void)processor;
    (void)packet;
    (void)params;
    (void)count;
    (void)first_time;
    (void)delta;
    for (index = 0; index < MAXPARTS; ++index, ++part)
        if ((part->active & 1) && part->kill_fn == PartKill_ForceThrow)
            KillPart(part, 0);
    return 1;
}

i32 __cdecl Action_EatVictim(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 count, i32 first_time, f32 delta) {
    (void)system;
    (void)processor;
    (void)params;
    (void)count;
    (void)first_time;
    (void)delta;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        EatVictim(packet->owner->objptr);
    return 1;
}

i32 __cdecl Action_ReleaseVictim(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 count, i32 first_time, f32 delta) {
    (void)system;
    (void)processor;
    (void)params;
    (void)count;
    (void)first_time;
    (void)delta;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        ReleaseEat(packet->owner->objptr);
    return 1;
}

static i32 Action_Launch(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **unused3,
                         i32 unused4, i32 first_time, f32 unused6) {
    if (first_time && packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        StartLaunch(packet->owner->objptr);
    return 1;
}

i32 __cdecl Action_AnimTimeRandom(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    (void)system;
    (void)processor;
    (void)params;
    (void)count;
    (void)first_time;
    (void)delta;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        object = packet->owner->objptr;
        SetAnimTimeRandom(object->character_model, &object->anim_packet);
    }
    return 1;
}

i32 __cdecl Action_AlertCreatures(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    APIOBJECT_PC *found;
    char *value;
    i32 index;
    (void)processor;
    (void)delta;
    if (!first_time)
        return 1;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character=");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL)
        AlertSurroundingCreatures(object, &object->collision_position);
    return 1;
}

i32 __cdecl Action_PlayGizObstacle(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 count, i32 first_time, f32 delta) {
    GIZOBSTACLE *obstacle = NULL;
    GIZMO_PC *gizmo;
    char *value;
    i32 index, backwards = 0, stay_open = 0, stay_shut = 0, snap = 0;
    (void)system;
    (void)processor;
    (void)packet;
    (void)delta;
    if (!first_time || count == 0)
        return 1;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "name=");
        if (value != NULL) {
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, value + 5);
            if (gizmo != NULL)
                obstacle = (GIZOBSTACLE *)gizmo->object;
        } else if (NuStrICmp(params[index], "backwards") == 0)
            backwards = 1;
        else if (NuStrICmp(params[index], "stayopen") == 0)
            stay_open = 1;
        else if (NuStrICmp(params[index], "stayshut") == 0)
            stay_shut = 1;
        else if (NuStrICmp(params[index], "SNAP") == 0)
            snap = 1;
    }
    if (obstacle != NULL) {
        if (backwards || stay_shut) {
            if (snap)
                GizObstacle_JumpToStart(obstacle);
            else
                GizObstacle_PlayBackwards(obstacle);
        } else if (snap)
            GizObstacle_JumpToEnd(obstacle);
        else
            GizObstacle_PlayForwards(obstacle);
        obstacle->runtime_flags =
            (obstacle->runtime_flags & ~0xcu) | ((u32)(stay_open & 1) << 2) | ((u32)(stay_shut & 1) << 3);
    }
    return 1;
}

typedef struct ai_force_fields_pc_s {
    u8 unknown000[0x1c];
    NUVEC position;
    u8 unknown028[0x74 - 0x28];
    u32 config_flags;
    u8 unknown078[0xa0 - 0x78];
    u32 runtime_flags;
} AI_FORCE_FIELDS_PC;
typedef struct ai_force_object_pc_s {
    u8 unknown000[0x10cc];
    GIZFORCE_PC *force;
} AI_FORCE_OBJECT_PC;
STATIC_ASSERT(offsetof(AI_FORCE_FIELDS_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(AI_FORCE_FIELDS_PC, config_flags) == 0x74);
STATIC_ASSERT(offsetof(AI_FORCE_FIELDS_PC, runtime_flags) == 0xa0);
STATIC_ASSERT(offsetof(AI_FORCE_OBJECT_PC, force) == 0x10cc);
STATIC_ASSERT(offsetof(GAMEPAD_PC, runtime_flags) == 0x5a);
i32 __cdecl Action_UseForce(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    GIZFORCE_PC *force, *candidates[16];
    AI_FORCE_FIELDS_PC *view;
    GIZMO_PC *gizmo;
    char *name;
    i32 index, candidate_count = 0, triggered_by_hit = 0, valid_category;
    (void)system;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        processor->action_data_3 = NULL;
        for (index = 0; index < count; ++index) {
            if (NuStrICmp(params[index], "throwable") == 0)
                processor->action_data_1 = 1;
            else if (NuStrICmp(params[index], "inrange") == 0)
                processor->action_data_2 = 1;
            else if (NuStrICmp(params[index], "triggered_by_hit") == 0)
                triggered_by_hit = 1;
            else {
                name = NuStrIStr(params[index], "name");
                name = name != NULL ? name + 5 : params[index];
                if (name != NULL && candidate_count < 16) {
                    gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, force_gizmotype_id, name);
                    if (gizmo != NULL && gizmo->object != NULL) {
                        force = (GIZFORCE_PC *)gizmo->object;
                        candidates[candidate_count] = force;
                        view = (AI_FORCE_FIELDS_PC *)force;
                        if ((view->runtime_flags & 2) && !(view->runtime_flags & 0x10000))
                            ++candidate_count;
                    }
                }
            }
        }
        if (candidate_count != 0)
            processor->action_data_3 = candidates[qrand() / (0xffff / candidate_count + 1)];
    }
    force = (GIZFORCE_PC *)processor->action_data_3;
    if (force == NULL)
        return 1;
    view = (AI_FORCE_FIELDS_PC *)force;
    if (!(view->runtime_flags & 1))
        return 0;
    valid_category = CharCategory_IsCategory(object, (view->config_flags & 0x10) ? 1 : 0) != 0;
    if (triggered_by_hit) {
        view->runtime_flags |= 0x8000;
        return 0;
    }
    if (valid_category) {
        packet->movement_look_target = &view->position;
        object->pad->runtime_flags |= 4;
        ((AI_FORCE_OBJECT_PC *)object)->force = force;
        if (processor->action_data_1 != 0) {
            if (packet->nearest_opponent == NULL) {
                object->pad->runtime_flags &= ~4;
                ((AI_FORCE_OBJECT_PC *)object)->force = NULL;
            }
            return 1;
        }
        return GizForce_Complete(force) != 0;
    }
    if (FreePlay != 0) {
        processor->action_timer -= delta;
        if (processor->action_timer < 0.0f) {
            processor->action_timer = 0.5f;
            object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
        }
    }
    return 0;
}

typedef struct ai_cable_head_pc_s {
    GAMEOBJECT *source;
    GAMEOBJECT *target;
} AI_CABLE_HEAD_PC;
typedef struct ai_cable_object_pc_s {
    u8 unknown000[0x1040];
    CABLE_RECORD_PC *cable;
} AI_CABLE_OBJECT_PC;
STATIC_ASSERT(offsetof(AI_CABLE_HEAD_PC, source) == 0);
STATIC_ASSERT(offsetof(AI_CABLE_HEAD_PC, target) == 4);
STATIC_ASSERT(offsetof(AI_CABLE_OBJECT_PC, cable) == 0x1040);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, locator) == 0x36c);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, mover_height) == 0x3e8);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, path_info) == 0x420);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, last_path_position) == 0x438);
STATIC_ASSERT(offsetof(AILOCATOR_PC, path_info) == 0x20);
i32 __cdecl Action_EngageOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
i32 __cdecl Action_ProbeDroid(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *towing_player = NULL, *bomb = NULL;
    AI_CABLE_OBJECT_PC *view, *player_view;
    AI_CABLE_HEAD_PC *cable;
    AILOCATOR_PC *locator;
    NUVEC difference;
    f32 distance;
    char *engage_params[3];
    (void)params;
    (void)count;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    player_view = (AI_CABLE_OBJECT_PC *)player;
    if (player_view->cable != NULL) {
        cable = (AI_CABLE_HEAD_PC *)player_view->cable;
        if (cable->target != NULL && cable->target->id == id_DRAGBOMB) {
            bomb = cable->target;
            towing_player = player;
        }
    }
    if (bomb == NULL && player2 != NULL) {
        player_view = (AI_CABLE_OBJECT_PC *)player2;
        if (player_view->cable != NULL) {
            cable = (AI_CABLE_HEAD_PC *)player_view->cable;
            if (cable->target != NULL && cable->target->id == id_DRAGBOMB) {
                bomb = cable->target;
                towing_player = player2;
            }
        }
    }
    view = (AI_CABLE_OBJECT_PC *)object;
    if (bomb != NULL && towing_player != NULL) {
        if (view->cable != NULL && ((AI_CABLE_HEAD_PC *)view->cable)->target != bomb) {
            ((AI_CABLE_HEAD_PC *)view->cable)->source = NULL;
            view->cable = NULL;
        }
        if (view->cable == NULL) {
            distance = NuVecXZDistSqr(&object->collision_position, &bomb->collision_position, &difference);
            if ((f64)max_cable_length * max_cable_length * 0.5 > distance)
                view->cable = CreateCable(object, bomb, 1);
        }
        if (view->cable != NULL) {
            locator = object->ai_packet.locator;
            if (locator != NULL)
                AIMoveInstruction(packet, &locator->position, 0.0f, &locator->path_info, 1, 0.0f);
            else
                AIMoveInstruction(packet, &towing_player->ai_packet.last_path_position,
                                  towing_player->ai_packet.mover_height, &towing_player->ai_packet.path_info, 2, 99.0f);
        } else {
            AIMoveInstruction(packet, &bomb->ai_packet.last_path_position, bomb->ai_packet.mover_height,
                              &bomb->ai_packet.path_info, 1, 0.0f);
        }
    } else {
        if (view->cable != NULL) {
            ((AI_CABLE_HEAD_PC *)view->cable)->source = NULL;
            view->cable = NULL;
        }
        AIMoveInstruction(packet, NULL, 0.0f, NULL, 4, packet->movement_instruction_parameter);
    }
    engage_params[0] = "goalrange 10";
    engage_params[1] = "firerange 15";
    engage_params[2] = "fireinterval 1.5";
    Action_EngageOpponent(system, processor, packet, engage_params, 3, first_time, delta);
    return 0;
}

/* Original AISYS callback family; PC-native seven-argument callback ABI. */
extern i32 __cdecl TagCharacter(GAMEOBJECT *, GAMEOBJECT *, i32);
extern void __cdecl RegisterTakeOverObject(GAMEOBJECT *);
STATIC_ASSERT(offsetof(AIPACKET_PC, owner) == 0xd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, tag_flags) == 0x7c5);
STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);

i32 __cdecl Action_SetTaggable(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *target = NULL;
    APIOBJECT_PC *found;
    char *value;
    i32 index, disabled = 0;
    (void)processor;
    (void)delta;
    if (!first_time)
        return 1;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757ee0)) != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 7) : NULL;
            target = found != NULL ? found->owner : NULL;
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            disabled = 1;
        }
    }
    if (object != NULL) {
        if (disabled && (object->flags_1fc_word & 0x80) != 0) {
            if (object->takeover_source != NULL)
                ReleaseTakeOver(object, 0);
            else if (TagCharacter(object, target, 0) == 0)
                disabled = 0;
            SetPlayer();
        }
        object->tag_flags = (object->tag_flags & ~2U) | ((u32)disabled << 1);
    }
    return 1;
}

i32 __cdecl Action_TagCharacter(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *target = NULL;
    APIOBJECT_PC *found;
    char *value;
    i32 index;
    (void)processor;
    (void)delta;
    if (!first_time)
        return 1;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (count == 0)
        return 1;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757ee0)) != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 7) : NULL;
            target = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL) {
        TagCharacter(object, target, 0);
        SetPlayer();
    }
    return 1;
}

i32 __cdecl Action_RegisterTakeOverObject(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    APIOBJECT_PC *found;
    char *value;
    i32 index;
    (void)processor;
    (void)delta;
    if (!first_time)
        return 1;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL)
        RegisterTakeOverObject(object);
    return 1;
}

extern const char AIActionBluePrefixPc[6];
extern const char AIActionGreenPrefixPc[7];
extern const char AIActionRedPrefixPc[5];
extern const char AIActionCountPrefixPc[7];
extern const char AIActionTotalPrefixPc[7];
extern const char AIActionMyPosNamePc[6];
extern const char AIActionCanBeStolenNamePc[12];
extern const char AIActionTorpedoNamePc[8];

typedef struct torpedo_action_packet_pc_s {
    u8 count;
    u8 flags;
    u8 unknown_02;
    u8 state;
    u8 unknown_04[4];
    f32 timer;
    u8 unknown_0c[0x30];
    NUVEC pickup_positions[5];
    u8 unknown_78[0x10];
} TORPEDO_ACTION_PACKET_PC;
STATIC_ASSERT(sizeof(TORPEDO_ACTION_PACKET_PC) == 0x88);
STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, count) == 0);
STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, flags) == 1);
STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, state) == 3);
STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, timer) == 8);
STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, pickup_positions) == 0x3c);

i32 __cdecl Action_AddGameMsgCount(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_count, i32 first_time, f32 unused6) {
    GAMEOBJECT *object;
    NUVEC *position;
    i32 count = 0;
    i32 total = 0;
    u8 red = 0;
    u8 green = 0;
    u8 blue = 0;
    i32 index;
    char *value;
    GIZAIMESSAGE *message;
    position = NULL;
    object = NULL;
    if (first_time != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(AIActionMyPosNamePc, params[index]) == 0 && object != NULL)
                position = &object->collision_position;
            else if ((value = NuStrIStr(params[index], (char *)AIActionTotalPrefixPc)) != NULL)
                total = (i32)AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], (char *)AIActionCountPrefixPc)) != NULL)
                count = (i32)AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757c20)) != NULL) {
                message = CheckGizAIMessage(gizaimessagesys, value + 11, NULL);
                if (message != NULL)
                    count = (i32)message->value;
            } else if ((value = NuStrIStr(params[index], (char *)AIActionRedPrefixPc)) != NULL)
                red = (u8)AIParamToFloat(processor, value + 4);
            else if ((value = NuStrIStr(params[index], (char *)AIActionGreenPrefixPc)) != NULL)
                green = (u8)AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], (char *)AIActionBluePrefixPc)) != NULL)
                blue = (u8)AIParamToFloat(processor, value + 5);
        }
        if (count != 0 && count <= total)
            AddGameMsgCount(position, count, total, red, green, blue, 0.75f);
    }
    return 1;
}

i32 __cdecl Action_AddTorpedoPacket(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **params,
                                    i32 param_count, i32 first_time, f32 unused6) {
    GAMEOBJECT *object = NULL;
    i32 count = 0;
    u8 flags = 0;
    i32 index;
    char *value;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    if (first_time != 0) {
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionTorpedoNamePc);
            if (value != NULL) {
                count = (i32)AIParamToFloat(&packet->script_processor, value + 8);
                if (count < 0)
                    count = 0;
                else if (count > getMaxTorpedos(NULL))
                    count = getMaxTorpedos(NULL);
            } else if (NuStrICmp(params[index], AIActionCanBeStolenNamePc) == 0)
                flags |= 0x20;
        }
    }
    if (object != NULL) {
        if (object->torpedo != NULL)
            FreeTorpedoPacket(&object->torpedo);
        object->torpedo = GetTorpedoPacket();
        if (object->torpedo != NULL && count != 0) {
            ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->count = (u8)count;
            ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->flags |= flags;
            for (index = 0; index < count; ++index) {
                ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->pickup_positions[index] = object->collision_position;
                ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->timer = 0.8f;
                ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->state = 0;
            }
        }
    }
    return 1;
}

/* Native 0x00517fd0, 105 bytes. */
AIGROUP_PC *__cdecl CreateAIGroup(AISYS_PC *system, i32 count_across, f32 x_spacing, f32 z_spacing, f32 max_speed) {
    AIGROUP_PC *group;
    i32 index;
    if (system == NULL || count_across == 0)
        return NULL;
    for (index = 0; index < 16; ++index) {
        group = &system->groups[index];
        if ((group->flags & 1) == 0) {
            group->flags |= 1;
            group->flags &= ~8u;
            group->x_spacing = x_spacing;
            group->z_spacing = z_spacing;
            group->count_across = (u8)count_across;
            group->max_speed = max_speed;
            return group;
        }
    }
    return NULL;
}

f32 __cdecl Condition_TimeOffPathPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                    char *parameter, void *argument) {
    if (packet != NULL)
        return packet->time_off_path;
    return 0.0f;
}

f32 __cdecl Condition_InterruptIDPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                    char *parameter, void *argument) {
    if (processor != NULL)
        return (f32)processor->interrupt_id;
    return -1.0f;
}

f32 __cdecl Condition_StuckTimePc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char *parameter,
                                  void *argument) {
    if (packet != NULL && packet->owner != NULL)
        return packet->owner->respawn_timer;
    return 0.0f;
}

f32 __cdecl Condition_ParamPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char *parameter,
                              void *argument) {
    return AIParamToFloatEx(packet, processor, parameter);
}

f32 __cdecl Condition_PlayerToOriginPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char *parameter, void *argument) {
    if (packet != NULL && system != NULL && system->player_1 != NULL) {
        NUVEC *origin;
        if (GetOriginPositionFn != NULL && (origin = GetOriginPositionFn(system, packet)) != NULL)
            return NuVecDist(&system->player_1->position, origin, NULL);
        if (packet->creature_index != 0xff)
            return NuVecDist(&system->player_1->position, &system->creatures[packet->creature_index].position, NULL);
    }
    return FLT_MAX;
}

STATIC_ASSERT(offsetof(AISYS_PC, player_1) == 0x138c);
STATIC_ASSERT(offsetof(AISYS_PC, player_2) == 0x1390);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator) == 0xa4);
STATIC_ASSERT(offsetof(AILOCATOR_PC, position) == 0x10);
STATIC_ASSERT(offsetof(APIOBJECT_PC, position) == 0x5c);

void *__cdecl Condition_PlayerToLocatorInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return AIPathFindLocator(system, name);
    return NULL;
}

f32 __cdecl Condition_PlayerToLocatorPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    NUVEC difference;
    if (packet != NULL && system != NULL && system->player_1 != NULL) {
        AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL)
            return NuVecDist(&system->player_1->position, &locator->position, &difference);
    }
    return FLT_MAX;
}

f32 __cdecl Condition_NearestPlayerToLocatorPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                               char *name, void *argument) {
    NUVEC difference;
    f32 distance = FLT_MAX;
    if (packet != NULL && system != NULL) {
        AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL) {
            if (system->player_1 != NULL)
                distance = NuVecDistSqr(&system->player_1->position, &locator->position, &difference);
            if (system->player_2 != NULL) {
                f32 second = NuVecDistSqr(&system->player_2->position, &locator->position, &difference);
                if (second < distance)
                    distance = second;
            }
        }
        if ((f64)distance != (f64)FLT_MAX)
            distance = NuFsqrt(distance);
    }
    return distance;
}

typedef struct condition_trigger_process_pc_s {
    u8 unknown_000[0xa0];
    AIAREA_PC *trigger_area;
} CONDITION_TRIGGER_PROCESS_PC;
STATIC_ASSERT(offsetof(CONDITION_TRIGGER_PROCESS_PC, trigger_area) == 0xa0);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, unknown_a0) == 0xa0);
STATIC_ASSERT(sizeof(AIAREA_PC) == 0x40);
STATIC_ASSERT(offsetof(AIAREA_PC, system) == 0x38);
STATIC_ASSERT(offsetof(AISYS_PC, areas) == 0x38);
STATIC_ASSERT(offsetof(AISYS_PC, player_1) == 0x138c);
STATIC_ASSERT(offsetof(AIPACKET_PC, owner) == 0xd0);
STATIC_ASSERT(offsetof(AIPACKET_PC, opponent) == 0xe4);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_area_mask) == 0x2b0);

void *__cdecl Condition_InTriggerAreaInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return AISysFindArea(system, name);
    return NULL;
}

f32 __cdecl Condition_InTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (packet != NULL && packet->owner != NULL) {
        GAMEOBJECT *object = packet->owner;
        AIAREA_PC *area = (AIAREA_PC *)argument;
        if (area == NULL)
            area = ((CONDITION_TRIGGER_PROCESS_PC *)process)->trigger_area;
        if (area != NULL && area->system != NULL) {
            i32 index = (i32)(area - area->system->areas);
            i64 mask = (i32)(1u << (index & 31));
            u64 membership = ((u64)object->ai_area_mask[1] << 32) | object->ai_area_mask[0];
            if ((membership & mask) != 0)
                return 1.0f;
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_PlayerInTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                            char *name, void *argument) {
    if (system != NULL && system->player_1 != NULL && system->areas != NULL) {
        GAMEOBJECT *object = (GAMEOBJECT *)system->player_1;
        AIAREA_PC *area = (AIAREA_PC *)argument;
        if (area == NULL)
            area = ((CONDITION_TRIGGER_PROCESS_PC *)process)->trigger_area;
        if (area != NULL && area->system != NULL) {
            i32 index = (i32)(area - area->system->areas);
            i64 mask = (i32)(1u << (index & 31));
            u64 membership = ((u64)object->ai_area_mask[1] << 32) | object->ai_area_mask[0];
            if ((membership & mask) != 0)
                return 1.0f;
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_OpponentInTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                              char *name, void *argument) {
    if (packet != NULL && packet->opponent != NULL && packet->owner != NULL) {
        GAMEOBJECT *object = packet->opponent;
        AIAREA_PC *area = (AIAREA_PC *)argument;
        if (area == NULL)
            area = ((CONDITION_TRIGGER_PROCESS_PC *)process)->trigger_area;
        if (area != NULL && area->system != NULL) {
            i32 index = (i32)(area - area->system->areas);
            i64 mask = (i32)(1u << (index & 31));
            u64 membership = ((u64)object->ai_area_mask[1] << 32) | object->ai_area_mask[0];
            if ((membership & mask) != 0)
                return 1.0f;
        }
    }
    return 0.0f;
}

STATIC_ASSERT(offsetof(AIPACKET_PC, opponent) == 0xe4);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator) == 0xa4);
STATIC_ASSERT(offsetof(AILOCATOR_PC, position) == 0x10);
STATIC_ASSERT(offsetof(GAMEOBJECT, position) == 0x5c);

void *__cdecl Condition_OpponentToLocatorInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return AIPathFindLocator(system, name);
    return NULL;
}

f32 __cdecl Condition_OpponentToLocatorPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    NUVEC difference;
    if (packet != NULL && packet->opponent != NULL) {
        AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL)
            return NuVecDist(&packet->opponent->position, &locator->position, &difference);
    }
    return FLT_MAX;
}

f32 __cdecl Condition_OpponentToLocatorXZPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                            char *name, void *argument) {
    NUVEC difference;
    if (packet != NULL && packet->opponent != NULL) {
        AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL)
            return NuVecXZDist(&packet->opponent->position, &locator->position, &difference);
    }
    return FLT_MAX;
}

f32 __cdecl Condition_OpponentToLocatorYPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    if (packet != NULL && packet->opponent != NULL) {
        AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL)
            return packet->opponent->position.y - locator->position.y;
    }
    return FLT_MAX;
}

typedef struct AI_EDITOR_CREATURE_PATH_SAVE_PC {
    u8 unknown_00[0x38];
    i8 draw_index;
} AI_EDITOR_CREATURE_PATH_SAVE_PC;
STATIC_ASSERT(offsetof(AI_EDITOR_CREATURE_PATH_SAVE_PC, draw_index) == 0x38);

typedef struct AI_EDITOR_CREATURE_SAVE_PC {
    NULISTLNK link;
    char name[16];
    char script_name[16];
    NUVEC position;
    u16 angle;
    u16 unknown_36;
    EDAIPATHCHECK_s check;
    u32 valid_positions;
    i16 character_type;
    u8 set;
    u8 group_count;
    u8 across_count;
    u8 unknown_5d[3];
    f32 x_spacing;
    f32 z_spacing;
    u32 flags;
    void *activation_area;
    f32 script_params[4];
    void *trigger_area;
    void *locator;
    void *respawn_locator;
    u8 difficulty;
    u8 min_respawns;
    u8 max_respawns;
    u8 activation;
    f32 min_respawn_time;
    f32 max_respawn_time;
    f32 stagger_start;
    f32 view_distance;
    f32 hear_distance;
    f32 max_view_height;
    f32 min_view_height;
} AI_EDITOR_CREATURE_SAVE_PC;
STATIC_ASSERT(sizeof(AI_EDITOR_CREATURE_SAVE_PC) == 0xac);
STATIC_ASSERT(offsetof(AI_EDITOR_CREATURE_SAVE_PC, check.path) == 0x3c);
STATIC_ASSERT(offsetof(AI_EDITOR_CREATURE_SAVE_PC, character_type) == 0x58);
STATIC_ASSERT(offsetof(AI_EDITOR_CREATURE_SAVE_PC, script_params) == 0x70);
STATIC_ASSERT(offsetof(AI_EDITOR_CREATURE_SAVE_PC, view_distance) == 0x9c);
STATIC_ASSERT(offsetof(AI_EDITOR_CREATURE_SAVE_PC, min_view_height) == 0xa8);
extern char *(__cdecl *GlobalCharacterNameFn)(i32);

void __cdecl creatureEditorSaveData(AIPATHSYS_PC *system) {
    AI_EDITOR_CREATURE_SAVE_PC *creature;
    i32 count = 0;
    if (GlobalCharacterNameFn == NULL) {
        EdFileWriteInt(0);
        return;
    }
    for (creature = (AI_EDITOR_CREATURE_SAVE_PC *)NuLinkedListGetHead((NULISTHDR *)((u8 *)aieditor + 0x371a4));
         creature != NULL; creature = (AI_EDITOR_CREATURE_SAVE_PC *)NuLinkedListGetNext(
                               (NULISTHDR *)((u8 *)aieditor + 0x371a4), &creature->link)) {
        if (creature->check.path != NULL && GlobalCharacterNameFn(creature->character_type) != NULL)
            count++;
    }
    EdFileWriteInt(count);
    for (creature = (AI_EDITOR_CREATURE_SAVE_PC *)NuLinkedListGetHead((NULISTHDR *)((u8 *)aieditor + 0x371a4));
         creature != NULL; creature = (AI_EDITOR_CREATURE_SAVE_PC *)NuLinkedListGetNext(
                               (NULISTHDR *)((u8 *)aieditor + 0x371a4), &creature->link)) {
        char *character_name = GlobalCharacterNameFn(creature->character_type);
        AIPATH_PC *path;
        i32 index, connection_index = 0, direction = 0;
        f64 saved;
        if (creature->check.path == NULL || character_name == NULL)
            continue;
        EdFileWrite(creature->name, 16);
        EdFileWrite(creature->script_name, 16);
        EdFileWrite(character_name, aidata_version > 13 ? 32 : 16);
        EdFileWriteFloat(creature->position.x);
        EdFileWriteFloat(creature->position.y);
        EdFileWriteFloat(creature->position.z);
        EdFileWriteShort(creature->angle);
        if (aidata_version > 15)
            EdFileWriteChar(creature->set);
        EdFileWriteChar(creature->group_count);
        EdFileWriteChar(creature->across_count);
        EdFileWriteInt(creature->valid_positions);
        EdFileWriteFloat(creature->x_spacing);
        EdFileWriteFloat(creature->z_spacing);
        EdFileWriteInt(creature->flags);
        EdFileWriteChar((u8)((AI_EDITOR_CREATURE_PATH_SAVE_PC *)creature->check.path)->draw_index);
        path = system->paths[((AI_EDITOR_CREATURE_PATH_SAVE_PC *)creature->check.path)->draw_index];
        for (index = 0; index < path->connection_count; index++) {
            AIPATHCNX_PC *connection = &path->connections[index];
            i32 first_index = creature->check.first->index;
            if (connection->node_indices[0] == first_index &&
                (u16)connection->node_indices[1] == (u16)creature->check.second->index) {
                i32 angle = creature->check.angle;
                i32 absolute = angle < 0 ? (i32)(0u - (u32)angle) : angle;
                connection_index = index;
                direction = absolute >= 0x4000;
                break;
            }
            if (connection->node_indices[0] == creature->check.second->index &&
                connection->node_indices[1] == first_index) {
                i32 angle = creature->check.angle;
                i32 absolute = angle < 0 ? (i32)(0u - (u32)angle) : angle;
                connection_index = index;
                direction = absolute < 0x4000;
                break;
            }
        }
        EdFileWriteChar(direction);
        EdFileWriteShort(connection_index);
        EdFileWriteFloat(creature->script_params[0]);
        EdFileWriteFloat(creature->script_params[1]);
        EdFileWriteFloat(creature->script_params[2]);
        EdFileWriteFloat(creature->script_params[3]);
        if (creature->trigger_area != NULL) {
            EdFileWriteInt(1);
            EdFileWrite((u8 *)creature->trigger_area + 8, 16);
        } else {
            EdFileWriteInt(0);
        }
        if (creature->locator != NULL) {
            EdFileWriteInt(1);
            EdFileWrite((u8 *)creature->locator + 8, 16);
        } else {
            EdFileWriteInt(0);
        }
        if (aidata_version > 16) {
            if (creature->respawn_locator != NULL) {
                EdFileWriteInt(1);
                EdFileWrite((u8 *)creature->respawn_locator + 8, 16);
            } else {
                EdFileWriteInt(0);
            }
        }
        if (aidata_version > 7) {
            EdFileWriteChar(creature->difficulty);
            EdFileWriteChar(creature->min_respawns);
            EdFileWriteChar(creature->max_respawns);
            EdFileWriteChar(creature->activation);
            EdFileWriteFloat(creature->min_respawn_time);
            EdFileWriteFloat(creature->max_respawn_time);
            if (aidata_version > 9)
                EdFileWriteFloat(creature->stagger_start);
            if (creature->activation == 1)
                EdFileWrite((u8 *)creature->activation_area + 8, 16);
        }
        if (aidata_version > 10) {
            if (GetViewRangeFn != NULL &&
                (saved = creature->view_distance, GetViewRangeFn(creature->character_type) == saved))
                EdFileWriteFloat(0.0f);
            else
                EdFileWriteFloat(creature->view_distance);
            if (GetHearDistanceFn != NULL &&
                (saved = creature->hear_distance, GetHearDistanceFn(creature->character_type) == saved))
                EdFileWriteFloat(0.0f);
            else
                EdFileWriteFloat(creature->hear_distance);
            if (GetMaxViewHeightFn != NULL &&
                (saved = creature->max_view_height, GetMaxViewHeightFn(creature->character_type) == saved))
                EdFileWriteFloat(0.0f);
            else
                EdFileWriteFloat(creature->max_view_height);
            if (GetMinViewHeightFn != NULL &&
                (saved = creature->min_view_height, GetMinViewHeightFn(creature->character_type) == saved))
                EdFileWriteFloat(0.0f);
            else
                EdFileWriteFloat(creature->min_view_height);
            EdFileWriteInt(0);
        }
    }
}
