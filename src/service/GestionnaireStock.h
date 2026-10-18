#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include "domaine/Exceptions.h"
#include "domaine/Montant.h"
#include "domaine/Produit.h"
#include "service/IMouvementRepository.h"
#include "service/IProduitRepository.h"

// GestionnaireStock : le "gerant" du stock (couche service).
//
// C'est ici que se trouvent les regles de gestion. Chaque operation suit
// le meme schema : VERIFIER (le produit existe, la demande est valide),
// AGIR (le produit modifie sa propre quantite), TRACER (on ajoute une
// ligne au journal des mouvements).
//
// Cette classe n'affiche rien et n'ouvre aucun fichier : elle passe par
// les contrats IProduitRepository et IMouvementRepository.
class GestionnaireStock {
public:
    // Les repositories sont recus de l'exterieur : on peut en brancher
    // un different (memoire, fichier) sans modifier cette classe.
    GestionnaireStock(IProduitRepository& produitRepo,
                      IMouvementRepository& mouvementRepo);

    // Le gestionnaire devient l'unique proprietaire du produit.
    void ajouterProduit(std::unique_ptr<Produit> produit);

    void ajouterStock(const std::string& id, std::int64_t quantite,
                      const std::string& auteur, const std::string& motif);

    // Garantit qu'un stock ne devient jamais negatif.
    void retirerStock(const std::string& id, std::int64_t quantite,
                      const std::string& auteur, const std::string& motif);

    // Ici on donne la quantite FINALE (inventaire), pas un ecart.
    void ajusterStock(const std::string& id, std::int64_t nouvelleQuantite,
                     const std::string& auteur, const std::string& motif);

    // Renvoie nullptr si le produit n'existe pas : ce n'est pas une erreur.
    [[nodiscard]] const Produit* trouver(const std::string& id) const;

    [[nodiscard]] Montant valeurTotale() const;
     // Tous les produits, en lecture seule, tries par identifiant
    // (pour que l'affichage soit toujours dans le meme ordre).
    [[nodiscard]] std::vector<const Produit*> listerProduits() const;

    // Journal complet des mouvements, du plus ancien au plus recent.
    [[nodiscard]] std::vector<MouvementStock> historique() const;

    void sauvegarder();

private:
    // Retrouve un produit modifiable, ou leve ProduitIntrouvableException.
    Produit& produitOuException(const std::string& id);

    // Cree un MouvementStock horodate et l'ajoute au journal.
    void tracer(const std::string& id, const std::string& type,
                std::int64_t quantite, const std::string& auteur,
                const std::string& motif);

    // Produits en stock, ranges par identifiant pour une recherche rapide.
    std::unordered_map<std::string, std::unique_ptr<Produit>> produits_;

    IProduitRepository& produitRepo_;
    IMouvementRepository& mouvementRepo_;
};