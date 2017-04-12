//
// Created by duhieu_b on 15/03/17.
//

#include <bits/unique_ptr.h>
#include <curses.h>
#include <iostream>
#include <Color.hpp>
#include "Tile.hpp"
#include "Ndk.hpp"

bool arcade::Ndk::pollEvent(arcade::Event &e)
{
    int c = getch();
    if (c == ERR)
        return false;
    if (keyboard.find(static_cast<char>(c)) != keyboard.end())
    {
        e.type = arcade::ET_KEYBOARD;
        e.action = arcade::AT_PRESSED;
        e.kb_key = keyboard[static_cast<char>(c)];
        return (true);
    }
    e.type = arcade::ET_NONE;
    e.action = arcade::AT_NONE;
    e.kb_key = arcade::KB_NONE;
    return false;
}

void arcade::Ndk::setColor(size_t pos, size_t j, size_t k, ITile const& tile)
{
    std::string tmp;

    wattron(win, COLOR_PAIR(pos) | A_BOLD);
    tmp = vecString[tile.getSpriteId()][tile.getSpritePos()];
    mvwprintw(win, static_cast<int>(j) + 1, static_cast<int>(k) + 1, tmp.c_str());
    wattroff(win, COLOR_PAIR(pos) | A_BOLD);
}

void arcade::Ndk::updateMap(arcade::IMap const &map)
{
    size_t layer = map.getLayerNb();
    size_t i;
    size_t j;
    size_t k;

    if (!pass)
    {
        win = newwin(static_cast<int>(map.getHeight() + 2), static_cast<int>(map.getWidth() + 2), static_cast<int>(0.1 * getmaxx(stdscr)),static_cast<int>(0.1 * getmaxy(stdscr)));
        set_escdelay(0);
        cbreak();
        noecho();
        curs_set(0);
        wtimeout(win, 0);
        pass = true;
    }
    wclear(win);
    werase(win);
    wborder(win, '|', '|', '-', '-', '-', '-', '-', '-');
    start_color();
    init_pair(0, COLOR_BLACK, COLOR_BLACK);
    init_pair(5, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_CYAN);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);
    init_pair(6, COLOR_BLACK, COLOR_RED);
    for (i = 0; i < layer; ++i)
    {
        for (j = 0; j < map.getHeight(); ++j)
        {
            for (k = 0; k < map.getWidth(); ++k)
            {
                ITile const& tile = map.at(i, k, j);
                if (tile.hasSprite())
                {
                    if (tile.getColor() == Color::Black)
                        setColor(0, j, k, tile);
                    else if (tile.getColor() == Color::Green)
                        setColor(5, j, k, tile);
                    else if (tile.getColor() == Color::Cyan)
                        setColor(2, j, k, tile);
                    else if (tile.getColor() == Color::White)
                        setColor(3, j, k, tile);
                    else if (tile.getColor() == Color::Yellow)
                        setColor(4, j, k, tile);
                    else if (tile.getColor() == Color::Red)
                        setColor(6, j, k, tile);
                }
            }
        }
    }
}

arcade::Ndk::~Ndk()
{
    clear();
    endwin();
}

arcade::Ndk::Ndk() : keyboard({
                              {'0', arcade::KeyboardKey::KB_0},
                              {'1', arcade::KeyboardKey::KB_1},
                              {'2', arcade::KeyboardKey::KB_2},
                              {'3', arcade::KeyboardKey::KB_3},
                              {'4', arcade::KeyboardKey::KB_4},
                              {'5', arcade::KeyboardKey::KB_5},
                              {'6', arcade::KeyboardKey::KB_6},
                              {'7', arcade::KeyboardKey::KB_7},
                              {'8', arcade::KeyboardKey::KB_8},
                              {'9', arcade::KeyboardKey::KB_9},
                              {'a', arcade::KeyboardKey::KB_A},
                              {'b', arcade::KeyboardKey::KB_B},
                              {'c', arcade::KeyboardKey::KB_C},
                              {'d', arcade::KeyboardKey::KB_D},
                              {'e', arcade::KeyboardKey::KB_E},
                              {'f', arcade::KeyboardKey::KB_F},
                              {'g', arcade::KeyboardKey::KB_G},
                              {'h', arcade::KeyboardKey::KB_H},
                              {'i', arcade::KeyboardKey::KB_I},
                              {'j', arcade::KeyboardKey::KB_J},
                              {'k', arcade::KeyboardKey::KB_K},
                              {'l', arcade::KeyboardKey::KB_L},
                              {'m', arcade::KeyboardKey::KB_M},
                              {'n', arcade::KeyboardKey::KB_N},
                              {'o', arcade::KeyboardKey::KB_O},
                              {'p', arcade::KeyboardKey::KB_P},
                              {'q', arcade::KeyboardKey::KB_Q},
                              {'r', arcade::KeyboardKey::KB_R},
                              {'s', arcade::KeyboardKey::KB_S},
                              {'t', arcade::KeyboardKey::KB_T},
                              {'u', arcade::KeyboardKey::KB_U},
                              {'v', arcade::KeyboardKey::KB_V},
                              {'w', arcade::KeyboardKey::KB_W},
                              {'x', arcade::KeyboardKey::KB_X},
                              {'y', arcade::KeyboardKey::KB_Y},
                              {'z', arcade::KeyboardKey::KB_Z},
                              {KEY_DOWN, arcade::KeyboardKey::KB_ARROW_DOWN},
                              {KEY_UP, arcade::KeyboardKey ::KB_ARROW_UP},
                              {KEY_RIGHT, arcade::KeyboardKey::KB_ARROW_RIGHT},
                              {KEY_LEFT, arcade::KeyboardKey::KB_ARROW_LEFT},
                              {'&', arcade::KeyboardKey::KB_AMPERSAND},
                              {'\\', arcade::KeyboardKey::KB_BACKSLASH},
                              {'*', arcade::KeyboardKey::KB_ASTERISK},
                              {'@', arcade::KeyboardKey::KB_ATSYMBOL},
                              {KEY_BACKSPACE, arcade::KeyboardKey::KB_BACKSPACE},
                              {-1, arcade::KeyboardKey::KB_CAPSLOCK},
                              {'^', arcade::KeyboardKey::KB_CIRCUMFLEX},
                              {':', arcade::KeyboardKey::KB_COLON},
                              {',', arcade::KeyboardKey::KB_COMMA},
                              {KEY_DC, arcade::KeyboardKey::KB_DELETE},
                              {'$', arcade::KeyboardKey::KB_DOLLAR},
                              {'.', arcade::KeyboardKey::KB_DOT},
                              {'\"', arcade::KeyboardKey::KB_DOUBLEQUOTE},
                              {KEY_END, arcade::KeyboardKey::KB_END},
                              {KEY_ENTER, arcade::KeyboardKey::KB_ENTER},
                              {'=', arcade::KeyboardKey::KB_EQUALS},
                              {27, arcade::KeyboardKey::KB_ESCAPE},
                              {'!', arcade::KeyboardKey::KB_EXCLAMATION},
                              {KEY_F(1), arcade::KeyboardKey::KB_FN1},
                              {KEY_F(2), arcade::KeyboardKey::KB_FN2},
                              {KEY_F(3), arcade::KeyboardKey::KB_FN3},
                              {KEY_F(4), arcade::KeyboardKey::KB_FN4},
                              {KEY_F(5), arcade::KeyboardKey::KB_FN5},
                              {KEY_F(6), arcade::KeyboardKey::KB_FN6},
                              {KEY_F(7), arcade::KeyboardKey::KB_FN7},
                              {KEY_F(8), arcade::KeyboardKey::KB_FN8},
                              {KEY_F(9), arcade::KeyboardKey::KB_FN9},
                              {KEY_F(10), arcade::KeyboardKey::KB_FN10},
                              {KEY_F(11), arcade::KeyboardKey::KB_FN11},
                              {KEY_F(12), arcade::KeyboardKey::KB_FN12},
                              {'#', arcade::KeyboardKey::KB_HASHTAG},
                              {KEY_HOME, arcade::KeyboardKey::KB_HOME},
                              {'<', arcade::KeyboardKey::KB_INFERIOR},
                              {KEY_A1 ,arcade::KeyboardKey::KB_LALT},
                              {KEY_C1,arcade::KeyboardKey::KB_LCTRL},
                              {'{', arcade::KeyboardKey::KB_LEFTBRACE},
                              {'[', arcade::KeyboardKey::KB_LEFTBRACKET},
                              {'(', arcade::KeyboardKey::KB_LEFTPAREN},
                              {KEY_SLEFT,arcade::KeyboardKey::KB_LSHIFT},
                              {'-', arcade::KeyboardKey::KB_MINUS},
                              {KEY_PPAGE ,arcade::KeyboardKey::KB_PAGEDOWN},
                              {KEY_PREVIOUS, arcade::KeyboardKey::KB_PAGEUP},
                              {'%', arcade::KeyboardKey::KB_PERCENT},
                              {'+', arcade::KeyboardKey::KB_PLUS},
                              {'?', arcade::KeyboardKey::KB_QUESTION},
                              {KEY_A3, arcade::KeyboardKey::KB_RALT},
                              {KEY_C3, arcade::KeyboardKey::KB_RCTRL},
                              {'}', arcade::KeyboardKey::KB_RIGHTBRACE},
                              {']', arcade::KeyboardKey::KB_RIGHTBRACKET},
                              {')', arcade::KeyboardKey::KB_RIGHTPAREN},
                              {KEY_SRIGHT, arcade::KeyboardKey::KB_RSHIFT},
                              {';', arcade::KeyboardKey::KB_SEMICOLON},
                              {' ', arcade::KeyboardKey::KB_SPACE},
                              {'>', arcade::KeyboardKey::KB_SUPERIOR},
                              {'\'', arcade::KeyboardKey::KB_SIMPLEQUOTE},
                              {'/', arcade::KeyboardKey ::KB_SLASH},
                              {KEY_STAB, arcade::KeyboardKey::KB_TAB},
                              {'_', arcade::KeyboardKey::KB_UNDERSCORE},
                              {'|', arcade::KeyboardKey::KB_VERTICALBAR}
                      })
{
    initscr();
    keypad(stdscr, true);
    timeout(0);
    win = NULL;
    pass = false;
}

void arcade::Ndk::display()
{
    refresh();
    wrefresh(win);
}

void arcade::Ndk::clear()
{
}

void arcade::Ndk::soundControl(const arcade::Sound &sound)
{
    (void) sound;
}

void arcade::Ndk::loadSprites(std::vector<std::unique_ptr<arcade::ISprite>> &&sprites)
{
    std::string tmp;

    for (std::vector<std::unique_ptr<arcade::ISprite>>::iterator it = sprites.begin(); it != sprites.end(); ++it)
    {
        for (size_t i = 0; i < (*it)->spritesCount(); ++i)
        {
            if (i == 0)
                tmp = (*it)->getAscii(i);
            else
                tmp += (*it)->getAscii(i);
        }
        vecString.push_back(tmp);
    }
}

void arcade::Ndk::updateGUI(arcade::IGUI &gui)
{
    for (size_t i = 0; i < gui.size(); ++i)
    {
        IComponent &component = gui.at(i);
        if (component.getText() != "")
        {
            start_color();
            init_pair(1, COLOR_RED, COLOR_BLACK);
            wattron(stdscr, COLOR_PAIR(component.getTextColor().full));
            mvwprintw(stdscr, static_cast<int>(component.getX() * getmaxx(stdscr)), static_cast<int>(component.getY() * getmaxy(stdscr)), "%s",
             component.getText().c_str());
        }
    }
}

bool arcade::Ndk::doesSupportSound() const
{
    return false;
}

void arcade::Ndk::setSize(size_t Height, size_t Width)
{
    height = Height;
    width = Width;
}

void arcade::Ndk::setPosition(size_t y, size_t x)
{
    (void) y;
    (void) x;
}

void arcade::Ndk::initializeWindow()
{
    if (!pass)
    {
        win = newwin(static_cast<int>(height), static_cast<int>(width), getmaxy(stdscr) / 4, getmaxx(stdscr) / 4);
        pass = true;
    }
}

void arcade::Ndk::loadSounds(const std::vector<std::pair<std::string, arcade::SoundType>> &sounds)
{
    (void) sounds;
}

extern "C" arcade::IGfxLib *getLib()
{
    return (new arcade::Ndk());
}