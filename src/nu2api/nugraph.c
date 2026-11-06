#include "saga.h"

f32 nugraphGetYatX(nugraph_s *graph, f32 x, i32 iterations) {
    f32 current_x = 0.0f;
    f32 upper_parameter = 1.0f;
    f32 lower_parameter = 0.0f;
    f32 lower_x = 0.0f;
    f32 upper_x = 1.0f;
    f32 bound_y;
    f32 current_y;
    f32 parameter = 0.0f;
    f32 parameter_squared = 0.0f;
    f32 step = 0.5f;
    i32 last_index = graph->point_count - 1;
    i32 segment;
    i32 i;
    NUVEC p0;
    NUVEC p1;
    NUVEC p2;
    NUVEC p3;
    x = x / (graph->x_extent * graph->x_scale);
    x -= graph->x_offset;
    if (x > 1.0f)
        x = 1.0f;
    if (x < 0.0f)
        x = 0.0f;
    if (iterations <= 0)
        iterations = 3;
    for (segment = 1; segment < graph->point_count; ++segment) {
        if (x <= graph->x[segment]) {
            switch (graph->interpolation) {
                case 0:
                    p1.x = graph->x[segment - 1];
                    p1.y = graph->y[segment - 1];
                    p2.x = graph->x[segment];
                    p2.y = graph->y[segment];
                    current_x = (x - p1.x) / (p2.x - p1.x);
                    current_y = p1.y + (p2.y - p1.y) * current_x;
                    return current_y;
                case 1:
                    if (segment > 1) {
                        p0.x = graph->x[segment - 2];
                        p0.y = graph->y[segment - 2];
                    } else {
                        p0.x = graph->x[0] + (graph->x[0] - graph->x[1]) * (f64)0.1f;
                        p0.y = graph->y[0] + (graph->y[0] - graph->y[1]) * (f64)0.1f;
                    }
                    p1.x = graph->x[segment - 1];
                    p1.y = graph->y[segment - 1];
                    p2.x = graph->x[segment];
                    p2.y = graph->y[segment];
                    if (segment <= graph->point_count - 2) {
                        p3.x = graph->x[segment + 1];
                        p3.y = graph->y[segment + 1];
                    } else {
                        p3.x = graph->x[last_index] + (graph->x[last_index] - graph->x[last_index - 1]) * (f64)0.1f;
                        p3.y = graph->y[last_index] + (graph->y[last_index] - graph->y[last_index - 1]) * (f64)0.1f;
                    }
                    lower_x = p1.x;
                    upper_x = p2.x;
                    current_x = p1.x;
                    for (i = 0; i < iterations; ++i) {
                        if (x > current_x) {
                            lower_parameter = parameter;
                            lower_x = current_x;
                            parameter += step;
                        } else {
                            upper_parameter = parameter;
                            upper_x = current_x;
                            parameter -= step;
                        }
                        step *= 0.5f;
                        parameter_squared = parameter * parameter;
                        current_x = 0.5 * (2.0 * p1.x + (p2.x - p0.x) * parameter +
                                           (2.0 * p0.x - 5.0 * p1.x + 4.0 * p2.x - p3.x) * parameter_squared +
                                           (3.0 * p1.x - p0.x - 3.0 * p2.x + p3.x) * (parameter * parameter_squared));
                    }
                    current_y = 0.5 * (2.0 * p1.y + (p2.y - p0.y) * parameter +
                                       (2.0 * p0.y - 5.0 * p1.y + 4.0 * p2.y - p3.y) * parameter_squared +
                                       (3.0 * p1.y - p0.y - 3.0 * p2.y + p3.y) * (parameter * parameter_squared));
                    if (x > current_x) {
                        bound_y =
                            0.5 * (2.0 * p1.y + (p2.y - p0.y) * upper_parameter +
                                   (2.0 * p0.y - 5.0 * p1.y + 4.0 * p2.y - p3.y) * (upper_parameter * upper_parameter) +
                                   (3.0 * p1.y - p0.y - 3.0 * p2.y + p3.y) *
                                       (upper_parameter * upper_parameter * upper_parameter));
                        current_y = current_y + (bound_y - current_y) * (x - current_x) / (upper_x - current_x);
                    } else {
                        bound_y =
                            0.5 * (2.0 * p1.y + (p2.y - p0.y) * lower_parameter +
                                   (2.0 * p0.y - 5.0 * p1.y + 4.0 * p2.y - p3.y) * (lower_parameter * lower_parameter) +
                                   (3.0 * p1.y - p0.y - 3.0 * p2.y + p3.y) *
                                       (lower_parameter * lower_parameter * lower_parameter));
                        current_y = current_y + (bound_y - current_y) * (x - current_x) / (lower_x - current_x);
                    }
                    return current_y;
                case 2:
                    return 0.0f;
            }
        }
    }
    return 0.0f;
}
