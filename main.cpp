// Copyright Aleksey Nikolaev 2020

#include <Magick++.h>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct Commands
{
    std::vector<std::string> names;
    std::function<bool(std::stringstream &)> executer;
    std::string description;
};

int main(int, char **argv)
{
    Magick::InitializeMagick(*argv);
    std::unordered_map<std::string, Magick::Image> namedImages;
    bool finish = false;
    std::vector<Commands> commands = {
        {{"load", "ld"},
         [&namedImages](std::stringstream &ss) {
             std::string name, filename;
             ss >> name >> filename;
             if (name.empty() || filename.empty())
                 return false;
             namedImages[name].read(filename);
             return true;
         },
         "load, ld <name> <filename>      load image from file\n"
         "                                <name> - identification name, used as source name by some command\n"
         "                                <filename> - file name of image on disk\n"},
        {{"store", "s"},
         [&namedImages](std::stringstream &ss) {
             std::string name, filename;
             ss >> name >> filename;
             auto imageId = namedImages.find(name);
             if (imageId == namedImages.end() || filename.empty())
                 return false;
             imageId->second.write(filename);
             return true;
         },
         "store, s <name> <filename>      Write image to file \n"
         "                                <name> - identification name of image \n"
         "                                <filename> - file name for image storing\n"},
        {{"blur"},
         [&namedImages](std::stringstream &ss) {
             std::string from_name, to_name;
             int size = -1;
             ss >> from_name >> to_name >> size;
             auto imageId = namedImages.find(from_name);
             if (imageId == namedImages.end() || to_name.empty() || size < 0)
                 return false;
             namedImages[to_name] = imageId->second;
             namedImages[to_name].blur(size);
             return true;
         },
         "blur <from_name> <to_name> <size>\n"
         "                                Blur image\n"
         "                                <from_name> - identification name of source image \n"
         "                                <to_name> - identification name of destination image\n"
         "                                <size> - size of blur \n"},
        {{"resize"},
         [&namedImages](std::stringstream &ss) {
             std::string from_name, to_name;
             int new_width = -1;
             int new_height = -1;
             ss >> from_name >> to_name >> new_width >> new_height;
             auto imageId = namedImages.find(from_name);
             if (imageId == namedImages.end() || to_name.empty() || new_width < 0 || new_height < 0)
                 return false;
             namedImages[to_name] = imageId->second;
             namedImages[to_name].resize(Magick::Geometry(new_width, new_height));
             return true;
         },
         "resize <from_name> <to_name> <new_width> <new_height> \n"
         "                                Image resizing \n"
         "                                <from_name> - identification name of source image \n"
         "                                <to_name> - identification name of destination image \n"
         "                                <new_width>, <new_height> - new image dimensions\n"},
        {{"help", "h"},
         [&commands = std::as_const(commands)](std::stringstream &) {
             for (const auto &command : commands) {
                 std::cout << command.description;
             }
             return true;
         },
         "help, h                         Displays help about supported commands\n"},
        {{"exit", "quit", "q"},
         [&finish](std::stringstream &) {
             finish = true;
             return true;
         },
         "exit, quit, q                   Exit\n"},
    };
    std::unordered_map<std::string, decltype(commands.size())> associate;
    for (auto i = commands.size(); i-- > 0;) {
        for (const auto &name : commands[i].names) {
            associate[name] = i;
        }
    }
    while (!finish) {
        std::cout << "> ";
        std::string row;
        std::getline(std::cin, row);
        std::stringstream ss(row);

        std::string name;
        ss >> name;

        auto commandId = associate.find(name);
        if (commandId == associate.end()) {
            std::cout << "Unknown command. Use 'help'" << std::endl;
        } else {
            try {
                if (!commands[commandId->second].executer(ss)) {
                    std::cout << "Wrong format" << std::endl;
                    std::cout << commands[commandId->second].description << std::endl;
                }
            } catch (Magick::Exception &error_) {
                std::cout << "Caught exception: " << error_.what() << std::endl;
            }
        }
    }

    return 0;
}
