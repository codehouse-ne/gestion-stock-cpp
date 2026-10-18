#include "domaine/Produit.h"
#include "domaine/Exceptions.h"

Produit::Produit(std::string id, std::string nom, std::string categorie,
                  std::int64_t quantite, std::int64_t seuilCritique,
                  Montant prixUnitaire)
    : id_(std::move(id)),
      nom_(std::move(nom)),
      categorie_(std::move(categorie)),
      quantite_(quantite),
      seuilCritique_(seuilCritique),
      prixUnitaire_(prixUnitaire) {
    if (id_.empty() || nom_.empty()) {
        throw ArgumentInvalideException("L'identifiant et le nom sont obligatoires");
    }
    if (quantite_ < 0 || seuilCritique_ < 0) {
        throw QuantiteInvalideException("Quantite et seuil critique doivent etre >= 0");
    }
}

Montant Produit::valeurStock() const {
    return prixUnitaire_ * quantite_;
}

bool Produit::sousSeuilCritique() const noexcept {
    return quantite_ <= seuilCritique_;
}

void Produit::ajouterQuantite(std::int64_t q) {
    if (q < 0) {
        throw QuantiteInvalideException("Impossible d'ajouter une quantite negative");
    }
    quantite_ += q;
}

void Produit::retirerQuantite(std::int64_t q) {
    if (q < 0) {
        throw QuantiteInvalideException("Impossible de retirer une quantite negative");
    }
    if (q > quantite_) {
        throw StockInsuffisantException(
            "Stock insuffisant pour '" + nom_ + "' : demande " + std::to_string(q) +
            ", disponible " + std::to_string(quantite_));
    }
    quantite_ -= q;
}

void Produit::remplirChampsCommuns(Enregistrement& e) const {
    e.champs["id"] = id_;
    e.champs["nom"] = nom_;
    e.champs["categorie"] = categorie_;
    e.champs["quantite"] = std::to_string(quantite_);
    e.champs["seuilCritique"] = std::to_string(seuilCritique_);
    e.champs["prixUnitaire"] = std::to_string(prixUnitaire_.valeur());
}