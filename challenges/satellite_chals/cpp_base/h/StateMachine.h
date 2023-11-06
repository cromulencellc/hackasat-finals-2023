#pragma once
#include <string> 
#include <map>
namespace cromulence
{
template<typename T, typename U , typename V=std::string > class State
{
    public:
        State( V stateid ) :
            in_(nullptr),
            out_(nullptr),
            stateId_(stateid)
        {}
        virtual ~State() {}
        V getId( )
        {
            return stateId_;
        }
        void link( T *in , U *out)
        {
            in_ =in ;
            out_ = out;
        }
        virtual V update()=0;
        virtual void enter() = 0;
    protected:
        T *in_;
        U *out_;
        V stateId_;
};

template< typename T, typename U , typename V=std::string> class StateMachine
{
    public:
    StateMachine( )  : current_(nullptr)

    {
        stateMap_.clear();
    }
    ~StateMachine()
    {

    }
    void add(  State<T,U> *state)
    {
        state->link( &in_ , &out_  );
        std::pair< V , State<T,U>*> i( state->getId() ,state);
        stateMap_.insert( i );
    }
    void forceState(V name )
    {
        current_ = stateMap_.at( name );
        
        next_ = name;
    }
    V update( )
    {

        
        
        try
        {
            current_ = stateMap_.at( next_ );
            if( next_ != lastId_  )
            {
                // the state changed
                current_->enter();   
            }
            lastId_ = next_;
            next_ = current_->update();
            
        }
        catch(...)
        {
            lastId_ = "ERROR";
        }
        return lastId_;
    }
    V currentState( )
    {
        return lastId_;
    }
    T in_;
    U out_;
    protected:
        State<T,U> *current_;

        V lastId_;
        V next_;
        std::map<V , State<T,U>* >stateMap_; 
};
}