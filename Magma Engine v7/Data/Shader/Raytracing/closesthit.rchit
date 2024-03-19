#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable

struct PayLoad
{
	vec3 HitValue;
	float Distance;
	vec3 Normal;
	float Reflector;
	uint Index;
	bool WasInShadow;
	uint Time;
	bool Reflect;
};

layout(location = 0) rayPayloadInEXT PayLoad HitValue;
layout(location = 1) rayPayloadEXT bool shadowed;
hitAttributeEXT vec2 Attribs;

struct Vertex
{
	vec4 pos;
	vec4 normal;
};

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 2, set = 0) uniform UniformBufferObject 
{
	mat4 ViewInverse;
	mat4 ProjInverse;
	vec4 LightDir;
	uint Time;
} UBO;

layout(binding = 3, set = 0) buffer Vertices { vec4 v[]; } vertices;
layout(binding = 4, set = 0) buffer Indices { uint i[]; } indices;
layout(binding = 5, set = 0) uniform sampler2D textures[];

struct VertexUnPacked
{
	vec3 Pos;
	vec3 Normal;
	vec2 TexCoord;
	float VertexOffset;
	float TexIndex;
};

VertexUnPacked unpack(uint index)
{
	// Unpack the vertices from the SSBO using the glTF vertex structure
	// The multiplier is the size of the vertex divided by four float components (=16 bytes)
	const int vertexSize = 16 * 3;
	const int m = vertexSize / 16;

	vec4 d0 = vertices.v[m * index + 0];
	vec4 d1 = vertices.v[m * index + 1];
	vec4 d2 = vertices.v[m * index + 2];

	VertexUnPacked v;
	v.Pos = d0.xyz;
	v.Normal = d1.xyz;
	v.TexCoord = vec2(d0.w, d1.w);
	v.VertexOffset = d2.x;
	v.TexIndex = d2.y;

	return v;
}


vec3 Uncharted2Tonemap(vec3 x)
{
	const float A = 0.15;
	const float B = 0.50;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.02;
	const float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}


void main()
{
	const vec3 BarycentricCoords = vec3(1.0f - Attribs.x - Attribs.y, Attribs.x, Attribs.y);


	ivec3 index = ivec3(indices.i[3 * gl_PrimitiveID], indices.i[3 * gl_PrimitiveID + 1], indices.i[3 * gl_PrimitiveID + 2]);

	VertexUnPacked v0 = unpack(index.x);
	VertexUnPacked v1 = unpack(index.y);
	VertexUnPacked v2 = unpack(index.z);

	
	vec3 normal = normalize(v0.Normal.xyz * BarycentricCoords.x + v1.Normal.xyz * BarycentricCoords.y + v2.Normal.xyz * BarycentricCoords.z);
	vec2 texCoord = v0.TexCoord * BarycentricCoords.x + v1.TexCoord * BarycentricCoords.y + v2.TexCoord * BarycentricCoords.z;
//	float TexIndex = v0.TexIndex * BarycentricCoords.x + v1.TexIndex * BarycentricCoords.y + v2.TexIndex * BarycentricCoords.z;
	float TexIndex = v0.TexIndex;

	float dot_product = max(dot(UBO.LightDir.xyz, normal), 0.6) * 2.0;
	HitValue.HitValue = vec3(texture(textures[nonuniformEXT(int(TexIndex))], texCoord).xyz) * vec3(dot_product);
//	HitValue.HitValue = vec3(dot_product);
	
	if (TexIndex == 12)
		HitValue.Reflect = true;
	else
		HitValue.Reflect = false;

	float Exposure = 4.0;
	float Gamma = 0.9;//1.3

	HitValue.HitValue = Uncharted2Tonemap(HitValue.HitValue * Exposure);

	const vec3 Uncharted2TonemapConst = (1.0f / Uncharted2Tonemap(vec3(11.2f)));
	HitValue.HitValue = HitValue.HitValue * Uncharted2TonemapConst;	
	HitValue.HitValue = pow(HitValue.HitValue, vec3(1.0f / Gamma));

	float Reflect = .3 - ((HitValue.HitValue.x + HitValue.HitValue.y + HitValue.HitValue.z) / 3);

//	HitValue.HitValue = vec3(texture(textures[nonuniformEXT(int(TexIndex))], texCoord).xyz * 0.6) * vec3(dot_product);
//	HitValue.HitValue = vec3(texture(textures[nonuniformEXT(int(1))], texCoord).xyz);
//	HitValue.HitValue = vec3((texCoord), 1.0);
//	HitValue.HitValue = vec3(1.0, 0.0, 0.0) * vec3(dot_product);
	HitValue.Distance = gl_RayTmaxEXT;
	HitValue.Normal = normal;
//	if (normal.y > 0.99)
//		HitValue.Reflector = -0.9;
//	else
		HitValue.Reflector = .8;
 
//	HitValue.HitValue = vec3(texture(textures[nonuniformEXT(int(TexIndex))], texCoord).xyz);

	// Shadow casting
	if (HitValue.Index == 0)
	{
		float tmin = 0.001;
		float tmax = 10000.0;
		vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
		shadowed = true;  
		// Trace shadow ray and offset indices to match shadow hit/miss shader group indices
		traceRayEXT(topLevelAS, gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, UBO.LightDir.xyz, tmax, 1);
		if (shadowed) {
			vec3 Ref = vec3(0.6) * 0.2;
		//	if (HitValue.HitValue.x > Ref.x &&
		//		HitValue.HitValue.y > Ref.y &&
		//		HitValue.HitValue.z > Ref.z)
				HitValue.HitValue *= 0.6;
		//	HitValue.Reflector = -0.1;
			HitValue.WasInShadow = true;
		//	HitValue.Reflector = 0.0;
		}
	}
	else
	{
	//	if (HitValue.WasInShadow)
	//	{
	//		HitValue.HitValue *= 0.3;
	//	//	HitValue.Reflector = 0.0;
	//	}
	}

	

//	HitValue = texture(Texture, texCoord).xyz
}
