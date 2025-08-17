#pragma once


#include "acme/prototype/collection/list.h"


template < typename TYPE >
class class_set_base :
   public ::list_base < TYPE >
{
public:

   using BASE_LIST = ::list_base < TYPE >;

   using iterator = typename BASE_LIST::iterator;

   using ARG_TYPE = ::argument_of< TYPE >;

   using BASE_LIST::BASE_LIST;
   using BASE_LIST::operator =;


   void add(ARG_TYPE t)
   {

      iterator p;

      if (find_first_greater_skip_equal(p, t))
      {

         return;

      }

      this->insert_before(p, t);

   }


   iterator find(ARG_TYPE t) const
   {

      auto p = this->begin();

      while (this->is_ok(p))
      {

         if (t == *p)
         {

            return p;

         }

         p++;

      }

      return this->end();

   }

   
   bool find_first_greater_skip_equal(iterator & p, ARG_TYPE t) const
   {

      p = this->begin();

      while (this->is_end(p))
      {

         if (t < *p)
         {

            break;

         }

         p++;

      }

      if (t == *p)
      {

         return true;

      }

      while (this->is_end(p))
      {

         if (t != *p)
         {

            break;

         }

         p++;

      }

      return false;

   }


   bool has(ARG_TYPE t) const
   {

      return this->find(t) != nullptr;

   }


};





