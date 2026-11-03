#include "persistance/InMemoryRepository.h"


std::vector<Enregistrement> InMemoryRepository::chargerTout(){
    return produits_;
}
void InMemoryRepository::sauvegarderTout(const std::vector<Enregistrement>& enregistrements){
     produits_ = enregistrements;
}

void InMemoryRepository::ajouter(const MouvementStock& mouvement){
    mouvements_.push_back(mouvement);
}
std::vector<MouvementStock> InMemoryRepository:: lister(){
    return mouvements_;
}