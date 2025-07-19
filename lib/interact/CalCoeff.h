#ifndef __CAL_COEFF_H__
#define __CAL_COEFF_H__

#include "DCL.h"
#include "DCLF32.h"

void computeDF22_PRcontrollerCoeff(DCL_DF22 *v, float32_t kp, float32_t ki,
                                   float32_t wo, float32_t fs, float32_t wrc);

void computeDF22_NotchFltrCoeff(DCL_DF22 *v, float32_t Fs, float32_t notch_freq,
                                float32_t c1, float32_t c2);

#endif // !__CAL_COEFF_H__