//
// Created by Pierre Bougon on 17/03/17.
//

#include <SFML/Window/Event.hpp>
#include "IGfxLib.hpp"
#include "SFMLCore.hpp"

arcade::SFMLCore::SFMLCore() : width(800), height(600), x(0), y(0)
{
    sf::VideoMode videoMode(width, height);

    window.create(videoMode, "Retro Furnace");
}

arcade::SFMLCore::~SFMLCore()
{}

bool arcade::SFMLCore::pollEvent(arcade::Event &e)
{
    sf::Event event;

    if (window.pollEvent(event))
    {
        updateEventType(e, event);
        e.kb_key = mappedKeyboard[event.key.code];
        updateMousePosition(e.pos_rel, sf::Mouse::getPosition());
        updateMousePosition(e.pos_abs, sf::Mouse::getPosition(window));
        return true;
    }
    return false;
}

bool arcade::SFMLCore::doesSupportSound() const
{
    return true;
}

void arcade::SFMLCore::soundControl(Sound const &sound)
{
    soundManager.soundControl(sound);
}

void arcade::SFMLCore::updateMap(const arcade::IMap &map)
{
    ITile const *tile;

    // Up to aff part of the map we must catch when the map is
    if (map.getWidth() * getTileSize(map).x > width
            || map.getHeight() * getTileSize(map).y > height)
        throw std::range_error("Map is too ");


    for (size_t l = 0; l < map.getLayerNb(); ++l)
    {
        for (size_t x = 0; x < map.getWidth(); ++x)
        {
            for (size_t y = 0; y < map.getHeight(); ++y)
            {
                tile = &map.at(l, x, y);
                if (tile->hasSprite())
                {
                    drawSprite(tile, map, x, y);
                }
                else
                {
                    drawColor(tile, map, x, y);
                }
            }
        }
    }
}

void arcade::SFMLCore::updateGUI(arcade::IGUI &gui)
{
    IComponent *component;

    for (size_t i = 0; i < gui.size(); ++i)
    {
        component = &gui.at(i);
        if (component->hasSprite())
        {
            drawSprite(component);
        }
        else
        {
            drawColor(component);
        }
    }
}

void arcade::SFMLCore::display()
{
    window.display();
}

void arcade::SFMLCore::clear()
{
    window.clear();
}

void arcade::SFMLCore::updateEventType(arcade::Event &e, sf::Event &event)
{
    e.type = mappedEventType[event.type];
    e.action = mappedEventAction[event.type];
}

void arcade::SFMLCore::updateMousePosition(arcade::MousePos &pos, sf::Vector2i sfPos)
{
    pos.x = sfPos.x;
    pos.y = sfPos.y;
}

void arcade::SFMLCore::loadSprites(std::vector<std::unique_ptr<arcade::ISprite>> &&sprites)
{
    for (std::vector<std::unique_ptr<arcade::ISprite>>::iterator it = sprites.begin(); it < sprites.end(); ++it)
    {
        for (size_t i = 0; i < (*it)->spritesCount(); ++i)
        {
            this->sprites.push_back((*it)->getGraphicPath(i));
        }
    }
}

void arcade::SFMLCore::drawColor(ITile const *tile, IMap const &map, size_t x, size_t y)
{
    //TODO calculate size of a tile in a relative way
    this->colorSprite.setSize(getTileSize(map));
    this->colorSprite.setFillColor(sf::Color(tile->getColor().r,
                                             tile->getColor().g,
                                             tile->getColor().b,
                                             tile->getColor().a));
    colorSprite.setPosition(getTilePosX(x, map) + static_cast<float>(tile->getShiftX()),
                            getTilePosY(y, map) + static_cast<float>(tile->getShiftY()));
    window.draw(colorSprite);
}

void arcade::SFMLCore::drawColor(IComponent *component)
{
    sf::Vector2f size(static_cast<float>(component->getWidth()),
                      static_cast<float>(component->getHeight()));

    //TODO calculate size of a tile in a relative way
    this->colorSprite.setSize(size);
    this->colorSprite.setFillColor(sf::Color(component->getBackgroundColor().r,
                                             component->getBackgroundColor().g,
                                             component->getBackgroundColor().b,
                                             component->getBackgroundColor().a));
    colorSprite.setPosition(static_cast<float>(component->getX()), static_cast<float>(component->getY()));
    window.draw(colorSprite);
}

void arcade::SFMLCore::drawSprite(ITile const *tile, IMap const &map, size_t x, size_t y)
{
    sf::Vector2f pos(getTilePosX(x, map) + static_cast<float>(tile->getShiftX()),
                     getTilePosY(y, map) + static_cast<float>(tile->getShiftY()));
    GfxSprite &sprite = sprites[tile->getSpriteId()];

    if (sprite.corrupted)
        return;
    sprite.sprite.setScale(1 / (sprite.texture.getSize().x / getTileSize(map).x),
                           1 / (sprite.texture.getSize().y / getTileSize(map).y));
    sprite.sprite.setPosition(pos);
    window.draw(sprite.sprite);
}

void arcade::SFMLCore::drawSprite(IComponent *component)
{
    GfxSprite &sprite = sprites[component->getBackgroundId()];

    if (sprite.corrupted)
        return;
    sprite.sprite.setScale(static_cast<float>(component->getWidth() * width),
                           static_cast<float>(component->getHeight() * height));
    sprite.sprite.setPosition(static_cast<float>(component->getX()), static_cast<float>(component->getY()));
    window.draw(sprite.sprite);
}

sf::Vector2f arcade::SFMLCore::getTileSize(IMap const &map)
{
    sf::Vector2f pos;

    pos.x = width / map.getWidth();
    pos.y = height / map.getHeight();
    return pos;
}

float arcade::SFMLCore::getTilePosX(size_t x, IMap const &map)
{
    return getTileSize(map).x * x;
}

float arcade::SFMLCore::getTilePosY(size_t y, IMap const &map)
{
    return getTileSize(map).y * y;
}

void arcade::SFMLCore::loadSounds(std::vector<std::pair<std::string, arcade::SoundType>> const &sounds)
{
    soundManager.loadSounds(sounds);
}

namespace arcade
{
    std::map<sf::Event::EventType, arcade::EventType> SFMLCore::mappedEventType =
            {
                    {sf::Event::EventType::KeyPressed, arcade::EventType::ET_KEYBOARD},
                    {sf::Event::EventType::KeyReleased, arcade::EventType::ET_KEYBOARD},
                    {sf::Event::EventType::MouseButtonPressed, arcade::EventType::ET_MOUSE},
                    {sf::Event::EventType::MouseButtonReleased, arcade::EventType::ET_MOUSE},
                    {sf::Event::EventType::MouseEntered, arcade::EventType::ET_MOUSE},
                    {sf::Event::EventType::MouseLeft, arcade::EventType::ET_MOUSE},
                    {sf::Event::EventType::MouseMoved, arcade::EventType::ET_MOUSE},
                    {sf::Event::EventType::MouseWheelMoved, arcade::EventType::ET_MOUSE},
                    {sf::Event::EventType::JoystickButtonPressed, arcade::EventType::ET_JOYSTICK},
                    {sf::Event::EventType::JoystickButtonReleased, arcade::EventType::ET_JOYSTICK},
                    {sf::Event::EventType::JoystickConnected, arcade::EventType::ET_JOYSTICK},
                    {sf::Event::EventType::JoystickDisconnected, arcade::EventType::ET_JOYSTICK},
                    {sf::Event::EventType::JoystickMoved, arcade::EventType::ET_JOYSTICK},
                    {sf::Event::EventType::TextEntered, arcade::EventType::ET_NONE},
                    {sf::Event::EventType::Closed, arcade::EventType::ET_QUIT},
                    {sf::Event::EventType::GainedFocus, arcade::EventType::ET_NONE},
                    {sf::Event::EventType::LostFocus, arcade::EventType::ET_NONE},
                    {sf::Event::EventType::Resized, arcade::EventType::ET_NONE},
                    {sf::Event::EventType::Count, arcade::EventType::NB_EVENT_TYPE},
            };

    std::map<sf::Event::EventType, arcade::ActionType> SFMLCore::mappedEventAction =
            {
                    {sf::Event::EventType::KeyPressed, arcade::ActionType::AT_PRESSED},
                    {sf::Event::EventType::KeyReleased, arcade::ActionType::AT_RELEASED},
                    {sf::Event::EventType::MouseButtonPressed, arcade::ActionType::AT_PRESSED},
                    {sf::Event::EventType::MouseButtonReleased, arcade::ActionType::AT_RELEASED},
                    {sf::Event::EventType::MouseEntered, arcade::ActionType::AT_NONE},
                    {sf::Event::EventType::MouseLeft, arcade::ActionType::AT_NONE},
                    {sf::Event::EventType::MouseMoved, arcade::ActionType::AT_MOVED},
                    {sf::Event::EventType::MouseWheelMoved, arcade::ActionType::AT_MOVED},
                    {sf::Event::EventType::JoystickButtonPressed, arcade::ActionType::AT_PRESSED},
                    {sf::Event::EventType::JoystickButtonReleased, arcade::ActionType::AT_RELEASED},
                    {sf::Event::EventType::JoystickConnected, arcade::ActionType::AT_NONE},
                    {sf::Event::EventType::JoystickDisconnected, arcade::ActionType::AT_NONE},
                    {sf::Event::EventType::JoystickMoved, arcade::ActionType::AT_MOVED},
                    {sf::Event::EventType::TextEntered, arcade::ActionType::AT_NONE},
                    {sf::Event::EventType::Closed, arcade::ActionType::AT_NONE},
                    {sf::Event::EventType::GainedFocus, arcade::ActionType::AT_NONE},
                    {sf::Event::EventType::LostFocus, arcade::ActionType::AT_NONE},
                    {sf::Event::EventType::Resized, arcade::ActionType::AT_NONE},
                    {sf::Event::EventType::Count, arcade::ActionType::NB_ACTION_TYPE},
            };

    std::map<sf::Keyboard::Key, arcade::KeyboardKey> SFMLCore::mappedKeyboard =
            {
                    {sf::Keyboard::Num0, arcade::KeyboardKey::KB_0},
                    {sf::Keyboard::Numpad0, arcade::KeyboardKey::KB_0},
                    {sf::Keyboard::Num1, arcade::KeyboardKey::KB_1},
                    {sf::Keyboard::Numpad1, arcade::KeyboardKey::KB_1},
                    {sf::Keyboard::Num2, arcade::KeyboardKey::KB_2},
                    {sf::Keyboard::Numpad2, arcade::KeyboardKey::KB_2},
                    {sf::Keyboard::Num3, arcade::KeyboardKey::KB_3},
                    {sf::Keyboard::Numpad3, arcade::KeyboardKey::KB_3},
                    {sf::Keyboard::Num4, arcade::KeyboardKey::KB_4},
                    {sf::Keyboard::Numpad4, arcade::KeyboardKey::KB_4},
                    {sf::Keyboard::Num5, arcade::KeyboardKey::KB_5},
                    {sf::Keyboard::Numpad5, arcade::KeyboardKey::KB_5},
                    {sf::Keyboard::Num6, arcade::KeyboardKey::KB_6},
                    {sf::Keyboard::Numpad6, arcade::KeyboardKey::KB_6},
                    {sf::Keyboard::Num7, arcade::KeyboardKey::KB_7},
                    {sf::Keyboard::Numpad7, arcade::KeyboardKey::KB_7},
                    {sf::Keyboard::Num8, arcade::KeyboardKey::KB_8},
                    {sf::Keyboard::Numpad8, arcade::KeyboardKey::KB_8},
                    {sf::Keyboard::Num9, arcade::KeyboardKey::KB_9},
                    {sf::Keyboard::Numpad9, arcade::KeyboardKey::KB_9},
                    {sf::Keyboard::A, arcade::KeyboardKey::KB_A},
                    {sf::Keyboard::B, arcade::KeyboardKey::KB_B},
                    {sf::Keyboard::C, arcade::KeyboardKey::KB_C},
                    {sf::Keyboard::D, arcade::KeyboardKey::KB_D},
                    {sf::Keyboard::E, arcade::KeyboardKey::KB_E},
                    {sf::Keyboard::F, arcade::KeyboardKey::KB_F},
                    {sf::Keyboard::G, arcade::KeyboardKey::KB_G},
                    {sf::Keyboard::H, arcade::KeyboardKey::KB_H},
                    {sf::Keyboard::I, arcade::KeyboardKey::KB_I},
                    {sf::Keyboard::J, arcade::KeyboardKey::KB_J},
                    {sf::Keyboard::K, arcade::KeyboardKey::KB_K},
                    {sf::Keyboard::L, arcade::KeyboardKey::KB_L},
                    {sf::Keyboard::M, arcade::KeyboardKey::KB_M},
                    {sf::Keyboard::N, arcade::KeyboardKey::KB_N},
                    {sf::Keyboard::O, arcade::KeyboardKey::KB_O},
                    {sf::Keyboard::P, arcade::KeyboardKey::KB_P},
                    {sf::Keyboard::Q, arcade::KeyboardKey::KB_Q},
                    {sf::Keyboard::R, arcade::KeyboardKey::KB_R},
                    {sf::Keyboard::S, arcade::KeyboardKey::KB_S},
                    {sf::Keyboard::T, arcade::KeyboardKey::KB_T},
                    {sf::Keyboard::U, arcade::KeyboardKey::KB_U},
                    {sf::Keyboard::V, arcade::KeyboardKey::KB_V},
                    {sf::Keyboard::W, arcade::KeyboardKey::KB_W},
                    {sf::Keyboard::X, arcade::KeyboardKey::KB_X},
                    {sf::Keyboard::Y, arcade::KeyboardKey::KB_Y},
                    {sf::Keyboard::Z, arcade::KeyboardKey::KB_Z},
                    {sf::Keyboard::F1, arcade::KeyboardKey::KB_FN1},
                    {sf::Keyboard::F2, arcade::KeyboardKey::KB_FN2},
                    {sf::Keyboard::F3, arcade::KeyboardKey::KB_FN3},
                    {sf::Keyboard::F4, arcade::KeyboardKey::KB_FN4},
                    {sf::Keyboard::F5, arcade::KeyboardKey::KB_FN5},
                    {sf::Keyboard::F6, arcade::KeyboardKey::KB_FN6},
                    {sf::Keyboard::F7, arcade::KeyboardKey::KB_FN7},
                    {sf::Keyboard::F8, arcade::KeyboardKey::KB_FN8},
                    {sf::Keyboard::F9, arcade::KeyboardKey::KB_FN9},
                    {sf::Keyboard::F10, arcade::KeyboardKey::KB_FN10},
                    {sf::Keyboard::F11, arcade::KeyboardKey::KB_FN11},
                    {sf::Keyboard::F12, arcade::KeyboardKey::KB_FN12},
                    {sf::Keyboard::F13, arcade::KeyboardKey::KB_NONE},
                    {sf::Keyboard::F14, arcade::KeyboardKey::KB_NONE},
                    {sf::Keyboard::F15, arcade::KeyboardKey::KB_NONE},
                    {sf::Keyboard::Down, arcade::KeyboardKey::KB_ARROW_DOWN},
                    {sf::Keyboard::Up, arcade::KeyboardKey ::KB_ARROW_UP},
                    {sf::Keyboard::Right, arcade::KeyboardKey::KB_ARROW_RIGHT},
                    {sf::Keyboard::Left, arcade::KeyboardKey::KB_ARROW_LEFT},
                    {sf::Keyboard::BackSlash, arcade::KeyboardKey::KB_BACKSLASH},
                    {sf::Keyboard::Multiply, arcade::KeyboardKey::KB_ASTERISK},
                    {sf::Keyboard::BackSpace, arcade::KeyboardKey::KB_BACKSPACE},
                    {sf::Keyboard::Comma, arcade::KeyboardKey::KB_COMMA},
                    {sf::Keyboard::Delete, arcade::KeyboardKey::KB_DELETE},
                    {sf::Keyboard::Period, arcade::KeyboardKey::KB_DOT},
                    {sf::Keyboard::End, arcade::KeyboardKey::KB_END},
                    {sf::Keyboard::Return, arcade::KeyboardKey::KB_ENTER},
                    {sf::Keyboard::Equal, arcade::KeyboardKey::KB_EQUALS},
                    {sf::Keyboard::Escape, arcade::KeyboardKey::KB_ESCAPE},
                    {sf::Keyboard::Home, arcade::KeyboardKey::KB_HOME},
                    {sf::Keyboard::LAlt, arcade::KeyboardKey::KB_LALT},
                    {sf::Keyboard::LControl, arcade::KeyboardKey::KB_LCTRL},
                    {sf::Keyboard::LBracket, arcade::KeyboardKey::KB_LEFTBRACKET},
                    {sf::Keyboard::LShift, arcade::KeyboardKey::KB_LSHIFT},
                    {sf::Keyboard::Dash, arcade::KeyboardKey::KB_MINUS},
                    {sf::Keyboard::PageDown, arcade::KeyboardKey::KB_PAGEDOWN},
                    {sf::Keyboard::PageUp, arcade::KeyboardKey::KB_PAGEUP},
                    {sf::Keyboard::Add, arcade::KeyboardKey::KB_PLUS},
                    {sf::Keyboard::RAlt, arcade::KeyboardKey::KB_RALT},
                    {sf::Keyboard::RControl, arcade::KeyboardKey::KB_RCTRL},
                    {sf::Keyboard::RBracket, arcade::KeyboardKey::KB_RIGHTBRACKET},
                    {sf::Keyboard::RShift, arcade::KeyboardKey::KB_RSHIFT},
                    {sf::Keyboard::SemiColon, arcade::KeyboardKey::KB_SEMICOLON},
                    {sf::Keyboard::Space, arcade::KeyboardKey::KB_SPACE},
                    {sf::Keyboard::Quote, arcade::KeyboardKey::KB_SIMPLEQUOTE},
                    {sf::Keyboard::Slash, arcade::KeyboardKey ::KB_SLASH},
                    {sf::Keyboard::Tab, arcade::KeyboardKey::KB_TAB},
                    {sf::Keyboard::Unknown, arcade::KeyboardKey::KB_NONE},
                    {sf::Keyboard::LSystem, arcade::KeyboardKey::KB_NONE},
                    {sf::Keyboard::RSystem, arcade::KeyboardKey::KB_NONE},
                    {sf::Keyboard::Pause, arcade::KeyboardKey::KB_NONE},
            };
}

extern "C" arcade::IGfxLib *getLib()
{
    return (new arcade::SFMLCore());
}