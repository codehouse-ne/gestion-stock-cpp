# Gestion de Stock — C++

Application console de gestion de stock développée en **C++17**, avec architecture en couches, tests automatisés (Catch2) et persistance fichier. Projet de groupe (3 personnes) mené par phases successives, du squelette initial jusqu'à la migration SQLite.

---

##  Objectifs du projet

- Gérer un catalogue de **produits** de types différents (périssables, électroniques) via une hiérarchie de classes
- Suivre les **quantités** en stock avec des règles métier strictes (jamais de stock négatif)
- Calculer des **montants exacts** (entiers 64 bits, pas de `double` — l'argent en virgule flottante est inexact)
- **Persister** les données dans un fichier texte, avec un format pivot neutre (`Enregistrement`)
- Garantir la qualité par des **tests automatisés** et une couverture de tous les contrats métier

##  Fonctionnalités par version


| Version | Contenu                                                                                     | Statut   |
| ------- | ------------------------------------------------------------------------------------------- | -------- |
| **V1**  | Produits (hiérarchie), montants, gestionnaire de stock, sauvegarde fichier texte            | en cours |
| **V2**  | Mouvements de stock, journal en « ajout seul », recherches et filtres, menu console complet | à venir  |
| **V3**  | Migration SQLite, nouveau type de produit (textile) et extensions                           | à venir  |


## 🏛️ Architecture

Le projet est organisé en couches. Chaque couche ne communique qu'avec les couches inférieures :

```
┌─────────────────────────────────┐
│  Interface (menu console)       │  → affiche, saisit, présente les erreurs
├─────────────────────────────────┤
│  Service (GestionnaireStock)    │  → orchestre les cas d'usage du métier
├──────────────────────┬──────────┴──┐
│  Domaine             │ Persistance │
│  (Produit, Montant,  │ (repository │
│  Enregistrement,     │ fichier     │
│  Exceptions)         │ texte)      │
└──────────────────────┴─────────────┘
```

**Règles de conception appliquées :**

- Le **domaine ne connaît ni la console ni les fichiers** — il est pur, testable, réutilisable
- Le service dépend d'**interfaces** (`IProduitRepository`, `IMouvementRepository`), jamais d'implémentations concrètes (principe de *dependency inversion*)
- La persistance manipule des **fiches neutres** (`Enregistrement`) : elle ne connaît **aucune** classe de produit. Ajouter un nouveau type de produit ne modifie **aucun** fichier de persistance (principe *ouvert/fermé*)
- Les erreurs métier forment une **hiérarchie d'exceptions** à base commune (`StockException`) : un seul `catch` les présente toutes à l'utilisateur

## 📁 Structure du dépôt

```
gestion-stock-cpp/
├── CMakeLists.txt              # Configuration de compilation (CMake ≥ 3.16)
├── src/
│   ├── domaine/                # Cœur métier : règles et objets
│   │   ├── Produit.h/.cpp      # Classe abstraite de base + logique commune
│   │   ├── Montant.h           # Montant entier validé (jamais négatif)
│   │   ├── Enregistrement.h    # Format pivot neutre pour la persistance
│   │   └── Exceptions.h        # Hiérarchie des erreurs métier
│   ├── service/                # Cas d'usage et interfaces de persistance
│   │   ├── GestionnaireStock   # Orchestration du métier
│   │   ├── IProduitRepository.h
│   │   └── IMouvementRepository.h
│   ├── persistance/            # Implémentations fichiers
│   └── interface/              # Menu console
├── tests/                      # Tests Catch2
└── README.md
```

## 🛠️ Pile technique


| Outil                     | Usage                                                                 |
| ------------------------- | --------------------------------------------------------------------- |
| **C++17**                 | langage (`std::optional`, `std::string_view`, …)                      |
| **CMake ≥ 3.16**          | build multi-plateforme                                                |
| **Catch2 v3.4.0**         | tests automatisés (téléchargée et épinglée automatiquement par CMake) |
| **GCC ≥ 11 / Clang ≥ 13** | compilateurs supportés                                                |
| **Git + GitHub**          | gestion de versions, pull requests et relectures croisées             |


## ⚙️ Compiler et exécuter

```bash
# 1. Configuration (télécharge Catch2 au premier lancement)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# 2. Compilation
cmake --build build

# 3. Exécution
./build/gestion_stock

# 4. Tests
ctest --test-dir build --output-on-failure
```

**Compilation sans les tests** (utile hors-ligne — Catch2 ne peut pas être téléchargée) :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DSTOCK_BUILD_TESTS=OFF
cmake --build build
```

**Prérequis versions :** Git ≥ 2.25, g++ ≥ 11, CMake ≥ 3.16.

## ✅ Qualité et règles de l'équipe

- **Zéro warning** : le build active `-Wall -Wextra -Wpedantic -Wshadow`. Un commit qui introduit un warning n'est pas terminé.
- **Un commit = compilation verte** : les tests passent avant chaque `git push`.
- **Jamais de commit direct sur `main`** : chaque travail vit dans sa branche (`feature/...`), fusionné après relecture par pull request.

## 🤝 Équipe et répartition


| Membre                                                  | Responsabilité                                                            |
| ------------------------------------------------------- | ------------------------------------------------------------------------- |
| **Maman Laouali** ([MLMD93](https://github.com/MLMD93)) | Domaine : classe `Produit`, produits concrets, usine (factory)            |
| **Hadiza(mohammedabdoulmoumounihadiza-maker)**          | Service &amp; persistance : `GestionnaireStock`, repository fichier texte |
| **Hassane(Sass2345)**                                   | Interface &amp; qualité : menu console, tests, relecture des PR           |


## 🗺️ Roadmap

- [x] Phase 0 — Squelette opérationnel : CMake, Catch2, interfaces, premiers tests
- [ ] V1 — Domaine complet (produits concrets, usine) + service de base
- [ ] V1 — Persistance fichier texte + menu console
- [ ] V2 — Mouvements, journal en ajout seul, recherches
- [ ] V2 — Menu complet, filtres
- [ ] V3 — Migration SQLite
- [ ] V3 — Nouveau type de produit (textile)

---

*Projet de groupe — dépôt : `codehouse-ne/gestion-stock-cpp`.*
