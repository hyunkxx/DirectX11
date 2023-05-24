#include "..\Public\Save_Loader.h"

IMPLEMENT_SINGLETON(CSave_Loader)

CSave_Loader::CSave_Loader()
{
	m_strDirPath = L"../../Resource/Tool_Effect/";
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

HRESULT CSave_Loader::Save_File(HWND hWnd, list<EFFECT_DESC*>* pEffectDesc, const char * pTag)
{
	if (!pEffectDesc)
	{
		MSG_BOX("Failed to Save : Save EffectList nullptr");
		return E_FAIL;
	}

	DWORD dwRead = 0;
	OPENFILENAME OFN;

	char CfilePathName[MAX_PATH] = "";
	_tchar filePathName[MAX_PATH] = L"";
	_tchar lpstrFile[MAX_PATH] = L"";
	_tchar lpstrFile2[MAX_PATH] = L"";
	static _tchar filter[] = L"Binary file\0*.bin";

	char			szFileName[MAX_PATH] = "";
	char			szExt[MAX_PATH] = "";
	_splitpath_s(pTag, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	MultiByteToWideChar(CP_ACP, 0, szFileName, (_int)strlen(szFileName),
		lpstrFile2, MAX_PATH);

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 100;
	OFN.lpstrInitialDir = m_strDirPath.c_str();

	wstring strFileEnd = L".bin";
	wstring fileName = m_strDirPath + lpstrFile2 + strFileEnd;
	HANDLE hFile = CreateFile(
		fileName.c_str(),
		GENERIC_WRITE,
		0, NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("failed hObject open");
		return E_FAIL;
	}
	_int iFileSize = (_int)pEffectDesc->size();
	WriteFile(hFile, &iFileSize, sizeof(_int), &dwRead, NULL);

	for (auto& iter : *pEffectDesc)
	{
		WriteFile(hFile, &iter->eEffectType, sizeof(_int), &dwRead, NULL);

		WriteFile(hFile, &iter->fLifeTime, sizeof(_float), &dwRead, NULL);
		WriteFile(hFile, &iter->fEffectTime, sizeof(_float), &dwRead, NULL);

		WriteFile(hFile, &iter->bSprite, sizeof(_bool), &dwRead, NULL);
		WriteFile(hFile, &iter->bSetColor, sizeof(_bool), &dwRead, NULL);

		WriteFile(hFile, &iter->bDelay, sizeof(_bool), &dwRead, NULL);
		WriteFile(hFile, &iter->fDelayTime, sizeof(_float), &dwRead, NULL);

		WriteFile(hFile, &iter->fStartAlpha, sizeof(_float), &dwRead, NULL);
		WriteFile(hFile, &iter->fCurAlpha, sizeof(_float), &dwRead, NULL);
		WriteFile(hFile, &iter->fEndAlpha, sizeof(_float), &dwRead, NULL);

		WriteFile(hFile, &iter->vStartColor, sizeof(_float3), &dwRead, NULL);
		WriteFile(hFile, &iter->vTextrueColor, sizeof(_float3), &dwRead, NULL);
		WriteFile(hFile, &iter->vEndColor, sizeof(_float3), &dwRead, NULL);

		WriteFile(hFile, &iter->vUV, sizeof(_float2), &dwRead, NULL);
		WriteFile(hFile, &iter->fUVSpeed, sizeof(_float2), &dwRead, NULL);
		WriteFile(hFile, &iter->vFrame, sizeof(_float2), &dwRead, NULL);
		WriteFile(hFile, &iter->fFrameSpeed, sizeof(_float), &dwRead, NULL);

		WriteFile(hFile, &iter->bRhombus, sizeof(_bool), &dwRead, NULL);
		WriteFile(hFile, &iter->bRect, sizeof(_bool), &dwRead, NULL);
		WriteFile(hFile, &iter->bTriangle, sizeof(_bool), &dwRead, NULL);

		WriteFile(hFile, &iter->bGlow, sizeof(_bool), &dwRead, NULL);
		WriteFile(hFile, &iter->iRenderGroup, sizeof(_int), &dwRead, NULL);
		WriteFile(hFile, &iter->iPass, sizeof(_int), &dwRead, NULL);

		WriteFile(hFile, &iter->bLoop, sizeof(_bool), &dwRead, NULL);
		WriteFile(hFile, &iter->bTracking, sizeof(_bool), &dwRead, NULL);

		//Particle

		WriteFile(hFile, &iter->bSpark, sizeof(_bool), &dwRead, NULL);
		WriteFile(hFile, &iter->bCircle, sizeof(_bool), &dwRead, NULL);

		WriteFile(hFile, &iter->bParticleDelay, sizeof(_bool), &dwRead, NULL);
		WriteFile(hFile, &iter->fParticleDelayTime, sizeof(_float), &dwRead, NULL);
		WriteFile(hFile, &iter->iParticleDelay, sizeof(_int), &dwRead, NULL);

		WriteFile(hFile, &iter->vDir, sizeof(_float3), &dwRead, NULL);
		WriteFile(hFile, &iter->vDir_Angle, sizeof(_float2), &dwRead, NULL);

		WriteFile(hFile, &iter->fMinSpeed, sizeof(_float), &dwRead, NULL);
		WriteFile(hFile, &iter->fMaxSpeed, sizeof(_float), &dwRead, NULL);

		WriteFile(hFile, &iter->vMinAngle, sizeof(_float3), &dwRead, NULL);
		WriteFile(hFile, &iter->vMaxAngle, sizeof(_float3), &dwRead, NULL);

		WriteFile(hFile, &iter->fMinDist, sizeof(_float), &dwRead, NULL);
		WriteFile(hFile, &iter->fMaxDist, sizeof(_float), &dwRead, NULL);

		WriteFile(hFile, &iter->vMinScale, sizeof(_float3), &dwRead, NULL);
		WriteFile(hFile, &iter->vMaxScale, sizeof(_float3), &dwRead, NULL);

		WriteFile(hFile, &iter->vMinPosition, sizeof(_float3), &dwRead, NULL);
		WriteFile(hFile, &iter->vMaxPosition, sizeof(_float3), &dwRead, NULL);

		WriteFile(hFile, &iter->iNumInstance, sizeof(_int), &dwRead, NULL);

		WriteFile(hFile, &iter->fParticlePower, sizeof(_float), &dwRead, NULL);
		WriteFile(hFile, &iter->fGravityPower, sizeof(_float), &dwRead, NULL);

		WriteFile(hFile, &iter->bGravity, sizeof(_bool), &dwRead, NULL);
		WriteFile(hFile, &iter->bParticleLoop, sizeof(_bool), &dwRead, NULL);

		WriteFile(hFile, &iter->eFigure, sizeof(_int), &dwRead, NULL);

		//Mesh

		WriteFile(hFile, &iter->vStartPosition, sizeof(_float3), &dwRead, NULL);
		WriteFile(hFile, &iter->vCurPosition, sizeof(_float3), &dwRead, NULL);
		WriteFile(hFile, &iter->vEndPosition, sizeof(_float3), &dwRead, NULL);

		WriteFile(hFile, &iter->vStartScale, sizeof(_float3), &dwRead, NULL);
		WriteFile(hFile, &iter->vCurScale, sizeof(_float3), &dwRead, NULL);
		WriteFile(hFile, &iter->vEndScale, sizeof(_float3), &dwRead, NULL);

		WriteFile(hFile, &iter->vStartAngle, sizeof(_float3), &dwRead, NULL);
		WriteFile(hFile, &iter->vCurAngle, sizeof(_float3), &dwRead, NULL);
		WriteFile(hFile, &iter->vEndAngle, sizeof(_float3), &dwRead, NULL);

		WriteFile(hFile, &iter->fStartDelay, sizeof(_float), &dwRead, NULL);

		_int iSize = (_int)strnlen_s(iter->MeshName, sizeof(char)*MAX_PATH);
		WriteFile(hFile, &iSize, sizeof(_int), &dwRead, NULL);
		WriteFile(hFile, iter->MeshName, sizeof(char) * iSize, &dwRead, NULL);

		WriteFile(hFile, &iter->bTexture, sizeof(_bool), &dwRead, NULL);
		WriteFile(hFile, &iter->bNoiseTexure, sizeof(_bool), &dwRead, NULL);
		WriteFile(hFile, &iter->bDissolveTexure, sizeof(_bool), &dwRead, NULL);

		iSize = (_int)strnlen_s(iter->TexName, sizeof(char)*MAX_PATH);
		WriteFile(hFile, &iSize, sizeof(_int), &dwRead, NULL);
		WriteFile(hFile, iter->TexName, sizeof(char) * iSize, &dwRead, NULL);

		iSize = (_int)strnlen_s(iter->NoiseTexName, sizeof(char)*MAX_PATH);
		WriteFile(hFile, &iSize, sizeof(_int), &dwRead, NULL);
		WriteFile(hFile, iter->NoiseTexName, sizeof(char) * iSize, &dwRead, NULL);

		iSize = (_int)strnlen_s(iter->DissolveTexName, sizeof(char)*MAX_PATH);
		WriteFile(hFile, &iSize, sizeof(_int), &dwRead, NULL);
		WriteFile(hFile, iter->DissolveTexName, sizeof(char) * iSize, &dwRead, NULL);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CSave_Loader::Load_File(HWND hWnd, wstring strFileName, list<EFFECT_DESC*>* pEffectDesc)
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