#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 FragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D SamplerSSAO;

layout(location = 0) out float OutColor;

void main()
{
	const int BlurRange = 2;
	int n = 0;
	vec2 TexelSize = 1.0 / vec2(textureSize(SamplerSSAO, 0));
	float Result = 0.0;

	for (int x = -BlurRange; x < BlurRange; x++) 
	{
		for (int y = -BlurRange; y < BlurRange; y++) 
		{
			vec2 offset = vec2(float(x), float(y)) * TexelSize;
			Result += texture(SamplerSSAO, FragTexCoord + offset).r;
			n++;
		}
	}

	OutColor = Result / (float(n));
}