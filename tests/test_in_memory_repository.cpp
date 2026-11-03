#include <catch2/catch_test_macros.hpp>

#include "persistance/InMemoryRepository.h"

namespace {

Enregistrement fiche(const std::string& id) {
    Enregistrement e;
    e.type = "test";
    e.champs["id"] = id;
    return e;
}

MouvementStock mouvement(const std::string& id, const std::string& type) {
    return MouvementStock{id, type, 5, "2026-10-20T10:00:00", "Hadiza", "test"};
}

}  // namespace

TEST_CASE("InMemoryRepository : un repository neuf est vide", "[repository]") {
    InMemoryRepository repo;

    REQUIRE(repo.chargerTout().empty());
    REQUIRE(repo.lister().empty());
}

TEST_CASE("InMemoryRepository : sauvegarderTout puis chargerTout", "[repository]") {
    InMemoryRepository repo;

    repo.sauvegarderTout({fiche("P001"), fiche("P002")});

    REQUIRE(repo.chargerTout().size() == 2);
    REQUIRE(repo.chargerTout()[0].champs.at("id") == "P001");
}

TEST_CASE("InMemoryRepository : sauvegarderTout remplace, il n'ajoute pas", "[repository]") {
    InMemoryRepository repo;
    repo.sauvegarderTout({fiche("P001"), fiche("P002")});

    repo.sauvegarderTout({fiche("P003")});

    REQUIRE(repo.chargerTout().size() == 1);
    REQUIRE(repo.chargerTout()[0].champs.at("id") == "P003");
}

TEST_CASE("InMemoryRepository : le journal garde l'ordre d'ajout", "[repository]") {
    InMemoryRepository repo;

    repo.ajouter(mouvement("P001", "entree"));
    repo.ajouter(mouvement("P001", "sortie"));

    const auto journal = repo.lister();
    REQUIRE(journal.size() == 2);
    REQUIRE(journal[0].type == "entree");
    REQUIRE(journal[1].type == "sortie");
}

TEST_CASE("InMemoryRepository : modifier la copie ne change pas le repository", "[repository]") {
    InMemoryRepository repo;
    repo.ajouter(mouvement("P001", "entree"));

    auto copie = repo.lister();
    copie.clear();

    REQUIRE(repo.lister().size() == 1);
}

TEST_CASE("InMemoryRepository : produits et journal sont independants", "[repository]") {
    InMemoryRepository repo;

    repo.sauvegarderTout({fiche("P001")});

    REQUIRE(repo.lister().empty());
}