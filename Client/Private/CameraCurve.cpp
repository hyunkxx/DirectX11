#include "stdafx.h"
#include "..\Public\CameraCurve.h"


CCameraCurve* CCameraCurve::m_pCamCurve[CURVE_END] =
{
	nullptr, nullptr
};


HRESULT CCameraCurve::Init_CameraCurves()
{
	m_pCamCurve[CURVE_NOEL_R] = new CCameraCurve();
	m_pCamCurve[CURVE_NOEL_R]->Set_Duration(80.0);
	m_pCamCurve[CURVE_NOEL_R]->Set_NumKeys(5);

	// 0
	CURVEKEY CurveKey;
	CurveKey.fXCurAngle = 10.f;
	CurveKey.fYCurAngle = 30.f;
	CurveKey.fCurDistance = 1.f;
	CurveKey.fPinHeight = 0.6f;
	CurveKey.Frame = 0.0;

	m_pCamCurve[CURVE_NOEL_R]->m_Curves.push_back(CurveKey);

	// 1
	CurveKey.fXCurAngle = 0.f;
	CurveKey.fYCurAngle = 30.f;
	CurveKey.fCurDistance = 1.f;
	CurveKey.fPinHeight = 0.6f;
	CurveKey.Frame = 20.0;

	m_pCamCurve[CURVE_NOEL_R]->m_Curves.push_back(CurveKey);

	// 2
	CurveKey.fXCurAngle = 20.f;
	CurveKey.fYCurAngle = 40.f;
	CurveKey.fCurDistance = 2.f;
	CurveKey.fPinHeight = 0.6f;
	CurveKey.Frame = 40.0;

	m_pCamCurve[CURVE_NOEL_R]->m_Curves.push_back(CurveKey);

	// 3
	CurveKey.fXCurAngle = 0.f;
	CurveKey.fYCurAngle = 180.f;
	CurveKey.fCurDistance = 2.8f;
	CurveKey.fPinHeight = 0.3f + CurveKey.fCurDistance * 0.5f;
	CurveKey.Frame = 50.0;

	m_pCamCurve[CURVE_NOEL_R]->m_Curves.push_back(CurveKey);

	// 4
	CurveKey.fXCurAngle = 10.f;
	CurveKey.fYCurAngle = 180.f;
	CurveKey.fCurDistance = 3.6f;
	CurveKey.fPinHeight = 0.3f + CurveKey.fCurDistance * 0.5f;
	CurveKey.Frame = 80.0;

	m_pCamCurve[CURVE_NOEL_R]->m_Curves.push_back(CurveKey);

	////////////////
	m_pCamCurve[CURVE_WEAPON_UPGRADE] = new CCameraCurve();
	m_pCamCurve[CURVE_WEAPON_UPGRADE]->Set_Duration(700.0);
	m_pCamCurve[CURVE_WEAPON_UPGRADE]->Set_NumKeys(5);


	// 0
	CurveKey.fXCurAngle = 20.f;
	CurveKey.fYCurAngle = 0.f;
	CurveKey.fCurDistance = 3.f;
	CurveKey.fPinHeight = 1.0f;
	CurveKey.Frame = 0.0;

	m_pCamCurve[CURVE_WEAPON_UPGRADE]->m_Curves.push_back(CurveKey);

	// 1
	CurveKey.fXCurAngle = 20.f;
	CurveKey.fYCurAngle = 0.f;
	CurveKey.fCurDistance = 2.f;
	CurveKey.fPinHeight = 0.8f;
	CurveKey.Frame = 200.0;

	m_pCamCurve[CURVE_WEAPON_UPGRADE]->m_Curves.push_back(CurveKey);

	// 2
	CurveKey.fXCurAngle = 10.f;
	CurveKey.fYCurAngle = 30.f;
	CurveKey.fCurDistance = 1.3f;
	CurveKey.fPinHeight = 0.8f;
	CurveKey.Frame = 540.0;

	m_pCamCurve[CURVE_WEAPON_UPGRADE]->m_Curves.push_back(CurveKey);

	// 3
	CurveKey.fXCurAngle = 20.f;
	CurveKey.fYCurAngle = 30.f;
	CurveKey.fCurDistance = 3.f;
	CurveKey.fPinHeight = 1.0f;
	CurveKey.Frame = 600.0;

	m_pCamCurve[CURVE_WEAPON_UPGRADE]->m_Curves.push_back(CurveKey);

	// 4
	CurveKey.fXCurAngle = 20.f;
	CurveKey.fYCurAngle = 180.f;
	CurveKey.fCurDistance = 3.f;
	CurveKey.fPinHeight = 1.8f;
	CurveKey.Frame = 650.0;

	m_pCamCurve[CURVE_WEAPON_UPGRADE]->m_Curves.push_back(CurveKey);



	return S_OK;
}

HRESULT CCameraCurve::Delete_CameraCurves()
{
	for (_uint i = 0; i < CURVE_END; ++i)
		Safe_Release(m_pCamCurve[i]);


	return S_OK;
}

CCameraCurve::CCameraCurve()
{
}

void CCameraCurve::Free()
{
}