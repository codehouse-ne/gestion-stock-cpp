#include "interface/Menu.h"
#include <iostream>
#include <string>
#include "domaine/Produit.h"

namespace {
void afficherProduit(const Produit& p) {
    std::cout << p.id() << " | " << p.nom()
              << " | qte : " << p.quantite()
              << " | prix : " << p.prixUnitaire().versTexte();
    if (p.sousSeuilCritique()) std::cout << "  [STOCK BAS]";
    std::cout << '\n';
}
}  // namespace
Menu::Menu(GestionnaireStock& gestionnaire) : gestionnaire_(gestionnaire) {}

void Menu::afficherMenu() const {
    std::cout << "\n=== Gestion de stock ===\n"
              << "1. Lister les produits\n"
              << "0. Quitter\n"
              << "Votre choix : ";
}
void Menu::listerProduits() const {
    const auto produits = gestionnaire_.listerProduits();
    if (produits.empty()) {
        std::cout << "Aucun produit en stock.\n";
        return;
    }
    for (const Produit* p : produits) {
        afficherProduit(*p);
    }
}

void Menu::lancer() {
    std::string saisie;
    while (true) {
        afficherMenu();
        if (!std::getline(std::cin, saisie)) break;   // flux fermé (Ctrl+D)

        try {
            if (saisie == "1") {
            listerProduits();
            } else if (saisie == "0") {
                std::cout << "Au revoir.\n";
                break;
            } else {
                std::cout << "Choix invalide : tapez 1 ou 0.\n";
            }
        } catch (const StockException& e) {
            std::cout << "Erreur : " << e.what() << '\n';
        }
    }
}