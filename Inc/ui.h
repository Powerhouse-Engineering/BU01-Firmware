#ifndef UI_H
#define UI_H

#include <stdint.h>

/*
 * Simple on-device UI driven by GPIO buttons and LEDs.
 * - Long-press power to turn the device on/off.
 * - Short-press power to start the motor, subsequent presses cycle modes.
 * - Inc/Dec short-press callbacks adjust the selected level while running.
 */


#define UI_BAT_V_ON_PORT GPIOB
#define UI_BAT_V_ON_PIN GPIO_Pin_12

#define UI_POWER_BUTTON_PORT GPIOB
#define UI_POWER_BUTTON_PIN GPIO_Pin_7
#define UI_INC_BUTTON_PORT GPIOB
#define UI_INC_BUTTON_PIN GPIO_Pin_9
#define UI_DEC_BUTTON_PORT GPIOA
#define UI_DEC_BUTTON_PIN GPIO_Pin_11

#define UI_LED1_PORT GPIOB // White
#define UI_LED1_PIN GPIO_Pin_5
#define UI_LED2_PORT GPIOB // White
#define UI_LED2_PIN GPIO_Pin_3
#define UI_LED3_PORT GPIOA // White
#define UI_LED3_PIN GPIO_Pin_15
#define UI_LED_ORANGE_PORT GPIOB
#define UI_LED_ORANGE_PIN GPIO_Pin_4

#define UI_DC_IN_PORT GPIOC
#define UI_DC_IN_PIN GPIO_Pin_13
#define UI_DC_IN_ACTIVE_HIGH 1
#define UI_CHG_DONE_PORT GPIOA
#define UI_CHG_DONE_PIN GPIO_Pin_12
#define UI_CHG_DONE_ACTIVE_LOW 1

#define UI_TICK_HZ 100U
#define UI_TICK_INTERVAL (LOOP_FREQUENCY_HZ / UI_TICK_HZ)
#define UI_DEBOUNCE_TICKS 3U
#define UI_LONG_PRESS_TICKS 100U // 1 second at 100Hz
#define UI_SHORT_PRESS_MAX_TICKS (UI_LONG_PRESS_TICKS - 1U)
#define UI_SHUTDOWN_DELAY_TICKS UI_TICK_HZ // 1 second
#define UI_SOC_UPDATE_TICKS (UI_TICK_HZ / 5U)
#define UI_BLINK_PERIOD_TICKS (UI_TICK_HZ / 2U)
#define UI_SOC_MIN_CV 330U  // 3.30V per cell shown as 330 (centivolts)
#define UI_SOC_MAX_CV 420U  // 4.20V per cell shown as 420 (centivolts)
#define UI_RPM_STEP 600U
#define UI_DEFAULT_MODE_INDEX 0U
#define UI_DEFAULT_LEVEL_INDEX 0U

#define BATTERY_LEVEL_UNKNOWN 0xFF
#define BATTERY_LEVEL_LOW 20
#define BATTERY_LEVEL_HYST 3
#define BATTERY_FULL_THRESHOLD_PERCENT 95
#define BATTERY_LEDS 3


void ui_init(void);
void ui_update(void);
void ui_light_tick_fast(void);
void ui_request_shutdown(void);
void ui_set_target_rpm(uint32_t rpm);

#endif /* UI_H */
