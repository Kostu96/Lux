#include "compiler.hpp"
#include "chunk.hpp"

#include <gtest/gtest.h>

TEST(ErrorOutputTests, givenSourceWithMissingSemicolonWhenInterpretingThenCorrectErrorOutputIsProduced)
{
    const char* source = R"(
var value;
value = 42
print value;
)";
    Lux::Compiler compiler;
    Lux::Chunk chunk;

    testing::internal::CaptureStderr();

    bool result = compiler.compile(source, chunk);
    EXPECT_EQ(result, false);

    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_STREQ(output.c_str(), "[line 4 | col 7] Error at 'print': Expect ';' after an expression.\n");
}
