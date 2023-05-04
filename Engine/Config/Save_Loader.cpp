#include "..\Public\Save_Loader.h"

IMPLEMENT_SINGLETON(CSave_Loader)

CSave_Loader::CSave_Loader()
{
	m_strDirPath = L"../../Resource/Effect/";
}

HRESULT CSave_Loader::Load_Effect(HWND hWnd, wstring strFileName, list<EFFECT_DESC*>* pEffectDesc)
{
	if (!pEffectDesc)
	{
		MSG_BOX("Failed to Save : Save EffectList nullptr");
		return E_FAIL;
	}


	wstring strFullPath = /*m_strDirPath +*/ strFileName;

	HANDLE hFile = CreateFileW(
		strFullPath.c_str(),
		GENERIC_READ,
		0, NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		wstring msg = strFileName + L" 파일이 없습니다.";
		MessageBox(hWnd, msg.c_str(), L"Mesh_bin", MB_OK);
		return E_FAIL;
	}
	char buf[100];
	ZeroMemory(&buf, sizeof(buf));

	DWORD dwRead = 0;

	int FileSize;
	char cntBuf[4];
	ZeroMemory(&cntBuf, sizeof(cntBuf));
	ReadFile(hFile, cntBuf, sizeof(int), &dwRead, NULL);
	FileSize = static_cast<int>(*cntBuf);

	//
	EFFECT_DESC* pEffect = nullptr;
	for (int i = 0; i < FileSize; i++)
	{
		pEffect = new EFFECT_DESC;
		ZeroMemory(pEffect, sizeof(EFFECT_DESC));

		ReadFile(hFile, &pEffect->eEffectType, sizeof(_int), &dwRead, NULL);

		ReadFile(hFile, &pEffect->fLifeTime, sizeof(_float), &dwRead, NULL);
		ReadFile(hFile, &pEffect->fEffectTime, sizeof(_float), &dwRead, NULL);

		ReadFile(hFile, &pEffect->bSprite, sizeof(_bool), &dwRead, NULL);
		ReadFile(hFile, &pEffect->bSetColor, sizeof(_bool), &dwRead, NULL);

		ReadFile(hFile, &pEffect->bDelay, sizeof(_bool), &dwRead, NULL);
		ReadFile(hFile, &pEffect->fDelayTime, sizeof(_float), &dwRead, NULL);

		ReadFile(hFile, &pEffect->fStartAlpha, sizeof(_float), &dwRead, NULL);
		ReadFile(hFile, &pEffect->fCurAlpha, sizeof(_float), &dwRead, NULL);
		ReadFile(hFile, &pEffect->fEndAlpha, sizeof(_float), &dwRead, NULL);

		ReadFile(hFile, &pEffect->vStartColor, sizeof(_float3), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vTextrueColor, sizeof(_float3), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vEndColor, sizeof(_float3), &dwRead, NULL);

		ReadFile(hFile, &pEffect->vUV, sizeof(_float2), &dwRead, NULL);
		ReadFile(hFile, &pEffect->fUVSpeed, sizeof(_float2), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vFrame, sizeof(_float2), &dwRead, NULL);
		ReadFile(hFile, &pEffect->fFrameSpeed, sizeof(_float), &dwRead, NULL);

		ReadFile(hFile, &pEffect->bRhombus, sizeof(_bool), &dwRead, NULL);
		ReadFile(hFile, &pEffect->bRect, sizeof(_bool), &dwRead, NULL);
		ReadFile(hFile, &pEffect->bTriangle, sizeof(_bool), &dwRead, NULL);

		ReadFile(hFile, &pEffect->bGlow, sizeof(_bool), &dwRead, NULL);
		ReadFile(hFile, &pEffect->iRenderGroup, sizeof(_int), &dwRead, NULL);
		ReadFile(hFile, &pEffect->iPass, sizeof(_int), &dwRead, NULL);

		ReadFile(hFile, &pEffect->bLoop, sizeof(_bool), &dwRead, NULL);
		ReadFile(hFile, &pEffect->bTracking, sizeof(_bool), &dwRead, NULL);

		//Particle

		ReadFile(hFile, &pEffect->bSpark, sizeof(_bool), &dwRead, NULL);
		ReadFile(hFile, &pEffect->bCircle, sizeof(_bool), &dwRead, NULL);

		ReadFile(hFile, &pEffect->bParticleDelay, sizeof(_bool), &dwRead, NULL);
		ReadFile(hFile, &pEffect->fParticleDelayTime, sizeof(_float), &dwRead, NULL);
		ReadFile(hFile, &pEffect->iParticleDelay, sizeof(_int), &dwRead, NULL);

		ReadFile(hFile, &pEffect->vDir, sizeof(_float3), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vDir_Angle, sizeof(_float2), &dwRead, NULL);

		ReadFile(hFile, &pEffect->fMinSpeed, sizeof(_float), &dwRead, NULL);
		ReadFile(hFile, &pEffect->fMaxSpeed, sizeof(_float), &dwRead, NULL);

		ReadFile(hFile, &pEffect->vMinAngle, sizeof(_float3), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vMaxAngle, sizeof(_float3), &dwRead, NULL);

		ReadFile(hFile, &pEffect->fMinDist, sizeof(_float), &dwRead, NULL);
		ReadFile(hFile, &pEffect->fMaxDist, sizeof(_float), &dwRead, NULL);

		ReadFile(hFile, &pEffect->vMinScale, sizeof(_float3), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vMaxScale, sizeof(_float3), &dwRead, NULL);

		ReadFile(hFile, &pEffect->vMinPosition, sizeof(_float3), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vMaxPosition, sizeof(_float3), &dwRead, NULL);

		ReadFile(hFile, &pEffect->iNumInstance, sizeof(_int), &dwRead, NULL);

		ReadFile(hFile, &pEffect->fParticlePower, sizeof(_float), &dwRead, NULL);
		ReadFile(hFile, &pEffect->fGravityPower, sizeof(_float), &dwRead, NULL);

		ReadFile(hFile, &pEffect->bGravity, sizeof(_bool), &dwRead, NULL);
		ReadFile(hFile, &pEffect->bParticleLoop, sizeof(_bool), &dwRead, NULL);

		ReadFile(hFile, &pEffect->eFigure, sizeof(_int), &dwRead, NULL);

		//Mesh

		ReadFile(hFile, &pEffect->vStartPosition, sizeof(_float3), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vCurPosition, sizeof(_float3), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vEndPosition, sizeof(_float3), &dwRead, NULL);

		ReadFile(hFile, &pEffect->vStartScale, sizeof(_float3), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vCurScale, sizeof(_float3), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vEndScale, sizeof(_float3), &dwRead, NULL);

		ReadFile(hFile, &pEffect->vStartAngle, sizeof(_float3), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vCurAngle, sizeof(_float3), &dwRead, NULL);
		ReadFile(hFile, &pEffect->vEndAngle, sizeof(_float3), &dwRead, NULL);

		ReadFile(hFile, &pEffect->fStartDelay, sizeof(_float), &dwRead, NULL);

		_int iSize = 0;
		ReadFile(hFile, &iSize, sizeof(_int), &dwRead, NULL);
		ReadFile(hFile, pEffect->MeshName, sizeof(char) * iSize, &dwRead, NULL);

		ReadFile(hFile, &pEffect->bTexture, sizeof(_bool), &dwRead, NULL);
		ReadFile(hFile, &pEffect->bNoiseTexure, sizeof(_bool), &dwRead, NULL);
		ReadFile(hFile, &pEffect->bDissolveTexure, sizeof(_bool), &dwRead, NULL);

		ReadFile(hFile, &iSize, sizeof(_int), &dwRead, NULL);
		ReadFile(hFile, pEffect->TexName, sizeof(char) * iSize, &dwRead, NULL);

		ReadFile(hFile, &iSize, sizeof(_int), &dwRead, NULL);
		ReadFile(hFile, pEffect->NoiseTexName, sizeof(char) * iSize, &dwRead, NULL);

		ReadFile(hFile, &iSize, sizeof(_int), &dwRead, NULL);
		ReadFile(hFile, pEffect->DissolveTexName, sizeof(char) * iSize, &dwRead, NULL);

		pEffectDesc->push_back(pEffect);
	}

	CloseHandle(hFile);

	return S_OK;
}

void CSave_Loader::Free()
{
}