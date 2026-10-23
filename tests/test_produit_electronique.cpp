#include <catch2/catch_test_macros.hpp>

#include "domaine/ProduitElectronique.h"
#include "domaine/Montant.h"
#include "domaine/Exceptions.h"

// =============================================================================
// TESTS DE CONSTRUCTION
// =============================================================================
TEST_CASE("ProduitElectronique : construction", "[ProduitElectronique][Contrat]") {

    SECTION("Garantie valide -> construction reussie") {
        REQUIRE_NOTHROW(
            ProduitElectronique("E001", "Televiseur", "Electromenager", 3, 1,
                                 Montant(500000), 24)
        );
    }

    SECTION("Garantie nulle -> ArgumentInvalideException") {
        REQUIRE_THROWS_AS(
            ProduitElectronique("E001", "Televiseur", "Electromenager", 3, 1,
                                 Montant(500000), 0),
            ArgumentInvalideException
        );
    }

    SECTION("Garantie negative -> ArgumentInvalideException") {
        REQUIRE_THROWS_AS(
            ProduitElectronique("E001", "Televiseur", "Electromenager", 3, 1,
                                 Montant(500000), -12),
            ArgumentInvalideException
        );
    }

    // Les invariants herites restent verifies par le constructeur de Produit
    SECTION("Invariants herites : ID vide -> ArgumentInvalideException") {
        REQUIRE_THROWS_AS(
            ProduitElectronique("", "Televiseur", "Electromenager", 3, 1,
                                 Montant(500000), 24),
            ArgumentInvalideException
        );
    }

    SECTION("Invariants herites : quantite negative -> QuantiteInvalideException") {
        REQUIRE_THROWS_AS(
            ProduitElectronique("E001", "Televiseur", "Electromenager", -3, 1,
                                 Montant(500000), 24),
            QuantiteInvalideException
        );
    }
}

// =============================================================================
// TESTS DE GARANTIE
// =============================================================================
TEST_CASE("ProduitElectronique : sousGarantie", "[ProduitElectronique][Garantie]") {
    ProduitElectronique p("E001", "Televiseur", "Electromenager", 3, 1,
                           Montant(500000), 24); // 24 mois de garantie

    SECTION("Achat recemment -> sous garantie") {
        // Achat janvier 2026, reference mars 2026 : 2 mois ecoules < 24
        REQUIRE(p.sousGarantie("2026-01-15", "2026-03-20") == true);
    }

    SECTION("Achat il y a exactement la duree -> plus sous garantie (comparaison stricte)") {
        // Achat janvier 2026, reference janvier 2028 : 24 mois ecoules,
        // 24 < 24 est faux -> garantie expiree le mois anniversaire.
        REQUIRE(p.sousGarantie("2026-01-15", "2028-01-20") == false);
    }

    SECTION("Achat avant la fin de la garantie -> sous garantie") {
        // 23 mois ecoules < 24
        REQUIRE(p.sousGarantie("2026-01-15", "2027-12-20") == true);
    }

    SECTION("Achat hors garantie -> false") {
        // 30 mois ecoules > 24
        REQUIRE(p.sousGarantie("2026-01-15", "2028-07-20") == false);
    }

    SECTION("Date d'achat au mauvais format -> ArgumentInvalideException") {
        REQUIRE_THROWS_AS(p.sousGarantie("15/01/2026", "2026-03-20"),
                          ArgumentInvalideException);
    }

    SECTION("Date de reference au mauvais format -> ArgumentInvalideException") {
        REQUIRE_THROWS_AS(p.sousGarantie("2026-01-15", "2026-03"),
                          ArgumentInvalideException);
    }
}

// =============================================================================
// TESTS DE SÉRIALISATION
// =============================================================================
TEST_CASE("ProduitElectronique : sérialisation", "[ProduitElectronique][Serialisation]") {
    ProduitElectronique p("E001", "Televiseur", "Electromenager", 3, 1,
                           Montant(500000), 24);
    auto e = p.versEnregistrement();

    SECTION("Type et champs communs") {
        REQUIRE(e.type == "electronique");
        REQUIRE(e.champs.count("id") == 1);
        REQUIRE(e.champs.at("id") == "E001");
        REQUIRE(e.champs.at("nom") == "Televiseur");
        REQUIRE(e.champs.at("quantite") == "3");
        REQUIRE(e.champs.at("seuil_critique") == "1");
        REQUIRE(e.champs.at("prix_unitaire_francs") == "500000");
    }

    SECTION("Champ spécifique : duree_garantie_mois") {
        REQUIRE(e.champs.count("duree_garantie_mois") == 1);
        REQUIRE(e.champs.at("duree_garantie_mois") == "24");
    }
}

// =============================================================================
// TESTS DE DESCRIPTION
// =============================================================================
TEST_CASE("ProduitElectronique : description", "[ProduitElectronique][Description]") {
    ProduitElectronique p("E001", "Televiseur", "Electromenager", 3, 1,
                           Montant(500000), 24);

    SECTION("La description contient le nom et la garantie") {
        const auto d = p.description();
        REQUIRE(d.find("Televiseur") != std::string::npos);
        REQUIRE(d.find("24") != std::string::npos);
    }
}

// =============================================================================
// TESTS DE SUBSTITUTION (Liskov) : le comportement herite de Produit tient
// =============================================================================
TEST_CASE("ProduitElectronique : hérite correctement du comportement de stock",
          "[ProduitElectronique][Liskov]") {
    ProduitElectronique p("E001", "Televiseur", "Electromenager", 3, 1,
                           Montant(500000), 24);

    SECTION("retirerQuantite fonctionne comme sur un Produit de base") {
        p.retirerQuantite(2);
        REQUIRE(p.quantite() == 1);
    }

    SECTION("retirerQuantite > stock -> StockInsuffisantException, stock inchangé") {
        REQUIRE_THROWS_AS(p.retirerQuantite(100), StockInsuffisantException);
        REQUIRE(p.quantite() == 3);
    }

    SECTION("valeurStock hérité fonctionne") {
        REQUIRE(p.valeurStock().valeur() == 1500000); // 500000 * 3
    }
}