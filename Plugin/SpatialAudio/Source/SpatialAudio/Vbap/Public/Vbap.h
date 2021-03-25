#pragma once


class SPATIALAUDIO_API FVbap
{
public:
    
    FVbap();
    ~FVbap();
    
    size_t GetNumChannels() const;
    size_t GetDimension() const;
    
    char Prepare(size_t nls, const float* angles);
    void Perform(const float azimuth, const float elevation, float * gains);
    
private:
    
    size_t  v_nls;
    size_t  v_n;
    float*  v_matrices;
    size_t* v_indices;
    size_t  v_dim;
    
    
    static size_t GetMax(size_t value);
    static void Circumcircle(const float x1, const float y1, const float z1,
                                   const float x2, const float y2, const float z2,
                                   const float x3, const float y3, const float z3,
                                   float* xr, float* yr, float* zr, float* l);
};
