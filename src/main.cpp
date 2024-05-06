#include <quest/entity_manager.hpp>
#include <quest/game.hpp>

#include <halcyon/utility/timer.hpp>

int main(int argc, char* argv[]) {
    if (argc < 2)
        return EXIT_FAILURE;

    std::srand(std::time(nullptr));

    HQ::RFEMTemplate<std::uint16_t, std::uint32_t, std::uint64_t> rfem;

    const std::size_t TotalElements { std::stoull(argv[1]) };

    for (std::size_t i { 0 }; i < TotalElements; ++i) {
        switch (std::rand() % rfem.Types()) {
        case 0:
            rfem.Spawn<std::uint16_t>(1);
            break;
        case 1:
            rfem.Spawn<std::uint32_t>(2);
            break;
        case 2:
            rfem.Spawn<std::uint64_t>(3);
            break;
        }
    }

    HAL_PRINT("Done generating elements");

    std::uint64_t sum { 0 };
    hal::timer    t;

    rfem.Visit([&](auto&& val) { sum += val; });

    const auto time = t();

    HAL_PRINT("Iterating over ", TotalElements, " entities took ", time, "s, sum = ", sum);

    return EXIT_SUCCESS;
}