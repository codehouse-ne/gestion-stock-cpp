#include "domaine/ProduitPerissable.h"
#include "domaine/Exceptions.h"

// 1. Le constructeur
ProduitPerissable::ProduitPerissable(std::string id, std::string nom, std::string categorie, 
                                     std::int64_t quantite, std::int64_t seuilCritique, 
                                     Montant prixUnitaire, std::string datePeremption)
    : Produit(std::move(id), std::move(nom), std::move(categorie), quantite, seuilCritique, std::move(prixUnitaire)),
      datePeremption_(std::move(datePeremption)) {
        //Règle métier : la date ne peut pas être vide (format ISO : YYYY-MM-DD) 
    if (datePeremption_.empty()) {
        throw ArgumentInvalideException("La date de péremption est obligatoire");
    }
}

// 2. Sérialisation pour la persistence
Enregistrement ProduitPerissable::versEnregistrement() const {
    Enregistrement e;
    e.type = "perissable";

    //On réutilise la méthode de la classe parente pour les 6 champs communs : 
    //id, nom, categorie, quantite, seuilCritique, prix_unitaire_francs
    remplirChampsCommuns(e);

    //On ajoute le champpropre à cette classe (en snake_case) : 
    e.champs["date_peremption"]= datePeremption_;
    return e;
}

// 3. Description textuelle
std::string ProduitPerissable::description() const {
    return nom() + " (périssable, date de péremption : " + datePeremption_ + ")";
}

// 4.Détection de la péremption par rapport à une date de référence (format ISO : YYYY-MM-DD)
bool ProduitPerissable::estPerimee(const std::string& dateReference) const {
    if (dateReference.empty()) {
        throw ArgumentInvalideException("La date de référence ne peut pas être vide ");
    }
    // Comparaison alphabétique ISO : YYYY-MM-DD
    // exemple : "2024-06-15" < "2024-06-20" => true
    return datePeremption_ < dateReference;
}