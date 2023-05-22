#pragma once
#include "Base.h"
#include "GameObject.h"
#include "Effect.h"

BEGIN(Engine)

class CSave_Loader final : public CBase
{
	DECLARE_SINGLETON(CSave_Loader)
private:
	CSave_Loader();
	virtual ~CSave_Loader() = default;

public:
	HRESULT		Load_Effect(HWND hWnd, wstring strFileName, list<EFFECT_DESC*>* pEffectDesc);

	HRESULT		Save_File(HWND hWnd, list<EFFECT_DESC*>* pEffectDesc, const char* pTag);
	HRESULT		Load_File(HWND hWnd, wstring strFileName, list<EFFECT_DESC*>* pEffectDesc);

public:
	virtual void Free()override;

private:
	wstring m_strDirPath;
};

END
