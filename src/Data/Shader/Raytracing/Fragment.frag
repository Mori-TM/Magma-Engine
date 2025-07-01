#version 450
#extension GL_ARB_separate_shader_objects : enable
#include "hg_sdf.frag"

layout(location = 0) out vec4 OutColor;

layout(set = 0, binding = 0) uniform UniformBufferObject
{
	vec4 CameraPos;
	vec4 CameraLookAt;
	vec4 LightPos;
	vec4 Background;
	vec2 Resolution;
	uint Ticks;
	uint MsaaSamples;

	int MaxSteps;
	float Fov;
	float MaxDist;
	float Epsilon;
} UBO;

float fDisplace(vec3 P) 
{
	float Time = UBO.Ticks * 0.001;
	pR(P.yz, cos(2.0 * Time));
	return (sin(P.x + 8.0 * Time) * cos(P.y + sin(8.0 * Time)) * cos(P.z + 8.0 * Time));
}

vec2 fOpUnionID(vec2 Res1, vec2 Res2) 
{
	return (Res1.x < Res2.x) ? Res1 : Res2;
}

vec2 fOpDifferenceID(vec2 Res1, vec2 Res2) 
{
	return (Res1.x > -Res2.x) ? Res1 : vec2(-Res2.x, Res2.y);
}

vec2 fOpDifferenceColumnsID(vec2 Res1, vec2 Res2, float R, float N) 
{
	float Dist = fOpDifferenceColumns(Res1.x, Res2.x, R, N);
	return (Res1.x > -Res2.x) ? vec2(Dist, Res1.y) : vec2(Dist, Res2.y);
}

vec2 fOpUnionStairsID(vec2 Res1, vec2 Res2, float R, float N) 
{
	float Dist = fOpUnionStairs(Res1.x, Res2.x, R, N);
	return (Res1.x < Res2.x) ? vec2(Dist, Res1.y) : vec2(Dist, Res2.y);
}

vec2 fOpUnionChamferID(vec2 Res1, vec2 Res2, float R) 
{
	float Dist = fOpUnionChamfer(Res1.x, Res2.x, R);
	return (Res1.x < Res2.x) ? vec2(Dist, Res1.y) : vec2(Dist, Res2.y);
}

#define MESH_PLANE 0
#define MESH_BOX 1
#define MESH_SPHERE 2
#define MESH_CYLINDER 3
#define MESH_TORUS 4
#define MESH_CAPSULE 5
#define MESH_CONE 6
#define MESH_BOX2 7

vec2 AddMesh(vec3 P, float ID, vec4 Scale, int Type)
{
	float Distance = 0.0;
	vec2 Mesh = vec2(0.0);

	switch (Type)
	{
		case MESH_PLANE:
			Distance = fPlane(P, Scale.xyz, Scale.w);
			Mesh = vec2(Distance, ID);
			break;

		case MESH_BOX:
			Distance = fBox(P, Scale.xyz);
			Mesh = vec2(Distance, ID);
			break;

		case MESH_SPHERE:
			Distance = fSphere(P, Scale.x);
			Mesh = vec2(Distance, ID);
			break;

		case MESH_CYLINDER:
			Distance = fCylinder(P, Scale.x, Scale.y);
			Mesh = vec2(Distance, ID);
			break;

		case MESH_TORUS:
			Distance = fTorus(P, Scale.x, Scale.y);
			Mesh = vec2(Distance, ID);
			break;

		case MESH_CAPSULE:
			Distance = fCapsule(P, Scale.x, Scale.y);
			Mesh = vec2(Distance, ID);
			break;

		case MESH_CONE:
			Distance = fCone(P, Scale.x, Scale.y);
			Mesh = vec2(Distance, ID);
			break;

		case MESH_BOX2:
			Distance = fBox2(P.xy, Scale.xy);
			Mesh = vec2(Distance, ID);
			break;
	}

	return Mesh;
}

vec2 Map(vec3 P)
{
/*
	float PlaneDist = fPlane(P, vec3(0, 1, 0), 14.0);
	float PlaneID = 2.0;
	vec2 Plane = vec2(PlaneDist, PlaneID);

	float SphereDist = fSphere(P, 9.0);
	float SphereID = 1.0;
	vec2 Sphere = vec2(SphereDist, SphereID);

	float CubeDist = fBox(P, vec3(7.0));
	float CubeID = 1.0;
	vec2 Cube = vec2(CubeDist, CubeID);

	pMirrorOctant(P.xz, vec2(50, 50));
	P.x = -abs(P.x) + 20;
	pMod1(P.z, 15);

	vec3 PR = P;
	PR.y -= 15.5;
	pR(PR.xy, 0.6);
	PR.x -= 18.0;
	float RoofDist = fBox2(PR.xy, vec2(20, 0.3));
	float RoofID = 3.0;
	vec2 Roof = vec2(RoofDist, RoofID);

	float BoxDist = fBox(P, vec3(3, 9, 4));
	float BoxID = 1.0;
	vec2 Box = vec2(BoxDist, BoxID);

	vec3 PC = P;
	PC.y -= 9.0;
	float CylinderDist = fCylinder(PC.yxz, 4, 3);
	float CylinderID = 1.0;
	vec2 Cylinder = vec2(CylinderDist, CylinderID);

	float WallDist = fBox2(P.xy, vec2(1, 15));
	float WallID = 1.0;
	vec2 Wall = vec2(WallDist, WallID);

	vec2 Result;
	Result = fOpUnionID(Box, Cylinder);
	Result = fOpDifferenceColumnsID(Wall, Result, 0.6, 3.0);
	Result = fOpUnionChamferID(Result, Roof, 0.9);
	Result = fOpUnionStairsID(Result, Plane, 4.0, 5.0);
	vec2 r = fOpDifferenceID(Cube, Sphere);
	Result = fOpUnionID(Result, r);
*/
	vec2 Plane = AddMesh(P, 2.0, vec4(0.0, 1.0, 0.0, 14.0), MESH_PLANE);
	vec2 Sphere = AddMesh(P, 3.0, vec4(9.0), MESH_SPHERE);
	vec2 Cube = AddMesh(P, 1.0, vec4(7.0), MESH_BOX);

//	vec2 Plane = AddMesh(P, 2.0, vec4(0.0, 1.0, 0.0, 14.0), MESH_PLANE);
//	vec2 Sphere = AddMesh(P, 3.0, vec4(9.5, 15.0, 0.0, 0.0), MESH_CONE);
//	vec2 Cube = AddMesh(P, 1.0, vec4(7.0), MESH_BOX);

	vec2 Sphere2 = AddMesh(P, 2.0, vec4(9.5), MESH_SPHERE);
	vec2 Cube2 = AddMesh(P, 1.0, vec4(7.5), MESH_BOX);

	pMirrorOctant(P.xz, vec2(50, 50));
	P.x = -abs(P.x) + 20;
	pMod1(P.z, 15);
	vec3 PR = P;
	PR.y -= 15.5;
	pR(PR.xy, 0.6);
	PR.x -= 18.0;
	vec2 Roof = AddMesh(PR, 3.0, vec4(20.0, 0.3, 0.0, 0.0), MESH_BOX2);

	vec2 Box = AddMesh(P, 1.0, vec4(3.0, 9.0, 4.0, 0.0), MESH_BOX);

	vec3 PC = P;
	PC.y -= 9.0;
	vec2 Cylinder = AddMesh(PC.yxz, 1.0, vec4(3.0, 9.0, 4.0, 0.0), MESH_CYLINDER);
	vec2 Wall = AddMesh(P, 1.0, vec4(1.0, 15.0, 0.0, 0.0), MESH_BOX2);

	vec2 Result;
	Result = fOpUnionID(Box, Cylinder);
	Result = fOpDifferenceColumnsID(Wall, Result, 0.6, 3.0);
	Result = fOpUnionChamferID(Result, Roof, 0.9);
	Result = fOpUnionStairsID(Result, Plane, 4.0, 5.0);
	vec2 r = fOpDifferenceID(Cube, Sphere);
	vec2 r2 = fOpDifferenceID(Cube2, Sphere2);
	vec2 r3 = fOpDifferenceID(r, r2);
	Result = fOpUnionID(Result, r3);

	return Result;
}

vec2 RayMarch(vec3 RayOrigin, vec3 RayDirection)
{
	vec2 Hit = vec2(0.0);
	vec2 Object = vec2(0.0);

	for (int i = 0; i < UBO.MaxSteps; i++)
	{
		vec3 P = RayOrigin + Object.x * RayDirection;
		Hit = Map(P);
		Object.x += Hit.x;
		Object.y = Hit.y;
		if (abs(Hit.x) < UBO.Epsilon || Object.x > UBO.MaxDist)
			break;
	}

	return Object;
}

vec3 GetNormal(vec3 P)
{
	vec2 E = vec2(UBO.Epsilon, 0.0);
	vec3 N = vec3(Map(P).x) - vec3(Map(P - E.xyy).x, Map(P - E.yxy).x, Map(P - E.yyx).x);
	return normalize(N);
}

float GetSoftShadow(vec3 P, vec3 LightPos) 
{
	float Result = 1.0;
	float Distance = 0.01;
	float LightSize = 0.01;
	for (int i = 0; i < UBO.MaxSteps; i++) 
	{
		float Hit = Map(P + LightPos * Distance).x;
		Result = min(Result, Hit / (Distance * LightSize));
		Distance += Hit;
		if (Hit < 0.0001 || Distance > 60.0) 
			break;
	}
	return clamp(Result, 0.0, 1.0);
}

float GetAmbientOcclusion(vec3 P, vec3 Normal) 
{
	float Occlusion = 0.0;
	float Weight = 1.0;
	for (int i = 0; i < 8; i++) 
	{
		float Length = 0.01 + 0.02 * float(i * i);
		float Distance = Map(P + Normal * Length).x;
		Occlusion += (Length - Distance) * Weight;
		Weight *= 0.85;
	}
	return 1.0 - clamp(0.6 * Occlusion, 0.0, 1.0);
}

vec3 GetLight(vec3 P, vec3 RayDirection, vec3 Color)
{
//	vec3 LightPos = vec3(30.0 * sin(UBO.Ticks * 0.001), 40.0, 30.0 * cos(UBO.Ticks * 0.001));
	vec3 LightPos = UBO.LightPos.xyz;
	vec3 L = normalize(LightPos - P);
	vec3 N = GetNormal(P);
	vec3 V = -RayDirection;
	vec3 R = reflect(-L, N);

	vec3 SpecularColor = vec3(0.5);
	vec3 Specular = SpecularColor * pow(clamp(dot(R, V), 0.0, 1.0), 10.0);
	vec3 Diffuse = Color * clamp(dot(L, N), 0.0, 1.0);
	vec3 Ambient = Color * 0.05;
	vec3 Fresnel = 0.25 * Color * pow(1.0 + dot(RayDirection, N), 3.0);

	float Shadow = GetSoftShadow(P + N * 0.02, normalize(LightPos));
	float Occlusion = GetAmbientOcclusion(P, N);
	vec3 Back = 0.05 * Color * clamp(dot(N, -L), 0.0, 1.0);

	return  (Back + Ambient + Fresnel) * Occlusion + (Specular * Occlusion + Diffuse) * Shadow;
}

vec3 GetMaterial(vec3 P, float ID)
{
	vec3 Material;
	switch (int(ID))
	{
		case 1:
			Material = vec3(0.2, 0.4, 0.8);
			break;

		case 2:
			Material = vec3(0.2 + 0.4 * mod(floor(P.x) + floor(P.z), 2.0));
			break;
		case 3:
			vec2 i = step(fract(0.5 * P.xz), vec2(1.0 / 10.0));
			Material = ((1.0 - i.x) * (1.0 - i.y)) * vec3(0.37, 0.12, 0.0);
			break;
	}

	return Material;
}

mat3 GetCamera(vec3 RayOrigin, vec3 LookAt)
{
	vec3 CameraFront = normalize(vec3(LookAt - RayOrigin));
	vec3 CameraRight = normalize(cross(vec3(0.0, 1.0, 0.0), CameraFront));
	vec3 CameraUp = -cross(CameraFront, CameraRight);
	return mat3(CameraRight, CameraUp, CameraFront);
}

vec3 Render(in vec2 UV)
{
	vec3 Color = vec3(0.0);
	vec3 RayOrigin = UBO.CameraPos.xyz;
	vec3 LookAt =  UBO.CameraLookAt.xyz;
	vec3 RayDirection = GetCamera(RayOrigin, LookAt) * normalize(vec3(UV, UBO.Fov));

	vec3 Background = UBO.Background.xyz;

	vec2 Object = RayMarch(RayOrigin, RayDirection);
	if (Object.x < UBO.MaxDist)
	{
		vec3 P = RayOrigin + Object.x * RayDirection;
		vec3 Material = GetMaterial(P, Object.y);
		Color += GetLight(P, RayDirection, Material);
		//Fog
		Color = mix(Color, Background, 1.0 - exp(UBO.Background.w * -0.00002 * Object.x * Object.x));
	}
	else
	{
		Color += Background - max(0.95 * RayDirection.y, 0.0);
	}
	
	return Color;
}

vec2 GetUV(vec2 Offset)
{
	return (2.0 * (gl_FragCoord.xy + Offset) - UBO.Resolution.xy) / UBO.Resolution.y;
}

vec3 RenderAAx1()
{
	return Render(GetUV(vec2(0.0)));
}

vec3 RenderAAx2() 
{
	float BXY = int(gl_FragCoord.x + gl_FragCoord.y) & 1;
	float NBXY = 1.0 - BXY;
	vec3 ColorAA = (Render(GetUV(vec2(0.33 * NBXY, 0.))) + Render(GetUV(vec2(0.33 * BXY, 0.66))));
	return ColorAA / 2.0;
}

vec3 RenderAAx4()
{
	vec4 E = vec4(0.125, -0.125, 0.375, -0.375);
	vec3 ColorAA = Render(GetUV(E.xz)) + Render(GetUV(E.yw)) + Render(GetUV(E.wx)) + Render(GetUV(E.zy));
	return ColorAA /= 4.0;
}

void main()
{
	vec3 Color = vec3(0.0);

	if (UBO.MsaaSamples == 0)
		Color = RenderAAx1();
	else if (UBO.MsaaSamples == 1)
		Color = RenderAAx2();
	else if (UBO.MsaaSamples == 2)
		Color = RenderAAx4();

	OutColor = vec4(pow(Color, vec3(0.4545)), 1.0);
}