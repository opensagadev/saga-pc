#include "saga.h"

enum {
    AIPATHINFO_FLAG_ON_PATH_LOCAL = 0x01,
    AIPATHINFO_FLAG_ROUTE_CHECKED_LOCAL = 0x04,
    AIPATH_CONNECTION_FLAG_RESELECT_ROUTE_LOCAL = 0x98000000u
};

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

i32 __cdecl WithinConnection(void *context, NUVEC *position, AIPATH_PC *path, AIPATHCNX_PC *connection,
                             i32 reserved_check, AIPATHCNX_PC *previous_connection, i32 route_index,
                             i32 reserved_ground, AIPATHINFO_PC *path_info, f32 radius, i32 update_once) {
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
