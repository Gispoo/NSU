#include "./include/Menu.hpp"
#include "./include/WavFile.hpp"
#include "./include/MixerConverter.hpp"
#include "./include/SilenceConverter.hpp"
#include <iostream>
#include <limits>
#include <memory>

const std::string MessageConsts::INPUT_FILENAME = "Enter input WAV filename:\n";
const std::string MessageConsts::OUTPUT_FILENAME = "Enter output WAV filename:\n";
const std::string MessageConsts::MENU_ACTION = "\nM - Mute files, A - Add files\n";
const std::string MessageConsts::WRONG_ACTION = "Wrong action\n";
const std::string MessageConsts::TIME_INTERVAL = "Enter the time interval\n";
const std::string MessageConsts::INVALID_TIME_INTERVAL = "Invalid time interval:\n";
const std::string MessageConsts::NAME_FILE_FOR_MIX = "Enter the file name for addition:\n";
const std::string MessageConsts::INCORRECT_NAME = "The name is incorrect:\n";

void Menu::run() {
    while (true) {
        std::cout << "\nSound Processor\n";
        std::cout << "1. Mute Fragment\n";
        std::cout << "2. Mix Fragment\n";
        std::cout << "3. Exit\n";
        std::cout << "Choose an action: ";

        int choice;
        std::cin >> choice;

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
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

void Menu::muteFragment() {
    std::string inputFile, outputFile;
    double startSecond, endSecond;

    std::cout << "Enter input file name: ";
    std::cin >> inputFile;
    std::cout << "Enter output file name: ";
    std::cin >> outputFile;
    std::cout << "Enter start second for muting: ";
    std::cin >> startSecond;
    std::cout << "Enter end second for muting: ";
    std::cin >> endSecond;

    WavFile wavFile;
    if (wavFile.read(inputFile)) {
            SilenceConverter silenceConverter;
        if (silenceConverter.mute(wavFile, startSecond, endSecond)) {
            if (wavFile.write(outputFile)) {
                std::cout << "Fragment muted successfully.\n";
            } else {
                std::cout << "Error writing file.\n";
            }
        } else {
            std::cout << "Failed to mute the fragment.\n";
        }
    } else {
        std::cout << "Failed to read the file.\n";
    }
}

void Menu::mixFragment() {
    std::string inputFile1, inputFile2, outputFile;
    double startSecond, endSecond;

    std::cout << "Enter the first input file name: ";
    std::cin >> inputFile1;
    std::cout << "Enter the second input file name: ";
    std::cin >> inputFile2;
    std::cout << "Enter output file name: ";
    std::cin >> outputFile;
    std::cout << "Enter start second for mixing: ";
    std::cin >> startSecond;
    std::cout << "Enter end second for mixing: ";
    std::cin >> endSecond;
    
    WavFile wavFile1, wavFile2;
    if (wavFile1.read(inputFile1) && wavFile2.read(inputFile2)) {
        MixerConverter mixerConverter;
        if (mixerConverter.mix(wavFile1, wavFile2, startSecond, endSecond)) {
            if (wavFile1.write(outputFile)) {
                std::cout << "Fragments mixed successfully.\n";
            } else {
                std::cout << "Error writing file.\n";
            }
        } else {
            std::cout << "Failed to mix the fragments.\n";
        }
    } else {
        std::cout << "Failed to read one of the files.\n";
    }
}
