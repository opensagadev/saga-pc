#define _CRT_SECURE_NO_DEPRECATE // NOLINT(readability-identifier-naming): VC8 CRT selector.
#include "saga.h"
#include <float.h>
#include <string.h>
extern "C" const f32 InitOnceHalf;

// Private PC path-build fragment. Original native filename is unknown.
// Native entry and private dependency group; the original source language is unknown.
struct AI_PATH_BUILD_PATH_PC {
    NULISTLNK link;
    char name[16];
    NULISTHDR nodes;
    u8 unknown20[0xc];
    u8 flags;
    u8 unknown2d[0xb];
    i8 draw_index;
    u8 unknown39[7];
    EDAIPATHROUTE_s routes[16];
};
STATIC_ASSERT(sizeof(AI_PATH_BUILD_PATH_PC) == 0x240);
STATIC_ASSERT(offsetof(AI_PATH_BUILD_PATH_PC, draw_index) == 0x38);
STATIC_ASSERT(offsetof(AI_PATH_BUILD_PATH_PC, routes) == 0x40);
struct AI_PATH_BUILD_SYSTEM_PC {
    u8 path_count;
    u8 unknown1;
    u16 special_route_count;
    AIPATH_PC **paths;
    AIPATH_PC *active_path;
    AIPATHSPECIALROUTE_PC *special_routes;
};
STATIC_ASSERT(sizeof(AI_PATH_BUILD_SYSTEM_PC) == 0x10);
STATIC_ASSERT(offsetof(AI_PATH_BUILD_SYSTEM_PC, special_route_count) == 2);
struct AI_PATH_BUILD_EDITOR_PC {
    u8 unknown00[0x64];
    AISYS_PC *ai_system;
    u8 unknown68[0x4800];
    AI_PATH_BUILD_PATH_PC *current_path;
    NULISTHDR free_paths;
    NULISTHDR paths;
    u8 unknown487c[0x2d310];
    NULISTHDR shared_path_nodes;
};
STATIC_ASSERT(offsetof(AI_PATH_BUILD_EDITOR_PC, current_path) == 0x4868);
STATIC_ASSERT(offsetof(AI_PATH_BUILD_EDITOR_PC, paths) == 0x4874);
STATIC_ASSERT(offsetof(AI_PATH_BUILD_EDITOR_PC, shared_path_nodes) == 0x31b8c);
STATIC_ASSERT(sizeof(((AIPATHROUTE_PC *)0)->character_mask) == 16);
STATIC_ASSERT(offsetof(AIPATHROUTE_PC, character_mask) == 0x18);
STATIC_ASSERT(sizeof(EDAIPATHNODE_s) == 0xb4);
STATIC_ASSERT(offsetof(EDAIPATHNODE_s, connections) == 0x30);
STATIC_ASSERT(offsetof(EDAIPATHNODE_s, index) == 0x90);
STATIC_ASSERT(offsetof(EDAIPATHNODE_s, route_mask) == 0x96);
STATIC_ASSERT(offsetof(EDAIPATHNODE_s, special) == 0x98);
#define PATH_EDITOR_BUILD_STATE ((AI_PATH_BUILD_EDITOR_PC *)aieditor)
static i32 iterator_count = 0;
static f32 ***distance_tables = NULL;

static void pathEditorCalcRouteIterator(AIPATH_PC *path, f32 *distances, u8 *visited, i32 previous, i32 current,
                                        f32 distance, i32 route_mask) {
    ++iterator_count;
    if (visited[current / 8] & (1 << (current % 8)))
        return;
    visited[current / 8] |= 1 << (current % 8);
    AIPATHNODE_PC *node = &path->nodes[current];
    NUVEC difference;
    f32 new_distance = NuVecDist(&path->nodes[previous].position, &node->position, &difference) + distance;
    if (new_distance <= distances[current]) {
        distances[current] = new_distance;
        for (i32 index = 0; index < node->connection_count; ++index) {
            AIPATHCNX_PC *connection = node->connections[index];
            i32 direction = connection->node_indices[0] != current;
            if ((connection->traversal_flags[direction] & 0x40000000) == 0 &&
                (route_mask == 0 || (connection->route_mask & route_mask) != 0)) {
                pathEditorCalcRouteIterator(path, distances, visited, current, connection->node_indices[!direction],
                                            new_distance, route_mask);
            }
        }
    }
    visited[current / 8] &= ~(1 << (current % 8));
}

static f32 **pathEditorCalculateDistanceTable(AIPATH_PC *path, i32 route_mask, VARIPTR *cursor, VARIPTR *end) {
    if (distance_tables == NULL || path == NULL || path->nodes == NULL)
        return NULL;
    f32 **table = (f32 **)AISysBufferAlloc(cursor, end, path->node_count * sizeof(f32 *));
    if (table != NULL) {
        memset(table, 0, path->node_count * sizeof(f32 *));
        for (i32 row = 0; row < path->node_count; ++row) {
            f32 *distances = (f32 *)AISysBufferAlloc(cursor, end, path->node_count * sizeof(f32));
            table[row] = distances;
            for (i32 column = 0; column < path->node_count; ++column)
                distances[column] = FLT_MAX;
        }
        for (i32 row = 0; row < path->node_count; ++row) {
            u8 visited[32];
            memset(visited, 0, sizeof(visited));
            pathEditorCalcRouteIterator(path, table[row], visited, row, row, 0.0f, route_mask);
        }
    }
    return table;
}

static void pathEditorPopulateRouteMatrix(AIPATH_PC *path, i32 route_mask, f32 **distances, u8 **matrix) {
    if (distance_tables == NULL || path == NULL || path->nodes == NULL || distances == NULL)
        return;
    for (i32 source = 0; source < path->node_count; ++source) {
        AIPATHNODE_PC *node = &path->nodes[source];
        for (i32 destination = 0; destination < path->node_count; ++destination) {
            matrix[source][destination] = 0xff;
            if (source == destination)
                continue;
            f32 best = FLT_MAX;
            for (i32 edge = 0; edge < path->nodes[source].connection_count; ++edge) {
                AIPATHCNX_PC *connection = node->connections[edge];
                i32 direction = connection->node_indices[0] != source;
                if (connection->traversal_flags[direction] & 0x40000000)
                    continue;
                if (route_mask != 0 && !(connection->route_mask & route_mask))
                    continue;
                i32 neighbor = connection->node_indices[!direction];
                f32 distance = (f32)(distances[neighbor][destination] + distances[source][neighbor]);
                if (distance < best) {
                    best = distance;
                    matrix[source][destination] = edge;
                }
            }
        }
    }
}

// Native inlined AISysBufferAlloc blocks retain the failure diagnostic and
// publication after the allocation memset. Size arithmetic remains unsigned32.
#define PATH_BUILD_ALLOCATE(type, destination, input_cursor, input_end, input_size)                                    \
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

static void pathEditorCreateSpecialRouteData(AIPATH_PC *path, AI_PATH_BUILD_PATH_PC *editor_path, VARIPTR *cursor,
                                             VARIPTR *end) {
    i32 route_indices[16] = {};
    u8 route_members[16][32] = {};
    for (i32 slot = 0; slot < 16; ++slot) {
        if (editor_path->routes[slot].flags & 1) {
            route_indices[slot] = path->route_count;
            path->route_count++;
        }
    }
    if (path->route_count == 0) {
    } else {
        PATH_BUILD_ALLOCATE(AIPATHROUTE_PC *, path->routes, cursor, end, path->route_count * sizeof(AIPATHROUTE_PC));
        memset(path->routes, 0, path->route_count * sizeof(AIPATHROUTE_PC));
        for (i32 slot = 0; slot < 16; ++slot) {
            if (!(editor_path->routes[slot].flags & 1)) {
                continue;
            }
            AIPATHROUTE_PC *route = &path->routes[route_indices[slot]];
            i32 name_length = strlen(editor_path->routes[slot].name);
            if ((name_length != 0)) {
                PATH_BUILD_ALLOCATE(char *, route->name, cursor, end, name_length + 1);
                strcpy(route->name, editor_path->routes[slot].name);
            }
            u32 *user_mask = (u32 *)&editor_path->routes[slot].user_mask;
            route->character_mask[0] = user_mask[0];
            u32 high_mask = user_mask[1];
            route->character_mask[1] = high_mask;
            if (high_mask & 0x80000000u) {
                route->character_mask[0] = 0xffffffffu;
                route->character_mask[1] = 0xffffffffu;
            }
            route->character_mask[2] = user_mask[0];
            route->character_mask[3] = user_mask[1];
        }

        for (EDAIPATHNODE_s *editor_node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&editor_path->nodes);
             editor_node != NULL;
             editor_node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&editor_path->nodes, &editor_node->link)) {
            AIPATHNODE_PC *runtime_node = &path->nodes[editor_node->index];
            if (editor_node->route_mask != 0) {
                for (i32 slot = 0; slot < 16; ++slot) {
                    if (editor_node->route_mask & (((u64)1) << slot)) {
                        runtime_node->route_boundary_mask |= ((u64)1) << route_indices[slot];
                    }
                }
            }
            for (i32 slot = 0; slot < 8; ++slot) {
                EDAIPATHCNX_s *editor_connection = &editor_node->connections[slot];
                if (editor_connection->node == NULL || editor_connection->route_mask == 0) {
                    continue;
                }
                if (runtime_node->connection_count == 0) {
                    continue;
                }
                AIPATHCNX_PC **connections = runtime_node->connections;
                for (i32 edge = 0; edge < runtime_node->connection_count; ++edge) {
                    const i16 other_index = editor_connection->node->index;
                    AIPATHCNX_PC *connection = connections[edge];
                    if (connection->node_indices[0] != other_index && connection->node_indices[1] != other_index) {
                        continue;
                    }
                    for (i32 route_slot = 0; route_slot < 16; ++route_slot) {
                        if (editor_connection->route_mask & (1u << route_slot)) {
                            i32 route_index = route_indices[route_slot];
                            u64 bit = ((u64)1) << route_index;
                            AIPATHCNX_PC *const volatile *connection_input = connections;
                            if ((connection_input[edge]->route_mask & bit) == 0) {
                                connections[edge]->route_mask |= bit;
                                connections = runtime_node->connections;
                                connection = connections[edge];
                                for (i32 endpoint = 0; endpoint < 2; ++endpoint) {
                                    route_members[route_index][connection->node_indices[endpoint] / 8] |=
                                        1 << (connection->node_indices[endpoint] & 7);
                                }
                            }
                        }
                    }
                }
            }
        }

        for (i32 route_index = 0; route_index < path->route_count; ++route_index) {
            AIPATHROUTE_PC *route = &path->routes[route_index];
            for (i32 node = 0; node < path->node_count; ++node) {
                if (route_members[route_index][node >> 3] & (1u << (node & 7))) {
                    ++route->route_count;
                }
                if (path->nodes[node].route_boundary_mask & (((u64)1) << route_indices[route_index])) {
                    ++route->exit_node_count;
                }
            }
            if (path->node_count != 0) {
                PATH_BUILD_ALLOCATE(u8 *, route->node_routes, cursor, end, path->node_count);
                memset(route->node_routes, 0, path->node_count);
            }
            if (route->route_count != 0) {
                PATH_BUILD_ALLOCATE(u8 *, route->node_directions, cursor, end, route->route_count);
                memset(route->node_directions, 0, route->route_count);
            }
            if (route->exit_node_count != 0) {
                PATH_BUILD_ALLOCATE(u8 *, route->exit_nodes, cursor, end, route->exit_node_count);
                memset(route->exit_nodes, 0, route->exit_node_count);
            }
            i32 member_index = 0;
            i32 exit_index = 0;
            for (i32 node = 0; node < path->node_count; ++node) {
                route->node_routes[node] = 0xff;
                if (!(route_members[route_index][node >> 3] & (1u << (node & 7)))) {
                    continue;
                }
                path->nodes[node].route_membership_mask |= ((u64)1) << route_indices[route_index];
                route->node_routes[node] = member_index;
                route->node_directions[member_index] = node;
                if (path->nodes[node].route_boundary_mask & (((u64)1) << route_indices[route_index])) {
                    route->exit_nodes[exit_index++] = node;
                }
                ++member_index;
            }
            if (route->route_count == 0) {
                continue;
            }
            PATH_BUILD_ALLOCATE(u8 **, route->route_nodes, cursor, end, route->route_count * sizeof(u8 *));
            memset(route->route_nodes, 0, route->route_count * sizeof(u8 *));
            for (u32 source = 0; source < route->route_count; ++source) {
                PATH_BUILD_ALLOCATE(u8 *, route->route_nodes[source], cursor, end, route->route_count);
                memset(route->route_nodes[source], 0, route->route_count);
            }
            // The distance and full-node next-hop tables are temporary. Keep their
            // allocations beyond the persistent route data without advancing it.
            VARIPTR scratch = *cursor;
            f32 **distances = pathEditorCalculateDistanceTable(path, ((u64)1) << route_index, &scratch, end);
            if (distances == NULL) {
                continue;
            }
            u8 **next_hops;
            PATH_BUILD_ALLOCATE(u8 **, next_hops, &scratch, end, path->node_count * sizeof(u8 *));
            memset(next_hops, 0, path->node_count * sizeof(u8 *));
            for (i32 source = 0; source < path->node_count; ++source) {
                PATH_BUILD_ALLOCATE(u8 *, next_hops[source], &scratch, end, path->node_count);
                memset(next_hops[source], 0, path->node_count);
            }
            pathEditorPopulateRouteMatrix(path, (i32)(((u64)1) << route_index), distances, next_hops);
            for (u32 source = 0; source < route->route_count; ++source) {
                for (i32 destination = 0; destination < route->route_count; ++destination) {
                    route->route_nodes[source][destination] =
                        next_hops[route->node_directions[source]][route->node_directions[destination]];
                }
            }
        }
    }
}

extern "C" AIPATHSYS_PC *__cdecl pathEditorCreateData(VARIPTR *cursor, VARIPTR *end, void **scratch_cursor,
                                                      void **scratch_end) {
    iterator_count = 0;
    VARIPTR scratch;
    scratch.void_ptr = *scratch_cursor;
    if (PATH_EDITOR_BUILD_STATE->current_path == NULL) {
        distance_tables = NULL;
        return NULL;
    }
    AI_PATH_BUILD_PATH_PC *first_editor_path =
        (AI_PATH_BUILD_PATH_PC *)NuLinkedListGetHead(&PATH_EDITOR_BUILD_STATE->paths);
    if ((first_editor_path == NULL)) {
        distance_tables = NULL;
        return NULL;
    }

    i32 path_count = 0;
    for (AI_PATH_BUILD_PATH_PC *path = first_editor_path; path != NULL;
         path = (AI_PATH_BUILD_PATH_PC *)NuLinkedListGetNext(&PATH_EDITOR_BUILD_STATE->paths, &path->link)) {
        path->draw_index = path_count++;
    }
    if (path_count == 0) {
        distance_tables = NULL;
        return NULL;
    }
    AI_PATH_BUILD_SYSTEM_PC *system =
        (AI_PATH_BUILD_SYSTEM_PC *)AISysBufferAlloc(cursor, end, sizeof(AI_PATH_BUILD_SYSTEM_PC));
    if (system == NULL) {
        distance_tables = NULL;
        return NULL;
    }
    memset(system, 0, sizeof(*system));
    system->path_count = path_count;
    distance_tables = NULL;
    VARIPTR *scratch_limit = reinterpret_cast<VARIPTR *>(scratch_end);
    if (system->path_count != 0) {
        distance_tables = (f32 ***)AISysBufferAlloc(&scratch, scratch_limit, system->path_count * sizeof(f32 **));
        memset(distance_tables, 0, system->path_count * sizeof(f32 **));
    }

    system->special_route_count = 0;
    for (EDAISHAREDPATHNODE_s *shared =
             (EDAISHAREDPATHNODE_s *)NuLinkedListGetHead(&PATH_EDITOR_BUILD_STATE->shared_path_nodes);
         shared != NULL; shared = (EDAISHAREDPATHNODE_s *)NuLinkedListGetNext(
                             &PATH_EDITOR_BUILD_STATE->shared_path_nodes, &shared->link)) {
        shared->runtime_index = system->special_route_count++;
    }
    if (system->special_route_count != 0) {
        system->special_routes = (AIPATHSPECIALROUTE_PC *)AISysBufferAlloc(
            &scratch, scratch_limit, system->special_route_count * sizeof(AIPATHSPECIALROUTE_PC));
        memset(system->special_routes, 0, system->special_route_count * sizeof(AIPATHSPECIALROUTE_PC));
        for (EDAISHAREDPATHNODE_s *shared =
                 (EDAISHAREDPATHNODE_s *)NuLinkedListGetHead(&PATH_EDITOR_BUILD_STATE->shared_path_nodes);
             shared != NULL; shared = (EDAISHAREDPATHNODE_s *)NuLinkedListGetNext(
                                 &PATH_EDITOR_BUILD_STATE->shared_path_nodes, &shared->link)) {
            AIPATHSPECIALROUTE_PC *route = &system->special_routes[shared->runtime_index];
            PATH_BUILD_ALLOCATE(AIPATH_PC **, route->paths, &scratch, scratch_limit,
                                shared->reference_count * sizeof(AIPATH_PC));
            memset(route->paths, 0, shared->reference_count * sizeof(AIPATH_PC));
        }
    }
    system->paths = (AIPATH_PC **)AISysBufferAlloc(cursor, end, system->path_count * sizeof(AIPATH_PC *));
    if (system->paths == NULL) {
        distance_tables = NULL;
        return (AIPATHSYS_PC *)system;
    }
    memset(system->paths, 0, system->path_count * sizeof(AIPATH_PC *));
    i32 path_index = 0;
    for (AI_PATH_BUILD_PATH_PC *editor_path =
             (AI_PATH_BUILD_PATH_PC *)NuLinkedListGetHead(&PATH_EDITOR_BUILD_STATE->paths);
         editor_path != NULL; editor_path = (AI_PATH_BUILD_PATH_PC *)NuLinkedListGetNext(
                                  &PATH_EDITOR_BUILD_STATE->paths, &editor_path->link),
                               ++path_index) {
        system->paths[path_index] = (AIPATH_PC *)AISysBufferAlloc(cursor, end, sizeof(AIPATH_PC));
        AIPATH_PC *path = system->paths[path_index];
        if (path == NULL) {
            continue;
        }
        memset(path, 0, sizeof(*path));
        strcpy(path->name, editor_path->name);
        path->index = path_index;

        for (EDAIPATHNODE_s *node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&editor_path->nodes); node != NULL;
             node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&editor_path->nodes, &node->link)) {
            node->index = path->node_count++;
            for (i32 slot = 0; slot < 8; ++slot) {
                if (node->connections[slot].node != NULL) {
                    ++path->connection_count;
                }
            }
            if (node->shared_node != NULL) {
                ++path->special_route_count;
            }
        }
        if (path->connection_count != 0) {
            path->connection_count /= 2;
            path->connections =
                (AIPATHCNX_PC *)AISysBufferAlloc(cursor, end, path->connection_count * sizeof(AIPATHCNX_PC));
            if (path->connections != NULL) {
                memset(path->connections, 0, path->connection_count * sizeof(AIPATHCNX_PC));
            }
        }
        if (path->special_route_count != 0) {
            path->special_routes = (AIPATHNODELINK_PC *)AISysBufferAlloc(
                cursor, end, path->special_route_count * sizeof(AIPATHNODELINK_PC));
            memset(path->special_routes, 0, path->special_route_count * sizeof(AIPATHNODELINK_PC));
            path->special_route_count = 0;
        }
        if (path->node_count != 0) {
            path->nodes = (AIPATHNODE_PC *)AISysBufferAlloc(cursor, end, path->node_count * sizeof(AIPATHNODE_PC));
            if (path->nodes != NULL) {
                memset(path->nodes, 0, path->node_count * sizeof(AIPATHNODE_PC));

                i32 connection_index = 0;
                for (EDAIPATHNODE_s *node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&editor_path->nodes); node != NULL;
                     node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&editor_path->nodes, &node->link)) {
                    AIPATHNODE_PC *runtime = &path->nodes[node->index];
                    i32 length = strlen(node->name);
                    if (length != 0) {
                        runtime->name = (char *)AISysBufferAlloc(cursor, end, length + 1);
                        strcpy(runtime->name, node->name);
                    }
                    runtime->position = node->position;
                    runtime->radius = node->radius;
                    runtime->radius_squared = node->radius * node->radius;
                    runtime->runtime_flags = node->flags;
                    runtime->special_handle = *(NUSPECIALHANDLE *)&node->special;
                    if (NuSpecialExistsFn(&runtime->special_handle)) {
                        runtime->has_special = 1;
                    }
                    runtime->special_position = node->special_position;
                    const volatile f32 *position_y = &runtime->position.y;
                    runtime->min_height = node->lower_height + *position_y;
                    runtime->max_height = node->upper_height + *position_y;
                    runtime->min_height_offset = runtime->min_height - *position_y;
                    runtime->max_height_offset = runtime->max_height - *position_y;
                    for (i32 slot = 0; slot < 8; ++slot) {
                        if ((node->connections[slot].node != NULL)) {
                            ++runtime->connection_count;
                        }
                    }
                    if ((path->connections != NULL)) {
                        if (runtime->connection_count != 0) {
                            PATH_BUILD_ALLOCATE(AIPATHCNX_PC **, runtime->connections, cursor, end,
                                                runtime->connection_count * sizeof(AIPATHCNX_PC *));
                            if (runtime->connections != NULL) {
                                memset(runtime->connections, 0, runtime->connection_count * sizeof(AIPATHCNX_PC *));
                            }
                            i32 next = 0;
                            for (i32 slot = 0; slot < 8; ++slot) {
                                EDAIPATHNODE_s *other = node->connections[slot].node;
                                if (other == NULL) {
                                    continue;
                                }
                                if (other->index < node->index) {
                                    AIPATHNODE_PC *other_runtime = &path->nodes[other->index];
                                    for (i32 reverse = 0; reverse < other_runtime->connection_count; ++reverse) {
                                        AIPATHCNX_PC *connection = other_runtime->connections[reverse];
                                        if (connection->node_indices[1] == node->index) {
                                            runtime->connections[next] = connection;
                                            runtime->connections[next]->traversal_flags[1] =
                                                node->connections[slot].flags;
                                            runtime->connections[next]->original_traversal_flags[1] =
                                                node->connections[slot].flags;
                                            break;
                                        }
                                    }
                                } else {
                                    runtime->connections[next] = &path->connections[connection_index++];
                                    runtime->connections[next]->node_indices[0] = node->index;
                                    runtime->connections[next]->node_indices[1] = node->connections[slot].node->index;
                                    runtime->connections[next]->traversal_flags[0] = node->connections[slot].flags;
                                    runtime->connections[next]->original_traversal_flags[0] =
                                        node->connections[slot].flags;
                                    NUVEC difference;
                                    runtime->connections[next]->distance = NuVecDist(
                                        &node->connections[slot].node->position, &node->position, &difference);
                                    runtime->connections[next]->horizontal_distance = NuVecXZDist(
                                        &node->connections[slot].node->position, &node->position, &difference);
                                    AIPATHCNX_PC *connection = runtime->connections[next];
                                    if (connection->horizontal_distance == 0.0f) {
                                        connection->horizontal_distance = 0.0001f;
                                    }
                                    runtime->connections[next]->rotation =
                                        (i16)(NuAtan2(difference.x, difference.z) * AIPathAngularScale);
                                }
                                ++next;
                            }
                        }
                    }
                    runtime->distance_cache_nodes[0] = 0xff;
                    runtime->distance_cache_nodes[1] = 0xff;
                    if (node->shared_node != NULL) {
                        AIPATHSPECIALROUTE_PC *shared_route = &system->special_routes[node->shared_node->runtime_index];
                        AIPATHNODELINK_PC *link =
                            &path->special_routes[runtime->special_route_index = path->special_route_count++];
                        shared_route->paths[shared_route->path_count++] = path;
                        link->node_index = node->index;
                        link->special_route_index = node->shared_node->runtime_index;
                    } else {
                        runtime->special_route_index = 0xff;
                    }
                }
            }
        }
        f32 ***distance_slot = &distance_tables[path_index];
        *distance_slot = pathEditorCalculateDistanceTable(path, 0, &scratch, scratch_limit);

        if (path->node_count != 0) {
            PATH_BUILD_ALLOCATE(u8 **, path->route_matrix, cursor, end, path->node_count * sizeof(u8 *));
            memset(path->route_matrix, 0, path->node_count * sizeof(u8 *));
            {
                for (u32 source = 0; source < path->node_count; ++source) {
                    PATH_BUILD_ALLOCATE(u8 *, path->route_matrix[source], cursor, end, path->node_count);
                    memset(path->route_matrix[source], 0, path->node_count);
                }
            }
            u8 **route_matrix = path->route_matrix;
            f32 **distances = *distance_slot;
            pathEditorPopulateRouteMatrix(path, 0, distances, route_matrix);
            pathEditorCreateSpecialRouteData(path, editor_path, &scratch, scratch_limit);
        }
    }
    if (system->path_count > 1) {
        AIPATHINFO_PC info;
        AIPATH_PC *first_path = system->paths[0];
        memset(&info, 0, sizeof(info));
        for (u32 path_index = 1; path_index < system->path_count; ++path_index) {
            AIPATH_PC *path = system->paths[path_index];
            for (i32 node_index = 0; node_index < path->node_count; ++node_index) {
                AIPATHNODE_PC *node = &path->nodes[node_index];
                AISysGetPathPosEx(PATH_EDITOR_BUILD_STATE->ai_system, &node->position, &info, first_path, 0xff, 0,
                                  NULL);
                if ((info.flags & 1)) {
                    f64 half = InitOnceHalf;
                    f64 path_distance = info.distance;
                    node->path_flags = info.connection - path->connections;
                    if (path_distance > half || (info.direction != 0 && path_distance < half)) {
                        node->runtime_flags |= 4;
                    }
                    node->runtime_flags |= 1;
                    node->path_flags = info.connection - info.path->connections;
                    path->flags |= 2;
                } else {
                    node->path_flags = -1;
                    node->runtime_flags &= (u8)~1;
                }
            }
            if ((!(path->flags & 2) || distance_tables == NULL || distance_tables[path_index] == NULL)) {
                continue;
            }
            for (i32 node_index = 0; node_index < path->node_count; ++node_index) {
                AIPATHNODE_PC *node = &path->nodes[node_index];
                if (node->runtime_flags & 1) {
                    continue;
                }
                f32 nearest = FLT_MAX;
                for (i32 candidate = 0; candidate < path->node_count; ++candidate) {
                    if (&path->nodes[candidate] != node && (path->nodes[candidate].runtime_flags & 1) &&
                        distance_tables[path_index][node_index][candidate] < nearest) {
                        node->path_flags = candidate;
                        nearest = distance_tables[path_index][node_index][candidate];
                    }
                }
            }
        }
    }
    distance_tables = NULL;
    return (AIPATHSYS_PC *)system;
}
