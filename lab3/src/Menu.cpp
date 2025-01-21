#include "./include/Menu.hpp"
#include "./include/WavFile.hpp"
#include "./include/ConfigParser.hpp"
#include "./include/Converter.hpp"
#include "./include/MixerConverter.hpp"
#include <iostream>
#include <limits>
#include <memory>

void Menu::run() {
    while (true) {
        showMenu();
        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1:
            processFile();
            break;
        case 0:
            std::cout << "Exiting...\n";
            return;
        default:
            std::cout << "Invalid choice, try again.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void Menu::showMenu() {
    std::cout << "\nSound Processor Menu\n";
    std::cout << "1. Process WAV file\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice: ";
}

void Menu::processFile() {
    std::string inputFilename;
    std::string configFilename;
    std::string outputFilename;

    std::cout << "Enter input WAV filename: ";
    std::cin >> inputFilename;
    std::cout << "Enter config filename: ";
    std::cin >> configFilename;
    std::cout << "Enter output WAV filename: ";
    std::cin >> outputFilename;
    
    WavFile wavFile(inputFilename);
    if (!wavFile.isValid()) {
        std::cout << "Error loading input file\n";
        return;
    }
    ConfigParser configParser(configFilename);
    if (!configParser.isValid()) {
        std::cout << "Error loading config file\n";
        return;
    }

    std::vector<std::unique_ptr<Converter>> converters = configParser.parseConfig();
    std::vector<int16_t> samples = wavFile.getSamples();

    for (const auto& converter : converters) {
        if (auto mixer = dynamic_cast<MixerConverter*>(converter.get())) {
            std::string mixFilename;
            std::cout << "Enter mix file name (if need): ";
            std::cin >> mixFilename;
            try {
                mixer->setMixFile(mixFilename);
            }
            catch (const std::runtime_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
        try {
            samples = converter->process(samples);
        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    if (wavFile.save(outputFilename,samples)) {
        std::cout << "File succesfuly processed" << std::endl;
    } else {
        std::cout << "Error when saving" << std::endl;
    }
}
