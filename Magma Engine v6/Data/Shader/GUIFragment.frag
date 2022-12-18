#version 450 core
layout(location = 0) out vec4 OutColor;
layout(set = 0, binding = 0) uniform sampler2D Sampler;
layout(location = 0) in struct { vec2 UV; vec4 Color; } In;

float RoundedBoxSDF(vec2 CenterPosition, vec2 Size, float Radius) {
//	return length(max(abs(CenterPosition),Size) - Size) - Radius;
	return length(max(abs(CenterPosition)-Size+Radius,0.0))-Radius;
}

const float Smoothing = 1.0/2.0;

const vec3 Style = vec3(0.1, 0.4, 0.75);//Blue
//const vec3 Style = vec3(0.9, 0.3, 0.2);//red

void main()
{
	OutColor = In.Color;
	if (In.UV.s < 500.0 || In.UV.t < 500.0)
	{
		
		float Distance = texture(Sampler, In.UV.st).r;
		float Alpha = smoothstep(0.5 - Smoothing, 0.5 + Smoothing, Distance);
	//	float Alpha = texture(Sampler, In.UV.st).r;
		OutColor.a = Alpha;
	//	if (Alpha <= 0.7 && Alpha != 0.0)
	//		OutColor = vec4(1.0) * (Alpha * 2.0);
	}
	else
	{
		if (In.Color.z == 0.0)
		{
			OutColor.rgb = vec3(In.Color.a) * Style;
			OutColor.a = 1.0;
		}
		else
		{
			vec2 UV = In.UV - vec2(1000.0);
			float EdgeSoftness = Smoothing * 2.0;
	
			// The radius of the corners (in pixels).
			float Radius = In.Color.z;
			vec2 Size = In.Color.xy;
			if (Radius > Size.x)
				Radius = Size.x;
			if (Radius > Size.y)
				Radius = Size.y;

			// Calculate distance to edge.   
			float Distance = RoundedBoxSDF((UV * Size), Size, Radius);
	
			// Smooth the result (free antialiasing).
			float SmoothedAlpha =  smoothstep(0.0f, EdgeSoftness * 2.0f, Distance);
	
			// Return the resultant shape.
			OutColor.rgb = vec3(In.Color.a) * Style;	
			OutColor.a =  1.0 - SmoothedAlpha;
		}
	}
}