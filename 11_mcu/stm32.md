# stm32f407 freertos hal

## gpio

### gpio的8中方式

输出

- 推挽输出(可以输出高/低电平 驱动能力强)
- 开漏输出(只能输出低电平，需要上拉电阻拉高电平)
- 复用推挽输出(主要用于较强驱动能力的外设接口，如uart iic spi can。能够输出高低电平)
- 复用开漏输出(适合多个设备共享同一条信号线，实现"线与"逻辑。如iic总线通信)

输入（可为输入模式配置中断）

- 浮空输入(没有上下拉电阻容易收到外部干扰)
- 上拉输入(内部连接到电源，由上拉电阻将引脚上拉到高电平，默认输入高电平，只能被外部拉低)
- 下拉输入(内部连接到地，由下拉电阻将引脚下拉到低电平，默认输入高电平，只能被外部拉低)
- 模拟输入(用于模拟信号的输入 引脚在模拟模式下不会涉及任何数字输入或输出配置)

### 使用寄存器控制gpio

共4大步骤
1. 使能时钟
2. 设置gpio模式  输入/输出/复用/模拟
3. 设置输出模式
4. 设置速率(输出才设置)
5. 设置上拉下拉电阻
6. 设置电平值(输出才设置)


寄存器方式
```c
/* 片上外设基地址
*/
#define PERIPH_BASE ((unsigned int)0x40000000)
/* 总线基地址 */
#define AHB1PERIPH_BASE (PERIPH_BASE + 0x00020000)
/*GPIO 外设基地址 */
#define GPIOF_BASE (AHB1PERIPH_BASE + 0x1400)
/* GPIOF 寄存器地址, 强制转换成指针 */
#define GPIOF_MODER *(unsigned int*)(GPIOF_BASE+0x00)
#define GPIOF_OTYPER *(unsigned int*)(GPIOF_BASE+0x04)
#define GPIOF_OSPEEDR *(unsigned int*)(GPIOF_BASE+0x08)
#define GPIOF_PUPDR *(unsigned int*)(GPIOF_BASE+0x0C)
#define GPIOF_IDR *(unsigned int*)(GPIOF_BASE+0x10)
#define GPIOF_ODR *(unsigned int*)(GPIOF_BASE+0x14)
#define GPIOF_BSRR *(unsigned int*)(GPIOF_BASE+0x18)
#define GPIOF_LCKR *(unsigned int*)(GPIOF_BASE+0x1C)
#define GPIOF_AFRL *(unsigned int*)(GPIOF_BASE+0x20)
#define GPIOF_AFRH *(unsigned int*)(GPIOF_BASE+0x24)
/*RCC 外设基地址 */
#define RCC_BASE (AHB1PERIPH_BASE + 0x3800)
/*RCC 的 AHB1 时钟使能寄存器地址, 强制转换成指针 */
#define RCC_AHB1ENR *(unsigned int*)(RCC_BASE+0x30)


/* 开启 GPIOF 时钟，使用外设时都要先开启它的时钟 */
RCC_AHB1ENR |= (1<<5);
/* LED 端口初始化 */
/*GPIOF MODER6 清空 */
GPIOF_MODER
&= ~( 0x03<< (2*6));
/*PF6 MODER6 = 01b 输出模式 */
GPIOF_MODER |= (1<<2*6);
/*GPIOF OTYPER6 清空 */
GPIOF_OTYPER &= ~(1<<1*6);
/*PF6 OTYPER6 = 0b 推挽模式 */
GPIOF_OTYPER |= (0<<1*6);
/*GPIOF OSPEEDR6 清空 */
GPIOF_OSPEEDR &= ~(0x03<<2*6);
/*PF6 OSPEEDR6 = 0b 速率 2MHz*/
GPIOF_OSPEEDR |= (0<<2*6);
/*GPIOF PUPDR6 清空 */
GPIOF_PUPDR &= ~(0x03<<2*6);
/*PF6 PUPDR6 = 01b 上拉模式 */
GPIOF_PUPDR |= (1<<2*6);
/*PF6 BSRR 寄存器的 BR6 置 1，使引脚输出低电平 */
GPIOF_BSRR |= (1<<16<<6);
/*PF6 BSRR 寄存器的 BS6 置 1，使引脚输出高电平 */
//GPIOF_BSRR |= (1<<6);
```
HAL api解析
```c

typedef struct
{
  uint32_t Pin;       /* 设置引脚 可以设置多个 */
  uint32_t Mode;      /* 引脚模式 输入 输出推挽/开漏  复用推挽/开漏  复用功能 中断*/
  uint32_t Pull;      /* 上拉下拉 */
  uint32_t Speed;     /* 输出 */

  uint32_t Alternate;  /*  引脚复用功能  UART、I2C、SPI等*/
}GPIO_InitTypeDef;

/* hal中所有的gpio被GPIO_TypeDef抽象，GPIO_TypeDef指针指向gpio寄存器地址 */
typedef struct
{
  __IO uint32_t MODER;    /* mode register 配置 GPIO 引脚的工作模式（输入模式、输出模式、复用功能模式或模拟模式）        */
  __IO uint32_t OTYPER;   /* output type register 配置 GPIO 引脚的输出类型（推挽输出或开漏输出）                       */
  __IO uint32_t OSPEEDR;  /* output speed register 配置 GPIO 引脚的输出速度（低速、中速、高速或非常高速）               */
  __IO uint32_t PUPDR;    /* pull-up/pull-down register 配置 GPIO 引脚的上拉或下拉电阻                                  */
  __IO uint32_t IDR;      /* input data register 读取 GPIO 引脚的输入数据                                               */
  __IO uint32_t ODR;      /* output data register  写入 GPIO 引脚的输出数据                                            */
  __IO uint32_t BSRR;     /* bit set/reset register 设置或复位 GPIO 引脚（原子操作）                                    */
  __IO uint32_t LCKR;     /* configuration lock register 锁定 GPIO 引脚的配置，防止修改                                  */
  __IO uint32_t AFR[2];   /* alternate function registers 配置 GPIO 引脚的复用功能（用于外设，如 UART、SPI、I2C 等）。AFR[0] 对应引脚 0-7，AFR[1] 对应引脚 8-15*/
} GPIO_TypeDef;  
#define GPIOB ((GPIO_TypeDef *) 0x40020400)

void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  //使能gpio时钟
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_RESET);
  /*Configure GPIO pins : PF6 PF7 PF8 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 输出推挽
  GPIO_InitStruct.Pull = GPIO_NOPULL;  // 没有上拉下拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  // 低速
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

}

void HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
  uint32_t position;
  uint32_t ioposition = 0x00U;
  uint32_t iocurrent = 0x00U;
  uint32_t temp = 0x00U;

  /* 一些必要的检查 */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Init->Pin));
  assert_param(IS_GPIO_MODE(GPIO_Init->Mode));

  /* 遍历所有引脚 */
  for(position = 0U; position < GPIO_NUMBER; position++)
  {

    // 通过掩码运算 计算需要初始化的引脚
    ioposition = 0x01U << position;
    iocurrent = (uint32_t)(GPIO_Init->Pin) & ioposition;
    if(iocurrent == ioposition)
    {
      /*--------------------- GPIO Mode Configuration ------------------------*/
      /* 如果是输出或者复用引脚   配置：速度 输出类型*/
      if(((GPIO_Init->Mode & GPIO_MODE) == MODE_OUTPUT) || \
          (GPIO_Init->Mode & GPIO_MODE) == MODE_AF)  //MODE_AF --> Alternate Function  复用功能
      {
        /* Check the Speed parameter */
        assert_param(IS_GPIO_SPEED(GPIO_Init->Speed));
        /* 设置引脚速度 */
        temp = GPIOx->OSPEEDR; 
        temp &= ~(GPIO_OSPEEDER_OSPEEDR0 << (position * 2U));
        temp |= (GPIO_Init->Speed << (position * 2U));
        GPIOx->OSPEEDR = temp;

        /* 设置输出类型 推挽开漏*/
        temp = GPIOx->OTYPER;
        temp &= ~(GPIO_OTYPER_OT_0 << position) ;
        temp |= (((GPIO_Init->Mode & OUTPUT_TYPE) >> OUTPUT_TYPE_Pos) << position);
        GPIOx->OTYPER = temp;
       }

      // 如果不是模拟复用引脚  则需要设置上拉下拉电阻
      if((GPIO_Init->Mode & GPIO_MODE) != MODE_ANALOG)
      {
        /* Check the parameters */
        assert_param(IS_GPIO_PULL(GPIO_Init->Pull));
        
        /* Activate the Pull-up or Pull down resistor for the current IO */
        temp = GPIOx->PUPDR;
        temp &= ~(GPIO_PUPDR_PUPDR0 << (position * 2U));
        temp |= ((GPIO_Init->Pull) << (position * 2U));
        GPIOx->PUPDR = temp;
      }

      /* 复用引脚 选择复用到哪些控制器 */
      if((GPIO_Init->Mode & GPIO_MODE) == MODE_AF)
      {
        /* Check the Alternate function parameter */
        assert_param(IS_GPIO_AF(GPIO_Init->Alternate));
        /* Configure Alternate function mapped with the current IO */
        temp = GPIOx->AFR[position >> 3U];
        temp &= ~(0xFU << ((uint32_t)(position & 0x07U) * 4U)) ;
        temp |= ((uint32_t)(GPIO_Init->Alternate) << (((uint32_t)position & 0x07U) * 4U));
        GPIOx->AFR[position >> 3U] = temp;
      }

      /* 配置引脚模式 (Input, Output, Alternate or Analog) */
      temp = GPIOx->MODER;
      temp &= ~(GPIO_MODER_MODER0 << (position * 2U));
      temp |= ((GPIO_Init->Mode & GPIO_MODE) << (position * 2U));
      GPIOx->MODER = temp;

      /*--------------------- 外部中断配置 ------------------------*/
      if((GPIO_Init->Mode & EXTI_MODE) != 0x00U)
      {
        /* 外部中断需要使用系统配置 启用系统配置时钟以启用外部中断 */
        __HAL_RCC_SYSCFG_CLK_ENABLE();

        /* 基于位置和引脚配置 */
        temp = SYSCFG->EXTICR[position >> 2U];
        temp &= ~(0x0FU << (4U * (position & 0x03U)));
        temp |= ((uint32_t)(GPIO_GET_INDEX(GPIOx)) << (4U * (position & 0x03U)));
        SYSCFG->EXTICR[position >> 2U] = temp;

        /* 设置上升沿触发  */
        temp = EXTI->RTSR;
        temp &= ~((uint32_t)iocurrent);
        if((GPIO_Init->Mode & TRIGGER_RISING) != 0x00U)
        {
          temp |= iocurrent;
        }
        EXTI->RTSR = temp;

        /* 设置下降沿触发 */
        temp = EXTI->FTSR;
        temp &= ~((uint32_t)iocurrent);
        if((GPIO_Init->Mode & TRIGGER_FALLING) != 0x00U)
        {
          temp |= iocurrent;
        }
        EXTI->FTSR = temp;

        temp = EXTI->EMR;
        temp &= ~((uint32_t)iocurrent);
        if((GPIO_Init->Mode & EXTI_EVT) != 0x00U)
        {
          temp |= iocurrent;
        }
        EXTI->EMR = temp;

        /* stm32默认屏蔽所有中断  清除位以开启中断 */
        temp = EXTI->IMR;
        temp &= ~((uint32_t)iocurrent);
        if((GPIO_Init->Mode & EXTI_IT) != 0x00U)
        {
          temp |= iocurrent;
        }
        EXTI->IMR = temp;
      }
    }
  }
}

```


## uart

### 结构体与初始化

uart在stm32上的传输机制

1. **数据寄存器**：STM32的UART模块有一个数据寄存器（通常是 `USART_DR` 寄存器）。当CPU写入数据到该寄存器时，UART模块会自动检测到数据寄存器中的数据改变。
2. **发送数据**：当数据寄存器（`USART_DR`）被写入数据时，UART模块会自动开始发送数据。这时，数据会从数据寄存器被移到发送移位寄存器（Transmit Shift Register, TSR），并通过串行输出引脚（TX）发送出去。
3. **发送完成标志**：
   - UART模块会设置一个发送数据寄存器空（Transmit Data Register Empty, TXE）标志，表示数据寄存器已经空，可以写入新的数据。
   - 还有一个发送完成（Transmission Complete, TC）标志，它表示所有数据已经发送完毕，包括发送移位寄存器中的数据。
4. **中断和DMA**：
   - UART模块可以配置为使用中断或DMA（直接存储器访问）方式来管理数据传输。
   - 当TXE标志被置位时，会触发一个中断，通知CPU可以写入新的数据到数据寄存器。
   - 如果使用DMA，DMA控制器会自动将数据从内存传输到UART数据寄存器，无需CPU干预。


```c
/* uart寄存器操作结构体 */
typedef struct
{
  __IO uint32_t SR;         /* Status Register 用于指示USART的当前状态和发生的事件。例如，发送完成、中断标志等。 */
  __IO uint32_t DR;         /* Data Register 用于存储发送或接收的数据。写入数据时，将数据写入DR寄存器；读取数据时，从DR寄存器读取接收到的数据。*/
  __IO uint32_t BRR;        /* Baud Rate Register 用于配置USART的波特率*/
  __IO uint32_t CR1;        /* Control Register1  用于配置USART的基本功能，如使能USART、数据字长、奇偶校验、发送和接收使能等*/
  __IO uint32_t CR2;        /* Control Register2 用于配置USART的模式和功能，如停止位数、时钟使能、LIN模式等*/
  __IO uint32_t CR3;        /* Control Register3 用于配置USART的高级功能，如DMA使能、错误中断使能、流控制等 */
  __IO uint32_t GTPR;       /*Guard time and Prescaler Register 用于配置守卫时间和预分频器值，主要用于智能卡模式*/
} USART_TypeDef;

/* uart句柄 */
typedef struct __UART_HandleTypeDef
{
  USART_TypeDef                 *Instance;        /* UART 基地址        */
  UART_InitTypeDef              Init;             /* uart通信配置参数 波特率 数据位等      */
  const uint8_t                 *pTxBuffPtr;      /* 发送缓冲区 */
  uint16_t                      TxXferSize;       /* 发送大小            */
  __IO uint16_t                 TxXferCount;      /*!< UART Tx Transfer Counter           */
  uint8_t                       *pRxBuffPtr;      /*!< Pointer to UART Rx transfer Buffer */
  uint16_t                      RxXferSize;       /*!< UART Rx Transfer size              */
  __IO uint16_t                 RxXferCount;      /*!< UART Rx Transfer Counter           */
  __IO HAL_UART_RxTypeTypeDef ReceptionType;      /*!< Type of ongoing reception          */
  __IO HAL_UART_RxEventTypeTypeDef RxEventType;   /*!< Type of Rx Event                   */
  DMA_HandleTypeDef             *hdmatx;          /*!< UART Tx DMA Handle parameters      */
  DMA_HandleTypeDef             *hdmarx;          /*!< UART Rx DMA Handle parameters      */
  HAL_LockTypeDef               Lock;             /*!< Locking object                     */
  __IO HAL_UART_StateTypeDef    gState;           /*!< UART state information related to global Handle management
                                                       and also related to Tx operations.
                                                       This parameter can be a value of @ref HAL_UART_StateTypeDef */
  __IO HAL_UART_StateTypeDef    RxState;          /*!< UART state information related to Rx operations.
                                                       This parameter can be a value of @ref HAL_UART_StateTypeDef */
  __IO uint32_t                 ErrorCode;        /*!< UART Error code                    */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  void (* TxHalfCpltCallback)(struct __UART_HandleTypeDef *huart);        /*!< UART Tx Half Complete Callback        */
  void (* TxCpltCallback)(struct __UART_HandleTypeDef *huart);            /*!< UART Tx Complete Callback             */
  void (* RxHalfCpltCallback)(struct __UART_HandleTypeDef *huart);        /*!< UART Rx Half Complete Callback        */
  void (* RxCpltCallback)(struct __UART_HandleTypeDef *huart);            /*!< UART Rx Complete Callback             */
  void (* ErrorCallback)(struct __UART_HandleTypeDef *huart);             /*!< UART Error Callback                   */
  void (* AbortCpltCallback)(struct __UART_HandleTypeDef *huart);         /*!< UART Abort Complete Callback          */
  void (* AbortTransmitCpltCallback)(struct __UART_HandleTypeDef *huart); /*!< UART Abort Transmit Complete Callback */
  void (* AbortReceiveCpltCallback)(struct __UART_HandleTypeDef *huart);  /*!< UART Abort Receive Complete Callback  */
  void (* WakeupCallback)(struct __UART_HandleTypeDef *huart);            /*!< UART Wakeup Callback                  */
  void (* RxEventCallback)(struct __UART_HandleTypeDef *huart, uint16_t Pos); /*!< UART Reception Event Callback     */
  void (* MspInitCallback)(struct __UART_HandleTypeDef *huart);           /*!< UART Msp Init callback                */
  void (* MspDeInitCallback)(struct __UART_HandleTypeDef *huart);         /*!< UART Msp DeInit callback              */
#endif  /* USE_HAL_UART_REGISTER_CALLBACKS */
} UART_HandleTypeDef;


```

```c
void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200; // 波特率
  huart1.Init.WordLength = UART_WORDLENGTH_8B; // 数据位
  huart1.Init.StopBits = UART_STOPBITS_1; // 停止位
  huart1.Init.Parity = UART_PARITY_NONE; // 检验
  huart1.Init.Mode = UART_MODE_TX_RX;  // uart输入输出模式
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;  // uart硬件控制流
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;  // 过采样倍数
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
{
  /* 必要的检查 */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

  /* 有的设备的uart硬件控制流不可使用 */
  if (huart->Init.HwFlowCtl != UART_HWCONTROL_NONE)
  {
    /* The hardware flow control is available only for USART1, USART2, USART3 and USART6.
       Except for STM32F446xx devices, that is available for USART1, USART2, USART3, USART6, UART4 and UART5.
    */
    assert_param(IS_UART_HWFLOW_INSTANCE(huart->Instance));
    assert_param(IS_UART_HARDWARE_FLOW_CONTROL(huart->Init.HwFlowCtl));
  }
  else
  {
    assert_param(IS_UART_INSTANCE(huart->Instance));
  }
    /* 检查数据位  过采样是否正确 */
  assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
  assert_param(IS_UART_OVERSAMPLING(huart->Init.OverSampling));

    // uart处于空闲状态
  if (huart->gState == HAL_UART_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    huart->Lock = HAL_UNLOCKED;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = HAL_UART_MspInit;
    }

    /* Init the low level hardware */
    huart->MspInitCallback(huart);
#else
    /* Init the low level hardware : GPIO, CLOCK */
    HAL_UART_MspInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = HAL_UART_STATE_BUSY;

  /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);

  /* Set the UART Communication parameters */
  UART_SetConfig(huart);

  /* 异步模式下 一下寄存器位应该被清除
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  /* 使能uart */
  __HAL_UART_ENABLE(huart);

  /* 初始化uart句柄状态 */
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;
  huart->RxEventType = HAL_UART_RXEVENT_TC;

  return HAL_OK;
}


```

### block模式

接收数据，阻塞模式下的串口传输本质上是循环发送，占用cpu时钟

```c

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  const uint8_t  *pdata8bits;
  const uint16_t *pdata16bits;
  uint32_t tickstart = 0U;

  /* Check that a Tx process is not already ongoing */
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

    /* Init tickstart for timeout management */
    tickstart = HAL_GetTick();

    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

    /* In case of 9bits/No Parity transfer, pData needs to be handled as a uint16_t pointer */
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (const uint16_t *) pData;
    }
    else
    {
      pdata8bits  = pData;
      pdata16bits = NULL;
    }

    while (huart->TxXferCount > 0U)
    {
      if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
      {
        huart->gState = HAL_UART_STATE_READY;

        return HAL_TIMEOUT;
      }
      if (pdata8bits == NULL)
      {
        huart->Instance->DR = (uint16_t)(*pdata16bits & 0x01FFU);
        pdata16bits++;
      }
      else
      {
        huart->Instance->DR = (uint8_t)(*pdata8bits & 0xFFU);
        pdata8bits++;
      }
      huart->TxXferCount--;
    }

    if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TC, RESET, tickstart, Timeout) != HAL_OK)
    {
      huart->gState = HAL_UART_STATE_READY;

      return HAL_TIMEOUT;
    }

    /* At end of Tx process, restore huart->gState to Ready */
    huart->gState = HAL_UART_STATE_READY;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
```

接收数据同样是如此，采用轮询的方式，暂用cpu

```c
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint8_t  *pdata8bits;
  uint16_t *pdata16bits;
  uint32_t tickstart = 0U;

  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    /* Init tickstart for timeout management */
    tickstart = HAL_GetTick();

    huart->RxXferSize = Size;
    huart->RxXferCount = Size;

    /* In case of 9bits/No Parity transfer, pRxData needs to be handled as a uint16_t pointer */
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (uint16_t *) pData;
    }
    else
    {
      pdata8bits  = pData;
      pdata16bits = NULL;
    }

    /* Check the remain data to be received */
    while (huart->RxXferCount > 0U)
    {
      if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
      {
        huart->RxState = HAL_UART_STATE_READY;

        return HAL_TIMEOUT;
      }
      if (pdata8bits == NULL)
      {
        *pdata16bits = (uint16_t)(huart->Instance->DR & 0x01FF);
        pdata16bits++;
      }
      else
      {
        if ((huart->Init.WordLength == UART_WORDLENGTH_9B) || ((huart->Init.WordLength == UART_WORDLENGTH_8B) && (huart->Init.Parity == UART_PARITY_NONE)))
        {
          *pdata8bits = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
        }
        else
        {
          *pdata8bits = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
        }
        pdata8bits++;
      }
      huart->RxXferCount--;
    }

    /* At end of Rx process, restore huart->RxState to Ready */
    huart->RxState = HAL_UART_STATE_READY;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
```



### interrupt模式

发送时，会调用__HAL_UART_ENABLE_IT(huart, UART_IT_TXE);函数启用TXE(数据寄存器空)的中断，当数据为空，则 触发中断

```c
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
{
  /* Check that a Tx process is not already ongoing */
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }
	
      // 设置数据传输的状态，所以在重复调用时  HAL会丢弃调之前没有传输的数据
    huart->pTxBuffPtr = pData;
    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

    /* 使能寄存器空中断 */
    __HAL_UART_ENABLE_IT(huart, UART_IT_TXE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

// 触发全局的中断处理函数
void HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
{
  uint32_t isrflags   = READ_REG(huart->Instance->SR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t cr3its     = READ_REG(huart->Instance->CR3);
  uint32_t errorflags = 0x00U;
  uint32_t dmarequest = 0x00U;

...

  //检查  TXE标志  执行数据传输
  if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
    UART_Transmit_IT(huart);
    return;
  }

  /* UART in mode Transmitter end --------------------------------------------*/
  if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
    UART_EndTransmit_IT(huart);
    return;
  }
}

// 中断方式的数据传输 检查数据位 然后给数据寄存器赋值  完成发送
static HAL_StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart)
{
  const uint16_t *tmp;

  /* Check that a Tx process is ongoing */
  if (huart->gState == HAL_UART_STATE_BUSY_TX)
  {
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      tmp = (const uint16_t *) huart->pTxBuffPtr;
      huart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
      huart->pTxBuffPtr += 2U;
    }
    else
    {
      huart->Instance->DR = (uint8_t)(*huart->pTxBuffPtr++ & (uint8_t)0x00FF);
    }

      
    // 传输完成  关闭串口 TXE中断
    if (--huart->TxXferCount == 0U)
    {
      /* Disable the UART Transmit Data Register Empty Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

      /* 启动串口传输完成中断 然后如果设置了回调则执行回调 */
      __HAL_UART_ENABLE_IT(huart, UART_IT_TC);
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
```



接收和发送类似

```c
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
...
    return (UART_Start_Receive_IT(huart, pData, Size));
...
}

HAL_StatusTypeDef UART_Start_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  huart->pRxBuffPtr = pData;
  huart->RxXferSize = Size;
  huart->RxXferCount = Size;

  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->RxState = HAL_UART_STATE_BUSY_RX;

  if (huart->Init.Parity != UART_PARITY_NONE)
  {
    /* 如果设置了奇偶校验 则启用奇偶校验错误中断 */
    __HAL_UART_ENABLE_IT(huart, UART_IT_PE);
  }

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);

  /* 启用数据寄存器不空中断 */
  __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);

  return HAL_OK;
}
// 在中断处理函数调用UART_Receive_IT函数
void HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
{
  uint32_t isrflags   = READ_REG(huart->Instance->SR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t cr3its     = READ_REG(huart->Instance->CR3);
  uint32_t errorflags = 0x00U;
  uint32_t dmarequest = 0x00U;

  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if (errorflags == RESET)
  {
    /* UART in mode Receiver -------------------------------------------------*/
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      UART_Receive_IT(huart);
      return;
    }
  }
    ···
}


static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart)
{
  uint8_t  *pdata8bits;
  uint16_t *pdata16bits;

  /* 持续将数据位上的字节赋值到buf上 并且调用完成回调函数 RxCpltCallback*/
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (uint16_t *) huart->pRxBuffPtr;
      *pdata16bits = (uint16_t)(huart->Instance->DR & (uint16_t)0x01FF);
      huart->pRxBuffPtr += 2U;
    }
    else
    {
      pdata8bits = (uint8_t *) huart->pRxBuffPtr;
      pdata16bits  = NULL;

      if ((huart->Init.WordLength == UART_WORDLENGTH_9B) || ((huart->Init.WordLength == UART_WORDLENGTH_8B) && (huart->Init.Parity == UART_PARITY_NONE)))
      {
        *pdata8bits = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
      }
      else
      {
        *pdata8bits = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
      }
      huart->pRxBuffPtr += 1U;
    }
	
      // 传输完成后关闭一些中断  然后
    if (--huart->RxXferCount == 0U)
    {
      /* Disable the UART Data Register not empty Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

      /* Disable the UART Parity Error Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
      __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

      /* Rx process is completed, restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;

      /* Initialize type of RxEvent to Transfer Complete */
      huart->RxEventType = HAL_UART_RXEVENT_TC;

      /* Check current reception Mode :
         If Reception till IDLE event has been selected : */
      if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      {
        /* Set reception type to Standard */
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        /* Disable IDLE interrupt */
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

        /* Check if IDLE flag is set */
        if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))
        {
          /* Clear IDLE flag in ISR */
          __HAL_UART_CLEAR_IDLEFLAG(huart);
        }

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx Event callback*/
        huart->RxEventCallback(huart, huart->RxXferSize);
#else
        /*Call legacy weak Rx Event callback*/
        HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }
      else
      {
        /* 接收完成时 调用回调 */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx complete callback*/
        huart->RxCpltCallback(huart);
#else
        /*Call legacy weak Rx complete callback*/
        HAL_UART_RxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }

      return HAL_OK;
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

// 这是接收完成回调的原型  我们可以在自己的文件中重新实现它以覆盖它
// 或者 使能USE_HAL_UART_REGISTER_CALLBACKS 并且调用HAL_UART_RegisterCallback()注册回调函数
__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
}

```



### dma模式

#### dma初始化

```c
#define __HAL_LINKDMA(__HANDLE__, __PPP_DMA_FIELD__, __DMA_HANDLE__)               \
                        do{                                                      \
                              (__HANDLE__)->__PPP_DMA_FIELD__ = &(__DMA_HANDLE__); \
                              (__DMA_HANDLE__).Parent = (__HANDLE__);             \
                          } while(0U)
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA2_Stream2;
    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }
    // 将uart的hdmarx字段赋值 hdma_usart1_rx，将hdma_usart1_rx的parent字段赋值hdmarx。以至于使它们相关联
    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA2_Stream7;
    hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);
  }
}

```

#### DMA方式发送数据

```C
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
{
  const uint32_t *tmp;

  /* Check that a Tx process is not already ongoing */
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }
	
      // 同样的初始化的缓冲区设置，使得可以自动结束
    huart->pTxBuffPtr = pData;
    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

    /* 设置dma传输完成回调函数 在这个回调中，会执行HAL_UART_TxCpltCallback uart传输完成回调 */
    huart->hdmatx->XferCpltCallback = UART_DMATransmitCplt;

    /* 设置dma半完成回调 */
    huart->hdmatx->XferHalfCpltCallback = UART_DMATxHalfCplt;

	// 设置dma错误 abort回调
    huart->hdmatx->XferErrorCallback = UART_DMAError;
    huart->hdmatx->XferAbortCallback = NULL;

    /* 使能dma */
    tmp = (const uint32_t *)&pData;
    // 用于启动 DMA 传输，并指定传输的源地址、目标地址以及数据大小
    HAL_DMA_Start_IT(huart->hdmatx, *(const uint32_t *)tmp, (uint32_t)&huart->Instance->DR, Size);

    /* Clear the TC flag in the SR register by writing 0 to it */
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TC);

    /* Enable the DMA transfer for transmit request by setting the DMAT bit
       in the UART CR3 register */
    ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
static void UART_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  /* DMA 普通模式*/
  if ((hdma->Instance->CR & DMA_SxCR_CIRC) == 0U)
  {
    huart->TxXferCount = 0x00U;

    /* Disable the DMA transfer for transmit request by setting the DMAT bit
       in the UART CR3 register */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    /* Enable the UART Transmit Complete Interrupt */
    ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);

  }
  /* DMA 循环模式 */
  else
  {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /*Call registered Tx complete callback*/
    huart->TxCpltCallback(huart);
#else
    /*Call legacy weak Tx complete callback*/
    HAL_UART_TxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }
}


// DMA中断处理函数
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma)
{
  
  /* calculate DMA base and stream number */
  DMA_Base_Registers *regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;

  tmpisr = regs->ISR;

  ...

  /* DMA半完成处理 当数据传输到一半，会触发半完成中断******************************/
  if ((tmpisr & (DMA_FLAG_HTIF0_4 << hdma->StreamIndex)) != RESET)
  {
    if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_HT) != RESET)
    {
      /* Clear the half transfer complete flag */
      regs->IFCR = DMA_FLAG_HTIF0_4 << hdma->StreamIndex;
      
      /* Multi_Buffering mode enabled */
      if(((hdma->Instance->CR) & (uint32_t)(DMA_SxCR_DBM)) != RESET)
      {
        /* Current memory buffer used is Memory 0 */
        if((hdma->Instance->CR & DMA_SxCR_CT) == RESET)
        {
          if(hdma->XferHalfCpltCallback != NULL)
          {
            /* Half transfer callback */
            hdma->XferHalfCpltCallback(hdma);
          }
        }
        /* Current memory buffer used is Memory 1 */
        else
        {
          if(hdma->XferM1HalfCpltCallback != NULL)
          {
            /* Half transfer callback */
            hdma->XferM1HalfCpltCallback(hdma);
          }
        }
      }
      else
      {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if((hdma->Instance->CR & DMA_SxCR_CIRC) == RESET)
        {
          /* Disable the half transfer interrupt */
          hdma->Instance->CR  &= ~(DMA_IT_HT);
        }
        
        if(hdma->XferHalfCpltCallback != NULL)
        {
          /* Half transfer callback */
          hdma->XferHalfCpltCallback(hdma);
        }
      }
    }
  }
  /* DMA传输完成处理 ***********************************/
  if ((tmpisr & (DMA_FLAG_TCIF0_4 << hdma->StreamIndex)) != RESET)
  {
    if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_TC) != RESET)
    {
      /* Clear the transfer complete flag */
      regs->IFCR = DMA_FLAG_TCIF0_4 << hdma->StreamIndex;
      
      if(HAL_DMA_STATE_ABORT == hdma->State)
      {
        /* Disable all the transfer interrupts */
        hdma->Instance->CR  &= ~(DMA_IT_TC | DMA_IT_TE | DMA_IT_DME);
        hdma->Instance->FCR &= ~(DMA_IT_FE);
        
        if((hdma->XferHalfCpltCallback != NULL) || (hdma->XferM1HalfCpltCallback != NULL))
        {
          hdma->Instance->CR  &= ~(DMA_IT_HT);
        }

        /* Clear all interrupt flags at correct offset within the register */
        regs->IFCR = 0x3FU << hdma->StreamIndex;

        /* Change the DMA state */
        hdma->State = HAL_DMA_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hdma);

        if(hdma->XferAbortCallback != NULL)
        {
          hdma->XferAbortCallback(hdma);
        }
        return;
      }

      if(((hdma->Instance->CR) & (uint32_t)(DMA_SxCR_DBM)) != RESET)
      {
        /* Current memory buffer used is Memory 0 */
        if((hdma->Instance->CR & DMA_SxCR_CT) == RESET)
        {
          if(hdma->XferM1CpltCallback != NULL)
          {
            /* Transfer complete Callback for memory1 */
            hdma->XferM1CpltCallback(hdma);
          }
        }
        /* Current memory buffer used is Memory 1 */
        else
        {
          if(hdma->XferCpltCallback != NULL)
          {
            /* 调用dma传输完成回调 */
            hdma->XferCpltCallback(hdma);
          }
        }
      }
```

#### DMA方式接收数据

```c
HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

  
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
	
     // 检查一些状态后调用UART_Start_Receive_DMA
    return (UART_Start_Receive_DMA(huart, pData, Size));
  }
  else
  {
    return HAL_BUSY;
  }
}

// 这部分和DMA发送类似  设置半完成 完成回调  使能dma
HAL_StatusTypeDef UART_Start_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  uint32_t *tmp;

  huart->pRxBuffPtr = pData;
  huart->RxXferSize = Size;

  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->RxState = HAL_UART_STATE_BUSY_RX;

// 设置半完成 完成 错误 终止回调
  huart->hdmarx->XferCpltCallback = UART_DMAReceiveCplt;
  huart->hdmarx->XferHalfCpltCallback = UART_DMARxHalfCplt;
  huart->hdmarx->XferErrorCallback = UART_DMAError;
  huart->hdmarx->XferAbortCallback = NULL;

  /* 启动dma流 */
  tmp = (uint32_t *)&pData;
  HAL_DMA_Start_IT(huart->hdmarx, (uint32_t)&huart->Instance->DR, *(uint32_t *)tmp, Size);

  /* Clear the Overrun flag just before enabling the DMA Rx request: can be mandatory for the second transfer */
  __HAL_UART_CLEAR_OREFLAG(huart);

  if (huart->Init.Parity != UART_PARITY_NONE)
  {
    /* Enable the UART Parity Error Interrupt */
    ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);
  }

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* Enable the DMA transfer for the receiver request by setting the DMAR bit
  in the UART CR3 register */
  ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
  return HAL_OK;
}

```



### idel接收

#### interrupt方式

- 调用HAL_UARTEx_ReceiveToIdle_IT以启动使用idel方式的接收，触发方式为中断
- 数据的处理方式设置的关键是 huart->ReceptionType字段  可设置为 标准模式 和 idel模式

```c
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status;

  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* Set Reception type to reception till IDLE Event*/
    huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
    huart->RxEventType = HAL_UART_RXEVENT_TC;

    status =  UART_Start_Receive_IT(huart, pData, Size);

    /* 设置接收完成的数据处理方式*/
    if (status == HAL_OK)
    {
      if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
      }
      else
      {
        /* In case of errors already pending when reception is started,
           Interrupts may have already been raised and lead to reception abortion.
           (Overrun error for instance).
           In such case Reception Type has been reset to HAL_UART_RECEPTION_STANDARD. */
        status = HAL_ERROR;
      }
    }

    return status;
  }
  else
  {
    return HAL_BUSY;
  }
}
```

####  DMA方式

```c
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status;

  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* 将接收类型设置为idel模式 */
    huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
    huart->RxEventType = HAL_UART_RXEVENT_TC;

    status =  UART_Start_Receive_DMA(huart, pData, Size);

    /* Check Rx process has been successfully started */
    if (status == HAL_OK)
    {
      if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      {
          // 清除空闲标志位 并使能空闲中断
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
      }
      else
      {
        /* In case of errors already pending when reception is started,
           Interrupts may have already been raised and lead to reception abortion.
           (Overrun error for instance).
           In such case Reception Type has been reset to HAL_UART_RECEPTION_STANDARD. */
        status = HAL_ERROR;
      }
    }

    return status;
  }

}
```







#### IDEL接收中断的处理

- 中断接收方式依赖USART_SR_RXNE(接收数据寄存器非空) , USART_CR1_RXNEIE(接收数据寄存器非空中断使能)

- 触发idel中断后进入HAL_UART_IRQHandler中断处理函数

- 之后调用UART_Receive_IT()
  - 在UART_Receive_IT()搬运完数据寄存器中的数据后，检查接收完成的处理模式
  - 根据idel还是标准模式去调用对应的RxEventCallback 和 RxCpltCallback回调





- DMA接收方式，只是在中断控制器中依赖USART_SR_RXNE(我们无法去接触到)，并且应该关闭 USART_CR1_RXNEIE
- 触发idel中断后进入HAL_UART_IRQHandler中断处理函数
- 调用idel模式下的回调

```c
void HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
{
...

  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if (errorflags == RESET)
  {
    /* UART在接收数据寄存器有数据，并且是中断接收模式 -------------------------------------------------*/
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      UART_Receive_IT(huart);
      return;
    }
  }
...
    /* 检查数据的处理方式 :
     如果是idel方式 : */
    
    /*
    HAL_UART_RECEPTION_TOIDLE 表示 UART 处于接收空闲模式（即接收到数据后等待一段时间，如果没有更多数据，则认为接收结束）。
	USART_SR_IDLE 是 USART 状态寄存器中的一个标志，表示接收线路空闲。当接收线路在一段时间内没有检测到数据时，该标志被置位。
	cr1its 应该是 USART_CR1_IDLEIE，这是 USART 控制寄存器1中的一个位，用于使能空闲中断。当设置了这个位时，USART 在空闲检测到时会触发中断。
通过与 USART_CR1_IDLEIE 进行按位与操作来检查该中断是否启用。
    */
  if ((huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      && ((isrflags & USART_SR_IDLE) != 0U)
      && ((cr1its & USART_SR_IDLE) != 0U))
  {
    __HAL_UART_CLEAR_IDLEFLAG(huart);

    /* 在DMA模式下 */
    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
    {
      /* DMA mode enabled */
      /* 检查接收长度:如果接收到所有预期的数据，不做任何操作;(DMA cplt回调将被调用)。
		否则，如果至少已经接收到一个数据，则将IDLE事件通知用户 */
      uint16_t nb_remaining_rx_data = (uint16_t) __HAL_DMA_GET_COUNTER(huart->hdmarx);
      if ((nb_remaining_rx_data > 0U)
          && (nb_remaining_rx_data < huart->RxXferSize))
      {
        /* Reception is not complete */
        huart->RxXferCount = nb_remaining_rx_data;

        /* 在正常模式下，关闭dma和uart接收*/
        if (huart->hdmarx->Init.Mode != DMA_CIRCULAR)
        {
          /* Disable PE and ERR (Frame error, noise error, overrun error) interrupts */
          ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
          ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

          /* Disable the DMA transfer for the receiver request by resetting the DMAR bit
             in the UART CR3 register */
          ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

          /* 将串口状态置为就绪 */
          huart->RxState = HAL_UART_STATE_READY;
            /* 同时将数据处理方式更改回标准方式 */
          huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
			/* 清除idel使能标志 */ 
          ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
          (void)HAL_DMA_Abort(huart->hdmarx);
        }

        /* 初始化接收事件类型 然后进行回调*/
        huart->RxEventType = HAL_UART_RXEVENT_IDLE;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        huart->RxEventCallback(huart, (huart->RxXferSize - huart->RxXferCount));
#else
        HAL_UARTEx_RxEventCallback(huart, (huart->RxXferSize - huart->RxXferCount));
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }
      return;
    }
    else
    {
    
      /* 检查接收长度:如果接收到所有期望的数据，则不做任何操作。
		否则，如果至少已经接收到一个数据，则调用IDLE事件回调函数 */
      uint16_t nb_rx_data = huart->RxXferSize - huart->RxXferCount;
      if ((huart->RxXferCount > 0U)
          && (nb_rx_data > 0U))
      {
        /* 处理数据 先关闭中断 */
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
        ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

        /* Rx process is completed, restore huart->RxState to Ready */
        huart->RxState = HAL_UART_STATE_READY;
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

        huart->RxEventType = HAL_UART_RXEVENT_IDLE;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        huart->RxEventCallback(huart, nb_rx_data);
#else
        HAL_UARTEx_RxEventCallback(huart, nb_rx_data);
#endif 
      }
      return;
    }
  }
}

// UART_Receive_IT 是中断方式下接收下的真正将数据寄存器搬运到buf中的函数。同时在这里检查接收模式 是idel还是标准的接收模式，同样
// idel接收模式回调可以通过重写weak符号的函数或者通过HAL_UART_RegisterCallback()设置huart->RxEventCallback回调
static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart)
{
  
...
    // 如果数据已经接收完成  关闭必要的中断
    if (--huart->RxXferCount == 0U)
    {
      /* Disable the UART Data Register not empty Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

      /* Disable the UART Parity Error Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
      __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

      /* Rx process is completed, restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;

      /* Initialize type of RxEvent to Transfer Complete */
      huart->RxEventType = HAL_UART_RXEVENT_TC;
     /* 如果是idel接收模式 */
      if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      {
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        /* 在处理时关闭中断 */
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

        /* 清除idel触发的标志 */
        if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))
        {
          /* Clear IDLE flag in ISR */
          __HAL_UART_CLEAR_IDLEFLAG(huart);
        }
// 调用 idel的中断
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx Event callback*/
        huart->RxEventCallback(huart, huart->RxXferSize);
#else
        /*Call legacy weak Rx Event callback*/
        HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }
      else
      {
        /* Standard reception API called */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx complete callback*/
        huart->RxCpltCallback(huart);
#else
        /*Call legacy weak Rx complete callback*/
        HAL_UART_RxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }
    }
...
}

```





## iic


## spi



## can

