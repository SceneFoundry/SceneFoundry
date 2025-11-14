#include "framework.h"
#include "entity.h"

Entity::Entity(::pointer<Model> model) : mModel(model)
{

}

void Entity::setPosition(floating_sequence3 position)
{
    mPosition = position;
}

floating_sequence3 Entity::getPosition()
{
    return mPosition;
}

void Entity::setScale(floating_sequence3 scale)
{
    mScale = scale;
}

floating_sequence3 Entity::getScale()
{
    return mScale;
}

void Entity::setOrientation(floating_quaternion orientation)
{
    mOrientation = orientation;
}

floating_quaternion Entity::getOrientation()
{
    return mOrientation;
}

::pointer<Model> Entity::getModel()
{
    return mModel;
}
