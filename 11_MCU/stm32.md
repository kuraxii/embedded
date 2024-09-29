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

### block模式

hal api解析

结构体
```c
typedef struct
{
  __IO uint32_t SR;         /*!< USART Status register,                   Address offset: 0x00 */
  __IO uint32_t DR;         /*!< USART Data register,                     Address offset: 0x04 */
  __IO uint32_t BRR;        /*!< USART Baud rate register,                Address offset: 0x08 */
  __IO uint32_t CR1;        /*!< USART Control register 1,                Address offset: 0x0C */
  __IO uint32_t CR2;        /*!< USART Control register 2,                Address offset: 0x10 */
  __IO uint32_t CR3;        /*!< USART Control register 3,                Address offset: 0x14 */
  __IO uint32_t GTPR;       /*!< USART Guard time and prescaler register, Address offset: 0x18 */
} USART_TypeDef;
```

```c
void MX_USART1_UART_Init(void)
{


  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

```

### interrupt模式

### dma模式


### idel接收



## iic


## spi



## can

