// tests/test_produit_perissable.cpp
#include <catch2/catch_test_macros.hpp>

#include "domaine/ProduitPerissable.h"
#include "domaine/Montant.h"
#include "domaine/Exceptions.h"

// =============================================================================
// TESTS DE CONSTRUCTION
// =============================================================================
TEST_CASE("ProduitPerissable : construction", "[ProduitPerissable][Contrat]") {

    SECTION("Date de péremption valide -> construction réussie") {
        REQUIRE_NOTHROW(
            ProduitPerissable("P001", "Lait", "Alimentaire", 10, 5,
                               Montant(100), "2026-12-31")
        );
    }

    SECTION("Date de péremption vide -> ArgumentInvalideException") {
        REQUIRE_THROWS_AS(
            ProduitPerissable("P001", "Lait", "Alimentaire", 10, 5,
                               Montant(100), ""),
            ArgumentInvalideException
        );
    }
}

// =============================================================================
// TESTS DE PÉREMPTION
// =============================================================================
TEST_CASE("ProduitPerissable : estPerimee", "[ProduitPerissable][Peremption]") {
    ProduitPerissable p("P001", "Lait", "Alimentaire", 10, 5,
                         Montant(100), "2026-06-15");

    SECTION("Référence après la péremption -> périmé") {
        REQUIRE(p.estPerimee("2026-06-20") == true);
    }

    SECTION("Référence avant la péremption -> pas périmé") {
        REQUIRE(p.estPerimee("2026-06-10") == false);
    }

    SECTION("Référence égale à la péremption -> pas encore périmé (comparaison stricte)") {
        REQUIRE(p.estPerimee("2026-06-15") == false);
    }

    SECTION("Référence vide -> ArgumentInvalideException") {
        REQUIRE_THROWS_AS(p.estPerimee(""), ArgumentInvalideException);
    }
}

// =============================================================================
// TEST DE SÉRIALISATION
// =============================================================================
TEST_CASE("ProduitPerissable : sérialisation", "[ProduitPerissable][Serialisation]") {
    ProduitPerissable p("P001", "Lait", "Alimentaire", 10, 5,
                         Montant(100), "2026-12-31");
    auto e = p.versEnregistrement();

    SECTION("Type et champs communs") {
        REQUIRE(e.type == "perissable");
        REQUIRE(e.champs.at("id") == "P001");
        REQUIRE(e.champs.at("nom") == "Lait");
        REQUIRE(e.champs.at("seuil_critique") == "5");
        REQUIRE(e.champs.at("prix_unitaire_francs") == "100");
    }

    SECTION("Champ spécifique : date_peremption") {
        REQUIRE(e.champs.at("date_peremption") == "2026-12-31");
    }
}

// =============================================================================
// TEST DE SUBSTITUTION : le comportement hérité de Produit tient
// =============================================================================
TEST_CASE("ProduitPerissable : hérite correctement du comportement de stock",
          "[ProduitPerissable][Liskov]") {
    ProduitPerissable p("P001", "Lait", "Alimentaire", 10, 5,
                         Montant(100), "2026-12-31");

    SECTION("retirerQuantite fonctionne comme sur un Produit de base") {
        p.retirerQuantite(4);
        REQUIRE(p.quantite() == 6);
    }

    SECTION("retirerQuantite > stock -> StockInsuffisantException, stock inchangé") {
        REQUIRE_THROWS_AS(p.retirerQuantite(100), StockInsuffisantException);
        REQUIRE(p.quantite() == 10);
    }
}