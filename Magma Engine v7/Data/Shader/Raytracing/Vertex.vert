#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 InPos;

void main() 
{
	gl_Position = vec4(InPos, 0.0, 1.0);
}