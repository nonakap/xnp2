/*!
 * @file	guard.h
 * @brief	�N���e�B�J�� �Z�N�V���� �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/*!
 * @brief �N���e�B�J�� �Z�N�V���� �N���X
 */
class CGuard
{
public:
	/*! �R���X�g���N�^ */
	CGuard() { ::InitializeCriticalSection(&m_cs); }

	/*! �f�X�g���N�^ */
	~CGuard() { ::DeleteCriticalSection(&m_cs); }

	/*! �N���e�B�J�� �Z�N�V�����J�n */
	void Enter() { ::EnterCriticalSection(&m_cs); }

	/*! �N���e�B�J�� �Z�N�V�����I�� */
	void Leave() { ::LeaveCriticalSection(&m_cs); }

private:
	CRITICAL_SECTION m_cs;		//!< �N���e�B�J�� �Z�N�V�������
};
