#include "ui.h"

#include "button.h"
#include "common.h"
#include "debug_uart.h"
#include "dshot.h"
#include "functions.h"
#include "led_anim.h"
#include "config.h"
#include "speed_sequence_data.h"
#include "main.h"
#include "targets.h"
#include <stdio.h>
#include <stddef.h>



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

typedef enum {
    PRODUCT_IDLE = 0,
    PRODUCT_POWER_ON,
    PRODUCT_STANDBY,
    PRODUCT_RUNNING,
    PRODUCT_MODE_CHANGE,
    PRODUCT_CHARGING,
    PRODUCT_POWER_OFF,
    PRODUCT_ERROR,
} product_state_t;

typedef enum {
    CHARGE_NONE = 0,
    CHARGE_CHARGING,
    CHARGE_COMPLETE,
    CHARGE_ERROR,
} charging_state_t;

typedef struct {
    product_state_t Now;
    product_state_t Prev;
    uint32_t Counter;
    bool Configured;
} product_state_handle_t;

typedef struct {
    charging_state_t Now;
    charging_state_t Prev;
    uint32_t Counter;
    bool Configured;
} charging_state_handle_t;

typedef struct {
    uint8_t stable_pressed;
    uint8_t debounce_ticks;
} ui_gpio_button_t;

typedef struct {
    uint8_t device_on;
    uint8_t motor_requested;
    bool charging_present;
    uint8_t soc;
    uint8_t blink_on;
    uint8_t mode_index;
    uint8_t level_index;
    uint16_t target_input;
    uint32_t target_rpm;
    uint32_t last_nonzero_rpm;
    uint16_t last_tick_count;
    uint16_t battery_tick_accum;
    uint16_t blink_tick_accum;
    uint16_t shutdown_ticks;
    product_state_handle_t state;
    charging_state_handle_t charging_state;
    ui_gpio_button_t power_btn;
    ui_gpio_button_t inc_btn;
    ui_gpio_button_t dec_btn;
} product_context_t;

static product_context_t product = {
    .device_on = 0,
    .motor_requested = 0,
    .charging_present = false,
    .soc = 0,
    .blink_on = 0,
    .mode_index = 0,
    .level_index = 0,
    .target_input = 0,
    .target_rpm = 0,
    .last_nonzero_rpm = 0,
    .last_tick_count = 0,
    .battery_tick_accum = 0,
    .blink_tick_accum = 0,
    .shutdown_ticks = 0,
    .state.Now = PRODUCT_POWER_ON,
    .state.Prev = PRODUCT_POWER_ON,
    .state.Counter = 0,
    .state.Configured = false,
    .charging_state.Now = CHARGE_NONE,
    .charging_state.Prev = CHARGE_NONE,
    .charging_state.Counter = 0,
    .charging_state.Configured = false,
    .power_btn = { 0, 0 },
    .inc_btn = { 0, 0 },
    .dec_btn = { 0, 0 }
};

static LedAnimHandle_t led_anim;

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
static void ui_log_context(void);
static void ui_init_button_module(void);
static void product_state_step(bool charging_present, uint8_t charge_done);
static void charging_state_step(bool charging_present, uint8_t charge_done);
static void product_set_state(product_state_t next, uint32_t counter);
static void charging_set_state(charging_state_t next, uint32_t counter);
static void ui_update_u_light(void);
static void ui_cycle_mode(void);
static void ui_change_level(int8_t delta);
static void ui_on_mode_or_level_changed(void);
static void ui_prepare_speed_sequence_request(void);
static uint16_t ui_rpm_to_input(uint32_t rpm);
static void ui_apply_target(void);
static void ui_start_motor(void);
static void ui_stop_motor(void);
static void ui_update_soc_and_leds(void);
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
static uint8_t ui_read_charge_done(void);
static int DisplayBatteryDischargeLevel(LedAnimHandle_t* this, int8_t battLevelPercent);
static int DisplayBatteryChargeLevel(LedAnimHandle_t* this, int8_t chargeLevelPercent, bool charge_done);

__attribute__((weak)) void ui_on_increment_short_callback(void) { }
__attribute__((weak)) void ui_on_decrement_short_callback(void) { }

void ui_init(void)
{
    ui_configure_gpio();
    GPIO_SetBits(UI_BAT_V_ON_PORT, UI_BAT_V_ON_PIN);
    LED_ANIM_Init(&led_anim);

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

    product.device_on = 0;
    product.motor_requested = 0;
    product.target_rpm = 0;
    product.last_nonzero_rpm = 0;
    product.target_input = 0;
    product.shutdown_ticks = 0;
    product.last_tick_count = tenkhzcounter;
    product.mode_index = (UI_DEFAULT_MODE_INDEX < NUM_MODES) ? UI_DEFAULT_MODE_INDEX : 0U;
    product.level_index = (UI_DEFAULT_LEVEL_INDEX < NUM_GEARS) ? UI_DEFAULT_LEVEL_INDEX : 0U;
    product_set_state(PRODUCT_POWER_ON, 0);
    charging_set_state(CHARGE_NONE, 0);

    /* Seed target_rpm/target_input so UI-driven throttle is non-zero. */
    ui_set_target_rpm(MINIMUM_RPM_SPEED_CONTROL);

    ui_init_button_module();
}

void ui_set_target_rpm(uint32_t rpm)
{
    if (rpm == 0) {
        product.target_rpm = 0;
        product.target_input = 0;
        product.motor_requested = 0;
    } else {
        if (rpm < MINIMUM_RPM_SPEED_CONTROL) {
            rpm = MINIMUM_RPM_SPEED_CONTROL;
        }
        if (rpm > MAXIMUM_RPM_SPEED_CONTROL) {
            rpm = MAXIMUM_RPM_SPEED_CONTROL;
        }
        product.target_rpm = rpm;
        product.target_input = ui_rpm_to_input(rpm);
        product.last_nonzero_rpm = rpm;
    }

    if (product.device_on) {
        ui_apply_target();
    }
}

void ui_request_shutdown(void)
{
    ui_stop_motor();
    product.device_on = 0;
    product.shutdown_ticks = UI_SHUTDOWN_DELAY_TICKS;
    armed = 0;
}

uint16_t DebugCount1 = 0;
static uint16_t ui_diag_tick_accum = 0;
static uint8_t u_light_level = 0;
static uint16_t breath_phase = 0;
static uint8_t breath_dir = 1;
static uint16_t mode_change_phase = 0;
static uint16_t fade_phase = 0;
static int battery_band_discharge = -1;
static int battery_band_charge = -1;
static void ui_debug_write(const char* msg);

void ui_update(void)
{
    uint16_t now = tenkhzcounter;
    uint16_t elapsed = (uint16_t)(now - product.last_tick_count);

    if (elapsed < UI_TICK_INTERVAL) {
        return;
    }

    product.last_tick_count = now;

    signaltimeout = 0;
    inputSet = 1;

    /* Handle suicide/shutdown */
    if (product.shutdown_ticks > 0) {
        product.shutdown_ticks--;
        if (product.shutdown_ticks == 0) {
            GPIO_ResetBits(UI_BAT_V_ON_PORT, UI_BAT_V_ON_PIN);
        }
    }

    uint8_t dc_in_raw = GPIO_ReadInputDataBit(UI_DC_IN_PORT, UI_DC_IN_PIN) == Bit_SET;
    uint8_t charging_now = 
#if UI_DC_IN_ACTIVE_HIGH
        dc_in_raw;
#else
        !dc_in_raw;
#endif
    uint8_t charge_done = ui_read_charge_done();

#if defined(DEBUG_UART_ENABLE) && defined(DEBUG_UART_USE_USART1)
    product.charging_present = false; /* PB6 remap for UART clashes with charge sense; disable while logging. */
#else
    product.charging_present = charging_now;
#endif

#ifdef DEBUG_UART_ENABLE
    if (DebugCount1 > 100)
    {
        DebugCount1 = 0;
        ui_debug_write("UI alive\r\n");
        if (charging_now)
        {
          ui_debug_write("DC IN\r\n");
        }
        else
        {
          ui_debug_write("DC not IN\r\n");
        }
    }
    else
    {
      DebugCount1++;
    }
#endif

    uint8_t power_pressed = ui_read_button(UI_POWER_BUTTON_PORT, UI_POWER_BUTTON_PIN);
    uint8_t inc_pressed = ui_read_button(UI_INC_BUTTON_PORT, UI_INC_BUTTON_PIN);
    uint8_t dec_pressed = ui_read_button(UI_DEC_BUTTON_PORT, UI_DEC_BUTTON_PIN);

    // ui_log_gpio_button(&product.power_btn, "power", power_pressed);
    // ui_log_gpio_button(&product.inc_btn, "inc", inc_pressed);
    // ui_log_gpio_button(&product.dec_btn, "dec", dec_pressed);

    BTN_Update(&ui_power_button, power_pressed);
    BTN_Update(&ui_inc_button, inc_pressed);
    BTN_Update(&ui_dec_button, dec_pressed);

    product.battery_tick_accum++;
    if (product.battery_tick_accum >= UI_SOC_UPDATE_TICKS) {
        product.battery_tick_accum = 0;
        ui_update_soc_and_leds();
    }

    product.blink_tick_accum++;
    if (product.blink_tick_accum >= UI_BLINK_PERIOD_TICKS) {
        product.blink_tick_accum = 0;
        product.blink_on = !product.blink_on;
    }

    product_state_step(product.charging_present, charge_done);
    charging_state_step(product.charging_present, charge_done);
    ui_update_u_light();

    LED_Anim_Step(&led_anim);

    ui_diag_tick_accum++;
    if (ui_diag_tick_accum >= UI_TICK_HZ) {
        ui_diag_tick_accum = 0;
        ui_log_context();
    }

    ui_apply_target();
}

static void product_set_state(product_state_t next, uint32_t counter)
{
    char msg[80];
    int n = snprintf(msg, sizeof(msg), "State: %u -> %u\r\n", (unsigned)product.state.Now, (unsigned)next);
    if (n > 0) {
        msg[sizeof(msg) - 1] = '\0';
        ui_debug_write(msg);
    }
    product.state = (product_state_handle_t){ next, product.state.Now, counter, false };
}

static void charging_set_state(charging_state_t next, uint32_t counter)
{
    char msg[80];
    int n = snprintf(msg, sizeof(msg), "ChargeState: %u -> %u\r\n", (unsigned)product.charging_state.Now, (unsigned)next);
    if (n > 0) {
        msg[sizeof(msg) - 1] = '\0';
        ui_debug_write(msg);
    }
    product.charging_state = (charging_state_handle_t){ next, product.charging_state.Now, counter, false };
}

static void product_state_step(bool charging_present, uint8_t charge_done)
{
    (void)charge_done;
    if (product.state.Counter > 0)
    {
        product.state.Counter--;
    }


    // if (product.state.Configured) {
    //     product.state.Counter++;
    // } else {
    //     product.state.Configured = true;
    //     fade_phase = 0;
    //     mode_change_phase = 0;
    // }

    switch (product.state.Now) {
    case PRODUCT_POWER_ON:
        if (!product.state.Configured)
        {
          product.device_on = 1;
          fade_phase = 0;
          product.state.Configured = true;
        } else
        {
          if (fade_phase < UI_TICK_HZ) {
              fade_phase++;
              u_light_level = (uint8_t)((fade_phase * 100U) / UI_TICK_HZ);
          } else
          {
              u_light_level = 100;
              product_set_state(PRODUCT_STANDBY, 0);
              break;
          }
          
          if (charging_present) {
              product_set_state(PRODUCT_CHARGING, 0);
          } else
          {
            /* Do nothing */
          }
        }
        break;

    case PRODUCT_STANDBY:
        if (!product.state.Configured)
        {
          product.device_on = 1;
          breath_phase = 0;
          breath_dir = 1;
          product.state.Configured = true;
        }
        else
        {
          if (charging_present) {
              product_set_state(PRODUCT_CHARGING, 0);
              break;
          } else
          {
            if (product.motor_requested || running) {
                product_set_state(PRODUCT_RUNNING, 0);
                break;
            } else
            {
              /* Do nothing */
            }
          }
        }

        break;

    case PRODUCT_RUNNING:
        if (!product.state.Configured) {
            product.device_on = 1;
            product.state.Configured = true;
        } else {
          if (charging_present) {
              product.motor_requested = 0;
              ui_stop_motor();
              product_set_state(PRODUCT_CHARGING, 0);
              break;
          }
          if (!product.motor_requested && !running) {
              product_set_state(PRODUCT_STANDBY, 0);
              break;
          }

          ui_set_target_rpm((product.level_index * 75 + 300)*CURRENT_GEAR_RATIO);
        }
        break;

    /// TODO: remove this state. Put everything in product running 
    case PRODUCT_MODE_CHANGE:
        if (!product.state.Configured) {
            mode_change_phase = 0;
            product.state.Configured = true;
        } else {
          mode_change_phase++;
          if (mode_change_phase >= (UI_TICK_HZ * 2U)) {
              product_set_state(PRODUCT_RUNNING, 0);
              break;
          }

          ui_set_target_rpm((product.level_index * 75 + 300)*CURRENT_GEAR_RATIO);
        }
        break;

    case PRODUCT_POWER_OFF:
        if (!product.state.Configured) {
            product.device_on = 0;
            product.motor_requested = 0;
            ui_stop_motor();
            fade_phase = UI_TICK_HZ;
            product.state.Configured = true;
        } else
        {
          if (fade_phase > 0) {
              fade_phase--;
              u_light_level = (uint8_t)((fade_phase * 100U) / UI_TICK_HZ);
          } else {
              u_light_level = 0;
          }
          if (charging_present) {

              product_set_state(PRODUCT_CHARGING, 0);
              break;
          }
        }

        break;

    case PRODUCT_CHARGING:
        if (!product.state.Configured) {
            product.device_on = 0;
            product.motor_requested = 0;
            ui_stop_motor();
            u_light_level = 0;
            product.state.Configured = true;
        } else
        {
          if (!charging_present) {
              product_set_state((product.motor_requested || running) ? PRODUCT_RUNNING : PRODUCT_STANDBY, 0);
          } else
          {
            /* Do nothing. */
          }
        }

        break;

    case PRODUCT_ERROR:
    case PRODUCT_IDLE:
    default:
        product_set_state(charging_present ? PRODUCT_CHARGING : PRODUCT_STANDBY, 0);
        break;
    }
}

static void charging_state_step(bool charging_present, uint8_t charge_done)
{
    if (product.charging_state.Configured) {
        product.charging_state.Counter++;
    } else {
        product.charging_state.Configured = true;
    }

    if (!charging_present) {
        if (product.charging_state.Now != CHARGE_NONE){
          charging_set_state(CHARGE_NONE, 0);
        }
        return;
    } else
    {
      if (charge_done)
      {
        if (product.charging_state.Now != CHARGE_COMPLETE) {
            charging_set_state(CHARGE_COMPLETE, 0);
        }
      } else
      {
          if (product.charging_state.Now != CHARGE_CHARGING) {
              charging_set_state(CHARGE_CHARGING, 0);
          }
      }
    }
}

static void ui_update_u_light(void)
{
    switch (product.state.Now) {
    case PRODUCT_STANDBY:
        if (breath_dir) {
            if (breath_phase < UI_TICK_HZ) {
                breath_phase++;
            } else {
                breath_dir = 0;
            }
        } else {
            if (breath_phase > 0) {
                breath_phase--;
            } else {
                breath_dir = 1;
            }
        }
        u_light_level = (uint8_t)((breath_phase * 100U) / UI_TICK_HZ);
        break;
    case PRODUCT_MODE_CHANGE:
        if (mode_change_phase < UI_TICK_HZ) {
            u_light_level = (uint8_t)((mode_change_phase * 100U) / UI_TICK_HZ);
        } else if (mode_change_phase < (UI_TICK_HZ * 2U)) {
            uint16_t desc = mode_change_phase - UI_TICK_HZ;
            if (desc > UI_TICK_HZ) {
                desc = UI_TICK_HZ;
            }
            u_light_level = (uint8_t)(((UI_TICK_HZ - desc) * 100U) / UI_TICK_HZ);
        } else {
            u_light_level = 100;
        }
        break;
    case PRODUCT_RUNNING:
        u_light_level = 100;
        break;
    case PRODUCT_POWER_OFF:
    case PRODUCT_CHARGING:
        u_light_level = 0;
        break;
    case PRODUCT_POWER_ON:
    case PRODUCT_IDLE:
    case PRODUCT_ERROR:
    default:
        /* PRODUCT_POWER_ON handled by fade logic in state step. */
        break;
    }
#if (BU01_PRODUCT == BU01_EXT)
    /* Only the BU01_EXT variant has the dedicated U-light. */
    LED_SetLevel(&led_anim, LED_U, u_light_level);
#endif
}

void ui_light_tick_fast(void)
{
    static uint8_t pwm_counter = 0;
    pwm_counter++;
    if (pwm_counter >= 100) {
        pwm_counter = 0;
    }

    uint32_t set_mask_b = 0;
    uint32_t clr_mask_b = 0;
    uint32_t set_mask_a = 0;
    uint32_t clr_mask_a = 0;

    struct {
        GPIO_TypeDef* port;
        uint16_t pin;
        led_id_t id;
    } map[] = {
        { UI_LED1_PORT, UI_LED1_PIN, LED1 },
        { UI_LED2_PORT, UI_LED2_PIN, LED2 },
        { UI_LED3_PORT, UI_LED3_PIN, LED3 },
        { UI_LED_ORANGE_PORT, UI_LED_ORANGE_PIN, LED_ORANGE },
    };

    for (size_t i = 0; i < sizeof(map) / sizeof(map[0]); i++) {
        uint8_t level = LED_GetBrightness(&led_anim, map[i].id);
        uint8_t on = (pwm_counter < level);
        if (map[i].port == GPIOB) {
            if (on) {
                set_mask_b |= map[i].pin;
            } else {
                clr_mask_b |= map[i].pin;
            }
        } else if (map[i].port == GPIOA) {
            if (on) {
                set_mask_a |= map[i].pin;
            } else {
                clr_mask_a |= map[i].pin;
            }
        }
    }

    if (set_mask_b) {
        GPIO_SetBits(GPIOB, set_mask_b);
    }
    if (clr_mask_b) {
        GPIO_ResetBits(GPIOB, clr_mask_b);
    }
    if (set_mask_a) {
        GPIO_SetBits(GPIOA, set_mask_a);
    }
    if (clr_mask_a) {
        GPIO_ResetBits(GPIOA, clr_mask_a);
    }
}

static void ui_debug_write(const char* msg)
{
    if (!msg) {
        return;
    }
#ifdef DEBUG_UART_ENABLE
    debug_uart_write(msg);
#else
    (void)msg;
#endif
}

static void ui_prepare_speed_sequence_request(void)
{
    const SpeedSeq_Step_t *steps = NULL;
    uint8_t num_steps = 0;
    uint32_t repeat = 0;

    if (!SpeedSeq_GetModeGearData(product.mode_index, product.level_index, &steps, &num_steps, &repeat)) {
        ui_debug_write("SpeedSeq lookup failed\r\n");
        return;
    }

    char buf[120];
    int n = snprintf(buf, sizeof(buf),
                     "mode=%u/%u level=%u/%u steps=%u repeat=%lu\r\n",
                     (unsigned)(product.mode_index + 1U),
                     (unsigned)NUM_MODES,
                     (unsigned)(product.level_index + 1U),
                     (unsigned)NUM_GEARS,
                     (unsigned)num_steps,
                     (unsigned long)repeat);
    if (n > 0) {
        buf[sizeof(buf) - 1] = '\0';
        debug_uart_write(buf);
    }

    /* TODO: hook this into the speed sequence driver when available. */
}

static void ui_on_mode_or_level_changed(void)
{
    product_set_state(PRODUCT_MODE_CHANGE, 0);
    ui_prepare_speed_sequence_request();
}

static void ui_cycle_mode(void)
{
    if (NUM_MODES == 0U) {
        return;
    }

    product.mode_index = (uint8_t)((product.mode_index + 1U) % NUM_MODES);
    ui_on_mode_or_level_changed();
}

static void ui_change_level(int8_t delta)
{
    if (NUM_GEARS == 0U) {
        return;
    }

    uint8_t next_level = product.level_index;

    if ((delta > 0) && ((next_level + 1U) < NUM_GEARS)) {
        next_level++;
    } else if ((delta < 0) && (next_level > 0U)) {
        next_level--;
    }

    if (next_level != product.level_index) {
        product.level_index = next_level;
        ui_on_mode_or_level_changed();
    }
}

static void ui_on_power_short(void)
{
  debug_uart_write("ButtonPower Short Press\r\n");

    if (product.state.Now == PRODUCT_CHARGING) {
        return;
    }

    product.device_on = 1;

    if (!product.motor_requested) {
        ui_start_motor();
        product_set_state(PRODUCT_RUNNING, 0);
        return;
    }

    ui_cycle_mode();
}

static void ui_on_power_long(void)
{
    debug_uart_write("ButtonPower Long Press\r\n");
    if (product.state.Now == PRODUCT_CHARGING) {
        return;
    }
    if (!product.device_on) {
        product.device_on = 1;
        product.shutdown_ticks = 0;
        debug_uart_write("device_on\r\n");
    } else {
        debug_uart_write("power_off\r\n");
        product_set_state(PRODUCT_POWER_OFF, 0);
        ui_request_shutdown();
    }
}

static void ui_on_inc_short(void)
{
  debug_uart_write("ButtonInc Short Press\r\n");

    if (!product.device_on || product.state.Now == PRODUCT_CHARGING) {
        return;
    }
    if (product.motor_requested || running) {
        ui_change_level(1);
        ui_on_increment_short_callback();
    }
}

static void ui_on_dec_short(void)
{
    debug_uart_write("ButtonDec Short Press\r\n");

    if (!product.device_on || product.state.Now == PRODUCT_CHARGING) {
        return;
    }
    if (product.motor_requested || running) {
        ui_change_level(-1);
        ui_on_decrement_short_callback();
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

    gpio.GPIO_Pin = UI_CHG_DONE_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(UI_CHG_DONE_PORT, &gpio);

    gpio.GPIO_Pin = UI_DC_IN_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(UI_DC_IN_PORT, &gpio);

}

static uint8_t ui_read_button(GPIO_TypeDef* port, uint16_t pin)
{
    return GPIO_ReadInputDataBit(port, pin) == Bit_RESET;
}

static uint8_t ui_read_charge_done(void)
{
    uint8_t raw = GPIO_ReadInputDataBit(UI_CHG_DONE_PORT, UI_CHG_DONE_PIN) == Bit_SET;
#if UI_CHG_DONE_ACTIVE_LOW
    return raw;
#else
    return !raw;
#endif
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

static void ui_log_context(void)
{
    char buf[180];
    int n = snprintf(buf, sizeof(buf),
                     "UI product: dev=%u motor_req=%u charging=%u chg_state=%u soc=%u tgt_in=%u state=%u mode=%u level=%u \r\n",
                     (unsigned)product.device_on,
                     (unsigned)product.motor_requested,
                     (unsigned)product.charging_present,
                     (unsigned)product.charging_state.Now,
                     (unsigned)product.soc,
                     (unsigned)product.target_input,
                     (unsigned)product.state.Now,
                     (unsigned)(product.mode_index + 1U),
                     (unsigned)(product.level_index + 1U));
    if (n > 0) {
        buf[sizeof(buf) - 1] = '\0';
        debug_uart_write(buf);
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

    if (!product.device_on || !product.motor_requested || product.target_input == 0 || product.charging_present) {
        newinput = 0;
        input_override = 0;
        speedPid.integral = 0;
        speedPid.error = 0;
    } else {
        newinput = product.target_input;
    }

    setInput();
}

static void ui_start_motor(void)
{
    if (product.charging_present) {
        return;
    }
    product.device_on = 1;
    product.shutdown_ticks = 0;
    if ((product.target_rpm == 0) || (product.target_input == 0)) {
        uint32_t rpm = product.target_rpm ? product.target_rpm : (product.last_nonzero_rpm ? product.last_nonzero_rpm : MINIMUM_RPM_SPEED_CONTROL);
        ui_set_target_rpm(rpm);
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
    product.motor_requested = 1;
    debug_uart_write("motor_start\r\n");
    ui_apply_target();
}

static void ui_stop_motor(void)
{
    product.motor_requested = 0;
    debug_uart_write("motor_stop\r\n");
    ui_apply_target();
}

static int led_band_with_hyst(int last_band, uint8_t percent)
{
    const uint8_t t1 = BATTERY_LEVEL_LOW;
    const uint8_t t2 = 40;
    const uint8_t t3 = 70;
    const uint8_t h  = BATTERY_LEVEL_HYST;

    if (last_band < 0) {
        if (percent < t1) return 0;
        if (percent < t2) return 1;
        if (percent < t3) return 2;
        return 3;
    }

    switch (last_band) {
    case 0:
        if (percent >= t1 + h) last_band = 1;
        break;
    case 1:
        if (percent >= t2 + h) last_band = 2;
        else if (percent < t1 - h) last_band = 0;
        break;
    case 2:
        if (percent >= t3 + h) last_band = 3;
        else if (percent < t2 - h) last_band = 1;
        break;
    case 3:
        if (percent < t3 - h) last_band = 2;
        break;
    default:
        last_band = -1;
        break;
    }

    if (last_band < 0) {
        if (percent < t1) return 0;
        if (percent < t2) return 1;
        if (percent < t3) return 2;
        return 3;
    }
    return last_band;
}

static int DisplayBatteryDischargeLevel(LedAnimHandle_t* this, int8_t battLevelPercent)
{
  if (!this || battLevelPercent == BATTERY_LEVEL_UNKNOWN)
  {
    return BATTERY_LEVEL_UNKNOWN;
  }

  int band = led_band_with_hyst(battery_band_discharge, (uint8_t)battLevelPercent);
  battery_band_discharge = band;

  switch (band)
  {
  case 0: /* critical */
    LED_ANIM_PRINTF("Battery Critical: %d%%", battLevelPercent);
    for (uint8_t i = 0; i < LED_COUNT; i++)
    {
      FadeOutLed(this, (led_id_t)i);
    }
    LED_Transition_ToBreath(&this->channels[LED_ORANGE], &LED_Animation_Breath, true);
    this->flags.LedOrangeFlashEnable = true;
    break;
  case 1: /* low */
    LED_ANIM_PRINTF("Battery Low: %d%%", battLevelPercent);
    FadeInLed(this, LED1);
    FadeOutLed(this, LED2);
    FadeOutLed(this, LED3);
    FadeOutLed(this, LED_ORANGE);
    this->flags.LedOrangeFlashEnable = false;
    break;
  case 2: /* medium */
    LED_ANIM_PRINTF("Battery Medium: %d%%", battLevelPercent);
    FadeInLed(this, LED1);
    FadeInLed(this, LED2);
    FadeOutLed(this, LED3);
    FadeOutLed(this, LED_ORANGE);
    this->flags.LedOrangeFlashEnable = false;
    break;
  case 3: /* high */
  default:
    LED_ANIM_PRINTF("Battery High: %d%%", battLevelPercent);
    for (uint8_t i = 0; i < BATTERY_LEDS; i++)
    {
      FadeInLed(this, (led_id_t)i);
    }
    FadeOutLed(this, LED_ORANGE);
    this->flags.LedOrangeFlashEnable = false;
    break;
  }

  return battLevelPercent;
}

static int DisplayBatteryChargeLevel(LedAnimHandle_t* this, int8_t chargeLevelPercent, bool charge_done)
{
  if (!this || chargeLevelPercent == BATTERY_LEVEL_UNKNOWN)
  {
    return BATTERY_LEVEL_UNKNOWN;
  }

  int band = led_band_with_hyst(battery_band_charge, (uint8_t)chargeLevelPercent);
  battery_band_charge = band;

  LED_ANIM_PRINTF("Charge Level: %d%% (done=%d)", chargeLevelPercent, charge_done ? 1 : 0);

  /* Orange off while charging */
  FadeOutLed(this, LED_ORANGE);

  if (band == 0) {
    LED_Transition_ToBreath(&this->channels[LED1], &LED_Animation_Breath, true);
    FadeOutLed(this, LED2);
    FadeOutLed(this, LED3);
  } else if (band == 1) {
    LED_Transition_ToSolid(&this->channels[LED1], &LED_Animation_Solid, true);
    LED_Transition_ToBreath(&this->channels[LED2], &LED_Animation_Breath, true);
    FadeOutLed(this, LED3);
  } else if (band == 2) {
    LED_Transition_ToSolid(&this->channels[LED1], &LED_Animation_Solid, true);
    LED_Transition_ToSolid(&this->channels[LED2], &LED_Animation_Solid, true);
    LED_Transition_ToBreath(&this->channels[LED3], &LED_Animation_Breath, true);
  } else { /* band 3 */
    if (!charge_done) {
      LED_Transition_ToSolid(&this->channels[LED1], &LED_Animation_Solid, true);
      LED_Transition_ToSolid(&this->channels[LED2], &LED_Animation_Solid, true);
      LED_Transition_ToBreath(&this->channels[LED3], &LED_Animation_Breath, true);
    } else {
      for (uint8_t i = 0; i < BATTERY_LEDS; i++) {
        LED_Transition_ToSolid(&this->channels[i], &LED_Animation_Solid, true);
      }
    }
  }

  FadeOutLed(this, LED_U);

  return chargeLevelPercent;
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

static void ui_update_soc_and_leds(void)
{
    product.soc = ui_calculate_soc(battery_voltage);
    if (product.charging_state.Now == CHARGE_CHARGING || product.charging_state.Now == CHARGE_COMPLETE) {
        battery_band_charge = DisplayBatteryChargeLevel(&led_anim, product.soc,
                                                        product.charging_state.Now == CHARGE_COMPLETE);
    } else {
        battery_band_discharge = DisplayBatteryDischargeLevel(&led_anim, product.soc);
    }
}
