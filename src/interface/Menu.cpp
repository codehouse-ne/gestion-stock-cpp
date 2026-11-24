#include "interface/Menu.h"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include "domaine/Produit.h"
#include "domaine/ProduitFactory.h"

namespace {

void afficherProduit(const Produit& p) {
    std::cout << p.id() << " | " << p.nom()
              << " | qte : " << p.quantite()
              << " | prix : " << p.prixUnitaire().versTexte();
    if (p.sousSeuilCritique()) std::cout << "  [STOCK BAS]";
    std::cout << '\n';
}

// Redemande tant que le champ est vide.
std::string lireTexte(const std::string& question) {
    std::string saisie;
    while (true) {
        std::cout << question;
        if (!std::getline(std::cin, saisie)) {
            throw ArgumentInvalideException("Saisie interrompue");
        }
        if (saisie.find_first_not_of(" \t") != std::string::npos) {
            return saisie;
        }
        std::cout << "Ce champ ne peut pas etre vide.\n";
    }
}

// Redemande tant que ce n'est pas un nombre entier >= minimum.
std::int64_t lireEntier(const std::string& question, std::int64_t minimum) {
    while (true) {
        const std::string saisie = lireTexte(question);
        try {
            std::size_t position = 0;
            const std::int64_t valeur = std::stoll(saisie, &position);
            if (position != saisie.size()) {
                throw std::invalid_argument("caracteres en trop");
            }
            if (valeur < minimum) {
                std::cout << "La valeur doit etre au moins " << minimum << ".\n";
                continue;
            }
            return valeur;
        } catch (const std::exception&) {
            std::cout << "Nombre invalide, recommencez.\n";
        }
    }
}

}  // namespace

Menu::Menu(GestionnaireStock& gestionnaire) : gestionnaire_(gestionnaire) {}

void Menu::afficherMenu() const {
    std::cout << "\n=== Gestion de stock ===\n"
              << "1. Lister les produits\n"
              << "2. Ajouter un produit\n"
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

void Menu::ajouterProduit() {
    std::cout << "\nType de produit :\n1. Perissable\n2. Electronique\n";
    const std::string choix = lireTexte("Votre choix : ");

    Enregistrement fiche;
    if (choix == "1") {
        fiche.type = "perissable";
    } else if (choix == "2") {
        fiche.type = "electronique";
    } else {
        std::cout << "Type invalide, retour au menu.\n";
        return;
    }

    // Champs communs : le service attend tout en texte.
    fiche.champs["id"] = lireTexte("Identifiant : ");
    fiche.champs["nom"] = lireTexte("Nom : ");
    fiche.champs["categorie"] = lireTexte("Categorie : ");
    fiche.champs["quantite"] = std::to_string(lireEntier("Quantite initiale : ", 0));
    fiche.champs["seuil_critique"] = std::to_string(lireEntier("Seuil critique : ", 0));
    fiche.champs["prix_unitaire_francs"] = std::to_string(lireEntier("Prix unitaire (FCFA) : ", 0));

    // Champs propres à chaque type.
    if (fiche.type == "perissable") {
        fiche.champs["date_peremption"] = lireTexte("Date de peremption (AAAA-MM-JJ) : ");
    } else {
        fiche.champs["duree_garantie_mois"] = std::to_string(lireEntier("Duree de garantie (mois) : ", 1));
    }

    gestionnaire_.ajouterProduit(ProduitFactory::creer(fiche));
    std::cout << "Produit ajoute.\n";
}

void Menu::lancer() {
    std::string saisie;
    while (true) {
        afficherMenu();
        if (!std::getline(std::cin, saisie)) break;   // flux fermé (Ctrl+D)

        try {
            if (saisie == "1") {
                listerProduits();
            } else if (saisie == "2") {
                ajouterProduit();
            } else if (saisie == "0") {
                std::cout << "Au revoir.\n";
                break;
            } else {
                std::cout << "Choix invalide : tapez 0, 1 ou 2.\n";
            }
        } catch (const StockException& e) {
            std::cout << "Erreur : " << e.what() << '\n';
        }
    }
}