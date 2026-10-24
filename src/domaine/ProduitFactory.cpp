#include "domaine/ProduitFactory.h"
#include "domaine/ProduitPerissable.h"
#include "domaine/ProduitElectronique.h"
#include "domaine/Exceptions.h"

#include <string>

/*Recontruit un produit depuis sa fiche neutre.
Règles : 
1. On vérifie l'existencce d'une eclé avec count() AVANT d'y accéder
    (map["clé"] créerait silencieusement une entrée vide).
2. Toute erreur de FORMAT lève FormaFichierInvalideException (la fiche est mal formée).
les erreurs METIER (date vide, garantie <=0) lèvées
    par les constructeurs des produits remontent telles quelles : elles 
    viennent du domaine, pas du fichier */
    std::unique_ptr<Produit> ProduitFactory::creer(const Enregistrement& e) {
        // --- Champs communs : tous obliatooires ---
        if (!e.champs.count("id") || !e.champs.count("nom") || !e.champs.count("categorie") ||
            !e.champs.count("quantite") || !e.champs.count("seuil_critique") || !e.champs.count("prix_unitaire_francs")) {
            throw FormatFichierInvalideException("Fiche produit mal formée : champs communs manquants (type'" + e.type + "')");
        }

        std::int64_t quantite;
        std::int64_t seuilCritique;
        std::int64_t prixFrancs;
        try {
            quantite = std::stoll(e.champs.at("quantite"));
            seuilCritique = std::stoll(e.champs.at("seuil_critique"));
            prixFrancs = std::stoll(e.champs.at("prix_unitaire_francs"));
        } catch (const std::exception&) {
            // stoll ecoue si ce n'est pas un nombre : c'est un problème de FORMAT du fichier, pas du domaine. On traduit l'erreur technique en erreur métier.
            throw FormatFichierInvalideException("Nombre illissible dans l'enregistrement (type '" + e.type + "')");
        }
        Montant prixUnitaire(prixFrancs);

        // --- Champs spécifiques : selon le type ---

        if (e.type == "perissable") {
            if (!e.champs.count("date_peremption")) {
                throw FormatFichierInvalideException("Fiche produit périssable mal formée : date_peremption manquante");
            }
            return std::make_unique<ProduitPerissable>(e.champs.at("id"), e.champs.at("nom"), e.champs.at("categorie"),
                                                       quantite, seuilCritique, prixUnitaire, e.champs.at("date_peremption"));
        } else if (e.type == "electronique") {
            if (!e.champs.count("duree_garantie_mois")) {
                throw FormatFichierInvalideException("Fiche produit electronique mal formée : duree_garantie_mois manquante");
            }
            std::int64_t garantie;
            try {
                garantie = std::stoll(e.champs.at("duree_garantie_mois"));
            } catch (const std::exception&) {
                throw FormatFichierInvalideException("Nombre illisible pour duree_garantie_mois dans l'enregistrement");
            }
            return std::make_unique<ProduitElectronique>(e.champs.at("id"), e.champs.at("nom"), e.champs.at("categorie"),
                                                         quantite, seuilCritique, prixUnitaire, garantie);
        } else {
            throw FormatFichierInvalideException("Type de produit inconnu : '" + e.type + "'");
        }
    }
