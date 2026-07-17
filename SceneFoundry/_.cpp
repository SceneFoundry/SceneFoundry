#include "framework.h"
#include "acme/_library.h"
#include "acme/platform/release_time.h"


namespace SceneFoundry_SceneFoundry
{


   ::pointer_array < ::string_array > parse_comma_separated_file(const ::scoped_string & scopedstrCsv)
   {

      ::pointer_array < ::string_array > straa;
      ::pointer < ::string_array > pstra;
      ::string strField;
      bool bQuoted = false;
      bool bRecordStarted = false;

      auto add_field = [&]()
      {

         if (!pstra)
         {

            pstra = straa.main_add();

         }

         pstra->add(strField);
         strField.empty();
         bRecordStarted = true;

      };

      auto finish_record = [&]()
      {

         add_field();
         pstra.release();
         bRecordStarted = false;

      };

      for (::collection::index i = 0; i < scopedstrCsv.size(); ++i)
      {

         const auto ch = scopedstrCsv[i];

         if (bQuoted)
         {

            if (ch == '"')
            {

               if (i + 1 < scopedstrCsv.size() && scopedstrCsv[i + 1] == '"')
               {

                  strField += '"';
                  ++i;

               }
               else
               {

                  bQuoted = false;

               }

            }
            else
            {

               strField += ch;

            }

         }
         else if (ch == '"' && strField.is_empty())
         {

            bQuoted = true;
            bRecordStarted = true;

         }
         else if (ch == ',')
         {

            add_field();

         }
         else if (ch == '\r' || ch == '\n')
         {

            finish_record();

            if (ch == '\r' && i + 1 < scopedstrCsv.size() && scopedstrCsv[i + 1] == '\n')
            {

               ++i;

            }

         }
         else
         {

            strField += ch;
            bRecordStarted = true;

         }

      }

      if (bRecordStarted || strField.has_character() || pstra)
      {

         finish_record();

      }

      return straa;

   }


} // namespace SceneFoundry_SceneFoundry


#define APPLICATION_NAMESPACE SceneFoundry_SceneFoundry
#include "acme/inline/application/main_application_library.inl"


__BEGIN_RELEASE_TIME(SceneFoundry_SceneFoundry)
#include "release_time.txt"
__END_RELEASE_TIME()



