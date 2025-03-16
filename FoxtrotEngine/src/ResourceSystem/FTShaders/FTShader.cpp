#include "FTShader.h"

void FTShader::CompileShader(FoxtrotRenderer* renderer)
{
}

const ShaderType& FTShader::GetType() const { return mType; }
void FTShader::SetType(ShaderType type) { mType = type; }

FTShader::FTShader()
{
}

#ifdef FOXTROT_EDITOR
FoxtrotRenderer* FTShader::GetRenderer()
{
	return mRenderer;
}

FTShader::FTShader(FoxtrotRenderer* renderer)
	: mRenderer(renderer)
{
}
#endif // FOXTROT_EDITOR