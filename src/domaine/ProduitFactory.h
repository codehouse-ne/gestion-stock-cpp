#pragma once
#include <memory>
#include "domaine/Produit.h"

/*Reconstruit le bon type de produit concret à partir d'un Enregistrement 
neutre chargé par la persistence. c'est le SEUL endroit du projet qui
connait la liste des types concrets*/
class ProduitFactory {
public:
    ProduitFactory() = delete; // classe utilitaire : pas d'instances
    
    [[nodiscard]] static std::unique_ptr<Produit> creer(const Enregistrement& e);
};
