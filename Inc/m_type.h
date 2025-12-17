#ifndef __MC_TYPE_H__
#define __MC_TYPE_H__

#include "common_type.h"

#define DIRECTION_CW (int8_t)(1)
#define DIRECTION_CCW (int8_t)(-1)

/** @name Fault source error codes */
/** @{ */
#define MC_NO_FAULTS ((uint16_t)0x0000)      /**< 0 | @brief No error.*/
#define MC_FOC_DURATION ((uint16_t)0x0001)   /**< 1 | @brief Error: FOC rate to high.*/
#define MC_OVER_VOLT ((uint16_t)0x0002)      /**< 2 | @brief Error: over voltage.*/
#define MC_UNDER_VOLT ((uint16_t)0x0004)     /**< 4 | @brief Error: under voltage.*/
#define MC_OVER_TEMP ((uint16_t)0x0008)      /**< 8 | @brief Error: over temperature.*/
#define MC_OVER_CURR ((uint16_t)0x0010)      /**< 16 | @brief Error: Overcurrent.*/
#define MC_START_UP ((uint16_t)0x0020)       /**< 32 | @brief Error: Startup failed.*/
#define MC_SPEED_FDBK_1 ((uint16_t)0x0040)   /**< 64 | @brief Error: Speed feedback 1.*/
#define MC_SPEED_FDBK_2 ((uint16_t)0x0080)   /**< 128 | @brief Error: Speed feedback 2.*/
#define MC_BREAK_IN ((uint16_t)0x0100)       /**< 256 | @brief Error: Emergency input (Over current).*/
#define MC_SW_ERROR ((uint16_t)0x0200)       /**< 512 | @brief Software Error.*/
#define MC_OFFSET_CAL ((uint16_t)0x0400)     /**< 1024 | @brief Offset calibration failed.*/

#define S16_180DEGREES ((int16_t)32768)
#define S16_150DEGREES ((int16_t)27307)
#define S16_120DEGREES ((int16_t)21845)
#define S16_90DEGREES  ((int16_t)16384)
#define S16_60DEGREES  ((int16_t)10923)
#define S16_30DEGREES  ((int16_t)5461)
#define S16_15DEGREES  ((int16_t)2731)

#define M_PI (3.14159265358979323846f)
#define M_SQRT2 (1.41421356237f)
#define M_SQRT3 (1.73205080757f)

#define MOTOR_DIRECTION_CW ((int8_t)1)
#define MOTOR_DIRECTION_CCW ((int8_t)-1)

/** @brief Not initialized pointer */
#define M_NULL (void *)(0x0)

typedef struct
{
  int16_t Alpha;
  int16_t Beta;
} AlphaBeta_t;

/** @brief q, d component type definition */
typedef struct
{
  int16_t q;
  int16_t d;
} QD_t;

typedef struct
{
  int16_t a;
  int16_t b;
} AB_t;

typedef struct
{
  int16_t a;
  int16_t b;
  int16_t c;
} ABC_t;

/**
 * @brief  Trigonometrical functions type definition
 */
typedef struct
{
  int16_t Cos;
  int16_t Sin;
} SinCos_t;

typedef struct
{
  AlphaBeta_t Valphabeta;
  AlphaBeta_t ValphabetaPrev;
  QD_t Vqd;

  AB_t Iab;
  AlphaBeta_t Ialphabeta;
  QD_t Iqd;

  int16_t ThetaEl;

  QD_t Iqdref;

  int16_t Tref;

} FOC_Handle_t;

typedef enum
{
  MotorPhase_U = 0,
  MotorPhase_V,
  MotorPhase_W,
} MotorPhase_t;

typedef struct
{
  int16_t Vphase;
  int16_t Isum;
  int16_t ThetaEl;
} SIXSTEP_Handle_t;

#endif
