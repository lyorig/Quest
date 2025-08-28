# Quest / Halcyon

## Zadání
_Quest_ je 2D aplikace, která sice měla originálně být 2D videohra, ale její tvůrce rychle zjistil, že to fakt nebude procházka růžovým sadem.
Z originálního nápadu tak vznikly dva projekty, které měly rozdělit problém na části a zajistit stabilní harmonogram:
- **Halcyon**, C++ wrapper známé multimediální C knihovny [SDL3](https://github.com/libsdl-org/SDL), a
- **Quest**, samotná hra, která využívá Halcyon.

Projekty dlouhou dobu sloužily zejména k zlepšení znalostí jazyka C++.

## Implementace

### Halcyon
Cílem knihovny je vytvořit co nejblbuvzdornější wrapper SDL3 při zachování 1:1 rychlosti.
Toto je zajištěno zejména pomocí asi nejlepší funkce, kterou C++ nabízí oproti C -- RAII.
Třída `hal::detail::resource` pomocí šablon umožňuje jednoduchou implementaci sdílených metod
pro veškeré objekty SDL3, se kterými se pracuje formou ukazatelů, přičemž je zjednodušena práce
s pamětí, jelikož třída interně využívá `std::unique_ptr`.

Samotné wrappery objektů SDL3 zachovávají 1:1 chování, ale pomocí tříd a jejich metod, porovnejte například zde příkladové využití SDL3:
```c
SDL_Surface* surf = SDL_CreateSurface(256, 256, SDL_PIXELFORMAT_RGB24);
uint8_t r, g, b;
SDL_GetSurfaceColorMod(surf, &r, &g, &b);
SDL_DestroySurface(surf);
```
vs.
```cpp
hal::surface surf{{256, 256}, hal::pixel::format::rgb24};
const hal::color mod{surf.color_mod().get()};
// automatická dealokace
```
Výsledkem je jednodušší a bezpečnější API s prakticky nulovým zpomalením.

Mezi jiné výhody patří:
- typově bezpečné výčtové typy (`enum class`)
- jednoduché, avšak užitečné API na ladění
- utility třídy (`hal::timer`, `hal::enum_bitmask`, jmenný prostor `hal::guard`)

### Quest
Jak bylo již dříve zmíněno, Quest využívá Halcyon, čímž se zde snižují starosti o práci s pamětí.
Programátor se tedy může soustředit na tvorbu samotné hry. Jmenný prostor je zde `hq`.

Projekt obsahuje několik konceptů:
- **Texture atlas**, který s pomocí open-source knihovny _rectpack2D_ nabízí API pro ukládání vysokého množství textur
na jednu velkou texturu a kreslení z ní; toto podstatně usnadňuje práci GPU a tím pádem i zrychluje program.
- **Scene manager**, jehož šablonová černá magie umožňuje využití `std::tuple` pro ukládání a iteraci přes libovolné množství
UI scén, které dodržují tzv. _Process-Update-Draw_ (PUD) cyklus.
- V budoucnu bude implementováno API pro _Entity-Component System_ (ECS), nejspíše s pomocí knihovny [EnTT](https://github.com/skypjack/entt).

Samotná hra -- třída `hq::game` -- sice připomíná "God object", ale vyhýbá se elegantně statickým proměnným, které jsou
nechvalně známým zdrojem problémů. Mnoho komponentů aplikace ale stále potřebuje komunikovat s různými jinými komponenty,
kterých lze dosáhnout jen přes tuto třídu, takže většina funkcí a API má jako první argument `hq::game& g`, které propaguje
dle potřeby do dalších funkcí.

Žádná třída není přemrštěně velká, nýbrž jsou rozumně definovány zóny zodpovědnosti, které jsou reprezentovány přehlednou množinou tříd
a jmenných prostorů. Samotná funkce `main()` má například díky tomuto pouhé tři řádky:
```cpp
int main(int argc, char* argv[]) {
    hq::game g { hq::args { argc, argv } };
    g.main_loop();
    return EXIT_SUCCESS;
}
```
