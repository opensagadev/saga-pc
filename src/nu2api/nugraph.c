#include "saga.h"

static i32 curve_points;
static NUVEC *curve;
static NUVEC *control;
static const char NugraphSourcePathPc[] = "d:\\projects\\game_saga\\nu2api.saga\\numath\\nugraph.c";

void nugraphInit(nugraph_s *graph) {
    graph->interpolation = 1;
    graph->point_count = 2;
    graph->x[0] = 0.0f;
    graph->y[0] = 0.0f;
    graph->x[1] = 1.0f;
    graph->y[1] = 1.0f;
    graph->x_scale = 1.0f;
    graph->y_scale = 1.0f;
    graph->x_extent = 10.0f;
    graph->y_extent = 10.0f;
}

i32 nugraphAddPoint(nugraph_s *graph, f32 x, f32 y) {
    i32 index;
    i32 insert_index = 0;
    if (graph == NULL) {
        return 0;
    }
    if (graph->point_count >= 8) {
        return 0;
    }
    if (graph->point_count < 2) {
        return 0;
    }
    for (index = 0; index < graph->point_count; ++index) {
        if (graph->x[index] == x) {
            return 0;
        }
        if (graph->x[index] > x) {
            insert_index = index;
            break;
        }
    }
    for (index = graph->point_count; index >= insert_index; --index) {
        graph->x[index] = graph->x[index - 1];
        graph->y[index] = graph->y[index - 1];
    }
    ++graph->point_count;
    graph->x[insert_index] = x;
    graph->y[insert_index] = y;
    return 1;
}

i32 nugraphDeletePoint(nugraph_s *graph, i32 index) {
    i32 i;
    if (graph == NULL) {
        return 0;
    }
    if (index >= graph->point_count - 1) {
        return 0;
    }
    if (index <= 0) {
        return 0;
    }
    --graph->point_count;
    for (i = index; i < graph->point_count; ++i) {
        graph->x[i] = graph->x[i + 1];
        graph->y[i] = graph->y[i + 1];
    }
    return 1;
}

f32 nugraphGetXatIndex(nugraph_s *graph, i32 index) {
    if (graph == NULL) {
        return 0.0f;
    }
    if (index >= 0 && index < graph->point_count) {
        return graph->x[index];
    }
    if (index == -1) {
        return graph->x[0] + (graph->x[0] - graph->x[1]);
    }
    if (index == graph->point_count) {
        return graph->x[graph->point_count] + (graph->x[graph->point_count] - graph->x[graph->point_count - 1]);
    }
    return 0.0f;
}

f32 nugraphGetYatIndex(nugraph_s *graph, i32 index) {
    if (graph == NULL) {
        return 0.0f;
    }
    if (index >= 0 && index < graph->point_count) {
        return graph->y[index];
    }
    if (index == -1) {
        return graph->y[0] + (graph->y[0] - graph->y[1]);
    }
    if (index == graph->point_count) {
        return graph->y[graph->point_count] + (graph->y[graph->point_count] - graph->y[graph->point_count - 1]);
    }
    return 0.0f;
}

f32 nugraph_blend(i32 interval, i32 degree, i32 *knots, f32 parameter) {
    f32 result;
    if (degree == 1) {
        if ((f32)knots[interval] <= parameter && parameter < (f32)knots[interval + 1]) {
            result = 1.0f;
        } else {
            result = 0.0f;
        }
    } else {
        if (knots[interval + degree - 1] == knots[interval] && knots[interval + degree] == knots[interval + 1]) {
            result = 0.0f;
        } else if (knots[interval + degree - 1] == knots[interval]) {
            result = ((f32)knots[interval + degree] - parameter) /
                     (f32)(knots[interval + degree] - knots[interval + 1]) *
                     nugraph_blend(interval + 1, degree - 1, knots, parameter);
        } else if (knots[interval + degree] == knots[interval + 1]) {
            result = (parameter - (f32)knots[interval]) / (f32)(knots[interval + degree - 1] - knots[interval]) *
                     nugraph_blend(interval, degree - 1, knots, parameter);
        } else {
            result = (parameter - (f32)knots[interval]) / (f32)(knots[interval + degree - 1] - knots[interval]) *
                         nugraph_blend(interval, degree - 1, knots, parameter) +
                     ((f32)knots[interval + degree] - parameter) /
                         (f32)(knots[interval + degree] - knots[interval + 1]) *
                         nugraph_blend(interval + 1, degree - 1, knots, parameter);
        }
    }
    return result;
}

void nugraph_compute_point(i32 *knots, i32 segment_count, i32 degree, f32 parameter, NUVEC *control_points,
                           NUVEC *point) {
    i32 i;
    f32 weight;
    point->x = 0.0f;
    point->y = 0.0f;
    point->z = 0.0f;
    for (i = 0; i <= segment_count; ++i) {
        weight = nugraph_blend(i, degree, knots, parameter);
        point->x += control_points[i].x * weight;
        point->y += control_points[i].y * weight;
    }
}

void nugraph_compute_catmull_point(i32 segment_count, f32 parameter, NUVEC *control_points, NUVEC *point) {
    NUVEC first;
    NUVEC last;
    i32 segment;
    NUVEC *p0;
    NUVEC *p3;
    f32 start;
    f32 fraction;
    NUVEC *p1;
    NUVEC *p2;
    if (segment_count <= 0) {
        return;
    }
    first.x = control_points[0].x + (control_points[0].x - control_points[1].x) * 0.1f;
    first.y = control_points[0].y + (control_points[0].y - control_points[1].y) * 0.1f;
    last.x = control_points[segment_count].x +
             (control_points[segment_count].x - control_points[segment_count - 1].x) * 0.1f;
    last.y = control_points[segment_count].y +
             (control_points[segment_count].y - control_points[segment_count - 1].y) * 0.1f;
    for (segment = 1; segment <= segment_count; ++segment) {
        if ((f32)segment / (f32)segment_count > parameter) {
            break;
        }
    }
    start = (f32)(segment - 1) / (f32)segment_count;
    fraction = parameter - start;
    fraction *= (f32)segment_count;
    if (segment == 1)
        p0 = &first;
    else
        p0 = &control_points[segment - 2];
    p1 = &control_points[segment - 1];
    p2 = &control_points[segment];
    if (segment >= segment_count)
        p3 = &last;
    else
        p3 = &control_points[segment + 1];
    point->x = 0.5f * (2.0f * p1->x + (p2->x - p0->x) * fraction +
                       (2.0f * p0->x - 5.0f * p1->x + 4.0f * p2->x - p3->x) * (fraction * fraction) +
                       (3.0f * p1->x - p0->x - 3.0f * p2->x + p3->x) * (fraction * fraction * fraction));
    point->y = 0.5f * (2.0f * p1->y + (p2->y - p0->y) * fraction +
                       (2.0f * p0->y - 5.0f * p1->y + 4.0f * p2->y - p3->y) * (fraction * fraction) +
                       (3.0f * p1->y - p0->y - 3.0f * p2->y + p3->y) * (fraction * fraction * fraction));
    point->z = 0.0f;
}

void nugraph_compute_linear_point(i32 segment_count, f32 parameter, NUVEC *control_points, NUVEC *point) {
    i32 segment;
    f32 start;
    f32 fraction;
    for (segment = 1; segment <= segment_count; ++segment) {
        if ((f32)segment / (f32)segment_count > parameter) {
            break;
        }
    }
    start = (f32)(segment - 1) / (f32)segment_count;
    fraction = parameter - start;
    fraction *= (f32)segment_count;
    point->x = control_points[segment - 1].x + (control_points[segment].x - control_points[segment - 1].x) * fraction;
    point->y = control_points[segment - 1].y + (control_points[segment].y - control_points[segment - 1].y) * fraction;
    point->z = 0.0f;
}

void nugraph_compute_intervals(i32 *knots, i32 segment_count, i32 degree) {
    i32 i;
    for (i = 0; i <= segment_count + degree; ++i) {
        if (i < degree) {
            knots[i] = 0;
        } else if (degree <= i && i <= segment_count) {
            knots[i] = i - degree + 1;
        } else if (i > segment_count) {
            knots[i] = segment_count - degree + 2;
        }
    }
}

void nugraph_bspline(i32 segment_count, i32 degree, NUVEC *control_points, NUVEC *output, i32 output_count) {
    i32 knots[18];
    f32 increment;
    f32 parameter;
    i32 i;
    NUVEC point;
    nugraph_compute_intervals(knots, segment_count, degree);
    increment = (f32)(segment_count - degree + 2) / (f32)(output_count - 1);
    parameter = 0.0f;
    for (i = 0; i < output_count - 1; ++i) {
        nugraph_compute_point(knots, segment_count, degree, parameter, control_points, &point);
        output[i].x = point.x;
        output[i].y = point.y;
        output[i].z = point.z;
        parameter += increment;
    }
    output[output_count - 1].x = control_points[segment_count].x;
    output[output_count - 1].y = control_points[segment_count].y;
    output[output_count - 1].z = control_points[segment_count].z;
}

void nugraph_catmullrom(i32 segment_count, NUVEC *control_points, NUVEC *output, i32 output_count) {
    f32 increment = 1.0f / (f32)(output_count - 1);
    f32 parameter = 0.0f;
    i32 i;
    NUVEC point;
    for (i = 0; i < output_count - 1; ++i) {
        nugraph_compute_catmull_point(segment_count, parameter, control_points, &point);
        output[i].x = point.x;
        output[i].y = point.y;
        output[i].z = point.z;
        parameter += increment;
    }
    output[output_count - 1].x = control_points[segment_count].x;
    output[output_count - 1].y = control_points[segment_count].y;
    output[output_count - 1].z = control_points[segment_count].z;
}

void nugraph_linear(i32 segment_count, NUVEC *control_points, NUVEC *output, i32 output_count) {
    f32 increment = 1.0f / (f32)(output_count - 1);
    f32 parameter = 0.0f;
    i32 i;
    NUVEC point;
    for (i = 0; i < output_count - 1; ++i) {
        nugraph_compute_linear_point(segment_count, parameter, control_points, &point);
        output[i].x = point.x;
        output[i].y = point.y;
        output[i].z = point.z;
        parameter += increment;
    }
    output[output_count - 1].x = control_points[segment_count].x;
    output[output_count - 1].y = control_points[segment_count].y;
    output[output_count - 1].z = control_points[segment_count].z;
}

void nugraphFreeTempCurveData(void) {
    if (curve != NULL) {
        NuFree(curve, NugraphSourcePathPc, 506);
    }
    curve = NULL;
    if (control != NULL) {
        NuFree(control, NugraphSourcePathPc, 508);
    }
    control = NULL;
}

i32 nugraphCalcCurve(nugraph_s *graph, i32 point_count) {
    i32 i;
    if (graph == NULL) {
        return 0;
    }
    nugraphFreeTempCurveData();
    if (curve == NULL) {
        curve = (NUVEC *)NuAlloc(100 * sizeof(NUVEC), NugraphSourcePathPc, 521);
    }
    if (control == NULL) {
        control = (NUVEC *)NuAlloc(8 * sizeof(NUVEC), NugraphSourcePathPc, 526);
    }
    if (curve == NULL || control == NULL) {
        return 0;
    }
    if (point_count > 100) {
        point_count = 100;
    }
    curve_points = point_count;
    for (i = 0; i < graph->point_count; ++i) {
        control[i].x = graph->x[i];
        control[i].y = graph->y[i];
        control[i].z = 0.0f;
    }
    switch (graph->interpolation) {
        case 0:
            nugraph_linear(graph->point_count - 1, control, curve, curve_points);
            break;
        case 1:
            nugraph_catmullrom(graph->point_count - 1, control, curve, curve_points);
            break;
        case 2:
            nugraph_bspline(graph->point_count - 1, 3, control, curve, curve_points);
            break;
    }
    return 1;
}

f32 nugraphGetXatT(nugraph_s *graph, f32 t) {
    i32 index;
    t *= (f32)curve_points;
    index = (i32)(t + 0.5f);
    if (index >= curve_points) {
        index = curve_points - 1;
    }
    if (curve == NULL) {
        return 0.0f;
    }
    return curve[index].x;
}

f32 nugraphGetYatT(nugraph_s *graph, f32 t) {
    i32 index;
    t *= (f32)curve_points;
    index = (i32)(t + 0.5f);
    if (index >= curve_points) {
        index = curve_points - 1;
    }
    if (curve == NULL) {
        return 0.0f;
    }
    return curve[index].y;
}

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
