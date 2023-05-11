#include "..\Public\Fmod\Sound_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_SINGLETON(CSound_Manager)

/* FMOD_LOOP_OFF: ������� �� */
/*  FMOD_LOOP_NORMAL : ������� �� */

HRESULT CSound_Manager::Initialize()
{
	m_eResult = FMOD::System_Create(&m_pSystem);
	if (m_eResult != FMOD_OK)
		return E_FAIL;

	m_eResult = m_pSystem->getVersion(&m_iVersion);
	if (m_eResult != FMOD_OK)
		return E_FAIL;

	else
		printf("FMOD version %08x\n", m_iVersion);

	m_eResult = m_pSystem->init(32, FMOD_INIT_NORMAL, m_pExtradriverdata);  // �ý��� �ʱ�ȭ : chaneel 32������ ���
	if (m_eResult != FMOD_OK)
		return E_FAIL;

	if (FAILED(Load_BgmFile()))
		return E_FAIL;
		
	/*if (FAILED(Load_SoundFile()))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CSound_Manager::Play_Sound(TCHAR * pSoundKey, int eChannel, float fVolume)
{
	if (MAX_CHANNEL <= eChannel)
		return E_FAIL;

	UMAP_SOUND::iterator iter;

	iter = find_if(
		m_umapSound.begin(),
		m_umapSound.end(),
		[&](auto& iter)->bool { return !lstrcmp(pSoundKey, iter.first); }
	);

	if (iter == m_umapSound.end())
		return E_FAIL;

	m_pSystem->playSound(
		iter->second,
		0,
		false,
		&m_pChannel[eChannel]
	);

	Set_SoundVolume(eChannel, SOUND_VOLUME::CUSTOM_VOLUME, fVolume);

	return S_OK;
}

HRESULT CSound_Manager::Play_Sound(TCHAR * pSoundKey, int eChannel, SOUND_VOLUME eVolume, float fVolume)
{
	if (MAX_CHANNEL <= eChannel)
		return E_FAIL;

	UMAP_SOUND::iterator iter;

	iter = find_if(
		m_umapSound.begin(),
		m_umapSound.end(),
		[&](auto& iter)->bool { return !lstrcmp(pSoundKey, iter.first); }
	);

	if (iter == m_umapSound.end())
		return E_FAIL;

	m_pSystem->playSound(
		iter->second,
		0,
		false,
		&m_pChannel[eChannel]
	);

	Set_SoundVolume(eChannel, eVolume, fVolume);

	return S_OK;
}

HRESULT CSound_Manager::Set_SoundVolume(int eChannel, SOUND_VOLUME eVolume, float fVolume)
{
	if (MAX_CHANNEL <= eChannel)
		return E_FAIL;

	switch (eVolume)
	{
	case CUSTOM_VOLUME:
		m_pChannel[eChannel]->setVolume(fVolume);
		break;
	case MIN_VOLUME:
		m_pChannel[eChannel]->setVolume(0.1f);
		break;
	case DEFAULT_VOLUME:
		m_pChannel[eChannel]->setVolume(0.5f);
		break;
	case MAX_VOLUME:
		m_pChannel[eChannel]->setVolume(1.f);
		break;
	default:
		break;
	}

	m_pSystem->update();

	return S_OK;
}

HRESULT CSound_Manager::Stop_Sound(int eChannel)
{
	if (MAX_CHANNEL <= eChannel)
		return E_FAIL;

	m_pChannel[eChannel]->setPaused(true);

	return S_OK;
}

void CSound_Manager::Stop_AllSound()
{
	for (_uint i = 0; i < MAX_CHANNEL; ++i)
		m_pChannel[i]->setPaused(true);
}

HRESULT CSound_Manager::Load_SoundFile()
{
	_finddatai64_t tFD;
	intptr_t hFile;

	char szCurPath[128] = "../../Sounds/";
	char szFullPath[128] = "";
	hFile = _findfirsti64("../../Sounds/Sounds/*.*", &tFD);

	if (-1 == hFile)
		return E_FAIL;

	_int iResult = 0;

	while (-1 != iResult)
	{
		strcpy_s(szFullPath, szCurPath);
		/* ���� �����̸� ���� (���� ���) */
		strcat_s(szFullPath, tFD.name);

		FMOD::Sound*     pSound = nullptr;

		/* ���� ���� */
		m_eResult = m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF, 0, &pSound);

		if (FMOD_OK == m_eResult)
		{
			/* null ���� ���� + 1 �ؼ� ���� å�� */
			_int iLength = static_cast<_int>(strlen(tFD.name) + 1);

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			/* �ƽ�Ű �ڵ� -> �����ڵ� �� ��ȯ */
			MultiByteToWideChar(CP_ACP, 0, tFD.name, iLength, pSoundKey, iLength);

			m_umapSound.emplace(pSoundKey, pSound);
		}
		/* ���� ��ġ�� ������ ã�´�. ���̻� ������ -1 ���� */
		iResult = _findnexti64(hFile, &tFD);
	} 

	_findclose(hFile);
	m_pSystem->update();

	return S_OK;
}

HRESULT CSound_Manager::Load_BgmFile()
{
	_finddatai64_t tFD;
	intptr_t hFile;

	char szCurPath[128] = "../../Sounds/Bgm/";
	char szFullPath[128] = "";
	hFile = _findfirsti64("../../Sounds/Bgm/*.*", &tFD);

	if (-1 == hFile)
		return E_FAIL;

	_int iResult = 0;

	while (-1 != iResult)
	{
		strcpy_s(szFullPath, szCurPath);
		/* ���� �����̸� ���� (���� ���) */
		strcat_s(szFullPath, tFD.name);

		FMOD::Sound*     pSound = nullptr;

		/* ���� ���� */
		m_eResult = m_pSystem->createSound(szFullPath, FMOD_LOOP_NORMAL, 0, &pSound);

		if (FMOD_OK == m_eResult)
		{
			/* null ���� ���� + 1 �ؼ� ���� å�� */
			_int iLength = static_cast<_int>(strlen(tFD.name) + 1);

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			/* �ƽ�Ű �ڵ� -> �����ڵ� �� ��ȯ */
			MultiByteToWideChar(CP_ACP, 0, tFD.name, iLength, pSoundKey, iLength);

			m_umapSound.emplace(pSoundKey, pSound);
		}
		/* ���� ��ġ�� ������ ã�´�. ���̻� ������ -1 ���� */
		iResult = _findnexti64(hFile, &tFD);
	}

	_findclose(hFile);
	m_pSystem->update();

	return S_OK;
}

void CSound_Manager::Free()
{
	for (auto& Pair : m_umapSound)
	{
		delete[] Pair.first;
		Pair.second->release();
	}
	m_umapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
}
