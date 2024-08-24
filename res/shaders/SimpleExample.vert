#version 460

vec2 positions[18] = vec2[](
    vec2(-1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2( 1.0, -1.0),
    vec2( 0.0, -0.75),
    vec2(-0.75, 0.75),
    vec2( 0.75, 0.75),
    vec2(-0.345,-0.059),
    vec2( 0.0,  0.75),
    vec2( 0.345,-0.059),
    vec2( 0.0, -0.06),
    vec2(-0.16, 0.37),
    vec2( 0.16, 0.37),
    vec2(-0.08,  0.16),
    vec2( 0.0,  0.37),
    vec2( 0.08,  0.16)
);

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    if(gl_VertexIndex >= 0 && gl_VertexIndex < 6)
    	fragColor = vec3(0.0, 1.0, 0.0);
    else if(gl_VertexIndex >= 6 && gl_VertexIndex < 9)
    	fragColor = vec3(1.0, 0.0, 0.0);
    else if(gl_VertexIndex >= 9 && gl_VertexIndex < 12)
    	fragColor = vec3(0.0, 0.0, 1.0);
    else if(gl_VertexIndex >= 12 && gl_VertexIndex < 15)
    	fragColor = vec3(1.0, 1.0, 0.0);
    else if(gl_VertexIndex >= 15 && gl_VertexIndex < 18)
    	fragColor = vec3(1.0, 1.0, 1.0);
}
