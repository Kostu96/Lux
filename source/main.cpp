#include "vm.hpp"
#include "chunk.hpp"
#include "debug.hpp"

#include <fstream>

bool readFile(const char* filename, char* data, size_t& size, bool binary = false)
{
    std::ifstream fin(filename, binary ? std::ios::binary : std::ios::in);
    if (!fin.is_open())
        return false;

    bool retVal;
    if (data) {
        retVal = (bool)fin.read(data, size);
    }
    else {
        fin.ignore(std::numeric_limits<std::streamsize>::max());
        retVal = true;
    }

    size = fin.gcount();
    fin.close();
    return retVal;
}

int main(int argc, const char* argv[])
{
    Lux::VM vm;
    Lux::InterpretResult result = Lux::InterpretResult::Success;

    if (argc == 1) {
        char line[1024];
        while(true) {
            std::printf("> ");
            if (!std::fgets(line, sizeof(line), stdin)) {
                std::printf("\n");
                break;
            }
            vm.interpret(line);
        }
    }
    else if (argc == 2) {
        size_t size;
        if (!readFile(argv[1], nullptr, size)) {
            std::printf("Could not open file %s", argv[1]);
            return -1;
        }

        char *source = new char[size + 1];
        if (!readFile(argv[1], source, size)) {
            std::printf("Could not read file %s", argv[1]);
            delete[] source;
            return -1;
        }
        source[size] = '\0';

        result = vm.interpret(source);

        delete[] source;
    }
    else {
        std::printf("Usage: lux [path]\n");
    }

    return static_cast<int>(result);
}
