#include "stdafx.h"
#include "SpikeyMaterial.h"

SpikeyMaterial::SpikeyMaterial() :
	Material<SpikeyMaterial>(L"Effects/SpikeyShader.fx") {}

void SpikeyMaterial::InitializeEffectVariables()
{
}
