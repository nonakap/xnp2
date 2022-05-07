/**
 * @file	filesel.h
 * @brief	Interface of the selection of files
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void filesel_fdd(REG8 drv);
void filesel_hdd(REG8 drv);

#ifdef __cplusplus
}
#endif

