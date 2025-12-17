#include "speed_sequence.h"
#include "m_math.h" // Includes SinCos_Table and MMath_SinCos function

inline void SpeedSeq_StartTransition(SpeedSeq_Handle_t *this);
inline void SpeedSeq_CalculateTransitionGradient(SpeedSeq_Handle_t *this);
inline void SpeedSeq_UpdateTransition(SpeedSeq_Handle_t *this);
void SpeedSeq_Update(SpeedSeq_Handle_t *this);
void SpeedSeq_CalculateCurrentValue(SpeedSeq_Handle_t *this);

void SpeedSeq_Init(SpeedSeq_Handle_t *this, const SpeedSeq_Step_t *steps, uint8_t numSteps, uint32_t repeat)
{
  SpeedSeq_Reset(this);
  SpeedSeq_SetSequence(this, steps, numSteps, repeat);
}

void SpeedSeq_Reset(SpeedSeq_Handle_t *this)
{
  this->CurrentStep = 0;
  this->TimeInStep = 0;
  this->CurrentValue = 0;
  this->RepeatCounter = this->Repeat;
}

void SpeedSeq_SetSequence(SpeedSeq_Handle_t *this, const SpeedSeq_Step_t *steps, uint8_t numSteps, uint32_t repeat)
{
  if (this->Steps != steps)
  {
      SpeedSeq_StartTransition(this);
      this->CurrentStep = 0;
      this->TimeInStep = 0;
      this->RepeatCounter = repeat;
  }
  this->Steps = steps;
  this->NumSteps = numSteps;
  this->Repeat = repeat;
}

inline void SpeedSeq_StartTransition(SpeedSeq_Handle_t *this)
{
  this->Transition.RemainingTicks = this->Transition.TransitionTicks;
  this->Transition.PreviousValue = this->CurrentValue;
}

inline void SpeedSeq_CalculateTransitionGradient(SpeedSeq_Handle_t *this)
{
  SpeedSeq_CalculateCurrentValue(this);
  this->Transition.TargetValue = this->CurrentValue;

  int32_t wAux = this->Transition.TargetValue - this->Transition.PreviousValue;
  wAux <<= 16;
  wAux /= this->Transition.TransitionTicks;

  this->Transition.IncrementQ31 = wAux;
  this->CurrentValue = this->Transition.PreviousValue;
}

inline void SpeedSeq_UpdateTransition(SpeedSeq_Handle_t *this)
{
  int16_t IncrementQ15 = this->Transition.IncrementQ31 >> 16;

  int32_t wAux = this->Transition.IncrementQ31;
  wAux -= IncrementQ15 << 16;

  this->Transition.Error += wAux;

  int32_t ErrorQ15 = this->Transition.Error >> 16;
  if (ErrorQ15 != 0)
  {
      this->Transition.Error -= (ErrorQ15 << 16);
      IncrementQ15 += ErrorQ15;
  }

  this->Transition.IncrementQ15 = IncrementQ15;
  this->CurrentValue += this->Transition.IncrementQ15;
}

void SpeedSeq_Update(SpeedSeq_Handle_t *this)
{
  if (this->Transition.RemainingTicks == this->Transition.TransitionTicks)
  {
      SpeedSeq_CalculateTransitionGradient(this);
      if (this->Transition.RemainingTicks > 0)
      {
          this->Transition.RemainingTicks--;
      }
  }
  else if (this->Transition.RemainingTicks > 0)
  {
      SpeedSeq_UpdateTransition(this);
      this->Transition.RemainingTicks--;
  }
  else
  {
      SpeedSeq_CalculateCurrentValue(this);
  }
}

void SpeedSeq_CalculateCurrentValue(SpeedSeq_Handle_t *this)
{
  if (this->NumSteps == 0)
  {
      this->CurrentValue = 0;
      return;
  }

  if (this->CurrentStep >= this->NumSteps)
  {
      if (this->RepeatCounter == 0 || this->Repeat == 0)
      {
          this->CurrentValue = 0;
          return;
      }
      else
      {
          this->CurrentStep = 0;
          this->TimeInStep = 0;
          if (this->Repeat != UINT32_MAX)
          {
              this->RepeatCounter--;
          }
      }
  }

  const SpeedSeq_Step_t *currentStep = &this->Steps[this->CurrentStep];
  this->TimeInStep++;

  if (this->TimeInStep >= currentStep->Duration)
  {
      this->TimeInStep -= currentStep->Duration;
      this->CurrentStep++;

      if (this->CurrentStep >= this->NumSteps)
      {
          if (this->RepeatCounter == 0 || this->Repeat == 0)
          {
              this->CurrentValue = 0;
              return;
          }
          else
          {
              this->CurrentStep = 0;
              this->TimeInStep = 0;
              if (this->Repeat != UINT32_MAX)
              {
                  this->RepeatCounter--;
              }
              currentStep = &this->Steps[this->CurrentStep];
          }
      }
      else
      {
          currentStep = &this->Steps[this->CurrentStep];
      }
  }

  switch (currentStep->Type)
  {
  case SPEEDSEQ_TYPE_SINE:
      SpeedSeq_CalculateSine(this, currentStep);
      break;
  case SPEEDSEQ_TYPE_QUADRATIC:
      SpeedSeq_CalculateQuadratic(this, currentStep);
      break;
  case SPEEDSEQ_TYPE_FIXED:
      SpeedSeq_CalculateFixed(this, currentStep);
      break;
  case SPEEDSEQ_TYPE_LINE:
      SpeedSeq_CalculateLine(this, currentStep);
      break;
  default:
      this->CurrentValue = 0;
      break;
  }
}

int16_t SpeedSeq_GetCurrentValue(SpeedSeq_Handle_t *this)
{
    return this->CurrentValue;
}

/* Individual sequence calculation functions */

void SpeedSeq_CalculateSine(SpeedSeq_Handle_t *this, const SpeedSeq_Step_t *currentStep)
{
  uint32_t t = this->TimeInStep;
  uint16_t freq = currentStep->Params.Sine.Frequency;
  uint32_t anglePerTick = ((uint32_t)freq * 65536U) / 1000000; // Angle increment per tick
  uint32_t angle = (anglePerTick * t) % 65536U;

  int16_t sinValue = MMath_SinCos((int16_t)angle).Sin;

  int32_t value = ((int32_t)currentStep->Params.Sine.Amplitude * sinValue) >> 15;
  value += currentStep->Params.Sine.Offset;

  if (value > INT16_MAX)
      value = INT16_MAX;
  else if (value < INT16_MIN)
      value = INT16_MIN;

  this->CurrentValue = (int16_t)value;
}

void SpeedSeq_CalculateQuadratic(SpeedSeq_Handle_t *this, const SpeedSeq_Step_t *currentStep)
{
  int16_t startValue = currentStep->Params.Quad.StartValue;
  int16_t endValue = currentStep->Params.Quad.EndValue;
  uint32_t t = this->TimeInStep;
  uint32_t duration = currentStep->Duration;

  if (t > duration)
      t = duration;

  uint32_t uQ15 = (t * 32768U) / duration;

  uint32_t oneMinusUQ15 = 32768U - uQ15;
  uint32_t oneMinusUSquaredQ15 = (oneMinusUQ15 * oneMinusUQ15) >> 15;
  uint32_t scalingFactorQ15 = 32768U - oneMinusUSquaredQ15;

  int32_t deltaValue = (int32_t)endValue - (int32_t)startValue;
  int32_t value = (int32_t)startValue + ((deltaValue * (int32_t)scalingFactorQ15) >> 15);

  if (value > INT16_MAX)
      value = INT16_MAX;
  else if (value < INT16_MIN)
      value = INT16_MIN;

  this->CurrentValue = (int16_t)value;
}

void SpeedSeq_CalculateFixed(SpeedSeq_Handle_t *this, const SpeedSeq_Step_t *currentStep)
{
    this->CurrentValue = currentStep->Params.Fixed.Level;
}

void SpeedSeq_CalculateLine(SpeedSeq_Handle_t *this, const SpeedSeq_Step_t *currentStep)
{
  int16_t startValue = currentStep->Params.Line.StartValue;
  int16_t endValue = currentStep->Params.Line.EndValue;
  uint32_t duration = currentStep->Duration;
  uint32_t t = this->TimeInStep;

  if (t > duration)
      t = duration;

  int32_t value = startValue + ((int32_t)(endValue - startValue) * (int32_t)t) / (int32_t)duration;

  if (value > INT16_MAX)
      value = INT16_MAX;
  else if (value < INT16_MIN)
      value = INT16_MIN;

  this->CurrentValue = (int16_t)value;
}
