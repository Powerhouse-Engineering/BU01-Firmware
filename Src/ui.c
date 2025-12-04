#include "ui.h"

#include "button.h"
#include "common.h"
#include "debug_uart.h"
#include "dshot.h"
#include "functions.h"
#include "main.h"
#include "targets.h"

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
#define UI_RPM_STEP 100U

extern uint8_t drive_by_rpm;
extern char use_speed_control_loop;
extern uint16_t newinput;
extern char dshot;
extern char servoPwm;
extern char inputSet;
extern uint16_t signaltimeout;
extern uint32_t tenkhzcounter;
extern int32_t input_override;
extern fastPID speedPid;
extern uint8_t running;
extern char armed;
extern uint32_t MAXIMUM_RPM_SPEED_CONTROL;
extern uint32_t MINIMUM_RPM_SPEED_CONTROL;

extern void setInput(void);

typedef struct {
    uint8_t stable_pressed;
    uint8_t debounce_ticks;
} ui_gpio_button_t;

typedef struct {
    uint8_t device_on;
    uint8_t motor_requested;
    uint8_t charging;
    uint8_t soc;
    uint8_t blink_on;
    uint16_t target_input;
    uint32_t target_rpm;
    uint32_t last_nonzero_rpm;
    uint16_t last_tick_count;
    uint16_t battery_tick_accum;
    uint16_t blink_tick_accum;
    uint16_t shutdown_ticks;
    ui_gpio_button_t power_btn;
    ui_gpio_button_t inc_btn;
    ui_gpio_button_t dec_btn;
} ui_context_t;

static ui_context_t ui_ctx = {
    .device_on = 0,
    .motor_requested = 0,
    .charging = 0,
    .soc = 0,
    .blink_on = 0,
    .target_input = 0,
    .target_rpm = 0,
    .last_nonzero_rpm = 0,
    .last_tick_count = 0,
    .battery_tick_accum = 0,
    .blink_tick_accum = 0,
    .shutdown_ticks = 0,
    .power_btn = { 0, 0 },
    .inc_btn = { 0, 0 },
    .dec_btn = { 0, 0 }
};

static Button_Handle_t ui_power_button = {
    .FilterTicks = UI_DEBOUNCE_TICKS,
    .LongPressTicks = UI_LONG_PRESS_TICKS,
    .LongPressHoldTicks = 0,
    .DoublePressEnabled = false,
    .DoublePressWaitTicks = 0,
    .ShortPressMaxTicks = UI_SHORT_PRESS_MAX_TICKS,
};

static Button_Handle_t ui_inc_button = {
    .FilterTicks = UI_DEBOUNCE_TICKS,
    .LongPressTicks = UI_LONG_PRESS_TICKS,
    .LongPressHoldTicks = 0,
    .DoublePressEnabled = false,
    .DoublePressWaitTicks = 0,
    .ShortPressMaxTicks = UI_SHORT_PRESS_MAX_TICKS,
};

static Button_Handle_t ui_dec_button = {
    .FilterTicks = UI_DEBOUNCE_TICKS,
    .LongPressTicks = UI_LONG_PRESS_TICKS,
    .LongPressHoldTicks = 0,
    .DoublePressEnabled = false,
    .DoublePressWaitTicks = 0,
    .ShortPressMaxTicks = UI_SHORT_PRESS_MAX_TICKS,
};

static void ui_configure_gpio(void);
static uint8_t ui_read_button(GPIO_TypeDef* port, uint16_t pin);
static void ui_log_button_event(const char* origin, const char* name, const char* event);
static void ui_log_gpio_button(ui_gpio_button_t* btn, const char* name, uint8_t pressed);
static void ui_init_button_module(void);
static uint16_t ui_rpm_to_input(uint32_t rpm);
static void ui_apply_target(void);
static void ui_start_motor(void);
static void ui_stop_motor(void);
static void ui_update_soc_and_leds(void);
static void ui_drive_leds(uint8_t soc, uint8_t charging, uint8_t blink_on);
static void ui_set_led(GPIO_TypeDef* port, uint16_t pin, uint8_t on);
static void ui_on_power_short(void);
static void ui_on_power_long(void);
static void ui_on_inc_short(void);
static void ui_on_dec_short(void);
static void ui_btn_power_down(void);
static void ui_btn_power_up(void);
static void ui_btn_power_short(void);
static void ui_btn_power_long(void);
static void ui_btn_inc_down(void);
static void ui_btn_inc_up(void);
static void ui_btn_inc_short(void);
static void ui_btn_dec_down(void);
static void ui_btn_dec_up(void);
static void ui_btn_dec_short(void);

__attribute__((weak)) void ui_on_power_short_running(void) { }
__attribute__((weak)) void ui_on_increment_short_callback(void) { }
__attribute__((weak)) void ui_on_decrement_short_callback(void) { }

void ui_init(void)
{
    ui_configure_gpio();
    GPIO_SetBits(UI_BAT_V_ON_PORT, UI_BAT_V_ON_PIN);

    drive_by_rpm = 1;
    use_speed_control_loop = 1;
    dshot = 0;
    servoPwm = 0;
    inputSet = 1;
    signaltimeout = 0;
    input_override = 0;
    speedPid.integral = 0;
    speedPid.error = 0;
    speedPid.last_error = 0;
    armed = 0;

    ui_ctx.device_on = 0;
    ui_ctx.motor_requested = 0;
    ui_ctx.target_rpm = MINIMUM_RPM_SPEED_CONTROL;
    ui_ctx.last_nonzero_rpm = MINIMUM_RPM_SPEED_CONTROL;
    ui_ctx.target_input = 0;
    ui_ctx.shutdown_ticks = 0;
    ui_ctx.last_tick_count = tenkhzcounter;

    ui_init_button_module();
}

void ui_set_target_rpm(uint32_t rpm)
{
    if (rpm == 0) {
        ui_ctx.target_rpm = 0;
        ui_ctx.target_input = 0;
        ui_ctx.motor_requested = 0;
    } else {
        if (rpm < MINIMUM_RPM_SPEED_CONTROL) {
            rpm = MINIMUM_RPM_SPEED_CONTROL;
        }
        if (rpm > MAXIMUM_RPM_SPEED_CONTROL) {
            rpm = MAXIMUM_RPM_SPEED_CONTROL;
        }
        ui_ctx.target_rpm = rpm;
        ui_ctx.target_input = ui_rpm_to_input(rpm);
        ui_ctx.last_nonzero_rpm = rpm;
    }

    if (ui_ctx.device_on) {
        ui_apply_target();
    }
}

void ui_request_shutdown(void)
{
    ui_stop_motor();
    ui_ctx.device_on = 0;
    ui_ctx.shutdown_ticks = UI_SHUTDOWN_DELAY_TICKS;
    armed = 0;
}

uint16_t DebugCount1 = 0;

void ui_update(void)
{
    uint16_t now = tenkhzcounter;
    uint16_t elapsed = (uint16_t)(now - ui_ctx.last_tick_count);


    

    if (elapsed < UI_TICK_INTERVAL) {
        return;
    }

    if (DebugCount1 > 100)
    {
        DebugCount1 = 0;
        debug_uart_write("UI alive\r\n");
    }
    else
    {
      DebugCount1++;
    }
        
    ui_ctx.last_tick_count = now;

    signaltimeout = 0;
    inputSet = 1;

    if (ui_ctx.shutdown_ticks > 0) {
        ui_ctx.shutdown_ticks--;
        if (ui_ctx.shutdown_ticks == 0) {
            GPIO_ResetBits(UI_BAT_V_ON_PORT, UI_BAT_V_ON_PIN);
        }
    }

    uint8_t dc_in_raw = ui_read_button(UI_DC_IN_PORT, UI_DC_IN_PIN);
#if UI_DC_IN_ACTIVE_HIGH
    uint8_t charging_now = dc_in_raw;
#else
    uint8_t charging_now = !dc_in_raw;
#endif
    if (charging_now != ui_ctx.charging) {
        ui_ctx.charging = charging_now;
        if (ui_ctx.charging) {
            ui_stop_motor();
            debug_uart_write("charge_detect\r\n");
        }
    }

    uint8_t power_pressed = ui_read_button(UI_POWER_BUTTON_PORT, UI_POWER_BUTTON_PIN);
    uint8_t inc_pressed = ui_read_button(UI_INC_BUTTON_PORT, UI_INC_BUTTON_PIN);
    uint8_t dec_pressed = ui_read_button(UI_DEC_BUTTON_PORT, UI_DEC_BUTTON_PIN);

    ui_log_gpio_button(&ui_ctx.power_btn, "power", power_pressed);
    ui_log_gpio_button(&ui_ctx.inc_btn, "inc", inc_pressed);
    ui_log_gpio_button(&ui_ctx.dec_btn, "dec", dec_pressed);

    BTN_Update(&ui_power_button, power_pressed);
    BTN_Update(&ui_inc_button, inc_pressed);
    BTN_Update(&ui_dec_button, dec_pressed);

    ui_ctx.battery_tick_accum++;
    if (ui_ctx.battery_tick_accum >= UI_SOC_UPDATE_TICKS) {
        ui_ctx.battery_tick_accum = 0;
        ui_update_soc_and_leds();
    }

    ui_ctx.blink_tick_accum++;
    if (ui_ctx.blink_tick_accum >= UI_BLINK_PERIOD_TICKS) {
        ui_ctx.blink_tick_accum = 0;
        ui_ctx.blink_on = !ui_ctx.blink_on;
    }

    ui_apply_target();
}

static void ui_on_power_short(void)
{
  debug_uart_write("ButtonPower Short Press\r\n");

    if (!ui_ctx.device_on) {
        return;
    }

    if (!ui_ctx.motor_requested) {
        ui_start_motor();
    } else {
        ui_on_power_short_running();
    }
}

static void ui_on_power_long(void)
{
    debug_uart_write("ButtonPower Long Press\r\n");
    if (!ui_ctx.device_on) {
        ui_ctx.device_on = 1;
        ui_ctx.shutdown_ticks = 0;
        debug_uart_write("device_on\r\n");
        return;
    }
    debug_uart_write("power_off\r\n");
    ui_request_shutdown();
}

static void ui_on_inc_short(void)
{
  debug_uart_write("ButtonInc Short Press\r\n");

    if (!ui_ctx.device_on) {
        return;
    }
    if (ui_ctx.motor_requested || running) {
        ui_set_target_rpm(ui_ctx.target_rpm + UI_RPM_STEP);
        ui_on_increment_short_callback();
        debug_uart_write("inc\r\n");
    }
}

static void ui_on_dec_short(void)
{
    debug_uart_write("ButtonDec Short Press\r\n");

    if (!ui_ctx.device_on) {
        return;
    }
    if (ui_ctx.motor_requested || running) {
        if (ui_ctx.target_rpm > UI_RPM_STEP) {
            ui_set_target_rpm(ui_ctx.target_rpm - UI_RPM_STEP);
        } else {
            ui_set_target_rpm(0);
        }
        ui_on_decrement_short_callback();
        debug_uart_write("dec\r\n");
    }
}

static void ui_configure_gpio(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef gpio = { 0 };

    gpio.GPIO_Pin = UI_BAT_V_ON_PIN | UI_LED1_PIN | UI_LED2_PIN | UI_LED_ORANGE_PIN;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(UI_BAT_V_ON_PORT, &gpio);

    gpio.GPIO_Pin = UI_LED3_PIN;
    GPIO_Init(UI_LED3_PORT, &gpio);

    GPIO_ResetBits(UI_BAT_V_ON_PORT, UI_BAT_V_ON_PIN | UI_LED1_PIN | UI_LED2_PIN | UI_LED_ORANGE_PIN);
    GPIO_ResetBits(UI_LED3_PORT, UI_LED3_PIN);

    gpio.GPIO_Pin = UI_POWER_BUTTON_PIN | UI_INC_BUTTON_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(UI_POWER_BUTTON_PORT, &gpio);

    gpio.GPIO_Pin = UI_DEC_BUTTON_PIN;
    GPIO_Init(UI_DEC_BUTTON_PORT, &gpio);

    gpio.GPIO_Pin = UI_DC_IN_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(UI_DC_IN_PORT, &gpio);
}

static uint8_t ui_read_button(GPIO_TypeDef* port, uint16_t pin)
{
    return GPIO_ReadInputDataBit(port, pin) == Bit_RESET;
}

static void ui_log_button_event(const char* origin, const char* name, const char* event)
{
    debug_uart_write(origin);
    debug_uart_write(" ");
    debug_uart_write(name);
    debug_uart_write(": ");
    debug_uart_write(event);
    debug_uart_write("\r\n");
}

static void ui_log_gpio_button(ui_gpio_button_t* btn, const char* name, uint8_t pressed)
{
    if (pressed != btn->stable_pressed) {
        if (btn->debounce_ticks < UI_DEBOUNCE_TICKS) {
            btn->debounce_ticks++;
        }
        if (btn->debounce_ticks >= UI_DEBOUNCE_TICKS) {
            btn->stable_pressed = pressed;
            ui_log_button_event("GPIO", name, pressed ? "DOWN" : "UP");
        }
    } else {
        btn->debounce_ticks = 0;
    }
}

static void ui_btn_power_down(void)
{
    ui_log_button_event("BTN", "power", "DOWN");
}

static void ui_btn_power_up(void)
{
    ui_log_button_event("BTN", "power", "UP");
}

static void ui_btn_power_short(void)
{
    ui_log_button_event("BTN", "power", "SHORT");
    ui_on_power_short();
}

static void ui_btn_power_long(void)
{
    ui_log_button_event("BTN", "power", "LONG");
    ui_on_power_long();
}

static void ui_btn_inc_down(void)
{
    ui_log_button_event("BTN", "inc", "DOWN");
}

static void ui_btn_inc_up(void)
{
    ui_log_button_event("BTN", "inc", "UP");
}

static void ui_btn_inc_short(void)
{
    ui_log_button_event("BTN", "inc", "SHORT");
    ui_on_inc_short();
}

static void ui_btn_dec_down(void)
{
    ui_log_button_event("BTN", "dec", "DOWN");
}

static void ui_btn_dec_up(void)
{
    ui_log_button_event("BTN", "dec", "UP");
}

static void ui_btn_dec_short(void)
{
    ui_log_button_event("BTN", "dec", "SHORT");
    ui_on_dec_short();
}

static void ui_init_button_module(void)
{
    BTN_Init(&ui_power_button);
    BTN_Init(&ui_inc_button);
    BTN_Init(&ui_dec_button);

    BTN_Reset(&ui_power_button);
    BTN_Reset(&ui_inc_button);
    BTN_Reset(&ui_dec_button);

    BTN_RegisterCallback_Down(&ui_power_button, ui_btn_power_down);
    BTN_RegisterCallback_Up(&ui_power_button, ui_btn_power_up);
    BTN_RegisterCallback_ShortPress(&ui_power_button, ui_btn_power_short);
    BTN_RegisterCallback_LongPress(&ui_power_button, ui_btn_power_long);

    BTN_RegisterCallback_Down(&ui_inc_button, ui_btn_inc_down);
    BTN_RegisterCallback_Up(&ui_inc_button, ui_btn_inc_up);
    BTN_RegisterCallback_ShortPress(&ui_inc_button, ui_btn_inc_short);

    BTN_RegisterCallback_Down(&ui_dec_button, ui_btn_dec_down);
    BTN_RegisterCallback_Up(&ui_dec_button, ui_btn_dec_up);
    BTN_RegisterCallback_ShortPress(&ui_dec_button, ui_btn_dec_short);
}

static uint16_t ui_rpm_to_input(uint32_t rpm)
{
    return (uint16_t)map((long)rpm, (long)MINIMUM_RPM_SPEED_CONTROL, (long)MAXIMUM_RPM_SPEED_CONTROL, 47, 2047);
}

static void ui_apply_target(void)
{
    signaltimeout = 0;

    if (!ui_ctx.device_on || !ui_ctx.motor_requested || ui_ctx.target_input == 0 || ui_ctx.charging) {
        newinput = 0;
        input_override = 0;
        speedPid.integral = 0;
        speedPid.error = 0;
    } else {
        newinput = ui_ctx.target_input;
    }

    setInput();
}

static void ui_start_motor(void)
{
    if (ui_ctx.charging) {
        return;
    }
    ui_ctx.device_on = 1;
    ui_ctx.shutdown_ticks = 0;
    if (ui_ctx.target_rpm == 0) {
        ui_set_target_rpm(ui_ctx.last_nonzero_rpm ? ui_ctx.last_nonzero_rpm : MINIMUM_RPM_SPEED_CONTROL);
    }

    drive_by_rpm = 1;
    use_speed_control_loop = 1;
    dshot = 0;
    servoPwm = 0;
    inputSet = 1;
    signaltimeout = 0;
    input_override = 0;
    speedPid.integral = 0;
    speedPid.error = 0;
    speedPid.last_error = 0;
    armed = 1;
    ui_ctx.motor_requested = 1;
    debug_uart_write("motor_start\r\n");
    ui_apply_target();
}

static void ui_stop_motor(void)
{
    ui_ctx.motor_requested = 0;
    debug_uart_write("motor_stop\r\n");
    ui_apply_target();
}

static uint8_t ui_calculate_soc(uint16_t voltage_cV)
{
    if (voltage_cV <= UI_SOC_MIN_CV) {
        return 0;
    }
    if (voltage_cV >= UI_SOC_MAX_CV) {
        return 100;
    }
    return (uint8_t)map((long)voltage_cV, UI_SOC_MIN_CV, UI_SOC_MAX_CV, 0, 100);
}

static void ui_set_led(GPIO_TypeDef* port, uint16_t pin, uint8_t on)
{
    if (on) {
        GPIO_SetBits(port, pin);
    } else {
        GPIO_ResetBits(port, pin);
    }
}

static void ui_drive_leds(uint8_t soc, uint8_t charging, uint8_t blink_on)
{
    uint8_t level = (soc + 24U) / 25U; // 0-4 bars

    ui_set_led(UI_LED1_PORT, UI_LED1_PIN, level >= 1);
    ui_set_led(UI_LED2_PORT, UI_LED2_PIN, level >= 2);
    ui_set_led(UI_LED3_PORT, UI_LED3_PIN, level >= 3);

    uint8_t orange_on = 0;
    if (charging) {
        orange_on = 1;
    } else if (soc <= 20U) {
        orange_on = blink_on;
    } else if (level >= 4U) {
        orange_on = 1;
    }
    ui_set_led(UI_LED_ORANGE_PORT, UI_LED_ORANGE_PIN, orange_on);
}

static void ui_update_soc_and_leds(void)
{
    ui_ctx.soc = ui_calculate_soc(battery_voltage);
    ui_drive_leds(ui_ctx.soc, ui_ctx.charging, ui_ctx.blink_on);
}
