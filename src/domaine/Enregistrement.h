#pragma once
#include <map>
#include <string>

// Representation neutre d'un produit, utilisee pour la persistance.
// Exemple : { type: "perissable", champs: { nom: "Lait", quantite: "40", datePeremption: "2026-12-31" } }
struct Enregistrement {
    std::string type;                          // ex. "perissable"
    std::map<std::string, std::string> champs; // cles/valeurs, format neutre
};
