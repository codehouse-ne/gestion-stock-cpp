#include "domaine/ProduitElectronique.h"
#include "domaine/Exceptions.h"

// 1. Le constructeur : meme pattern que ProduitPerissable
// (validation sur le champ membre, APRES le std::move)
ProduitElectronique::ProduitElectronique(std::string id, std::string nom, std::string categorie,
                                         std::int64_t quantite, std::int64_t seuilCritique,
                                         Montant prixUnitaire, std::int64_t dureeGarantieMois)
    : Produit(std::move(id), std::move(nom), std::move(categorie),
              quantite, seuilCritique, std::move(prixUnitaire)),
      dureeGarantieMois_(dureeGarantieMois) {
    // Regle metier : une garantie nulle ou negative n'a pas de sens
    if (dureeGarantieMois_ <= 0) {
        throw ArgumentInvalideException("La duree de garantie doit etre strictement positive");
    }
}

// 2. Serialisation pour la persistance
Enregistrement ProduitElectronique::versEnregistrement() const {
    Enregistrement e;
    e.type = "electronique";

    // On reutilise la methode du parent pour les champs communs
    remplirChampsCommuns(e);

    // Champ propre a cette classe (en snake_case)
    e.champs["duree_garantie_mois"] = std::to_string(dureeGarantieMois_);
    return e;
}

// 3. Description textuelle
std::string ProduitElectronique::description() const {
    return nom() + " (electronique, garantie : " + std::to_string(dureeGarantieMois_) + " mois)";
}

// 4. Sous garantie ? Une garantie n'est pas une date stockee : elle se CALCULE
// a partir de la date d'achat. V1 simple : les dates ISO sont converties en
// "mois absolus" (annee * 12 + mois) pour comparer.
bool ProduitElectronique::sousGarantie(const std::string& dateAchat,
                                        const std::string& dateReference) const {
    if (dateAchat.size() != 10 || dateReference.size() != 10) {
        throw ArgumentInvalideException("Les dates doivent etre au format ISO YYYY-MM-DD");
    }
    try {
        // Convertit "2026-04-15" en mois absolus : 2026*12 + 4
        auto moisAbsolus = [](const std::string& d) -> std::int64_t {
            return std::stoll(d.substr(0, 4)) * 12 + std::stoll(d.substr(5, 2));
        };
        // Garantie strictement inferieure a la duree : coherent avec la
        // comparaison stricte d'estPerimee (le mois anniversaire n'est pas couvert).
        return moisAbsolus(dateReference) - moisAbsolus(dateAchat) < dureeGarantieMois_;
    } catch (const std::exception&) {
        // stoll echoue si l'annee ou le mois ne sont pas des nombres (ex: "15/01/2026")
        // ou contiennent des lettres (ex: "2026-AB-15") : on traduit l'erreur
        // technique en erreur metier, comme partout ailleurs dans le domaine.
        // Regle : tout ce qui sort du domaine herite de StockException.
        throw ArgumentInvalideException("Les dates doivent etre au format ISO YYYY-MM-DD");
    }
}