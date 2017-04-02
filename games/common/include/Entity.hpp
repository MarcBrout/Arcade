//
// Created by duhieu_b on 01/04/17.
//

#ifndef CPP_ARCADE_ENTITY_HPP
#define CPP_ARCADE_ENTITY_HPP

#include <Protocol.hpp>
#include <GameState.hpp>
#include <Color.hpp>
#include "Vector2.hpp"
#include "Sprite.hpp"

namespace arcade
{
    class Entity
    {
    private:
        Vector2i abs;
        Vector2d shift;
        Vector2i prev;
        Sprite sprite;
        TileType type;
        TileTypeEvolution typeEvolution;
        Color color;
        Entity() = delete;

    public:
        // Constructors / Destructor
        virtual ~Entity() {}
        Entity(Entity const& cpy);
        Entity(Vector2i const& pos, size_t id, size_t spriteCount, TileType Type, TileTypeEvolution TypeEvolution, Color col);

        // Getters
        const Vector2i &getAbs() const;
        const Vector2d &getShift() const;
        const Vector2i &getPrev() const;
        size_t getSpriteId() const;
        TileType getType() const;
        TileTypeEvolution getTypeEvolution() const;
        const Color &getColor() const;
        size_t getSpriteCount() const;

        // Setters
        void setAbs(const Vector2i &abs);
        void setShift(const Vector2d &shift);
        void setPrev(const Vector2i &prev);
        void setType(TileType type);
        void setTypeEvolution(TileTypeEvolution typeEvolution);
        void setColor(const Color &color);

        // Special
        size_t advance();
    };
}


#endif //CPP_ARCADE_ENTITY_HPP