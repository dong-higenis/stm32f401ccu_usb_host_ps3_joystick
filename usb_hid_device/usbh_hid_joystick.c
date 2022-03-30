/* Includes ------------------------------------------------------------------*/
#include "usbh_hid_joystick.h"
#include "usbh_hid_parser.h"

#include "uart_proto.h"

//#define DEBUG_JOY_INFO
//#define DEBUG_JOY_RAW_INFO

#define MIN_JOY_SEND_TIME_MS (1000)

static USBH_StatusTypeDef USBH_HID_JoystickDecode(USBH_HandleTypeDef *phost);

HID_JOYSTICK_Info_TypeDef    joystick_info;

static uint32_t           old_report_data[8] = {0};
uint32_t                  joystick_report_data[8];
uint32_t                  joystick_rx_report_buf[8];

/* Structures defining how to access items in a HID joystick report */

/* Access x coordinate change. */
static const HID_Report_ItemTypedef prop_x =
{
  (uint8_t *)(void *)joystick_report_data + 1, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  1,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};

/* Access y coordinate change. */
static const HID_Report_ItemTypedef prop_y =
{
  (uint8_t *)(void *)joystick_report_data + 2, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  1,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};


/* Access y coordinate change. */
static const HID_Report_ItemTypedef prop_z =
{
  (uint8_t *)(void *)joystick_report_data + 3, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  1,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};


/* Access y coordinate change. */
static const HID_Report_ItemTypedef prop_rz =
{
  (uint8_t *)(void *)joystick_report_data + 4, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  1,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef prop_pad =
{
  (uint8_t *)(void *)joystick_report_data + 5, /*data*/
  4,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0x0F,  /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button A state. */
static const HID_Report_ItemTypedef prop_btn_a =
{
  (uint8_t *)(void *)joystick_report_data + 6, /*data*/
  1,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,  /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button B state. */
static const HID_Report_ItemTypedef prop_btn_b =
{
  (uint8_t *)(void *)joystick_report_data + 6, /*data*/
  1,     /*size*/
  1,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,  /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};


/* Access button X state. */
static const HID_Report_ItemTypedef prop_btn_x =
{
  (uint8_t *)(void *)joystick_report_data + 6, /*data*/
  1,     /*size*/
  3,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button Y state. */
static const HID_Report_ItemTypedef prop_btn_y =
{
  (uint8_t *)(void *)joystick_report_data + 6, /*data*/
  1,     /*size*/
  4,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button L1 state. */
static const HID_Report_ItemTypedef prop_btn_l1 =
{
  (uint8_t *)(void *)joystick_report_data + 6, /*data*/
  1,     /*size*/
  6,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button R1 state. */
static const HID_Report_ItemTypedef prop_btn_r1 =
{
  (uint8_t *)(void *)joystick_report_data + 6, /*data*/
  1,     /*size*/
  7,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access hat switch right state. */
static const HID_Report_ItemTypedef prop_hat_switch_left =
{
  (uint8_t *)(void *)joystick_report_data + 7, /*data*/
  1,     /*size*/
  5,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access hat switch right state. */
static const HID_Report_ItemTypedef prop_hat_switch_right =
{
  (uint8_t *)(void *)joystick_report_data + 7, /*data*/
  1,     /*size*/
  6,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button L2 state. */
static const HID_Report_ItemTypedef prop_btn_l2 =
{
  (uint8_t *)(void *)joystick_report_data + 7, /*data*/
  1,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button R2 state. */
static const HID_Report_ItemTypedef prop_btn_r2 =
{
  (uint8_t *)(void *)joystick_report_data + 7, /*data*/
  1,     /*size*/
  1,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button Select state. */
static const HID_Report_ItemTypedef prop_btn_select =
{
  (uint8_t *)(void *)joystick_report_data + 7, /*data*/
  1,     /*size*/
  2,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button Start state. */
static const HID_Report_ItemTypedef prop_btn_start =
{
  (uint8_t *)(void *)joystick_report_data + 7, /*data*/
  1,     /*size*/
  3,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};


/**
  * @}
  */


/** @defgroup USBH_HID_JOYSTICK_Private_Functions
  * @{
  */

/**
  * @brief  USBH_HID_JoystickInit
  *         The function init the HID Joystick.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_HID_JoystickInit(USBH_HandleTypeDef *phost)
{
  uint32_t i;
  HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;

  memset(&joystick_info, 0, sizeof(HID_JOYSTICK_Info_TypeDef));

  for (i = 0U; i < (sizeof(joystick_report_data) / sizeof(uint32_t)); i++)
  {
  	joystick_report_data[i] = 0U;
  }

  if (HID_Handle->length > sizeof(joystick_report_data))
  {
    HID_Handle->length = sizeof(joystick_report_data);
  }

  HID_Handle->pData = (uint8_t *)(void *)joystick_rx_report_buf;
  USBH_HID_FifoInit(&HID_Handle->fifo, phost->device.Data, HID_QUEUE_SIZE * sizeof(joystick_report_data));

  return USBH_OK;
}

/**
  * @brief  USBH_HID_GetJoystickInfo
  *         The function return joystick information.
  * @param  phost: Host handle
  * @retval joystick information
  */
HID_JOYSTICK_Info_TypeDef *USBH_HID_GetJoystickInfo(USBH_HandleTypeDef *phost)
{
  if (USBH_HID_JoystickDecode(phost) == USBH_OK)
  {
    return &joystick_info;
  }
  else
  {
    return NULL;
  }
}

#ifdef DEBUG_JOY_INFO

void print_pushed(uint8_t b) {
	printf("%c ", b?'O':'X');
}

void print_joy_info(HID_JOYSTICK_Info_TypeDef joystick_info)
{
  printf("\n");
  printf("%3d ", (char)joystick_info.left_axis_x);
  printf("%3d ", (char)joystick_info.left_axis_y);
  printf("%3d ", (char)joystick_info.right_axis_x);
  printf("%3d ", (char)joystick_info.right_axis_y);
  printf("%02X ", joystick_info.pad_arrow);
  print_pushed(joystick_info.left_hat);
  print_pushed(joystick_info.right_hat);
  print_pushed(joystick_info.select);
  print_pushed(joystick_info.start);
  print_pushed(joystick_info.pad_a);
  print_pushed(joystick_info.pad_b);
  print_pushed(joystick_info.pad_x);
  print_pushed(joystick_info.pad_y);
  print_pushed(joystick_info.l1);
  print_pushed(joystick_info.r1);
  print_pushed(joystick_info.l2);
  print_pushed(joystick_info.r2);
}

#endif

/**
  * @brief  USBH_HID_JoystickDecode
  *         The function decode joystick data.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_HID_JoystickDecode(USBH_HandleTypeDef *phost)
{
  HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;
  static uint32_t prev_time = 0;

  if (HID_Handle->length == 0U)
  {
    return USBH_FAIL;
  }

  /*Fill report */
  if (USBH_HID_FifoRead(&HID_Handle->fifo, &joystick_report_data, HID_Handle->length) ==  HID_Handle->length)
  {
  	uint8_t* p = (uint8_t*)joystick_report_data;
    uint8_t is_diff=0;

    for(uint8_t i=0;i<HID_Handle->length/4;i++) {
    	if(old_report_data[i] != joystick_report_data[i]) {
    		is_diff = 1;
    	}
    }

    if(!is_diff && ((HAL_GetTick() - prev_time) <  MIN_JOY_SEND_TIME_MS)) {
    	return USBH_OK;
    }

    prev_time = HAL_GetTick();

    memcpy(old_report_data, p, HID_Handle->length);

#ifdef DEBUG_JOY_RAW_INFO
    printf("\n");
    for(uint16_t i=0;i<HID_Handle->length;i++) {
    	printf("%02X ", HID_Handle->pData[i]);
    }
#endif

    /*Decode report */
    joystick_info.pad_arrow    = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_pad, 0U) & 0x0F;
    joystick_info.left_hat     = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_hat_switch_left, 0U) ? 1 : 0;
  	joystick_info.right_hat    = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_hat_switch_right, 0U) ? 1 : 0;

    joystick_info.left_axis_x  = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_x, 0U);
    joystick_info.left_axis_y  = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_y, 0U);
    joystick_info.right_axis_x = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_z, 0U);
    joystick_info.right_axis_y = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_rz, 0U);

    joystick_info.pad_a        = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_btn_a, 0U) ? 1 : 0;
    joystick_info.pad_b        = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_btn_b, 0U) ? 1 : 0;
    joystick_info.pad_x        = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_btn_x, 0U) ? 1 : 0;
    joystick_info.pad_y        = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_btn_y, 0U) ? 1 : 0;

    joystick_info.l1           = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_btn_l1, 0U) ? 1 : 0;
    joystick_info.r1           = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_btn_r1, 0U) ? 1 : 0;
    joystick_info.l2           = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_btn_l2, 0U) ? 1 : 0;
    joystick_info.r2           = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_btn_r2, 0U) ? 1 : 0;

    joystick_info.select       = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_btn_select, 0U) ? 1 : 0;
    joystick_info.start        = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_btn_start, 0U) ? 1 : 0;
#ifdef DEBUG_JOY_INFO
    print_joy_info(joystick_info);
#endif
    send_uart_data(UART_CMD_JOY, (uint8_t*)&joystick_info, sizeof(HID_JOYSTICK_Info_TypeDef));
    return USBH_OK;
  }

  return USBH_FAIL;
}

// hid event callback
void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
	USBH_HID_GetJoystickInfo(phost);
}


/************************ END OF FILE****/
