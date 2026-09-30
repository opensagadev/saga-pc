#include "saga.h"
#include <float.h>

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

void *__cdecl AIPAthFindPathCnx(AISYS_PC *system, AIPATH_PC *path, char *from_name, char *to_name, i32 *direction) {
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

void __cdecl AISysCharacterSetPathCnx(AIPACKET_PC *packet, NUVEC *position, AIPATHCNX_PC *connection, i32 direction) {
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
