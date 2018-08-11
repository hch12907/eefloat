#include "ps2float.hpp"

#define LIKELY(cond) (__builtin_expect(!!(cond), 1))
#define UNLIKELY(cond) (__builtin_expect(!!(cond), 0))

Float Float::operator+(const Float& rhs) 
{
    if UNLIKELY(this->is_inf() || rhs.is_inf())
    {
        
        return Float(std::numeric_limits<float>::infinity()); 
    }
        
    if UNLIKELY(this->is_nan() || rhs.is_nan() || this->is_subnormal() || rhs.is_subnormal())
    {
        const double a = this->promote();
        const double b = rhs.promote();
        return Float::depromote(a + b);
    }

    return Float(this->get_ieee() + rhs.get_ieee());
}

Float Float::operator-(const Float& rhs)
{
    if UNLIKELY(this->is_nan() || this->is_inf() || rhs.is_nan() || rhs.is_inf() || this->is_subnormal() || rhs.is_subnormal())
    {
        const double a = this->promote();
        const double b = rhs.promote();
        return Float::depromote(a - b);
    }

    return Float::depromote(this->get_ieee() - rhs.get_ieee());
}

Float Float::operator*(const Float& rhs) 
{
    if UNLIKELY(this->get_exp() + rhs.get_exp() >= 255 || this->is_inf() || rhs.is_inf())
    {
        return Float(std::numeric_limits<float>::infinity()); 
    }

    if UNLIKELY(this->is_nan() || rhs.is_nan() || this->is_subnormal() || rhs.is_subnormal())
    {
        const double a = this->promote();
        const double b = rhs.promote();
        return Float::depromote(a + b);
    }

    return Float(this->get_ieee() * rhs.get_ieee());
}

Float Float::operator/(const Float& rhs) 
{
    if UNLIKELY(this->is_nan() || this->is_inf() || rhs.is_nan() || rhs.is_inf() || this->is_subnormal() || rhs.is_subnormal())
    {
        const double a = this->promote();
        const double b = rhs.promote();
        return Float::depromote(a / b);
    }

    return Float(this->get_ieee() / rhs.get_ieee());
}

bool Float::is_inf() const
{
    return *reinterpret_cast<const unsigned int*>(&this->value) == 0xFFFFFFFF;
}

bool Float::is_nan() const
{
    return get_exp() == 0xFF && !is_inf();
}

bool Float::is_subnormal() const
{
    return get_exp() == 0;
}

float Float::get_ieee() const
{
    return this->value;
}

unsigned char Float::get_exp() const
{
    const unsigned int u = *reinterpret_cast<const unsigned int*>(&this->value);
    return (u >> 23) & 0xFF;
}

int Float::get_mantissa() const
{
    const unsigned int u = *reinterpret_cast<const unsigned int*>(&this->value);
    return u & ((1 << 23)-1);
}

bool Float::get_sign() const
{
    const unsigned int u = *reinterpret_cast<const unsigned int*>(&this->value);
    return (u >> 31) & 1;
}

// Manually cast the Single into a Double, as casting infinites or NaNs to
// other types causes UB
double Float::promote() const
{
    const long long new_sign = this->get_sign();
    const long long new_exp = this->get_exp() - 127 + 1023;
    const long long new_mantissa = (long long)this->get_mantissa() << 29;

    const long long promoted = new_mantissa | (new_exp << 52) | (new_sign << 63);
    return *reinterpret_cast<const double*>(&promoted);
    
}

Float Float::depromote(double val) const 
{
    const unsigned long long u = *reinterpret_cast<const unsigned long long*>(&val);
    // get sign
    const int new_sign = (u >> 63) & 1;
    // get exponent, do the bias
    const int new_exp = ((u >> 52) & 0x7FF) - 1023 + 127;
    // get mantissa, shift it by 29 bits
    const int new_mantissa = (u & ((1LL << 52) - 1)) >> 29;

    if UNLIKELY(new_exp >= 255) 
    { 
        float sign = new_sign ? -1 : 0;
        return Float(sign * std::numeric_limits<float>::infinity()); 
    }
    
    if UNLIKELY(new_exp <= 0)
    {
        float result = new_sign ? -0.0f : 0.0f;
        return Float(result);
    }

    const int depromoted = new_mantissa | (new_exp << 23) | (new_sign << 31);
    return Float(*reinterpret_cast<const float*>(&depromoted));
}