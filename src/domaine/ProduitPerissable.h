#pragma once
#include <string>
#include "domaine/Produit.h"

//Produit perissable : possède une date de limite de consommation (format ISO : YYYY-MM-DD).
class ProduitPerissable : public Produit {
public:
    ProduitPerissable(std::string id, std::string nom, std::string categorie,
                      std::int64_t quantite, std::int64_t seuilCritique,
                      Montant prixUnitaire, std::string datePeremption);

//Contrat Produit : obligatoire d'implementer ces deux methodes
[[nodiscard]] Enregistrement versEnregistrement() const override;
[[nodiscard]] std::string description() const override;

//Spécifique aux produits périssables : 
[[nodiscard]] const std::string& datePeremption() const noexcept { return datePeremption_;}
[[nodiscard]] bool estPerimee(const std::string& dateReference) const;
private:
    std::string datePeremption_; // format ISO : YYYY-MM-DD

};