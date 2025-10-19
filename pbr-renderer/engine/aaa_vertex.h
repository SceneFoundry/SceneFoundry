#pragma once


namespace SceneFoundry_pbr_renderer
{
   /**
    * Vertex attributes.
    */
   struct Vertex
   {
      glm::vec3 mPosition;
      glm::vec3 mNormal;
      glm::vec2 mTextureCoordinates;
      glm::vec3 mTangent;
      glm::vec3 mBitangent;
   };


} // namespace SceneFoundry_pbr_renderer
 