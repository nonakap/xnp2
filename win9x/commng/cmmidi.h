/**
 * @file	cmmidi.h
 * @brief	MIDI �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "cmbase.h"
#include "mimpidef.h"

extern const TCHAR cmmidi_midimapper[];
extern const TCHAR cmmidi_midivst[];
#if defined(VERMOUTH_LIB)
extern const TCHAR cmmidi_vermouth[];
#endif
#if defined(MT32SOUND_DLL)
extern const TCHAR cmmidi_mt32sound[];
#endif
extern LPCTSTR cmmidi_mdlname[12];

void cmmidi_initailize(void);

class CComMidiIn32;
class CComMidiOut;

/**
 * @brief commng MIDI �f�o�C�X �N���X
 */
class CComMidi : public CComBase
{
public:
	static CComMidi* CreateInstance(LPCTSTR lpMidiOut, LPCTSTR lpMidiIn, LPCTSTR lpModule);

protected:
	CComMidi();
	virtual ~CComMidi();
	virtual UINT Read(UINT8* pData);
	virtual UINT Write(UINT8 cData);
	virtual UINT8 GetStat();
	virtual INTPTR Message(UINT msg, INTPTR param);

private:
	enum
	{
		MIDI_BUFFER			= (1 << 10)
	};

	/**
	 * �t�F�C�Y
	 */
	enum tagMidiCtrl
	{
		MIDICTRL_READY		= 0,
		MIDICTRL_2BYTES,
		MIDICTRL_3BYTES,
		MIDICTRL_EXCLUSIVE,
		MIDICTRL_TIMECODE,
		MIDICTRL_SYSTEM
	};

	/**
	 * @brief MIDI
	 */
	struct MIDICH
	{
		UINT8	prog;
		UINT8	press;
		UINT16	bend;
		UINT8	ctrl[28];
	};

	CComMidiIn32* m_pMidiIn;		/*!< MIDI IN */
	CComMidiOut* m_pMidiOut;		/*!< MIDI OUT */
	UINT m_nModule;					/*!< ���W���[���ԍ� */
	tagMidiCtrl m_nMidiCtrl;		/*!< �t�F�[�Y */
	UINT m_nIndex;					/*!< �o�b�t�@�ʒu */
	UINT m_nRecvSize;				/*!< ��M�T�C�Y */
	UINT8 m_cLastData;				/*!< �Ō�̃f�[�^ */
	bool m_bMimpiDef;				/*!< MIMPIDEF �L�� */
	MIMPIDEF m_mimpiDef;			/*!< MIMPIDEF */
	MIDICH m_midich[16];			/*!< MIDI CH */
	UINT8 m_sBuffer[MIDI_BUFFER];	/*!< �o�b�t�@ */

	bool Initialize(LPCTSTR lpMidiOut, LPCTSTR lpMidiIn, LPCTSTR lpModule);
	static UINT module2number(LPCTSTR lpModule);
	void midiallnoteoff();
	void midireset();
	void midisetparam();
};
