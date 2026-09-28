#pragma once
#include <string>
#include <cstdint>
#include "domaine/Montant.h"
#include "domaine/Enregistrement.h"

// Classe de base abstraite : le CONTRAT commun a tous les produits.
class Produit {
public:
    Produit(std::string id, std::string nom, std::string categorie,
            std::int64_t quantite, std::int64_t seuilCritique, Montant prixUnitaire);

    virtual ~Produit() = default;               // destructeur virtuel (obligatoire)

    Produit(const Produit&) = delete;           // copie interdite (anti-slicing)
    Produit& operator=(const Produit&) = delete;

    // Ce que chaque produit concret DOIT savoir faire :
    virtual Enregistrement versEnregistrement() const = 0;
    virtual std::string description() const = 0;

    // Comportement commun a tous les produits :
    [[nodiscard]] Montant valeurStock() const;   // quantite * prix unitaire
    [[nodiscard]] const std::string& id() const noexcept { return id_; }
    [[nodiscard]] const std::string& nom() const noexcept { return nom_; }
    [[nodiscard]] std::int64_t quantite() const noexcept { return quantite_; }
    [[nodiscard]] bool sousSeuilCritique() const noexcept;
    [[nodiscard]] const Montant& prixUnitaire() const noexcept { return prixUnitaire_; }

    // Modifies uniquement via le service (GestionnaireStock) :
    void ajouterQuantite(std::int64_t q);
    void retirerQuantite(std::int64_t q);       // leve StockInsuffisantException

protected:
    // Methode utilitaire pour les classes enfants (remplir la fiche commune)
    void remplirChampsCommuns(Enregistrement& e) const;

private:
    std::string id_;
    std::string nom_;
    std::string categorie_;
    std::int64_t quantite_;
    std::int64_t seuilCritique_;
    Montant prixUnitaire_;
};

