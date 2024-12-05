// abstract-trader https://github.com/ougi-washi/abstract-trader

#pragma once
#include "core.h"
#include "gl.h"

typedef struct at_render_object {
    GLuint vao, vbo, ebo, instance_vbo, shader_program; 
    sz data_size;
} at_render_object;

typedef struct at_render {
    GLFWwindow* window;
    at_render_object* object;
    sz object_count;
} at_render;

extern void at_init_render(at_render* render);
extern void at_free_render(at_render* render);
extern void at_draw_render(at_render* render);
extern b8 at_should_loop_render(at_render* render);
extern void at_add_render_object(at_render* render, at_render_object* object);
extern void at_remove_render_object(at_render* render, at_render_object* object);
void at_free_render_object(at_render_object* object);

extern void at_candles_to_render_object(at_candle_array* candles, at_render_object* object);
extern void at_ticks_to_render_object(at_tick* ticks, sz tick_count, at_render_object* object);

GLuint at_compile_shader(const c8* vertex_src, const c8* fragment_src);
