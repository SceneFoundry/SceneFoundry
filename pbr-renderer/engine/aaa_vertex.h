#pragma once


namespace SceneFoundry_pbr_renderer
{
   /**
    * Vertex attributes.
    */
   struct Vertex
   {
      floating_sequence3 mPosition;
      floating_sequence3 mNormal;
      floating_sequence2 mTextureCoordinates;
      floating_sequence3 mTangent;
      floating_sequence3 mBitangent;
   };


} // namespace SceneFoundry_pbr_renderer
 