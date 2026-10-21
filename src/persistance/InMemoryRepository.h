#pragma once
#include <vector>
#include "service/IMouvementRepository.h"
#include "service/IProduitRepository.h"

// InMemoryRepository : stockage en memoire vive (rien n'est ecrit sur le disque).
//
// Cette classe respecte les DEUX contrats du service : elle sait conserver
// les produits (sous forme d'Enregistrement) et le journal des mouvements.
// Tout disparait a la fermeture du programme : elle sert aux tests et au
// developpement. La sauvegarde sur disque sera une autre classe qui
// respectera les memes contrats, sans rien changer a GestionnaireStock.

class InMemoryRepository : public IMouvementRepository,public IProduitRepository{
    private:
    std::vector<Enregistrement> produits_;
    std::vector<MouvementStock> mouvements_;
    public:
    //------IproduitRepository------
    std::vector<Enregistrement> chargerTout() override;
    void sauvegarderTout( const std::vector<Enregistrement>& enregistrements) override;

    //------IMouvementRepository------
    //Ajouter à la fin du journal sans jamais modifier ni effacer l'existant
    void ajouter(const MouvementStock& mouvement) override;
    std::vector<MouvementStock> lister() override;

};
