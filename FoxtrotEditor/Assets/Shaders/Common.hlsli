// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#define MAX_LIGHTS 3 
#define NUM_DIR_LIGHTS 1
#define NUM_POINT_LIGHTS 1
#define NUM_SPOT_LIGHTS 1

struct BlinnPhongData
{
    float3 ambient;
    float shininess;
    float3 diffuse;
    float dummy1;
    float3 specular;
    float dummy2;
};

struct Light
{
    float3 strength;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
};

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal,
                   float3 toEye, BlinnPhongData mat)
{
    float3 halfway = normalize(toEye + lightVec);
    float3 specular =
        mat.specular * pow(max(dot(halfway, normal), 0.0f), mat.shininess);

    return mat.ambient + (mat.diffuse + specular) * lightStrength;
}

float3 ComputeDirectionalLight(Light L, BlinnPhongData mat, float3 normal,
                                float3 toEye)
{
    float3 lightVec = -L.direction;
    
    float ndotl = max(dot(lightVec, normal), 0.0);
    float3 lightStrength = L.strength * ndotl;
    
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float CalcAttenuation(float dist, float falloffStart, float falloffEnd)
{
    return saturate((falloffEnd - dist) / (falloffEnd - falloffStart));
}

float3 ComputePointLight(Light L, BlinnPhongData mat, float3 pos, float3 normal,
                          float3 toEye)
{
    float3 lightVec = L.position - pos;

    float dist = length(lightVec);

    // Lighting is not applied it its too far away.
    if (dist > L.fallOffEnd)
    {
        return float3(0.0, 0.0, 0.0);
    }
    else
    {
        lightVec /= dist;
        float ndotl = max(dot(lightVec, normal), 0.0);
        float3 lightStrength = L.strength * ndotl;
        
        float att = CalcAttenuation(dist, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;

        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }
}

float3 ComputeSpotLight(Light L, BlinnPhongData mat, float3 pos, float3 normal,
                         float3 toEye)
{
    float3 lightVec = L.position - pos;

    float dist = length(lightVec);

    // Lighting is not applied it its too far away.
    if (dist > L.fallOffEnd)
    {
        return float3(0.0, 0.0, 0.0);
    }
    else
    {
        lightVec /= dist;
        float ndotl = max(dot(lightVec, normal), 0.0);
        float3 lightStrength = L.strength * ndotl;
        
        float att = CalcAttenuation(dist, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;
        
        float spotfactor = pow(max(dot(-lightVec, L.direction), 0.0), L.spotPower);
        lightStrength *= spotfactor;

        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }
}

struct SolidVSInput
{
    float3 posModel : POSITION;
    float3 normalModel : NORMAL;
    float4 color : COLOR;
};

struct SolidPSInput
{
    float4 posProj : SV_POSITION;
    float3 posWorld : POSITION;
    float3 normalWorld : NORMAL;
    float4 color : COLOR;
};

struct TexVSInput
{
    float3 posModel : POSITION;
    float3 normalModel : NORMAL;
    float2 texcoord : TEXCOORD0;
};

struct TexPSInput
{
    float4 posProj : SV_POSITION;
    float3 posWorld : POSITION;
    float3 normalWorld : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct IndexPSOutput
{
    float4 pixelColor : SV_Target0;
    float4 indexColor : SV_Target1;
};

struct DebugVSInput
{
    float3 posModel : POSITION0;
    float3 color : COLOR0;
};

struct DebugPSInput
{
    float4 pos      : SV_POSITION;
    float4 color    : COLOR;
};
