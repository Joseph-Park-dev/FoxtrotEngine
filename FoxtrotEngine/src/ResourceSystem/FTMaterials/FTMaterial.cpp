#include "FTMaterial.h"

#include "FileSystem/FileIOHelper.h"
#include "FileSystem/BufferSizes.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"
#include "ResourceSystem/FTBasicMeshGroup.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
#endif // FOXTROT_EDITOR

#ifdef FOXTROT_EDITOR
// FTVector3& FTMaterial::GetAmbientRef() { return mAmbient; }
// float& FTMaterial::GetShininessRef() { return mShininess; }
// FTVector3& FTMaterial::GetDiffuseRef() { return mDiffuse; }
// FTVector3& FTMaterial::GetSpecularRef() { return mSpecular; }
#endif
