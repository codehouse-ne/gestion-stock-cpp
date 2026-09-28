#pragma once
#include <vector>
#include <string>
#include <cstdint>

// Un mouvement de stock : une entree, une sortie ou un ajustement.
// Immuable : un mouvement qui a eu lieu n'est jamais modifie ni efface.
struct MouvementStock {
    std::string idProduit;
    std::string type;        // "entree" / "sortie" / "ajustement"
    std::int64_t quantite;
    std::string horodatage;  // ISO 8601, ex. 2026-09-28T19:30:00
    std::string auteur;
    std::string motif;       // obligatoire : pour l'audit
};

// Contrat du journal : ajout seul (append). On ne reecrit pas l'histoire.
class IMouvementRepository {
public:
    virtual ~IMouvementRepository() = default;

    virtual void ajouter(const MouvementStock& mouvement) = 0;
    virtual std::vector<MouvementStock> lister() = 0;
};
