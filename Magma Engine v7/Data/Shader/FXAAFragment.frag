#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 FragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D SamplerTexture;

layout(location = 0) out vec4 OutColor;

vec2 u_texelStep;
int u_showEdges = 1;
int u_fxaaOn = 1;

float u_lumaThreshold = 0.25;
float u_mulReduce = 1.0 / 8.0;
float u_minReduce = 1.0 / 128.0;
float u_maxSpan = 8.0;

float Avg3(vec3 v)
{
	return (v.x + v.y + v.z) / 3.0;
}

// see FXAA
// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
// http://iryoku.com/aacourse/downloads/09-FXAA-3.11-in-15-Slides.pdf
// http://horde3d.org/wiki/index.php5?title=Shading_Technique_-_FXAA
// https://github.com/McNopper/OpenGL/blob/master/Example42/shader/fxaa.frag.glsl
void main()
{
	u_texelStep = textureSize(SamplerTexture, 0);
	u_texelStep = vec2(1.0) / u_texelStep;
    vec3 rgbM = texture(SamplerTexture, FragTexCoord).rgb;
	OutColor = vec4(rgbM, 1.0);

	// Possibility to toggle FXAA on and off.
	if (u_fxaaOn == 0)
	{
		OutColor = vec4(rgbM, 1.0);
		
		return;
	}

	// Sampling neighbour texels. Offsets are adapted to OpenGL texture coordinates. 
	vec3 rgbNW = textureOffset(SamplerTexture, FragTexCoord, ivec2(-1, 1)).rgb;
    vec3 rgbNE = textureOffset(SamplerTexture, FragTexCoord, ivec2(1, 1)).rgb;
    vec3 rgbSW = textureOffset(SamplerTexture, FragTexCoord, ivec2(-1, -1)).rgb;
    vec3 rgbSE = textureOffset(SamplerTexture, FragTexCoord, ivec2(1, -1)).rgb;

	// see http://en.wikipedia.org/wiki/Grayscale
	//const vec3 toLuma = vec3(0.299, 0.587, 0.114);

	const vec3 toLuma = vec3(0.299, 0.587, 0.114);
	const vec3 toLuma1 = vec3(0.216, 0.7152, 0.0722);
	const vec3 toLuma2 = vec3(0.2627, 0.6780, 0.0593);

	vec3 lumaNW3 = (rgbNW * toLuma1);
	vec3 lumaNE3 = (rgbNE * toLuma1);
	vec3 lumaSW3 = (rgbSW * toLuma1);
	vec3 lumaSE3 = (rgbSE * toLuma1);
	vec3 lumaM3 =  (rgbM  * toLuma1);

	vec3 lumaMin3 = min(lumaM3, min(min(lumaNW3, lumaNE3), min(lumaSW3, lumaSE3)));
	vec3 lumaMax3 = max(lumaM3, max(max(lumaNW3, lumaNE3), max(lumaSW3, lumaSE3)));
	
	// If contrast is lower than a maximum threshold ...
	if (lumaMax3.x - lumaMin3.x <= lumaMax3.x * u_lumaThreshold &&
		lumaMax3.y - lumaMin3.y <= lumaMax3.y * u_lumaThreshold &&
		lumaMax3.z - lumaMin3.z <= lumaMax3.z * u_lumaThreshold)
	{
		// ... do no AA and return.
		OutColor = vec4(rgbM, 1.0);
		
		return;
	}  
	/*

	vec2 samplingDirection;	
	samplingDirection.x = Avg3(-((lumaNW3 + lumaNE3) - (lumaSW3 + lumaSE3)));
    samplingDirection.y = Avg3( ((lumaNW3 + lumaSW3) - (lumaNE3 + lumaSE3)));
    
    // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
    // This results, that brighter areas are less blurred/more sharper than dark areas.  
    float samplingDirectionReduce = Avg3(max((lumaNW3 + lumaNE3 + lumaSW3 + lumaSE3) * 0.25 * u_mulReduce, u_minReduce));

	// Factor for norming the sampling direction plus adding the brightness influence. 
	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);
    
    // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
    samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-u_maxSpan), vec2(u_maxSpan)) * u_texelStep;
	
	// Inner samples on the tab.
	vec3 rgbSampleNeg = texture(SamplerTexture, FragTexCoord + samplingDirection * (1.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePos = texture(SamplerTexture, FragTexCoord + samplingDirection * (2.0/3.0 - 0.5)).rgb;

	vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;  

	// Outer samples on the tab.
	vec3 rgbSampleNegOuter = texture(SamplerTexture, FragTexCoord + samplingDirection * (0.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePosOuter = texture(SamplerTexture, FragTexCoord + samplingDirection * (3.0/3.0 - 0.5)).rgb;
	
	vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;   
	
	// Calculate luma for checking against the minimum and maximum value.
	

	
	float lumaMin = Avg3(min(lumaM3, min(min(lumaNW3, lumaNE3), min(lumaSW3, lumaSE3))));
	float lumaMax = Avg3(max(lumaM3, max(max(lumaNW3, lumaNE3), max(lumaSW3, lumaSE3))));

	float lumaFourTab = dot(rgbFourTab, toLuma1);
	// Are outer samples of the tab beyond the edge ... 
	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax)

//	vec3 lumaFourTab = (rgbFourTab * toLuma2);
//	if (lumaFourTab.x < lumaMax3.x || lumaFourTab.x > lumaMax3.x&&
//		lumaFourTab.y < lumaMax3.y || lumaFourTab.y > lumaMax3.y&&
//		lumaFourTab.z < lumaMax3.z || lumaFourTab.z > lumaMax3.z)
	{
		// ... yes, so use only two samples.
		OutColor = vec4(rgbTwoTab, 1.0); 
	}
	else
	{
		// ... no, so use four samples. 
		OutColor = vec4(rgbFourTab, 1.0);
	}
	*/
	
	float lumaNW = dot(rgbNW, toLuma1);
	float lumaNE = dot(rgbNE, toLuma1);
	float lumaSW = dot(rgbSW, toLuma1);
	float lumaSE = dot(rgbSE, toLuma1);
	float lumaM = dot(rgbM, toLuma1);

	// Sampling is done along the gradient.
	vec2 samplingDirection;	
	samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    samplingDirection.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
    // This results, that brighter areas are less blurred/more sharper than dark areas.  
    float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * u_mulReduce, u_minReduce);

	// Factor for norming the sampling direction plus adding the brightness influence. 
	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);
    
    // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
    samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-u_maxSpan), vec2(u_maxSpan)) * u_texelStep;
	
	// Inner samples on the tab.
	vec3 rgbSampleNeg = texture(SamplerTexture, FragTexCoord + samplingDirection * (1.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePos = texture(SamplerTexture, FragTexCoord + samplingDirection * (2.0/3.0 - 0.5)).rgb;

	vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;  

	// Outer samples on the tab.
	vec3 rgbSampleNegOuter = texture(SamplerTexture, FragTexCoord + samplingDirection * (0.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePosOuter = texture(SamplerTexture, FragTexCoord + samplingDirection * (3.0/3.0 - 0.5)).rgb;
	
	vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;   
	
	// Calculate luma for checking against the minimum and maximum value.
	

	
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	float lumaFourTab = dot(rgbFourTab, toLuma1);
	// Are outer samples of the tab beyond the edge ... 
	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax)

//	vec3 lumaFourTab = (rgbFourTab * toLuma2);
//	if (lumaFourTab.x < lumaMax3.x || lumaFourTab.x > lumaMax3.x&&
//		lumaFourTab.y < lumaMax3.y || lumaFourTab.y > lumaMax3.y&&
//		lumaFourTab.z < lumaMax3.z || lumaFourTab.z > lumaMax3.z)
	{
		// ... yes, so use only two samples.
		OutColor = vec4(rgbTwoTab, 1.0); 
	}
	else
	{
		// ... no, so use four samples. 
		OutColor = vec4(rgbFourTab, 1.0);
	}
	
//	OutColor = 1.0 - OutColor;

//	OutColor.r = 1.0;
}


/*
void main()
{
	u_texelStep = textureSize(SamplerTexture, 0);
	u_texelStep = vec2(1.0) / u_texelStep;
    vec3 rgbM = texture(SamplerTexture, FragTexCoord).rgb;

	// Possibility to toggle FXAA on and off.
	if (u_fxaaOn == 0)
	{
		OutColor = vec4(rgbM, 1.0);
		
		return;
	}

	// Sampling neighbour texels. Offsets are adapted to OpenGL texture coordinates. 
	vec3 rgbNW = textureOffset(SamplerTexture, FragTexCoord, ivec2(-1, 1)).rgb;
    vec3 rgbNE = textureOffset(SamplerTexture, FragTexCoord, ivec2(1, 1)).rgb;
    vec3 rgbSW = textureOffset(SamplerTexture, FragTexCoord, ivec2(-1, -1)).rgb;
    vec3 rgbSE = textureOffset(SamplerTexture, FragTexCoord, ivec2(1, -1)).rgb;

	// see http://en.wikipedia.org/wiki/Grayscale
	//const vec3 toLuma = vec3(0.299, 0.587, 0.114);

	const vec3 toLuma = vec3(0.299, 0.587, 0.114);
	const vec3 toLuma1 = vec3(0.216, 0.7152, 0.0722);
	const vec3 toLuma2 = vec3(0.2627, 0.6780, 0.0593);

	// Convert from RGB to luma.
	float lumaNW = dot(rgbNW, toLuma1);
	float lumaNE = dot(rgbNE, toLuma1);
	float lumaSW = dot(rgbSW, toLuma1);
	float lumaSE = dot(rgbSE, toLuma1);
	float lumaM = dot(rgbM, toLuma);

	// Gather minimum and maximum luma.
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
	
	// If contrast is lower than a maximum threshold ...
	if (lumaMax - lumaMin <= lumaMax * u_lumaThreshold)
	{
		// ... do no AA and return.
		OutColor = vec4(rgbM, 1.0);
		
		return;
	}  
	
	// Sampling is done along the gradient.
	vec2 samplingDirection;	
	samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    samplingDirection.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
    // This results, that brighter areas are less blurred/more sharper than dark areas.  
    float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * u_mulReduce, u_minReduce);

	// Factor for norming the sampling direction plus adding the brightness influence. 
	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);
    
    // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
    samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-u_maxSpan), vec2(u_maxSpan)) * u_texelStep;
	
	// Inner samples on the tab.
	vec3 rgbSampleNeg = texture(SamplerTexture, FragTexCoord + samplingDirection * (1.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePos = texture(SamplerTexture, FragTexCoord + samplingDirection * (2.0/3.0 - 0.5)).rgb;

	vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;  

	// Outer samples on the tab.
	vec3 rgbSampleNegOuter = texture(SamplerTexture, FragTexCoord + samplingDirection * (0.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePosOuter = texture(SamplerTexture, FragTexCoord + samplingDirection * (3.0/3.0 - 0.5)).rgb;
	
	vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;   
	
	// Calculate luma for checking against the minimum and maximum value.
	float lumaFourTab = dot(rgbFourTab, toLuma);
	
	// Are outer samples of the tab beyond the edge ... 
	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax)
	{
		// ... yes, so use only two samples.
		OutColor = vec4(rgbTwoTab, 1.0); 
	}
	else
	{
		// ... no, so use four samples. 
		OutColor = vec4(rgbFourTab, 1.0);
	}

	// Show edges for debug purposes.	
	if (u_showEdges != 0)
	{
		OutColor.r = 1.0;
	}
}
*/