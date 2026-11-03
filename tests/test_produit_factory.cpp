#include <catch2/catch_test_macros.hpp>

#include "domaine/ProduitFactory.h"
#include "domaine/ProduitPerissable.h"
#include "domaine/ProduitElectronique.h"
#include "domaine/Exceptions.h"

namespace {
// Construit une fiche complete ; les tests pourront ensuite ecraser ou
// effacer une cle pour la casser volontairement.
Enregistrement fichePerissable() {
    Enregistrement e;
    e.type = "perissable";
    e.champs["id"] = "P001";
    e.champs["nom"] = "Lait";
    e.champs["categorie"] = "Alimentaire";
    e.champs["quantite"] = "10";
    e.champs["seuil_critique"] = "5";
    e.champs["prix_unitaire_francs"] = "100";
    e.champs["date_peremption"] = "2026-12-31";
    return e;
}

Enregistrement ficheElectronique() {
    Enregistrement e;
    e.type = "electronique";
    e.champs["id"] = "E001";
    e.champs["nom"] = "Televiseur";
    e.champs["categorie"] = "Electromenager";
    e.champs["quantite"] = "3";
    e.champs["seuil_critique"] = "1";
    e.champs["prix_unitaire_francs"] = "500000";
    e.champs["duree_garantie_mois"] = "24";
    return e;
}
} // namespace

// =============================================================================
// RECONSTRUCTION DES TYPES CONNUS
// =============================================================================
TEST_CASE("ProduitFactory : reconstruit le bon type", "[Factory][Creation]") {

    SECTION("Type perissable -> ProduitPerissable avec tous ses champs") {
        auto p = ProduitFactory::creer(fichePerissable());

        REQUIRE(dynamic_cast<ProduitPerissable*>(p.get()) != nullptr);
        REQUIRE(p->id() == "P001");
        REQUIRE(p->nom() == "Lait");
        REQUIRE(p->quantite() == 10);
        REQUIRE(p->valeurStock().valeur() == 1000); // 100 * 10
    }

    SECTION("Type electronique -> ProduitElectronique avec tous ses champs") {
        auto p = ProduitFactory::creer(ficheElectronique());

        REQUIRE(dynamic_cast<ProduitElectronique*>(p.get()) != nullptr);
        REQUIRE(p->id() == "E001");
        REQUIRE(p->valeurStock().valeur() == 1500000); // 500000 * 3
    }
}

// =============================================================================
// ALLER-RETOUR COMPLET (le test le plus important du domaine)
// =============================================================================
TEST_CASE("ProduitFactory : aller-retour produit -> enregistrement -> produit",
          "[Factory][AllerRetour]") {

    SECTION("Perissable : la fiche reconstruite est identique a l'originale") {
        ProduitPerissable original("P001", "Lait", "Alimentaire", 10, 5,
                                    Montant(100), "2026-12-31");
        auto e1 = original.versEnregistrement();
        auto copie = ProduitFactory::creer(e1);
        auto e2 = copie->versEnregistrement();

        REQUIRE(e1.type == e2.type);
        REQUIRE(e1.champs == e2.champs); // la fiche doit etre IDENTIQUE
    }

    SECTION("Electronique : la fiche reconstruite est identique a l'originale") {
        ProduitElectronique original("E001", "Televiseur", "Electromenager", 3, 1,
                                      Montant(500000), 24);
        auto e1 = original.versEnregistrement();
        auto copie = ProduitFactory::creer(e1);
        auto e2 = copie->versEnregistrement();

        REQUIRE(e1.type == e2.type);
        REQUIRE(e1.champs == e2.champs);
    }
}

// =============================================================================
// ERREURS DE FORMAT
// =============================================================================
TEST_CASE("ProduitFactory : refuse les fiches mal formees", "[Factory][Erreurs]") {

    SECTION("Champ commun manquant -> FormatFichierInvalideException") {
        auto e = fichePerissable();
        e.champs.erase("quantite");
        REQUIRE_THROWS_AS(ProduitFactory::creer(e), FormatFichierInvalideException);
    }

    SECTION("Champ specifique manquant (perissable) -> FormatFichierInvalideException") {
        auto e = fichePerissable();
        e.champs.erase("date_peremption");
        REQUIRE_THROWS_AS(ProduitFactory::creer(e), FormatFichierInvalideException);
    }

    SECTION("Champ specifique manquant (electronique) -> FormatFichierInvalideException") {
        auto e = ficheElectronique();
        e.champs.erase("duree_garantie_mois");
        REQUIRE_THROWS_AS(ProduitFactory::creer(e), FormatFichierInvalideException);
    }

    SECTION("Nombre illisible -> FormatFichierInvalideException") {
        auto e = fichePerissable();
        e.champs["quantite"] = "dix"; // pas un nombre !
        REQUIRE_THROWS_AS(ProduitFactory::creer(e), FormatFichierInvalideException);
    }

    SECTION("Type inconnu -> FormatFichierInvalideException") {
        auto e = fichePerissable();
        e.type = "textile"; // type qui n'existe pas encore
        REQUIRE_THROWS_AS(ProduitFactory::creer(e), FormatFichierInvalideException);
    }

    SECTION("Erreur metier remonte telle quelle : date vide -> ArgumentInvalideException") {
        auto e = fichePerissable();
        e.champs["date_peremption"] = ""; // fiche bien formee, c'est le METIER qui refuse
        REQUIRE_THROWS_AS(ProduitFactory::creer(e), ArgumentInvalideException);
    }
}