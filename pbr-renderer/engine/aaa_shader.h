#pragma once

//#include <string>
//#include <vector>
//
//#include <glm/glm.hpp>
//
///**
// * A shader program created from vertex + fragment shader.
// */
//class Shader {
//public: 
//	Shader(const std::string &vertexPath, const std::string &fragmentPath);
//
//	// activate the shader
//	void use();
//
//	// uniform functions
//	void setBool(const std::string& name, bool value) const;
//	void setInt(const std::string& name, int value) const;
//	void setFloat(const std::string& name, float value) const;
//	void setVec2(const std::string& name, floating_sequence2& value) const;
//	void setVec3(const std::string& name, floating_sequence3& value) const;
//	void setVec3Array(const std::string& name, std::vector<floating_sequence3> values) const;
//	void setMat4(const std::string& name, floating_matrix4 &value) const;
//
//	void setModelViewProjectionMatrices(floating_matrix4& model, floating_matrix4& view, floating_matrix4& projection);
//
//private:
//    // program ID
//    unsigned int mId;
//};