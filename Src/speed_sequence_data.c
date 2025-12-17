#include "speed_sequence_data.h"
#include "speed_sequence_config.h"
#include "parameter_conversion.h"
#include "ui.h"

/* We now still have 6 modes, but only 4 gears */
#define NUM_MODES 6
#define NUM_GEARS 4

#define PRODUCT_TASK_FREQUENCY (UI_TICK_HZ) /*  */

/***********************************************************
 * Mode 0: Fixed Speed
 *   Now has 3 steps per gear:
 *     1) 10 seconds
 *     2) 15 seconds
 *     3) Infinite / "a lot of seconds"
 ***********************************************************/
#if M0_NUM_STEPS > 0

/* Mode 0, Gear 0 */
const SpeedSeq_Step_t Mode0_Gear0_Steps[M0_NUM_STEPS] =
{
    {
        .Type = M0_STEP1_TYPE,
        .Duration = ms_Conversion(M0_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),  /* 10s */
        .Params.Fixed = {
            .Level = M0_G0_STEP1_LEVEL
        }
    },
    {
        .Type = M0_STEP2_TYPE,
        .Duration = ms_Conversion(M0_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),  /* 15s */
        .Params.Fixed = {
            .Level = M0_G0_STEP2_LEVEL
        }
    },
    {
        .Type = M0_STEP3_TYPE,
        .Duration = UINT32_MAX,  /* "a lot of seconds" */
        .Params.Fixed = {
            .Level = M0_G0_STEP3_LEVEL
        }
    }
};

/* Gear 1 */
const SpeedSeq_Step_t Mode0_Gear1_Steps[M0_NUM_STEPS] =
{
    {
        .Type = M0_STEP1_TYPE,
        .Duration = ms_Conversion(M0_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY), 
        .Params.Fixed = {
            .Level = M0_G1_LEVEL
        }
    },
    {
        .Type = M0_STEP2_TYPE,
        .Duration = ms_Conversion(M0_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M0_G1_LEVEL
        }
    },
    {
        .Type = M0_STEP3_TYPE,
        .Duration = UINT32_MAX,
        .Params.Fixed = {
            .Level = M0_G1_LEVEL
        }
    }
};

/* Gear 2 */
const SpeedSeq_Step_t Mode0_Gear2_Steps[M0_NUM_STEPS] =
{
    {
        .Type = M0_STEP1_TYPE,
        .Duration = ms_Conversion(M0_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M0_G2_LEVEL
        }
    },
    {
        .Type = M0_STEP2_TYPE,
        .Duration = ms_Conversion(M0_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M0_G2_LEVEL
        }
    },
    {
        .Type = M0_STEP3_TYPE,
        .Duration = UINT32_MAX,
        .Params.Fixed = {
            .Level = M0_G2_LEVEL
        }
    }
};

/* Gear 3 */
const SpeedSeq_Step_t Mode0_Gear3_Steps[M0_NUM_STEPS] =
{
    {
        .Type = M0_STEP1_TYPE,
        .Duration = ms_Conversion(M0_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M0_G3_LEVEL
        }
    },
    {
        .Type = M0_STEP2_TYPE,
        .Duration = ms_Conversion(M0_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M0_G3_LEVEL
        }
    },
    {
        .Type = M0_STEP3_TYPE,
        .Duration = UINT32_MAX,
        .Params.Fixed = {
            .Level = M0_G3_LEVEL
        }
    }
};

#endif // M0_NUM_STEPS > 0

/***********************************************************
 * Mode 1: Sinusoidal
 ***********************************************************/
#if M1_NUM_STEPS > 0

/* Gear 0 */
const SpeedSeq_Step_t Mode1_Gear0_Steps[M1_NUM_STEPS] =
{
    {
        .Type = M1_STEP1_TYPE,
        .Duration = UINT32_MAX,
        .Params.Sine = {
            .Offset    = M1_G0_OFFSET,
            .Amplitude = M1_G0_AMPLITUDE,
            .Frequency = M1_G0_FREQUENCY
        }
    }
};

/* Gear 1 */
const SpeedSeq_Step_t Mode1_Gear1_Steps[M1_NUM_STEPS] =
{
    {
        .Type = M1_STEP1_TYPE,
        .Duration = UINT32_MAX,
        .Params.Sine = {
            .Offset    = M1_G1_OFFSET,
            .Amplitude = M1_G1_AMPLITUDE,
            .Frequency = M1_G1_FREQUENCY
        }
    }
};

/* Gear 2 */
const SpeedSeq_Step_t Mode1_Gear2_Steps[M1_NUM_STEPS] =
{
    {
        .Type = M1_STEP1_TYPE,
        .Duration = UINT32_MAX,
        .Params.Sine = {
            .Offset    = M1_G2_OFFSET,
            .Amplitude = M1_G2_AMPLITUDE,
            .Frequency = M1_G2_FREQUENCY
        }
    }
};

/* Gear 3 */
const SpeedSeq_Step_t Mode1_Gear3_Steps[M1_NUM_STEPS] =
{
    {
        .Type = M1_STEP1_TYPE,
        .Duration = UINT32_MAX,
        .Params.Sine = {
            .Offset    = M1_G3_OFFSET,
            .Amplitude = M1_G3_AMPLITUDE,
            .Frequency = M1_G3_FREQUENCY
        }
    }
};

#endif // M1_NUM_STEPS > 0

/***********************************************************
 * Mode 2
 *  3 steps: Quadratic up, then hold, then near zero
 ***********************************************************/
#if M2_NUM_STEPS > 0

/* Gear 0 */
const SpeedSeq_Step_t Mode2_Gear0_Steps[M2_NUM_STEPS] =
{
    /* Step 1 */
    {
        .Type = M2_STEP1_TYPE,
        .Duration = ms_Conversion(M2_G0_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Quad = {
            .StartValue   = M2_G0_START_LEVEL,
            .EndValue     = M2_G0_END_LEVEL,
            .IsIncreasing = M2_G0_ISINCREASING
        }
    },
    /* Step 2 */
    {
        .Type = M2_STEP2_TYPE,
        .Duration = ms_Conversion(M2_G0_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M2_G0_END_LEVEL
        }
    },
    /* Step 3 */
    {
        .Type = M2_STEP3_TYPE,
        .Duration = ms_Conversion(M2_G0_STEP3_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = MIN_LEVEL_S16
        }
    }
};

/* Gear 1 */
const SpeedSeq_Step_t Mode2_Gear1_Steps[M2_NUM_STEPS] =
{
    {
        .Type = M2_STEP1_TYPE,
        .Duration = ms_Conversion(M2_G1_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Quad = {
            .StartValue   = M2_G1_START_LEVEL,
            .EndValue     = M2_G1_END_LEVEL,
            .IsIncreasing = M2_G1_ISINCREASING
        }
    },
    {
        .Type = M2_STEP2_TYPE,
        .Duration = ms_Conversion(M2_G1_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M2_G1_END_LEVEL
        }
    },
    {
        .Type = M2_STEP3_TYPE,
        .Duration = ms_Conversion(M2_G1_STEP3_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = MIN_LEVEL_S16
        }
    }
};

/* Gear 2 */
const SpeedSeq_Step_t Mode2_Gear2_Steps[M2_NUM_STEPS] =
{
    {
        .Type = M2_STEP1_TYPE,
        .Duration = ms_Conversion(M2_G2_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Quad = {
            .StartValue   = M2_G2_START_LEVEL,
            .EndValue     = M2_G2_END_LEVEL,
            .IsIncreasing = M2_G2_ISINCREASING
        }
    },
    {
        .Type = M2_STEP2_TYPE,
        .Duration = ms_Conversion(M2_G2_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M2_G2_END_LEVEL
        }
    },
    {
        .Type = M2_STEP3_TYPE,
        .Duration = ms_Conversion(M2_G2_STEP3_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = MIN_LEVEL_S16
        }
    }
};

/* Gear 3 */
const SpeedSeq_Step_t Mode2_Gear3_Steps[M2_NUM_STEPS] =
{
    {
        .Type = M2_STEP1_TYPE,
        .Duration = ms_Conversion(M2_G3_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Quad = {
            .StartValue   = M2_G3_START_LEVEL,
            .EndValue     = M2_G3_END_LEVEL,
            .IsIncreasing = M2_G3_ISINCREASING
        }
    },
    {
        .Type = M2_STEP2_TYPE,
        .Duration = ms_Conversion(M2_G3_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M2_G3_END_LEVEL
        }
    },
    {
        .Type = M2_STEP3_TYPE,
        .Duration = ms_Conversion(M2_G3_STEP3_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = MIN_LEVEL_S16
        }
    }
};

#endif // M2_NUM_STEPS > 0

/***********************************************************
 * Mode 3
 *  2 steps: Fixed, then near zero
 ***********************************************************/
#if M3_NUM_STEPS > 0

/* Gear 0 */
const SpeedSeq_Step_t Mode3_Gear0_Steps[M3_NUM_STEPS] =
{
    {
        .Type = M3_STEP1_TYPE,
        .Duration = ms_Conversion(M3_G0_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M3_G0_STEP1_LEVEL
        }
    },
    {
        .Type = M3_STEP2_TYPE,
        .Duration = ms_Conversion(M3_G0_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M3_G0_STEP2_LEVEL
        }
    }
};

/* Gear 1 */
const SpeedSeq_Step_t Mode3_Gear1_Steps[M3_NUM_STEPS] =
{
    {
        .Type = M3_STEP1_TYPE,
        .Duration = ms_Conversion(M3_G1_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M3_G1_STEP1_LEVEL
        }
    },
    {
        .Type = M3_STEP2_TYPE,
        .Duration = ms_Conversion(M3_G1_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M3_G1_STEP2_LEVEL
        }
    }
};

/* Gear 2 */
const SpeedSeq_Step_t Mode3_Gear2_Steps[M3_NUM_STEPS] =
{
    {
        .Type = M3_STEP1_TYPE,
        .Duration = ms_Conversion(M3_G2_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M3_G2_STEP1_LEVEL
        }
    },
    {
        .Type = M3_STEP2_TYPE,
        .Duration = ms_Conversion(M3_G2_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M3_G2_STEP2_LEVEL
        }
    }
};

/* Gear 3 */
const SpeedSeq_Step_t Mode3_Gear3_Steps[M3_NUM_STEPS] =
{
    {
        .Type = M3_STEP1_TYPE,
        .Duration = ms_Conversion(M3_G3_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M3_G3_STEP1_LEVEL
        }
    },
    {
        .Type = M3_STEP2_TYPE,
        .Duration = ms_Conversion(M3_G3_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M3_G3_STEP2_LEVEL
        }
    }
};

#endif // M3_NUM_STEPS > 0

/***********************************************************
 * Mode 4
 *  8 steps, alternating fixed/min, for 4 gears
 ***********************************************************/
#if M4_NUM_STEPS > 0

/* Gear 0 */
const SpeedSeq_Step_t Mode4_Gear0_Steps[M4_NUM_STEPS] =
{
    {
        .Type = M4_STEP1_TYPE,
        .Duration = ms_Conversion(M4_G0_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G0_STEP1_LEVEL
        }
    },
    {
        .Type = M4_STEP2_TYPE,
        .Duration = ms_Conversion(M4_G0_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G0_STEP2_LEVEL
        }
    },
    {
        .Type = M4_STEP3_TYPE,
        .Duration = ms_Conversion(M4_G0_STEP3_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G0_STEP3_LEVEL
        }
    },
    {
        .Type = M4_STEP4_TYPE,
        .Duration = ms_Conversion(M4_G0_STEP4_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G0_STEP4_LEVEL
        }
    },
    {
        .Type = M4_STEP5_TYPE,
        .Duration = ms_Conversion(M4_G0_STEP5_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G0_STEP5_LEVEL
        }
    },
    {
        .Type = M4_STEP6_TYPE,
        .Duration = ms_Conversion(M4_G0_STEP6_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G0_STEP6_LEVEL
        }
    },
    {
        .Type = M4_STEP7_TYPE,
        .Duration = ms_Conversion(M4_G0_STEP7_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G0_STEP7_LEVEL
        }
    },
    {
        .Type = M4_STEP8_TYPE,
        .Duration = ms_Conversion(M4_G0_STEP8_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G0_STEP8_LEVEL
        }
    }
};

/* Gear 1 */
const SpeedSeq_Step_t Mode4_Gear1_Steps[M4_NUM_STEPS] =
{
    {
        .Type = M4_STEP1_TYPE,
        .Duration = ms_Conversion(M4_G1_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G1_STEP1_LEVEL
        }
    },
    {
        .Type = M4_STEP2_TYPE,
        .Duration = ms_Conversion(M4_G1_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G1_STEP2_LEVEL
        }
    },
    {
        .Type = M4_STEP3_TYPE,
        .Duration = ms_Conversion(M4_G1_STEP3_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G1_STEP3_LEVEL
        }
    },
    {
        .Type = M4_STEP4_TYPE,
        .Duration = ms_Conversion(M4_G1_STEP4_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G1_STEP4_LEVEL
        }
    },
    {
        .Type = M4_STEP5_TYPE,
        .Duration = ms_Conversion(M4_G1_STEP5_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G1_STEP5_LEVEL
        }
    },
    {
        .Type = M4_STEP6_TYPE,
        .Duration = ms_Conversion(M4_G1_STEP6_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G1_STEP6_LEVEL
        }
    },
    {
        .Type = M4_STEP7_TYPE,
        .Duration = ms_Conversion(M4_G1_STEP7_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G1_STEP7_LEVEL
        }
    },
    {
        .Type = M4_STEP8_TYPE,
        .Duration = ms_Conversion(M4_G1_STEP8_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G1_STEP8_LEVEL
        }
    }
};

/* Gear 2 */
const SpeedSeq_Step_t Mode4_Gear2_Steps[M4_NUM_STEPS] =
{
    {
        .Type = M4_STEP1_TYPE,
        .Duration = ms_Conversion(M4_G2_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G2_STEP1_LEVEL
        }
    },
    {
        .Type = M4_STEP2_TYPE,
        .Duration = ms_Conversion(M4_G2_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G2_STEP2_LEVEL
        }
    },
    {
        .Type = M4_STEP3_TYPE,
        .Duration = ms_Conversion(M4_G2_STEP3_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G2_STEP3_LEVEL
        }
    },
    {
        .Type = M4_STEP4_TYPE,
        .Duration = ms_Conversion(M4_G2_STEP4_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G2_STEP4_LEVEL
        }
    },
    {
        .Type = M4_STEP5_TYPE,
        .Duration = ms_Conversion(M4_G2_STEP5_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G2_STEP5_LEVEL
        }
    },
    {
        .Type = M4_STEP6_TYPE,
        .Duration = ms_Conversion(M4_G2_STEP6_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G2_STEP6_LEVEL
        }
    },
    {
        .Type = M4_STEP7_TYPE,
        .Duration = ms_Conversion(M4_G2_STEP7_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G2_STEP7_LEVEL
        }
    },
    {
        .Type = M4_STEP8_TYPE,
        .Duration = ms_Conversion(M4_G2_STEP8_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G2_STEP8_LEVEL
        }
    }
};

/* Gear 3 */
const SpeedSeq_Step_t Mode4_Gear3_Steps[M4_NUM_STEPS] =
{
    {
        .Type = M4_STEP1_TYPE,
        .Duration = ms_Conversion(M4_G3_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G3_STEP1_LEVEL
        }
    },
    {
        .Type = M4_STEP2_TYPE,
        .Duration = ms_Conversion(M4_G3_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G3_STEP2_LEVEL
        }
    },
    {
        .Type = M4_STEP3_TYPE,
        .Duration = ms_Conversion(M4_G3_STEP3_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G3_STEP3_LEVEL
        }
    },
    {
        .Type = M4_STEP4_TYPE,
        .Duration = ms_Conversion(M4_G3_STEP4_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G3_STEP4_LEVEL
        }
    },
    {
        .Type = M4_STEP5_TYPE,
        .Duration = ms_Conversion(M4_G3_STEP5_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G3_STEP5_LEVEL
        }
    },
    {
        .Type = M4_STEP6_TYPE,
        .Duration = ms_Conversion(M4_G3_STEP6_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G3_STEP6_LEVEL
        }
    },
    {
        .Type = M4_STEP7_TYPE,
        .Duration = ms_Conversion(M4_G3_STEP7_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G3_STEP7_LEVEL
        }
    },
    {
        .Type = M4_STEP8_TYPE,
        .Duration = ms_Conversion(M4_G3_STEP8_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M4_G3_STEP8_LEVEL
        }
    }
};

#endif // M4_NUM_STEPS > 0

/***********************************************************
 * Mode 5
 *  5 steps with multiple sublevels, for 4 gears
 ***********************************************************/
#if M5_NUM_STEPS > 0

/* Gear 0 */
const SpeedSeq_Step_t Mode5_Gear0_Steps[M5_NUM_STEPS] =
{
    {
        .Type = M5_STEP1_TYPE,
        .Duration = ms_Conversion(M5_G0_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G0_LEVEL1
        }
    },
    {
        .Type = M5_STEP2_TYPE,
        .Duration = ms_Conversion(M5_G0_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G0_LEVEL2
        }
    },
    {
        .Type = M5_STEP3_TYPE,
        .Duration = ms_Conversion(M5_G0_STEP3_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G0_LEVEL3
        }
    },
    {
        .Type = M5_STEP4_TYPE,
        .Duration = ms_Conversion(M5_G0_STEP4_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G0_LEVEL4
        }
    },
    {
        .Type = M5_STEP5_TYPE,
        .Duration = ms_Conversion(M5_G0_STEP5_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G0_LEVEL0
        }
    }
};

/* Gear 1 */
const SpeedSeq_Step_t Mode5_Gear1_Steps[M5_NUM_STEPS] =
{
    {
        .Type = M5_STEP1_TYPE,
        .Duration = ms_Conversion(M5_G1_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G1_LEVEL1
        }
    },
    {
        .Type = M5_STEP2_TYPE,
        .Duration = ms_Conversion(M5_G1_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G1_LEVEL2
        }
    },
    {
        .Type = M5_STEP3_TYPE,
        .Duration = ms_Conversion(M5_G1_STEP3_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G1_LEVEL3
        }
    },
    {
        .Type = M5_STEP4_TYPE,
        .Duration = ms_Conversion(M5_G1_STEP4_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G1_LEVEL4
        }
    },
    {
        .Type = M5_STEP5_TYPE,
        .Duration = ms_Conversion(M5_G1_STEP5_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G1_LEVEL0
        }
    }
};

/* Gear 2 */
const SpeedSeq_Step_t Mode5_Gear2_Steps[M5_NUM_STEPS] =
{
    {
        .Type = M5_STEP1_TYPE,
        .Duration = ms_Conversion(M5_G2_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G2_LEVEL1
        }
    },
    {
        .Type = M5_STEP2_TYPE,
        .Duration = ms_Conversion(M5_G2_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G2_LEVEL2
        }
    },
    {
        .Type = M5_STEP3_TYPE,
        .Duration = ms_Conversion(M5_G2_STEP3_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G2_LEVEL3
        }
    },
    {
        .Type = M5_STEP4_TYPE,
        .Duration = ms_Conversion(M5_G2_STEP4_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G2_LEVEL4
        }
    },
    {
        .Type = M5_STEP5_TYPE,
        .Duration = ms_Conversion(M5_G2_STEP5_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G2_LEVEL0
        }
    }
};

/* Gear 3 */
const SpeedSeq_Step_t Mode5_Gear3_Steps[M5_NUM_STEPS] =
{
    {
        .Type = M5_STEP1_TYPE,
        .Duration = ms_Conversion(M5_G3_STEP1_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G3_LEVEL1
        }
    },
    {
        .Type = M5_STEP2_TYPE,
        .Duration = ms_Conversion(M5_G3_STEP2_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G3_LEVEL2
        }
    },
    {
        .Type = M5_STEP3_TYPE,
        .Duration = ms_Conversion(M5_G3_STEP3_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G3_LEVEL3
        }
    },
    {
        .Type = M5_STEP4_TYPE,
        .Duration = ms_Conversion(M5_G3_STEP4_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G3_LEVEL4
        }
    },
    {
        .Type = M5_STEP5_TYPE,
        .Duration = ms_Conversion(M5_G3_STEP5_DURATION_MS, PRODUCT_TASK_FREQUENCY),
        .Params.Fixed = {
            .Level = M5_G3_LEVEL0
        }
    }
};

#endif // M5_NUM_STEPS > 0

/***********************************************************
 * Mode Repeats
 ***********************************************************/
const uint32_t SpeedSeq_ModeRepeat[NUM_MODES] =
{
    M0_REPEAT,  // Mode 0
    M1_REPEAT,  // Mode 1
    M2_REPEAT,  // Mode 2
    M3_REPEAT,  // Mode 3
    M4_REPEAT,  // Mode 4
    M5_REPEAT   // Mode 5
};

/***********************************************************
 * Big array: SpeedSeq_ModeGearSteps[mode][gear]
 ***********************************************************/
const SpeedSeq_Step_t * const SpeedSeq_ModeGearSteps[NUM_MODES][NUM_GEARS] =
{
    /* Mode 0 */
    {
        Mode0_Gear0_Steps,
        Mode0_Gear1_Steps,
        Mode0_Gear2_Steps,
        Mode0_Gear3_Steps
    },
    /* Mode 1 */
    {
        Mode1_Gear0_Steps,
        Mode1_Gear1_Steps,
        Mode1_Gear2_Steps,
        Mode1_Gear3_Steps
    },
    /* Mode 2 */
    {
        Mode2_Gear0_Steps,
        Mode2_Gear1_Steps,
        Mode2_Gear2_Steps,
        Mode2_Gear3_Steps
    },
    /* Mode 3 */
    {
        Mode3_Gear0_Steps,
        Mode3_Gear1_Steps,
        Mode3_Gear2_Steps,
        Mode3_Gear3_Steps
    },
    /* Mode 4 */
    {
        Mode4_Gear0_Steps,
        Mode4_Gear1_Steps,
        Mode4_Gear2_Steps,
        Mode4_Gear3_Steps
    },
    /* Mode 5 */
    {
        Mode5_Gear0_Steps,
        Mode5_Gear1_Steps,
        Mode5_Gear2_Steps,
        Mode5_Gear3_Steps
    }
};

/***********************************************************
 * Big array: SpeedSeq_ModeGearNumSteps[mode][gear]
 ***********************************************************/
const uint8_t SpeedSeq_ModeGearNumSteps[NUM_MODES][NUM_GEARS] =
{
    /* Mode 0 */
    {M0_NUM_STEPS, M0_NUM_STEPS, M0_NUM_STEPS, M0_NUM_STEPS},
    /* Mode 1 */
    {M1_NUM_STEPS, M1_NUM_STEPS, M1_NUM_STEPS, M1_NUM_STEPS},
    /* Mode 2 */
    {M2_NUM_STEPS, M2_NUM_STEPS, M2_NUM_STEPS, M2_NUM_STEPS},
    /* Mode 3 */
    {M3_NUM_STEPS, M3_NUM_STEPS, M3_NUM_STEPS, M3_NUM_STEPS},
    /* Mode 4 */
    {M4_NUM_STEPS, M4_NUM_STEPS, M4_NUM_STEPS, M4_NUM_STEPS},
    /* Mode 5 */
    {M5_NUM_STEPS, M5_NUM_STEPS, M5_NUM_STEPS, M5_NUM_STEPS}
};

bool SpeedSeq_GetModeGearData(uint8_t mode,
                              uint8_t gear,
                              const SpeedSeq_Step_t **steps,
                              uint8_t *num_steps,
                              uint32_t *repeat)
{
    if ((steps == NULL) || (num_steps == NULL) || (repeat == NULL))
    {
        return false;
    }

    if ((mode >= NUM_MODES) || (gear >= NUM_GEARS))
    {
        return false;
    }

    *steps = SpeedSeq_ModeGearSteps[mode][gear];
    *num_steps = SpeedSeq_ModeGearNumSteps[mode][gear];
    *repeat = SpeedSeq_ModeRepeat[mode];

    return true;
}
