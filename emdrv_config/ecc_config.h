/***************************************************************************//**
 * @file ecc_config.h
 * @brief ECC driver configuration file.
 * @version x.x.x
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef __SILICON_LABS_ECC_CONFIG_H__
#define __SILICON_LABS_ECC_CONFIG_H__

/***************************************************************************//**
 * @addtogroup emdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ECC
 * @{
 ******************************************************************************/

/** ECC curve support inclusion / exclusion
    The user may choose to include or exclude support for any of the ECC curves
    listed below, by defining or undefining the INCLUDE_ECC_XXX definition.
    Inclusion of each curve consumes approximately 250 bytes of flash memory.*/

#define INCLUDE_ECC_P192
#define INCLUDE_ECC_P224
#define INCLUDE_ECC_P256
#define INCLUDE_ECC_BIN163
#define INCLUDE_ECC_BIN163K
#define INCLUDE_ECC_BIN233
#define INCLUDE_ECC_BIN233K

/** @} (end addtogroup ECC) */
/** @} (end addtogroup emdrv) */

#endif /* __SILICON_LABS_ECC_CONFIG_H__ */
