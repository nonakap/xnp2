/**
 *	@file	joymng.h
 *	@brief	�W���C�p�b�h���͂̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#ifdef __cplusplus
extern "C"
{

void joymng_initialize();
bool joymng_isEnabled();
void joymng_sync();

#endif

REG8 joymng_getstat(void);

#ifdef __cplusplus
}
#endif
