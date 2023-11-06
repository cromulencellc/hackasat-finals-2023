#pragma once

#include <memory>
#include <unordered_map>
#include <list>
#include <utility>

namespace cromulence
{
        template<typename T, typename U, bool (*Func)(T)> bool wrapper( U  item  )
        {
            return Func( *item->second  );
        }

    // a queue with multiple indexing strategies
    template< typename KeyType,typename ItemType  > class key_queue
    {
        
        typedef std::shared_ptr< ItemType > ItemPtr;
        typedef std::pair< KeyType , ItemPtr  > KeyPair;
        typedef std::shared_ptr< KeyPair > KeyPairPtr;


    public:
        key_queue()
        {

        }
        ~key_queue()
        {

        }

        void push( const KeyType &key,  const ItemType &item )
        {
            ItemPtr i = std::make_shared< ItemType >(  );
            *i = item;
            KeyPair pair( key , i );
            KeyPairPtr e = std::make_shared< KeyPair >(pair );
            list_.push_back( e );
            map_.insert( {key , e} );
        }
        void pop( )
        {
            KeyPairPtr f = list_.front();
            //
            map_.erase( f->first );
            list_.pop_front();
        }
        void clear() 
        {
            map_.clear();
            list_.clear();
        }
        size_t size( )
        {
            return list_.size();
        }
        bool contains( KeyType key )
        {
            return map_.contains(key);
        }

        void remove(  KeyType key )
        {
            KeyPairPtr item;
            item = map_.at(key);

            // remove from both
            list_.remove( item );
            map_.erase( key );
        }
        ItemPtr get( KeyType key )
        {
            return map_[key]->second;
        }
        ItemPtr get( int index )
        {
            auto it = list_.begin();
            std::advance(it, index);
            KeyPairPtr k = *it;
            return k->second;
        }
        KeyType getKey( int index )
        {
            auto it = list_.begin();
            std::advance(it, index);
            KeyPairPtr k = *it;
            return k->first;
        }
        ItemPtr front()
        {
            return list_.front()->second;
        }
        typedef bool (*ItemMatcher)(ItemType);
        template<ItemMatcher Func> KeyType first_match( )
        {
            KeyType out;
            auto it  = std::find_if( list_.begin() , list_.end()   ,wrapper<ItemType,KeyPairPtr,Func> );
            if( it != list_.end() )
            {
                KeyPairPtr p = *it;
                out = p->first;
            }
            return out;
        }
        template<ItemMatcher Func> size_t count_match( )
        {
            auto it  = std::count_if( list_.begin() , list_.end()   ,wrapper<ItemType,KeyPairPtr,Func> );
        
            return static_cast<size_t>(it);
        }
        
    protected:
        

        std::unordered_map< KeyType , KeyPairPtr > map_;
        std::list<KeyPairPtr> list_;
    };
}