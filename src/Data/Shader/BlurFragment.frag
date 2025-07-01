#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 FragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D SamplerColor;

layout(location = 0) out vec4 OutColor;

layout(push_constant) uniform PushConstants
{
	layout(offset = 0) uint Horizontal;
    layout(offset = 4) float BlurScale;
    layout(offset = 8) float BlurStrength;
} PushConst;

const float Weight[11] = float[] (0.08426433772693104, 0.0881395046984587, 0.09127622393803544, 0.09358479552487586, 0.09499801739725992, 0.09547424142887809, 0.09499801739725992, 0.09358479552487586, 0.09127622393803544, 0.0881395046984587, 0.08426433772693104);
const float Weight5[5] = float[] (0.18763017727110853, 0.20606651600549156, 0.21260661344679974, 0.20606651600549156, 0.18763017727110853);
//const float Weight[20] = float[] (0.002321663832297116, 0.004459996487200199, 0.008051314682360233, 0.013658236168322794, 0.0217729446865966, 0.03261629319043533, 0.04591429895379315, 0.06073775172801577, 0.0755036236986729, 0.0882011875184302, 0.0968226473630437, 0.09988008338166451, 0.0968226473630437, 0.0882011875184302, 0.0755036236986729, 0.06073775172801577, 0.04591429895379315, 0.03261629319043533, 0.0217729446865966, 0.013658236168322794);

const float Scale = 0.5;

void main()
{
    vec2 TexOffset = 1.0 / textureSize(SamplerColor, 0) * PushConst.BlurScale; // gets size of single texel
    vec3 Result = texture(SamplerColor, FragTexCoord).rgb * Weight5[0]; // current fragment's contribution
    if(PushConst.Horizontal == 1)
    {
        for(int i = 1; i < 5; ++i)
        {
            Result += texture(SamplerColor, FragTexCoord + vec2(TexOffset.x * i, 0.0)).rgb * Weight5[i] * PushConst.BlurStrength;
            Result += texture(SamplerColor, FragTexCoord - vec2(TexOffset.x * i, 0.0)).rgb * Weight5[i] * PushConst.BlurStrength;
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            Result += texture(SamplerColor, FragTexCoord + vec2(0.0, TexOffset.y * i)).rgb * Weight5[i] * PushConst.BlurStrength;
            Result += texture(SamplerColor, FragTexCoord - vec2(0.0, TexOffset.y * i)).rgb * Weight5[i] * PushConst.BlurStrength;
        }
    }

	OutColor = vec4(Result, 1.0);
}