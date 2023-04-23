#include "IAlphaSort.h"
#include "PipeLine.h"

_float IAlphaSort::ComputeCameraDistance(_fvector vWorldPos)
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	return XMVectorGetX(XMVector3Length(vWorldPos - XMLoadFloat4(&pPipeLine->Get_CamPosition())));
}
