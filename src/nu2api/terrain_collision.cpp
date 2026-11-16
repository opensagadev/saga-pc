#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/core/terrain_internal.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nucore/numem.h"
#include "nu2api/nudynamiclight_gen.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include <string.h>
#include <math.h>

enum {
    NUTRIGTABLE_COUNT = 32768,
    TERRAIN_HIT_TYPE_NONE = 0,
    TERRAIN_HIT_TYPE_FACE = 1,
    TERRAIN_HIT_TYPE_CYLINDER = 2,
    TERRAIN_HIT_TYPE_VERTEX = 3,
    TERRAIN_HIT_TYPE_SPHERE = 4,
    TERRAIN_HIT_TYPE_SECOND_NORMAL = 16,
    TERRAIN_HIT_TYPE_CLASS_MASK = 15,
    TERRAIN_TRACK_FLAG_CONNECTED = 1,
    TERRAIN_TRACK_CONTACT_ACTIVE = 4,
    TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL = 1,
    TERRAIN_SURFACE_CLASS_MASK = 0x7c,
    TERRAIN_SURFACE_CLASS_WALL_OVERRIDE = 4,
    TERRAIN_IMPACT_RESULT_WALL = 1,
    TERRAIN_IMPACT_RESULT_GROUND = 2,
    TERRAIN_IMPACT_RESULT_STATIC_WALL = 16
};

enum {
    TERRAIN_HIT_EDGE_0_1 = 1,
    TERRAIN_HIT_EDGE_2_0 = 3,
    TERRAIN_HIT_EDGE_1_2 = 2,
    TERRAIN_HIT_EDGE_1_3 = 4,
    TERRAIN_HIT_EDGE_3_2 = 5,
    TERRAIN_TRACK_FLAG_NONE = 0
};

extern "C" {
    void DerotateMovementVector() {
        TerrainQuery_s *query = TerI;
        const f64 yaw = NuAtan2DA(query->movement.x, query->movement.z);
        query = TerI;
        query->movement_yaw = static_cast<f32>(yaw);
        const i32 pitch =
            NuAtan2DA(-TerI->movement.y, NuFsqrt(query->movement.x * query->movement.x + query->movement.z * query->movement.z));
        query = TerI;
        query->movement_pitch = static_cast<f32>(pitch);
        TerI->movement_length =
            NuFsqrt(query->movement.x * query->movement.x + query->movement.y * query->movement.y + query->movement.z * query->movement.z);
    }

    void RotateVec(NUVEC *source, NUVEC *destination) {
        TerrainQuery_s *query = TerI;
        const f32 rotated_z = NU_COS_LUT_F(query->movement_pitch) * source->z + NU_SIN_LUT(query->movement_pitch) * source->y;
        destination->y = source->y * NU_COS_LUT_F(query->movement_pitch) - NU_SIN_LUT(query->movement_pitch) * source->z;
        destination->z = NU_COS_LUT_F(query->movement_yaw) * rotated_z - NU_SIN_LUT(query->movement_yaw) * source->x;
        destination->x = NU_COS_LUT_F(query->movement_yaw) * source->x + NU_SIN_LUT(query->movement_yaw) * rotated_z;
    }

    void DeRotateTerrain(TERRAIN_POLYGON_INFO *surface) {
        TerrainQuery_s *query = TerI;

        const f32 pitch = -query->movement_pitch;
        const f32 sin_pitch = NU_SIN_LUT(pitch);
        const f32 cos_pitch = NU_COS_LUT_F(pitch);
        const f32 yaw = -query->movement_yaw;
        const f32 sin_yaw = NU_SIN_LUT(yaw);
        const f32 cos_yaw = NU_COS_LUT_F(yaw);

        {
            const f64 relative_z = surface->vertices[0].z - query->local_start.z;
            const f64 relative_x = surface->vertices[0].x - query->local_start.x;
            const f32 rotated_z = relative_z * cos_yaw - relative_x * sin_yaw;
            query->transformed_vertices[0].x = relative_z * sin_yaw + relative_x * cos_yaw;
            query->transformed_vertices[0].y = (surface->vertices[0].y - query->local_start.y) * cos_pitch - rotated_z * sin_pitch;
            query->transformed_vertices[0].z = (surface->vertices[0].y - query->local_start.y) * sin_pitch + rotated_z * cos_pitch;
        }
        {
            const f64 relative_z = surface->vertices[1].z - query->local_start.z;
            const f64 relative_x = surface->vertices[1].x - query->local_start.x;
            const f32 rotated_z = relative_z * cos_yaw - relative_x * sin_yaw;
            query->transformed_vertices[1].x = relative_z * sin_yaw + relative_x * cos_yaw;
            query->transformed_vertices[1].y = (surface->vertices[1].y - query->local_start.y) * cos_pitch - rotated_z * sin_pitch;
            query->transformed_vertices[1].z = (surface->vertices[1].y - query->local_start.y) * sin_pitch + rotated_z * cos_pitch;
        }
        {
            const f64 relative_z = surface->vertices[2].z - query->local_start.z;
            const f64 relative_x = surface->vertices[2].x - query->local_start.x;
            const f32 rotated_z = relative_z * cos_yaw - relative_x * sin_yaw;
            query->transformed_vertices[2].x = relative_z * sin_yaw + relative_x * cos_yaw;
            query->transformed_vertices[2].y = (surface->vertices[2].y - query->local_start.y) * cos_pitch - rotated_z * sin_pitch;
            query->transformed_vertices[2].z = (surface->vertices[2].y - query->local_start.y) * sin_pitch + rotated_z * cos_pitch;
        }
        if (surface->normals[1].y < 65536.0f) {
            {
                const f64 relative_z = surface->vertices[3].z - query->local_start.z;
                const f64 relative_x = surface->vertices[3].x - query->local_start.x;
                const f32 rotated_z = relative_z * cos_yaw - relative_x * sin_yaw;
                query->transformed_vertices[3].x = relative_z * sin_yaw + relative_x * cos_yaw;
                query->transformed_vertices[3].y = (surface->vertices[3].y - query->local_start.y) * cos_pitch - rotated_z * sin_pitch;
                query->transformed_vertices[3].z = (surface->vertices[3].y - query->local_start.y) * sin_pitch + rotated_z * cos_pitch;
            }
        }
    }

    void DeRotatePoint(NUVEC *point) {
        TerrainQuery_s *query = TerI;

        const f32 pitch = -query->movement_pitch;
        const f32 sin_pitch = NU_SIN_LUT(pitch);
        const f32 cos_pitch = NU_COS_LUT_F(pitch);
        const f32 yaw = -query->movement_yaw;
        const f32 sin_yaw = NU_SIN_LUT(yaw);
        const f32 cos_yaw = NU_COS_LUT_F(yaw);

        const f64 relative_z = point->z - query->position.z;
        const f64 relative_x = point->x - query->position.x;
        const f32 rotated_z = relative_z * cos_yaw - relative_x * sin_yaw;
        point->x = relative_z * sin_yaw + relative_x * cos_yaw;
        point->z = (query->collision_radius + point->y - query->position.y) * sin_pitch + rotated_z * cos_pitch;
        point->y = (query->collision_radius + point->y - query->position.y) * cos_pitch - rotated_z * sin_pitch;
    }

    i32 CheckCylinder(i32 first_vertex, i32 second_vertex, i32 *vertex_mask, i32 remaining_vertex_mask) {
        TerrainQuery_s *query = TerI;

        // Reject an edge when both endpoints lie outside the swept sphere's
        // expanded bounds. Once an edge is rejected this way, its endpoint bits
        // can be removed from the later vertex tests as well.
        if ((query->transformed_vertices[first_vertex].x > query->collision_radius &&
             query->transformed_vertices[second_vertex].x > query->collision_radius) ||
            (query->transformed_vertices[first_vertex].x < -query->collision_radius &&
             query->transformed_vertices[second_vertex].x < -query->collision_radius) ||
            (query->transformed_vertices[first_vertex].y > query->collision_radius &&
             query->transformed_vertices[second_vertex].y > query->collision_radius) ||
            (query->transformed_vertices[first_vertex].y < -query->collision_radius &&
             query->transformed_vertices[second_vertex].y < -query->collision_radius) ||
            (query->transformed_vertices[first_vertex].z < -query->collision_radius &&
             query->transformed_vertices[second_vertex].z < -query->collision_radius) ||
            (query->transformed_vertices[first_vertex].z > query->movement_length + query->collision_radius &&
             query->transformed_vertices[second_vertex].z > query->movement_length + query->collision_radius)) {
            *vertex_mask &= remaining_vertex_mask;
            return 0;
        }

        NUVEC edge_direction;
        edge_direction.x = query->transformed_vertices[second_vertex].x - query->transformed_vertices[first_vertex].x;
        edge_direction.y = query->transformed_vertices[second_vertex].y - query->transformed_vertices[first_vertex].y;
        f64 horizontal_length_sq_wide;
        const f32 horizontal_length_sq = horizontal_length_sq_wide =
            edge_direction.x * edge_direction.x + edge_direction.y * edge_direction.y;

        // An almost vertical edge has no stable radial cylinder in the XY plane.
        // It can only overlap the sphere at the start of the sweep when it crosses
        // z = 0.
        if (horizontal_length_sq < 1.0e-12f) {
            if ((query->transformed_vertices[first_vertex].z <= 0.0f && query->transformed_vertices[second_vertex].z >= 0.0f) ||
                (query->transformed_vertices[first_vertex].z >= 0.0f && query->transformed_vertices[second_vertex].z <= 0.0f)) {
                f32 distance = query->transformed_vertices[second_vertex].x * query->transformed_vertices[second_vertex].x +
                               query->transformed_vertices[second_vertex].y * query->transformed_vertices[second_vertex].y;
                if (distance <= query->collision_radius_sq) {
                    distance = NuFsqrt(distance);
                    query = TerI;
                    if (distance - query->collision_radius < query->hit_time) {
                        NUVEC local_normal;
                        local_normal.x = -query->transformed_vertices[second_vertex].x;
                        local_normal.y = -query->transformed_vertices[second_vertex].y;
                        local_normal.z = 0.0f;
                        RotateVec(&local_normal, &local_normal);
                        if (query->working_surface->normals[0].y * local_normal.y + query->working_surface->normals[0].x * local_normal.x +
                                query->working_surface->normals[0].z * local_normal.z >
                            0.0) {
                            query->hit_time = distance - TerI->collision_radius;
                            const f32 inverse_distance = TerrainDivideNonzeroPC(1.0f, distance);
                            query->hit_type = TERRAIN_HIT_TYPE_CYLINDER | TERRAIN_HIT_TYPE_SECOND_NORMAL;
                            query->movement_normal.x = -query->transformed_vertices[second_vertex].x * inverse_distance;
                            query->movement_normal.y = -query->transformed_vertices[second_vertex].y * inverse_distance;
                            query->movement_normal.z = 0.0f;
                            return 1;
                        }
                    }
                }
            }
            return 0;
        }

        // Squared radial distance from the movement axis to the infinite line
        // containing the edge. This avoids a square root for the common miss.
        const f32 line_cross =
            query->transformed_vertices[first_vertex].x * edge_direction.y - query->transformed_vertices[first_vertex].y * edge_direction.x;
        if (line_cross * line_cross > query->collision_radius_sq * horizontal_length_sq) {
            *vertex_mask &= remaining_vertex_mask;
            return 0;
        }

        const f32 line_distance_sq = TerrainDivideNonzeroPC(line_cross, horizontal_length_sq) * line_cross;
        query = TerI;
        edge_direction.z = query->transformed_vertices[second_vertex].z - query->transformed_vertices[first_vertex].z;
        const f32 edge_length = NuFsqrt(edge_direction.z * edge_direction.z + horizontal_length_sq_wide);
        const f32 inverse_edge_length = TerrainDivideNonzeroPC(1.0f, edge_length);
        query = TerI;
        edge_direction.x *= inverse_edge_length;
        edge_direction.y *= inverse_edge_length;
        edge_direction.z *= inverse_edge_length;

        f32 horizontal_perpendicular[2];
        horizontal_perpendicular[0] = -edge_direction.y;
        horizontal_perpendicular[1] = edge_direction.x;
        NUVEC line_cross_vector;
        line_cross_vector.x = -query->transformed_vertices[first_vertex].x;
        line_cross_vector.y = -query->transformed_vertices[first_vertex].y;
        line_cross_vector.z = -query->transformed_vertices[first_vertex].z;
        NuVecCross(&line_cross_vector, &line_cross_vector, &edge_direction);

        const f32 closest_height = -TerrainDivideNonzeroPC(
            line_cross_vector.y * horizontal_perpendicular[1] + line_cross_vector.x * horizontal_perpendicular[0],
            horizontal_perpendicular[0] * horizontal_perpendicular[0] + horizontal_perpendicular[1] * horizontal_perpendicular[1]);

        // This normal lies in the plane formed by the edge and the movement axis.
        // Its z component tells how far along the sweep the cylinder surface is
        // reached.
        NUVEC cylinder_side;
        cylinder_side.x = edge_direction.x * edge_direction.z;
        cylinder_side.y = -(edge_direction.y * edge_direction.z);
        cylinder_side.z = -edge_direction.x * edge_direction.x - edge_direction.y * edge_direction.y;
        const f32 cylinder_side_z =
            TerrainDivideNonzeroPC(cylinder_side.z, NuFsqrt(cylinder_side.x * cylinder_side.x + cylinder_side.y * cylinder_side.y +
                                                            cylinder_side.z * cylinder_side.z));
        const f32 height_offset = TerrainDivideNonzeroPC(NuFsqrt(TerI->collision_radius_sq - line_distance_sq), cylinder_side_z);

        f32 hit_distance;
        if (height_offset < 0.0f) {
            hit_distance = height_offset + closest_height;
        } else {
            hit_distance = closest_height - height_offset;
        }
        query = TerI;
        if (hit_distance >= 0.0f && hit_distance <= query->movement_length) {
            const f32 edge_parameter = -query->transformed_vertices[first_vertex].x * edge_direction.x +
                                       -query->transformed_vertices[first_vertex].y * edge_direction.y +
                                       (hit_distance - query->transformed_vertices[first_vertex].z) * edge_direction.z;
            if (0.0f < edge_parameter && edge_parameter <= edge_length) {
                if (TerrainDivideNonzeroPC(hit_distance, query->movement_length) < query->hit_time) {
                    query->hit_type = TERRAIN_HIT_TYPE_CYLINDER;
                    query->hit_time = TerrainDivideNonzeroPC(hit_distance, query->movement_length);
                    query->movement_normal.x = -(edge_parameter * edge_direction.x + query->transformed_vertices[first_vertex].x);
                    query->movement_normal.y = -(edge_parameter * edge_direction.y + query->transformed_vertices[first_vertex].y);
                    query->movement_normal.z =
                        hit_distance - (edge_parameter * edge_direction.z + query->transformed_vertices[first_vertex].z);
                    *vertex_mask &= remaining_vertex_mask;
                    return 1;
                }
                *vertex_mask &= remaining_vertex_mask;
                return 0;
            }
        }

        // No forward-time hit was found. Test whether the sphere already overlaps
        // the finite edge at the start of the sweep.
        const f32 negative_start_radius = -query->collision_radius;
        if (query->transformed_vertices[first_vertex].z < negative_start_radius &&
            query->transformed_vertices[second_vertex].z < negative_start_radius) {
            return 0;
        }
        if (query->transformed_vertices[first_vertex].z > query->collision_radius &&
            query->transformed_vertices[second_vertex].z > query->collision_radius) {
            return 0;
        }

        const f32 closest_parameter = -edge_direction.x * query->transformed_vertices[first_vertex].x -
                                      query->transformed_vertices[first_vertex].y * edge_direction.y -
                                      query->transformed_vertices[first_vertex].z * edge_direction.z;
        if (closest_parameter < 0.0f ||
            closest_parameter >
                (query->transformed_vertices[second_vertex].x - query->transformed_vertices[first_vertex].x) * edge_direction.x +
                    (query->transformed_vertices[second_vertex].y - query->transformed_vertices[first_vertex].y) * edge_direction.y +
                    (query->transformed_vertices[second_vertex].z - query->transformed_vertices[first_vertex].z) * edge_direction.z) {
            return 0;
        }

        // The normalized direction is no longer needed after the closest
        // parameter has been bounded, so reuse it for the closest point itself.
        edge_direction.x = edge_direction.x * closest_parameter + query->transformed_vertices[first_vertex].x;
        edge_direction.y = edge_direction.y * closest_parameter + query->transformed_vertices[first_vertex].y;
        edge_direction.z = edge_direction.z * closest_parameter + query->transformed_vertices[first_vertex].z;
        f32 distance = edge_direction.x * edge_direction.x + edge_direction.y * edge_direction.y + edge_direction.z * edge_direction.z;
        if (distance < query->collision_radius_sq) {
            distance = NuFsqrt(distance);
            query = TerI;
            if (distance - query->collision_radius < query->hit_time) {
                NUVEC rotated;
                RotateVec(&edge_direction, &rotated);
                const NUVEC &surface_normal = query->working_surface->normals[0];
                if (surface_normal.y * rotated.y + surface_normal.x * rotated.x + surface_normal.z * rotated.z < 0.0) {
                    query->hit_type = TERRAIN_HIT_TYPE_CYLINDER | TERRAIN_HIT_TYPE_SECOND_NORMAL;
                    query->hit_time = distance - TerI->collision_radius;
                    const f32 inverse_distance = 1.0f / distance;
                    query->movement_normal.x = -edge_direction.x * inverse_distance;
                    query->movement_normal.y = -edge_direction.y * inverse_distance;
                    query->movement_normal.z = -edge_direction.z * inverse_distance;
                    return 1;
                }
            }
        }
        return 0;
    }

    i32 CheckSphere(i32 vertex_index) {
        TerrainQuery_s *query = TerI;
        if (query->transformed_vertices[vertex_index].z < -query->collision_radius ||
            query->transformed_vertices[vertex_index].z > query->movement_length + query->collision_radius) {
            return 0;
        }

        f32 distance_sq = query->transformed_vertices[vertex_index].x * query->transformed_vertices[vertex_index].x +
                          query->transformed_vertices[vertex_index].y * query->transformed_vertices[vertex_index].y;
        if (distance_sq > query->collision_radius_sq) {
            return 0;
        }

        const f32 radial_extent = NuFsqrt(query->collision_radius_sq - distance_sq);
        query = TerI;
        if (query->transformed_vertices[vertex_index].z - radial_extent < 0.0f ||
            query->transformed_vertices[vertex_index].z - radial_extent > query->movement_length) {
            distance_sq += query->transformed_vertices[vertex_index].z * query->transformed_vertices[vertex_index].z;
            if (distance_sq < query->collision_radius_sq) {
                const f32 distance = NuFsqrt(distance_sq);
                query = TerI;
                const f64 overlap_time = distance - query->collision_radius;
                if (overlap_time < query->hit_time) {
                    TERRAIN_POLYGON_INFO *surface = query->working_surface;
                    const f32 primary_distance = (query->local_start.x - surface->vertices[vertex_index].x) * surface->normals[0].x +
                                                 (query->local_start.y - surface->vertices[vertex_index].y) * surface->normals[0].y +
                                                 (query->local_start.z - surface->vertices[vertex_index].z) * surface->normals[0].z;
                    if (primary_distance > 0.0f) {
                        query->hit_time = overlap_time;
                        const f32 inverse_distance = TerrainDivideNonzeroPC(1.0f, distance);
                        query = TerI;
                        query->hit_type = TERRAIN_HIT_TYPE_VERTEX | TERRAIN_HIT_TYPE_SECOND_NORMAL;
                        query->movement_normal.x = -query->transformed_vertices[vertex_index].x * inverse_distance;
                        query->movement_normal.y = -query->transformed_vertices[vertex_index].y * inverse_distance;
                        query->movement_normal.z = -query->transformed_vertices[vertex_index].z * inverse_distance;
                        return 1;
                    }
                }
            }
            return 0;
        }

        const f32 hit_time = (query->transformed_vertices[vertex_index].z - radial_extent) / query->movement_length;
        if (hit_time < query->hit_time) {
            query->hit_time = hit_time;
            query->hit_type = TERRAIN_HIT_TYPE_VERTEX;
            query->movement_normal.x = -query->transformed_vertices[vertex_index].x;
            query->movement_normal.y = -query->transformed_vertices[vertex_index].y;
            query->movement_normal.z = -radial_extent;
            return 1;
        }
        return 0;
    }

    i32 CheckSphereTer(NUVEC *position, f32 radius) {
        TerrainQuery_s *query = TerI;
        if (position->z < -query->collision_radius - radius || position->z > radius + query->collision_radius + query->movement_length) {
            return 0;
        }

        f32 distance_sq = position->x * position->x + position->y * position->y;
        const f32 combined_radius_sq = (radius + query->collision_radius) * (radius + query->collision_radius);
        if (distance_sq > combined_radius_sq) {
            return 0;
        }

        radius = NuFsqrt(combined_radius_sq - distance_sq);
        if (position->z - radius < 0.0f || position->z - radius > TerI->movement_length) {
            distance_sq += position->z * position->z;
            if (distance_sq < combined_radius_sq) {
                const f32 inverse_distance = TerrainDivideNonzeroPC(1.0f, NuFsqrt(distance_sq));
                query = TerI;
                query->hit_time = 0.0f;
                query->hit_type = TERRAIN_HIT_TYPE_SPHERE | TERRAIN_HIT_TYPE_SECOND_NORMAL;
                query->movement_normal.x = -position->x * inverse_distance;
                query->movement_normal.y = -position->y * inverse_distance;
                query->movement_normal.z = -position->z * inverse_distance;
                return 1;
            }
            return 0;
        }

        query = TerI;
        distance_sq = (position->z - radius) / query->movement_length;
        if (distance_sq < query->hit_time) {
            query->hit_time = distance_sq;
            query->hit_type = TERRAIN_HIT_TYPE_SPHERE;
            query->movement_normal.x = -position->x;
            query->movement_normal.y = -position->y;
            query->movement_normal.z = -radius;
            return 1;
        }
        return 0;
    }

    i32 HitPoly(f32 primary_start, f32 primary_end, f32 secondary_start, f32 secondary_end, TERRAIN_POLYGON_INFO *surface) {
        TerrainQuery_s *query = TerI;
        i32 collision_found = 0;
        i32 test_face;
        f32 candidate_time;
        NUVEC contact_point;

        contact_point.x = query->local_start.x - surface->normals[0].x * query->collision_radius - surface->vertices[0].x;
        contact_point.y = query->local_start.y - surface->normals[0].y * query->collision_radius - surface->vertices[0].y;
        contact_point.z = query->local_start.z - surface->normals[0].z * query->collision_radius - surface->vertices[0].z;
        test_face = 1;
        if (primary_start > 0.0f && primary_end < 0.0f) {
            candidate_time = primary_start / (primary_start - primary_end);
            contact_point.x += query->movement.x * candidate_time;
            contact_point.y += query->movement.y * candidate_time;
            contact_point.z += query->movement.z * candidate_time;
        } else if (primary_start <= 0.0f && primary_end <= 0.0f && primary_start >= -query->collision_radius &&
                   primary_end >= -query->collision_radius) {
            candidate_time = primary_start;
            contact_point.x -= primary_start * surface->normals[0].x;
            contact_point.y -= primary_start * surface->normals[0].y;
            contact_point.z -= primary_start * surface->normals[0].z;
        } else {
            test_face = 0;
        }
        if (test_face != 0 &&
            InsidePolLines(contact_point.x, contact_point.y, contact_point.z, surface->vertices[1].x - surface->vertices[0].x,
                           surface->vertices[1].y - surface->vertices[0].y, surface->vertices[1].z - surface->vertices[0].z,
                           surface->vertices[2].x - surface->vertices[0].x, surface->vertices[2].y - surface->vertices[0].y,
                           surface->vertices[2].z - surface->vertices[0].z, &surface->normals[0]) != 0 &&
            candidate_time <= query->hit_time) {
            if (primary_start > 0.0f) {
                query->hit_type = TERRAIN_HIT_TYPE_FACE;
            } else {
                query->hit_type = TERRAIN_HIT_TYPE_FACE | TERRAIN_HIT_TYPE_SECOND_NORMAL;
            }
            collision_found = 1;
            query->hit_time = candidate_time;
            query->movement_normal.x = surface->normals[0].x;
            query->movement_normal.y = surface->normals[0].y;
            query->movement_normal.z = surface->normals[0].z;
            query->shape_adjusted = surface->normal_flags & 1;
        }
        if (surface->normals[1].y < 65536.0f) {
            contact_point.x = query->local_start.x - surface->normals[1].x * query->collision_radius - surface->vertices[3].x;
            contact_point.y = query->local_start.y - surface->normals[1].y * query->collision_radius - surface->vertices[3].y;
            contact_point.z = query->local_start.z - surface->normals[1].z * query->collision_radius - surface->vertices[3].z;
            test_face = 1;
            if (secondary_start > 0.0f && secondary_end < 0.0f) {
                candidate_time = secondary_start / (secondary_start - secondary_end);
                contact_point.x += query->movement.x * candidate_time;
                contact_point.y += query->movement.y * candidate_time;
                contact_point.z += query->movement.z * candidate_time;
            } else if (secondary_start <= 0.0f && secondary_end <= 0.0f && secondary_start >= -query->collision_radius &&
                       secondary_end >= -query->collision_radius) {
                candidate_time = secondary_start;
                contact_point.x -= secondary_start * surface->normals[1].x;
                contact_point.y -= secondary_start * surface->normals[1].y;
                contact_point.z -= secondary_start * surface->normals[1].z;
            } else {
                test_face = 0;
            }
            if (test_face != 0 &&
                InsidePolLines(contact_point.x, contact_point.y, contact_point.z, surface->vertices[2].x - surface->vertices[3].x,
                               surface->vertices[2].y - surface->vertices[3].y, surface->vertices[2].z - surface->vertices[3].z,
                               surface->vertices[1].x - surface->vertices[3].x, surface->vertices[1].y - surface->vertices[3].y,
                               surface->vertices[1].z - surface->vertices[3].z, &surface->normals[1]) != 0 &&
                candidate_time < query->hit_time) {
                query->hit_time = candidate_time;
                if (secondary_start > 0.0f) {
                    query->hit_type = TERRAIN_HIT_TYPE_FACE;
                } else {
                    query->hit_type = TERRAIN_HIT_TYPE_FACE | TERRAIN_HIT_TYPE_SECOND_NORMAL;
                }
                collision_found = 1;
                query->movement_normal.x = surface->normals[1].x;
                query->movement_normal.y = surface->normals[1].y;
                query->movement_normal.z = surface->normals[1].z;
                query->shape_adjusted = (surface->normal_flags >> 1) & 1;
            }
        }

        if (query->collision_radius != 0.0f) {
            query->working_surface = surface;
            i32 vertex_mask = 0xf;
            DeRotateTerrain(surface);

            if (primary_start >= -query->collision_radius) {
                if (CheckCylinder(0, 1, &vertex_mask, 0xc) != 0) {
                    TerrainQuery_s *edge_query = TerI;
                    edge_query->hit_edge = TERRAIN_HIT_EDGE_0_1;
                    collision_found |= 1;
                    edge_query->shape_adjusted = surface->normal_flags & 1;
                }
                if (CheckCylinder(2, 0, &vertex_mask, 0xa) != 0) {
                    TerrainQuery_s *edge_query = TerI;
                    edge_query->hit_edge = TERRAIN_HIT_EDGE_2_0;
                    collision_found |= 1;
                    edge_query->shape_adjusted = surface->normal_flags & 1;
                }
            }
            i32 hit = CheckCylinder(1, 2, &vertex_mask, 0x9);
            query = TerI;
            if (hit != 0) {
                query->hit_edge = TERRAIN_HIT_EDGE_1_2;
                collision_found |= 1;
                query->shape_adjusted = ((surface->normal_flags >> 1) | surface->normal_flags) & 1;
            }

            if (surface->normals[1].y < 65536.0f && secondary_start >= -query->collision_radius) {
                if (CheckCylinder(1, 3, &vertex_mask, 0x5) != 0) {
                    TerrainQuery_s *edge_query = TerI;
                    edge_query->hit_edge = TERRAIN_HIT_EDGE_1_3;
                    collision_found |= 1;
                    edge_query->shape_adjusted = (surface->normal_flags >> 1) & 1;
                }
                hit = CheckCylinder(3, 2, &vertex_mask, 0x3);
                query = TerI;
                if (hit != 0) {
                    query->hit_edge = TERRAIN_HIT_EDGE_3_2;
                    collision_found |= 1;
                    query->shape_adjusted = (surface->normal_flags >> 1) & 1;
                }
                if ((vertex_mask & (1 << 3)) != 0) {
                    const i32 vertex_hit = CheckSphere(3);
                    query = TerI;
                    if (vertex_hit != 0) {
                        collision_found = 1;
                        query->shape_adjusted = (surface->normal_flags >> 1) & 1;
                    }
                }
            }

            if (primary_start >= -query->collision_radius) {
                if ((vertex_mask & (1 << 0)) != 0) {
                    const i32 vertex_hit = CheckSphere(0);
                    query = TerI;
                    if (vertex_hit != 0) {
                        collision_found = 1;
                        query->shape_adjusted = surface->normal_flags & 1;
                    }
                }
                if ((vertex_mask & (1 << 1)) != 0) {
                    const i32 vertex_hit = CheckSphere(1);
                    query = TerI;
                    if (vertex_hit != 0) {
                        collision_found = 1;
                        query->shape_adjusted = ((surface->normal_flags >> 1) | surface->normal_flags) & 1;
                    }
                }
            }
            if ((vertex_mask & (1 << 2)) != 0) {
                const i32 vertex_hit = CheckSphere(2);
                query = TerI;
                if (vertex_hit != 0) {
                    collision_found = 1;
                    query->shape_adjusted = ((surface->normal_flags >> 1) | surface->normal_flags) & 1;
                }
            }
        }

        if (collision_found != 0) {
            query->surface = surface;
        }
        query->unclamped_hit_time = query->hit_time;
        if (query->hit_time < 0.0f) {
            query->hit_time = 0.0f;
        }
        return collision_found;
    }

    i32 HitWallSpline() {
        f32 hit = 0.0f;
        NUVEC normal;
        NUVEC edge;
        f32 temp;
        f32 absolute_x;
        f32 absolute_z;
        normal.y = 0.0f;
        if (WallSplCount == 0) {
            return hit;
        }
        for (i32 i = 0; i < WallSplCount; i += 2) {
            if (!(NU_FABS_COPY(TerI->position.x - WallSplList[i].position.x, temp, absolute_x) < 64.0f &&
                  NU_FABS_COPY(TerI->position.z - WallSplList[i].position.z, temp, absolute_z) < 64.0f)) {
                continue;
            }
            normal.x = WallSplList[i + 1].position.z - WallSplList[i].position.z;
            normal.z = WallSplList[i].position.x - WallSplList[i + 1].position.x;
            NuVecNorm(&normal, &normal);
            TerrainQuery_s *query = TerI;
            const f32 end_distance = ((query->movement.x + query->position.x) - WallSplList[i].position.x) * normal.x +
                                     ((query->movement.z + query->position.z) - WallSplList[i].position.z) * normal.z -
                                     query->collision_radius;
            const f32 start_distance = (query->position.x - WallSplList[i].position.x) * normal.x +
                                       (query->position.z - WallSplList[i].position.z) * normal.z - query->collision_radius;
            if ((end_distance < 0.0f || start_distance < 0.0f) && start_distance > -query->collision_radius) {
                f32 time;
                f32 x;
                f32 z;
                edge.x = WallSplList[i + 1].position.x - WallSplList[i].position.x;
                edge.z = WallSplList[i + 1].position.z - WallSplList[i].position.z;
                if (start_distance < 0.0f) {
                    time = start_distance;
                    x = (query->position.x - normal.x * start_distance) - normal.x * query->collision_radius;
                    z = (query->position.z - normal.z * start_distance) - normal.z * query->collision_radius;
                } else {
                    time = start_distance / (start_distance - end_distance);
                    x = (query->movement.x * time + query->position.x) - normal.x * query->collision_radius;
                    z = (query->movement.z * time + query->position.z) - normal.z * query->collision_radius;
                }
                f32 side = (x - WallSplList[i].position.x) * edge.x + (z - WallSplList[i].position.z) * edge.z;
                if (side >= -0.00005f) {
                    side = -edge.x * (x - WallSplList[i + 1].position.x) - (z - WallSplList[i + 1].position.z) * edge.z;
                    if (side >= -0.00005f && time < query->hit_time) {
                        if (start_distance > 0.0f) {
                            query->hit_type = TERRAIN_HIT_TYPE_FACE;
                        } else {
                            query->hit_type = TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE;
                        }
                        query->hit_time = time;
                        TerrWallInfo = 1;
                        query->movement_normal.x = normal.x;
                        query->movement_normal.y = normal.y;
                        query->movement_normal.z = normal.z;
                        query->terrain_group_index = -1;
                        query->shape_adjusted = 4;
                        query->surface = NULL;
                        TerrWallTab[0] = WallSplList[i].material[0];
                        TerrWallTab[1] = WallSplList[i].material[1];
                        hit = 1.0f;
                    }
                }
            }
            if (!(NU_FABS_COPY(WallSplList[i].position.x - query->position.x, temp, absolute_x) < 64.0f &&
                  NU_FABS_COPY(WallSplList[i].position.z - query->position.z, temp, absolute_z) < 64.0f)) {
                continue;
            }
            const f32 reach_sq = (query->collision_radius + 0.005f + query->horizontal_movement_length) *
                                 (query->collision_radius + 0.005f + query->horizontal_movement_length);
            if (!(reach_sq > (WallSplList[i].position.x - query->position.x) * (WallSplList[i].position.x - query->position.x) +
                                 (WallSplList[i].position.z - query->position.z) * (WallSplList[i].position.z - query->position.z))) {
                continue;
            }
            edge.x = query->movement.x;
            edge.y = 0.0f;
            edge.z = query->movement.z;
            NuVecNorm(&edge, &edge);
            const f32 along =
                (WallSplList[i].position.x - TerI->position.x) * edge.x + (WallSplList[i].position.z - TerI->position.z) * edge.z;
            bool swept = false;
            if (along >= 0.0f) {
                NUVEC closest;
                closest.x = edge.x * along + TerI->position.x;
                closest.z = edge.z * along + TerI->position.z;
                const f32 padded_radius = TerI->collision_radius + 0.0005f;
                if ((WallSplList[i].position.x - closest.x) * (WallSplList[i].position.x - closest.x) +
                        (WallSplList[i].position.z - closest.z) * (WallSplList[i].position.z - closest.z) <=
                    padded_radius * padded_radius) {
                    const f32 side_x = WallSplList[i].position.x - closest.x;
                    const f32 side_z = WallSplList[i].position.z - closest.z;
                    const f32 inside = TerI->collision_radius * TerI->collision_radius - side_x * side_x - side_z * side_z;
                    const f32 distance = along - NuFsqrt(inside);
                    if (distance >= -0.0005f &&
                        TerI->movement.x * TerI->movement.x + TerI->movement.z * TerI->movement.z >= distance * distance) {
                        const f32 movement_sq = TerI->movement.x * TerI->movement.x + TerI->movement.z * TerI->movement.z;
                        f32 time = TerrainDivideNonzeroPC(distance, NuFsqrt(movement_sq));
                        if (time < 0.0f) {
                            time = 0.0f;
                        }
                        if (!(TerI->hit_time > time)) {
                            continue;
                        }
                        normal.x = edge.x * distance + TerI->position.x - WallSplList[i].position.x;
                        normal.z = distance * edge.z + TerI->position.z - WallSplList[i].position.z;
                        NuVecNorm(&normal, &normal);
                        TerI->hit_type = 1;
                        TerI->hit_time = time;
                        swept = true;
                    }
                }
            }
            if (!swept) {
                const f32 distance_sq = (WallSplList[i].position.x - TerI->position.x) * (WallSplList[i].position.x - TerI->position.x) +
                                        (WallSplList[i].position.z - TerI->position.z) * (WallSplList[i].position.z - TerI->position.z);
                const f32 padded_radius = TerI->collision_radius + 0.0005f;
                if (distance_sq > padded_radius * padded_radius) {
                    continue;
                }
                normal.x = TerI->position.x - WallSplList[i].position.x;
                normal.z = TerI->position.z - WallSplList[i].position.z;
                NuVecNorm(&normal, &normal);
                TerI->hit_type = 0x11;
                TerI->hit_time = (NuFsqrt(distance_sq) - TerI->collision_radius) - 0.0005f;
            }
            query = TerI;
            TerrWallInfo = 1;
            query->movement_normal.x = normal.x;
            query->movement_normal.y = normal.y;
            query->movement_normal.z = normal.z;
            query->shape_adjusted = 4;
            query->terrain_group_index = -1;
            query->surface = NULL;
            TerrWallTab[0] = WallSplList[i].material[0];
            TerrWallTab[1] = WallSplList[i].material[1];
            hit = 1.0f;
        }
        TerI->unclamped_hit_time = TerI->hit_time;
        if (TerI->hit_time < 0.0f) {
            TerI->hit_time = 0.0f;
        }
        return hit;
    }

    i32 HitTerrain() {
        f32 primary_start_distance = 0.0f;
        f32 secondary_start_distance = 0.0f;
        f32 secondary_end_distance = 0.0f;
        TerrainQuery_s *query = TerI;
        i16 *scan = static_cast<i16 *>(TerrOverRideScan);
        if (scan == NULL) {
            scan = reinterpret_cast<i16 *>(query->scan_list_storage);
        }

        const f32 radius = query->collision_radius;
        i32 collision_found = 0;
        query->shape_adjusted = 0;
        query->hit_time = 999.9f;
        query->hit_type = TERRAIN_HIT_TYPE_NONE;
        query->terrain_group_index = -1;

        HitWallSpline();

        query = TerI;
        const f64 horizontal_length = NuFsqrt(query->movement.x * query->movement.x + query->movement.z * query->movement.z);
        query = TerI;
        query->horizontal_movement_length = horizontal_length;

        for (;;) {
            while (scan[0] > 0) {
                TERRAIN_GROUP *groups = CurrentTerrain->groups;
                TERRAIN_POLYGON_INFO **entry = reinterpret_cast<TERRAIN_POLYGON_INFO **>(scan + 2);
                query->local_start.x = query->position.x - groups[scan[1]].origin.x;
                query->local_start.y = query->position.y - groups[scan[1]].origin.y;
                query->local_start.z = query->position.z - groups[scan[1]].origin.z;
                query->local_end.x = query->movement.x + query->position.x - groups[scan[1]].origin.x;
                query->local_end.y = query->movement.y + query->position.y - groups[scan[1]].origin.y;
                query->local_end.z = query->movement.z + query->position.z - groups[scan[1]].origin.z;

                for (i32 count = scan[0]; count > 0; --count, ++entry) {
                    TERRAIN_POLYGON_INFO *surface = *entry;
                    i32 crosses_surface = 0;
                    const f32 primary_end_distance = ((query->local_end.y - surface->vertices[0].y) * surface->normals[0].y +
                                                      (query->local_end.x - surface->vertices[0].x) * surface->normals[0].x +
                                                      (query->local_end.z - surface->vertices[0].z) * surface->normals[0].z) -
                                                     radius - query->compare_epsilon;
                    if (primary_end_distance < 0.0f) {
                        primary_start_distance = ((query->local_start.y - surface->vertices[0].y) * surface->normals[0].y +
                                                  (query->local_start.x - surface->vertices[0].x) * surface->normals[0].x +
                                                  (query->local_start.z - surface->vertices[0].z) * surface->normals[0].z) -
                                                 radius;
                        if (primary_start_distance > -radius) {
                            crosses_surface = 1;
                        }
                    }
                    if (surface->normals[1].y < 65536.0f) {
                        secondary_end_distance = ((query->local_end.y - surface->vertices[3].y) * surface->normals[1].y +
                                                  (query->local_end.x - surface->vertices[3].x) * surface->normals[1].x +
                                                  (query->local_end.z - surface->vertices[3].z) * surface->normals[1].z) -
                                                 radius - query->compare_epsilon;
                        if (secondary_end_distance < 0.0f) {
                            secondary_start_distance = ((query->local_start.y - surface->vertices[3].y) * surface->normals[1].y +
                                                        (query->local_start.x - surface->vertices[3].x) * surface->normals[1].x +
                                                        (query->local_start.z - surface->vertices[3].z) * surface->normals[1].z) -
                                                       radius;
                            if (secondary_start_distance > -radius) {
                                crosses_surface = 1;
                            }
                        }
                    }

                    if (crosses_surface != 0) {
                        const i32 hit = HitPoly(primary_start_distance, primary_end_distance, secondary_start_distance,
                                                secondary_end_distance, surface);
                        query = TerI;
                        if (hit != 0) {
                            query->terrain_group_index = scan[1];
                            collision_found = 1;
                            if ((query->hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) != 0 &&
                                (query->surface->flags & TERRAIN_SHAPE_FLAG_MATERIAL_ALIAS) != 0) {
                                PlatCrush = query->surface->material[0] + 1;
                            }
                        }
                    }
                }
                scan = reinterpret_cast<i16 *>(entry);
            }

            if (scan[0] >= 0) {
                break;
            }
            do {
                scan += abs(scan[0]) * 2 + 2;
            } while (scan[0] < 0);
        }

        for (i32 sphere_index = 0; sphere_index < curSphereter; ++sphere_index) {
            NUVEC sphere_position = SphereData[sphere_index].position;
            sphere_position.y = query->inverse_object_scale * sphere_position.y - query->object_scale * query->collision_radius;
            DeRotatePoint(&sphere_position);
            collision_found |= CheckSphereTer(&sphere_position, SphereData[sphere_index].radius);
            query = TerI;
        }

        if (query->hit_type != TERRAIN_HIT_TYPE_NONE && query->terrain_group_index != -1) {
            TERRAIN_GROUP *group = &CurrentTerrain->groups[query->terrain_group_index];
            if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
                plathitid = group->scene_index;
            }
        }

        TerrOverRideScan = NULL;
        return collision_found;
    }

// auto_inline(off): keeps VC8 from inlining the helpers below into NewTerrainScaleYMask, as the original does not.
#pragma auto_inline(off)

    void NewTerrStoreAnyInfo() {
        TerrainQuery_s *query = TerI;
        if (query->surface == NULL || query->terrain_group_index == -1) {
            return;
        }

        if (query->surface->material[0] != 0) {
            TerrPolyInfo.material[0] = query->surface->material[0];
        }
        if (query->surface->material[1] != 0) {
            TerrPolyInfo.material[1] = query->surface->material[1];
        }
        if (query->surface->flags != 0) {
            TerrPolyInfo.flags = query->surface->flags;
        }
        if (query->surface->normal_flags != 0) {
            TerrPolyInfo.normal_flags = query->surface->normal_flags;
        }
    }

#pragma auto_inline(on)

    void TerrainMoveImpactData() {
        TerrainQuery_s *query = TerI;
        switch (query->hit_type) {
            case TERRAIN_HIT_TYPE_FACE:
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SPHERE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE:
                if (query->terrain_group_index != -1) {
                    NewTerrStoreAnyInfo();
                    const i32 group_index = query->terrain_group_index;
                    TerrPoly = &TerrPolyInfo;
                    TerrPolyObj = group_index;
                }
                break;
        }
    }

    void TerrainImpactNorm() {
        TerrainMoveImpactData();

        TerrainQuery_s *query = TerI;
        switch (query->hit_type) {
            case TERRAIN_HIT_TYPE_SPHERE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE:
                terrhitflags |= 4;
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_VERTEX: {
                NUVEC *normal = &query->movement_normal;
                RotateVec(normal, normal);
                // Curved hits are produced in collision-height-scaled space and
                // need to be transformed back.
                if ((query->hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) == 0) {
                    query->movement_normal.x *= query->inverse_collision_radius;
                    const f64 inverse_radius = query->inverse_collision_radius;
                    query->movement_normal.y = inverse_radius * query->movement_normal.y;
                    query->movement_normal.z *= query->inverse_collision_radius;
                }
            }
            case TERRAIN_HIT_TYPE_FACE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE:
                if (query->object_scale == 1.0f) {
                    query->impact_normal.x = query->movement_normal.x;
                    query->impact_normal.y = query->movement_normal.y;
                    query->impact_normal.z = query->movement_normal.z;
                    return;
                }
                {
                    const f32 normal_length = NuFsqrt(query->movement_normal.x * query->movement_normal.x +
                                                      query->movement_normal.y * query->movement_normal.y * query->inverse_object_scale_sq +
                                                      query->movement_normal.z * query->movement_normal.z);
                    const f32 inverse_normal_length = normal_length != 0.0f ? (f32)(1.0 / normal_length) : 0.0f;
                    query = TerI;
                    query->impact_normal.x = query->movement_normal.x * inverse_normal_length;
                    query->impact_normal.y = query->movement_normal.y * query->inverse_object_scale * inverse_normal_length;
                    query->impact_normal.z = query->movement_normal.z * inverse_normal_length;
                }
                break;
        }
    }

    TERRAIN_POLYGON_INFO PlatImpactTer = {0};
    NUVEC PlatImpactNorm = {0};
    void *TerImpactData = 0;

    void StorePlatImpact() {
        TerrainQuery_s *query = TerI;
        const u8 hit_type = query->hit_type;
        if (hit_type == TERRAIN_HIT_TYPE_NONE || query->terrain_group_index == -1) {
            return;
        }

        TERRAIN_GROUP *group = &CurrentTerrain->groups[query->terrain_group_index];
        if (group->chunk_type != TERRAIN_CHUNK_GROUP_SECONDARY) {
            return;
        }

        castnum = query->terrain_group_index;
        NUVEC impact_normal = query->movement_normal;

        switch (hit_type) {
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SPHERE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE:
                RotateVec(&impact_normal, &impact_normal);
                if ((hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) == 0) {
                    impact_normal.x *= query->inverse_collision_radius;
                    impact_normal.y *= query->inverse_collision_radius;
                    impact_normal.z *= query->inverse_collision_radius;
                }
            case TERRAIN_HIT_TYPE_FACE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE:
                if (query->object_scale != 1.0f) {
                    const f32 inverse_normal_length =
                        TerrainDivideNonzeroPC(1.0f, NuFsqrt(impact_normal.x * impact_normal.x +
                                                             impact_normal.y * impact_normal.y * query->inverse_object_scale_sq +
                                                             impact_normal.z * impact_normal.z));
                    query = TerI;
                    impact_normal.x *= inverse_normal_length;
                    impact_normal.y *= query->inverse_object_scale * inverse_normal_length;
                    impact_normal.z *= inverse_normal_length;
                }

                PlatImpactId = CurrentTerrain->groups[query->terrain_group_index].scene_index;
                PlatImpactTer = *query->surface;
                PlatImpactNorm = impact_normal;
                break;
        }
    }

    f32 wallover = 0;
    TERRAIN_TRACK_SLOT *CurTrackInfo = 0;

    void TerrainImpact(NUVEC *position, NUVEC *movement, u8 *hit_flags) {
        f32 walkable_normal_y = 0.0f;
        TERRAIN_TRACK_SLOT *track;
        TerrainMoveImpactData();

        TerrainQuery_s *query = TerI;
        if (query->hit_type != TERRAIN_HIT_TYPE_NONE) {
            TERRAIN_POLYGON_INFO *surface = query->surface;
            if (surface != NULL) {
                walkable_normal_y = query->movement_normal.x * surface->normals[0].x + query->movement_normal.y * surface->normals[0].y +
                                    query->movement_normal.z * surface->normals[0].z;
            } else {
                walkable_normal_y = 0.707f;
            }

            if ((query->terrain_group_index != -1 &&
                 CurrentTerrain->groups[query->terrain_group_index].chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY)) {
                if ((query->hit_type & TERRAIN_HIT_TYPE_CLASS_MASK) > TERRAIN_HIT_TYPE_FACE && walkable_normal_y < (f64)0.95f) {
                    walkable_normal_y = 0.98f;
                } else {
                    walkable_normal_y = 0.707f;
                }
            } else if (query->shape_adjusted != 0) {
                walkable_normal_y = 1.1f;
            } else {
                walkable_normal_y = -1.1f;
            }

            if (query->terrain_group_index >= 0 &&
                (surface->normal_flags & TERRAIN_SURFACE_CLASS_MASK) == TERRAIN_SURFACE_CLASS_WALL_OVERRIDE) {
                query->position.x += query->movement_normal.x * (f64)0.0015f;
                query->position.z += query->movement_normal.z * (f64)0.0015f;
                walkable_normal_y = 1.1f;
            }
            if (wallover != 0.0f) {
                walkable_normal_y = wallover;
            }
        }

        switch (query->hit_type) {
            case TERRAIN_HIT_TYPE_NONE:
                hit_flags[0] = 0;
                position->x = query->movement.x + query->position.x;
                position->y = (query->movement.y + query->position.y) * query->object_scale - query->collision_radius * query->object_scale;
                position->z = query->movement.z + query->position.z;
                query->flags &= ~TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL;
                break;
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SPHERE:
                if (!(query->terrain_group_index >= 0 &&
                      (query->surface->normal_flags & TERRAIN_SURFACE_CLASS_MASK) == TERRAIN_SURFACE_CLASS_WALL_OVERRIDE)) {
                    walkable_normal_y = 0.707f;
                }
                // Fall through to the shared first-normal response.
            case TERRAIN_HIT_TYPE_FACE: {
                query->hit_time -= query->separation_epsilon;
                if (query->hit_time < 0.0f) {
                    query->hit_time = 0.0f;
                }

                const f64 travelled_x = query->movement.x * query->hit_time;
                query->position.x += travelled_x;
                const f64 travelled_y = query->movement.y * query->hit_time;
                query->position.y += travelled_y;
                const f64 travelled_z = query->movement.z * query->hit_time;
                query->position.z += travelled_z;
                query->movement.x -= travelled_x;
                query->movement.y -= travelled_y;
                query->movement.z -= travelled_z;

                if (query->impact_normal.y < walkable_normal_y) {
                    hit_flags[0] = 0;
                    terrhitflags |= TERRAIN_IMPACT_RESULT_WALL;
                    if (!(query->terrain_group_index != -1 &&
                          CurrentTerrain->groups[query->terrain_group_index].chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY)) {
                        terrhitflags |= TERRAIN_IMPACT_RESULT_STATIC_WALL;
                    }

                    query->position.x += query->movement_normal.x * (f64)0.0006f;
                    query->position.z += query->movement_normal.z * (f64)0.0006f;
                    query->movement.x += query->movement_normal.x * (f64)0.0006f;
                    query->movement.z += query->movement_normal.z * (f64)0.0006f;
                    movement->x += query->movement_normal.x * (f64)0.0009f;
                    movement->z += query->movement_normal.z * (f64)0.0009f;

                    FullDeflect(&query->movement_normal, &query->movement, &query->movement);
                    FullDeflectSmallY(&query->impact_normal, movement, movement);

                    track = CurTrackInfo;
                    if (track == NULL) {
                        track = AllocTerrId();
                        track->id = query->hit_flags;
                        CurTrackInfo = track;
                        track->flags = TERRAIN_TRACK_FLAG_NONE;
                        track->platform_index = 0;
                        track->platform_contact_state = TERRAIN_TRACK_CONTACT_NONE;
                    }
                    track->wall_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
                    track->impact_normal = query->impact_normal;
                    TerrWallNorm = query->impact_normal;
                } else {
                    FullDeflect(&query->movement_normal, &query->movement, &query->movement);
                    FullDeflect(&query->impact_normal, movement, movement);

                    terrhitflags |= TERRAIN_IMPACT_RESULT_GROUND;
                    hit_flags[0] = 1;
                    hit_flags[1] = 1;
                    query->position.y += query->movement_normal.y * (f64)0.0003f;

                    if ((query->terrain_group_index != -1 &&
                         CurrentTerrain->groups[query->terrain_group_index].chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY)) {
                        CurrentTerrain->platforms[CurrentTerrain->groups[query->terrain_group_index].scene_index].flags |=
                            TERRAIN_PLATFORM_FLAG_COLLIDED;
                    }
                }
                query->flags &= ~TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL;
                break;
            }
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE:
                walkable_normal_y = 0.707f;
                // Fall through to the shared second-normal response.
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE: {
                NUVEC response_normal = query->movement_normal;
                if ((query->flags & TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL) != 0) {
                    if (response_normal.x * query->previous_movement_normal.x + response_normal.y * query->previous_movement_normal.y +
                            response_normal.z * query->previous_movement_normal.z <
                        0.0) {
                        NUVEC corrected_normal;
                        FullDeflect(&response_normal, &query->previous_movement_normal, &corrected_normal);
                        const f32 correction_alignment = corrected_normal.x * response_normal.x + corrected_normal.y * response_normal.y +
                                                         corrected_normal.z * response_normal.z;
                        if (correction_alignment > 0.25f) {
                            response_normal.x = corrected_normal.x / correction_alignment;
                            response_normal.y = corrected_normal.y / correction_alignment;
                            response_normal.z = corrected_normal.z / correction_alignment;
                        }
                    }
                }

                if (query->impact_normal.y < walkable_normal_y) {
                    const f64 contact_push = (f64)0.0035f - query->unclamped_hit_time * (f64)1.05f;
                    query->position.x += response_normal.x * contact_push;
                    query->position.y += response_normal.y * contact_push;
                    query->position.z += response_normal.z * contact_push;
                    const f64 movement_push = (f64)0.0035f - query->unclamped_hit_time * (f64)0.35f;
                    query->movement.x += response_normal.x * movement_push;
                    query->movement.y += response_normal.y * movement_push;
                    query->movement.z += response_normal.z * movement_push;
                    hit_flags[0] = 1;

                    FullDeflect(&response_normal, &query->movement, &query->movement);
                    FullDeflectSmallY(&query->impact_normal, movement, movement);

                    track = CurTrackInfo;
                    if (track == NULL) {
                        track = AllocTerrId();
                        track->id = query->hit_flags;
                        CurTrackInfo = track;
                        track->flags = TERRAIN_TRACK_FLAG_NONE;
                        track->platform_index = 0;
                        track->platform_contact_state = TERRAIN_TRACK_CONTACT_NONE;
                    }
                    track->wall_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
                    track->impact_normal = query->impact_normal;
                } else {
                    const f64 contact_push = (f64)0.0035f - query->unclamped_hit_time * (f64)1.05f;
                    query->position.x += response_normal.x * contact_push;
                    query->position.y += response_normal.y * contact_push;
                    query->position.z += response_normal.z * contact_push;
                    query->position.y += response_normal.y * (f64)0.01f;
                    query->movement.y += response_normal.y * (f64)0.01f;

                    FullDeflect(&response_normal, &query->movement, &query->movement);
                    FullDeflect(&query->impact_normal, movement, movement);
                }
                query->flags |= TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL;
                query->previous_movement_normal = query->movement_normal;
                break;
            }
        }

        if (query->hit_type != TERRAIN_HIT_TYPE_NONE && query->hit_type < TERRAIN_HIT_TYPE_SECOND_NORMAL &&
            (query->terrain_group_index != -1 &&
             CurrentTerrain->groups[query->terrain_group_index].chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) &&
            query->impact_normal.y >= walkable_normal_y) {
            PlatformConnect(reinterpret_cast<char *>(query->hit_flags), &query->movement, movement,
                            CurrentTerrain->groups[query->terrain_group_index].scene_index);
        }

        if (TerImpactData != NULL && *TerImpactDataCount < TerImpactDataMax && query->hit_type != TERRAIN_HIT_TYPE_NONE) {
            TERRAIN_IMPACT_RECORD *records = static_cast<TERRAIN_IMPACT_RECORD *>(TerImpactData);
            i32 *count = TerImpactDataCount;
            records[*count].position.x = query->position.x - query->movement_normal.x * query->collision_radius;
            records[*count].position.y = (query->position.y - query->movement_normal.y * query->collision_radius) * query->object_scale;
            records[*count].position.z = query->position.z - query->movement_normal.z * query->collision_radius;
            records[*count].normal = query->movement_normal;
            if (query->surface != NULL) {
                records[*count].material[0] = query->surface->material[0];
                records[*count].material[1] = query->surface->material[1];
                records[*count].flags = query->surface->flags;
                records[*count].normal_flags = query->surface->normal_flags;
            } else if (TerrWallInfo != 0) {
                records[*count].material[0] = TerrWallTab[0];
                records[*count].material[1] = TerrWallTab[1];
                records[*count].flags = 0;
                records[*count].normal_flags = 0;
            }
            ++*count;
        }
    }

    i32 TerrainImpactPlatform(unsigned char *hit_flags) {
        TerrainMoveImpactData();

        TerrainQuery_s *query = TerI;
        switch (query->hit_type) {
            case TERRAIN_HIT_TYPE_NONE:
                hit_flags[0] = 0;
                query->position.x += query->movement.x;
                query->position.y += query->movement.y;
                query->position.z += query->movement.z;
                return 0;
            case TERRAIN_HIT_TYPE_FACE:
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SPHERE:
                query->hit_time -= query->separation_epsilon;
                if (query->hit_time < 0.0f) {
                    query->hit_time = 0.0f;
                }

                query->position.x += query->movement.x * query->hit_time;
                query->position.y += query->movement.y * query->hit_time;
                query->position.z += query->movement.z * query->hit_time;

                if (query->impact_normal.y < 0.707f) {
                    hit_flags[0] = 0;
                    return 0;
                }

                hit_flags[0] = 1;
                hit_flags[1] = 1;
                query->position.y += query->movement_normal.y * 0.004f;
                return 0;
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE:
                return 1;
        }
        return 1;
    }

    void FullDeflect(NUVEC *normal, NUVEC *movement, NUVEC *result) {
        // Move just far enough out of the surface to retain a small separation.
        const f32 reverse_x = -movement->x;
        const f32 reverse_y = -movement->y;
        const f32 reverse_z = -movement->z;
        const f32 deflection = reverse_y * normal->y + reverse_x * normal->x + reverse_z * normal->z + 0.0003f;
        result->x = normal->x * deflection + movement->x;
        result->y = normal->y * deflection + movement->y;
        result->z = normal->z * deflection + movement->z;
    }

    void FullDeflectSmallY(NUVEC *normal, NUVEC *movement, NUVEC *result) {
        // Move just far enough out of the surface to retain a small separation.
        const f32 reverse_x = -movement->x;
        const f32 reverse_y = -movement->y;
        const f32 reverse_z = -movement->z;
        const f32 deflection = reverse_y * normal->y + reverse_x * normal->x + reverse_z * normal->z + 0.0003f;
        result->x = normal->x * deflection + movement->x;
        result->y = normal->y * deflection * TerrWallDeflectYScale + movement->y;
        result->z = normal->z * deflection + movement->z;
    }

    i32 FullDeflectTest(NUVEC *normal, NUVEC *movement, NUVEC *result) {
        const f32 reverse_x = -movement->x;
        const f32 reverse_y = -movement->y;
        const f32 reverse_z = -movement->z;
        const f32 deflection = reverse_y * normal->y + reverse_x * normal->x + reverse_z * normal->z + 0.0003f;
        result->x = normal->x * deflection + movement->x;
        result->y = normal->y * deflection + movement->y;
        result->z = normal->z * deflection + movement->z;
        return deflection > 0.0f;
    }

    void PlatformConnect(char *track_id, NUVEC *position_delta, NUVEC *movement_delta, i32 platform_index) {
        TERRAIN_TRACK_SLOT *track = CurTrackInfo;
        if (track != NULL) {
            track->flags |= TERRAIN_TRACK_FLAG_CONNECTED;
            track->platform_index = static_cast<i16>(platform_index);
            track->platform_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
        } else {
            track = AllocTerrId();
            CurTrackInfo = track;
            if (track != NULL) {
                track->id = track_id;
                track->flags |= TERRAIN_TRACK_FLAG_CONNECTED;
                track->platform_index = static_cast<i16>(platform_index);
                track->platform_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
                position_delta->y = 0.0f;
                movement_delta->y = 0.0f;
            }
        }

        castnum = CurrentTerrain->platforms[platform_index].terrain_group_index;
    }

    NUMTX tertempmat = {0};

    i32 PlatformChecks(i32 count, NUVEC *movement) {
        TerrainQuery_s *old_query = TerI;
        if (old_query->scan_list == NULL || *static_cast<i16 *>(old_query->scan_list) == 0 || CurTrackInfo == NULL ||
            CurTrackInfo->platform_contact_state == 0 || TerConTol == 0.0f) {
            return count;
        }
        TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
        i16 *input = static_cast<i16 *>(old_query->scan_list);
        TerI = query;
        i16 *output = reinterpret_cast<i16 *>(query->scan_list_storage);
        while (*input > 0) {
            output[0] = input[0];
            i32 remaining = input[0];
            output[1] = input[1];
            output += 2;
            input += 2;
            for (; remaining > 0; --remaining) {
                *output++ = *input++;
                *output++ = *input++;
            }
        }
        output[0] = 0;
        output[1] = 0;
        query->collision_radius = old_query->collision_radius;
        query->object_scale = old_query->object_scale;
        query->inverse_object_scale_sq = old_query->inverse_object_scale_sq;
        query->inverse_collision_radius = old_query->inverse_collision_radius;
        query->separation_epsilon = old_query->separation_epsilon;
        query->inverse_object_scale = old_query->inverse_object_scale;
        query->compare_epsilon = old_query->compare_epsilon;
        query->collision_radius_sq = old_query->collision_radius_sq;
        query->object_scale_sq = old_query->object_scale_sq;
        TerrainQuery_s *current = query;
        current->platform_position = old_query->position;
        TERRAIN_TRACK_SLOT *track = CurTrackInfo;
        if (track != NULL && (track->flags & TERRAIN_TRACK_FLAG_CONNECTED)) {
            TERRAIN_PLATFORM *platform = &CurrentTerrain->platforms[track->platform_index];
            if (platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING) {
                NuMtxInvRSS(&tertempmat, reinterpret_cast<NUMTX *>(platform));
                track = CurTrackInfo;
                tertempvec4.x = old_query->position.x - CurrentTerrain->platforms[track->platform_index].previous_matrix.m30;
                tertempvec4.y = (old_query->position.y - TerI->collision_radius) * TerI->object_scale -
                                CurrentTerrain->platforms[track->platform_index].previous_matrix.m31;
                tertempvec4.z = old_query->position.z - CurrentTerrain->platforms[track->platform_index].previous_matrix.m32;
                tertempvec4.w = 1.0f;
                NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, &tertempmat);
                track = CurTrackInfo;
                NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, CurrentTerrain->platforms[track->platform_index].scene_object);
                current = TerI;
                current->platform_position.x = tertempvec4.x;
                current->platform_position.y = current->inverse_object_scale * tertempvec4.y + current->collision_radius;
                current->platform_position.z = tertempvec4.z;
            } else {
                query->platform_position.x += CurrentTerrain->platforms[track->platform_index].scene_object->m30 -
                                              CurrentTerrain->platforms[track->platform_index].previous_matrix.m30;
                query->platform_position.y += (CurrentTerrain->platforms[track->platform_index].scene_object->m31 -
                                               CurrentTerrain->platforms[track->platform_index].previous_matrix.m31) *
                                              old_query->inverse_object_scale;
                query->platform_position.z += CurrentTerrain->platforms[track->platform_index].scene_object->m32 -
                                              CurrentTerrain->platforms[track->platform_index].previous_matrix.m32;
            }
        }
        current->platform_normal = current->platform_position;
        current->movement.x = 0.0f;
        current->movement.z = 0.0f;
        current->position = current->platform_position;
        const f32 tolerance = TerConTol;
        current->position.y += tolerance;
        current->movement.y = tolerance * -2.0;
        DerotateMovementVector();
        f32 saved_epsilon = TerI->separation_epsilon;
        TerI->separation_epsilon = 0.0f;
        HitTerrain();
        StorePlatImpact();
        TerI->separation_epsilon = saved_epsilon;
        TerrainImpactNorm();
        u8 flags[2];
        TerrainImpactPlatform(flags);
        current = TerI;
        if (current->hit_type != 0) {
            ShadNorm = current->impact_normal;
        }
        current->platform_position.y = current->position.y - current->collision_radius;
        TerI = old_query;
        TERRAIN_FLOAT_BITS absolute_4;
        absolute_4.value = query->platform_normal.y - query->platform_position.y - old_query->collision_radius;
        absolute_4.bits &= 0x7fffffff;
        if (query->hit_type == 0 || absolute_4.value > TerConTol * 0.5 || query->movement_normal.y < 0.707f) {
            NuScratchRelease();
            return count;
        }
        {
            {
                TERRAIN_GROUP *group = &CurrentTerrain->groups[query->terrain_group_index];
                i32 platform_index = group->scene_index;
                if (movement->y > TerConTol * 0.5) {
                    TERRAIN_PLATFORM *platform = &CurrentTerrain->platforms[platform_index];
                    if (movement->y > TerConTol * 0.5 + platform->scene_object->m31 - platform->previous_matrix.m31) {
                        NuScratchRelease();
                        return count;
                    }
                }
                if (query->movement_normal.y >= 0.707f && query->terrain_group_index >= 0 && group->chunk_type == 1) {
                    CurrentTerrain->platforms[group->scene_index].flags |= TERRAIN_PLATFORM_FLAG_COLLIDED;
                }
                old_query->platform_position.x = query->platform_position.x;
                track = CurTrackInfo;
                if (CurrentTerrain->platforms[track->platform_index].flags & TERRAIN_PLATFORM_FLAG_ROTATING) {
                    old_query->platform_position.y = old_query->collision_radius + query->platform_position.y;
                } else {
                    old_query->platform_position.y = old_query->collision_radius + query->platform_position.y + (f64)0.0025f;
                }
                const f32 platform_z = query->platform_position.z;
                old_query->platform_position.z = platform_z;
                NUVEC direction;
                direction.x = old_query->platform_position.x - old_query->position.x;
                direction.y = old_query->platform_position.y - old_query->position.y;
                direction.z = platform_z - old_query->position.z;
                f32 distance = NuFsqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
                direction.x = TerrainDivideNonzeroPC(direction.x, distance);
                direction.y = TerrainDivideNonzeroPC(direction.y, distance);
                direction.z = TerrainDivideNonzeroPC(direction.z, distance);
                NUVEC previous_position = TerI->position;
                i32 clear = TerrainPlatformMoveCheck(&TerI->platform_position, &TerI->platform_normal, platform_index, 1, 0);
                current = TerI;
                if (current->hit_type != 0 && current->terrain_group_index >= 0) {
                    TERRAIN_GROUP *hit_group = &CurrentTerrain->groups[current->terrain_group_index];
                    if (hit_group->chunk_type == 1) {
                        CurTrackInfo->platform_index = hit_group->scene_index;
                    }
                    if (current->platform_normal.y * direction.y + current->platform_normal.x * direction.x +
                            current->platform_normal.z * direction.z <
                        TerrainOpposingContactThreshold) {
                        clear = 1;
                    }
                }
                --count;
                if (clear == 0) {
                    if (count > 0) {
                        const f32 travelled = (current->position.x - previous_position.x) * direction.x +
                                              (current->position.y - previous_position.y) * direction.y +
                                              (current->position.z - previous_position.z) * direction.z;
                        if (distance > travelled && FullDeflectTest(&current->platform_normal, &direction, &current->platform_position)) {
                            const f32 dot = current->platform_position.y * direction.y + current->platform_position.x * direction.x +
                                            current->platform_position.z * direction.z;
                            if (dot > 0.0f) {
                                const f32 fraction = (distance - travelled) / dot;
                                if (fraction < (f64)0.8f) {
                                    current->platform_position.x = current->platform_position.x * fraction + current->position.x;
                                    current->platform_position.y = current->platform_position.y * fraction + current->position.y;
                                    current->platform_position.z = current->platform_position.z * fraction + current->position.z;
                                }
                            }
                        }
                    }
                } else {
                    TerrPoly = query->surface;
                    current->position = current->platform_position;
                    if (0.0f > movement->y) {
                        FullDeflect(&ShadNorm, &current->movement, &current->movement);
                        FullDeflect(&ShadNorm, movement, movement);
                    } else {
                        current->movement.y *= (f64)0.75f;
                    }
                    current->hit_flags[1] = 1;
                    PlatformConnect(reinterpret_cast<char *>(old_query->hit_flags), &old_query->movement, movement,
                                    CurrentTerrain->groups[query->terrain_group_index].scene_index);
                }
                NuScratchRelease();
                TerI = old_query;
                return count;
            }
        }
    }

    i32 TerrainPlatformEmbedded(NUVEC *movement_delta) {
        i32 transformed_count = 0;
        TerrainQuery_s *old_query = TerI;
        i32 platform_index = CurrentTerrain->groups[old_query->terrain_group_index].scene_index;
        TERRAIN_PLATFORM *platform = &CurrentTerrain->platforms[platform_index];
        if (platform->scene_object == NULL) {
            return 1;
        }
        if (CurrentTerrain->platforms[platform_index].previous_matrix.m30 == CurrentTerrain->platforms[platform_index].scene_object->m30 &&
            CurrentTerrain->platforms[platform_index].previous_matrix.m31 == CurrentTerrain->platforms[platform_index].scene_object->m31 &&
            CurrentTerrain->platforms[platform_index].previous_matrix.m32 == CurrentTerrain->platforms[platform_index].scene_object->m32) {
            if (!(platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING)) {
                return 1;
            }
            if (CurrentTerrain->platforms[platform_index].previous_matrix.m00 ==
                    CurrentTerrain->platforms[platform_index].scene_object->m00 &&
                CurrentTerrain->platforms[platform_index].previous_matrix.m01 ==
                    CurrentTerrain->platforms[platform_index].scene_object->m01 &&
                CurrentTerrain->platforms[platform_index].previous_matrix.m02 ==
                    CurrentTerrain->platforms[platform_index].scene_object->m02 &&
                CurrentTerrain->platforms[platform_index].previous_matrix.m10 ==
                    CurrentTerrain->platforms[platform_index].scene_object->m10 &&
                CurrentTerrain->platforms[platform_index].previous_matrix.m11 ==
                    CurrentTerrain->platforms[platform_index].scene_object->m11 &&
                CurrentTerrain->platforms[platform_index].previous_matrix.m12 ==
                    CurrentTerrain->platforms[platform_index].scene_object->m12 &&
                CurrentTerrain->platforms[platform_index].previous_matrix.m20 ==
                    CurrentTerrain->platforms[platform_index].scene_object->m20 &&
                CurrentTerrain->platforms[platform_index].previous_matrix.m21 ==
                    CurrentTerrain->platforms[platform_index].scene_object->m21 &&
                CurrentTerrain->platforms[platform_index].previous_matrix.m22 ==
                    CurrentTerrain->platforms[platform_index].scene_object->m22) {
                return 1;
            }
        }
        i32 saved_sphere = curSphereter;
        curSphereter = 0;
        ScaleTerrain = static_cast<TERRAIN_POLYGON_INFO *>(ScaleTerrainT2);
        TerrainQuery_s *new_query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
        TerI = new_query;
        NUVEC4 *vertices = reinterpret_cast<NUVEC4 *>((reinterpret_cast<u32>(NuScratchAlloc32(0xd0)) + 31) & ~15u);
        TerrainQuery_s *query = TerI;
        query->collision_radius = old_query->collision_radius;
        query->object_scale = old_query->object_scale;
        query->inverse_object_scale_sq = old_query->inverse_object_scale_sq;
        query->inverse_collision_radius = old_query->inverse_collision_radius;
        query->separation_epsilon = old_query->separation_epsilon;
        query->inverse_object_scale = old_query->inverse_object_scale;
        query->compare_epsilon = old_query->compare_epsilon;
        query->collision_radius_sq = old_query->collision_radius_sq;
        query->object_scale_sq = old_query->object_scale_sq;
        query->object_index = old_query->object_index;
        query->hit_flags = old_query->hit_flags;
        query->scan_result = old_query->scan_result;
        query->radius = old_query->radius;
        query->movement.x =
            (CurrentTerrain->platforms[platform_index].previous_matrix.m30 - CurrentTerrain->platforms[platform_index].scene_object->m30) *
            (f64)1.73f;
        query->movement.y =
            (CurrentTerrain->platforms[platform_index].previous_matrix.m31 - CurrentTerrain->platforms[platform_index].scene_object->m31) *
            (f64)1.73f * query->inverse_object_scale;
        query->movement.z =
            (CurrentTerrain->platforms[platform_index].previous_matrix.m32 - CurrentTerrain->platforms[platform_index].scene_object->m32) *
            (f64)1.73f;
        if (CurrentTerrain->platforms[platform_index].flags & TERRAIN_PLATFORM_FLAG_ROTATING) {
            NuMtxInvRSS(&tertempmat, &CurrentTerrain->platforms[platform_index].previous_matrix);
            query = TerI;
            tertempvec4.x = old_query->position.x - CurrentTerrain->platforms[platform_index].previous_matrix.m30;
            tertempvec4.y = (old_query->position.y - query->collision_radius) * query->object_scale -
                            CurrentTerrain->platforms[platform_index].previous_matrix.m31;
            tertempvec4.z = old_query->position.z - CurrentTerrain->platforms[platform_index].previous_matrix.m32;
            tertempvec4.w = 1.0f;
            NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, &tertempmat);
            NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, CurrentTerrain->platforms[platform_index].scene_object);
            query = TerI;
            query->movement.x = (old_query->position.x - tertempvec4.x) * (f64)1.3f;
            query->movement.y =
                (old_query->position.y - (query->inverse_object_scale * tertempvec4.y + query->collision_radius)) * (f64)1.3f;
            query->movement.z = (old_query->position.z - tertempvec4.z) * (f64)1.3f;
        }
        TERRAIN_POLYGON_INFO **cursor = reinterpret_cast<TERRAIN_POLYGON_INFO **>(query->scan_list_storage + 4);
        i16 *header = reinterpret_cast<i16 *>(query->scan_list_storage);
        query->movement.x -= old_query->movement_normal.x * (f64)0.01f;
        query->movement.y -= old_query->movement_normal.y * (f64)0.01f;
        query->movement.z -= old_query->movement_normal.z * (f64)0.01f;
        query->position.x = old_query->position.x - query->movement.x * (f64)0.91f;
        query->position.y = old_query->position.y - query->movement.y * (f64)0.91f;
        query->position.z = old_query->position.z - query->movement.z * (f64)0.91f;

        i32 count = 0;
        TERRAIN_GROUP *group = &CurrentTerrain->groups[old_query->terrain_group_index];
        TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
        if (1.0f == query->object_scale) {
            if (CurrentTerrain->platforms[platform_index].flags & TERRAIN_PLATFORM_FLAG_ROTATING) {
                while (batch->marker >= 0) {
                    TERRAIN_POLYGON_INFO *source = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                    for (i32 remaining = batch->shape_count; remaining > 0; --remaining, ++source) {
                        if (source->material[1] != 0) {
                            continue;
                        }
                        TERRAIN_LOAD_VERTEX(vertices[0], source->vertices[0]);
                        TERRAIN_LOAD_VERTEX(vertices[1], source->vertices[1]);
                        TERRAIN_LOAD_VERTEX(vertices[2], source->vertices[2]);
                        NuVec4MtxTransformVU0x3(vertices, vertices, TERRAIN_GROUP_MATRIX(group));
                        if (source->normals[1].y < 65535.0) {
                            TERRAIN_LOAD_VERTEX(vertices[3], source->vertices[3]);
                            NuVec4MtxTransformVU0(&vertices[3], &vertices[3], TERRAIN_GROUP_MATRIX(group));
                        } else {
                            vertices[3] = vertices[2];
                        }
                        TERRAIN_POLYGON_INFO *surface = &ScaleTerrain[transformed_count];
                        surface->packed_metadata = source->packed_metadata;
                        TERRAIN_STORE_VERTEX(surface->vertices[0], vertices[0]);
                        TERRAIN_STORE_VERTEX(surface->vertices[1], vertices[1]);
                        TERRAIN_STORE_VERTEX(surface->vertices[2], vertices[2]);
                        if (source->normals[1].y < 65535.0) {
                            TERRAIN_STORE_VERTEX(surface->vertices[3], vertices[3]);
                            vertices[0].x = surface->vertices[1].x - surface->vertices[3].x;
                            vertices[0].y = surface->vertices[1].y - surface->vertices[3].y;
                            vertices[0].z = surface->vertices[1].z - surface->vertices[3].z;
                            vertices[1].x = surface->vertices[2].x - surface->vertices[3].x;
                            vertices[1].y = surface->vertices[2].y - surface->vertices[3].y;
                            vertices[1].z = surface->vertices[2].z - surface->vertices[3].z;
                            {
                                const NUVEC4 *edge_a = &vertices[0];
                                const NUVEC4 *edge_b = &vertices[1];
                                NUVEC cross;
                                cross.x = (edge_b->z * edge_a->y - edge_b->y * edge_a->z);
                                cross.y = edge_b->x * edge_a->z - edge_b->z * edge_a->x;
                                cross.z = edge_a->x * edge_b->y - edge_b->x * edge_a->y;
                                surface->normals[1] = cross;
                            }
                            f32 length =
                                NuFsqrt(surface->normals[1].x * surface->normals[1].x + surface->normals[1].y * surface->normals[1].y +
                                        surface->normals[1].z * surface->normals[1].z);
                            f32 inverse = 0.0f != length ? 1.0f / length : 0.0f;
                            surface = &ScaleTerrain[transformed_count];
                            surface->normals[1].x *= inverse;
                            surface->normals[1].y *= inverse;
                            surface->normals[1].z *= inverse;
                        } else {
                            surface->normals[1].y = 65536.0f;
                        }
                        vertices[0].x = surface->vertices[2].x - surface->vertices[0].x;
                        vertices[0].y = surface->vertices[2].y - surface->vertices[0].y;
                        vertices[0].z = surface->vertices[2].z - surface->vertices[0].z;
                        vertices[1].x = surface->vertices[1].x - surface->vertices[0].x;
                        vertices[1].y = surface->vertices[1].y - surface->vertices[0].y;
                        vertices[1].z = surface->vertices[1].z - surface->vertices[0].z;
                        {
                            const NUVEC4 *edge_a = &vertices[0];
                            const NUVEC4 *edge_b = &vertices[1];
                            NUVEC cross;
                            cross.x = (edge_b->z * edge_a->y - edge_b->y * edge_a->z);
                            cross.y = edge_b->x * edge_a->z - edge_b->z * edge_a->x;
                            cross.z = edge_a->x * edge_b->y - edge_b->x * edge_a->y;
                            surface->normals[0] = cross;
                        }
                        f32 length = NuFsqrt(surface->normals[0].x * surface->normals[0].x + surface->normals[0].y * surface->normals[0].y +
                                             surface->normals[0].z * surface->normals[0].z);
                        f32 inverse = 0.0f == length ? 0.0f : 1.0f / length;
                        surface = &ScaleTerrain[transformed_count];
                        surface->normals[0].x *= inverse;
                        surface->normals[0].y *= inverse;
                        surface->normals[0].z *= inverse;
                        *cursor++ = surface;
                        ++count;
                        ++transformed_count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                }
            } else {
                while (batch->marker >= 0) {
                    TERRAIN_POLYGON_INFO *source = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                    for (i32 remaining = batch->shape_count; remaining > 0; --remaining, ++source) {
                        if (source->material[1] != 0) {
                            continue;
                        }
                        *cursor++ = source;
                        ++count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                }
            }
        } else if (CurrentTerrain->platforms[platform_index].flags & TERRAIN_PLATFORM_FLAG_ROTATING) {
            while (batch->marker >= 0) {
                TERRAIN_POLYGON_INFO *source = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                for (i32 remaining = batch->shape_count; remaining > 0; --remaining, ++source) {
                    if (source->material[1] != 0) {
                        continue;
                    }
                    TERRAIN_LOAD_VERTEX(vertices[0], source->vertices[0]);
                    TERRAIN_LOAD_VERTEX(vertices[1], source->vertices[1]);
                    TERRAIN_LOAD_VERTEX(vertices[2], source->vertices[2]);
                    NuVec4MtxTransformVU0x3(vertices, vertices, TERRAIN_GROUP_MATRIX(group));
                    if (source->normals[1].y < 65535.0) {
                        TERRAIN_LOAD_VERTEX(vertices[3], source->vertices[3]);
                        NuVec4MtxTransformVU0(&vertices[3], &vertices[3], TERRAIN_GROUP_MATRIX(group));
                    } else {
                        vertices[3] = vertices[2];
                    }
                    TERRAIN_POLYGON_INFO *surface = &ScaleTerrain[transformed_count];
                    surface->packed_metadata = source->packed_metadata;
                    surface->vertices[0].x = vertices[0].x;
                    surface->vertices[0].z = vertices[0].z;
                    surface->vertices[0].y = (vertices[0].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                    surface->vertices[1].x = vertices[1].x;
                    surface->vertices[1].z = vertices[1].z;
                    surface->vertices[1].y = (vertices[1].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                    surface->vertices[2].x = vertices[2].x;
                    surface->vertices[2].z = vertices[2].z;
                    surface->vertices[2].y = (vertices[2].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                    if (source->normals[1].y < 65535.0) {
                        surface->vertices[3].x = vertices[3].x;
                        surface->vertices[3].z = vertices[3].z;
                        surface->vertices[3].y = (vertices[3].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                        vertices[0].x = surface->vertices[1].x - surface->vertices[3].x;
                        vertices[0].y = surface->vertices[1].y - surface->vertices[3].y;
                        vertices[0].z = surface->vertices[1].z - surface->vertices[3].z;
                        vertices[1].x = surface->vertices[2].x - surface->vertices[3].x;
                        vertices[1].y = surface->vertices[2].y - surface->vertices[3].y;
                        vertices[1].z = surface->vertices[2].z - surface->vertices[3].z;
                        {
                            const NUVEC4 *edge_a = &vertices[0];
                            const NUVEC4 *edge_b = &vertices[1];
                            NUVEC cross;
                            cross.x = (edge_a->y * edge_b->z - edge_a->z * edge_b->y);
                            cross.y = edge_b->x * edge_a->z - edge_b->z * edge_a->x;
                            cross.z = edge_a->x * edge_b->y - edge_b->x * edge_a->y;
                            surface->normals[1] = cross;
                        }
                        f32 length =
                            NuFsqrt((surface->normals[1].z * surface->normals[1].z) +
                                    (surface->normals[1].x * surface->normals[1].x + surface->normals[1].y * surface->normals[1].y));
                        f32 inverse = length != 0.0f ? 1.0f / length : 0.0f;
                        surface = &ScaleTerrain[transformed_count];
                        surface->normals[1].x *= inverse;
                        surface->normals[1].y *= inverse;
                        surface->normals[1].z *= inverse;
                    } else {
                        surface->normals[1].y = 65536.0f;
                    }
                    vertices[0].x = surface->vertices[2].x - surface->vertices[0].x;
                    vertices[0].y = surface->vertices[2].y - surface->vertices[0].y;
                    vertices[0].z = surface->vertices[2].z - surface->vertices[0].z;
                    vertices[1].x = surface->vertices[1].x - surface->vertices[0].x;
                    vertices[1].y = surface->vertices[1].y - surface->vertices[0].y;
                    vertices[1].z = surface->vertices[1].z - surface->vertices[0].z;
                    {
                        const NUVEC4 *edge_a = &vertices[0];
                        const NUVEC4 *edge_b = &vertices[1];
                        NUVEC cross;
                        cross.x = (edge_b->z * edge_a->y - edge_b->y * edge_a->z);
                        cross.y = edge_b->x * edge_a->z - edge_b->z * edge_a->x;
                        cross.z = edge_a->x * edge_b->y - edge_b->x * edge_a->y;
                        surface->normals[0] = cross;
                    }
                    f32 length = NuFsqrt(surface->normals[0].x * surface->normals[0].x + surface->normals[0].y * surface->normals[0].y +
                                         surface->normals[0].z * surface->normals[0].z);
                    f32 inverse = 0.0f == length ? 0.0f : 1.0f / length;
                    surface = &ScaleTerrain[transformed_count];
                    surface->normals[0].x = inverse * surface->normals[0].x;
                    surface->normals[0].y = inverse * surface->normals[0].y;
                    surface->normals[0].z = inverse * surface->normals[0].z;
                    *cursor++ = surface;
                    ++count;
                    ++transformed_count;
                }
                batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
            }
        } else {
            while (batch->marker >= 0) {
                TERRAIN_POLYGON_INFO *source = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                for (i32 remaining = batch->shape_count; remaining > 0; --remaining, ++source) {
                    if (source->material[1] != 0) {
                        continue;
                    }
                    TERRAIN_POLYGON_INFO *surface = &ScaleTerrain[transformed_count];
                    surface->packed_metadata = source->packed_metadata;
                    surface->vertices[0].x = source->vertices[0].x;
                    surface->vertices[0].z = source->vertices[0].z;
                    surface->vertices[0].y = (source->vertices[0].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                    surface->vertices[1].x = source->vertices[1].x;
                    surface->vertices[1].z = source->vertices[1].z;
                    surface->vertices[1].y = (source->vertices[1].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                    surface->vertices[2].x = source->vertices[2].x;
                    surface->vertices[2].z = source->vertices[2].z;
                    surface->vertices[2].y = (source->vertices[2].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                    if (source->normals[1].y < 65535.0) {
                        surface->vertices[3].x = source->vertices[3].x;
                        surface->vertices[3].z = source->vertices[3].z;
                        surface->vertices[3].y = (source->vertices[3].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                        f32 length = NuFsqrt(source->normals[1].y * source->normals[1].y * TerI->object_scale_sq +
                                             source->normals[1].x * source->normals[1].x + source->normals[1].z * source->normals[1].z);
                        f32 inverse = 0.0f != length ? 1.0f / length : 0.0f;
                        surface = &ScaleTerrain[transformed_count];
                        surface->normals[1].x = inverse * source->normals[1].x;
                        surface->normals[1].y = TerI->object_scale * source->normals[1].y * inverse;
                        surface->normals[1].z = inverse * source->normals[1].z;
                    } else {
                        surface->normals[1].y = 65536.0f;
                    }
                    f32 length = NuFsqrt(source->normals[0].y * source->normals[0].y * TerI->object_scale_sq +
                                         source->normals[0].x * source->normals[0].x + source->normals[0].z * source->normals[0].z);
                    f32 inverse = length != 0.0f ? 1.0f / length : 0.0f;
                    surface = &ScaleTerrain[transformed_count];
                    surface->normals[0].x = source->normals[0].x * inverse;
                    surface->normals[0].y = TerI->object_scale * source->normals[0].y * inverse;
                    surface->normals[0].z = inverse * source->normals[0].z;
                    *cursor++ = surface;
                    ++count;
                    ++transformed_count;
                }
                batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
            }
        }
        if (count != 0) {
            header[0] = count;
            header[1] = old_query->terrain_group_index;
            header = reinterpret_cast<i16 *>(cursor);
        }
        header[0] = 0;
        header[1] = 0;
        u8 flags[2];
        DerotateMovementVector();
        HitTerrain();
        TerrainImpactNorm();
        if (TerrainImpactPlatform(flags) != 0) {
            NuScratchRelease();
            NuScratchRelease();
            ScaleTerrain = ScaleTerrainT1;
            curSphereter = saved_sphere;
            TerI = old_query;
            return 1;
        }
        new_query->start_position = new_query->position;
        TerI->position.y = new_query->start_position.y + (f64)0.025f;
        TerI->movement.x = 0.0f;
        TerI->movement.y = -0.05f;
        TerI->movement.z = 0.0f;
        DerotateMovementVector();
        TerrOverRideScan = reinterpret_cast<TERRAIN_POLYGON_INFO **>(old_query->scan_list_storage);
        HitTerrain();
        const f32 start_y = new_query->start_position.y;
        TerI->position.y = start_y;
        if (TerI->hit_type != TERRAIN_HIT_TYPE_NONE && TerI->hit_type < TERRAIN_HIT_TYPE_SECOND_NORMAL) {
            new_query->start_position.y = TerI->hit_time * TerI->movement.y + start_y + (f64)0.001f + (f64)0.025f;
        } else {
            new_query->start_position.y = 2000000.0f;
        }
        TerI = old_query;
        flags[0] = 0;
        if (!(new_query->start_position.y >= 2000000.0)) {
            const f32 delta = new_query->start_position.y - new_query->position.y;
            if (!(delta < -0.008f)) {
                if (0.01f > delta) {
                    new_query->position.y = delta + new_query->position.y;
                    flags[0] = 1;
                } else if (new_query->movement.x * new_query->movement.x + new_query->movement.z * new_query->movement.z >
                           (delta - (f64)0.0025f) * (delta - (f64)0.0025f)) {
                    new_query->position.y = delta + new_query->position.y;
                    flags[0] = 1;
                }
            }
        } else {
            new_query->start_position.y = new_query->position.y;
        }
        if (!TerrainPlatformMoveCheck(&new_query->position, NULL, platform_index, 1, 1)) {
            flags[0] = 0;
        }
        TerI->position = new_query->position;
        if (flags[0] != 0 && CurrentTerrain->groups[castnum].chunk_type == 1) {
            TerI->hit_flags[1] = 1;
            PlatformConnect(reinterpret_cast<char *>(old_query->hit_flags), &old_query->movement, movement_delta,
                            CurrentTerrain->groups[castnum].scene_index);
        } else if (CurTrackInfo != NULL) {
            CurTrackInfo->flags &= ~TERRAIN_TRACK_FLAG_CONNECTED;
        }
        NuScratchRelease();
        NuScratchRelease();
        ScaleTerrain = ScaleTerrainT1;
        TerI = old_query;
        curSphereter = saved_sphere;
        return 0;
    }

    i32 TerrainPlatformMoveCheck(NUVEC *position, NUVEC *normal, i32 platform_index, i32 resolve_impact, i32 keep_embedded_disabled) {
        const i32 group_index = CurrentTerrain->platforms[platform_index].terrain_group_index;
        TerrainQuery_s *query = TerI;
        i16 *entry = static_cast<i16 *>(query->scan_list);
        while (entry[0] > 0) {
            if (entry[1] == group_index) {
                entry[0] = static_cast<i16>(-abs(entry[0]));
            }
            entry += abs(entry[0]) * 2 + 2;
        }
        if (position->y > query->position.y && position->y < query->position.y + (f64)0.01f) {
            query->position.y = position->y;
        }
        const f32 saved_time = query->hit_time;
        const NUVEC saved_movement = query->movement;
        const i16 saved_group = query->terrain_group_index;
        const f32 saved_unclamped_time = query->unclamped_hit_time;
        const u16 saved_hit_type = query->hit_type;
        TERRAIN_POLYGON_INFO *saved_surface = query->surface;
        const i16 saved_scan_group = query->scan_group_index;
        const NUVEC saved_movement_normal = query->movement_normal;
        const NUVEC saved_impact_normal = query->impact_normal;
        query->movement.x = position->x - query->position.x;
        query->movement.y = position->y - query->position.y;
        query->movement.z = position->z - query->position.z;
        DerotateMovementVector();
        HitTerrain();
        query = TerI;
        if (resolve_impact != 0 && query->hit_type != TERRAIN_HIT_TYPE_NONE) {
            TerrainImpactNorm();
            query = TerI;
            TerrainImpactPlatform(query->hit_flags);
            if (normal != NULL) {
                *normal = query->movement_normal;
            }
            *position = query->position;
        }
        query->hit_time = saved_time;
        query->unclamped_hit_time = saved_unclamped_time;
        query->movement = saved_movement;
        query->terrain_group_index = saved_group;
        query->surface = saved_surface;
        query->scan_group_index = saved_scan_group;
        query->movement_normal = saved_movement_normal;
        query->impact_normal = saved_impact_normal;
        entry = static_cast<i16 *>(query->scan_list);
        if (query->hit_type < TERRAIN_HIT_TYPE_SECOND_NORMAL || keep_embedded_disabled == 0) {
            while (entry[0] != 0) {
                if (entry[1] == group_index) {
                    entry[0] = static_cast<i16>(abs(entry[0]));
                }
                entry += abs(entry[0]) * 2 + 2;
            }
        }
        if (query->hit_type != TERRAIN_HIT_TYPE_NONE) {
            query->hit_type = static_cast<u8>(saved_hit_type);
            return 0;
        }
        query->hit_type = static_cast<u8>(saved_hit_type);
        return 1;
    }

    i32 TerrShapeSideStep(NUVEC *, NUVEC *, u8 *) {
        TerrainQuery_s *query = TerI;
        i32 push;
        switch (query->hit_type) {
            case TERRAIN_HIT_TYPE_NONE:
                return 1;
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
                if (query->impact_normal.y < 0.5f) {
                    push = 0;
                } else {
                    push = 1;
                }
                break;
            default:
                if (query->impact_normal.y >= 0.707f || query->impact_normal.y < -0.8f) {
                    return 1;
                }
                push = 0;
                break;
        }

        TERRAIN_AXIS_FREEDOM_SHAPE *shape = TerrShape;
        NUVEC axis;
        axis.x = NuTrigTable[static_cast<i32>(shape->angle + 16384.0) >> 1 & 0x7fff];
        axis.y = 0.0f;
        axis.z = -NuTrigTable[static_cast<i32>(shape->angle) >> 1 & 0x7fff];

        query->start_position.x = query->position.x;
        query->start_position.y = query->position.y;
        query->start_position.z = query->position.z;
        query->start_movement.x = query->movement.x;
        query->start_movement.y = query->movement.y;
        query->start_movement.z = query->movement.z;
        query->movement.y = 0.0f;

        if (push == 1) {
            query->movement.x = query->movement_normal.x * shape->radius * (f64)0.15f;
            query->movement.z = query->movement_normal.z * shape->radius * (f64)0.15f;
            shape->offset.x += query->movement_normal.x * shape->radius * (f64)0.05f;
            shape->offset.z += query->movement_normal.z * shape->radius * (f64)0.05f;
        } else {
            f32 axis_impact = query->impact_normal.x * axis.x + query->impact_normal.z * axis.z;
            if (axis_impact == 0.0f) {
                axis_impact = (shape->offset.x * axis.x + shape->offset.z * axis.z) / shape->radius;
                if (axis_impact == 0.0f) {
                    return 1;
                }
            }
            if (axis_impact < 0.0f) {
                axis_impact = -NuFsqrt(-axis_impact);
            } else {
                axis_impact = NuFsqrt(axis_impact);
            }
            shape = TerrShape;
            query = TerI;
            query->movement.x = axis_impact * axis.x * shape->radius + shape->offset.x;
            query->movement.z = axis_impact * axis.z * shape->radius + shape->offset.z;
        }

        u8 side_step_hit_flags[2];
        NUVEC impact_result;
        do {
            DerotateMovementVector();
            HitTerrain();
            TerrainImpactNorm();
            TerrainImpact(&impact_result, &impact_result, side_step_hit_flags);

            TerrShapeAdjCnt--;
        } while (TerrShapeAdjCnt > 0 && TerI->hit_type != TERRAIN_HIT_TYPE_NONE);
        query = TerI;

        if (query->hit_type == TERRAIN_HIT_TYPE_NONE) {
            query->position.x = query->movement.x + query->position.x;
            query->position.y = query->movement.y + query->position.y;
            query->position.z = query->movement.z + query->position.z;
        }

        TerrShape->offset.x += query->start_position.x - query->position.x;
        TerrShape->offset.z += query->start_position.z - query->position.z;
        query->hit_type = TERRAIN_HIT_TYPE_FACE;
        query->movement.x = query->start_movement.x;
        query->movement.y = query->start_movement.y;
        query->movement.z = query->start_movement.z;
        TerrainSideClamp(&axis, &query->position);
        return 0;
    }

    void TerrainSideClamp(NUVEC *axis, NUVEC *position) {
        f32 forward = axis->x * TerrShape->offset.x + TerrShape->offset.z * axis->z;
        f32 side = axis->z * TerrShape->offset.x - axis->x * TerrShape->offset.z;
        TERRAIN_AXIS_FREEDOM_SHAPE *shape = TerrShape;
        if (forward > shape->radius) {
            forward = shape->radius;
        }
        const f32 negative_radius = -shape->radius;
        if (forward < negative_radius) {
            forward = negative_radius;
        }
        if (side > shape->radius * (f64)0.2f) {
            side = shape->radius * (f64)0.2f;
        }
        if (side < negative_radius * (f64)0.2f) {
            side = negative_radius * (f64)0.2f;
        }

        position->x -= axis->x * forward + side * axis->z - shape->offset.x;
        position->z -= forward * axis->z - axis->x * side - shape->offset.z;

        shape->offset.x = axis->x * forward + side * axis->z;
        shape->offset.z = forward * axis->z - side * axis->x;
    }

    i16 InsidePolLines(f32 point_x, f32 point_y, f32 point_z, f32 edge_a_x, f32 edge_a_y, f32 edge_a_z, f32 edge_b_x, f32 edge_b_y,
                       f32 edge_b_z, NUVEC *normal) {
        f32 abs_temp;
        u32 abs_a;
        u32 abs_b;

        // Project onto the plane perpendicular to the dominant normal component.
        // The call order preserves the polygon winding in that projection.
        if (NU_FABS_GE_BITS(normal->y, normal->x, abs_temp, abs_a, abs_b) &&
            NU_FABS_GE_BITS(normal->y, normal->z, abs_temp, abs_a, abs_b)) {
            if (normal->y < 0.0f) {
                if (InsideLineF(point_x, point_z, 0.0f, 0.0f, edge_a_x, edge_a_z) != 0 &&
                    InsideLineF(point_x, point_z, edge_b_x, edge_b_z, 0.0f, 0.0f) != 0 &&
                    InsideLineF(point_x, point_z, edge_a_x, edge_a_z, edge_b_x, edge_b_z) != 0) {
                    return 1;
                }
                return 0;
            }

            if (InsideLineF(point_x, point_z, edge_a_x, edge_a_z, 0.0f, 0.0f) != 0 &&
                InsideLineF(point_x, point_z, 0.0f, 0.0f, edge_b_x, edge_b_z) != 0 &&
                InsideLineF(point_x, point_z, edge_b_x, edge_b_z, edge_a_x, edge_a_z) != 0) {
                return 1;
            }
            return 0;
        }

        if (NU_FABS_GE_BITS(normal->x, normal->y, abs_temp, abs_a, abs_b) &&
            NU_FABS_GE_BITS(normal->x, normal->z, abs_temp, abs_a, abs_b)) {
            if (normal->x < 0.0f) {
                if (InsideLineF(point_y, point_z, edge_a_y, edge_a_z, 0.0f, 0.0f) != 0 &&
                    InsideLineF(point_y, point_z, 0.0f, 0.0f, edge_b_y, edge_b_z) != 0 &&
                    InsideLineF(point_y, point_z, edge_b_y, edge_b_z, edge_a_y, edge_a_z) != 0) {
                    return 1;
                }
                return 0;
            }

            if (InsideLineF(point_y, point_z, 0.0f, 0.0f, edge_a_y, edge_a_z) != 0 &&
                InsideLineF(point_y, point_z, edge_b_y, edge_b_z, 0.0f, 0.0f) != 0 &&
                InsideLineF(point_y, point_z, edge_a_y, edge_a_z, edge_b_y, edge_b_z) != 0) {
                return 1;
            }
            return 0;
        }

        if (normal->z < 0.0f) {
            if (InsideLineF(point_y, point_x, 0.0f, 0.0f, edge_a_y, edge_a_x) != 0 &&
                InsideLineF(point_y, point_x, edge_b_y, edge_b_x, 0.0f, 0.0f) != 0 &&
                InsideLineF(point_y, point_x, edge_a_y, edge_a_x, edge_b_y, edge_b_x) != 0) {
                return 1;
            }
        } else if (InsideLineF(point_y, point_x, edge_a_y, edge_a_x, 0.0f, 0.0f) != 0 &&
                   InsideLineF(point_y, point_x, 0.0f, 0.0f, edge_b_y, edge_b_x) != 0 &&
                   InsideLineF(point_y, point_x, edge_b_y, edge_b_x, edge_a_y, edge_a_x) != 0) {
            return 1;
        }
        return 0;
    }

    i16 InsideLineF(f32 point_u, f32 point_v, f32 line_start_u, f32 line_start_v, f32 line_end_u, f32 line_end_v) {
        if ((point_u - line_start_u) * (line_end_v - line_start_v) + (point_v - line_start_v) * (line_start_u - line_end_u) < 0.0f) {
            return 0;
        }
        return 1;
    }

    TERRAIN_TRACK_SLOT *AllocTerrId() {
        TERRSET *terrain = CurrentTerrain;
        if (terrain == NULL) {
            return NULL;
        }

        for (i32 slot_index = 0; slot_index < 64; ++slot_index) {
            if (terrain->track_slots[slot_index].id == NULL) {
                return &terrain->track_slots[slot_index];
            }
        }

        return NULL;
    }
}

extern "C" {

    void AddCollisionSphere(NUVEC *position, f32 radius) {
        if (curSphereter < 16) {
            SphereData[curSphereter].position = *position;
            SphereData[curSphereter].radius = radius;
            ++curSphereter;
        }
    }
}

extern "C" i32 TerrainFreeId(void *id) {
    TERRAIN_SET *terrain = CurrentTerrain;
    i32 i = 0;
    if (terrain == NULL) {
        return 0;
    }
    for (; i < 64; ++i) {
        if (terrain->track_slots[i].id == id) {
            terrain->track_slots[i].id = NULL;
            return 1;
        }
    }
    return 0;
}

extern "C" void TerrainTrackFlush(void) {
    i32 slot_index;
    if (CurrentTerrain == NULL) {
        return;
    }
    for (slot_index = 0; slot_index < 64; ++slot_index) {
        CurrentTerrain->track_slots[slot_index].id = NULL;
    }
}

extern "C" i32 TerrainIntensityInfo(void) {
    if (TerrPoly != NULL) {
        return TerrPoly->normal_flags - 8;
    }
    if (TerrWallInfo != 0) {
        return TerrWallTab[3] - 8;
    }
    return -1;
}

extern "C" i32 TerrainInfo(void) {
    if (TerrPoly != NULL) {
        return TerrPoly->material[0];
    }
    if (TerrWallInfo != 0) {
        return TerrWallTab[0];
    }
    return -1;
}

extern "C" i32 TerrainInfoExtra(void) {
    if (TerrPoly != NULL) {
        return TerrPoly->material[1];
    }
    if (TerrWallInfo != 0) {
        return TerrWallTab[1];
    }
    return -1;
}

extern "C" i32 AddMSituExtraTerrRot(i32 source_index, NUMTX *matrix, NUMTX *previous, i32 rotating, TERRAIN_SET *source) {
    TERRAIN_SET *terrain;
    TERRAIN_GROUP *group;
    i32 index;
    i32 i;

    if (source == NULL) {
        return -1;
    }
    if (CurrentTerrain == NULL) {
        CurrentTerrain = source;
        for (i = 0; i < CurrentTerrain->removed_platform_count; ++i) {
            DeletePlatinst(CurrentTerrain->removed_platforms[i]);
        }
        CurrentTerrain->removed_platform_count = 0;
        curSphereter = 0;
    }
    terrain = CurrentTerrain;
    if (terrain->group_index_count >= terrain->max_group_indices) {
        return -1;
    }
    if (terrain->group_count >= terrain->max_groups) {
        return -1;
    }
    if (matrix == NULL || (previous == NULL && rotating) || source->removed_platform_count >= 32 || source_index >= source->group_count) {
        return -1;
    }
    index = -1;
    for (i = 0; i < terrain->max_platforms; ++i) {
        if (terrain->platforms[i].scene_object == NULL) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        return -1;
    }

    group = &terrain->groups[terrain->group_count];
    *group = source->groups[source_index];
    group->scene_index = index;
    group->chunk_type = 1;
    CurrentTerrain->platforms[index].scene_object = matrix;
    if (rotating) {
        CurrentTerrain->platforms[index].previous_matrix = *previous;
        CurrentTerrain->platforms[index].flags |= TERRAIN_PLATFORM_FLAG_ROTATING;
    } else {
        if (previous != NULL) {
            CurrentTerrain->platforms[index].previous_matrix.m30 = previous->m30;
            CurrentTerrain->platforms[index].previous_matrix.m31 = previous->m31;
            CurrentTerrain->platforms[index].previous_matrix.m32 = previous->m32;
        } else {
            CurrentTerrain->platforms[index].previous_matrix.m30 = matrix->m30;
            CurrentTerrain->platforms[index].previous_matrix.m31 = matrix->m31;
            CurrentTerrain->platforms[index].previous_matrix.m32 = matrix->m32;
        }
        CurrentTerrain->platforms[index].flags &= ~TERRAIN_PLATFORM_FLAG_ROTATING;
    }
    group->origin.x = matrix->m30;
    group->origin.y = matrix->m31;
    group->origin.z = matrix->m32;
    CurrentTerrain->platforms[index].terrain_group_index = CurrentTerrain->group_count;
    CurrentTerrain->platforms[index].scene_object_index = 0;
    CurrentTerrain->platforms[index].instance_flags = NULL;
    CurrentTerrain->group_indices[CurrentTerrain->group_index_count] = CurrentTerrain->group_count;
    CurrentTerrain->group_index_count++;
    CurrentTerrain->platforms[index].bounce_54 = 0.0f;
    CurrentTerrain->platforms[index].bounce_58 = 0.0f;
    CurrentTerrain->platforms[index].bounce_5c = 0.0f;
    CurrentTerrain->platforms[index].bounce_64 = 0.0f;
    CurrentTerrain->platforms[index].bounce_60 = 0.0f;
    CurrentTerrain->platforms[index].flags &= ~TERRAIN_PLATFORM_FLAG_COLLIDED;
    if (CurrentTerrain->active_platform_count < 96) {
        CurrentTerrain->active_platform_groups[CurrentTerrain->active_platform_count] = CurrentTerrain->group_count;
        CurrentTerrain->active_platform_count++;
    }
    CurrentTerrain->group_count++;
    CurrentTerrain->platform_cell.group_count++;
    CurrentTerrain->removed_platforms[CurrentTerrain->removed_platform_count] = index;
    CurrentTerrain->removed_platform_count++;
    return index;
}

extern "C" {
    NUVEC TerrImpactPos = {0};
    i32 curSphereter = 0;
    TERRAIN_LAST_IMPACT TerrLastImpact = {0};
    NUVEC TerrImpactNormal = {0};
}

extern "C" void NewTerrainScaleYMask(NUVEC *position, NUVEC *movement, u8 *hit_flags, i32 object_index, f32 radius, f32 collision_radius,
                                     f32 object_scale, i32 embedded_retry, i32 scan_flags, i32 terrain_mask) {
    TerrPolyInfo.material[0] = 0;
    TerrPolyInfo.material[1] = 0;
    TerrPolyInfo.flags = 0;
    TerrPolyInfo.normal_flags = 0;
    TerrImpact = 0;
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = 0;
    TerrWallInfo = 0;
    PlatCrush = 0;
    terrhitflags = 0;
    castnum = -1;

    if (CurrentTerrain == NULL) {
        TerImpactData = 0;
        return;
    }

    TerrLastImpact.hit_type = 0.0f;
    TERRAIN_TRACK_SLOT *track;
    for (i32 slot_index = 0; slot_index < 64; ++slot_index) {
        if (CurrentTerrain->track_slots[slot_index].id == hit_flags) {
            track = &CurrentTerrain->track_slots[slot_index];
            goto found_track;
        }
    }
    track = NULL;
found_track:
    CurTrackInfo = track;
    TerrOverRideScan = 0;

    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = object_scale;
    TerI = query;
    query->object_scale_sq = object_scale * object_scale;
    f32 inverse = object_scale != 0.0f ? 1.0f / object_scale : 0.0f;
    query->inverse_object_scale = inverse;
    query->inverse_object_scale_sq = inverse * inverse;
    query->collision_radius = collision_radius;
    inverse = collision_radius != 0.0f ? 1.0f / collision_radius : 0.0f;
    query->inverse_collision_radius = inverse;
    query->collision_radius_sq = collision_radius * collision_radius;
    castnum = -1;

    TERRAIN_FLOAT_BITS value;
    value.value = position->x;
    query->position.x = value.value;
    query->start_position.x = value.value;
    value.value = collision_radius * object_scale + position->y;
    query->position.y = value.value;
    query->start_position.y = value.value;
    value.value = position->z;
    query->position.z = value.value;
    query->start_position.z = value.value;
    value.value = movement->x;
    query->movement.x = value.value;
    query->start_movement.x = value.value;
    value.value = movement->y;
    query->movement.y = value.value;
    query->start_movement.y = value.value;
    value.value = movement->z;
    query->movement.z = value.value;
    query->start_movement.z = value.value;
    query->flags &= ~1u;
    query->radius = radius;
    query->separation_epsilon = 0.005f;
    query->object_index = static_cast<i16>(object_index);
    query->hit_flags = hit_flags;
    query->compare_epsilon = 0.000005f;
    query->scan_result = 0;
    ScanTerrain(1, terrain_mask, scan_flags != 0 ? 0x40 : 0);

    if (hit_flags[1] != 0) {
        TERRAIN_FLOAT_BITS absolute_x;
        absolute_x.value = movement->x;
        absolute_x.bits &= 0x7fffffff;
        if (absolute_x.value < radius) {
            TERRAIN_FLOAT_BITS absolute_y;
            absolute_y.value = movement->y;
            absolute_y.bits &= 0x7fffffff;
            if (absolute_y.value < radius) {
                TERRAIN_FLOAT_BITS absolute_z;
                absolute_z.value = movement->z;
                absolute_z.bits &= 0x7fffffff;
                if (absolute_z.value < radius && platinrange == 0) {
                    NuScratchRelease();
                    terrhitflags |= 2;
                    ShadNorm.x = 0.0f;
                    ShadNorm.y = 1.0f;
                    ShadNorm.z = 0.0f;
                    TerrShapeAdjCnt = 0;
                    TerImpactData = 0;
                    return;
                }
            }
        }
    }

    query = TerI;
    query->position.y = query->inverse_object_scale * query->position.y;
    query->movement.y = query->inverse_object_scale * query->movement.y;
    hit_flags[0] = 0;
    hit_flags[1] = 0;

    i32 scan_count = PlatformChecks(6, movement);
    do {
        DerotateMovementVector();
        HitTerrain();
        TerrainImpactNorm();
        StorePlatImpact();

        query = TerI;
        if (query->hit_type > TERRAIN_HIT_TYPE_SECOND_NORMAL && query->terrain_group_index != -1 &&
            CurrentTerrain->groups[query->terrain_group_index].chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
            --scan_count;
            NewTerrStoreAnyInfo();
            NUVEC before = query->position;
            i32 embedded = TerrainPlatformEmbedded(movement);
            query = TerI;
            if (embedded == 0 && (before.x != query->position.x || before.y != query->position.y || before.z != query->position.z)) {
                goto resolved;
            }
        }

        --scan_count;
        if (query->hit_type != TERRAIN_HIT_TYPE_NONE && query->terrain_group_index >= 0) {
            ShadNorm = query->impact_normal;

            TERRAIN_POLYGON_INFO *surface = query->surface;
            f32 wall_limit;
            if (surface != NULL) {
                wall_limit = surface->normals[0].y * query->movement_normal.y + surface->normals[0].x * query->movement_normal.x +
                             surface->normals[0].z * query->movement_normal.z;
            } else {
                wall_limit = 0.707f;
            }

            TERRAIN_GROUP *group = &CurrentTerrain->groups[query->terrain_group_index];
            if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
                wall_limit =
                    (query->hit_type & TERRAIN_HIT_TYPE_CLASS_MASK) > TERRAIN_HIT_TYPE_FACE && (f64)0.95f > wall_limit ? 0.98f : 0.707f;
            } else {
                wall_limit = query->shape_adjusted != 0 ? 1.1f : -1.1f;
            }

            if (wallover != 0.0f) {
                wall_limit = wallover;
            }
            if ((query->hit_type & TERRAIN_HIT_TYPE_CLASS_MASK) != TERRAIN_HIT_TYPE_FACE) {
                wall_limit = 0.707f;
            }
            if (surface != NULL && query->terrain_group_index >= 0 &&
                (surface->normal_flags & TERRAIN_SURFACE_CLASS_MASK) == TERRAIN_SURFACE_CLASS_WALL_OVERRIDE) {
                query->position.x += query->movement_normal.x * (f64)0.001f;
                query->position.z += query->movement_normal.z * (f64)0.001f;
                wall_limit = 1.1f;
            }

            if (query->impact_normal.y >= wall_limit && group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
                CurrentTerrain->platforms[group->scene_index].flags |= TERRAIN_PLATFORM_FLAG_COLLIDED;
            }
        }

        if (TerrShapeAdjCnt != 0) {
            if (TerrShapeSideStep(position, movement, hit_flags) != 0) {
                TerrainImpact(position, movement, hit_flags);
            }
            query = TerI;
        } else {
            TerrainImpact(position, movement, hit_flags);
        }

        TerrLastImpact.position.x = query->position.x - query->movement_normal.x * query->collision_radius;
        TerrLastImpact.position.y = (query->position.y - query->movement_normal.y * query->collision_radius) * query->object_scale;
        TerrLastImpact.position.z = query->position.z - query->movement_normal.z * query->collision_radius;
        TerrLastImpact.hit_type = static_cast<f32>(static_cast<i32>(query->hit_type));

    resolved:
        if (query->hit_type != TERRAIN_HIT_TYPE_NONE) {
            TerrImpact = query->surface == NULL ? 2 : 1;
            TerrImpactPos.x = query->position.x - query->movement_normal.x * query->collision_radius;
            TerrImpactPos.y = (query->position.y - query->movement_normal.y * query->collision_radius) * query->object_scale;
            TerrImpactPos.z = query->position.z - query->movement_normal.z * query->collision_radius;
            TerrImpactNormal = query->impact_normal;
        }
    } while (query->hit_type != TERRAIN_HIT_TYPE_NONE && scan_count > 0 &&
             query->movement_normal.y * query->movement_normal.y + query->movement_normal.x * query->movement_normal.x +
                     query->movement_normal.z * query->movement_normal.z <=
                 1.5);

    if (query->hit_type != TERRAIN_HIT_TYPE_NONE) {
        position->x = query->position.x;
        position->y = query->position.y * query->object_scale - query->collision_radius * query->object_scale;
        position->z = query->position.z;
    } else if (scan_count > 3 && hit_flags[0] == 0 && hit_flags[1] == 0 && embedded_retry != 0) {
        query->position.x = position->x;
        query->position.y = query->inverse_object_scale * position->y + query->collision_radius + (f64)0.003f;
        query->position.z = position->z;
        query->movement.x = 0.0f;
        query->movement.y = -0.007f;
        query->movement.z = 0.0f;
        DerotateMovementVector();
        HitTerrain();
        query = TerI;
        if (query->hit_type != TERRAIN_HIT_TYPE_NONE &&
            (query->terrain_group_index < 0 ||
             (query->surface->normal_flags & TERRAIN_SURFACE_CLASS_MASK) != TERRAIN_SURFACE_CLASS_WALL_OVERRIDE)) {
            TerrainImpactNorm();
            query = TerI;
            ShadNorm = query->impact_normal;
            query->start_movement.x = movement->x;
            query->start_movement.y = movement->y;
            query->start_movement.z = movement->z;
            TerrainImpact(position, movement, hit_flags);
            position->x = query->position.x;
            position->y = query->position.y * query->object_scale - query->collision_radius * query->object_scale;
            position->z = query->position.z;
            movement->x = query->start_movement.x;
            movement->y = query->start_movement.y;
            movement->z = query->start_movement.z;
        }
    }

    NuScratchRelease();
    TerrShapeAdjCnt = 0;
    TerImpactData = 0;
}

extern "C" void NewTerrainScaleY(NUVEC *position, NUVEC *movement, u8 *hit_flags, i32 object_index, f32 radius, f32 collision_radius,
                                 f32 object_scale, i32 embedded_retry, i32 scan_flags) {
    NewTerrainScaleYMask(position, movement, hit_flags, object_index, radius, collision_radius, object_scale, embedded_retry, scan_flags,
                         0);
}

extern "C" i32 NewRayCastScaleY(NUVEC *position, NUVEC *movement, f32 radius, f32 scale_y, i32 scan_flags) {
    return NewRayCastScaleYMask(position, movement, radius, scale_y, scan_flags, 0);
}

extern "C" void FullReflect(NUVEC *normal, NUVEC *movement, NUVEC *result) {
    const f32 reverse_x = -movement->x;
    const f32 reverse_y = -movement->y;
    const f32 reverse_z = -movement->z;
    const f32 reflection = reverse_y * normal->y + reverse_x * normal->x + reverse_z * normal->z;
    result->x = normal->x * reflection * 2.0 + movement->x;
    result->y = normal->y * reflection * 2.0 + movement->y;
    result->z = normal->z * reflection * 2.0 + movement->z;
}

extern "C" void fun_006786e0(f32 value) {
    wallover = value;
}
