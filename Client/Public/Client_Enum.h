#pragma once

enum LEVEL_ID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_CITY, LEVEL_FOREST, LEVEL_TEST, LEVEL_ANIMTOOL, LEVEL_END };

enum COLLISION_LAYER { COLL_BASE, COLL_MOVE, COLL_PLAYERATTACK, COLL_MONSTERATTACK, COLL_END };

enum SOUND_CHANNEL
{
	BGM,
	VFX
};

// ���� �Ӽ�
enum ELEMENT
{
	ELMT_NONE,		// ���Ӽ�	- �Ͼ��(����� ��Ʈ ����)
	ELMT_SPECTRA,	// ��		- �����
	ELMT_HAVOC,		// ���		- ���ֻ�
	ELMT_AERO,		// �ٶ�		- ���λ�
	ELMT_FUSION,	// ��		- ��Ȳ��
	ELMT_GLACIO,	// ����		- �ϴû�
	ELMT_CONDUCTO,	// ����		- �����
	ELMT_END
};