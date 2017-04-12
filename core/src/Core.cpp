//
// Created by Pierre Bougon on 15/03/17.
//

#include <unistd.h>
#include <algorithm>
#include "DLLoader.hpp"
#include "Logger.hpp"
#include "Core.hpp"

arcade::Core::Core() : tabLib(), tabGame(), pars(),
                       open(false), events(),
                       currentLib(nullptr), currentGame(nullptr)
{
    loadDependencies();

    setLib("./lib/lib_arcade_lapin.so");

    //TODO: Basic choice should be decided on a menu
    setGame(GAME);
}

arcade::Core::Core(std::string const &lib) : tabLib(), tabGame(), pars(),
                                             open(false), events(),
                                             currentLib(nullptr), currentGame(nullptr)
{
    loadDependencies();

    setLib(lib);
    //TODO: Basic choice should be decided on a menu
    setGame(GAME);
}

arcade::Core::~Core()
{
}

void arcade::Core::init(std::string const &lib)
{
    loadDependencies();

    setLib(lib);
    //TODO: Basic choice should be decided on a menu
    setGame("./games/lib_arcade_snake.so");
}

void arcade::Core::run()
{
    Tick tick;

    open = true;
    while (isOpen())
    {
        manageEvents();
        if (!isOpen())
            return;
        if (currentGame && tick.isTick())
        {
            playSound();
            switch (currentGame->getGameState())
            {
                case GameState::QUIT :
                    quitArcade();
                    return;
                case GameState::INGAME :
                    currentGame->process();
                    drawFrame();
                    break;
                case GameState::MENU :
                    quitGame();
                    break;
                default:
                    break;
            }
        }
        else if (!currentGame)
        {
            currentLib->clear();
            //TODO: display arcade menu
        }
        else
        {
            usleep(100);
        }
        currentLib->display();
    }
}

void arcade::Core::feedSound()
{
    sound = std::make_unique<DLLoader<IGfxLib>>("./soundManager/lib_sound_manager.so", RTLD_GLOBAL);
}

void arcade::Core::feedLib()
{
    for (std::vector<std::string>::const_iterator it = pars.getVecLib().begin(); it != pars.getVecLib().end(); it++)
    {
        std::unique_ptr<DLLoader<IGfxLib>> loader = std::make_unique<DLLoader<IGfxLib>>("./lib/" + *it);
        if (loader->getError() == DLLoadingError::DLLError::NONE)
        {
            tabLib.push_back(std::move(loader));
        }
        else
        {
            Logger::log(Logger::Error,
                        *it + " : This lib cannot be loaded, check out your lib/ directory to see your library");
        }
    }
}

void arcade::Core::feedGame()
{
    for (std::vector<std::string>::const_iterator it = pars.getVecGame().begin(); it != pars.getVecGame().end(); it++)
    {
        std::unique_ptr<DLLoader<IGame>> loader = std::make_unique<DLLoader<IGame>>("./games/" + *it);
        if (loader->getError() == DLLoadingError::DLLError::NONE)
        {
            tabGame.push_back(std::move(loader));
        }
        else
        {
            Logger::log(Logger::Error,
                        *it + " : This game cannot be loaded, check out your games/ directory to see your games");
        }
    }
}

int arcade::Core::getIndexLib(std::string const &lib) const
{
    return (getIndexVec(lib, pars.getVecLib()));
}

int arcade::Core::getIndexGame(std::string const &game) const
{
    return (getIndexVec(game, pars.getVecGame()));
}

int arcade::Core::getIndexVec(std::string const &lib, std::vector<std::string> vec) const
{
    int i = 0;

    for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (*it == lib)
            return i;
        ++i;
    }
    return (-1);
}

arcade::IGame *arcade::Core::findGame(const std::string &game)
{
    std::vector<std::string>::const_iterator it =
            std::find_if(pars.getVecGame().begin(), pars.getVecGame().end(), [game](std::string const &_game) {
                return _game == game.substr((game.find_last_of("/") == game.npos) ? 0 : game.find_last_of("/") + 1);
            });
    if (it == pars.getVecGame().end())
    {
        Logger::log(Logger::Error,
                    game + " : This game has not been found it may happen when a lib cannot be loaded");
        return nullptr;
    }
    IGame *found = tabGame[it - pars.getVecGame().begin()]->getInstance("getGame");
    posGame = static_cast<size_t >(it - pars.getVecGame().begin());
    if (!found)
    {
        Logger::log(Logger::Warning, game + " : This game is corrupted and cannot be loaded");
    }
    return found;
}

arcade::IGfxLib *arcade::Core::findLib(const std::string &lib)
{
    std::vector<std::string>::const_iterator it =
            std::find_if(pars.getVecLib().begin(), pars.getVecLib().end(), [lib](std::string const &_lib){
                return _lib == lib.substr((lib.find_last_of("/") == lib.npos) ? 0 : lib.find_last_of("/") + 1);
            });
    if (it == pars.getVecLib().end())
    {
        Logger::log(Logger::Error,
                    lib + " : This lib has not been found it may happen when a lib cannot be loaded");
        return nullptr;
    }
    IGfxLib *found = tabLib[it - pars.getVecLib().begin()]->getInstance("getLib");
    posLib = static_cast<size_t >(it - pars.getVecLib().begin());
    if (!found)
    {
        Logger::log(Logger::Warning, lib + " : This lib is corrupted and cannot be loaded");
    }
    return found;
}

void arcade::Core::setGame(const std::string &game)
{
    size_t nbTested = 0;
    std::vector<std::string>::const_iterator it = std::find_if(pars.getVecGame().cbegin(), pars.getVecGame().cend(), [game](std::string const &_game){
        return _game == game.substr((game.find_last_of("/") == game.npos) ? 0 : game.find_last_of("/") + 1);
    });

    currentGame = nullptr;
    while (!currentGame && nbTested < pars.getVecGame().size())
    {
        if (it == pars.getVecGame().end())
            it = pars.getVecGame().begin();
        currentGame = findGame(*it);
        if (currentGame)
            initGame();
        ++it;
        ++nbTested;
    }
    if (nbTested == tabGame.size())
        throw DLLoadingError(ALL_GAME_CORRUPTED_ERROR_MSG, DLLoadingError::DLLError::GAMES_CORRUPTED);
}

void arcade::Core::setLib(const std::string &lib)
{
    size_t nbTested = 0;
    std::vector<std::string>::const_iterator it = std::find_if(pars.getVecLib().cbegin(), pars.getVecLib().cend(), [lib](std::string const &_lib){
        return _lib == lib.substr((lib.find_last_of("/") == lib.npos) ? 0 : lib.find_last_of("/") + 1);
    });

    currentLib = nullptr;
    while (!currentLib && nbTested < pars.getVecLib().size())
    {
        if (it == pars.getVecLib().end())
            it = pars.getVecLib().begin();
        currentLib = findLib(*it);
        ++it;
        ++nbTested;
    }
    if (nbTested == tabLib.size())
        throw DLLoadingError(ALL_LIB_CORRUPTED_ERROR_MSG, DLLoadingError::DLLError::LIBRARIES_CORRUPTED);
}

void arcade::Core::nextGame()
{
    if (posGame >= tabGame.size() - 1)
    {
        currentGame = tabGame.front()->getInstance("getGame");
        posLib = 0;
    }
    else
        currentGame = tabGame[++posGame]->getInstance("getGame");
}

void arcade::Core::prevGame()
{
    if (posLib == 0)
    {
        currentLib = tabLib.back()->getInstance("getGame");
        posLib = tabLib.size() - 1;
    }
    else
        currentGame = tabGame[--posGame]->getInstance("getGame");
}

void arcade::Core::nextLib()
{
    if (posLib >= tabLib.size() - 1)
    {
        currentLib = tabLib.front()->getInstance("getLib");
        posLib = 0;
    }
    else
        currentLib = tabLib[++posLib]->getInstance("getLib");
}

void arcade::Core::prevLib()
{
    if (posLib == 0)
    {
        currentLib = tabLib.back()->getInstance("getLib");
        posLib = tabLib.size() - 1;
    }
    else
        currentLib = tabLib[++posLib]->getInstance("getLib");
}

void arcade::Core::drawFrame()
{
    // TODO : check
    currentLib->clear();

    currentLib->updateMap(currentGame->getCurrentMap());
    currentLib->updateGUI(currentGame->getGUI());
}

void arcade::Core::initGame()
{
    currentLib->loadSounds(currentGame->getSoundsToLoad());
    currentLib->loadSprites(currentGame->getSpritesToLoad());
}

bool arcade::Core::isOpen() const
{
    return open;
}

void arcade::Core::quitGame()
{
    currentGame = nullptr;
}

void arcade::Core::quitArcade()
{
    open = false;
}

void arcade::Core::playSound()
{
    for (Sound sound : currentGame->getSoundsToPlay())
        currentLib->soundControl(std::move(sound));
}

void arcade::Core::sendNetwork()
{
    //currentGame->getNetworkToSend()
}

void arcade::Core::manageEvents()
{
    Event               event;
    std::vector<Event>  _events;

    while (currentLib->pollEvent(event))
    {
        switch (event.type)
        {
            case EventType::ET_QUIT :
                open = false;
                break;
            case EventType::ET_KEYBOARD :
                if (coreEvent(event))
                    break;
            default:
                _events.push_back(event);
                break;
        }
    }
    if (currentGame)
        currentGame->notifyEvent(std::move(_events));
    else
        events = _events;
}

bool arcade::Core::coreEvent(const arcade::Event &event)
{
    if (event.action == ActionType::AT_PRESSED)
    {
        switch (event.kb_key)
        {
/*
            case KB_2:
                prevLib();
                return true;
            case KB_3:
                nextLib();
                return true;
            case KB_4:
                prevGame();
                return true;
            case KB_5:
                nextGame();
                return true;
*/
            case KB_ESCAPE:
                open = false;
                return true;
            default:
                return false;
        }
    }
    return false;
}

void arcade::Core::loadDependencies()
{
    pars.FeedVecLib("./lib");
    if (pars.getVecLib().size() == 0)
        throw LoadingExceptions("Cannot open lib/ directory");
    pars.FeedVecGame("./games");
    if (pars.getVecGame().size() == 0)
        throw LoadingExceptions("Cannot open game/ directory");
    feedLib();
    if (tabLib.size() == 0)
        throw DLLoadingError(NO_LIB_ERROR_MSG, DLLoadingError::DLLError::NO_LIB_LOADED_ERROR);
    feedGame();
    if (tabGame.size() == 0)
        throw DLLoadingError(NO_GAME_ERROR_MSG, DLLoadingError::DLLError::NO_GAME_LOADED_ERROR);
}


/**************************************************************************************
 * Some error messages to make the code easier to understand and more beautiful
 **************************************************************************************/
namespace arcade
{
    const std::string Core::NO_LIB_ERROR_MSG = "Cannot load any graphic library, please checkout your lib/ directory to check if there is your library, else your library cannot be loaded";

    const std::string Core::NO_GAME_ERROR_MSG = "Cannot load any game, please checkout your games/ directory to check if there is your games, else your games cannot be loaded";

    const std::string Core::ALL_LIB_CORRUPTED_ERROR_MSG = "All your libraries are corrupted, it may happen when a library is badly implemented";

    const std::string Core::ALL_GAME_CORRUPTED_ERROR_MSG = "All your games are corrupted, it may happen when a game is badly implemented";
}