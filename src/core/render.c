// abstract-trader https://github.com/ougi-washi/abstract-trader
#include "render.h"
#include "log.h"
#include "fs.h"
#include <assert.h>

#define AT_WINDOW_TITLE "Abstract Trader"
#define AT_WINDOW_WIDTH 800
#define AT_WINDOW_HEIGHT 600


#define AT_CANDLE_VS_PATH "shaders/candle_vs.glsl"
#define AT_CANDLE_FS_PATH "shaders/candle_fs.glsl"
#define AT_TICK_VS_PATH "shaders/tick_vs.glsl"
#define AT_TICK_FS_PATH "shaders/tick_fs.glsl"

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
        glBindVertexArray(obj->vao);
        glUseProgram(obj->shader_program);
        glDrawElements(GL_TRIANGLES, obj->data_size * 6, GL_UNSIGNED_INT, 0);
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
    GLfloat* instance_data = malloc(candle_count * 4 * 7 * sizeof(GLfloat));
    GLuint* indices = malloc(candle_count * 6 * sizeof(GLuint));

    const f32 distance_between_candles = 0.05f;
    const f32 candle_width = 0.02f;
    f32 current_x = -1.0f;

    f32 min_value = 0.f;
    f32 max_value = 200.0f;

    for (sz i = 0; i < candle_count; ++i) {
        f32 x_left = current_x;
        f32 x_right = current_x + candle_width;
        f32 y_low = normalize_value(candles[i].low, min_value, max_value);
        f32 y_high = normalize_value(candles[i].high, min_value, max_value);

        const i8 candle_direction = at_get_candle_direction(&candles[i]);

        instance_data[i * 28 + 0] = x_left;  // Bottom left X
        instance_data[i * 28 + 1] = y_low;   // Bottom left Y
        instance_data[i * 28 + 2] = candle_direction == 1 ? 0.0f : 1.0f; // Color: Red
        instance_data[i * 28 + 3] = candle_direction == 1 ? 1.0f : 0.0f; // Color: Green
        instance_data[i * 28 + 4] = 0.0f;    // Color: Blue
        instance_data[i * 28 + 5] = 0.0f;    // u (texture coordinate, if needed)
        instance_data[i * 28 + 6] = 0.0f;    // v (texture coordinate, if needed)

        instance_data[i * 28 + 7] = x_right; // Bottom right X
        instance_data[i * 28 + 8] = y_low;   // Bottom right Y
        instance_data[i * 28 + 9] = candle_direction == 1 ? 0.0f : 1.0f;
        instance_data[i * 28 + 10] = candle_direction == 1 ? 1.0f : 0.0f;
        instance_data[i * 28 + 11] = 0.0f;
        instance_data[i * 28 + 12] = 1.0f;
        instance_data[i * 28 + 13] = 0.0f;

        instance_data[i * 28 + 14] = x_left;  // Top left X
        instance_data[i * 28 + 15] = y_high;  // Top left Y
        instance_data[i * 28 + 16] = candle_direction == 1 ? 0.0f : 1.0f;
        instance_data[i * 28 + 17] = candle_direction == 1 ? 1.0f : 0.0f;
        instance_data[i * 28 + 18] = 0.0f;
        instance_data[i * 28 + 19] = 0.0f;
        instance_data[i * 28 + 20] = 1.0f;

        instance_data[i * 28 + 21] = x_right; // Top right X
        instance_data[i * 28 + 22] = y_high;  // Top right Y
        instance_data[i * 28 + 23] = candle_direction == 1 ? 0.0f : 1.0f;
        instance_data[i * 28 + 24] = candle_direction == 1 ? 1.0f : 0.0f;
        instance_data[i * 28 + 25] = 0.0f;
        instance_data[i * 28 + 26] = 1.0f;
        instance_data[i * 28 + 27] = 1.0f;

        indices[i * 6 + 0] = i * 4 + 0; // Bottom-left
        indices[i * 6 + 1] = i * 4 + 1; // Bottom-right
        indices[i * 6 + 2] = i * 4 + 2; // Top-left
        indices[i * 6 + 3] = i * 4 + 2; // Top-left
        indices[i * 6 + 4] = i * 4 + 1; // Bottom-right
        indices[i * 6 + 5] = i * 4 + 3; // Top-right

        current_x += candle_width + distance_between_candles;
    }

    // Create VAO
    glGenVertexArrays(1, &object->vao);
    glBindVertexArray(object->vao);

    // Create VBO
    glGenBuffers(1, &object->instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, object->instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, candle_count * 4 * 7 * sizeof(GLfloat), instance_data, GL_STATIC_DRAW);

    // Create EBO
    glGenBuffers(1, &object->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, candle_count * 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    // Vertex Attributes
    glEnableVertexAttribArray(0); // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1); // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2); // Texture Coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

    char* vertex_shader_src = at_read_file(AT_CANDLE_VS_PATH);
    char* fragment_shader_src = at_read_file(AT_CANDLE_FS_PATH);
    object->shader_program = at_compile_shader(vertex_shader_src, fragment_shader_src);
    object->data_size = candle_count;

    free(instance_data);
    free(indices);
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

    const char* vertex_shader_src = at_read_file(AT_TICK_VS_PATH);
    const char* fragment_shader_src = at_read_file(AT_TICK_FS_PATH);
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
