//
// Created by duhieu_b on 02/04/17.
//

#include "../include/LifelessEntity.hpp"

arcade::LifelessEntity::LifelessEntity(Entity const& cpy, bool collide) : Entity(cpy), collidable(collide)
{
}

arcade::LifelessEntity::LifelessEntity(arcade::Vector2i pos, size_t count, size_t spriteCount, TileType Type, TileTypeEvolution TypeEvolution, Color col, bool collide) :
    Entity(pos, count, spriteCount, Type, TypeEvolution, col), collidable(collide)
{

}

bool arcade::LifelessEntity::isCollidable() const
{
    return collidable;
}