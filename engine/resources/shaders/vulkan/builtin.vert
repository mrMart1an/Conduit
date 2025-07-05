#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 text_coord;

layout(binding = 0) uniform CameraModel {
    mat4 model;
    mat4 view;
    mat4 proj;
} cam;

layout(location = 0) out vec4 out_color;

void main() {
    gl_Position = cam.proj * cam.view * cam.model * vec4(in_position, 1.0);
    out_color = in_color;
}
