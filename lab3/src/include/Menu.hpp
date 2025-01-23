#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

enum class Action {
    MUTE,
    ADD,
    INVALID
};

class MessageConsts {
public:
    static const std::string INPUT_FILENAME;
    static const std::string OUTPUT_FILENAME;
    static const std::string MENU_ACTION;
    static const std::string WRONG_ACTION;
    static const std::string TIME_INTERVAL;
    static const std::string INVALID_TIME_INTERVAL;
    static const std::string NAME_FILE_FOR_MIX;
    static const std::string INCORRECT_NAME;
};

class Menu {
public:
    void run();
private:
    void muteFragment();
    void mixFragment();
};

#endif
