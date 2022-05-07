/**
 * @file	menu.h
 * @brief	���j���[�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#define MFCHECK(a) ((a) ? MF_CHECKED : MF_UNCHECKED)

UINT InsertMenuPopup(HMENU hMenu, UINT uItem, BOOL fByPosition, HMENU hPopup);
UINT AppendMenuResource(HMENU hMenu, UINT nMenuID);
UINT InsertMenuResource(HMENU hMenu, UINT uItem, BOOL fByPosition, UINT nMenuID);
HMENU GetMenuOwner(HMENU hMenu, UINT uItem, int* pnPos = NULL);

void sysmenu_initialize(HMENU hMenu);
void sysmenu_update(HMENU hMenu);

void xmenu_initialize(HMENU hMenu);
void xmenu_update(HMENU hMenu);
