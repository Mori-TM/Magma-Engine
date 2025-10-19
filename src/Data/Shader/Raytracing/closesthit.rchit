#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

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


layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 2, set = 0) uniform UniformBufferObject 
{
	mat4 ViewInverse;
	mat4 ProjInverse;
	vec4 LightDir;
	uint Time;
} UBO;

struct ObjDesc
{
  uint64_t      txtOffset;             // Texture index offset in the array of textures
  uint64_t vertexAddress;         // Address of the Vertex buffer
 
	uint64_t indexAddress;          // Address of the index buffer
  uint64_t materialAddress;       // Address of the material buffer

	uint64_t materialIndexAddress;  // Address of the triangle material index buffer
	uint64_t Allignment;

	uint64_t bs1;
	uint64_t bs2;
};

/*
typedef struct
{
	float VertexOffset;//Used for Indices offset
	float TextureIndex;
	float Unused0;
	float Unused1;
} SceneVertexExtData;

typedef struct
{
	vec4 PosTexX;
	vec4 NormalTexY;
	SceneVertexExtData Data;

//	vec3 Pos;
//	vec2 TexCoord;
//	vec3 Normal;
} SceneVertex;
*/

layout(binding = 3, set = 0) uniform sampler2D textures[];

layout(buffer_reference, scalar) buffer Vertices {vec4 v[]; }; // Positions of an object
layout(buffer_reference, scalar) buffer Indices {uint i[]; };
layout(set = 1, binding = 0) buffer ObjDesc_ { ObjDesc i[]; } objDesc;

struct Vertex
{
  vec3 pos;
  vec2 uv;
};

struct Triangle {
	Vertex vertices[3];
	vec2 uv;
};

// This function will unpack our vertex buffer data into a single triangle and calculates uv coordinates
Triangle unpackTriangle(uint index, int vertexSize) 
{
	ObjDesc    objResource = objDesc.i[0];//gl_InstanceCustomIndexEXT

	Triangle tri;
	const uint triIndex = index * 3;

//	Indices    indices     = Indices(bufferReferences.indices);
	Indices    indices     = Indices(objResource.indexAddress);
	Vertices   vertices    = Vertices(objResource.vertexAddress);

	// Unpack vertices
	// Data is packed as vec4 so we can map to the glTF vertex structure from the host side
	for (uint i = 0; i < 3; i++) {
		const uint offset = indices.i[triIndex + i] * (vertexSize / 16);
		vec4 d0 = vertices.v[offset + 0]; // pos.xyz, n.x
		vec4 d1 = vertices.v[offset + 1]; // n.yz, uv.xy
		tri.vertices[i].pos = d0.xyz;
		tri.vertices[i].uv = vec2(d0.w, d1.w);
	}
	// Calculate values at barycentric coordinates
	vec3 barycentricCoords = vec3(1.0f - Attribs.x - Attribs.y, Attribs.x, Attribs.y);
	tri.uv = tri.vertices[0].uv * barycentricCoords.x + tri.vertices[1].uv * barycentricCoords.y + tri.vertices[2].uv * barycentricCoords.z;
	return tri;
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

	ObjDesc    objResource = objDesc.i[gl_InstanceID];

	Triangle tri = unpackTriangle(gl_PrimitiveID, 48);


	HitValue.HitValue = vec3(tri.uv, 0.0);//tri.uv, gl_InstanceID

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

/*


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

struct Vertex
{
	vec4 pos;
	vec4 normal;
};

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
	
	if (TexIndex == 36)//17
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
*/