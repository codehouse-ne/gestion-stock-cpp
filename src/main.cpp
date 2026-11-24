#include "interface/Menu.h"
#include "persistance/InMemoryRepository.h"
#include "service/GestionnaireStock.h"

int main() {
    InMemoryRepository repo;
    GestionnaireStock gestionnaire(repo, repo);
    Menu menu(gestionnaire);
    menu.lancer();
    return 0;
}