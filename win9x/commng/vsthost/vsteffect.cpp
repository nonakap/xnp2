/**
 * @file	vsteffect.cpp
 * @brief	VST effect �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "vsteffect.h"
#include "vsteditwndbase.h"

#ifdef _WIN32
#include <shlwapi.h>
#include <atlbase.h>
#pragma comment(lib, "shlwapi.lib")
#else	// _WIN32
#include <dlfcn.h>
#endif	// _WIN32

/*! �G�t�F�N�g �n���h���[ */
std::map<AEffect*, CVstEffect*> CVstEffect::sm_effects;

/**
 * �R���X�g���N�^
 */
CVstEffect::CVstEffect()
	: m_effect(NULL)
	, m_hModule(NULL)
	, m_lpDir(NULL)
	, m_pWnd(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CVstEffect::~CVstEffect()
{
	Unload();
}

/**
 * ���[�h����
 * @param[in] lpVst �v���O�C��
 * @retval true ����
 * @retval false ���s
 */
bool CVstEffect::Load(LPCTSTR lpVst)
{
	Unload();

#ifdef _WIN32
	/* VSTi�ǂݍ��� */
	HMODULE hModule = ::LoadLibrary(lpVst);
	if (hModule == NULL)
	{
		return false;
	}
	typedef AEffect* (*FnMain)(::audioMasterCallback audioMaster);
	FnMain fnMain = reinterpret_cast<FnMain>(::GetProcAddress(hModule, "VSTPluginMain"));
	if (fnMain == NULL)
	{
		fnMain = reinterpret_cast<FnMain>(::GetProcAddress(hModule, "main"));
	}
	if (fnMain == NULL)
	{
		::FreeLibrary(hModule);
		return false;
	}

	// ������
	AEffect* effect = (*fnMain)(cAudioMasterCallback);
	if (effect == NULL)
	{
		::FreeLibrary(hModule);
		return false;
	}
	if (effect->magic != kEffectMagic)
	{
		::FreeLibrary(hModule);
		return false;
	}

	TCHAR szDir[MAX_PATH];
	::lstrcpyn(szDir, lpVst, _countof(szDir));
	::PathRemoveFileSpec(szDir);

	USES_CONVERSION;
	m_lpDir = ::strdup(T2A(szDir));

#else	// _WIN32

	/* VSTi�ǂݍ��� */
	void* hModule = ::dlopen(lpVst, 262);
	if (hModule == NULL)
	{
		return false;
	}
	typedef AEffect* (*FnMain)(::audioMasterCallback audioMaster);
	FnMain fnMain = reinterpret_cast<FnMain>(::dlsym(hModule, "VSTPluginMain"));
	if (fnMain == NULL)
	{
		fnMain = reinterpret_cast<FnMain>(::dlsym(hModule, "main"));
	}
	if (fnMain == NULL)
	{
		::dlclose(hModule);
		return false;
	}

	// ������
	AEffect* effect = (*fnMain)(cAudioMasterCallback);
	if (effect == NULL)
	{
		::dlclose(hModule);
		return false;
	}
	if (effect->magic != kEffectMagic)
	{
		::dlclose(hModule);
		return false;
	}

	m_lpDir = ::strdup(lpVst);
	char* pSlash = strrchr(m_lpDir, '/');
	if (pSlash)
	{
		*(pSlash + 1) = 0;
	}
	else
	{
		free(m_lpDir);
		m_lpDir = NULL;
	}
#endif

	printf("%d input(s), %d output(s)\n", effect->numInputs, effect->numOutputs);

	sm_effects[effect] = this;
	m_effect = effect;
	m_hModule = hModule;

	return true;
}

/**
 * �A�����[�h����
 */
void CVstEffect::Unload()
{
	if (m_effect)
	{
		sm_effects.erase(m_effect);
		m_effect = NULL;
	}
	if (m_hModule)
	{
#ifdef _WIN32
		::FreeLibrary(m_hModule);
#else	// _WIN32
		::dlclose(m_hModule);
#endif	// _WIN32
		m_hModule = NULL;
	}
	if (m_lpDir)
	{
		free(m_lpDir);
		m_lpDir = NULL;
	}
}

/**
 * �E�B���h�E �A�^�b�`
 * @param[in] pWnd �n���h��
 * @return �ȑO�̃n���h��
 */
IVstEditWnd* CVstEffect::Attach(IVstEditWnd* pWnd)
{
	IVstEditWnd* pRet = m_pWnd;
	m_pWnd = pWnd;
	return pRet;
}

/**
 * �f�B�X�p�b�`
 * @param[in] opcode The operation code
 * @param[in] index The index
 * @param[in] value The value
 * @param[in] ptr The pointer
 * @param[in] opt The option
 * @return The result
 */
VstIntPtr CVstEffect::dispatcher(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
	if (m_effect)
	{
		return (*m_effect->dispatcher)(m_effect, opcode, index, value, ptr, opt);
	}
	return 0;
}

/**
 * �v���Z�X
 * @param[in] inputs ����
 * @param[in] outputs �o��
 * @param[in] sampleFrames �T���v����
 */
void CVstEffect::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
	if (m_effect)
	{
		(*m_effect->processReplacing)(m_effect, inputs, outputs, sampleFrames);
	}
}

/**
 * �R�[���o�b�N
 * @param[in] effect The instance of effect
 * @param[in] opcode The operation code
 * @param[in] index The index
 * @param[in] value The value
 * @param[in] ptr The pointer
 * @param[in] opt The option
 * @return The result
 */
VstIntPtr CVstEffect::cAudioMasterCallback(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
	switch (opcode)
	{
		case audioMasterVersion:
			return 2400;

		default:
			break;
	}

	std::map<AEffect*, CVstEffect*>::iterator it = sm_effects.find(effect);
	if (it != sm_effects.end())
	{
		return it->second->audioMasterCallback(opcode, index, value, ptr, opt);
	}
	return 0;
}

/**
 * �R�[���o�b�N
 * @param[in] opcode The operation code
 * @param[in] index The index
 * @param[in] value The value
 * @param[in] ptr The pointer
 * @param[in] opt The option
 * @return The result
 */
VstIntPtr CVstEffect::audioMasterCallback(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
	VstIntPtr ret = 0;

	switch (opcode)
	{
		case audioMasterGetDirectory:
			ret = reinterpret_cast<VstIntPtr>(m_lpDir);
			break;

		case DECLARE_VST_DEPRECATED(audioMasterWantMidi):
			break;

		case audioMasterSizeWindow:
			if (m_pWnd)
			{
				ret = m_pWnd->OnResize(index, static_cast<VstInt32>(value));
			}
			break;

		case audioMasterUpdateDisplay:
			if (m_pWnd)
			{
				ret = m_pWnd->OnUpdateDisplay();
			}
			break;

		default:
			printf("callback: AudioMasterCallback %d %d\n", opcode, index);
			break;
	}

	return ret;
}
