#ifndef LED_ANIM_H
#define LED_ANIM_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    LED1 = 0,
    LED2,
    LED3,
    LED_ORANGE,
    LED_U,
    LED_COUNT
} led_id_t;

typedef enum {
    LED_MODE_OFF = 0,
    LED_MODE_SOLID,
    LED_MODE_FADE_IN,
    LED_MODE_FADE_OUT,
    LED_MODE_BREATH
} led_mode_t;

typedef struct {
    bool LedOrangeFlashEnable;
} led_anim_flags_t;

typedef struct {
    uint8_t brightness;   /* 0-100 current level */
    uint8_t target;       /* 0-100 target level */
    led_mode_t mode;
    uint16_t phase;       /* general-purpose phase accumulator */
    uint8_t min_level;
    uint8_t max_level;
    uint16_t period_ticks;
    uint8_t step;
} led_channel_t;

typedef struct {
    led_channel_t channels[LED_COUNT];
    led_anim_flags_t flags;
} LedAnimHandle_t;

typedef struct {
    led_mode_t mode;
    uint8_t solid_level;
    uint8_t min_level;
    uint8_t max_level;
    uint16_t period_ticks;
    uint8_t step;
} led_animation_def_t;

extern const led_animation_def_t LED_Animation_Solid;
extern const led_animation_def_t LED_Animation_Breath;

void LED_ANIM_Init(LedAnimHandle_t* h);
void LED_Anim_Step(LedAnimHandle_t* h);
uint8_t LED_GetBrightness(const LedAnimHandle_t* h, led_id_t id);
void LED_SetLevel(LedAnimHandle_t* h, led_id_t id, uint8_t level);
void LED_Transition_ToSolid(led_channel_t* ch, const led_animation_def_t* def, bool interpolate);
void LED_Transition_ToBreath(led_channel_t* ch, const led_animation_def_t* def, bool interpolate);
void FadeInLed(LedAnimHandle_t* h, led_id_t id);
void FadeOutLed(LedAnimHandle_t* h, led_id_t id);

#ifdef DEBUG_UART_ENABLE
#include "debug_uart.h"
#include <stdio.h>
#define LED_ANIM_PRINTF(fmt, ...)                    \
    do {                                             \
        char __buf[96];                              \
        int __n = snprintf(__buf, sizeof(__buf), fmt, ##__VA_ARGS__); \
        if (__n > 0) {                               \
            __buf[sizeof(__buf) - 1] = '\0';         \
            debug_uart_write(__buf);                 \
            debug_uart_write("\r\n");                \
        }                                            \
    } while (0)
#else
#define LED_ANIM_PRINTF(...) do { } while (0)
#endif

#endif /* LED_ANIM_H */
