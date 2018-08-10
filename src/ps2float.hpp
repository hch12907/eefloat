#pragma once

#include <limits>

class Float 
{
public:
    Float(float val) : value(val) {};

    Float operator+(const Float& rhs);
    Float operator-(const Float& rhs);
    Float operator*(const Float& rhs);
    Float operator/(const Float& rhs);
    
    bool is_inf() const;
    bool is_nan() const;
    float get_ieee() const;
    unsigned char get_exp() const;
    int get_mantissa() const;
    bool get_sign() const;

protected:
    float value;

    double promote() const;
    Float depromote(double val) const;
};