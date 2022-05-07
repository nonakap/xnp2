/**
 * @file	juliet.h
 * @brief	ROMEO �A�N�Z�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "..\externalchip.h"
#include "misc\guard.h"
#include "misc\threadbase.h"

/**
 * @brief ROMEO �A�N�Z�X �N���X
 */
class CJuliet : protected CThreadBase
{
public:
	CJuliet();
	~CJuliet();
	bool Initialize();
	void Deinitialize();
	void Reset();
	IExternalChip* GetInterface(IExternalChip::ChipType nChipType, UINT nClock);
	bool IsEnabled() const;

protected:
	virtual bool Task();

private:
	//! @brief ���[�h�֐�
	struct ProcItem
	{
		LPCSTR lpSymbol;		//!< �֐���
		size_t nOffset;			//!< �I�t�Z�b�g
	};

	// ��`
	typedef ULONG (WINAPI * FnRead32)(ULONG ulPciAddress, ULONG ulRegAddress);	//!< �R���t�B�O���[�V�����ǂݎ��֐���`
	typedef VOID (WINAPI * FnOut8)(ULONG ulAddress, UCHAR ucParam);				//!< outp �֐���`
	typedef VOID (WINAPI * FnOut32)(ULONG ulAddress, ULONG ulParam);			//!< outpd �֐���`
	typedef UCHAR (WINAPI * FnIn8)(ULONG ulAddress);							//!< inp �֐���`

	HMODULE m_hModule;			//!< ���W���[��
	FnRead32 m_fnRead32;		//!< �R���t�B�O���[�V�����ǂݎ��֐�
	FnOut8 m_fnOut8;			//!< outp �֐�
	FnOut32 m_fnOut32;			//!< outpd �֐�
	FnIn8 m_fnIn8;				//!< inp �֐�
	ULONG m_ulAddress;			//!< ROMEO �x�[�X �A�h���X
	UCHAR m_ucIrq;				//!< ROMEO IRQ
	CGuard m_pciGuard;			/*!< The guard of PCI */
	CGuard m_queGuard;			/*!< The guard of que */
	size_t m_nQueIndex;			/*!< The position in que */
	size_t m_nQueCount;			/*!< The count in que */
	UINT m_que[0x400];			/*!< que */

	ULONG SearchRomeo() const;
	void Write288(UINT nAddr, UINT8 cData);

	/**
	 * @brief �`�b�v �N���X
	 */
	class Chip288 : public IExternalChip
	{
		public:
			Chip288(CJuliet* pJuliet);
			virtual ~Chip288();
			virtual ChipType GetChipType();
			virtual void Reset();
			virtual void WriteRegister(UINT nAddr, UINT8 cData);
			virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

		private:
			CJuliet* m_pJuliet;			//!< �e�C���X�^���X
	};
	IExternalChip* m_pChip288;		//!< YMF288 �C���X�^���X

	void Detach(IExternalChip* pChip);
	friend class Chip288;
};

/**
 * ROMEO �͗L��?
 * @retval true �L��
 * @retval false ����
 */
inline bool CJuliet::IsEnabled() const
{
	return (m_hModule != NULL);
}
