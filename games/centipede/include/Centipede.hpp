//
// Created by brout_m on 04/04/17.
//

#ifndef CPP_ARCADE_CENTIPEDE_HPP
# define CPP_ARCADE_CENTIPEDE_HPP
# define DEFAULT_SIZE 10

# include <deque>
# include "ALivingEntity.hpp"
# include "Bullet.hpp"
# include "Mushroom.hpp"

namespace arcade
{
    class CentipedePart : public ALivingEntity
    {
    public:
        CentipedePart(Vector2s const& pos);
        void setNb(size_t nb);
        void move();
        void action();
        void updatePlayerInput(std::vector<Event> &events);
        size_t getNb() const;
        CentipedePart& operator=(const CentipedePart& centipedePart);

    private:
        size_t _nb;
    };

    class Centipede
    {
    public:
        enum CentipedeDirection
        {
            EAST,
            WEST,
            SOUTH,
            NORTH
        };

    public:
        ~Centipede();
        Centipede(Vector2s const& pos,
                  size_t length = DEFAULT_SIZE,
                  CentipedeDirection dirH = WEST,
                  CentipedeDirection dirV = SOUTH);
        Centipede(Centipede const &centipede);
        Centipede &operator=(Centipede const& centipede);
        std::deque<arcade::CentipedePart*> &getBody();
        void setBody();
        void setBody(std::deque<arcade::CentipedePart*> &newBody);
        void oneTurn(std::list<Centipede*> &centipedes,
                     std::list<Mushroom*> &mushrooms,
                     Map const& map);
        bool hitByBullet(Bullet &bullet, std::list<Centipede*> &centipedes,
                         std::list<Mushroom*> &mushrooms);

    private:
        size_t                      _poped;
        Vector2s                    _start;
        CentipedeDirection          _directionH;
        CentipedeDirection          _directionV;
        std::deque<arcade::CentipedePart*>  _body;

        // private methods
        bool checkCentipedeCollision(std::list<Centipede*> &centipedes, Vector2s const& pos);
        void testMove(Map const& map, Vector2s const& pos);
        void moveBody();
        void moveHead(std::list<Centipede*> &centipedes,
                      std::list<Mushroom*> &mushrooms,
                      Map const& map);
        void split(size_t pos, std::list<Mushroom*> &mushroom, std::list<Centipede*> &centipedes);
        // deleted methods
        Centipede() = delete;
    };
}

#endif //CPP_ARCADE_CENTIPEDE_HPP
