/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define S_CHA_Pin GPIO_PIN_0
#define S_CHA_GPIO_Port GPIOA
#define S_CHB_Pin GPIO_PIN_1
#define S_CHB_GPIO_Port GPIOA
#define LED_CHA_Pin GPIO_PIN_3
#define LED_CHA_GPIO_Port GPIOA
#define LED_CHB_Pin GPIO_PIN_4
#define LED_CHB_GPIO_Port GPIOA
#define CENTERLINE_CHA_P_Pin GPIO_PIN_7
#define CENTERLINE_CHA_P_GPIO_Port GPIOA
#define CENTERLINE_CHA_P_EXTI_IRQn EXTI9_5_IRQn
#define CENTERLINE_CHA_D_Pin GPIO_PIN_0
#define CENTERLINE_CHA_D_GPIO_Port GPIOB
#define CENTERLINE_CHA_D_EXTI_IRQn EXTI0_IRQn
#define CENTERLINE_CHB_P_Pin GPIO_PIN_1
#define CENTERLINE_CHB_P_GPIO_Port GPIOB
#define CENTERLINE_CHB_P_EXTI_IRQn EXTI1_IRQn
#define CENTERLINE_CHB_D_Pin GPIO_PIN_10
#define CENTERLINE_CHB_D_GPIO_Port GPIOB
#define CENTERLINE_CHB_D_EXTI_IRQn EXTI15_10_IRQn
#define SET_Pin GPIO_PIN_13
#define SET_GPIO_Port GPIOB
#define SET_EXTI_IRQn EXTI15_10_IRQn
/* USER CODE BEGIN Private defines */

#define TIM_PERIOD 							1000-1  		//���ö�ʱ���������ڣ�����Ԥ��Ƶ��Ϊ72�������������Ƶ��Ϊ72M/72/TIM_PERIOD
#define SET_BLINK__DELAY_COUNT  300		//������̩��ͨ��led��˸Ƶ�ʣ�Ƶ��ΪTIM_PERIOD/SET_BLINK__DELAY_COUNT
#define SET__DELAY_COUNT_MIN  3000			//���ü������״̬�µ�adc�ɼ�������ͬʱ���ó���ʱ�䣨s=SET__DELAY_COUNT_MIN/TIM_PERIOD��
#define SET__DELAY_COUNT_MAX SET__DELAY_COUNT_MIN*2//ͬ��
#define KEY_DELAY_COUNT  1000					//����������s=KEY_DELAY_COUNT/TIM_PERIOD)
#define CHA_EN htim1.Instance->CCER=0x01										//�������ͨ��
#define CHB_EN htim1.Instance->CCER=0x10										//ͬ��
#define CH_DIS htim1.Instance->CCER=0x00										//ͬ��
#define O_CHA htim1.Instance->CCR1									//����pwmռ�ձ�
#define O_CHB htim1.Instance->CCR2

#define S_CH ADC_Value								//adc�ɼ�����
#define CHA 0U
#define CHB 1U
#define S_CHANNEL__NUM 2  //�ź�ͨ����
#define SUBDIVIDE 200 				 //�źŷּ�
#define SPEED_SUBDIVIDE 10			//�ٶȷּ�
#define S_VALUE_CYCLE_COUNT  200 //�����ڲɼ�����
#define	S_VALUE_CYCLE__SET_COUNT 1//����״̬�²ɼ�����ѭ������,һ������200ms��5��Ϊ1s��3*5=15=3s
#define SET_DELAY	40							//����״̬��ת��ʱ
#define S_VALUE_CYCLE_RANGE 3		//������ֵ�������������Χ=S_VALUE_CYCLE_Range*2
#define S_VALUE_RANGE 1241
#define S_VALUE_RESOLUTION 20		//�ź��ܼ����
#define S_VALUE_CENTERLINE_SET_MIN_MAX 30 //����״̬���ź������߿�������ֵ
#define S_VALUE_CENTERLINE_SET_MAX_MAX 200 //����״̬���ź������߿�������ֵ
#define S_VALUE_CENTERLINE_COUNT 20 //����
#define S_VALUE_MAX	180					//
#define  LED_PERIOD	2000				//led��������
#define LED_SUBDIVIDE	100	//ledƵ�ʷֱ���
#define S_VALUE_COUNT_MIN 0
#define S_VALUE_COUNT_MIN_SET 1

#define CH_IS_DIS 0xFF
#define SET_MIN 0U
#define SET_MAX 1U

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
