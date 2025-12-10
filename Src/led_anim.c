#include "led_anim.h"

#include <string.h>

const led_animation_def_t LED_Animation_Solid = {
    .mode = LED_MODE_SOLID,
    .solid_level = 100,
    .min_level = 0,
    .max_level = 0,
    .period_ticks = 0,
    .step = 4,
};

const led_animation_def_t LED_Animation_Breath = {
    .mode = LED_MODE_BREATH,
    .solid_level = 0,
    .min_level = 10,
    .max_level = 100,
    .period_ticks = 200, /* assuming ~100Hz tick => 2s breath period */
    .step = 2,
};

static uint8_t clamp_level(int level)
{
    if (level < 0) {
        return 0;
    }
    if (level > 100) {
        return 100;
    }
    return (uint8_t)level;
}

void LED_ANIM_Init(LedAnimHandle_t* h)
{
    if (!h) {
        return;
    }
    memset(h, 0, sizeof(*h));
}

uint8_t LED_GetBrightness(const LedAnimHandle_t* h, led_id_t id)
{
    if (!h || id >= LED_COUNT) {
        return 0;
    }
    return h->channels[id].brightness;
}

void LED_SetLevel(LedAnimHandle_t* h, led_id_t id, uint8_t level)
{
    if (!h || id >= LED_COUNT) {
        return;
    }
    led_channel_t* ch = &h->channels[id];
    ch->mode = LED_MODE_SOLID;
    ch->target = clamp_level(level);
    ch->brightness = ch->target;
}

void LED_Transition_ToSolid(led_channel_t* ch, const led_animation_def_t* def, bool interpolate)
{
    if (!ch || !def) {
        return;
    }
    ch->mode = LED_MODE_SOLID;
    ch->target = clamp_level(def->solid_level);
    ch->step = def->step ? def->step : 4;
    if (!interpolate) {
        ch->brightness = ch->target;
    }
}

void LED_Transition_ToBreath(led_channel_t* ch, const led_animation_def_t* def, bool interpolate)
{
    if (!ch || !def) {
        return;
    }
    ch->mode = LED_MODE_BREATH;
    ch->min_level = clamp_level(def->min_level);
    ch->max_level = clamp_level(def->max_level);
    ch->period_ticks = def->period_ticks ? def->period_ticks : 200;
    ch->step = def->step ? def->step : 2;
    if (!interpolate) {
        ch->phase = 0;
    }
}

void FadeInLed(LedAnimHandle_t* h, led_id_t id)
{
    if (!h || id >= LED_COUNT) {
        return;
    }
    led_channel_t* ch = &h->channels[id];
    ch->mode = LED_MODE_FADE_IN;
    ch->target = 100;
    if (ch->step == 0) {
        ch->step = 4;
    }
}

void FadeOutLed(LedAnimHandle_t* h, led_id_t id)
{
    if (!h || id >= LED_COUNT) {
        return;
    }
    led_channel_t* ch = &h->channels[id];
    ch->mode = LED_MODE_FADE_OUT;
    ch->target = 0;
    if (ch->step == 0) {
        ch->step = 4;
    }
}

static void led_anim_step_channel(led_channel_t* ch)
{
    switch (ch->mode) {
    case LED_MODE_OFF:
        ch->brightness = 0;
        break;
    case LED_MODE_SOLID:
        if (ch->brightness < ch->target) {
            int next = ch->brightness + ch->step;
            if (next > ch->target) {
                next = ch->target;
            }
            ch->brightness = (uint8_t)next;
        } else if (ch->brightness > ch->target) {
            int next = ch->brightness - ch->step;
            if (next < ch->target) {
                next = ch->target;
            }
            ch->brightness = (uint8_t)next;
        }
        break;
    case LED_MODE_FADE_IN:
        if (ch->brightness < ch->target) {
            int next = ch->brightness + ch->step;
            if (next >= ch->target) {
                next = ch->target;
                ch->mode = LED_MODE_SOLID;
            }
            ch->brightness = (uint8_t)next;
        } else {
            ch->mode = LED_MODE_SOLID;
        }
        break;
    case LED_MODE_FADE_OUT:
        if (ch->brightness > ch->target) {
            int next = (int)ch->brightness - ch->step;
            if (next <= (int)ch->target) {
                next = ch->target;
                ch->mode = LED_MODE_OFF;
            }
            ch->brightness = (uint8_t)next;
        } else {
            ch->mode = LED_MODE_OFF;
        }
        break;
    case LED_MODE_BREATH:
    default:
        if (ch->period_ticks == 0) {
            ch->period_ticks = 200;
        }
        ch->phase = (uint16_t)((ch->phase + 1U) % ch->period_ticks);
        {
            uint16_t half = ch->period_ticks / 2U;
            uint16_t pos = ch->phase % ch->period_ticks;
            int span = (int)ch->max_level - (int)ch->min_level;
            if (span < 0) {
                span = -span;
            }
            int level;
            if (pos < half) {
                level = ch->min_level + (span * (int)pos) / (int)half;
            } else {
                uint16_t desc = pos - half;
                level = ch->max_level - (span * (int)desc) / (int)half;
            }
            ch->brightness = clamp_level(level);
        }
        break;
    }
}

void LED_Anim_Step(LedAnimHandle_t* h)
{
    if (!h) {
        return;
    }
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        led_anim_step_channel(&h->channels[i]);
    }
}
