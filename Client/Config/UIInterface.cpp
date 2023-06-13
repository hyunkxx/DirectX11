#include "stdafx.h"
#include "UIInterface.h"

_float3 GetGradeColor(COLOR_TYPE eColorType, _uint iGrade)
{
	_float3 vColor;

	if (eColorType == NORMAL)
	{
		switch (iGrade)
		{
		case 0:
			vColor = DEFAULT_COLOR;
			break;
		case 1:
			vColor = ADVANCED_COLOR;
			break;
		case 2:
			vColor = RARE_COLOR;
			break;
		case 3:
			vColor = UNIQUE_COLOR;
			break;
		case 4:
			vColor = LEGEND_COLOR;
			break;
		case 5:
			vColor = DEFAULT_COLOR;
			break;
		default:
			vColor = DEFAULT_COLOR;
			break;
		}
	}
	else if (eColorType == TONEDOWN)
	{
		switch (iGrade)
		{
		case 0:
			vColor = SLOT_COLOR;
			break;
		case 1:
			vColor = SLOT_ADVANCED_COLOR;
			break;
		case 2:
			vColor = SLOT_RARE_COLOR;
			break;
		case 3:
			vColor = SLOT_UNIQUE_COLOR;
			break;
		case 4:
			vColor = SLOT_LEGEND_COLOR;
			break;
		case 5:
			vColor = SLOT_RED_COLOR;
			break;
		default:
			vColor = SLOT_COLOR;
			break;
		}
	}


	return vColor;
}
