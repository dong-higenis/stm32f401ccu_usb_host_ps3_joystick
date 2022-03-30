/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_HID_JOYSTICK_H
#define __USBH_HID_JOYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HID_CLASS
  * @{
  */

/** @defgroup USBH_HID_JOYSTICK
  * @brief This file is the Header file for usbh_hid_joystick.c
  * @{
  */


/** @defgroup USBH_HID_JOYSTICK_Exported_Types
  * @{
  */

typedef struct _HID_JOYSTICK_Info
{
  uint8_t              left_axis_x;
  uint8_t              left_axis_y;
  uint8_t              right_axis_x;
  uint8_t              right_axis_y;

  uint8_t              pad_arrow:4;
  uint8_t              left_hat:1;
  uint8_t              right_hat:1;
  uint8_t              select:1;
  uint8_t              start:1;

  uint8_t              pad_a:1;
  uint8_t              pad_b:1;
  uint8_t              pad_x:1;
  uint8_t              pad_y:1;
  uint8_t              reserved:4;

  uint8_t              l1:1;
  uint8_t              l2:1;
  uint8_t              r1:1;
  uint8_t              r2:1;
} HID_JOYSTICK_Info_TypeDef;
/**
  * @}
  */

/** @defgroup USBH_HID_JOYSTICK_Exported_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_HID_JOYSTICK_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_HID_JOYSTICK_Exported_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_HID_JOYSTICK_Exported_FunctionsPrototype
  * @{
  */
extern USBH_StatusTypeDef USBH_HID_JoystickInit(USBH_HandleTypeDef *phost);
HID_JOYSTICK_Info_TypeDef *USBH_HID_GetJoystickInfo(USBH_HandleTypeDef *phost);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBH_HID_JOYSTICK_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
