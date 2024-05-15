#pragma once
#include <cstdint>
#include <vector>

namespace Lux {

    enum class OpCode : uint8_t {
        Return
    };

    class Chunk :
        public std::vector<OpCode>
    {
    public:

    private:

    };

} // namespace Lux
