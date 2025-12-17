#ifndef __SPEED_SEQUENCE_DATA_H__
#define __SPEED_SEQUENCE_DATA_H__

#include <stdbool.h>
#include "speed_sequence.h"
#include "speed_sequence_config.h"

extern const uint32_t SpeedSeq_ModeRepeat[NUM_MODES];
extern const SpeedSeq_Step_t * const SpeedSeq_ModeGearSteps[NUM_MODES][NUM_GEARS];
extern const uint8_t SpeedSeq_ModeGearNumSteps[NUM_MODES][NUM_GEARS];

bool SpeedSeq_GetModeGearData(uint8_t mode,
                              uint8_t gear,
                              const SpeedSeq_Step_t **steps,
                              uint8_t *num_steps,
                              uint32_t *repeat);

#endif // __SPEED_SEQUENCE_DATA_H__
