// ----------------------------------------------------------------
// Foxtrot Engine
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

float3 RimEffect(float3 normalWorld, float3 toEye, float rimPower, float rimStrength, float3 rimColor)
{
    float rim = (1.0 - dot(normalWorld, toEye));
    
    rim = pow(abs(rim), rimPower);
    
    return (rim * rimStrength * rimColor);
}