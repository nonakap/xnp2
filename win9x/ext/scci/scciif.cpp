/**
 * @file	scciif.cpp
 * @brief	SCCI �A�N�Z�X �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "scciif.h"
#include "scci.h"
#include "SCCIDefines.h"

/**
 * �R���X�g���N�^
 */
CScciIf::CScciIf()
	: m_hModule(NULL)
	, m_pManager(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CScciIf::~CScciIf()
{
	Deinitialize();
}

/**
 * ������
 * @retval true ����
 * @retval false ���s
 */
bool CScciIf::Initialize()
{
	if (m_hModule)
	{
		return false;
	}

	do
	{
		m_hModule = ::LoadLibrary(TEXT("SCCI.DLL"));
		if (m_hModule == NULL)
		{
			break;
		}

		/* �T�E���h�C���^�[�t�F�[�X�}�l�[�W���[�擾�p�֐��A�h���X�擾 */
		SCCIFUNC fnGetSoundInterfaceManager = reinterpret_cast<SCCIFUNC>(::GetProcAddress(m_hModule, "getSoundInterfaceManager"));
		if (fnGetSoundInterfaceManager == NULL)
		{
			break;
		}

		/* �T�E���h�C���^�[�t�F�[�X�}�l�[�W���[�擾 */
		m_pManager = (*fnGetSoundInterfaceManager)();
		if (m_pManager == NULL)
		{
			break;
		}

		/* �T�E���h�C���^�[�t�F�[�X�}�l�[�W���[�C���X�^���X������ */
		/* �K���ŏ��Ɏ��s���Ă������� */
		if (!m_pManager->initializeInstance())
		{
			break;
		}

		/* ���Z�b�g���s�� */
		Reset();
		return true;
	} while (false /*CONSTCOND*/);

	Deinitialize();
	return false;
}

/**
 * ���
 */
void CScciIf::Deinitialize()
{
	if (m_pManager)
	{
		/* �ꊇ�J������ꍇ�i�`�b�v�ꊇ�J���̏ꍇ�j */
		m_pManager->releaseAllSoundChip();

		/* �T�E���h�C���^�[�t�F�[�X�}�l�[�W���[�C���X�^���X�J�� */
		/* FreeLibrary���s���O�ɕK���Ăяo�����Ă������� */
		m_pManager->releaseInstance();

		m_pManager = NULL;
	}

	if (m_hModule)
	{
		::FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
}

/**
 * �������Z�b�g
 */
void CScciIf::Reset()
{
	if (m_pManager)
	{
		/* ���Z�b�g���s�� */
		m_pManager->reset();
	}
}

/**
 * �C���^�[�t�F�C�X�擾
 * @param[in] nChipType �^�C�v
 * @param[in] nClock �N���b�N
 * @return �C���X�^���X
 */
IExternalChip* CScciIf::GetInterface(IExternalChip::ChipType nChipType, UINT nClock)
{
	const bool bInitialized = Initialize();

	do
	{
		if (m_pManager == NULL)
		{
			break;
		}

		SC_CHIP_TYPE iSoundChipType = SC_TYPE_NONE;
		switch (nChipType)
		{
			case IExternalChip::kAY8910:
				iSoundChipType = SC_TYPE_AY8910;
				break;

			case IExternalChip::kYM2203:
				iSoundChipType = SC_TYPE_YM2203;
				break;

			case IExternalChip::kYM2608:
				iSoundChipType = SC_TYPE_YM2608;
				break;

			case IExternalChip::kYM3438:
				iSoundChipType = SC_TYPE_YM2612;
				break;

			case IExternalChip::kYMF288:
				iSoundChipType = SC_TYPE_YMF288;
				break;

			case IExternalChip::kYM3812:
				iSoundChipType = SC_TYPE_YM3812;
				break;

			case IExternalChip::kYMF262:
				iSoundChipType = SC_TYPE_YMF262;
				break;

			case IExternalChip::kY8950:
				iSoundChipType = SC_TYPE_Y8950;
				break;

			case IExternalChip::kYM2151:
				iSoundChipType = SC_TYPE_YM2151;
				break;

			default:
				break;
		}

		SoundChip* pSoundChip = m_pManager->getSoundChip(iSoundChipType, nClock);
		if (pSoundChip != NULL)
		{
			/* �T�E���h�`�b�v�擾�ł��� */
			return new Chip(this, pSoundChip);
		}
	} while (false /*CONSTCOND*/);

	if (bInitialized)
	{
//		Deinitialize();
	}
	return NULL;
}

/**
 * ���
 * @param[in] pChip �`�b�v
 */
void CScciIf::Detach(CScciIf::Chip* pChip)
{
	/* �`�b�v�̊J���i�`�b�v�P�ʂŊJ���̏ꍇ�j */
	if (m_pManager)
	{
		m_pManager->releaseSoundChip(*pChip);
	}
}

/* ---- �`�b�v */

/**
 * �R���X�g���N�^
 * @param[in] pScciIf �e�C���X�^���X
 * @param[in] pSoundChip �`�b�v �C���X�^���X
 */
CScciIf::Chip::Chip(CScciIf* pScciIf, SoundChip* pSoundChip)
	: m_pScciIf(pScciIf)
	, m_pSoundChip(pSoundChip)
{
}

/**
 * �f�X�g���N�^
 */
CScciIf::Chip::~Chip()
{
	m_pScciIf->Detach(this);
}

/**
 * �I�y���[�^
 */
CScciIf::Chip::operator SoundChip*()
{
	return m_pSoundChip;
}

/**
 * Get chip type
 * @return The type of the chip
 */
IExternalChip::ChipType CScciIf::Chip::GetChipType()
{
	int iSoundChip = m_pSoundChip->getSoundChipType();

	const SCCI_SOUND_CHIP_INFO* pInfo = m_pSoundChip->getSoundChipInfo();
	if (pInfo)
	{
		iSoundChip = pInfo->iSoundChip;
	}

	switch (iSoundChip)
	{
		case SC_TYPE_AY8910:
			return IExternalChip::kAY8910;

		case SC_TYPE_YM2203:
			return IExternalChip::kYM2203;

		case SC_TYPE_YM2608:
			return IExternalChip::kYM2608;

		case SC_TYPE_YM2612:
			return IExternalChip::kYM3438;

		case SC_TYPE_YM3812:
			return IExternalChip::kYM3812;

		case SC_TYPE_YMF262:
			return IExternalChip::kYMF262;

		case SC_TYPE_YMF288:
			return IExternalChip::kYMF288;

		case SC_TYPE_Y8950:
			return IExternalChip::kY8950;

		case SC_TYPE_YM2151:
			return IExternalChip::kYM2151;

		default:
			break;
	}
	return IExternalChip::kNone;
}

/**
 * ���Z�b�g
 */
void CScciIf::Chip::Reset()
{
}

/**
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CScciIf::Chip::WriteRegister(UINT nAddr, UINT8 cData)
{
	m_pSoundChip->setRegister(nAddr, cData);
}

/**
 * ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 * @param[in] nParameter �p�����[�^
 * @return ���U���g
 */
INTPTR CScciIf::Chip::Message(UINT nMessage, INTPTR nParameter)
{
	return 0;
}
