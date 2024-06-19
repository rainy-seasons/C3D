#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform float normLen;

in DATA
{
    vec3 Normal;
	vec3 color;
	vec2 texCoord;
    mat4 projection;
} data_in[];


void main()
{
	// Transform vertices of the triangle by modelMatrix and camMatrix
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = data_in[i].projection * gl_in[i].gl_Position;
        EmitVertex();
        gl_Position = data_in[i].projection * (gl_in[i].gl_Position + normLen * vec4(data_in[i].Normal, 0.0f));
        EmitVertex();
        EndPrimitive();
    }
}