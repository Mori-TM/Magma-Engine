#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 OutColor;

layout(location = 0) in vec2 FragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D SamplerPosition;
layout(set = 0, binding = 1) uniform sampler2D SamplerNormal;
layout(set = 0, binding = 2) uniform sampler2D SamplerAlbedo;
layout(set = 0, binding = 3) uniform sampler2D SamplerPBR;

layout(set = 0, binding = 4) uniform UniformBufferObject
{
	mat4 Projection;
	mat4 InvProjection;
	mat4 View;
	mat4 InvView;
	uint MsaaSamples;
} UBO;
/*
void main()
{
	OutColor = texture(LightPass, FragTexCoord);
}
*/

/*
void main() {
  float maxDistance = 8;
  float resolution  = 0.3;
  int   steps       = 5;
  float thickness   = 0.5;

  vec2 texSize  = textureSize(DepthPosition, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  vec4 uv = vec4(0.0);

  vec4 positionFrom = texture(DepthPosition, texCoord);
  vec4 mask         = vec4(texture(PBRMap, texCoord).g);

  if (  positionFrom.w <= 0.0
	 || mask.r         <= 0.0
	 ) { OutColor = texture(LightPass, texCoord); return; }

  vec3 unitPositionFrom = normalize(positionFrom.xyz);
  vec3 normal           = normalize(texture(NormalMap, texCoord).xyz);
  vec3 pivot            = normalize(reflect(unitPositionFrom, normal));

  vec4 positionTo = positionFrom;

  vec4 startView = vec4(positionFrom.xyz + (pivot *         0.0), 1.0);
  vec4 endView   = vec4(positionFrom.xyz + (pivot * maxDistance), 1.0);

  vec4 startFrag      = startView;
	   startFrag      = UBO.Projection * startFrag;
	   startFrag.xyz /= startFrag.w;
	   startFrag.xy   = startFrag.xy * 0.5 + 0.5;
	   startFrag.xy  *= texSize;

  vec4 endFrag      = endView;
	   endFrag      = UBO.Projection * endFrag;
	   endFrag.xyz /= endFrag.w;
	   endFrag.xy   = endFrag.xy * 0.5 + 0.5;
	   endFrag.xy  *= texSize;

  vec2 frag  = startFrag.xy;
	   uv.xy = frag / texSize;

  float deltaX    = endFrag.x - startFrag.x;
  float deltaY    = endFrag.y - startFrag.y;
  float useX      = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
  float delta     = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0.0, 1.0);
  vec2  increment = vec2(deltaX, deltaY) / max(delta, 0.001);

  float search0 = 0;
  float search1 = 0;

  int hit0 = 0;
  int hit1 = 0;

  float viewDistance = startView.y;
  float depth        = thickness;

  float i = 0;

  for (i = 0; i < int(delta); ++i) {
	frag      += increment;
	uv.xy      = frag / texSize;
	positionTo = texture(DepthPosition, uv.xy);

	search1 =
	  mix
		( (frag.y - startFrag.y) / deltaY
		, (frag.x - startFrag.x) / deltaX
		, useX
		);

	search1 = clamp(search1, 0.0, 1.0);

	viewDistance = (startView.y * endView.y) / mix(endView.y, startView.y, search1);
	depth        = viewDistance - positionTo.y;

	if (depth > 0 && depth < thickness) {
	  hit0 = 1;
	  break;
	} else {
	  search0 = search1;
	}
  }

  search1 = search0 + ((search1 - search0) / 2.0);

  steps *= hit0;

  for (i = 0; i < steps; ++i) {
	frag       = mix(startFrag.xy, endFrag.xy, search1);
	uv.xy      = frag / texSize;
	positionTo = texture(DepthPosition, uv.xy);

	viewDistance = (startView.y * endView.y) / mix(endView.y, startView.y, search1);
	depth        = viewDistance - positionTo.y;

	if (depth > 0 && depth < thickness) {
	  hit1 = 1;
	  search1 = search0 + ((search1 - search0) / 2);
	} else {
	  float temp = search1;
	  search1 = search1 + ((search1 - search0) / 2);
	  search0 = temp;
	}
  }

  float visibility =
	  hit1
	* positionTo.w
	* ( 1
	  - max
		 ( dot(-unitPositionFrom, pivot)
		 , 0
		 )
	  )
	* ( 1
	  - clamp
		  ( depth / thickness
		  , 0
		  , 1
		  )
	  )
	* ( 1
	  - clamp
		  (   length(positionTo - positionFrom)
			/ maxDistance
		  , 0
		  , 1
		  )
	  )
	* (uv.x < 0 || uv.x > 1 ? 0 : 1)
	* (uv.y < 0 || uv.y > 1 ? 0 : 1);

  visibility = clamp(visibility, 0, 1);

  uv.ba = vec2(visibility);

  OutColor = uv;
}
*/
/*
const float step = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 30;
const int numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;

float Metallic;

#define Scale vec3(.8, .8, .8)
#define K 19.19


vec3 CalcViewPosition(in vec2 TexCoord)
{
	// Combine UV & depth into XY & Z (NDC)
	vec3 rawPosition                = vec3(TexCoord, texture(DepthPosition, TexCoord).r);

	// Convert from (0, 1) range to (-1, 1)
	vec4 ScreenSpacePosition        = vec4( rawPosition * 2 - 1, 1);

	// Undo Perspective transformation to bring into view space
	vec4 ViewPosition               = UBO.InvProjection * ScreenSpacePosition;

	// Perform perspective divide and return
	return                          ViewPosition.xyz / ViewPosition.w;
}




vec3 PositionFromDepth(float depth) {
	float z = depth * 2.0 - 1.0;

	vec4 clipSpacePosition = vec4(FragTexCoord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = UBO.InvProjection * clipSpacePosition;

	// Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	return viewSpacePosition.xyz;
}

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
	float depth;

	vec4 projectedCoord;
 
	for(int i = 0; i < numBinarySearchSteps; i++)
	{

		projectedCoord = UBO.Projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
		depth = texture(DepthPosition, projectedCoord.xy).z;

 
		dDepth = hitCoord.z - depth;

		dir *= 0.5;
		if(dDepth > 0.0)
			hitCoord += dir;
		else
			hitCoord -= dir;    
	}

		projectedCoord = UBO.Projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
	return vec3(projectedCoord.xy, depth);
}

vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{

	dir *= step;
 
 
	float depth;
	int steps;
	vec4 projectedCoord;

 
	for(int i = 0; i < maxSteps; i++)
	{
		hitCoord += dir;
 
		projectedCoord = UBO.Projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
		depth = texture(DepthPosition, projectedCoord.xy).z;
		if(depth > 1000.0)
			continue;
 
		dDepth = hitCoord.z - depth;

		if((dir.z - dDepth) < 1.2)
		{
			if(dDepth <= 0.0)
			{   
				vec4 Result;
				Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);

				return Result;
			}
		}
		
		steps++;
	}
 
	
	return vec4(projectedCoord.xy, depth, 0.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


vec3 hash(vec3 a)
{
	a = fract(a * Scale);
	a += dot(a, a.yxz + K);
	return fract((a.xxy + a.yxx)*a.zyx);
}

void main(){

	vec3 viewNormal = vec3(vec4(texture(NormalMap, FragTexCoord).xyz, 1.0) * UBO.InvView);
	vec3 viewPos = CalcViewPosition(FragTexCoord);
	vec3 albedo = texture(LightPass,FragTexCoord).rgb;

	Metallic = texture(PBRMap,FragTexCoord).r;



	float spec = texture(PBRMap,FragTexCoord).g;


   vec3 F0 = vec3(0.04); 
	F0      = mix(F0, albedo, Metallic);
	vec3 Fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPos)), 0.0), F0);


   vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));

  vec3 hitPos = viewPos;
	float dDepth;
 
	vec3 wp = vec3(vec4(viewPos, 1.0) * UBO.InvView);
	vec3 jitt = mix(vec3(0.0), vec3(hash(wp)), spec);
	vec4 coords = RayMarch((vec3(jitt) + reflected * max(minRayStep, -viewPos.z)), hitPos, dDepth);
 
	


	vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
 
	  float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

	float ReflectionMultiplier = pow(Metallic, reflectionSpecularFalloffExponent) * 
				screenEdgefactor * 
				-reflected.z;
 
	// Get color
	vec3 SSR = texture(LightPass, coords.xy).rgb;// * clamp(ReflectionMultiplier, 0.0, 0.9) * Fresnel;  



	OutColor = vec4(SSR, 1.0);


}
*/
/*
float rayStep = 0.1;
int iterationCount = 100;
float distanceBias = 0.03f;
bool enableSSR = true;
int sampleCount = 1;
bool isSamplingEnabled = false;
bool isExponentialStepEnabled = false;
bool isAdaptiveStepEnabled = true;
bool isBinarySearchEnabled = false;
bool debugDraw = false;
float samplingCoefficient = 0.001;

float random (vec2 uv) {
//	return uv.x * uv.y / 2.0 + 21.3214 * 0.01;
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453123); //simple random function
}

vec3 generatePositionFromDepth(vec2 texturePos, float depth) {
	vec4 ndc = vec4((texturePos - 0.5) * 2, depth, 1.f);
	vec4 inversed = UBO.InvProjection * ndc;// going back from projected
	inversed /= inversed.w;
	return inversed.xyz;
}

vec2 generateProjectedPosition(vec3 pos){
	vec4 samplePosition = UBO.Projection * vec4(pos, 1.f);
	samplePosition.xy = (samplePosition.xy / samplePosition.w) * 0.5 + 0.5;
	return samplePosition.xy;
}

vec3 SSR(vec3 position, vec3 reflection) {
	vec3 step = rayStep * reflection;
	vec3 marchingPosition = position + step;
	float delta;
	float depthFromScreen;
	vec2 screenPosition;
	
	int i = 0;
	for (; i < iterationCount; i++) {
		screenPosition = generateProjectedPosition(marchingPosition);
		depthFromScreen = abs(generatePositionFromDepth(screenPosition, texture(DepthPosition, screenPosition).x).z);
		delta = abs(marchingPosition.z) - depthFromScreen;
		if (abs(delta) < distanceBias) {
			vec3 color = vec3(1);
			if(debugDraw)
				color = vec3( 0.5+ sign(delta)/2,0.3,0.5- sign(delta)/2);
			return texture(LightPass, screenPosition).xyz * color;
		}
		if (isBinarySearchEnabled && delta > 0) {
			break;
		}
		if (isAdaptiveStepEnabled){
			float directionSign = sign(abs(marchingPosition.z) - depthFromScreen);
			//this is sort of adapting step, should prevent lining reflection by doing sort of iterative converging
			//some implementation doing it by binary search, but I found this idea more cheaty and way easier to implement
			step = step * (1.0 - rayStep * max(directionSign, 0.0));
			marchingPosition += step * (-directionSign);
		}
		else {
			marchingPosition += step;
		}
		if (isExponentialStepEnabled){
			step *= 1.05;
		}
	}
	if(isBinarySearchEnabled){
		for(; i < iterationCount; i++){
			
			step *= 0.5;
			marchingPosition = marchingPosition - step * sign(delta);
			
			screenPosition = generateProjectedPosition(marchingPosition);
			depthFromScreen = abs(generatePositionFromDepth(screenPosition, texture(DepthPosition, screenPosition).x).z);
			delta = abs(marchingPosition.z) - depthFromScreen;
			
			if (abs(delta) < distanceBias) {
				vec3 color = vec3(1);
				if(debugDraw)
					color = vec3( 0.5+ sign(delta)/2,0.3,0.5- sign(delta)/2);
				return texture(LightPass, screenPosition).xyz * color;
			}
		}
	}
	
	return vec3(0.0);
}

void main(){
//	OutColor = texture(LightPass, FragTexCoord);
//    OutColor = vec4(1.0);
//	return;

	vec3 position = generatePositionFromDepth(FragTexCoord, texture(DepthPosition, FragTexCoord).w);
	vec4 normal = vec4(texture(NormalMap, FragTexCoord).xyz, 0.0);
	float metallic = texture(PBRMap, FragTexCoord).x;
	if (!enableSSR || metallic < 0.01) {
		OutColor = texture(LightPass, FragTexCoord);
	} else {
		vec3 reflectionDirection = normalize(reflect(position, normalize(normal.xyz)));
		if (isSamplingEnabled) {
			vec3 firstBasis = normalize(cross(vec3(0.f, 0.f, 1.f), reflectionDirection));
			vec3 secondBasis = normalize(cross(reflectionDirection, firstBasis));
			vec4 resultingColor = vec4(0.f);
			for (int i = 0; i < sampleCount; i++) {
				vec2 coeffs = vec2(random(FragTexCoord + vec2(0, i)) + random(FragTexCoord + vec2(i, 0))) * samplingCoefficient;
				vec3 reflectionDirectionRandomized = reflectionDirection + firstBasis * coeffs.x + secondBasis * coeffs.y;
				vec3 tempColor = SSR(position, normalize(reflectionDirectionRandomized));
				if (tempColor != vec3(0.f)) {
					resultingColor += vec4(tempColor, 1.f);
				}
			}
			if (resultingColor.w == 0){
				OutColor = texture(LightPass, FragTexCoord);
			} else {
				resultingColor /= resultingColor.w;
				OutColor = vec4(resultingColor.xyz, 1.f);
			}
		}
		else {
			OutColor = vec4(SSR(position, normalize(reflectionDirection)), 1.f);
			if (OutColor.xyz == vec3(0.f)){
				OutColor = texture(LightPass, FragTexCoord);
			}
		}
	}
}
*/
/*
const float step = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 30;
const int numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;

float Metallic;

#define Scale vec3(.8, .8, .8)
#define K 19.19

vec3 PositionFromDepth(float depth);

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth);
 
vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth);

vec3 fresnelSchlick(float cosTheta, vec3 F0);

vec3 hash(vec3 a);

void main()
{
//	OutColor = vec4(texture(NormalMap, FragTexCoord).rgb, 1.0);
//	return;
//	OutColor = vec4(vec3(texture(NormalMap, FragTexCoord).w), 1.0);
//    return;

	vec2 PBR = texture(PBRMap, FragTexCoord).rg;
	Metallic = PBR.r;
//	Metallic = 1.0;

	if(Metallic < 0.01)
	{
		OutColor = texture(LightPass, FragTexCoord);
		return;
	}
	//    discard;
 
	vec3 viewNormal = vec3(vec4(texture(NormalMap, FragTexCoord).xyz, 1.0) * UBO.InvView);
	vec3 viewPos = textureLod(DepthPosition, FragTexCoord, 2).xyz;
//	vec3 ViewPos = PositionFromDepth(texture(DepthPosition, FragTexCoord).w);
//    vec3 viewPos = texture(DepthPosition, FragTexCoord).xyz;
	vec3 albedo = texture(LightPass, FragTexCoord).rgb;

	float spec = texture(NormalMap, FragTexCoord).w;

	vec3 F0 = vec3(0.04); 
	F0      = mix(F0, albedo, Metallic);
	vec3 Fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPos)), 0.0), F0);

	// Reflection vector
	vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));


	vec3 hitPos = viewPos;
	float dDepth;
 
	vec3 wp = vec3(vec4(viewPos, 1.0) * UBO.InvView);
	vec3 jitt = mix(vec3(0.0), vec3(hash(wp)), spec);
	vec4 coords = RayMarch((vec3(jitt) + reflected * max(minRayStep, -viewPos.z)), hitPos, dDepth);
 
 
	vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
 
 
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

	float ReflectionMultiplier = pow(Metallic, reflectionSpecularFalloffExponent) * 
				screenEdgefactor * 
				-reflected.z;
 
	// Get color
	vec3 SSR = textureLod(LightPass, coords.xy, 0).rgb * clamp(ReflectionMultiplier, 0.0, 0.9) * Fresnel;  

	OutColor = vec4(SSR, Metallic);
}

vec3 PositionFromDepth(float depth) {
	float z = depth * 2.0 - 1.0;

	vec4 clipSpacePosition = vec4(FragTexCoord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = UBO.InvProjection * clipSpacePosition;

	// Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	return viewSpacePosition.xyz;
}

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
	float depth;

	vec4 projectedCoord;
 
	for(int i = 0; i < numBinarySearchSteps; i++)
	{

		projectedCoord = UBO.Projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
		depth = textureLod(DepthPosition, projectedCoord.xy, 2).z;

 
		dDepth = hitCoord.z - depth;

		dir *= 0.5;
		if(dDepth > 0.0)
			hitCoord += dir;
		else
			hitCoord -= dir;    
	}

		projectedCoord = UBO.Projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
	return vec3(projectedCoord.xy, depth);
}

vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{

	dir *= step;
 
 
	float depth;
	int steps;
	vec4 projectedCoord;

 
	for(int i = 0; i < maxSteps; i++)
	{
		hitCoord += dir;
 
		projectedCoord = UBO.Projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
		depth = textureLod(DepthPosition, projectedCoord.xy, 2).z;
		if(depth > 1000.0)
			continue;
 
		dDepth = hitCoord.z - depth;

		if((dir.z - dDepth) < 1.2)
		{
			if(dDepth <= 0.0)
			{   
				vec4 Result;
				Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);

				return Result;
			}
		}
		
		steps++;
	}
 
	
	return vec4(projectedCoord.xy, depth, 0.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


vec3 hash(vec3 a)
{
	a = fract(a * Scale);
	a += dot(a, a.yxz + K);
	return fract((a.xxy + a.yxx)*a.zyx);
}
*/

/*
vec4 resolve(sampler2DMS tex, vec2 inUv)
{
	ivec2 attDim = textureSize(tex);
	ivec2 UV = ivec2(inUv * attDim);

	vec4 result = vec4(0.0);	   
	for (int i = 0; i < UBO.MsaaSamples; i++)
	{
		vec4 val = texelFetch(tex, UV, i); 
		result += val;
	}    
	// Average resolved samples
	return result / float(UBO.MsaaSamples);
}
*/

vec4 resolve(sampler2D tex, vec2 inUv)
{
	return texture(tex, inUv);
}




const float step = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 90;
const int numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;

float Metallic;

#define Scale vec3(.8, .8, .8)
#define K 19.19

vec3 PositionFromDepth(float depth);

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth);
 
vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth);

vec3 fresnelSchlick(float cosTheta, vec3 F0);

vec3 hash(vec3 a);

const float PI = 3.141592653589793238;
const float PI2 = 6.28318530718;

float atan2(in float y, in float x)
{
    bool s = (abs(x) > abs(y));
    return mix(PI/2.0 - atan(x,y), atan(y,x), s);
}

void main()
{
	int Mode = 5;

	switch(Mode)
	{
	case 0:
		OutColor = vec4(resolve(SamplerPosition, FragTexCoord).rgb, 1.0);
		return;
		break;
	case 1:
		OutColor = vec4(resolve(SamplerPosition, FragTexCoord).www, 1.0);//PositionFromDepth unnscessary
		return;
		break;
	case 2:
		OutColor = vec4(vec2(resolve(SamplerPBR, FragTexCoord).rg), 0.0, 1.0);
		return;
		break;
	case 3:
		OutColor = vec4(resolve(SamplerNormal, FragTexCoord).rgb, 1.0);
		return;
		break;
	case 4:
		OutColor = vec4(resolve(SamplerAlbedo, FragTexCoord).rgb, 1.0);
		return;
		break;
	default:
		break;
	}

//	OutColor = vec4(texture(DepthPosition, FragTexCoord).rgb, 1.0);
//	return;

	vec3 Color = resolve(SamplerAlbedo, FragTexCoord).rgb;

	OutColor = vec4(Color, 1.0);
//    return;

	vec2 PBR = resolve(SamplerPBR, FragTexCoord).rg;
	Metallic = PBR.r;
//	Metallic = 1.0;

	if(Metallic < 0.01)
	{
		OutColor = resolve(SamplerAlbedo, FragTexCoord);
		return;
	}
	//    discard;
 
//	vec3 viewNormal = resolve(SamplerNormal, FragTexCoord).xyz;
	vec4 NormalTex = texture(SamplerNormal, FragTexCoord);
	vec3 viewNormal = normalize(NormalTex.rgb);
//	vec3 viewPos = textureLod(DepthPosition, FragTexCoord, 2).xyz;
//	vec3 viewPos = PositionFromDepth(resolve(DepthPosition, FragTexCoord).w);
    vec3 viewPos = resolve(SamplerPosition, FragTexCoord).xyz;
	vec3 viewPosNormalized = normalize(viewPos);
	vec3 albedo = resolve(SamplerAlbedo, FragTexCoord).rgb;

	float spec = PBR.g;

	vec3 F0 = vec3(0.04); 
	F0      = mix(F0, albedo, Metallic);
	vec3 Fresnel = fresnelSchlick(max(dot(viewNormal, viewPosNormalized), 0.0), F0);

	// Reflection vector
	vec3 reflected = normalize(reflect(viewPosNormalized, viewNormal));


	vec3 hitPos = viewPos;
	float dDepth;
 
//	vec3 wp = vec3(vec4(viewPos, 1.0) * UBO.InvView);
	vec3 jitt = mix(vec3(-spec / 2), vec3(hash(albedo)), spec/ 2);
	vec4 coords = RayMarch((vec3(jitt) + reflected * max(minRayStep, -viewPos.z)), hitPos, dDepth);
 
 
	vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
 
 
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

	float ReflectionMultiplier = pow(Metallic, reflectionSpecularFalloffExponent) * 
				screenEdgefactor * 
				-reflected.z;
 
	// Get color
	const float strength = 0.6;
	vec4 SSR = vec4(resolve(SamplerAlbedo, coords.xy).rgb, clamp(ReflectionMultiplier * Fresnel * strength, 0.0, 0.9));  
	vec3 blending =  SSR.rgb * SSR.a + albedo * (1.0 - SSR.a);
	OutColor = vec4(blending, 1.0);
}

vec3 PositionFromDepth(float depth) {
	float z = depth * 2.0 - 1.0;

	vec4 clipSpacePosition = vec4(FragTexCoord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = UBO.InvProjection * clipSpacePosition;

	// Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	return viewSpacePosition.xyz;
}

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
	float depth;

	vec4 projectedCoord;
 
	for(int i = 0; i < numBinarySearchSteps; i++)
	{

		projectedCoord = UBO.Projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
		depth = resolve(SamplerPosition, projectedCoord.xy).z;

 
		dDepth = hitCoord.z - depth;

		dir *= 0.5;
		if(dDepth > 0.0)
			hitCoord += dir;
		else
			hitCoord -= dir;    
	}

		projectedCoord = UBO.Projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
	return vec3(projectedCoord.xy, depth);
}

vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{

	dir *= step;
 
 
	float depth;
	int steps;
	vec4 projectedCoord;

 
	for(int i = 0; i < maxSteps; i++)
	{
		hitCoord += dir;
 
		projectedCoord = UBO.Projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
		depth = resolve(SamplerPosition, projectedCoord.xy).z;
		if(depth > 1000.0)
			continue;
 
		dDepth = hitCoord.z - depth;

		if((dir.z - dDepth) < 1.2)
		{
			if(dDepth <= 0.0)
			{   
				vec4 Result;
				Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);

				return Result;
			}
		}
		
		steps++;
	}
 
	
	return vec4(projectedCoord.xy, depth, 1.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


vec3 hash(vec3 a)
{
	a = fract(a * Scale);
	a += dot(a, a.yxz + K);
	return fract((a.xxy + a.yxx)*a.zyx);
}







/*

const int NUM_SAMPLES = 8;
const float SAMPLE_SPACING = 3.0f;
const float SAMPLE_TOLLERANCE = SAMPLE_SPACING*5.0f;
const float SAMPLE_RANDOMISE = 0.02f;

// Method to generate a falloff coefficient for the edge of the screen
// So reflections get faded out as they come close to the edge
float edgeFalloff(vec2 uv)
{
	vec2 s = pow(abs(uv*2-1), vec2(6,4));
	return 1-max(0, min(1, s.x+s.y));
}

void main()
{
	vec3 normal = texture(SamplerNormal, FragTexCoord).xyz*2-1;


	vec3 world = texture(uTexWorld, FragTexCoord).xyz;


	vec3 eye = normalize(uCameraPosition – world);


	vec3 ray = reflect(-eye, normal);

	float fresnel = pow(1-max(0, dot(eye, normal)), 2);


	vec3 rand = texture(uTexNoise, vTexCoord*vec2(1.6,1)*4).xyz*2-1;
	float n = rand.x*0.5+1;

	vec3 col = vec3(0);
	for (int i=0; i<NUM_SAMPLES; i++)
	{
	// Pick a point on the ray
	vec3 raysamp = world + rand*SAMPLE_RANDOMISE + ray*(i+n)*SAMPLE_SPACING;

	// Put ray into screenspace
	vec4 vssamp = uProjViewMatrix * vec4(raysamp, 1);
	vssamp /= vssamp.w; // Now its between -1 and 1
	vssamp = vssamp*0.5+0.5; // Now its between 0 and 1

	// World space sample position
	vec3 p = texture(uTexWorld, vssamp.xy).xyz;

	// Colour of world space sample position
	vec3 c = texture(uTexSource, vssamp.xy).xyz * edgeFalloff(vssamp.xy);

	// Calculate the blend amount for the sample 
	float b = max(0, 0.5-length(raysamp-p)/SAMPLE_TOLLERANCE);

	// Add the colour to the output
	col += c * b;
}

// Send across the colour to the blend shader which blends it with the source colour based on the w component
// The w component consists of Fresnel and perhaps shininess or metalness etc…
oColor = vec4(col, 0.3*fresnel);
}


*/





/*
struct RayTraceOutput
{
	bool Hit;
	vec2 UV;
};

const int loops = 50;

RayTraceOutput TraceRay(vec3 reflPosition, vec3 reflDir)
{
	RayTraceOutput OutPut;

	vec3 curPos = vec3(0);
 
// The Current UV
vec3 curUV = vec3(0);
 
// The Current Length
float curLength = 1;

// Now loop
	for (int i = 0; i < loops; i++)
	{
		// Has it hit anything yet
		if (OutPut.Hit == false)
		{
			// Update the Current Position of the Ray
		   curPos = reflPosition + reflDir * curLength ;
			// Get the UV Coordinates of the current Ray
			curUV = GetUVFromPosition(curPos);
			// The Depth of the Current Pixel
			float curDepth = GetDepth(curUV .xy);
			for (int i = 0; i < SAMPLE_COUNT; i++)
			{
				if (abs(curUV .z - curDepth) < DepthCheckBias)
				{
					// If it's hit something, then return the UV position
					OutPut.Hit = true;
					OutPut.UV = curUV .xy;
					break;
				}
				curDepth = GetDepth(curUV .xy + (RAND_SAMPLES[i].xy * HalfPixel * 2));
			}

			// Get the New Position and Vector
			float3 newPos = GetWorldPosition(curUV.xy, curDepth );
			curLength = length(reflPosition - newPos);
		}
	}
	return OutPut;
}

void main()
{
	vec3 albedo = texture(LightPass, FragTexCoord).rgb;

	if(textureLod(DepthPosition, FragTexCoord, 0).xyz == vec3(0)) {
		OutColor = vec4(albedo, 1.0);
	   return;
   }

	float metallic = texture(PBRMap, FragTexCoord).r;
	float roughness = texture(NormalMap, FragTexCoord).w;
	
	if(1 - roughness < 0.1){
		OutColor = vec4(albedo, 1.0);
		return;
	}
 
	vec3 viewNormal = texture(NormalMap, FragTexCoord).xyz;
	vec3 viewPos = textureLod(DepthPosition, FragTexCoord, 2).xyz;
	//vec3 albedo = texture(mainBuffer, TexCoords).rgb;

	vec3 ViewNormalNor = normalize(viewNormal);
	vec3 ViewPosNor = normalize(viewPos);
	vec3 reflected = normalize(reflect(ViewPosNor, ViewNormalNor));
}
*/
/*
float rayStep = 0.1;
int iterationCount = 100;
float distanceBias = 0.03f;
bool enableSSR = true;
int sampleCount = 1;
bool isSamplingEnabled = false;
bool isExponentialStepEnabled = false;
bool isAdaptiveStepEnabled = true;
bool isBinarySearchEnabled = true;
bool debugDraw = false;
float samplingCoefficient = 0.001;

float random (vec2 uv) {
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453123); //simple random function
}

vec3 generatePositionFromDepth(vec2 texturePos, float depth) {
	vec4 ndc = vec4((texturePos - 0.5) * 2, depth, 1.f);
	vec4 inversed = UBO.InvProjection * ndc;// going back from projected
	inversed /= inversed.w;
	return inversed.xyz;
}

vec2 generateProjectedPosition(vec3 pos){
	vec4 samplePosition = UBO.Projection * vec4(pos, 1.f);
	samplePosition.xy = (samplePosition.xy / samplePosition.w) * 0.5 + 0.5;
	return samplePosition.xy;
}

vec3 SSR(vec3 position, vec3 reflection) {
	vec3 step = rayStep * reflection;
	vec3 marchingPosition = position + step;
	float delta;
	float depthFromScreen;
	vec2 screenPosition;
	
	int i = 0;
	for (; i < iterationCount; i++) {
		screenPosition = generateProjectedPosition(marchingPosition);
		depthFromScreen = abs(generatePositionFromDepth(screenPosition, texture(DepthPosition, screenPosition).w).z);
		delta = abs(marchingPosition.z) - depthFromScreen;
		if (abs(delta) < distanceBias) {
			vec3 color = vec3(1);
			if(debugDraw)
				color = vec3( 0.5+ sign(delta)/2,0.3,0.5- sign(delta)/2);
			return texture(LightPass, screenPosition).xyz * color;
		}
		if (isBinarySearchEnabled && delta > 0) {
			break;
		}
		if (isAdaptiveStepEnabled){
			float directionSign = sign(abs(marchingPosition.z) - depthFromScreen);
			//this is sort of adapting step, should prevent lining reflection by doing sort of iterative converging
			//some implementation doing it by binary search, but I found this idea more cheaty and way easier to implement
			step = step * (1.0 - rayStep * max(directionSign, 0.0));
			marchingPosition += step * (-directionSign);
		}
		else {
			marchingPosition += step;
		}
		if (isExponentialStepEnabled){
			step *= 1.05;
		}
	}
	if(isBinarySearchEnabled){
		for(; i < iterationCount; i++){
			
			step *= 0.5;
			marchingPosition = marchingPosition - step * sign(delta);
			
			screenPosition = generateProjectedPosition(marchingPosition);
			depthFromScreen = abs(generatePositionFromDepth(screenPosition, texture(DepthPosition, screenPosition).w).z);
			delta = abs(marchingPosition.z) - depthFromScreen;
			
			if (abs(delta) < distanceBias) {
				vec3 color = vec3(1);
				if(debugDraw)
					color = vec3( 0.5+ sign(delta)/2,0.3,0.5- sign(delta)/2);
				return texture(LightPass, screenPosition).xyz * color;
			}
		}
	}
	
	return vec3(0.0);
}

void main(){
	vec3 position = generatePositionFromDepth(FragTexCoord, texture(DepthPosition, FragTexCoord).w);
	vec4 normal = UBO.View * vec4(texture(NormalMap, FragTexCoord).xyz, 0.0);
	float metallic = texture(PBRMap, FragTexCoord).r;
	if (!enableSSR || metallic < 0.01) {
		OutColor = texture(LightPass, FragTexCoord);
	} else {
		vec3 reflectionDirection = normalize(reflect(position, normalize(normal.xyz)));
		if (isSamplingEnabled) {
			vec3 firstBasis = normalize(cross(vec3(0.f, 0.f, 1.f), reflectionDirection));
			vec3 secondBasis = normalize(cross(reflectionDirection, firstBasis));
			vec4 resultingColor = vec4(0.f);
			for (int i = 0; i < sampleCount; i++) {
				vec2 coeffs = vec2(random(FragTexCoord + vec2(0, i)) + random(FragTexCoord + vec2(i, 0))) * samplingCoefficient;
				vec3 reflectionDirectionRandomized = reflectionDirection + firstBasis * coeffs.x + secondBasis * coeffs.y;
				vec3 tempColor = SSR(position, normalize(reflectionDirectionRandomized));
				if (tempColor != vec3(0.f)) {
					resultingColor += vec4(tempColor, 1.f);
				}
			}
			if (resultingColor.w == 0){
				OutColor = texture(LightPass, FragTexCoord);
			} else {
				resultingColor /= resultingColor.w;
				OutColor = vec4(resultingColor.xyz, 1.f);
			}
		}
		else {
			OutColor = vec4(SSR(position, normalize(reflectionDirection)), 1.f);
			if (OutColor.xyz == vec3(0.f)){
				OutColor = texture(LightPass, FragTexCoord);
			}
		}
	}
}
*/