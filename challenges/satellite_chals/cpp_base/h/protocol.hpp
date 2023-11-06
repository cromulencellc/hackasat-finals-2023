#pragma once
#include <string>
namespace cromulence::protocol
{
    class Item
    {
    public:
        Item();
        ~Item();
        virtual bool transmit( void* data , size_t size )=0;
        virtual size_t receive( void* data, size_t size ) =0;
        void setBelow( Item *item )
        {
            below = item;
        }
        void setAbove( Item *item )
        {
            above = item;
        }
    protected:
        Item *below;
        Item *above;
        
    };

    class Stack
    {
    public:
        Stack();
        ~Stack();
        void transmit( );
        void add_top( Item *top );
        void add_bottom( Item *bottom );
    protected:
        Item *top_;
        Item *bottom_;
    };
}