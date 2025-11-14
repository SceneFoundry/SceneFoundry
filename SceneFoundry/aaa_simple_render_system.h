#pragma once
//
//
// #include "bred/gpu/properties.h"
// #include "bred/graphics3d/render_systems/object_render_system.h"
// #include "bred/graphics3d/render_systems/point_light_render_system.h"
//
//
// namespace SceneFoundry_SceneFoundry
// {
//
//
//
// 	struct global_ubo
//    {
//
// 	   floating_matrix4                           projection{ 1.f };
// 		floating_matrix4                           view{ 1.f };
// 		floating_matrix4                           inverseView{ 1.f };
// 		floating_sequence4                           ambientLightColor{ 1.f, 1.f, 1.f, .02f };
// 		::graphics3d::point_light_t         pointLights[::graphics3d::point_light_t::MAX_LIGHTS];
// 		int                                 numLights;
//
// 	};
//
//
// 	// inline ::gpu::properties simple_render_properties()
// 	// {
// 	//
// 	// 	static ::gpu::property s_properties[] =
// 	// 	{
// 	// 		{"modelMatrix", ::gpu::e_type_mat4},
// 	// 		{"normalMatrix", ::gpu::e_type_mat4},
// 	// 		{ nullptr, ::gpu::e_type_none}
// 	//
// 	// 	};
// 	//
// 	// 	return s_properties;
// 	//
// 	// }
// 	//
//
// 	class simple_render_system:
// 		virtual public ::graphics3d::object_render_system
// 	{
// 	public:
//
//
// 		simple_render_system();
// 		~simple_render_system();
//
//
// 		virtual void initialize_simple_render_system(::graphics3d::engine* pengine);
//
//
//
// 		void on_update(::gpu::context* pgpucontext, ::graphics3d::scene* pscene) override;
// 		void on_render(::gpu::context* pgpucontext, ::graphics3d::scene* pscene) override;
//
//
// 		//void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
// 		//void createPipeline(VkRenderPass renderPass);
//
// 		void on_prepare(::gpu::context* pgpucontext) override;
//
//
// 		//virtual void renderGameObjects(::graphics3d::scene* pscene);
// 	//private:
// 		//virtual void createPipelineLayout();
// 		//virtual void createPipeline();
//
// 	};
//
//
// } // namespace SceneFoundry_SceneFoundry
//
// // BEGIN_GPU_PROPERTIES(::SceneFoundry_SceneFoundry::point_light)
// // GPU_PROPERTY("position", ::gpu::e_type_seq4)
// // GPU_PROPERTY("color", ::gpu::e_type_seq4)
// // END_GPU_PROPERTIES()
// //
//
//
// BEGIN_GPU_PROPERTIES(::SceneFoundry_SceneFoundry::global_ubo)
// GPU_PROPERTY("projection", ::gpu::e_type_mat4)
// GPU_PROPERTY("view", ::gpu::e_type_mat4)
// GPU_PROPERTY("invView", ::gpu::e_type_mat4)
// GPU_PROPERTY("ambientLightColor", ::gpu::e_type_seq4)
// GPU_PROPERTY("pointLights", ::gpu_properties<::graphics3d::point_light>(), ::graphics3d::point_light_t::MAX_LIGHTS)
// GPU_PROPERTY("numLights", ::gpu::e_type_int)
// GPU_PROPERTY("padding1", ::gpu::e_type_float)
// GPU_PROPERTY("padding2", ::gpu::e_type_float)
// GPU_PROPERTY("padding3", ::gpu::e_type_float)
// END_GPU_PROPERTIES()
//
