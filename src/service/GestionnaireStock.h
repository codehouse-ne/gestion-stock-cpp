#pragma once;
#include "string"
#include "memory"
#include "unordered_map"
#include "domaine/Produit.h"
#include "domaine/Exceptions.h" 
#include "service/IMouvementRepository.h"
#include "service/IProduitRepository.h"

// ============================================================================
// GestionnaireStock : le "gerant" du magasin (couche SERVICE).
//
// Role : appliquer les regles de gestion du stock. C'est le seul endroit ou
// l'on decide si une operation est permise (le produit existe-t-il ? y a-t-il
// assez de stock ?), ou l'on demande au produit de se modifier, et ou l'on
// garde une trace de l'operation dans le journal.

class gestionnairestock
{
    private:
std::unordered_map<std::string, std::unique_ptr<Produit>> produits_;
IProduitRepository& produitRepo_;
IMouvementRepository& mouvementRepo;

public:
gestionnairestock(IProduitRepository& produitRepo,IMouvementRepository& mouvementRepo);
void ajouterProduit(std::unique_ptr <Produit> produit);
void ajouterStock(const std::string& id,std::int64_t quantite,const std::string& auteur,const std::string& motif);
void supprimerStock(const std::string& id,const std::string& auteur,const std:: string& motif,std::int64_t quantite );
void ajusterStock(const std::string& id, const std::string& auteur, const std::string& motif,std::int64_t nouvellequantite);

[[nodiscard]] const Produit* Trouver(std::string& id)const;
[[nodiscard]] Montant valeurTotale()const;

void SauvegarderTout();
void ChargerTout();
};