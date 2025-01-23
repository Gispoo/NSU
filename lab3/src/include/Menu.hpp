#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

class MessageConsts {
public:
    static const std::string INPUT_FILENAME;
    static const std::string OUTPUT_FILENAME;
    static const std::string MENU_ACTION;
    static const std::string WRONG_ACTION;
    static const std::string TIME_INTERVAL;
    static const std::string INVALID_TIME_INTERVAL;
    static const std::string NAME_FILE1_FOR_MIX;
    static const std::string NAME_FILE2_FOR_MIX;
    static const std::string INCORRECT_NAME;
    static const std::string START_SECOND;
    static const std::string END_SECOND;
    static const std::string FRAGMENT_MUTED;
    static const std::string FRAGMENTS_MIXED;
    static const std::string ERROR_READING_FILE;
    static const std::string ERROR_WRITING_FILE;
    static const std::string FAILED_MUTE_FRAGMENT;
    static const std::string FAILED_MIX_FRAGMENT;
    static const std::string INVALID_WAV_FORMAT;
    static const std::string FILES_MUST_BE_SAME;
};

class Menu {
public:
    void run();
private:
    void muteFragment();
    void mixFragment();

    MessageConsts messages;
    std::string inputFile, outputFile;
    double startSecond, endSecond;
};

#endif
