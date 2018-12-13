# CPU_Rasterization
CG project

Program je přeložitelný pomocí scriptu "compile-linux.sh", který vytvoří spustitelný soubor "prog".

Relativní cestu s aktuálního adresáře k modelu je třeba nastavit na konci souboru "src/Defines.h" jako konstantu "std::string MODEL".

Některé grafické metody lze vypnout a zapnout odstraněním definic příslušných maker v souboru "src/Defines.h"

Ve scriptu "compile-linux.sh" je nutné přidat cesty k include složkám a knihovnám SDL2, glm a assimp, pokud nejsou dostupné při běžném prohledávání adresářů v systému.

Podrobnou dokumentaci k projektu naleznete v souboru "PGR-doc-xmichl02.pdf".

Veškeré zdrojové kódy kódy projektu jsou uloženy ve složce "src".

Jeden z testovacích modelů pro ukázku lze nalést ve složce "examples".
Odkaz na další modeli je možné najít v dokumentaci.

Ve složce "res" se nachází hlavičkový soubor knihovny stbi.
Ostatní knihovny je potřeba doinstalovat pro správný běh programu.
