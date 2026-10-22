// tests/test_produit_contrat.cpp
#include <catch2/catch_test_macros.hpp>

#include "domaine/Produit.h"
#include "domaine/Montant.h"
#include "domaine/Exceptions.h"

// Produit est ABSTRAITE : on teste via une classe concrete minimale.
class ProduitTest : public Produit {
public:
    ProduitTest(std::string id, std::string nom, std::string categorie,
                std::int64_t quantite, std::int64_t seuil, Montant prix)
        : Produit(std::move(id), std::move(nom), std::move(categorie),
                  quantite, seuil, std::move(prix)) {}

    Enregistrement versEnregistrement() const override {
        Enregistrement e;
        e.type = "test";
        remplirChampsCommuns(e); // champs communs de la base
        return e;
    }

    std::string description() const override {
        return "Produit de test : " + nom();
    }
};

// =============================================================================
// TESTS DE CONSTRUCTION
// =============================================================================
TEST_CASE("Produit : Constructeur valide les invariants", "[Produit][Contrat]") {

    SECTION("Construction avec des arguments valides") {
        REQUIRE_NOTHROW(
            ProduitTest("P001", "Lait", "Alimentaire", 10, 5, Montant(100))
        );
    }

    SECTION("ID vide -> ArgumentInvalideException") {
        REQUIRE_THROWS_AS(
            ProduitTest("", "Lait", "Alimentaire", 10, 5, Montant(100)),
            ArgumentInvalideException
        );
    }

    SECTION("Nom vide -> ArgumentInvalideException") {
        REQUIRE_THROWS_AS(
            ProduitTest("P001", "", "Alimentaire", 10, 5, Montant(100)),
            ArgumentInvalideException
        );
    }

    SECTION("Quantité négative -> QuantiteInvalideException") {
        REQUIRE_THROWS_AS(
            ProduitTest("P001", "Lait", "Alimentaire", -10, 5, Montant(100)),
            QuantiteInvalideException
        );
    }

    SECTION("Seuil critique négatif -> QuantiteInvalideException") {
        REQUIRE_THROWS_AS(
            ProduitTest("P001", "Lait", "Alimentaire", 10, -5, Montant(100)),
            QuantiteInvalideException
        );
    }

    SECTION("Prix négatif -> refus via Montant (QuantiteInvalideException)") {
        REQUIRE_THROWS_AS(
            ProduitTest("P001", "Lait", "Alimentaire", 10, 5, Montant(-100)),
            QuantiteInvalideException
        );
    }
}

// =============================================================================
// TESTS DE STOCK
// =============================================================================
TEST_CASE("Produit : Gestion du stock", "[Produit][Stock]") {
    ProduitTest p("P001", "Lait", "Alimentaire", 10, 5, Montant(100));

    SECTION("Retrait valide") {
        p.retirerQuantite(5);
        REQUIRE(p.quantite() == 5);
    }

    SECTION("Retrait > stock -> StockInsuffisantException, stock inchangé") {
        REQUIRE_THROWS_AS(p.retirerQuantite(15), StockInsuffisantException);
        REQUIRE(p.quantite() == 10);
    }

    SECTION("Retrait négatif -> QuantiteInvalideException") {
        REQUIRE_THROWS_AS(p.retirerQuantite(-3), QuantiteInvalideException);
        REQUIRE(p.quantite() == 10);
    }

    SECTION("Retrait = 0 (comportement documenté : silencieux)") {
        p.retirerQuantite(0);
        REQUIRE(p.quantite() == 10);
    }

    SECTION("Ajout valide") {
        p.ajouterQuantite(5);
        REQUIRE(p.quantite() == 15);
    }

    SECTION("Ajout négatif -> QuantiteInvalideException") {
        REQUIRE_THROWS_AS(p.ajouterQuantite(-3), QuantiteInvalideException);
        REQUIRE(p.quantite() == 10);
    }

    SECTION("Ajout = 0 (comportement documenté : silencieux)") {
        p.ajouterQuantite(0);
        REQUIRE(p.quantite() == 10);
    }
}

// =============================================================================
// TESTS DE SEUIL CRITIQUE
// =============================================================================
TEST_CASE("Produit : Seuil critique", "[Produit][Seuil]") {

    SECTION("Sous le seuil") {
        ProduitTest p("P001", "Lait", "Alimentaire", 3, 5, Montant(100));
        REQUIRE(p.sousSeuilCritique() == true);
    }

    SECTION("À la limite du seuil") {
        ProduitTest p("P001", "Lait", "Alimentaire", 5, 5, Montant(100));
        REQUIRE(p.sousSeuilCritique() == true);
    }

    SECTION("Au-dessus du seuil") {
        ProduitTest p("P001", "Lait", "Alimentaire", 10, 5, Montant(100));
        REQUIRE(p.sousSeuilCritique() == false);
    }
}

// =============================================================================
// TESTS DE VALEUR DU STOCK
// =============================================================================
TEST_CASE("Produit : Valeur du stock", "[Produit][Valeur]") {

    SECTION("Valeur = prix * quantité") {
        ProduitTest p("P001", "Lait", "Alimentaire", 10, 5, Montant(100));
        REQUIRE(p.valeurStock().valeur() == 1000);
    }

    SECTION("Valeur avec quantité = 0") {
        ProduitTest p("P001", "Lait", "Alimentaire", 0, 5, Montant(100));
        REQUIRE(p.valeurStock().valeur() == 0);
    }

    SECTION("Grands nombres (test int64_t)") {
        ProduitTest p("P001", "Lait", "Alimentaire", 1000000, 5, Montant(2000000));
        REQUIRE(p.valeurStock().valeur() == 2000000000000LL);
    }
}

// =============================================================================
// TESTS DE SÉRIALISATION
// =============================================================================
TEST_CASE("Produit : Sérialisation en Enregistrement", "[Produit][Serialisation]") {
    ProduitTest p("P001", "Lait", "Alimentaire", 10, 5, Montant(100));
    auto e = p.versEnregistrement();

    SECTION("Les champs communs sont sérialisés") {
        REQUIRE(e.champs.count("id") == 1);
        REQUIRE(e.champs.at("id") == "P001");
        REQUIRE(e.champs.at("nom") == "Lait");
        REQUIRE(e.champs.at("quantite") == "10");
        REQUIRE(e.champs.at("seuilCritique") == "5");
        REQUIRE(e.champs.at("prixUnitaire") == "100");
    }
}