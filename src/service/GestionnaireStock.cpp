#include "service/GestionnaireStock.h"
#include <ctime>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <vector>

namespace{
// Heure actuelle au format ISO 8601, ex. 2026-10-15T14:30:00
std::string horodatageActuel(){
    std::time_t maintenant = std::time (nullptr);
    std::tm local = *std::localtime(&maintenant);
      char tampon[32];
    std::strftime(tampon, sizeof(tampon), "%Y-%m-%dT%H:%M:%S", &local);
    return tampon;
}

}  // namespace

GestionnaireStock::GestionnaireStock(IProduitRepository& produitRepo,
  IMouvementRepository& mouvementRepo)
   : produitRepo_(produitRepo),mouvementRepo_(mouvementRepo){}
   Produit& GestionnaireStock::produitOuException(const std::string& id){
        auto it = produits_.find(id);
        if(it == produits_.end()){
        throw ProduitIntrouvableException("Produit introuvable" +id);
        }
        return(*it->second);
   }
   void GestionnaireStock::tracer(const std::string& id, const std::string& type,
                std::int64_t quantite, const std::string& auteur,
                const std::string& motif){
     MouvementStock mouvement{id,type, quantite,horodatageActuel(),auteur,motif};
     mouvementRepo_.ajouter(mouvement);
   }

   void GestionnaireStock::ajouterProduit(std::unique_ptr<Produit> produit){
    if(!produit){
      throw std::invalid_argument("produit nul fourni à ajouterproduit");
    }
    const std::string id = produit->id();
    if(produits_.find(id) != produits_.end()){
      throw ProduitDejaExistantException("produit deja existant:"+id);
    }
    produits_[id]=std::move(produit);
   }
   void GestionnaireStock::ajouterStock(const std::string& id, std::int64_t quantite,
    const std::string& auteur, const std::string& motif){
    if(quantite < 0){
      throw QuantiteInvalideException("quantite negative refusee pour"+id);
    }
    Produit& produit = produitOuException(id);
    produit.ajouterQuantite(quantite);
    tracer(id,"entree",quantite,auteur,motif);
 }
 void GestionnaireStock::retirerStock(const std::string& id, std::int64_t quantite,
                      const std::string& auteur, const std::string& motif){
if(quantite < 0){
  throw QuantiteInvalideException("quantite refusee pour"+id);
}
Produit& produit = produitOuException(id);
// On verifie AVANT de modifier, pour ne jamais laisser le stock a moitie change.
if(quantite > produit.quantite()){
  throw StockInsuffisantException("stock insuffisant pour"+id+ "demande=" + 
    std::to_string(quantite) + ",disponible=" +std::to_string (produit.quantite()));
}
produit.retirerQuantite(quantite);
tracer(id,"sortie",quantite,motif,auteur);
 }

void GestionnaireStock::ajusterStock(const std::string& id, std::int64_t nouvelleQuantite,
   const std::string& auteur, const std::string& motif){   
    if(nouvelleQuantite<0){
      throw QuantiteInvalideException("quantite refusee pour" +id);
    }
    Produit& produit = produitOuException(id);
    // Produit n'a que ajouter/retirer : on applique l'ecart avec l'ancienne quantite.
    const std::int64_t ecart = nouvelleQuantite - produit.quantite();
    if (ecart > 0) {
        produit.ajouterQuantite(ecart);
    } else if (ecart < 0) {
        produit.retirerQuantite(-ecart);
    }
    // Le journal garde l'ecart signe (ex. -3 = trois unites perdues).
    tracer(id, "ajustement", ecart, auteur, motif);
}

const Produit* GestionnaireStock::trouver(const std::string& id) const {
    auto it = produits_.find(id);
    if (it == produits_.end()) {
        return nullptr;  // pas trouve : resultat normal, pas d'exception
    }
    return it->second.get();
}
std::vector<const Produit*> GestionnaireStock::listerProduits() const {
    std::vector<const Produit*> resultat;
    for (const auto& entree : produits_) {
        resultat.push_back(entree.second.get());
    }
    // unordered_map n'a pas d'ordre : on trie pour un affichage stable.
    std::sort(resultat.begin(), resultat.end(),
              [](const Produit* a, const Produit* b) { return a->id() < b->id(); });
    return resultat;
}

std::vector<MouvementStock> GestionnaireStock::historique() const {
    return mouvementRepo_.lister();
}

Montant GestionnaireStock::valeurTotale() const {
    Montant total(0);
    for (const auto& entree : produits_) {
        total = total + entree.second->valeurStock();
    }
    return total;
}

void GestionnaireStock::sauvegarder(){
  std::vector<Enregistrement> enregistrements;
  for(const auto& entree:produits_){
    enregistrements.push_back(entree.second -> versEnregistrement());
  }
  produitRepo_.sauvegarderTout(enregistrements);
}

 