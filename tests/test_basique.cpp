#include <catch2/catch_test_macros.hpp>
#include "domaine/Montant.h"
#include "domaine/Exceptions.h"

TEST_CASE("Montant : addition exacte d'entiers", "[montant]") {
    Montant a(1500);
    Montant b(2500);
    REQUIRE((a + b).valeur() == 4000); // 1500 + 2500 = 4000 : exact en entiers
}

TEST_CASE("Montant : un montant negatif est refuse", "[montant]") {
    REQUIRE_THROWS_AS(Montant(-100), QuantiteInvalideException);
}
