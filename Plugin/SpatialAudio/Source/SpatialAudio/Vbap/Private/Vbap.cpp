#include "../Public/Vbap.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdint.h>

#define VBAP_DEG_TO_RAD_F 0.01745329238474369049072266f


FVbap::FVbap()
    : v_nls(0)
    , v_n(0)
    , v_matrices(nullptr)
    , v_indices(nullptr)
    , v_dim(0)
{
}

FVbap::~FVbap()
{ }

size_t FVbap::GetNumChannels() const
{
    return this->v_nls;
}
size_t FVbap::GetDimension() const
{
    return this->v_dim;
}

char FVbap::Prepare(size_t nls, const float* angles)
{
    char valid, err = 0;
    float x1, y1, z1, x2, y2, z2, x3, y3, z3, xc, yc, zc, xr, yr, zr, dc, deta, ra, re;
    size_t i = 0, j = 0, k = 0, l = 0;
    size_t const max = GetMax(nls);
    
    this->v_dim     = 3;
    if (this->v_n && this->v_matrices)
    {
        delete[] this->v_matrices;
    }
    if (this->v_n && this->v_indices)
    {
        delete[] this->v_indices;
    }
    
    this->v_nls      = nls;
    this->v_n        = 0;
    this->v_matrices = new float[max * 9]();
    this->v_indices  = new size_t[max * 3]();
    
    if (!this->v_matrices || !this->v_indices)
    {
        // LCOV_EXCL_START
        if (this->v_indices)
        {
            delete[] this->v_indices;
            this->v_indices = nullptr;
        }
        if (this->v_matrices)
        {
            delete[] this->v_matrices;
            this->v_matrices = nullptr;
        }
        return -1;
        // LCOV_EXCL_STOP
    }
    
    for (i = 0; i < nls - 2; ++i)
    {
        for (j = i+1; j < nls - 1; ++j)
        {
            for (k = j+1; k < nls; ++k)
            {
                ra = angles[i*2]   * VBAP_DEG_TO_RAD_F;
                re = angles[i*2+1] * VBAP_DEG_TO_RAD_F;
                x1 = -sinf(ra) * cosf(re);
                y1 = cosf(ra)  * cosf(re);
                z1 = sinf(re);
                ra = angles[j*2]   * VBAP_DEG_TO_RAD_F;
                re = angles[j*2+1] * VBAP_DEG_TO_RAD_F;
                x2 = -sinf(ra) * cosf(re);
                y2 = cosf(ra)  * cosf(re);
                z2 = sinf(re);
                ra = angles[k*2]   * VBAP_DEG_TO_RAD_F;
                re = angles[k*2+1] * VBAP_DEG_TO_RAD_F;
                x3 = -sinf(ra) * cosf(re);
                y3 = cosf(ra)  * cosf(re);
                z3 = sinf(re);
                
                Circumcircle(x3, y3, z3, x1, y1, z1, x2, y2, z2, &xr, &yr, &zr, &dc);
                valid = (char)(sqrtf(xr * xr + yr * yr + zr * zr) > FLT_EPSILON);
                
                for (l = 0; l < nls && valid; ++l)
                {
                    if (l != i && l != j && l != k)
                    {
                        ra = angles[l*2]   * VBAP_DEG_TO_RAD_F;
                        re = angles[l*2+1] * VBAP_DEG_TO_RAD_F;
                        xc = -sinf(ra) * cosf(re) - xr;
                        yc = cosf(ra) * cosf(re) - yr;
                        zc = sinf(re) - zr;
                        
                        valid = (char)((sqrtf(xc * xc + yc * yc + zc * zc) + FLT_EPSILON) > dc);
                    }
                }
                if (valid)
                {
                    deta = x1 * y2 * z3 + y1 * z2 * x3 + z1 * x2 * y3 - z1 * y2 * x3 - x1 * z2 * y3 - y1 * x2 * z3;
                    if (fabsf(deta) < FLT_EPSILON)
                    {
                        deta = 1.f;
                        err++;
                    }
                    this->v_indices[this->v_n*3]   = i;
                    this->v_indices[this->v_n*3+1] = j;
                    this->v_indices[this->v_n*3+2] = k;
                    
                    this->v_matrices[this->v_n*9]   = (y2 * z3 - z2 * y3) / deta;
                    this->v_matrices[this->v_n*9+1] = (z1 * y3 - y1 * z3) / deta;
                    this->v_matrices[this->v_n*9+2] = (y1 * z2 - z1 * y2) / deta;
                    this->v_matrices[this->v_n*9+3] = (z2 * x3 - x2 * z3) / deta;
                    this->v_matrices[this->v_n*9+4] = (x1 * z3 - z1 * x3) / deta;
                    this->v_matrices[this->v_n*9+5] = (z1 * x2 - x1 * z2) / deta;
                    this->v_matrices[this->v_n*9+6] = (x2 * y3 - y2 * x3) / deta;
                    this->v_matrices[this->v_n*9+7] = (y1 * x3 - x1 * y3) / deta;
                    this->v_matrices[this->v_n*9+8] = (x1 * y2 - y1 * x2) / deta;
                    this->v_n++;
                }
            }
        }
    }
    
    return err;
}

void FVbap::Perform(const float azimuth, const float elevation, float * gains)
{
    size_t i = 0, index = SIZE_MAX;
    float r1, r2, r3, s1 = 0.f, s2 = 0.f, s3 = 0.f, powr, ref = 0.f;
    const float ra = azimuth * VBAP_DEG_TO_RAD_F;
    const float re = elevation * VBAP_DEG_TO_RAD_F;
    const float x = -sinf(ra) * cosf(re);
    const float y = cosf(ra) * cosf(re);
    const float z = sinf(re);
    
    for (i = 0; i < this->v_n; ++i)
    {
        r1 = this->v_matrices[i*9] * x + this->v_matrices[i*9+3] * y + this->v_matrices[i*9+6] * z;
        r2 = this->v_matrices[i*9+1] * x + this->v_matrices[i*9+4] * y + this->v_matrices[i*9+7] * z;
        r3 = this->v_matrices[i*9+2] * x + this->v_matrices[i*9+5] * y + this->v_matrices[i*9+8] * z;
        if (r1 >= 0.f && r2 >= 0.f && r3 >= 0.f)
        {
            powr = sqrtf(r1 * r1 + r2 * r2 + r3 * r3);
            if (powr > ref)
            {
                ref = powr;
                index = i;
                s1 = r1;
                s2 = r2;
                s3 = r3;
            }
        }
    }
    for (i = 0; i < this->v_nls; ++i)
    {
        gains[i] = 0.f;
    }
    
    if (ref != 0.f)
    {
        gains[this->v_indices[index*3]]   = s1 / ref;
        gains[this->v_indices[index*3+1]] = s2 / ref;
        gains[this->v_indices[index*3+2]] = s3 / ref;
    }
}






size_t FVbap::GetMax(size_t value)
{
    size_t count = 0;
    for (size_t i = 0; i < value - 2; ++i)
    {
        for (size_t j = i+1; j < value - 1; ++j)
        {
            for (size_t k = j+1; k < value; ++k)
            {
                count += 9;
            }
        }
    }
    return count * 2;
}

void FVbap::Circumcircle(const float x1, const float y1, const float z1,
                               const float x2, const float y2, const float z2,
                               const float x3, const float y3, const float z3,
                               float* xr, float* yr, float* zr, float* l)
{
    const double mx1 = (double)x2 - (double)x1, my1 = (double)y2 - (double)y1, mz1 = (double)z2 - (double)z1;
    const double mx2 = (double)x3 - (double)x1, my2 = (double)y3 - (double)y1, mz2 = (double)z3 - (double)z1;
    const double crx1 = my1 * mz2 - mz1 * my2, cry1 = mz1 * mx2 - mx1 * mz2, crz1 = mx1 * my2 - my1 * mx2;
    
    const double l1 = pow(sqrt(crx1*crx1+cry1*cry1+crz1*crz1), 2.) * 2.;
    const double l2 = pow(sqrt(mx1*mx1+my1*my1+mz1*mz1), 2.);
    const double l3 = pow(sqrt(mx2*mx2+my2*my2+mz2*mz2), 2.);
    
    const double crx2 = (cry1 * mz1 - crz1 * my1) * l3, cry2 = (crz1 * mx1 - crx1 * mz1) * l3, crz2 = (crx1 * my1 - cry1 * mx1) * l3;
    const double crx3 = (my2 * crz1 - mz2 * cry1) * l2, cry3 = (mz2 * crx1 - mx2 * crz1) * l2, crz3 = (mx2 * cry1 - my2 * crx1) * l2;

    *xr = (float)((crx2 + crx3) / l1); *yr = (float)((cry2 + cry3) / l1); *zr = (float)((crz2 + crz3) / l1);
    *l = sqrtf(*xr * *xr + *yr * *yr + *zr * *zr);
    *xr += x1; *yr += y1; *zr += z1;
}
