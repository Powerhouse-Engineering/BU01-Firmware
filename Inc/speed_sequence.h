#ifndef __SPEED_SEQUENCE_H__
#define __SPEED_SEQUENCE_H__

#include "common_type.h"
#include "m_math.h" // Assuming this includes the necessary math functions

typedef enum {
  SPEEDSEQ_TYPE_SINE = 0,
  SPEEDSEQ_TYPE_QUADRATIC = 1,
  SPEEDSEQ_TYPE_FIXED = 2,
  SPEEDSEQ_TYPE_LINE = 3,
} SPEEDSEQ_t;

typedef struct
{
  int16_t Offset;       /*!< Offset value of the sine wave */
  int16_t Amplitude;    /*!< Amplitude of the sine wave */
  uint16_t Frequency;   /*!< Frequency in mHz */
} SpeedSeq_SineParams_t;

typedef struct
{
  int16_t StartValue;   /*!< Starting value of the quadratic sequence */
  int16_t EndValue;     /*!< Ending value of the quadratic sequence */
  bool IsIncreasing;    /*!< Direction of the quadratic curve */
} SpeedSeq_QuadParams_t;

typedef struct
{
  int16_t StartValue;    /*!< Starting value */
  int16_t EndValue;      /*!< Ending value */
} SpeedSeq_LineParams_t;

typedef struct
{
  int16_t Level;        /*!< Fixed level value */
} SpeedSeq_FixedParams_t;

typedef struct
{
  uint8_t Type;         /*!< Type of the sequence: sine, quadratic, fixed, line */
  uint32_t Duration;    /*!< Total duration of this sequence step in milliseconds */
  union
  {
    SpeedSeq_SineParams_t Sine;
    SpeedSeq_QuadParams_t Quad;   // Changed from Exp to Quad
    SpeedSeq_FixedParams_t Fixed;
    SpeedSeq_LineParams_t Line;
  } Params;             /*!< Parameters specific to the sequence type */
} SpeedSeq_Step_t;

typedef struct
{
  /******************* Configuration Parameters *******************/

  uint16_t TransitionTicks; /*!< Default ticks to transition between different sequences */
  
  /***************************************************************/

  int16_t PreviousValue;

  int16_t TargetValue;

  int32_t IncrementQ31;

  int16_t IncrementQ15;

  int32_t Error;

  uint16_t RemainingTicks;

} SpeedSeq_Transition_t;

typedef struct
{
  const SpeedSeq_Step_t *Steps;  /*!< Array of sequence steps */
  uint8_t NumSteps;              /*!< Number of steps in the sequence */
  uint8_t CurrentStep;           /*!< Current step index */
  uint32_t TimeInStep;           /*!< Time elapsed in the current step in milliseconds */
  int16_t CurrentValue;          /*!< Current speed reference value */
  uint32_t Repeat;               /*!< Number of times to repeat the sequence */
  uint32_t RepeatCounter;        /*!< Counter for repetitions */
  SpeedSeq_Transition_t Transition; /*!< To handle transitions between sequences */
} SpeedSeq_Handle_t;

/* Function prototypes */
void SpeedSeq_Init(SpeedSeq_Handle_t *this, const SpeedSeq_Step_t *steps, uint8_t numSteps, uint32_t repeat);
void SpeedSeq_SetSequence(SpeedSeq_Handle_t *this, const SpeedSeq_Step_t *steps, uint8_t numSteps, uint32_t repeat);
void SpeedSeq_Reset(SpeedSeq_Handle_t *this);
void SpeedSeq_Update(SpeedSeq_Handle_t *this);
int16_t SpeedSeq_GetCurrentValue(SpeedSeq_Handle_t *this);

/* Inline functions for each sequence type */
void SpeedSeq_CalculateSine(SpeedSeq_Handle_t *this, const SpeedSeq_Step_t *currentStep);
void SpeedSeq_CalculateQuadratic(SpeedSeq_Handle_t *this, const SpeedSeq_Step_t *currentStep); // Changed
void SpeedSeq_CalculateFixed(SpeedSeq_Handle_t *this, const SpeedSeq_Step_t *currentStep);
void SpeedSeq_CalculateLine(SpeedSeq_Handle_t *this, const SpeedSeq_Step_t *currentStep);

#endif // __SPEED_SEQUENCE_H__
