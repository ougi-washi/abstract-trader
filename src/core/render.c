// abstract-trader https://github.com/ougi-washi/abstract-trader
#include "render.h"
#include "log.h"
#include <assert.h>

const char* vertex_shader_src = 
"#version 330 core\n"
"layout(location = 0) in vec2 aPos;\n"  // 2D position (quad)
"layout(location = 1) in vec2 instancePos;\n"  // Instance position (Open, High)
"layout(location = 2) in vec3 instanceColor;\n"  // Instance color
"out vec3 fragColor;\n"
"void main() {\n"
"    gl_Position = vec4(aPos + instancePos, 0.0, 1.0);\n"
"    fragColor = instanceColor;\n"
"}\n";

const char* fragment_shader_src = 
"#version 330 core\n"
"in vec3 fragColor;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(fragColor, 1.0f);\n"
"}\n";

#define AT_WINDOW_TITLE "Abstract Trader"
#define AT_WINDOW_WIDTH 800
#define AT_WINDOW_HEIGHT 600

void at_gl_error_check(){
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        log_error("OpenGL error: %d", error);
    }
}

void at_init_render(at_render *render){
    if (!glfwInit()) {
        log_error("Failed to initialize GLFW");
    }

    render->window = glfwCreateWindow(AT_WINDOW_WIDTH, AT_WINDOW_HEIGHT, AT_WINDOW_TITLE, NULL, NULL);
    if (!render->window) {
        glfwTerminate();
    }

    glfwMakeContextCurrent(render->window);
    glfwSwapInterval(1);  // Enable vsync

    at_init_opengl();
    glViewport(0, 0, AT_WINDOW_WIDTH, AT_WINDOW_HEIGHT); // Adjust this as per the actual window size

    render->object = NULL;
    render->object_count = 0;

    at_gl_error_check();
}

void at_free_render(at_render *render){
    for (sz i = 0; i < render->object_count; ++i) {
        glDeleteBuffers(1, &render->object[i].vbo);
        glDeleteBuffers(1, &render->object[i].instance_vbo);
        glDeleteVertexArrays(1, &render->object[i].vao);
    }
    free(render->object);
    glfwDestroyWindow(render->window);
    glfwTerminate();

    at_gl_error_check();
}

void at_draw_render(at_render *render){
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (sz i = 0; i < render->object_count; ++i) {
        at_render_object* obj = &render->object[i];
        glUseProgram(obj->shader_program);
        glBindVertexArray(obj->vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, obj->data_size);
    }

    glfwSwapBuffers(render->window);
    glfwPollEvents();

    at_gl_error_check();
}

b8 at_should_loop_render(at_render *render){
    assert(render);
    return !glfwWindowShouldClose(render->window);
}

void at_add_render_object(at_render *render, at_render_object *object){
    render->object = realloc(render->object, (render->object_count + 1) * sizeof(at_render_object));
    render->object[render->object_count++] = *object;
}

void at_remove_render_object(at_render *render, at_render_object *object){
    sz index = -1;
    for (sz i = 0; i < render->object_count; ++i) {
        if (render->object[i].vao == object->vao) {
            index = i;
            break;
        }
    }
    if (index != (sz)-1) {
        at_free_render_object(&render->object[index]);
        for (sz i = index; i < render->object_count - 1; ++i) {
            render->object[i] = render->object[i + 1];
        }
        render->object_count--;
        render->object = realloc(render->object, render->object_count * sizeof(at_render_object));
    }
}

void at_free_render_object(at_render_object *object){
    if (object->vao) {
        glDeleteVertexArrays(1, &object->vao);
    }
    if (object->vbo) {
        glDeleteBuffers(1, &object->vbo);
    }
    if (object->instance_vbo) {
        glDeleteBuffers(1, &object->instance_vbo);
    }
    if (object->shader_program) {
        glDeleteProgram(object->shader_program);
    }
}

f32 normalize_value(f32 value, f32 min_val, f32 max_val) {
    f32 normalized = 2.0f * ((value - min_val) / (max_val - min_val)) - 1.0f;
    if (normalized < -1.0f) {
        return -1.0f;
    } else if (normalized > 1.0f) {
        return 1.0f;
    }
    return normalized;
}
void at_candles_to_render_object(at_candle *candles, sz candle_count, at_render_object *object) {
    
    GLfloat* instance_data = malloc(candle_count * 6 * sizeof(GLfloat));  // 2 for position, 3 for color
    f32 min_value = 0.0f;
    f32 max_value = 200.0f;

    // 2 triangles, 4 vertices
    GLfloat quad_vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    for (sz i = 0; i < candle_count; ++i) {
        instance_data[i * 6 + 0] = normalize_value(candles[i].open, min_value, max_value);   // Position X (Open)
        instance_data[i * 6 + 1] = normalize_value(candles[i].high, min_value, max_value);   // Position Y (High)

        i8 candle_dir = at_get_candle_direction(&candles[i]);
        instance_data[i * 6 + 2] = candle_dir == 1 ? 0.f : 1.f;   // Red
        instance_data[i * 6 + 3] = candle_dir == 1 ? 1.0f : 0.0f;   // Green
        instance_data[i * 6 + 4] = 0.0f;   // Blue
    }

    glGenVertexArrays(1, &object->vao);
    glBindVertexArray(object->vao);

    GLuint quad_vbo;
    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);  // Position data for the quad
    glEnableVertexAttribArray(0);
    
    glGenBuffers(1, &object->instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, object->instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, candle_count * 6 * sizeof(GLfloat), instance_data, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0); // Instance Position (X, Y)
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);  // Use per-instance data

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);  // Use per-instance data

    object->shader_program = at_compile_shader(vertex_shader_src, fragment_shader_src);
    object->data_size = candle_count;

    free(instance_data);
}


void at_ticks_to_render_object(at_tick *ticks, sz tick_count, at_render_object *object){
    GLfloat* instance_data = malloc(tick_count * 2 * sizeof(GLfloat)); // Position data
    for (sz i = 0; i < tick_count; ++i) {
        instance_data[i * 2 + 0] = ticks[i].price;  // Price
        instance_data[i * 2 + 1] = ticks[i].volume; // Volume
    }

    glGenVertexArrays(1, &object->vao);
    glBindVertexArray(object->vao);

    glGenBuffers(1, &object->instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, object->instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, tick_count * 2 * sizeof(GLfloat), instance_data, GL_STATIC_DRAW);

    // Define instance attributes (per tick data)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    glVertexAttribDivisor(0, 1);

    object->shader_program = at_compile_shader(vertex_shader_src, fragment_shader_src);
    object->data_size = tick_count;

    free(instance_data);
}

GLuint at_compile_shader(const char *vertex_src, const char *fragment_src){
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_src, NULL);
    glCompileShader(vertex_shader);

    // Check for compile errors
    GLint success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint log_length;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
        char* log = (char*)malloc(log_length);
        glGetShaderInfoLog(vertex_shader, log_length, &log_length, log);
        log_error("Error compiling vertex shader: %s\n", log);
        assert(success);
        free(log);
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_src, NULL);
    glCompileShader(fragment_shader);

    // Check for compile errors
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint log_length;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
        char* log = (char*)malloc(log_length);
        glGetShaderInfoLog(fragment_shader, log_length, &log_length, log);
        log_error("Error compiling fragment shader: %s\n", log);
        assert(success);
        free(log);
    }

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Check for linking errors
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint log_length;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);
        char* log = (char*)malloc(log_length);
        glGetProgramInfoLog(shader_program, log_length, &log_length, log);
        log_error("Error linking shader program: %s\n", log);
        assert(success);
        free(log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}
