#include "CalCoeff.h"

void computeDF22_PRcontrollerCoeff(DCL_DF22 *v, float32_t kp, float32_t ki,
                                   float32_t wo, float32_t fs, float32_t wrc)
{
    float32_t temp1, temp2, wo_adjusted;
    wo_adjusted = 2.0f*fs*tanf(wo/(2.0f*fs));

    temp1 = 4.0f*fs*fs+wo_adjusted*wo_adjusted+4.0f*fs*wrc;
    temp2 = 4.0f*ki*wrc*fs/temp1;
    v->b0 = temp2;
    v->b1 = 0;
    v->b2=-temp2;
    v->a1=((-8.0f*fs*fs+2*wo_adjusted*wo_adjusted)/temp1);
    v->a2=((temp1-8.0f*fs*wrc)/temp1);
    v->x1 = 0;
    v->x2 = 0;

    if(kp!=0)
    {
        v->b0+=kp;
        v->b1+=kp*v->a1;
        v->b2+=kp*v->a2;
    }

    v->a1=(v->a1);
    v->a2=(v->a2);
}

void computeDF22_NotchFltrCoeff(DCL_DF22 *v, float32_t Fs, float32_t notch_freq,
                                float32_t c1, float32_t c2)
{
    float32_t temp1;
    float32_t temp2;
    float32_t wn2;
    float32_t Ts;
    Ts = 1.0f/Fs;

    // pre warp the notch frequency
    wn2 = 2.0f*Fs*tanf(notch_freq*CONST_PI_32*Ts);  /////////// Here there was a PI, not able to localized in the project and used directly CONST_PI_32!!!!!!/////

    temp1= 4.0f*Fs*Fs + 4.0f* wn2 * c2 * Fs + wn2*wn2;
    temp2= 1.0f/ (4.0f*Fs*Fs + 4.0f* wn2 * c1 * Fs + wn2*wn2);

    v->b0 = temp1* temp2;
    v->b1 = (-8.0f*Fs*Fs + 2.0f* wn2* wn2)* temp2;
    v->b2 = (4.0f*Fs*Fs-4*wn2*c2*Fs+wn2*wn2)*temp2;
    v->a1 = (-8.0f*Fs*Fs + 2.0f* wn2* wn2)*temp2;
    v->a2 = (4.0f*Fs*Fs-4.0f*wn2*c1*Fs+wn2*wn2)*temp2;

}