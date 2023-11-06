#pragma once

#include <memory>

    template< typename T, size_t START_BIT , size_t N_BITS > T UnpackBits( T in )
    {
        T out;

        T mask(0);
        mask = ( 1 << ( N_BITS )) - 1; // create the mask with no shift
        mask = mask << START_BIT; // shift the mask 
        out = ( in & mask ) >> START_BIT;
        return out;
    }

    template< typename T , size_t START_BIT , size_t N_BITS > T PackBits( T in )
    {
        T out;
        T mask(0);
        mask = ( 1 << N_BITS) - 1;
        out = ( in & mask ) << START_BIT;
        return out;
    }

    template< typename T > double FixedToDouble(  void *ptr , double SCALE, double OFFST=0.0 )
    {
        T typed = *reinterpret_cast<T*>(ptr);
        double out;
        out = SCALE*(typed) + OFFSET;
        return out;
    }