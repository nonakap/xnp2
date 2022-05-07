/**
 * @file	asiodriverlist.h
 * @brief	ASIO �h���C�o ���X�g �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <vector>

interface IASIO;

/**
 * ASIO �h���C�o���
 */
struct AsioDriverInfo
{
	CLSID clsid;					/*!< �N���X ID */
	TCHAR szDllPath[MAX_PATH];		/*!< DLL �p�X */
	TCHAR szDriverName[128];		/*!< �h���C�o�� */
};

/**
 * @brief ASIO �h���C�o ���X�g �N���X
 */
class AsioDriverList : public std::vector<AsioDriverInfo>
{
public:
	void EnumerateDrivers();
	IASIO* OpenDriver(LPCTSTR lpDriverName);

private:
	static bool FindDrvPath(LPCTSTR lpClsId, LPTSTR lpDllPath, UINT cchDllPath);
};
