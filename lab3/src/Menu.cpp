#include "./include/Menu.hpp"
#include "./include/WavFile.hpp"
#include "./include/MixerConverter.hpp"
#include "./include/SilenceConverter.hpp"
#include "./exception/include/WavFileException.hpp"
#include <iostream>
#include <limits>
#include <memory>

const std::string MessageConsts::INPUT_FILENAME = "Enter input WAV filename: ";
const std::string MessageConsts::OUTPUT_FILENAME = "Enter output WAV filename: ";
const std::string MessageConsts::MENU_ACTION = "\nSound Processor\n1. Mute Fragment\n2. Mix Fragment\n3. Exit\nChoose an action: ";
const std::string MessageConsts::WRONG_ACTION = "\nInvalid choice. Please try again.\n";
const std::string MessageConsts::TIME_INTERVAL = "Enter the time interval\n";
const std::string MessageConsts::INVALID_TIME_INTERVAL = "Invalid time interval:\n";
const std::string MessageConsts::NAME_FILE1_FOR_MIX = "Enter the first input file name: ";
const std::string MessageConsts::NAME_FILE2_FOR_MIX = "Enter the second input file name: ";
const std::string MessageConsts::INCORRECT_NAME = "The name is incorrect:\n";
const std::string MessageConsts::START_SECOND = "Enter start second: ";
const std::string MessageConsts::END_SECOND = "Enter end second: ";
const std::string MessageConsts::FRAGMENT_MUTED = "Fragment muted successfully.\n";
const std::string MessageConsts::FRAGMENTS_MIXED = "Fragments mixed successfully.\n";
const std::string MessageConsts::ERROR_READING_FILE = "Failed to read the file.\n";
const std::string MessageConsts::ERROR_WRITING_FILE = "Error writing file.\n";
const std::string MessageConsts::FAILED_MUTE_FRAGMENT = "Failed to mute the fragment.\n";
const std::string MessageConsts::FAILED_MIX_FRAGMENT = "Failed to mix the fragments.\n";
const std::string MessageConsts::INVALID_WAV_FORMAT = "Invalid WAV file format.\n";
const std::string MessageConsts::FILES_MUST_BE_SAME = "Files must have the same number of channels, bit rate, and sample rate.\n";

void Menu::run() {
    while (true) {
        std::cout << messages.MENU_ACTION;

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cout << messages.WRONG_ACTION;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистка буфера ввода до новой строки
            continue;
        }

        switch (choice) {
            case 1:
                muteFragment();
                break;
            case 2:
                mixFragment();
                break;
            case 3:
                return;
            default:
                std::cout << messages.WRONG_ACTION;
        }
    }
}

void Menu::muteFragment() {
    std::cout << messages.INPUT_FILENAME;
    std::cin >> inputFile;
    std::cout << messages.OUTPUT_FILENAME;
    std::cin >> outputFile;
    std::cout << messages.START_SECOND;
    std::cin >> startSecond;
    std::cout << messages.END_SECOND;
    std::cin >> endSecond;

    try {
    WavFile wavFile;
    if (wavFile.read(inputFile)) {
        SilenceConverter silenceConverter;
        if (silenceConverter.mute(wavFile, startSecond, endSecond)) {
            if (wavFile.write(outputFile)) {
                std::cout << messages.FRAGMENT_MUTED;
            } else {
                std::cout << messages.ERROR_WRITING_FILE;
            }
        } else {
            std::cout << messages.FAILED_MUTE_FRAGMENT;
        }
    } else {
        std::cout << messages.ERROR_READING_FILE;
    }
    } catch (const WavFileException& e) {
        std::cerr << e.what() << std::endl;
    }
}

void Menu::mixFragment() {
    std::string inputFile2;
    std::cout << messages.NAME_FILE1_FOR_MIX;
    std::cin >> inputFile;
    std::cout << messages.NAME_FILE2_FOR_MIX;
    std::cin >> inputFile2;
    std::cout << messages.OUTPUT_FILENAME;
    std::cin >> outputFile;
    std::cout << messages.START_SECOND;
    std::cin >> startSecond;
    std::cout << messages.END_SECOND;
    std::cin >> endSecond;
    
    try {
    WavFile wavFile1, wavFile2;
    if (wavFile1.read(inputFile) && wavFile2.read(inputFile2)) {
        MixerConverter mixerConverter;
        if (mixerConverter.mix(wavFile1, wavFile2, startSecond, endSecond)) {
            if (wavFile1.write(outputFile)) {
                std::cout << messages.FRAGMENTS_MIXED;
            } else {
                std::cout << messages.ERROR_WRITING_FILE;
            }
        } else {
            std::cout << messages.FAILED_MIX_FRAGMENT;
        }
    } else {
        std::cout << messages.ERROR_READING_FILE;
    }
    } catch (const WavFileException& e) {
        std::cerr << e.what() << std::endl;
    }
}
