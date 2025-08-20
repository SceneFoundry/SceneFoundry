#pragma once
#include "SceneFoundry/sandbox/window_input.h"

#include "SceneFoundry/sandbox/scene.h"
#include "SceneFoundry/sandbox/entity.h"
#include "SceneFoundry/sandbox_game/player.h"
#include "SceneFoundry/sandbox_engine/asset_manager.h"
#include "SceneFoundry/sandbox_game/game_object.h"
//#include <memory>
//#include <vector>
//////#include <stdexcept>
//#include <fstream>
#include <optional>


namespace sandbox_game
{


	class sandbox_scene :
		virtual public sandbox::IScene
	{
	public:


		::pointer<::sandbox::IWindowInput> m_pInput;
		::pointer < ::sandbox_engine::asset_manager > m_passetmanager;

		::pointer_array_base<sandbox_player> m_players;
		::map<unsigned int, ::pointer<IGameObject>>  m_gameObjects;
		glm::vec3 m_initialCameraPosition{ 0.f };
		glm::vec3 m_initialCameraRotation{ 0.f };

		std::optional<uint32_t> m_skyboxId;
		::pointer<IGameObject> m_skyboxObject;
		::string m_skyboxCubemapName = "skybox_hdr";


		sandbox_scene();

		// pass input so your Player can read it
		void initialize_sandbox_scene(IWindowInput * input, ::sandbox_engine::asset_manager * passetmanager);


		void init() override;                 // load models, spawn entities
		void update(float dt) override;        // advance all entities

		void loadSceneFile(const ::scoped_string& fileName);

		::graphics3d::scene_object::map * getGameObjects() override ;


		::pair<glm::mat4, glm::mat4> getMainCameraMatrices()const;

		void setSkyboxObject(::pointer<IGameObject> obj) {
			m_skyboxId = obj->getId();
			m_skyboxObject = std::move(obj);
		}

		sandbox_camera& getCamera();

		void addGameObject(uint32_t id, sandbox_game_object obj);
		void removeGameObject(uint32_t id);

		std::optional<std::reference_wrapper<IGameObject>>
			getSkyboxObject() const override;

		std::optional<std::reference_wrapper<sandbox_game_object>>
			getSkyboxObject();

		::string getSkyboxCubemapName() const {
			return m_skyboxCubemapName;
		}


	};


} // namespace sandbox_game



