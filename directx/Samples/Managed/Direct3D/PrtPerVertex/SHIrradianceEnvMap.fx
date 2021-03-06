//-----------------------------------------------------------------------------
// File: SHIrradianceEnvMap.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4x4 worldViewProjection;
texture  AlbedoTexture;

float4 MaterialDiffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };    

float4 ar;
float4 ag;
float4 ab;
float4 br;
float4 bg;
float4 bb;
float4 c;


//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler AlbedoSampler = 
sampler_state
{
    Texture = <AlbedoTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position  : POSITION;    // position of the vertex
    float4 Diffuse   : COLOR0;      // diffuse color of the vertex
    float2 TextureUV : TEXCOORD0;   // typical texture coords stored here
};


//-----------------------------------------------------------------------------
VS_OUTPUT SHIrradianceEnvironmentMapVS( float4 vPos : POSITION, 
                                        float4 vNormal : NORMAL,
                                        float2 vTexCoord0 : TEXCOORD0,
                                        uniform bool bTexture )
{
    VS_OUTPUT Output;
    
    // Output the vetrex position in projection space
    Output.Position = mul(vPos, worldViewProjection);
    
    float3 x1, x2, x3;
    
    // Linear + constant polynomial terms
    x1.r = dot(ar,vNormal);
    x1.g = dot(ag,vNormal);
    x1.b = dot(ab,vNormal);
    
    // 4 of the quadratic polynomials
    float4 vB = vNormal.xyzz * vNormal.yzzx;   
    x2.r = dot(br,vB);
    x2.g = dot(bg,vB);
    x2.b = dot(bb,vB);
   
    // Final quadratic polynomial
    float vC = vNormal.x*vNormal.x - vNormal.y*vNormal.y;
    x3 = c.rgb * vC;    

    Output.Diffuse.rgb = x1 + x2 + x3;
    Output.Diffuse.a   = 1.0f; 
    
    Output.Diffuse *= MaterialDiffuseColor;
    
	if( bTexture )
		Output.TextureUV = vTexCoord0;
	else
		Output.TextureUV = 0;
    
    return Output;
}


//-----------------------------------------------------------------------------
// Pixel shader output structure
//-----------------------------------------------------------------------------
struct PS_OUTPUT
{   
    float4 RGBColor : COLOR0;  // Pixel color    
};


//-----------------------------------------------------------------------------
// Trival pixel shader (could use FF if desired)
//-----------------------------------------------------------------------------
PS_OUTPUT StandardPS( VS_OUTPUT In,
                      uniform bool bTexture ) 
{ 
    PS_OUTPUT Output;

	if( bTexture )
		Output.RGBColor = tex2D(AlbedoSampler, In.TextureUV) * In.Diffuse;
	else
		Output.RGBColor = In.Diffuse;

    return Output;
}


//-----------------------------------------------------------------------------
technique RenderWithSHIrradEnvMap
{
    pass P0
    {          
        VertexShader = compile vs_1_1 SHIrradianceEnvironmentMapVS( true );
        PixelShader  = compile ps_1_1 StandardPS( true ); 
    }
}


//-----------------------------------------------------------------------------
technique RenderWithSHIrradEnvMapNoAlbedo
{
    pass P0
    {          
        VertexShader = compile vs_1_1 SHIrradianceEnvironmentMapVS( false );
        PixelShader  = compile ps_1_1 StandardPS( false );
    }
}




