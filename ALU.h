#pragma once
#include <iostream>
#include <cstdint>
#include "OpCodes.h"
using namespace std;

class ALU
{
private:
    static uint8_t _AddTwos(uint8_t val1, uint8_t val2)
    {
        return val1 + val2;
    }

    static uint8_t _AddFloat(uint8_t val1, uint8_t val2)
    {
        // 1.Handling zero inputs like 1000 0000 or 0000 0000
        if ((val1 & 0x7F) == 0) return val2;
        if ((val2 & 0x7F) == 0) return val1;

        // 2.Unpacking (sign 1 bit, exponent 3 bits, mantissa 4 bits)
        uint8_t sign1 = (val1 & 0x80) >> 7;
        uint8_t sign2 = (val2 & 0x80) >> 7;
        uint8_t exponent1 = (val1 & 0x70) >> 4;
        uint8_t exponent2 = (val2 & 0x70) >> 4;
        uint8_t mantissa1 = val1 & 0x0F;
        uint8_t mantissa2 = val2 & 0x0F;

        // 3.Alignment
        while (exponent1 < exponent2)
        {
            exponent1 += 1;
            mantissa1 >>= 1;
        }
        while (exponent1 > exponent2)
        {
            exponent2 += 1;
            mantissa2 >>= 1;
        }

        uint8_t resSign, resMantissa, resExponent = exponent1;

        // 4.Add / Subtract mantissa
        if (sign1 == sign2)
        {
            resMantissa = mantissa1 + mantissa2;
            resSign = sign1;
        }
        else if (mantissa1 >= mantissa2)
        {
            resMantissa = mantissa1 - mantissa2;
            resSign = sign1;
        }
        else 
        {
            resMantissa = mantissa2 - mantissa1;
            resSign = sign2;
        }

        // 5.overflow normalization (shift right if mantissa overflowed 4 bits)
        if (resMantissa > 0x0F)
        {
            resMantissa >>= 1;
            resExponent++;
        }

        // 6.underflow normalization (shift left if leading bit is not set, adjust exponent down)
        // resMantissa must be 1 in 0000 1000 
        while ((resMantissa & 0x08) == 0 && resExponent > 0)
        {
            resMantissa <<= 1;
            resExponent--;
        }
        //  resExponent take 3 bits (max = 7)
        if (resExponent > 7) resExponent = 7;

        // 7. repacking
        resSign <<= 7;
        resExponent = (resExponent & 0x07) << 4;
        resMantissa &= 0x0F;
        return resSign | resExponent | resMantissa;
    }
    
    static uint8_t _And(uint8_t val1, uint8_t val2)
    {
        // only 1 & 1 = 1
        return val1 & val2;
    }
    
    static uint8_t _Or(uint8_t val1, uint8_t val2)
    {
        // only 0 | 0 = 0
        return val1 | val2;
    }
    
    static uint8_t _Xor(uint8_t val1, uint8_t val2)
    {
        // only 0 ^ 1 = 1 ^ 0 = 1
        return val1 ^ val2;
    }
    
    static uint8_t _RotateRight(uint8_t val, uint8_t steps)
    {
        steps %= 8;
        // Circular Rotate Logic
        // 0001 1001 rotate 1 steps = 1000 1100 
        // 0001 1001 >> 1 = 0000 1100
        // or gate
        // 0001 1001 << (8 - 1) = 1000 0000
        // result = 1000 1100
        return _Or((val >> steps), (val << (8 - steps)));
    }

public:
    static uint8_t Execute(OpCode opCode, uint8_t val1, uint8_t val2)
    {
        uint8_t result;
        switch (opCode)
        {
        case OpCode::AddTwos:
            result = _AddTwos(val1, val2);
            break;

        case OpCode::AddFloat:
            result = _AddFloat(val1, val2);
            break;

        case OpCode::BitwiseAnd:
            result = _And(val1, val2);
            break;

        case OpCode::BitwiseOr:
            result = _Or(val1, val2);
            break;

        case OpCode::BitwiseXor:
            result = _Xor(val1, val2);
            break;

        case OpCode::Rotate:
            result = _RotateRight(val1, val2);
            break;

        default:
            result = 0x00;
            break;
        }
        return result;
    }
};
