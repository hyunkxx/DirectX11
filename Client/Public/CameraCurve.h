#pragma once

#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CCameraCurve : public CBase
{
public:
	enum CURVE
	{
		CURVE_NOEL_R,
		CURVE_WEAPON_UPGRADE,
		CURVE_END
	};
public:
	typedef struct CameraCurveKey
	{
		_float fXCurAngle = { 0.f };
		_float fYCurAngle = { 0.f };
		_float fCurDistance = { 0.f };
		_float fPinHeight = { 0.f };
		_double Frame = { 0.0 };
	}CURVEKEY;

public:
	static CCameraCurve*	m_pCamCurve[CURVE_END];
	static HRESULT Init_CameraCurves();
	static HRESULT Delete_CameraCurves();

	void Set_Duration(_double Duration)
	{
		m_CamCurveDuration = Duration;
	}

	_double Get_Duration()
	{
		return m_CamCurveDuration;
	}

	void Set_NumKeys(_uint iNumKeys)
	{
		m_iNumKeys = iNumKeys;
	}

	_uint Get_NumKeys()
	{
		return m_iNumKeys;
	}

	CURVEKEY Get_CurCamCurve(_uint* iCurKey, _double FrameAcc)
	{
		CURVEKEY	LastCurveKey = m_Curves.back();

		if (FrameAcc >= LastCurveKey.Frame)
		{
			return LastCurveKey;
		}
		else
		{
			while (FrameAcc >= m_Curves[*iCurKey + 1].Frame)
				*iCurKey = *iCurKey + 1;

			CURVEKEY tOut;
			ZeroMemory(&tOut, sizeof CURVEKEY);
			_float fRatio = _float((FrameAcc - m_Curves[*iCurKey].Frame) / (m_Curves[*iCurKey + 1].Frame - m_Curves[*iCurKey].Frame));

			if (fRatio > 1.f || fRatio < 0.f)
				int a = 10;


			tOut.fXCurAngle = m_Curves[*iCurKey].fXCurAngle *  (1 - fRatio) + m_Curves[*iCurKey + 1].fXCurAngle * fRatio;
			tOut.fYCurAngle = m_Curves[*iCurKey].fYCurAngle *  (1 - fRatio) + m_Curves[*iCurKey + 1].fYCurAngle * fRatio;
			tOut.fCurDistance = m_Curves[*iCurKey].fCurDistance *  (1 - fRatio) + m_Curves[*iCurKey + 1].fCurDistance * fRatio;
			tOut.fPinHeight = m_Curves[*iCurKey].fPinHeight *  (1 - fRatio) + m_Curves[*iCurKey + 1].fPinHeight * fRatio;
			return tOut;
		}
	}


private:
	CCameraCurve();
	virtual ~CCameraCurve() = default;

private:
	vector<CURVEKEY> m_Curves;
	_uint	m_iNumKeys;
	_double m_CamCurveDuration;




public:
	virtual void Free() override;
};

END