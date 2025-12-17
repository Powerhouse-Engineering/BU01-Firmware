#ifndef __M_MATH_H__
#define __M_MATH_H__

#include "m_type.h"

#define SQRT_2 (1.4142f)
#define SQRT_3 (1.732f)

#define divSQRT_3 ((int32_t)0x49E6) /* 1/sqrt(3) in q1.15 format=0.5773315*/

/** @brief  Calculate log2  */
#ifndef LOG2
#define LOG2(x) \
  (((x) == 65535 ) ? 16 : \
   (((x) == (2*2*2*2*2*2*2*2*2*2*2*2*2*2*2)) ? 15 : \
    (((x) == (2*2*2*2*2*2*2*2*2*2*2*2*2*2)) ? 14 : \
     (((x) == (2*2*2*2*2*2*2*2*2*2*2*2*2)) ? 13 : \
      (((x) == (2*2*2*2*2*2*2*2*2*2*2*2)) ? 12 : \
       (((x) == (2*2*2*2*2*2*2*2*2*2*2)) ? 11 : \
        (((x) == (2*2*2*2*2*2*2*2*2*2)) ? 10 : \
         (((x) == (2*2*2*2*2*2*2*2*2)) ? 9 : \
          (((x) == (2*2*2*2*2*2*2*2)) ? 8 : \
           (((x) == (2*2*2*2*2*2*2)) ? 7 : \
            (((x) == (2*2*2*2*2*2)) ? 6 : \
             (((x) == (2*2*2*2*2)) ? 5 : \
              (((x) == (2*2*2*2)) ? 4 : \
               (((x) == (2*2*2)) ? 3 : \
                (((x) == (2*2)) ? 2 : \
                 (((x) == 2) ? 1 : \
                  (((x) == 1) ? 0 : -1)))))))))))))))))
#endif

#define SATURATION_TO_S16(a) \
  if ((a) > 32767)           \
  {                          \
    (a) = 32767;             \
  }                          \
  else if ((a) < -32767)     \
  {                          \
    (a) = -32767;            \
  }                          \
  else                       \
  {                          \
  }

#define SIN_COS_TABLE                                                   \
  {                                                                     \
    0x0000, 0x00C9, 0x0192, 0x025B, 0x0324, 0x03ED, 0x04B6, 0x057F,     \
        0x0648, 0x0711, 0x07D9, 0x08A2, 0x096A, 0x0A33, 0x0AFB, 0x0BC4, \
        0x0C8C, 0x0D54, 0x0E1C, 0x0EE3, 0x0FAB, 0x1072, 0x113A, 0x1201, \
        0x12C8, 0x138F, 0x1455, 0x151C, 0x15E2, 0x16A8, 0x176E, 0x1833, \
        0x18F9, 0x19BE, 0x1A82, 0x1B47, 0x1C0B, 0x1CCF, 0x1D93, 0x1E57, \
        0x1F1A, 0x1FDD, 0x209F, 0x2161, 0x2223, 0x22E5, 0x23A6, 0x2467, \
        0x2528, 0x25E8, 0x26A8, 0x2767, 0x2826, 0x28E5, 0x29A3, 0x2A61, \
        0x2B1F, 0x2BDC, 0x2C99, 0x2D55, 0x2E11, 0x2ECC, 0x2F87, 0x3041, \
        0x30FB, 0x31B5, 0x326E, 0x3326, 0x33DF, 0x3496, 0x354D, 0x3604, \
        0x36BA, 0x376F, 0x3824, 0x38D9, 0x398C, 0x3A40, 0x3AF2, 0x3BA5, \
        0x3C56, 0x3D07, 0x3DB8, 0x3E68, 0x3F17, 0x3FC5, 0x4073, 0x4121, \
        0x41CE, 0x427A, 0x4325, 0x43D0, 0x447A, 0x4524, 0x45CD, 0x4675, \
        0x471C, 0x47C3, 0x4869, 0x490F, 0x49B4, 0x4A58, 0x4AFB, 0x4B9D, \
        0x4C3F, 0x4CE0, 0x4D81, 0x4E20, 0x4EBF, 0x4F5D, 0x4FFB, 0x5097, \
        0x5133, 0x51CE, 0x5268, 0x5302, 0x539B, 0x5432, 0x54C9, 0x5560, \
        0x55F5, 0x568A, 0x571D, 0x57B0, 0x5842, 0x58D3, 0x5964, 0x59F3, \
        0x5A82, 0x5B0F, 0x5B9C, 0x5C28, 0x5CB3, 0x5D3E, 0x5DC7, 0x5E4F, \
        0x5ED7, 0x5F5D, 0x5FE3, 0x6068, 0x60EB, 0x616E, 0x61F0, 0x6271, \
        0x62F1, 0x6370, 0x63EE, 0x646C, 0x64E8, 0x6563, 0x65DD, 0x6656, \
        0x66CF, 0x6746, 0x67BC, 0x6832, 0x68A6, 0x6919, 0x698B, 0x69FD, \
        0x6A6D, 0x6ADC, 0x6B4A, 0x6BB7, 0x6C23, 0x6C8E, 0x6CF8, 0x6D61, \
        0x6DC9, 0x6E30, 0x6E96, 0x6EFB, 0x6F5E, 0x6FC1, 0x7022, 0x7083, \
        0x70E2, 0x7140, 0x719D, 0x71F9, 0x7254, 0x72AE, 0x7307, 0x735E, \
        0x73B5, 0x740A, 0x745F, 0x74B2, 0x7504, 0x7555, 0x75A5, 0x75F3, \
        0x7641, 0x768D, 0x76D8, 0x7722, 0x776B, 0x77B3, 0x77FA, 0x783F, \
        0x7884, 0x78C7, 0x7909, 0x794A, 0x7989, 0x79C8, 0x7A05, 0x7A41, \
        0x7A7C, 0x7AB6, 0x7AEE, 0x7B26, 0x7B5C, 0x7B91, 0x7BC5, 0x7BF8, \
        0x7C29, 0x7C59, 0x7C88, 0x7CB6, 0x7CE3, 0x7D0E, 0x7D39, 0x7D62, \
        0x7D89, 0x7DB0, 0x7DD5, 0x7DFA, 0x7E1D, 0x7E3E, 0x7E5F, 0x7E7E, \
        0x7E9C, 0x7EB9, 0x7ED5, 0x7EEF, 0x7F09, 0x7F21, 0x7F37, 0x7F4D, \
        0x7F61, 0x7F74, 0x7F86, 0x7F97, 0x7FA6, 0x7FB4, 0x7FC1, 0x7FCD, \
        0x7FD8, 0x7FE1, 0x7FE9, 0x7FF0, 0x7FF5, 0x7FF9, 0x7FFD, 0x7FFE  \
  }

#define SIN_MASK 0x0300u
#define U0_90 0x0200u
#define U90_180 0x0300u
#define U180_270 0x0000u
#define U270_360 0x0100u

static const int16_t SinCos_Table[256] = SIN_COS_TABLE;

int32_t MMath_Sqrt(int32_t Input);

/* Note: if changing to int16_t for angle standard, need to
   reverse all the signs in the return values and also shift the
   angle by 32768 to get strictly positive index value.*/
static inline SinCos_t MMath_SinCos(int16_t Angle)
{
  uint16_t Index;
  SinCos_t Trig;

  /* 10 bit index computation  */
  Index = (uint16_t)(((int32_t)32768) + ((int32_t)Angle));
  Index /= ((uint16_t)64);

  switch (((uint16_t)Index) & SIN_MASK)
  {
  case U0_90:
  {
    Trig.Sin = SinCos_Table[(uint8_t)(Index)];
    Trig.Cos = SinCos_Table[(uint8_t)(0xFFu - (uint8_t)(Index))];
    break;
  }

  case U90_180:
  {
    Trig.Sin = SinCos_Table[(uint8_t)(0xFFu - (uint8_t)(Index))];
    Trig.Cos = -SinCos_Table[(uint8_t)(Index)];
    break;
  }

  case U180_270:
  {
    Trig.Sin = -SinCos_Table[(uint8_t)(Index)];
    Trig.Cos = -SinCos_Table[(uint8_t)(0xFFu - (uint8_t)(Index))];
    break;
  }

  case U270_360:
  {
    Trig.Sin = -SinCos_Table[(uint8_t)(0xFFu - (uint8_t)(Index))];
    Trig.Cos = SinCos_Table[(uint8_t)(Index)];
    break;
  }

  default:
  {
    break;
  }
  }
  return Trig;
}

static inline void MMath_S16Saturation(int32_t *Value)
{
  if (*Value > INT16_MAX)
  {
    *Value = INT16_MAX;
  }
  else if (*Value < -INT16_MAX)
  {
    *Value = -INT16_MAX;
  }
}

/**
 * @brief  It executes Modulus algorithm
 * @param  alpha component
 *         beta component
 * @retval int16_t Modulus
 */
static inline int16_t MMath_Modulus(int16_t alpha, int16_t beta)
{
  int32_t wAux1;
  int32_t wAux2;

  wAux1 = (int32_t)(alpha * alpha);
  wAux2 = (int32_t)(beta * beta);

  wAux1 += wAux2;
  wAux1 = MMath_Sqrt(wAux1);

  if (wAux1 > INT16_MAX)
  {
    wAux1 = (int32_t)INT16_MAX;
  }

  return ((int16_t)wAux1);
}

/**
 * @brief  This function transforms stator values a and b (which are
 *         directed along axes each displaced by 120 degrees) into values
 *         alpha and beta in a stationary qd reference frame.
 *                               alpha = a
 *                       beta = -(2*b + a) / sqrt(3)
 * @param  Input: stator values a and b in AB_t format
 * @retval Stator values alpha and beta in AlphaBeta_t format
 */
static inline void MMath_Clarke(AB_t *Input, AlphaBeta_t *Output)
{
  int32_t TmpA;
  int32_t TmpB;
  int32_t wTmpBeta;

  /* Ialpha = Ia */
  Output->Alpha = Input->a;

  TmpA = divSQRT_3 * ((int32_t)Input->a);
  TmpB = divSQRT_3 * ((int32_t)Input->b);

  /* Ibeta = -(2*Ib + Ia) / sqrt(3) */
  wTmpBeta = (-TmpA - TmpB - TmpB) / 32768;

  /* Check saturation of Ibeta
     We want output to be symmetrical around zero which is why
    on the lower bound check we use -32767 instead of -32768. */
  if (wTmpBeta > INT16_MAX)
  {
    Output->Beta = INT16_MAX;
  }
  else if (wTmpBeta < -INT16_MAX)
  {
    Output->Beta = -INT16_MAX;
  }
  else
  {
    Output->Beta = (int16_t)wTmpBeta;
  }
}

/**
 * @brief  This function transforms stator values alpha and beta, which
 *         belong to a stationary qd reference frame, to a rotor flux
 *         synchronous reference frame (properly oriented), so as q and d.
 *                   d = alpha * sin(theta) + beta * cos(Theta)
 *                   q = alpha * cos(Theta) - beta * sin(Theta)
 * @param  Input: stator values alpha and beta in alphabeta_t format
 * @param  Theta: rotating frame angular position in q1.15 format
 * @retval Stator values q and d in qd_t format
 */
static inline void MMath_Park(AlphaBeta_t *Input, QD_t *Output, int16_t Theta)
{
  int32_t wTmp1, wTmp2, wTmp3;
  SinCos_t Trig;

  Trig = MMath_SinCos(Theta);

  /* Calculate Iq. */
  wTmp1 = Input->Alpha * ((int32_t)Trig.Cos);
  wTmp2 = Input->Beta * ((int32_t)Trig.Sin);
  wTmp3 = (wTmp1 - wTmp2) / 32768;

  /* Check saturation of Iq */
  if (wTmp3 > INT16_MAX)
  {
    Output->q = INT16_MAX;
  }
  else if (wTmp3 < -INT16_MAX)
  {
    Output->q = -INT16_MAX;
  }
  else
  {
    Output->q = ((int16_t)wTmp3);
  }

  /* Calculate Id. */
  wTmp1 = Input->Alpha * ((int32_t)Trig.Sin);
  wTmp2 = Input->Beta * ((int32_t)Trig.Cos);
  wTmp3 = (wTmp1 + wTmp2) / 32768;

  /* Check saturation of Id */
  if (wTmp3 > INT16_MAX)
  {
    Output->d = INT16_MAX;
  }
  else if (wTmp3 < -INT16_MAX)
  {
    Output->d = -INT16_MAX;
  }
  else
  {
    Output->d = ((int16_t)wTmp3);
  }
}

/**
 * @brief  This function transforms stator voltage Vq and Vd from rotating
 *         reference frame to stationary reference frame, so as to obtain
 *         Valpha and Vbeta:
 *                  Valpha =  Vq*Cos(theta)+ Vd*Sin(theta)
 *                  Vbeta  = -Vq*Sin(theta)+ Vd*Cos(theta)
 * @param  Input: stator voltage Vq and Vd in QD_t format
 * @param  Theta: rotating frame angular position in u16 format
 * @retval Stator voltage Valpha and Vbeta in AlphaBeta_t format
 */
static inline void MMath_ReversePark(QD_t *Input, AlphaBeta_t *Output, int16_t Theta)
{
  int32_t AlphaTmp1;
  int32_t AlphaTmp2;
  int32_t BetaTmp1;
  int32_t BetaTmp2;
  SinCos_t Trig;

  Trig = MMath_SinCos(Theta);

  AlphaTmp1 = Input->q * ((int32_t)Trig.Cos);
  AlphaTmp2 = Input->d * ((int32_t)Trig.Sin);

  Output->Alpha = (int16_t)(((AlphaTmp1) + (AlphaTmp2)) / 32768);

  BetaTmp1 = Input->q * ((int32_t)Trig.Sin);
  BetaTmp2 = Input->d * ((int32_t)Trig.Cos);

  Output->Beta = (int16_t)((BetaTmp2 - BetaTmp1) / 32768);
}

#define ATAN1DIV1 (int16_t)8192
#define ATAN1DIV2 (int16_t)4836
#define ATAN1DIV4 (int16_t)2555
#define ATAN1DIV8 (int16_t)1297
#define ATAN1DIV16 (int16_t)651
#define ATAN1DIV32 (int16_t)326
#define ATAN1DIV64 (int16_t)163
#define ATAN1DIV128 (int16_t)81
#define ATAN1DIV256 (int16_t)41
#define ATAN1DIV512 (int16_t)20
#define ATAN1DIV1024 (int16_t)10
#define ATAN1DIV2048 (int16_t)5
#define ATAN1DIV4096 (int16_t)3
#define ATAN1DIV8192 (int16_t)1

static inline int16_t MMath_ArcTan2(int32_t Ealpha, int32_t Ebeta)
{
  int16_t Angle;
  int32_t Xi, Yi, Xold;

  /* Determining quadrant */
  if (Ealpha < 0)
  {
    if (Ebeta < 0)
    {
      /* Quadrant III, add 90 degrees so as to move to quadrant IV */
      Angle = 16384;
      Xi = -(Ebeta >> 1);
      Yi = Ealpha >> 1;
    }
    else
    {
      /* Quadrant II, subtract 90 degrees so as to move to quadrant I */
      Angle = -16384;
      Xi = Ebeta >> 1;
      Yi = -(Ealpha >> 1);
    }
  }
  else
  {
    /* Quadrant I or IV */
    Angle = 0;
    Xi = Ealpha >> 1;
    Yi = Ebeta >> 1;
  }
  Xold = Xi;

  /* Begin the successive approximation process */
  /* Iteration 0 */
  if (Yi < 0)
  {
    /* Vector is in Quadrant IV */
    Angle += ATAN1DIV1;
    Xi = Xi - Yi;
    Yi = Xold + Yi;
  }
  else
  {
    /* Vector is in Quadrant I */
    Angle -= ATAN1DIV1;
    Xi = Xi + Yi;
    Yi = -Xold + Yi;
  }
  Xold = Xi;

  /* Iteration 1 */
  if (Yi < 0)
  {
    /* Vector is in Quadrant IV */
    Angle += ATAN1DIV2;
    Xi = Xi - (Yi >> 1);
    Yi = (Xold >> 1) + Yi;
  }
  else
  {
    /* Vector is in Quadrant I*/
    Angle -= ATAN1DIV2;
    Xi = Xi + (Yi >> 1);
    Yi = (-Xold >> 1) + Yi;
  }
  Xold = Xi;

  /* Iteration 2 */
  if (Yi < 0)
  {
    /* Vector is in Quadrant IV */
    Angle += ATAN1DIV4;
    Xi = Xi - (Yi >> 2);
    Yi = (Xold >> 2) + Yi;
  }
  else
  {
    /* Vector is in Quadrant I */
    Angle -= ATAN1DIV4;
    Xi = Xi + (Yi >> 2);
    Yi = (-Xold >> 2) + Yi;
  }
  Xold = Xi;

  /* Iteration 3 */
  if (Yi < 0)
  {
    /* Vector is in Quadrant IV */
    Angle += ATAN1DIV8;
    Xi = Xi - (Yi >> 3);
    Yi = (Xold >> 3) + Yi;
  }
  else
  {
    /* Vector is in Quadrant I */
    Angle -= ATAN1DIV8;
    Xi = Xi + (Yi >> 3);
    Yi = (-Xold >> 3) + Yi;
  }
  Xold = Xi;

  /* Iteration 4 */
  if (Yi < 0)
  {
    /* Vector is in Quadrant IV */
    Angle += ATAN1DIV16;
    Xi = Xi - (Yi >> 4);
    Yi = (Xold >> 4) + Yi;
  }
  else
  {
    /* Vector is in Quadrant I */
    Angle -= ATAN1DIV16;
    Xi = Xi + (Yi >> 4);
    Yi = (-Xold >> 4) + Yi;
  }
  Xold = Xi;

  /* Iteration 5 */
  if (Yi < 0)
  {
    /* Vector is in Quadrant IV */
    Angle += ATAN1DIV32;
    Xi = Xi - (Yi >> 5);
    Yi = (Xold >> 5) + Yi;
  }
  else
  {
    /* Vector is in Quadrant I */
    Angle -= ATAN1DIV32;
    Xi = Xi + (Yi >> 5);
    Yi = (-Xold >> 5) + Yi;
  }
  Xold = Xi;

  /* Iteration 6 */
  if (Yi < 0)
  {
    /* Vector is in Quadrant IV */
    Angle += ATAN1DIV64;
    Xi = Xi - (Yi >> 6);
    Yi = (Xold >> 6) + Yi;
  }
  else
  {
    /* Vector is in Quadrant I */
    Angle -= ATAN1DIV64;
    Xi = Xi + (Yi >> 6);
    Yi = (-Xold >> 6) + Yi;
  }
  Xold = Xi;

  /* Iteration 7 */
  if (Yi < 0)
  {
    /* Vector is in Quadrant IV */
    Angle += ATAN1DIV128;
    Xi = Xi - (Yi >> 7);
    Yi = (Xold >> 7) + Yi;
  }
  else
  {
    /* Vector is in Quadrant I */
    Angle -= ATAN1DIV128;
    Xi = Xi + (Yi >> 7);
    Yi = (-Xold >> 7) + Yi;
  }

  return -Angle;
}

/********************** MMATH2 Library  *********************************/
/* Alternative Clark/Park transform, that are more efficient. */

/* MMATH2_ONE_DIV_SQRT3_S16 define, the value is: 32768 / sqrt(3) = 18919 = 0x49E7 */
#define MMATH2_ONE_DIV_SQRT3_S16  (Q15_t)18919
/* MMATH2_SQRT3_DIV_2_S16 define, the value is: 32768 * sqrt(3) / 2 = 28378 = 0x6EDA */
#define MMATH2_SQRT3_DIV_2_S16    (Q15_t)28378

/*             d = beta * cos(theta) + alpha * sin(theta)
*              q = alpha * cos(theta) - beta * sin(theta)
*******************************************************************************/
static inline void MMATH2_Park_Sat_S16(const AlphaBeta_t *In, const SinCos_t *Trig, QD_t *Out)
{
  register Q31_t s16A;
  register Q31_t s16B;
  register Q31_t s16Sin;
  register Q31_t s16Cos;
  register Q31_t s32Temp;

  /* Store values for calculation */
  s16A = (In->Alpha);
  s16B = (In->Beta);
  s16Sin = (Trig->Sin);
  s16Cos = (Trig->Cos);

  /* Calculate d = beta * cos(theta) + alpha * sin(theta) */
  s32Temp = (s16Cos * s16B + s16Sin * s16A) >> 15;
  /* Result limitation to 16-bit range*/
  s32Temp = (s32Temp < -32768) ? INT16_MIN : s32Temp;
  s32Temp = (s32Temp > 32767) ? INT16_MAX : s32Temp;
  Out->d = (Q15_t)s32Temp;

  /* Calculate q = alpha * cos(theta) - beta * sin(theta) */
  s32Temp = ((s16Cos * s16A) - (s16Sin * s16B)) >> 15;
  /* Result limitation to 16-bit range */
  s32Temp = (s32Temp < -32768) ? INT16_MIN : s32Temp;
  s32Temp = (s32Temp > 32767) ? INT16_MAX : s32Temp;
  Out->q = (Q15_t)s32Temp;
}

/*             d = beta * cos(theta) + alpha * sin(theta)
*              q = alpha * cos(theta) - beta * sin(theta)
*******************************************************************************/
static inline void MMATH2_Park_S16(const AlphaBeta_t *In, const SinCos_t *Trig, QD_t *Out)
{
  register Q31_t s16A;
  register Q31_t s16B;
  register Q31_t s16Sin;
  register Q31_t s16Cos;
  register Q31_t s32Temp;

  /* Store values for calculation */
  s16A = (In->Alpha);
  s16B = (In->Beta);
  s16Sin = (Trig->Sin);
  s16Cos = (Trig->Cos);
  /* Calculate d = beta * cos(theta) + alpha * sin(theta) */
  s32Temp = (s16Cos * s16B + s16Sin * s16A) >> 15;
  Out->d = (Q15_t)s32Temp;

  /* Calculate q = alpha * cos(theta) - beta * sin(theta) */
  s32Temp = ((s16Cos * s16A) - (s16Sin * s16B)) >> 15;
  Out->q = (Q15_t)s32Temp;
}

/*              alpha = d * sin(theta) + q * cos(theta)
*               beta  = d * cos(theta) - q * sin(theta)
*******************************************************************************/
static inline void MMATH2_InvPark_Sat_S16(const QD_t *In, const SinCos_t *Trig, AlphaBeta_t *Out)
{
  register Q31_t s16D;
  register Q31_t s16Q;
  register Q31_t s16Sin;
  register Q31_t s16Cos;
  register Q31_t s32Temp;

  /* Store values for calculation */
  s16D = (In->d);
  s16Q = (In->q);
  s16Sin = (Trig->Sin);
  s16Cos = (Trig->Cos);

  /* Calculate beta  = d * cos(theta) - q * sin(theta) */
  s32Temp = ((s16Cos * s16D) - (s16Sin * s16Q)) >> 15;
  /* Result limitation to 16-bit range*/
  s32Temp = (s32Temp < -32768) ? INT16_MIN : s32Temp;
  s32Temp = (s32Temp > 32767) ? INT16_MAX : s32Temp;
  Out->Beta = (Q15_t)s32Temp;

  /* Calculate alpha = d * sin(theta) + q * cos(theta) */
  s32Temp = ((s16Sin * s16D) + (s16Cos * s16Q)) >> 15;
  /* Result limitation to 16-bit range*/
  s32Temp = (s32Temp < -32768) ? INT16_MIN : s32Temp;
  s32Temp = (s32Temp > 32767) ? INT16_MAX : s32Temp;
  Out->Alpha = (Q15_t)s32Temp;
}

/*              alpha = d * sin(theta) + q * cos(theta)
*               beta  = d * cos(theta) - q * sin(theta)
*******************************************************************************/

static inline void MMATH2_InvPark_S16(const QD_t *In, const SinCos_t *Trig, AlphaBeta_t *Out)
{
  register Q31_t s16D;
  register Q31_t s16Q;
  register Q31_t s16Sin;
  register Q31_t s16Cos;
  register Q31_t s32Temp;

  /* Store values for calculation */
  s16D = (In->d);
  s16Q = (In->q);
  s16Sin = (Trig->Sin);
  s16Cos = (Trig->Cos);

  /* Calculate beta  = d * cos(theta) - q * sin(theta) */
  s32Temp = ((s16Cos * s16D) - (s16Sin * s16Q)) >> 15;
  Out->Beta = (Q15_t)s32Temp;

  /* Calculate alpha = d * sin(theta) + q * cos(theta) */
  s32Temp = ((s16Sin * s16D) + (s16Cos * s16Q)) >> 15;
  Out->Alpha = (Q15_t)s32Temp;
}

/* Modifies the structure pointed by Out pointer according to the following equations:
*
*          alpha = a
*          beta  = c * 1 / sqrt(3)  - b * 1 / sqrt(3)
****************************************************************************/
static inline void MMATH2_Clark_Sat_S16(const ABC_t *In, AlphaBeta_t *Out)
{
  register Q31_t s32Temp;

  /* Calculate Alpha orthogonal coordinate [alpha = a] */
  Out->Alpha = In->a;

  /* Calculate Beta orthogonal coordinate [beta  = c * 1 / sqrt(3)  - b * 1 / sqrt(3)
  and right shift to 16-bit range */
  s32Temp = ((In->c - In->b) * MMATH2_ONE_DIV_SQRT3_S16) >> 15;

  /* Result limitation to 16-bit range*/
  s32Temp = (s32Temp < INT16_MIN) ? INT16_MIN : s32Temp;
  s32Temp = (s32Temp > INT16_MAX) ? INT16_MAX : s32Temp;
  Out->Beta = s32Temp;
}

/*              a = alpha
*               b = -0.5 * alpha - sgrt(3) / 2 * beta
*               c = -0.5 * alpha + sgrt(3) / 2 * beta
****************************************************************************/
static inline void MMATH2_InvClark_Sat_S16(const AlphaBeta_t *In, ABC_t *Out)
{
  register Q31_t s16M1;
  register Q31_t s16M2;
  register Q31_t s32Temp;

  /* Store constant -0.5*alpha to s16M1 */
  s16M1 = -(In->Alpha >> 1);
  /* Store constant beta * sqrt(3) / 2 shifted right to s16 range as s16M2 */
  s16M2 = (In->Beta * MMATH2_SQRT3_DIV_2_S16) >> 15;

  /* Calculate a coordinate [a = alpha] */
  Out->a = In->Alpha;

  /* Calculate b coordinate [b = -alpha/2 - beta * sqrt(3) / 2  ] */
  s32Temp = s16M1 - s16M2;
  /* Result limitation to 16-bit range*/
  s32Temp = (s32Temp < INT16_MIN) ? INT16_MIN : s32Temp;
  s32Temp = (s32Temp > INT16_MAX) ? INT16_MAX : s32Temp;
  Out->b = s32Temp;

  /* Calculate c coordinate [c = -alpha/2 + beta * sqrt(3) / 2  ] */
  s32Temp = s16M1 + s16M2;
  /* Result limitation to 16-bit range*/
  s32Temp = (s32Temp < INT16_MIN) ? INT16_MIN : s32Temp;
  s32Temp = (s32Temp > INT16_MAX) ? INT16_MAX : s32Temp;
  Out->c = s32Temp;
}

#endif
