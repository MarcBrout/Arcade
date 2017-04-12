//
// Created by brout_m on 03/04/17.
//

#include <new>
#include <cstdlib>
#include <unistd.h>
#include <algorithm>
#include "SpriteGenerator.hpp"
#include "Sound.hpp"
#include "LifelessEntity.hpp"
#include "CentipedeGame.hpp"
#include "Protocol.hpp"

static bool talinette = false;

arcade::CentipedeGame::CentipedeGame() :
        _gameState(GameState::NONE),
        _map("./games/centipede/assets/map1.cfg", LASTLAYER),
        _gui(),
        _bullet(Vector2s(0, 0)),
        _centipedeKiller(placePlayer(_map),
                         {KILLER},
                         {1},
                         TileType::EMPTY,
                         TileTypeEvolution::PLAYER,
                         Color::Red,
                         1)
{
    _map.updateLayer(_centipedeKiller, PLAYER);
    randomize(_map, 0.05);
    createCentipede();
    _centipedes.front()->oneTurn(_centipedes, _mushrooms, _map);
    for (Centipede *centipede : _centipedes)
        for (CentipedePart *part : centipede->getBody())
            _map.updateLayer(*part, CentipedeLayers::CENTIPEDE);
    this->updateMap();
    _tick = 0;
    _gameState = GameState::INGAME;
}

void arcade::CentipedeGame::randomize(arcade::Map &map, double density)
{
    Vector2s vec;

    for (size_t y = 0; y < map.getHeight() - 1; ++y)
    {
        for (size_t x = 0; x < map.getWidth(); ++x)
        {
            vec.x = x;
            vec.y = y;
            if (_centipedeKiller.getAbs() != vec && std::rand() % static_cast<int>(1.0 / density) == 0)
                _mushrooms.push_back(new Mushroom(vec));
        }
    }

    for (Entity *entity : _mushrooms)
        map.updateLayer(*entity, PLAYER);
}

arcade::GameState arcade::CentipedeGame::getGameState() const
{
    return _gameState;
}

void arcade::CentipedeGame::notifyEvent(std::vector<arcade::Event> &&_events)
{
    std::vector<arcade::Event> events = std::move(_events);

    for (arcade::Event &event : events)
    {
        if (event.kb_key == arcade::KeyboardKey::KB_ESCAPE)
        {
            _gameState = arcade::GameState::MENU;
            return ;
        }
    }

    _centipedeKiller.updatePlayerInput(events);
}

void arcade::CentipedeGame::notifyNetwork(std::vector<arcade::NetworkPacket> &&_events)
{
    std::vector<arcade::NetworkPacket> events = _events;
    (void)events;
}

std::vector<arcade::NetworkPacket> arcade::CentipedeGame::getNetworkToSend()
{
    std::vector<arcade::NetworkPacket> npt;
    return std::move(npt);
}

void arcade::CentipedeGame::createCentipede()
{
    Vector2s pos;

    pos.x = rand() % _map.getWidth();
    pos.y = 0;
    _centipedes.push_back(new Centipede(pos));
}

void arcade::CentipedeGame::process()
{
    _tick++;
    if (!(_tick % (SPEED / 5)) || talinette)
    {
        _centipedeKiller.move(_bullet, _map, _mushrooms);
        _centipedeKiller.touched(_centipedes);
        _centipedeKiller.action(_bullet);
        bulletAndMushrooms();
        _centipedeKiller.bulletVsCentipede(_bullet, _centipedes, _mushrooms);
        removeDeadCentipedes();
        this->updateMap();
    }

    if (!(_tick % (SPEED * 2)) || talinette)
    {
        if (!_centipedes.size())
            createCentipede();

        for (Centipede *centipede : _centipedes)
        {
            centipede->oneTurn(_centipedes, _mushrooms, _map);
        }

        this->updateMap();

        if (!_centipedeKiller.getHp())
            _gameState = MENU;
    }
}

std::vector<std::unique_ptr<arcade::ISprite>> arcade::CentipedeGame::getSpritesToLoad() const
{
    std::vector<std::unique_ptr<arcade::ISprite>> tmp;

    tmp.push_back(std::make_unique<SpriteGenerator>("V", "./games/snake/assets/img/", "killer", 1, ".png"));
    tmp.push_back(std::make_unique<SpriteGenerator>("T", "./games/snake/assets/img/", "mushroom", 1, ".png"));
    tmp.push_back(std::make_unique<SpriteGenerator>("O", "./games/snake/assets/img/", "head", 1, ".png"));
    tmp.push_back(std::make_unique<SpriteGenerator>("o", "./games/snake/assets/img/", "body", 1, ".png"));
    tmp.push_back(std::make_unique<SpriteGenerator>("'", "./games/snake/assets/img/", "fire", 1, ".png"));
    return std::move(tmp);
}

std::vector<std::pair<std::string, arcade::SoundType>> arcade::CentipedeGame::getSoundsToLoad() const
{
    std::vector<std::pair<std::string, arcade::SoundType>> sounds;

    return sounds;
}

std::vector<arcade::Sound> arcade::CentipedeGame::getSoundsToPlay()
{
    std::vector<Sound> sounds;

    return std::move(sounds);
}

const arcade::IMap &arcade::CentipedeGame::getCurrentMap() const
{
    return _map;
}

arcade::IGUI &arcade::CentipedeGame::getGUI()
{
    return _gui;
}

arcade::Vector2s arcade::CentipedeGame::placePlayer(arcade::Map &map)
{
    Vector2s pos;
    size_t height = map.getHeight();
    size_t width = map.getWidth();

    pos.x = std::rand() % width;
    pos.y = height - 1;
    return pos;
}

void arcade::CentipedeGame::updateMap()
{
    _map.resetMapFromLayer(PLAYER);
    _map.updateLayer(_centipedeKiller, CentipedeLayers::PLAYER);

    for (Mushroom *mushroom : _mushrooms)
        _map.updateLayer(*mushroom, CentipedeLayers::PLAYER);

    if (_bullet.isAlive())
        _map.updateLayer(_bullet, CentipedeLayers::BULLET);

    for (Centipede *centipede : _centipedes)
        for (CentipedePart *part : centipede->getBody())
            _map.updateLayer(*part, CentipedeLayers::CENTIPEDE);
}

const arcade::Map &arcade::CentipedeGame::getMouliMap() const {
    return _map;
}

arcade::CentipedeGame::~CentipedeGame()
{
}

const arcade::Vector2s &arcade::CentipedeGame::getPlayerpos() const
{
    return _centipedeKiller.getAbs();
}

void arcade::CentipedeGame::bulletAndMushrooms()
{
    std::list<arcade::Mushroom *>::iterator it;

    if ((it = std::find_if(_mushrooms.begin(), _mushrooms.end(),
                           [this](Mushroom *mush) {
                               return (mush->getAbs() == _bullet.getAbs());
                           })) != _mushrooms.end())
    {
        _bullet.reset(_centipedeKiller.getAbs());
        _bullet.hit();
        (*it)->takeDamage(1);
        if ((*it)->getHp() == 0)
            _mushrooms.erase(it);
    }
}

void arcade::CentipedeGame::removeDeadCentipedes()
{
    std::list<arcade::Centipede *>::iterator it;

    it = std::remove_if(_centipedes.begin(), _centipedes.end(),
                        [](arcade::Centipede *cent) {
                            return (cent->getBody().size() == 0);
                        });
    if (it != _centipedes.end())
        _centipedes.erase(it, _centipedes.end());
}

bool arcade::CentipedeGame::hasNetwork() const
{
    return false;
}

extern "C" arcade::IGame *getGame()
{
    return (new arcade::CentipedeGame());
}

void printMap(arcade::GetMap *m)
{
    for (int y = 0; y < m->height; ++y)
    {
        for (int x = 0; x < m->width; ++x) {
            std::cerr << (int)m->tile[y * m->width + x];
        }
        std::cerr << std::endl;
    }
}

static void updateMap(arcade::GetMap *map, const arcade::Map& imap)
{
    for (size_t y = 0; y < imap.getHeight(); ++y)
    {
        for (size_t x = 0; x < imap.getWidth(); ++x)
        {
            for (size_t layer = 0; layer < imap.getLayerNb(); ++layer)
            {
                map->tile[y * imap.getWidth() + x] = imap.atMouli(layer, x, y).getType();
                if (imap.atMouli(layer, x, y).getType() != arcade::TileType::EMPTY)
                    break;
            }
        }
    }
}

extern "C" void Play()
{
    arcade::CentipedeGame centipedeGame;
    arcade::CommandType command;
    arcade::Map const& map = centipedeGame.getMouliMap();
    std::vector<arcade::Event> events;
    size_t whereAmISize = sizeof(arcade::WhereAmI) + sizeof(arcade::Position);
    size_t mapSize = sizeof(arcade::GetMap) + (map.getWidth() * map.getHeight() * sizeof(arcade::TileType));
    arcade::WhereAmI *whereAmI = reinterpret_cast<arcade::WhereAmI *>(new char[whereAmISize] {0});
    arcade::GetMap *getMap = reinterpret_cast<arcade::GetMap*>(new char[mapSize] {0});
    talinette = true;

    whereAmI->lenght = 1;
    whereAmI->type = arcade::CommandType::WHERE_AM_I;
    getMap->height = static_cast<uint16_t>(map.getHeight());
    getMap->width = static_cast<uint16_t>(map.getWidth());
    getMap->type = arcade::CommandType::GET_MAP;
    updateMap(getMap, map);
    while (std::cin.read(reinterpret_cast<char*>(&command), sizeof(arcade::CommandType)))
    {
        arcade::Event event;
        event.type = arcade::EventType::ET_KEYBOARD;
        event.action = arcade::ActionType::AT_PRESSED;
        switch (command)
        {
            case (arcade::CommandType::WHERE_AM_I) :
                whereAmI->position[0].x = static_cast<uint16_t >(centipedeGame.getPlayerpos().x);
                whereAmI->position[0].y = static_cast<uint16_t >(centipedeGame.getPlayerpos().y);
                std::cout.write(reinterpret_cast<const char *>(whereAmI), whereAmISize);
                std::cout.flush();
                break;
            case (arcade::CommandType::GET_MAP) :
                updateMap(getMap, map);
                std::cout.write(reinterpret_cast<const char *>(getMap), mapSize);
                std::cout.flush();
                break;
            case (arcade::CommandType::GO_UP) :
                event.kb_key = arcade::KB_ARROW_UP;
                events.push_back(event);
                centipedeGame.notifyEvent(std::move(events));
                break;
            case (arcade::CommandType::GO_DOWN) :
                event.kb_key = arcade::KB_ARROW_DOWN;
                events.push_back(event);
                centipedeGame.notifyEvent(std::move(events));
                break;
            case (arcade::CommandType::GO_LEFT) :
                event.kb_key = arcade::KB_ARROW_LEFT;
                events.push_back(event);
                centipedeGame.notifyEvent(std::move(events));
                break;
            case (arcade::CommandType::GO_RIGHT) :
                event.kb_key = arcade::KB_ARROW_RIGHT;
                events.push_back(event);
                centipedeGame.notifyEvent(std::move(events));
                break;
            case (arcade::CommandType::GO_FORWARD) :
                events.push_back(event);
                centipedeGame.notifyEvent(std::move(events));
                break;
            case (arcade::CommandType::SHOOT) :
                event.kb_key = arcade::KB_SPACE;
                events.push_back(event);
                centipedeGame.notifyEvent(std::move(events));
                break;
            case (arcade::CommandType::ILLEGAL) :
                break;
            case (arcade::CommandType::PLAY) :
                centipedeGame.process();
                break;
        }
    }
    delete [] whereAmI;
    delete [] getMap;
}