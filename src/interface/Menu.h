#pragma once
#include "service/GestionnaireStock.h"

// Menu : l'interface console. Elle affiche, lit la saisie et appelle le
// service. Elle ne contient aucune règle de gestion.
class Menu {
public:
    explicit Menu(GestionnaireStock& gestionnaire);
    void lancer();

private:
    void afficherMenu() const;

    GestionnaireStock& gestionnaire_;
    void listerProduits() const;
};