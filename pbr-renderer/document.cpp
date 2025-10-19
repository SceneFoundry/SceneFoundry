#include "framework.h"
#include "document.h"
#include "impact.h"
#include "application.h"
#include "acme/filesystem/filesystem/file_context.h"


namespace SceneFoundry_pbr_renderer
{


   document::document()
   {

   }


   document::~document()
   {

   }


   bool document::on_new_document()
   {

      //::file::path path;
      //
      //::pointer < ::SceneFoundry_pbr_renderer::application > papp = application();
      //
      //path = papp->m_pathApplicationText;

      ////const char * pszPath = path.c_str();
      //
      //try
      //{

      //   file()->get_lines(m_straLine, path);
      //   
      //}
      //catch(...)
      //{
      // 
      //}


      //if (m_straLine.is_empty())
      //{
      //   
      //   try
      //   {
      //      
      //      file()->get_lines(m_straLine, "document://application/app-simple/application/application.txt");
      //      
      //   }
      //   catch (...)
      //   {
      //   
      //      
      //   }

      //}

      //   


      return true;
      //return ::user::document::on_new_document();

   }


   bool document::on_open_document(const ::payload & payloadFile)
   {

      impact * pimpact = get_typed_impact < impact >();

      if(pimpact == NULL)
      {

         return true;

      }

      return true;

   }


#ifdef _DEBUG


   long long document::increment_reference_count()
   {

      return  ::user::document::increment_reference_count();

   }


   long long document::decrement_reference_count()
   {

      return  ::user::document::decrement_reference_count();

   }


#endif


} // namespace SceneFoundry_pbr_renderer



