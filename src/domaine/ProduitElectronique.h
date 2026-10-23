#pragma once
#include <string>
#include "domaine/Produit.h"

// Produit electronique : possede une duree de garantie en mois (strictement positive).
class ProduitElectronique : public Produit {
public:
    ProduitElectronique(std::string id, std::string nom, std::string categorie,
                        std::int64_t quantite, std::int64_t seuilCritique,
                        Montant prixUnitaire, std::int64_t dureeGarantieMois);

    // Contrat Produit : obligatoire d'implementer ces deux methodes
    [[nodiscard]] Enregistrement versEnregistrement() const override;
    [[nodiscard]] std::string description() const override;

    // Specifique aux produits electroniques :
    [[nodiscard]] std::int64_t dureeGarantieMois() const noexcept { return dureeGarantieMois_; }

    // Le produit est-il encore sous garantie a la date de reference ?
    // Dates au format ISO YYYY-MM-DD. Regle : la garantie couvre strictement
    // moins de dureeGarantieMois_ mois apres l'achat (le mois anniversaire
    // exact de la fin de garantie, il n'est plus couvert).
    [[nodiscard]] bool sousGarantie(const std::string& dateAchat,
                                    const std::string& dateReference) const;
private:
    std::int64_t dureeGarantieMois_; // en mois, strictement positif
};