#pragma once

#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)

public:
	enum SOUND_TYPE { SOUND_BGM, SOUND_VFX, SOUND_VOICE };

private:
	explicit CSound_Manager() = default;
	virtual ~CSound_Manager() = default;

private:
	enum { MAX_CHANNEL = 30 };

public:
	HRESULT Initialize();

	/* 매게변수 SOUND_VOLUME eVolum = CUSTOM_VOLUM 빼는것도 고려해볼 것 */
	HRESULT Play_Sound(
		TCHAR* pSoundKey, 
		int eChannel, 
		SOUND_VOLUME eVolum = CUSTOM_VOLUME,
		float fVolume = 0.1f
	);

	HRESULT Play_Sound(
		TCHAR* pSoundKey,
		int eChannel,
		float fVolume = 1.f
	);

	HRESULT Set_SoundVolume(
		int eChannel, 
		SOUND_VOLUME eVolum = CUSTOM_VOLUME,
		float fVolume = 0.1f
	);
	
	/*HRESULT Up_SoundVolum(SOUND_CHANNEL eChannel);
	HRESULT Down_SoundVolum(SOUND_CHANNEL eChannel);*/

	HRESULT Stop_Sound(int eChannel);
	void Stop_AllSound();

	void SetVolume(SOUND_TYPE eSoundType, _float fVolume) {
		switch (eSoundType)
		{
		case SOUND_BGM:
			m_fVolume_BGM = m_fCurVolume_BGM = fVolume;
			break;
		case SOUND_VFX:
			m_fVolume_VFX = fVolume;
			break;
		case SOUND_VOICE:
			m_fVolume_Voice = fVolume;
			break;
		}
	}

	_float GetVolume(SOUND_TYPE eSoundType) const {
		switch (eSoundType)
		{
		case SOUND_BGM:
			return m_fCurVolume_BGM;
		case SOUND_VFX:
			return m_fVolume_VFX;
		case SOUND_VOICE:
			return m_fVolume_Voice;
		default:
			return 1.f;
		}
	}

	_bool BGMSmoothOff(_double TimeDelta)
	{
		if (m_fCurVolume_BGM < 0.f)
		{
			mfTimeAcc = 0.f;
			m_fCurVolume_BGM = 0.f;
			Stop_Sound(0);
			return true;
		}

		mfTimeAcc += (_float)TimeDelta * 0.01f;
		m_fCurVolume_BGM -= mfTimeAcc;
		Set_SoundVolume(0, SOUND_VOLUME::CUSTOM_VOLUME, m_fCurVolume_BGM);
		return false;
	}

	void BGMSmoothOn(_double TimeDelta)
	{
		if (m_fCurVolume_BGM >= m_fVolume_BGM)
		{
			mfTimeAcc = 0.f;
			Set_SoundVolume(0, SOUND_VOLUME::CUSTOM_VOLUME, m_fCurVolume_BGM);
			return;
		}

		mfTimeAcc += (_float)TimeDelta * 0.01f;
		m_fCurVolume_BGM += mfTimeAcc;
		Set_SoundVolume(0, SOUND_VOLUME::CUSTOM_VOLUME, m_fCurVolume_BGM);
	}

private:
	/* 사운드 폴더에 파일 다박고 로드할 때 */
	HRESULT Load_SoundFile();

private:
	/* 사운드 폴더에서 폴더 분할후 로드 할 때 (종류별로 하나씩 만들어주면 됨) */
	HRESULT Load_BgmFile();
	HRESULT Load_SFX_Ambience();
	HRESULT Load_SFX_Footstep();
	HRESULT Load_SFX_Sequence();
	HRESULT Load_SFX_UI();

	//SFX_Monster
	HRESULT Load_SFX_Monster_Anjin();
	HRESULT Load_SFX_Monster_AWukaka();
	HRESULT Load_SFX_Monster_Binglie();
	HRESULT Load_SFX_Monster_Fenglie();
	HRESULT Load_SFX_Monster_FHuxiuxiu();
	HRESULT Load_SFX_Monster_GAzizi();
	HRESULT Load_SFX_Monster_Huojin();
	HRESULT Load_SFX_Monster_Leilie();
	HRESULT Load_SFX_Monster_Qunjing();
	HRESULT Load_SFX_Monster_Wuguanzhe();
	HRESULT Load_SFX_Monster_MonImpact();
	
	//SFX_PC
	HRESULT Load_SFX_PC_Chixia();
	HRESULT Load_SFX_PC_Common();
	HRESULT Load_SFX_PC_Foley();
	HRESULT Load_SFX_PC_Rover();
	HRESULT Load_SFX_PC_Yangyang();
	HRESULT Load_SFX_PC_PlayerImpact();

	//Voice
	HRESULT Load_Voice_Chixia();
	HRESULT Load_Voice_Rover();
	HRESULT Load_Voice_Yangyang();



private:
	typedef unordered_map<TCHAR*, FMOD::Sound*>		UMAP_SOUND;
	UMAP_SOUND			m_umapSound;

	FMOD::System*		m_pSystem = nullptr;
	FMOD::Channel*		m_pChannel[MAX_CHANNEL];
	FMOD_RESULT			m_eResult;
	unsigned int		m_iVersion;
	void*				m_pExtradriverdata = nullptr;

private:
	_float mfTimeAcc = 0.f;
	_float m_fCurVolume_BGM = 0.5f;
	
	_float m_fVolume_BGM = 0.5f;
	_float m_fVolume_VFX = 0.5f;
	_float m_fVolume_Voice = 1.f;

public:
	virtual void Free() override;
};

END