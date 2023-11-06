#pragma once



namespace cromulence
{
namespace io
{

    template<typename T> class RegisterOperator
    {
    public:
        RegisterOperator();
        ~RegisterOperator();

        size_t read();
        size_t write( )
    }
    template<typename T> class Register
    {
    public:
        Register( );
        ~Register();
    protected:
        RegisterOperator *op_;
    };

    template<typename T> class BitField
    {
    public:
        BitField( );
        ~BitField( );

        write(const T& in);
        read( );
    
    };

    

    template<typename REGISTERS > class RegisterWithFields : FIELDS
    {

    };

}

}