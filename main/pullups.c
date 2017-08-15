#include "driver/gpio.h"
#include "soc/rtc_io_reg.h"
#include "soc/io_mux_reg.h"
void gpio_enable_pull_up(gpio_num_t gpio_num)
{
    switch(gpio_num) {
        case GPIO_NUM_0:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD1_REG,RTC_IO_TOUCH_PAD1_RUE_M);
            break;
        case GPIO_NUM_2:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD2_REG,RTC_IO_TOUCH_PAD2_RUE_M);
            break;
        case GPIO_NUM_4:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG,RTC_IO_TOUCH_PAD0_RUE_M);
            break;
        case GPIO_NUM_12:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD5_REG, RTC_IO_TOUCH_PAD5_RUE_M);
            break;
        case GPIO_NUM_13:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD4_REG, RTC_IO_TOUCH_PAD4_RUE_M);
            break;
        case GPIO_NUM_14:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD6_REG, RTC_IO_TOUCH_PAD6_RUE_M);
            break;
        case GPIO_NUM_15:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD3_REG, RTC_IO_TOUCH_PAD3_RUE_M);
            break;
        case GPIO_NUM_25:
            SET_PERI_REG_MASK(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_RUE_M);
            break;
        case GPIO_NUM_26:
            SET_PERI_REG_MASK(RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_RUE_M);
            break;
        case GPIO_NUM_27:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD7_REG, RTC_IO_TOUCH_PAD7_RUE_M);
            break;
        case GPIO_NUM_32:
            SET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32P_RUE_M);
            break;
        case GPIO_NUM_33:
            SET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32N_RUE_M);
            break;
        default:
            if (GPIO_PIN_MUX_REG[gpio_num]!=0) PIN_PULLUP_EN(GPIO_PIN_MUX_REG[gpio_num]);
            break;
    }
}

void gpio_disable_pull_up(gpio_num_t gpio_num)
{
    switch(gpio_num) {
        case GPIO_NUM_0:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD1_REG,RTC_IO_TOUCH_PAD1_RUE_M);
            break;
        case GPIO_NUM_2:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD2_REG,RTC_IO_TOUCH_PAD2_RUE_M);
            break;
        case GPIO_NUM_4:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG,RTC_IO_TOUCH_PAD0_RUE_M);
            break;
        case GPIO_NUM_12:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD5_REG, RTC_IO_TOUCH_PAD5_RUE_M);
            break;
        case GPIO_NUM_13:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD4_REG, RTC_IO_TOUCH_PAD4_RUE_M);
            break;
        case GPIO_NUM_14:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD6_REG, RTC_IO_TOUCH_PAD6_RUE_M);
            break;
        case GPIO_NUM_15:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD3_REG, RTC_IO_TOUCH_PAD3_RUE_M);
            break;
        case GPIO_NUM_25:
            CLEAR_PERI_REG_MASK(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_RUE_M);
            break;
        case GPIO_NUM_26:
            CLEAR_PERI_REG_MASK(RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_RUE_M);
            break;
        case GPIO_NUM_27:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD7_REG, RTC_IO_TOUCH_PAD7_RUE_M);
            break;
        case GPIO_NUM_32:
            CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32P_RUE_M);
            break;
        case GPIO_NUM_33:
            CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32N_RUE_M);
            break;
        default:
            if (GPIO_PIN_MUX_REG[gpio_num]!=0) PIN_PULLUP_DIS(GPIO_PIN_MUX_REG[gpio_num]);
            break;
    }
}

void gpio_enable_pull_down(gpio_num_t gpio_num)
{
    switch(gpio_num) {
        case GPIO_NUM_0:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD1_REG,RTC_IO_TOUCH_PAD1_RDE_M);
            break;
        case GPIO_NUM_2:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD2_REG,RTC_IO_TOUCH_PAD2_RDE_M);
            break;
        case GPIO_NUM_4:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG,RTC_IO_TOUCH_PAD0_RDE_M);
            break;
        case GPIO_NUM_12:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD5_REG, RTC_IO_TOUCH_PAD5_RDE_M);
            break;
        case GPIO_NUM_13:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD4_REG, RTC_IO_TOUCH_PAD4_RDE_M);
            break;
        case GPIO_NUM_14:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD6_REG, RTC_IO_TOUCH_PAD6_RDE_M);
            break;
        case GPIO_NUM_15:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD3_REG, RTC_IO_TOUCH_PAD3_RDE_M);
            break;
        case GPIO_NUM_25:
            SET_PERI_REG_MASK(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_RDE_M);
            break;
        case GPIO_NUM_26:
            SET_PERI_REG_MASK(RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_RDE_M);
            break;
        case GPIO_NUM_27:
            SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD7_REG, RTC_IO_TOUCH_PAD7_RDE_M);
            break;
        case GPIO_NUM_32:
            SET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32P_RDE_M);
            break;
        case GPIO_NUM_33:
            SET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32N_RDE_M);
            break;
        default:
            if (GPIO_PIN_MUX_REG[gpio_num]!=0) PIN_PULLDWN_EN(GPIO_PIN_MUX_REG[gpio_num]);
            break;
    }
}
void gpio_disable_pull_down(gpio_num_t gpio_num)
{
    switch(gpio_num) {
        case GPIO_NUM_0:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD1_REG,RTC_IO_TOUCH_PAD1_RDE_M);
            break;
        case GPIO_NUM_2:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD2_REG,RTC_IO_TOUCH_PAD2_RDE_M);
            break;
        case GPIO_NUM_4:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG,RTC_IO_TOUCH_PAD0_RDE_M);
            break;
        case GPIO_NUM_12:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD5_REG, RTC_IO_TOUCH_PAD5_RDE_M);
            break;
        case GPIO_NUM_13:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD4_REG, RTC_IO_TOUCH_PAD4_RDE_M);
            break;
        case GPIO_NUM_14:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD6_REG, RTC_IO_TOUCH_PAD6_RDE_M);
            break;
        case GPIO_NUM_15:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD3_REG, RTC_IO_TOUCH_PAD3_RDE_M);
            break;
        case GPIO_NUM_25:
            CLEAR_PERI_REG_MASK(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_RDE_M);
            break;
        case GPIO_NUM_26:
            CLEAR_PERI_REG_MASK(RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_RDE_M);
            break;
        case GPIO_NUM_27:
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD7_REG, RTC_IO_TOUCH_PAD7_RDE_M);
            break;
        case GPIO_NUM_32:
            CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32P_RDE_M);
            break;
        case GPIO_NUM_33:
            CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32N_RDE_M);
            break;
        default:
            if (GPIO_PIN_MUX_REG[gpio_num]!=0) PIN_PULLDWN_DIS(GPIO_PIN_MUX_REG[gpio_num]);
            break;
    }
}

