#include "saga.h"
#include <float.h>
#include <math.h>

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
    u8 connection_index;

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
            f32 radius_extent;

            if (other_index == (u8)node_index) {
                other_index = connection->node_indices[1];
            }
            other = &path->nodes[other_index];

            if (connection->node_indices[0] == (u8)node_index)
                connection->horizontal_distance = NuVecXZDist(&other->position, &node->position, &delta);
            else
                connection->horizontal_distance = NuVecXZDist(&node->position, &other->position, &delta);
            connection->rotation = (i16)(NuAtan2(delta.x, delta.z) * AIPathAngularScale);

            radius_extent = node->radius + connection->max_horizontal_distance + other->radius;
            if (connection->max_horizontal_distance != 0.0f) {
                if (radius_extent <= connection->horizontal_distance) {
                    connection->traversal_flags[0] &= 0xf7ffffffu;
                    connection->traversal_flags[1] &= 0xf7ffffffu;
                } else {
                    connection->traversal_flags[0] |= 0x08000000u;
                    connection->traversal_flags[1] |= 0x08000000u;
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
    f32 radius = packet->fallback_stopping_distance > packet->movement_parameter ? packet->fallback_stopping_distance
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
    NUVEC offset;
    NUVEC rotated;
    NUVEC centre;
    i32 node_index;
    i32 direction_node;
    i32 destination_node;
    u8 connection_index;
    i32 current_angle;
    i32 next_angle;
    NUVEC movement;
    i32 angle;
    i32 difference;
    i32 step;
    f32 distance;
    AIPATHCNX_PC *connection;
    AIPATHINFO_PC *destination;
    AIPATHCNX_PC *next;
    AIPATHNODE_PC *nodes;
    AIPATHNODE_PC *node;
    APIOBJECT_PC *object;
    connection = row->path_info.connection;
    if (connection == NULL)
        return 0;
    node_index = connection->node_indices[row->path_info.direction == 0];
    direction_node = node_index;
    if (row->next_connection == NULL) {
        if (previous != NULL) {
            row->next_connection = previous->path_info.connection == connection ? previous->next_connection
                                                                                : previous->path_info.connection;
        } else if (group->leader != NULL) {
            destination = &group->leader->ai->script_destination_path_info;
            if (destination->path == row->path_info.path) {
                destination_node = destination->connection->node_indices[destination->distance > 0.5f];
                direction_node = destination_node;
                connection_index = destination->path->route_matrix[node_index][destination_node];
                if (connection_index != 0xff) {
                    row->next_connection = destination->path->nodes[node_index].connections[connection_index];
                    if (row->next_connection->traversal_flags[node_index != row->next_connection->node_indices[0]] != 0)
                        row->next_connection = NULL;
                }
            }
        }
        next = row->next_connection;
        if (next != NULL && next->traversal_flags[direction_node != next->node_indices[0]] == 0) {
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
    if (row->next_connection == NULL) {
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
        step = group->rotation_speed;
        if (difference <= step) {
            step = -step;
            if (difference >= step)
                step = difference;
        }
        row->rotation = NuAngAdd(step, row->rotation);
        movement.x = movement.y = movement.z = 0.0f;
        if (GetCharacterGoalSpeedFn != NULL)
            movement.z = GetCharacterGoalSpeedFn(leader);
        NuVecRotateY(&movement, &movement, row->rotation);
        NuVecAdd(&row->position, &row->position, &movement);
        row->position.y = leader->position.y;
        return 0;
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
        if ((packet->navigation_flags & 4) != 0) {
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
                    for (end = 0; end < 2; ++end) {
                        node = &path->nodes[destination_connection->node_indices[end]];
                        if ((((u64)1 << packet->current_route) &
                             (node->route_membership_mask & ~node->route_boundary_mask)) != 0) {
                            destination_distances[end] = FLT_MAX;
                        }
                    }
                }
                if ((connection->traversal_flags[packet->path_info.direction] & 0x40000000) == 0) {
                    {
                        candidate = FLT_MAX;
                        if (destination_distances[0] != FLT_MAX &&
                            endpoint_distances[packet->path_info.direction == 0] != FLT_MAX) {
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
                            goal_index = destination_connection->node_indices[0];
                            direction = packet->path_info.direction;
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
                if ((connection->traversal_flags[packet->path_info.direction == 0] & 0x40000000) == 0) {
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
                            goal_index = destination_connection->node_indices[0];
                            direction = packet->path_info.direction == 0;
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
                                candidate_4 +=
                                    destination_distances[1] + endpoint_distances[packet->path_info.direction];
                            }
                        }
                        if (candidate_4 < best_distance) {
                            best_distance = candidate_4;
                            goal_index = destination_connection->node_indices[1];
                            direction = packet->path_info.direction == 0;
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
    if (current_2->node_indices[0] == destination_connection->node_indices[0]) {
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
