#include <QApplication>
#include <memory>

#include "interface/MainWindow.h"
#include "service/GestionnaireStock.h"
#include "persistance/InMemoryRepository.h"
#include "domaine/ProduitPerissable.h"
#include "domaine/ProduitElectronique.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialisation des repositories (en mémoire pour le moment)
    InMemoryRepository produitRepo;
    InMemoryRepository mouvementRepo;

    // Orchestrateur du métier
    GestionnaireStock gestionnaire(produitRepo, mouvementRepo);

    // Initialisation du jeu de données fidèle à la maquette validée
    gestionnaire.ajouterProduit(std::make_unique<ProduitPerissable>(
        "P1", "Lait", "Alimentaire", 40, 10, Montant(750), "2026-12-31"
    ));

    gestionnaire.ajouterProduit(std::make_unique<ProduitPerissable>(
        "P2", "Riz", "Alimentaire", 3, 10, Montant(450), "2027-06-30"
    ));

    gestionnaire.ajouterProduit(std::make_unique<ProduitElectronique>(
        "P3", "Clavier", "Électronique", 15, 5, Montant(12500), 12
    ));

    gestionnaire.ajouterProduit(std::make_unique<ProduitPerissable>(
        "P4", "Savon", "Hygiène", 25, 5, Montant(350), "2028-01-01"
    ));

    // Lancement de l'interface graphique Qt6
    MainWindow fenetre(gestionnaire);
    fenetre.show();

    return app.exec();
}
