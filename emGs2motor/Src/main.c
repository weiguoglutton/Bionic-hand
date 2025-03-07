/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config ( void );
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void UserInit ( void );
void PowerOut ( void );
uint8_t  GetValue ( uint16_t value[S_CHANNEL__NUM][SUBDIVIDE], uint16_t valueLen[S_CHANNEL__NUM], uint8_t sCh );
void GetPosition ( uint16_t  value[SUBDIVIDE], uint16_t valueDividing[3] );
void SetValue ( void );

uint32_t DMA_Transfer_Complete_Count = 0U;
uint8_t ADC_Flag = 0U;
uint16_t Blink_Tick = 0U;
uint16_t Blink_Status = 0U;
uint16_t Key__Tick = 0U;
uint8_t Key_Status = 0U;
uint8_t Set_Status = 0U; 							//设置是否更新Value_Max与Value_Min的值，0=是否开启录入，非0为当前录入项
uint32_t ADC_Value[S_CHANNEL__NUM];						//adc采集数组
uint16_t S_Value_CyCle_Tick = 0U, S_Value_CyCle_Set_Tick = 0U, Blink_CyCle_Tick = 0U;
uint8_t S_CH_Set = 0U;
uint16_t  S_Value_Level[S_CHANNEL__NUM][SUBDIVIDE];	//存放单轮采集周期不同能级样本数量
uint16_t Value_Dividing[S_CHANNEL__NUM][3];					//标记S_Value_Level数组有效数据位置范围
uint16_t Blink_Delay[S_CHANNEL__NUM];

uint16_t Static_S_Value_Len[S_CHANNEL__NUM];				//标记S_Value_Level数组有效数据长度,并作为是否为有效控制信号参照数据
uint16_t S_Value_Len[S_CHANNEL__NUM] = {0U, 0U};;								//信号能级宽度
uint8_t Value_Is_Ok[S_CHANNEL__NUM];
uint16_t S_value_Centerline[S_CHANNEL__NUM] = {S_VALUE_CENTERLINE_COUNT, S_VALUE_CENTERLINE_COUNT};//肌肉放松状态下的能量宽度
uint16_t S_value_Max[S_CHANNEL__NUM] = {S_VALUE_MAX, S_VALUE_MAX};					//最大力量下的信号阈值
uint16_t Speed_Level_Len[S_CHANNEL__NUM][SPEED_SUBDIVIDE];					//标记不同电机转速级别对应的肌肉信号能级
uint32_t Set_Value_Count = 0U;
uint16_t S_Value_SET_Status = -SET_MIN;

const double Static_S_value_Subdivide = 0xFFF / SUBDIVIDE;				//对adc信号进行格式化，以便进行能级标记能级宽度
const double Static_Speed_Subdivide = ( TIM_PERIOD + 1 ) / SPEED_SUBDIVIDE;//对电机转速进行格式化
double S_Speed_Subdivide =  S_VALUE_MAX  / SPEED_SUBDIVIDE;					//对最大肌肉力量下的信号能级进行格式化
double Value2Led = ( S_VALUE_MAX - S_VALUE_CENTERLINE_COUNT ) / ( LED_PERIOD / LED_SUBDIVIDE );//将肌肉信号转化为led灯闪烁显示

uint8_t CH_State = CH_IS_DIS;
uint16_t Sin2Speed[S_CHANNEL__NUM];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main ( void )
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_TIM1_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */
    UserInit();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while ( 1 )
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }

    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config ( void )
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;

    if ( HAL_RCC_OscConfig ( &RCC_OscInitStruct ) != HAL_OK )
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if ( HAL_RCC_ClockConfig ( &RCC_ClkInitStruct, FLASH_LATENCY_2 ) != HAL_OK )
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;

    if ( HAL_RCCEx_PeriphCLKConfig ( &PeriphClkInit ) != HAL_OK )
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
void UserInit ( void )
{
    double valuePosition = S_Speed_Subdivide;
    //定时器初始化
    htim1.Init.Period = TIM_PERIOD;
    HAL_ADC_Start_DMA ( &hadc1, ( uint32_t* ) &S_CH, S_CHANNEL__NUM );
    HAL_TIM_Base_Start_IT ( &htim1 );
    htim1.Instance->CCER = 0U;
    __HAL_TIM_MOE_ENABLE ( &htim1 );


    for ( uint16_t i = 0U; i < SUBDIVIDE; i++ )
    {
        for ( uint16_t j = 0U; j < S_CHANNEL__NUM; j++ )
        {
            S_Value_Level[j][i] = 0U;
        }
    }

    for ( uint16_t i = 0U; i < SPEED_SUBDIVIDE; i++ )
    {
        for ( uint16_t j = 0U; j < S_CHANNEL__NUM; j++ )
        {
            Speed_Level_Len[j][i] = ( uint16_t ) valuePosition;
        }

        valuePosition += S_Speed_Subdivide;
    }

    for ( uint16_t i = 0U; i < S_CHANNEL__NUM; i++ )
    {
        Value_Is_Ok[i] = 0U;
    }
}

void HAL_ADC_ConvCpltCallback ( ADC_HandleTypeDef* AdcHandle )
{
    ADC_Flag = 1U;
    HAL_ADC_Stop_DMA ( &hadc1 );
}

void GetPosition ( uint16_t  valueCount[SUBDIVIDE], uint16_t valueDividing[3] )
{
    //通过采集到的样本数量判断是否为毛刺，从而标记有效边界
    for ( uint16_t i = 0, j = 0, k = 0, t; i < SUBDIVIDE  ; i++ )
    {
        switch ( j )
        {
        case 0:
            if ( valueCount[i] > S_VALUE_COUNT_MIN )
            {
                valueDividing[0] = i;
                j++;

            }

            break;

        case 1:
            /*     if ( ( valueCount[i] > k ) || ( valueCount[i] < S_VALUE_CENTERLINE_COUNT ) )
                 {
                     k = valueCount[i];
                     valueDividing[1] = i;
                 }
                 else
            		*/
            j++;

            break;

        case 2:
            if ( valueCount[i] > S_VALUE_COUNT_MIN )
            {
                valueDividing[2] = i;
            }

            break;
        }
    }
}

uint8_t GetValue ( uint16_t value[S_CHANNEL__NUM][SUBDIVIDE], uint16_t valueLen[S_CHANNEL__NUM], uint8_t sCh )
{
    if ( ADC_Flag )
    {

        ADC_Flag = 0U;
        HAL_ADC_Start_DMA ( &hadc1, ( uint32_t* ) &S_CH, S_CHANNEL__NUM );
        uint8_t valueIsOk[S_CHANNEL__NUM] = {0U, 0U};//标记不同通道的遍历状态，以便缩短遍历时间

        if ( Value_Is_Ok[CHA] || Value_Is_Ok[CHB] )
        {
            //判断两个通道的信号宽度，取最大值
            uint16_t vLen = ( S_Value_Len[CHA] > S_Value_Len[CHB] ) || ( S_Value_Len[CHA] == S_Value_Len[CHB] )
                            ? S_Value_Len[CHA] : S_Value_Len[CHB];
            //判断两个通道的信号起点，取最小值
            uint16_t valueMin = ( Value_Dividing[CHA][0] < Value_Dividing[CHB][0] ) || ( Value_Dividing[CHA][0] == Value_Dividing[CHB][0] )
                                ? Value_Dividing[CHA][0] : Value_Dividing[CHB][0];
            vLen += valueMin + 2;

            if ( vLen > SUBDIVIDE )
                vLen = SUBDIVIDE;

            //判断当前要获取的通道，0XFF 为全部通道
            if ( sCh == 0XFF )
            {
                Value_Is_Ok[CHA] = 0U;
                Value_Is_Ok[CHB] = 0U;
            }
            else
                Value_Is_Ok[sCh] = 0U;

            //清空对应的信号数组备用
            for ( uint16_t i = valueMin; i < vLen ; i++ )
            {
                if ( sCh == 0XFF )
                {
                    value[CHA][i] = 0U;
                    value[CHB][i] = 0U;

                }
                else
                {
                    value[sCh][i] = 0U;
                }
            }

        }

        for ( uint16_t i = 0U, j, k; i < SUBDIVIDE ; i++ )
        {
            //j标记当前能级起点，k标记当前能级终点
            j = ( uint16_t ) ( Static_S_value_Subdivide * i );
            k = j + Static_S_value_Subdivide;

            if ( sCh == 0XFF )
            {
                if ( S_CH[CHA] + 1 > j  &&   S_CH[CHA] < k )
                {
                    value[CHA][i]++;
                    valueIsOk[CHA] = !0U;
                }

                if ( S_CH[CHB] + 1 > j &&  S_CH[CHB] < k )
                {
                    value[CHB][i]++;
                    valueIsOk[CHB] = !0U;
                }

                if ( valueIsOk[CHA] && valueIsOk[CHB] )
                {
                    valueIsOk[CHA] = 0U;
                    valueIsOk[CHB] = 0U;

                    break;
                }

            }
            else

            {
                if ( ( sCh == CHA ? S_CH[CHA] : S_CH[CHB] ) + 1 > j
                        && ( sCh == CHA ? S_CH[CHA] : S_CH[CHB] ) < k )
                {
                    value[sCh][i]++;
                    break;
                }

            }
        }

        //判断是否达到单轮周期采样次数

        if ( S_Value_CyCle_Tick > S_VALUE_CYCLE_COUNT )
        {
            if ( sCh == 0XFF )
            {
                //获取信号边界位置，并计算信号长度
                GetPosition ( value[CHA], Value_Dividing[CHA] );
                valueLen[CHA] = Value_Dividing[CHA][2] - Value_Dividing[CHA][0];
                GetPosition ( value[CHB], Value_Dividing[CHB] );
                valueLen[CHB] = Value_Dividing[CHB][2] - Value_Dividing[CHB][0];
                Value_Is_Ok[CHA] = !0U;
                Value_Is_Ok[CHB] = !0U;
            }
            else
            {
                GetPosition ( value[sCh], Value_Dividing[sCh] );
                valueLen[sCh] = Value_Dividing[sCh][2] - Value_Dividing[sCh][0];
                Value_Is_Ok[sCh] = !0U;
            }

            S_Value_CyCle_Tick = 0U;

            return !0U;
        }
        else
            return 0U;

    }

}


void PowerOut ( void )
{
    if ( GetValue ( S_Value_Level, S_Value_Len, 0xFF ) )
    {
        //设置不同通道led灯状态
        Blink_Delay[CHA] = ( uint16_t ) ( ( S_Value_Len[CHA] - S_VALUE_CENTERLINE_COUNT ) / Value2Led ) * LED_SUBDIVIDE;
        Blink_Delay[CHB] = ( uint16_t ) ( ( S_Value_Len[CHB] - S_VALUE_CENTERLINE_COUNT ) / Value2Led ) * LED_SUBDIVIDE;

        if ( Blink_CyCle_Tick < Blink_Delay[CHA] )
            HAL_GPIO_WritePin ( LED_CHA_GPIO_Port, LED_CHA_Pin, !0U );
        else
            HAL_GPIO_WritePin ( LED_CHA_GPIO_Port, LED_CHA_Pin, 0U );

        if ( Blink_CyCle_Tick < Blink_Delay[CHB] )
            HAL_GPIO_WritePin ( LED_CHB_GPIO_Port, LED_CHB_Pin, !0U );
        else
            HAL_GPIO_WritePin ( LED_CHB_GPIO_Port, LED_CHB_Pin, 0U );

            //获取当前信号对应的转速位置

        for ( uint16_t i = 0U; i < SPEED_SUBDIVIDE - 1; i++ )
        {
            if ( S_Value_Len[CHA] + 1 > Speed_Level_Len[CHA][i] && S_Value_Len[CHA] < Speed_Level_Len[CHA][i + 1] )
            {
                Sin2Speed[CHA] = i;
            }

            if ( S_Value_Len[CHB] + 1 > Speed_Level_Len[CHB][i] && S_Value_Len[CHB] < Speed_Level_Len[CHB][i + 1] )
            {
                Sin2Speed[CHB] = i;
            }
        }

            //通过判断通道信号是否大于肌肉放松状态下的宽度并且有效能级另一通道能级，从而判断当前通道操作信号是否有效
        if ( S_Value_Len[CHA] > S_value_Centerline[CHA] && Sin2Speed[CHA] > Sin2Speed[CHB] )
        {

            CH_State = CHA;
        }
        else if ( S_Value_Len[CHB] > S_value_Centerline[CHB] && Sin2Speed[CHB] > Sin2Speed[CHA] )
        {
            CH_State = CHB;
        }
        else
        {
            CH_DIS;
            CH_State = CH_IS_DIS;
        }

        if ( CH_State != CH_IS_DIS )
        {
            if ( S_Value_Len[CH_State] < S_value_Max[CH_State] )
            {
                if ( CH_State == CHA )
                {
                    O_CHA = ( uint16_t ) ( Static_Speed_Subdivide * Sin2Speed[CHA] ) ;
                    CHA_EN;
                }
                else
                {
                    O_CHB = ( uint16_t ) ( Static_Speed_Subdivide * Sin2Speed[CHB] );
                    CHB_EN;
                }

            }
            else
            {
                CH_DIS;
                CH_State = CH_IS_DIS;
            }

        }
    }

}

void SetValue ( void )
{
    GetValue ( S_Value_Level, S_Value_Len, S_CH_Set ) ;

		//获取肌肉放松和最大力量状态的最大信号宽度
    if ( S_Value_SET_Status == SET_MIN )
    {
        if ( S_Value_Len[S_CH_Set] < S_VALUE_CENTERLINE_SET_MIN_MAX )
        {
            Set_Value_Count = ( S_Value_Len[S_CH_Set] > Set_Value_Count ) ? S_Value_Len[S_CH_Set] : Set_Value_Count;

        }

    }
    else
    {
        if ( S_Value_Len[S_CH_Set] < S_VALUE_CENTERLINE_SET_MAX_MAX )
        {
            Set_Value_Count = ( S_Value_Len[S_CH_Set] > Set_Value_Count ) ? S_Value_Len[S_CH_Set] : Set_Value_Count;

        }
    }

    //判断当前是设置放松还是最大力量状态下的值
    if ( S_Value_CyCle_Set_Tick > SET__DELAY_COUNT_MIN )
    {
        if ( S_Value_SET_Status == SET_MIN )
        {
            S_value_Centerline[S_CH_Set] = Set_Value_Count;
            S_Value_SET_Status = SET_MAX;
            Set_Value_Count = 0U;
        }
    }

    if ( S_Value_CyCle_Set_Tick > SET__DELAY_COUNT_MAX )
    {
        S_Value_CyCle_Set_Tick = 0U;

        if ( Set_Value_Count > S_VALUE_MAX )
            S_value_Max[S_CH_Set] = Set_Value_Count;

        for ( uint16_t i = 0; i < SPEED_SUBDIVIDE; i++ )
        {
            Speed_Level_Len[S_CH_Set][i] = 0U;
        }

        S_Speed_Subdivide =  Set_Value_Count  / SPEED_SUBDIVIDE;

        double valuePosition = S_Speed_Subdivide;

        for ( uint16_t i = 0; i < SPEED_SUBDIVIDE; i++ )
        {
            Speed_Level_Len[S_CH_Set][i] = ( uint16_t ) valuePosition;
            valuePosition += S_Speed_Subdivide;
        }

        Set_Status = 0U;
        Set_Value_Count = 0U;
        S_Value_SET_Status = SET_MIN;
        HAL_GPIO_WritePin ( LED_CHA_GPIO_Port, LED_CHA_Pin, 0U );
        HAL_GPIO_WritePin ( LED_CHB_GPIO_Port, LED_CHB_Pin, 0U );

    }

		//设置两种肌肉状态下的led状态，设置放松状态为闪烁，最大力量为常亮，完成后熄灭led
    if ( Blink_CyCle_Tick > SET_BLINK__DELAY_COUNT )
    {
        Blink_CyCle_Tick = 0U;

        switch ( S_CH_Set )
        {
        case CHA:
            if ( S_Value_SET_Status == SET_MIN )
                HAL_GPIO_TogglePin ( LED_CHA_GPIO_Port, LED_CHA_Pin );
            else
                HAL_GPIO_WritePin ( LED_CHA_GPIO_Port, LED_CHA_Pin, !0U );


            HAL_GPIO_WritePin ( LED_CHB_GPIO_Port, LED_CHB_Pin, 0U );
            break;

        case CHB:
            if ( S_Value_SET_Status == SET_MIN )
                HAL_GPIO_TogglePin ( LED_CHB_GPIO_Port, LED_CHB_Pin );
            else
                HAL_GPIO_WritePin ( LED_CHB_GPIO_Port, LED_CHB_Pin, !0U );


            HAL_GPIO_WritePin ( LED_CHA_GPIO_Port, LED_CHA_Pin, 0U );
        }
    }

    Blink_CyCle_Tick++;
    S_Value_CyCle_Set_Tick++;
}


void HAL_TIM_PeriodElapsedCallback ( TIM_HandleTypeDef *htim )
{

    if ( htim->Instance == htim1.Instance )
    {
        Blink_CyCle_Tick++;

        if ( Blink_CyCle_Tick > LED_PERIOD )
            Blink_CyCle_Tick = 0U;

        Key__Tick++;
        S_Value_CyCle_Tick++;

        if ( Set_Status )
            SetValue();
        else
            PowerOut();

        if ( Key__Tick > KEY_DELAY_COUNT )
        {
            Key__Tick = 0;
            Key_Status = !0U;
        }

    }

}


void HAL_GPIO_EXTI_Callback ( uint16_t GPIO_Pin )
{

    if ( Key_Status )
    {
        Key_Status = 0U;

        switch ( GPIO_Pin )
        {
        //切换设置不同的通道
        case SET_Pin:
            S_CH_Set = S_CH_Set == CHA ? CHB : CHA;
            Set_Status = !0U;
            S_Value_CyCle_Set_Tick = 0U;
            break;

        case CENTERLINE_CHA_P_Pin:
            if ( S_value_Centerline[CHA] < SUBDIVIDE )
                S_value_Centerline[CHA]++;

            break;

        case CENTERLINE_CHA_D_Pin:
            if ( S_value_Centerline[CHA] > 1U )
                S_value_Centerline[CHA]--;

            break;

        case CENTERLINE_CHB_P_Pin:
            if ( S_value_Centerline[CHB] < SUBDIVIDE )
                S_value_Centerline[CHB]++;

            break;

        case CENTERLINE_CHB_D_Pin:
            if ( S_value_Centerline[CHB] > 1U )
                S_value_Centerline[CHB]--;

            break;
        }
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler ( void )
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return Status */

    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed ( uint8_t *file, uint32_t line )
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
