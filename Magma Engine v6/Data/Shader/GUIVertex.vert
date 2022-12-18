#version 450 core
layout(location = 0) in vec2 InPos;
layout(location = 1) in vec2 InUV;
layout(location = 2) in vec4 InColor;
out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out struct { vec2 UV; vec4 Color;  } Out;
void main()
{
	gl_Position = vec4(InPos, 0.0, 1.0);
	Out.UV = InUV;
	Out.Color = InColor;
}