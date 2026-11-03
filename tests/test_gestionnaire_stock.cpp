#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "domaine/ProduitElectronique.h"
#include "domaine/ProduitPerissable.h"
#include "service/GestionnaireStock.h"

namespace {

// Faux produit, uniquement pour les tests : simple et sans regle propre.
class ProduitDeTest : public Produit {
public:
    ProduitDeTest(std::string id, std::int64_t quantite)
        : Produit(std::move(id), "Lait", "Frais", quantite, 10, Montant(300)) {}

    Enregistrement versEnregistrement() const override {
        Enregistrement e;
        e.type = "test";
        remplirChampsCommuns(e);
        return e;
    }
    std::string description() const override { return "produit de test"; }
};

// Faux repository : garde tout en memoire, rien sur le disque.
class FauxRepository : public IProduitRepository, public IMouvementRepository {
public:
    std::vector<Enregistrement> chargerTout() override { return produits; }
    void sauvegarderTout(const std::vector<Enregistrement>& e) override { produits = e; }
    void ajouter(const MouvementStock& m) override { mouvements.push_back(m); }
    std::vector<MouvementStock> lister() override { return mouvements; }

    std::vector<Enregistrement> produits;
    std::vector<MouvementStock> mouvements;
};

}  // namespace

// ---------------------------------------------------------------- ajouterProduit

TEST_CASE("ajouterProduit : le produit est retrouvable", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 40));

    REQUIRE(g.trouver("P001") != nullptr);
    REQUIRE(g.trouver("P001")->quantite() == 40);
    REQUIRE(g.trouver("INCONNU") == nullptr);
}

TEST_CASE("ajouterProduit : un identifiant en double est refuse", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 40));

    REQUIRE_THROWS_AS(g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 5)),
                      ProduitDejaExistantException);
}

TEST_CASE("ajouterProduit : un pointeur nul est refuse", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);

    REQUIRE_THROWS_AS(g.ajouterProduit(nullptr), ArgumentInvalideException);
}

// ---------------------------------------------------------------- mouvements de stock

TEST_CASE("ajouterStock : augmente la quantite et trace une entree", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 40));

    g.ajouterStock("P001", 20, "Hadiza", "Livraison");

    REQUIRE(g.trouver("P001")->quantite() == 60);
    REQUIRE(repo.mouvements.size() == 1);
    REQUIRE(repo.mouvements[0].type == "entree");
    REQUIRE(repo.mouvements[0].quantite == 20);
    REQUIRE(repo.mouvements[0].auteur == "Hadiza");
    REQUIRE(repo.mouvements[0].motif == "Livraison");
}

TEST_CASE("retirerStock : diminue la quantite et trace une sortie", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 40));

    g.retirerStock("P001", 10, "Hadiza", "Vente");

    REQUIRE(g.trouver("P001")->quantite() == 30);
    REQUIRE(repo.mouvements[0].type == "sortie");
    REQUIRE(repo.mouvements[0].auteur == "Hadiza");
    REQUIRE(repo.mouvements[0].motif == "Vente");
}

TEST_CASE("retirerStock : stock insuffisant, rien n'est modifie", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 40));

    REQUIRE_THROWS_AS(g.retirerStock("P001", 50, "Hadiza", "Vente"),
                      StockInsuffisantException);
    REQUIRE(g.trouver("P001")->quantite() == 40);  // inchange
    REQUIRE(repo.mouvements.empty());              // aucune trace
}

TEST_CASE("operations sur un produit inconnu", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);

    REQUIRE_THROWS_AS(g.retirerStock("ZZZ", 1, "H", "x"), ProduitIntrouvableException);
    REQUIRE_THROWS_AS(g.ajouterStock("ZZZ", 1, "H", "x"), ProduitIntrouvableException);
    REQUIRE_THROWS_AS(g.ajusterStock("ZZZ", 1, "H", "x"), ProduitIntrouvableException);
}

TEST_CASE("quantites negatives refusees", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 40));

    REQUIRE_THROWS_AS(g.ajouterStock("P001", -5, "H", "x"), QuantiteInvalideException);
    REQUIRE_THROWS_AS(g.retirerStock("P001", -5, "H", "x"), QuantiteInvalideException);
    REQUIRE_THROWS_AS(g.ajusterStock("P001", -5, "H", "x"), QuantiteInvalideException);
    REQUIRE(g.trouver("P001")->quantite() == 40);
}

TEST_CASE("ajusterStock : fixe la quantite et trace l'ecart", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 30));

    g.ajusterStock("P001", 27, "Hadiza", "Inventaire");

    REQUIRE(g.trouver("P001")->quantite() == 27);
    REQUIRE(repo.mouvements[0].type == "ajustement");
    REQUIRE(repo.mouvements[0].quantite == -3);
}

// ---------------------------------------------------------------- consultation

TEST_CASE("valeurTotale : somme quantite x prix", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 40));  // 40 x 300
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P002", 10));  // 10 x 300

    REQUIRE(g.valeurTotale().valeur() == 15000);
}

TEST_CASE("listerProduits : tries par identifiant", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P003", 1));
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 1));
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P002", 1));

    const auto liste = g.listerProduits();

    REQUIRE(liste.size() == 3);
    REQUIRE(liste[0]->id() == "P001");
    REQUIRE(liste[1]->id() == "P002");
    REQUIRE(liste[2]->id() == "P003");
}

TEST_CASE("listerProduits : liste vide si aucun produit", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);

    REQUIRE(g.listerProduits().empty());
}

TEST_CASE("historique : un mouvement par operation, dans l'ordre", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 40));

    g.ajouterStock("P001", 20, "Hadiza", "Livraison");
    g.retirerStock("P001", 10, "Hadiza", "Vente");

    const auto journal = g.historique();
    REQUIRE(journal.size() == 2);
    REQUIRE(journal[0].type == "entree");
    REQUIRE(journal[1].type == "sortie");
}

// ---------------------------------------------------------------- sauvegarde et chargement

TEST_CASE("sauvegarder : une fiche par produit", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 40));
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P002", 10));

    g.sauvegarder();

    REQUIRE(repo.produits.size() == 2);
}

TEST_CASE("sauvegarder puis charger : les produits sont retrouves", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g1(repo, repo);
    g1.ajouterProduit(std::make_unique<ProduitPerissable>(
        "P001", "Lait", "Frais", 40, 10, Montant(300), "2026-12-31"));
    g1.ajouterProduit(std::make_unique<ProduitElectronique>(
        "E001", "Casque", "Audio", 15, 3, Montant(5000), 24));
    g1.sauvegarder();

    GestionnaireStock g2(repo, repo);
    g2.charger();

    REQUIRE(g2.listerProduits().size() == 2);
    REQUIRE(g2.trouver("P001")->quantite() == 40);
    REQUIRE(g2.trouver("E001")->quantite() == 15);
}

TEST_CASE("charger : remplace le contenu precedent", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g1(repo, repo);
    g1.ajouterProduit(std::make_unique<ProduitPerissable>(
        "P001", "Lait", "Frais", 40, 10, Montant(300), "2026-12-31"));
    g1.sauvegarder();

    GestionnaireStock g2(repo, repo);
    g2.ajouterProduit(std::make_unique<ProduitDeTest>("P002", 5));
    g2.charger();

    REQUIRE(g2.trouver("P002") == nullptr);
    REQUIRE(g2.trouver("P001") != nullptr);
}

TEST_CASE("charger : un type inconnu est refuse, l'etat precedent est conserve", "[gestionnaire]") {
    FauxRepository repo;
    GestionnaireStock g(repo, repo);
    g.ajouterProduit(std::make_unique<ProduitDeTest>("P001", 40));

    Enregistrement mauvaise;
    mauvaise.type = "type_qui_n_existe_pas";
    repo.produits = {mauvaise};

    REQUIRE_THROWS_AS(g.charger(), FormatFichierInvalideException);
    REQUIRE(g.trouver("P001") != nullptr);
    REQUIRE(g.trouver("P001")->quantite() == 40);
}

TEST_CASE("charger : un identifiant en double est refuse", "[gestionnaire]") {
    FauxRepository repo;
    ProduitPerissable lait("P001", "Lait", "Frais", 40, 10, Montant(300), "2026-12-31");
    repo.produits = {lait.versEnregistrement(), lait.versEnregistrement()};
    GestionnaireStock g(repo, repo);

    REQUIRE_THROWS_AS(g.charger(), FormatFichierInvalideException);
    REQUIRE(g.listerProduits().empty());
}