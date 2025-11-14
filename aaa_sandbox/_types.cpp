//
// Created by camilo on 2025-08-20 01:20 <3ThomasBorregaardSorensen!!
//
#include "framework.h"


namespace graphics3d
{


    floating_matrix4 ::graphics3d::transform::mat4() const
    {
        // 1) Translation
        floating_matrix4 T = glm::translate(floating_matrix4(1.0f), translation);
        // 2) Rotation
        floating_matrix4 R = glm::rotate(floating_matrix4(1.0f), rotation.x, floating_sequence3(1, 0, 0));
        R = glm::rotate(R, rotation.y, floating_sequence3(0, 1, 0));
        R = glm::rotate(R, rotation.z, floating_sequence3(0, 0, 1));
        // 3) Scale
        floating_matrix4 S = glm::scale(floating_matrix4(1.0f), scale);

        // Compose: T * R * S
        return T * R * S;
    }
    floating_matrix3 ::graphics3d::transform::normalMatrix() const
    {
        // Build RS (ignore translation)
        floating_matrix4 R = glm::rotate(floating_matrix4(1.0f), rotation.y, floating_sequence3(0, 1, 0));
        R = glm::rotate(R, rotation.x, floating_sequence3(1, 0, 0));
        R = glm::rotate(R, rotation.z, floating_sequence3(0, 0, 1));

        floating_matrix4 RS = R * glm::scale(floating_matrix4(1.0f), scale);

        // Normal matrix = inverse-transpose of the 3x3 upper-left
        return glm::transpose(glm::inverse(floating_matrix3(RS)));
    }


} // namespace graphics3d



