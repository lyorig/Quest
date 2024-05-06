#include <quest/entity_manager.hpp>
#include <quest/game.hpp>

#include <halcyon/utility/timer.hpp>

int main(int argc, char* argv[]) {
    if (argc < 2)
        return EXIT_FAILURE;

    std::srand(std::time(nullptr));

    using List = hal::type_list<std::uint16_t, std::uint32_t, std::uint64_t>;
    using RFEM = List::wrap<HQ::RFEMTemplate>;

    RFEM       rfem;
    hal::timer t;

    const std::size_t TotalElements { std::stoull(argv[1]) };

    for (std::size_t i { 0 }; i < TotalElements; ++i) {
        switch (std::rand() % List::size) {
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

    HAL_PRINT("Generating elements took ", t(), 's');
    HAL_PRINT("u16: ", rfem.Get<List::at_index<0>>().size(), ", u32: ", rfem.Get<List::at_index<1>>().size(), ", u64: ", rfem.Get<List::at_index<2>>().size());

    std::uint64_t sum { 0 };

    t.reset();

    rfem.Visit([&](auto&& val) { sum += val; });

    const auto time = t();

    HAL_PRINT("Iterating over ", TotalElements, " entities took ", time, "s, sum = ", sum);

    return EXIT_SUCCESS;
}