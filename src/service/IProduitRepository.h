#pragma once
#include <vector>
#include "domaine/Enregistrement.h"

// Contrat de stockage des produits. "I" = Interface : purement virtuelle.
class IProduitRepository {
public:
    virtual ~IProduitRepository() = default;

    virtual std::vector<Enregistrement> chargerTout() = 0;
    virtual void sauvegarderTout(const std::vector<Enregistrement>& enregistrements) = 0;
};
