#pragma once


#include "SceneFoundry/sandbox/entity.h"
#include "SceneFoundry/sandbox/game_object.h"
#include "SceneFoundry/sandbox/model.h"
#include "SceneFoundry/sandbox_renderer/object.h"
#include <glm/gtc/matrix_transform.hpp>

// std
//#include <memory>
//#include <unordered_map>
//////#include <stdexcept>

namespace sandbox_game
{


   class game_object :
   virtual public ::sandbox::IGameObject
   {
   public:


      ::sandbox::TransformComponent m_transform;
      ::pointer<::sandbox::IModel> m_pmodel;
      glm::vec3 m_color{};
      bool m_bIsOBJ{ false };
      ::sandbox::point_light_component m_pointlight;

      ::string m_cubemapTextureName;
      long long m_llId;
      bool m_bIsSkybox = false;



      game_object();
      // sandbox_game_object() = default;
      // sandbox_game_object(const sandbox_game_object&) = delete;
      // sandbox_game_object& operator=(const sandbox_game_object&) = delete;
      // sandbox_game_object(sandbox_game_object&&) = default;
      // sandbox_game_object& operator=(sandbox_game_object&&) = default;


      void initialize_game_object(long long llId);

      long long getId() const override { return m_llId; }

      glm::vec3 getColor() const override { return m_color; }

      const ::sandbox::point_light_component * getPointLight() const override {
         return &m_pointlight;
      }

      ::sandbox::TransformComponent& getTransform() override {
         return m_transform;
      }

      ::sandbox::IModel * getModel() const override {
         return m_pmodel;
      }

      void setModel(const ::sandbox::IModel * pmodel) {
         m_pmodel = pmodel;
      }



   };


} // namespace sandbox_game


