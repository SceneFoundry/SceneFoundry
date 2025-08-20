#pragma once
//////#include <stdexcept>
#define GLFW_INCLUDE_VULKAN
////#include <GLFW/glfw3.h>
//#include <stdexcept>
#include <vulkan/vulkan.h>
//#include <vector>
#include "instance.h"
#include "SceneFoundry/sandbox_interfaces/window_input_i.h"


class SandboxWindow {

public:
	SandboxWindow(int w, int h, ::string name);
	~SandboxWindow();

	//bool shouldClose() { return glfwWindowShouldClose(m_pwindow); }
	VkExtent2D getExtent() { return { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) }; }
	bool wasWindowResized() { return m_bFramebufferResized; }
	void resetWindowResizedFlag() { m_bFramebufferResized = false; }
	//GLFWwindow* getGLFWwindow() const { return m_pwindow; }

	void createSurface(VkInstance instance, VkSurfaceKHR* surface) const;
private:
	void initWindow();
	//static void framebufferResizeCallback(GLFWwindow*, int, int);

	int          m_width, m_height;
	bool         m_bFramebufferResized = false;
	::string  m_window_name;
	//GLFWwindow*  m_pwindow = nullptr;

};

struct WindowUserData {
	SandboxWindow* window = nullptr;
	IWindowInput* input = nullptr;
};