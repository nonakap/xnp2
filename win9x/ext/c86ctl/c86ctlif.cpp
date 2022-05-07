/**
 * @file	c86ctlif.cpp
 * @brief	G.I.M.I.C �A�N�Z�X �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "c86ctlif.h"
#include "c86ctl.h"

using namespace c86ctl;

/*! �C���^�t�F�C�X */
typedef HRESULT (WINAPI * FnCreateInstance)(REFIID riid, LPVOID* ppi);

/**
 * �R���X�g���N�^
 */
C86CtlIf::C86CtlIf()
	: m_hModule(NULL)
	, m_pChipBase(NULL)
{
}

/**
 * �f�X�g���N�^
 */
C86CtlIf::~C86CtlIf()
{
	Deinitialize();
}

/**
 * ������
 * @retval true ����
 * @retval false ���s
 */
bool C86CtlIf::Initialize()
{
	if (m_hModule)
	{
		return false;
	}

	do
	{
		/* DLL �ǂݍ��� */
		m_hModule = ::LoadLibrary(TEXT("c86ctl.dll"));
		if (m_hModule == NULL)
		{
			break;
		}
		FnCreateInstance CreateInstance = reinterpret_cast<FnCreateInstance>(::GetProcAddress(m_hModule, "CreateInstance"));
		if (CreateInstance == NULL)
		{
			break;
		}

		/* �C���X�^���X�쐬 */
		(*CreateInstance)(IID_IRealChipBase, reinterpret_cast<LPVOID*>(&m_pChipBase));
		if (m_pChipBase == NULL)
		{
			break;
		}

		/* ������ */
		if (m_pChipBase->initialize() != C86CTL_ERR_NONE)
		{
			break;
		}
		return true;

	} while (0 /*CONSTCOND*/);

	Deinitialize();
	return false;
}

/**
 * ���
 */
void C86CtlIf::Deinitialize()
{
	if (m_pChipBase)
	{
		while (!m_chips.empty())
		{
			std::map<int, Chip*>::iterator it = m_chips.begin();
			delete it->second;
		}

		m_pChipBase->deinitialize();
		m_pChipBase = NULL;
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
void C86CtlIf::Reset()
{
}

/**
 * �C���^�[�t�F�C�X�擾
 * @param[in] nChipType �^�C�v
 * @param[in] nClock �N���b�N
 * @return �C���X�^���X
 */
IExternalChip* C86CtlIf::GetInterface(IExternalChip::ChipType nChipType, UINT nClock)
{
	const bool bInitialized = Initialize();

	do
	{
		if (m_pChipBase == NULL)
		{
			break;
		}

		/* ������T�� */
		const int nDeviceCount = m_pChipBase->getNumberOfChip();
		for (int i = 0; i < nDeviceCount; i++)
		{
			/* �g�p��? */
			if (m_chips.find(i) != m_chips.end())
			{
				continue;
			}

			/* �`�b�v��T�� */
			IRealChip* pRealChip = NULL;
			m_pChipBase->getChipInterface(i, IID_IRealChip, reinterpret_cast<LPVOID*>(&pRealChip));
			if (pRealChip == NULL)
			{
				continue;
			}

			/* G.I.M.I.C ���� */
			IGimic* pGimic = NULL;
			m_pChipBase->getChipInterface(i, IID_IGimic, reinterpret_cast<LPVOID*>(&pGimic));
			if (pGimic)
			{
				Devinfo info;
				if (pGimic->getModuleInfo(&info) == C86CTL_ERR_NONE)
				{
					IExternalChip::ChipType nRealChipType = IExternalChip::kNone;
					if (!memcmp(info.Devname, "GMC-OPN3L", 9))
					{
						nRealChipType = IExternalChip::kYMF288;
					}
					else if (!memcmp(info.Devname, "GMC-OPNA", 8))
					{
						nRealChipType = IExternalChip::kYM2608;
					}
					else if (!memcmp(info.Devname, "GMC-OPL3", 8))
					{
						nRealChipType = IExternalChip::kYMF262;
					}
					else if (!memcmp(info.Devname, "GMC-OPM", 7))
					{
						nRealChipType = IExternalChip::kYM2151;
					}

					if (nChipType == nRealChipType)
					{
						/* �T�E���h�`�b�v�擾�ł��� */
						Chip* pChip = new Chip(this, pRealChip, pGimic, nRealChipType, nClock);
						m_chips[i] = pChip;
						return pChip;
					}
				}
			}

			/* ���̑��̔��� */
			IRealChip3* pChip3 = NULL;
			m_pChipBase->getChipInterface(i, IID_IRealChip3, reinterpret_cast<LPVOID*>(&pChip3));
			if (pChip3 != NULL)
			{
				c86ctl::ChipType nType = CHIP_UNKNOWN;
				pChip3->getChipType(&nType);

				IExternalChip::ChipType nRealChipType = IExternalChip::kNone;
				if (nType == CHIP_YM2203)
				{
					nRealChipType = IExternalChip::kYM2203;
				}
				else if (nType == CHIP_OPNA)
				{
					nRealChipType = IExternalChip::kYM2608;
				}
				else if ((nType == CHIP_YM2608NOADPCM) || (nType == CHIP_OPN3L))
				{
					nRealChipType = IExternalChip::kYMF288;
				}
				else if (nType == CHIP_Y8950ADPCM)
				{
					nRealChipType = IExternalChip::kY8950;
				}
				if (nChipType == nRealChipType)
				{
					/* �T�E���h�`�b�v�擾�ł��� */
					Chip* pChip = new Chip(this, pChip3, NULL, nRealChipType, nClock);
					m_chips[i] = pChip;
					return pChip;
				}
			}
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
void C86CtlIf::Detach(C86CtlIf::Chip* pChip)
{
	std::map<int, Chip*>::iterator it = m_chips.begin();
	while (it != m_chips.end())
	{
		if (it->second == pChip)
		{
			it = m_chips.erase(it);
		}
		else
		{
			++it;
		}
	}
}

/* ---- �`�b�v */

/**
 * �R���X�g���N�^
 * @param[in] pC86CtlIf C86CtlIf �C���X�^���X
 * @param[in] pRealChip �`�b�v �C���X�^���X
 * @param[in] pGimic G.I.M.I.C �C���X�^���X
 * @param[in] nChipType �`�b�v �^�C�v
 * @param[in] nClock �N���b�N
 */
C86CtlIf::Chip::Chip(C86CtlIf* pC86CtlIf, c86ctl::IRealChip* pRealChip, c86ctl::IGimic* pGimic, ChipType nChipType, UINT nClock)
	: m_pC86CtlIf(pC86CtlIf)
	, m_pRealChip(pRealChip)
	, m_pGimic(pGimic)
	, m_nChipType(nChipType)
	, m_nClock(nClock)
{
}

/**
 * �f�X�g���N�^
 */
C86CtlIf::Chip::~Chip()
{
	m_pC86CtlIf->Detach(this);
}

/**
 * Get chip type
 * @return The type of the chip
 */
IExternalChip::ChipType C86CtlIf::Chip::GetChipType()
{
	return m_nChipType;
}

/**
 * ���Z�b�g
 */
void C86CtlIf::Chip::Reset()
{
	m_pRealChip->reset();
	if (m_pGimic)
	{
		m_pGimic->setPLLClock(m_nClock);
		m_pGimic->setSSGVolume(31);
	}
}

/**
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void C86CtlIf::Chip::WriteRegister(UINT nAddr, UINT8 cData)
{
	m_pRealChip->out(nAddr, cData);
}

/**
 * ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 * @param[in] nParameter �p�����[�^
 * @return ���U���g
 */
INTPTR C86CtlIf::Chip::Message(UINT nMessage, INTPTR nParameter)
{
	return 0;
}
