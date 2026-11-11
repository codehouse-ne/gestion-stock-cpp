#include "interface/MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QSpinBox>
#include <QStatusBar>
#include <QFrame>

#include "domaine/ProduitPerissable.h"
#include "domaine/ProduitElectronique.h"
#include "domaine/Exceptions.h"

MainWindow::MainWindow(GestionnaireStock& gestionnaire, QWidget* parent)
    : QMainWindow(parent), gestionnaire_(gestionnaire) {
    setWindowTitle("Gestion de Stock · V1");
    resize(1024, 660);
    setMinimumSize(900, 600);

    initialiserInterface();
    rafraichir();
}

void MainWindow::initialiserInterface() {
    auto* widgetCentral = new QWidget(this);
    setCentralWidget(widgetCentral);

    auto* layoutPrincipal = new QHBoxLayout(widgetCentral);
    layoutPrincipal->setContentsMargins(0, 0, 0, 0);
    layoutPrincipal->setSpacing(0);

    // 1. Barre latérale de navigation (gauche)
    layoutPrincipal->addWidget(creerPanneauNavigation());

    // 2. Zone de contenu principale (droite)
    auto* zoneContenu = new QWidget(this);
    zoneContenu->setStyleSheet("background-color: #f4f6f9;");
    auto* layoutContenu = new QVBoxLayout(zoneContenu);
    layoutContenu->setContentsMargins(28, 20, 28, 20);
    layoutContenu->setSpacing(18);

    // En-tête : Titre + Recherche
    auto* layoutEnTete = new QHBoxLayout();
    auto* layoutTitres = new QVBoxLayout();
    auto* titrePage = new QLabel("Tableau de bord", this);
    titrePage->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");
    auto* sousTitrePage = new QLabel("Vue d'ensemble du stock en temps réel", this);
    sousTitrePage->setStyleSheet("font-size: 11px; color: #6b7280;");
    layoutTitres->addWidget(titrePage);
    layoutTitres->addWidget(sousTitrePage);
    layoutEnTete->addLayout(layoutTitres);

    layoutEnTete->addStretch();

    champRecherche_ = new QLineEdit(this);
    champRecherche_->setPlaceholderText("🔍 Rechercher un produit...");
    champRecherche_->setFixedWidth(240);
    champRecherche_->setStyleSheet(
        "QLineEdit {"
        "  background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 8px;"
        "  padding: 6px 12px; font-size: 12px; color: #1f2937;"
        "}"
        "QLineEdit:focus { border: 1px solid #2563eb; }"
    );
    connect(champRecherche_, &QLineEdit::textChanged, this, &MainWindow::onRechercheTexteChange);
    layoutEnTete->addWidget(champRecherche_);

    layoutContenu->addLayout(layoutEnTete);

    // Cartes de synthèse (Valeur, Quantité, Alertes)
    layoutContenu->addWidget(creerCartesSynthese());

    // Section Tableau des produits
    layoutContenu->addWidget(creerSectionTableau(), 1);

    // Barre des boutons d'actions
    layoutContenu->addWidget(creerBarreActions());

    layoutPrincipal->addWidget(zoneContenu, 1);

    // Barre d'état en bas
    statusBar()->setStyleSheet("QStatusBar { background-color: #eceff4; color: #6b7280; font-size: 11px; }");
    statusBar()->showMessage("Prêt  ·  V1.0  ·  Qt 6.10.2");
}

QWidget* MainWindow::creerPanneauNavigation() {
    auto* nav = new QWidget(this);
    nav->setFixedWidth(210);
    nav->setStyleSheet("background-color: #232a38;");

    auto* layout = new QVBoxLayout(nav);
    layout->setContentsMargins(16, 20, 16, 20);
    layout->setSpacing(8);

    // Logo et titre
    auto* logoLabel = new QLabel("📦 Stock Manager", nav);
    logoLabel->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: 700; padding-bottom: 16px;");
    layout->addWidget(logoLabel);

    // Boutons de navigation
    auto creerBoutonNav = [nav](const QString& texte, bool actif) {
        auto* btn = new QPushButton(texte, nav);
        btn->setCursor(Qt::PointingHandCursor);
        if (actif) {
            btn->setStyleSheet(
                "QPushButton {"
                "  background-color: #2f3b54; color: #ffffff; font-weight: 600;"
                "  text-align: left; padding: 10px 14px; border-radius: 8px; border-left: 3px solid #2563eb;"
                "}"
            );
        } else {
            btn->setStyleSheet(
                "QPushButton {"
                "  background-color: transparent; color: #9aa5b5;"
                "  text-align: left; padding: 10px 14px; border-radius: 8px; border: none;"
                "}"
                "QPushButton:hover { background-color: #2a3344; color: #ffffff; }"
            );
        }
        return btn;
    };

    layout->addWidget(creerBoutonNav("📊  Tableau de bord", true));
    layout->addWidget(creerBoutonNav("📦  Produits", false));
    layout->addWidget(creerBoutonNav("🔄  Mouvements", false));
    layout->addWidget(creerBoutonNav("💾  Sauvegarde", false));

    layout->addStretch();

    // Profil utilisateur
    auto* profilBox = new QFrame(nav);
    profilBox->setStyleSheet("background-color: #1a202c; border-radius: 8px; padding: 8px;");
    auto* profilLayout = new QHBoxLayout(profilBox);
    profilLayout->setContentsMargins(8, 6, 8, 6);

    auto* avatar = new QLabel("ML", profilBox);
    avatar->setFixedSize(26, 26);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet("background-color: #475569; color: #ffffff; border-radius: 13px; font-weight: 700; font-size: 10px;");

    auto* infosLayout = new QVBoxLayout();
    infosLayout->setSpacing(1);
    auto* nomUser = new QLabel("Maman Laouali", profilBox);
    nomUser->setStyleSheet("color: #e5e9f0; font-size: 11px; font-weight: 600;");
    auto* roleUser = new QLabel("Administrateur", profilBox);
    roleUser->setStyleSheet("color: #8b94a6; font-size: 9px;");
    infosLayout->addWidget(nomUser);
    infosLayout->addWidget(roleUser);

    profilLayout->addWidget(avatar);
    profilLayout->addLayout(infosLayout);

    layout->addWidget(profilBox);

    return nav;
}

QWidget* MainWindow::creerCartesSynthese() {
    auto* conteneur = new QWidget(this);
    auto* layout = new QHBoxLayout(conteneur);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);

    auto creerCarte = [conteneur](const QString& titre, QLabel*& etiquetteValeur, const QString& couleurAccent) {
        auto* carte = new QFrame(conteneur);
        carte->setStyleSheet(
            "QFrame {"
            "  background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 10px; padding: 14px;"
            "}"
        );
        auto* carteLayout = new QVBoxLayout(carte);
        carteLayout->setContentsMargins(12, 10, 12, 10);
        carteLayout->setSpacing(4);

        auto* titreLabel = new QLabel(titre, carte);
        titreLabel->setStyleSheet("color: #6b7280; font-size: 10px; font-weight: 600; letter-spacing: 0.5px;");
        carteLayout->addWidget(titreLabel);

        etiquetteValeur = new QLabel("—", carte);
        etiquetteValeur->setStyleSheet(QString("color: %1; font-size: 18px; font-weight: 700;").arg(couleurAccent));
        carteLayout->addWidget(etiquetteValeur);

        return carte;
    };

    layout->addWidget(creerCarte("VALEUR TOTALE", etiquetteValeurTotale_, "#2563eb"));
    layout->addWidget(creerCarte("PRODUITS EN STOCK", etiquetteNombreProduits_, "#16a34a"));
    layout->addWidget(creerCarte("ALERTES DE SEUIL", etiquetteAlertesSeuil_, "#dc2626"));

    return conteneur;
}

QWidget* MainWindow::creerSectionTableau() {
    auto* conteneur = new QFrame(this);
    conteneur->setStyleSheet(
        "QFrame {"
        "  background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 10px;"
        "}"
    );
    auto* layout = new QVBoxLayout(conteneur);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(10);

    // En-tête de section
    auto* layoutHaut = new QHBoxLayout();
    auto* titreSection = new QLabel("Produits", conteneur);
    titreSection->setStyleSheet("font-size: 14px; font-weight: 700; color: #1f2937; border: none;");
    etiquetteResumeTableau_ = new QLabel("", conteneur);
    etiquetteResumeTableau_->setStyleSheet("font-size: 11px; color: #6b7280; border: none;");
    layoutHaut->addWidget(titreSection);
    layoutHaut->addStretch();
    layoutHaut->addWidget(etiquetteResumeTableau_);
    layout->addLayout(layoutHaut);

    // Tableau
    tableauProduits_ = new QTableWidget(conteneur);
    tableauProduits_->setColumnCount(7);
    tableauProduits_->setHorizontalHeaderLabels({"ID", "PRODUIT", "CATÉGORIE", "QTÉ", "PRIX UNITAIRE", "VALEUR", "ÉTAT"});
    tableauProduits_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableauProduits_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableauProduits_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tableauProduits_->verticalHeader()->setVisible(false);
    tableauProduits_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableauProduits_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableauProduits_->setStyleSheet(
        "QTableWidget {"
        "  border: none; gridline-color: #f1f5f9; background-color: #ffffff; font-size: 12px; color: #1f2937;"
        "}"
        "QHeaderView::section {"
        "  background-color: #f8fafc; color: #6b7280; font-size: 10px; font-weight: 600;"
        "  padding: 8px; border: none; border-bottom: 1px solid #e5e7eb;"
        "}"
        "QTableWidget::item { padding: 6px; }"
    );
    layout->addWidget(tableauProduits_);

    // Total en bas du tableau
    auto* layoutPied = new QHBoxLayout();
    auto* labelTotal = new QLabel("TOTAL", conteneur);
    labelTotal->setStyleSheet("font-weight: 700; font-size: 11px; color: #374151; border: none;");
    etiquetteTotalPied_ = new QLabel("0 FCFA", conteneur);
    etiquetteTotalPied_->setStyleSheet("font-weight: 700; font-size: 12px; color: #1f2937; border: none;");
    layoutPied->addWidget(labelTotal);
    layoutPied->addStretch();
    layoutPied->addWidget(etiquetteTotalPied_);
    layout->addLayout(layoutPied);

    return conteneur;
}

QWidget* MainWindow::creerBarreActions() {
    auto* conteneur = new QWidget(this);
    auto* layout = new QHBoxLayout(conteneur);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    auto* btnNouveau = new QPushButton("➕  Nouveau produit", conteneur);
    btnNouveau->setCursor(Qt::PointingHandCursor);
    btnNouveau->setStyleSheet(
        "QPushButton {"
        "  background-color: #2563eb; color: #ffffff; font-weight: 600; font-size: 12px;"
        "  padding: 10px 18px; border-radius: 8px; border: none;"
        "}"
        "QPushButton:hover { background-color: #1d4ed8; }"
    );
    connect(btnNouveau, &QPushButton::clicked, this, &MainWindow::onNouveauProduitClique);
    layout->addWidget(btnNouveau);

    auto* btnRetirer = new QPushButton("➖  Retirer du stock", conteneur);
    btnRetirer->setCursor(Qt::PointingHandCursor);
    btnRetirer->setStyleSheet(
        "QPushButton {"
        "  background-color: #ffffff; color: #374151; font-weight: 600; font-size: 12px;"
        "  padding: 10px 18px; border-radius: 8px; border: 1px solid #d1d5db;"
        "}"
        "QPushButton:hover { background-color: #f9fafb; }"
    );
    connect(btnRetirer, &QPushButton::clicked, this, &MainWindow::onRetirerStockClique);
    layout->addWidget(btnRetirer);

    auto* btnSauvegarder = new QPushButton("💾  Sauvegarder", conteneur);
    btnSauvegarder->setCursor(Qt::PointingHandCursor);
    btnSauvegarder->setStyleSheet(
        "QPushButton {"
        "  background-color: #ffffff; color: #374151; font-weight: 600; font-size: 12px;"
        "  padding: 10px 18px; border-radius: 8px; border: 1px solid #d1d5db;"
        "}"
        "QPushButton:hover { background-color: #f9fafb; }"
    );
    connect(btnSauvegarder, &QPushButton::clicked, this, &MainWindow::onSauvegarderClique);
    layout->addWidget(btnSauvegarder);

    layout->addStretch();
    return conteneur;
}

void MainWindow::rafraichir() {
    const auto produits = gestionnaire_.listerProduits();
    const QString filtre = champRecherche_ ? champRecherche_->text().trimmed().toLower() : "";

    std::int64_t alertesCount = 0;
    tableauProduits_->setRowCount(0);

    int ligneVisuelle = 0;
    for (const auto* p : produits) {
        if (!p) continue;

        const bool alerte = p->sousSeuilCritique();
        if (alerte) {
            alertesCount++;
        }

        const QString nom = QString::fromStdString(p->nom());
        const QString id = QString::fromStdString(p->id());
        if (!filtre.isEmpty() && !nom.toLower().contains(filtre) && !id.toLower().contains(filtre)) {
            continue; // Filtré par la recherche
        }

        tableauProduits_->insertRow(ligneVisuelle);

        auto* itemID = new QTableWidgetItem(id);
        itemID->setTextAlignment(Qt::AlignCenter);
        auto* itemNom = new QTableWidgetItem(nom);
        auto* itemCat = new QTableWidgetItem("Général");
        auto* itemQte = new QTableWidgetItem(QString::number(p->quantite()));
        itemQte->setTextAlignment(Qt::AlignCenter);
        auto* itemPrix = new QTableWidgetItem(QString::fromStdString(p->prixUnitaire().versTexte()));
        auto* itemVal = new QTableWidgetItem(QString::fromStdString(p->valeurStock().versTexte()));

        auto* itemEtat = new QTableWidgetItem(alerte ? "Sous le seuil" : "En stock");
        itemEtat->setTextAlignment(Qt::AlignCenter);

        if (alerte) {
            QColor rougeAlerte("#b91c1c");
            QColor fondAlerte("#fef2f2");
            for (auto* item : {itemID, itemNom, itemCat, itemQte, itemPrix, itemVal, itemEtat}) {
                item->setBackground(fondAlerte);
            }
            itemQte->setForeground(rougeAlerte);
            itemEtat->setForeground(rougeAlerte);
        } else {
            itemEtat->setForeground(QColor("#15803d"));
        }

        tableauProduits_->setItem(ligneVisuelle, 0, itemID);
        tableauProduits_->setItem(ligneVisuelle, 1, itemNom);
        tableauProduits_->setItem(ligneVisuelle, 2, itemCat);
        tableauProduits_->setItem(ligneVisuelle, 3, itemQte);
        tableauProduits_->setItem(ligneVisuelle, 4, itemPrix);
        tableauProduits_->setItem(ligneVisuelle, 5, itemVal);
        tableauProduits_->setItem(ligneVisuelle, 6, itemEtat);

        ligneVisuelle++;
    }

    // Mise à jour des cartes et étiquettes
    const QString valeurTotaleTexte = QString::fromStdString(gestionnaire_.valeurTotale().versTexte());
    etiquetteValeurTotale_->setText(valeurTotaleTexte);
    etiquetteNombreProduits_->setText(QString::number(produits.size()));
    etiquetteAlertesSeuil_->setText(QString::number(alertesCount));
    etiquetteTotalPied_->setText(valeurTotaleTexte);
    etiquetteResumeTableau_->setText(QString("%1 produits · %2 alerte(s)").arg(produits.size()).arg(alertesCount));
}

void MainWindow::onRechercheTexteChange(const QString&) {
    rafraichir();
}

void MainWindow::onNouveauProduitClique() {
    QDialog dialog(this);
    dialog.setWindowTitle("Nouveau produit");
    dialog.resize(360, 280);

    auto* layout = new QFormLayout(&dialog);

    auto* idEdit = new QLineEdit(&dialog);
    auto* nomEdit = new QLineEdit(&dialog);
    auto* catEdit = new QLineEdit(&dialog);
    catEdit->setText("Alimentaire");
    auto* qteEdit = new QSpinBox(&dialog);
    qteEdit->setRange(0, 100000);
    auto* seuilEdit = new QSpinBox(&dialog);
    seuilEdit->setRange(0, 100000);
    seuilEdit->setValue(5);
    auto* prixEdit = new QSpinBox(&dialog);
    prixEdit->setRange(0, 100000000);
    prixEdit->setSuffix(" FCFA");
    prixEdit->setValue(500);

    auto* typeCombo = new QComboBox(&dialog);
    typeCombo->addItems({"Périssable", "Électronique"});
    auto* extraEdit = new QLineEdit(&dialog);
    extraEdit->setPlaceholderText("YYYY-MM-DD (Date péremption)");

    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [extraEdit](int index) {
        if (index == 0) {
            extraEdit->setPlaceholderText("YYYY-MM-DD (Date péremption)");
            extraEdit->setText("");
        } else {
            extraEdit->setPlaceholderText("Mois de garantie (ex: 12)");
            extraEdit->setText("12");
        }
    });

    layout->addRow("Identifiant :", idEdit);
    layout->addRow("Nom :", nomEdit);
    layout->addRow("Catégorie :", catEdit);
    layout->addRow("Quantité initiale :", qteEdit);
    layout->addRow("Seuil critique :", seuilEdit);
    layout->addRow("Prix unitaire :", prixEdit);
    layout->addRow("Type de produit :", typeCombo);
    layout->addRow("Spécificité :", extraEdit);

    auto* boutons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(boutons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(boutons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(boutons);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            Montant prix(prixEdit->value());
            std::unique_ptr<Produit> nouveau;

            if (typeCombo->currentIndex() == 0) {
                // Périssable
                std::string dateP = extraEdit->text().toStdString();
                if (dateP.empty()) dateP = "2026-12-31";
                nouveau = std::make_unique<ProduitPerissable>(
                    idEdit->text().toStdString(), nomEdit->text().toStdString(), catEdit->text().toStdString(),
                    qteEdit->value(), seuilEdit->value(), prix, dateP
                );
            } else {
                // Électronique
                std::int64_t garantie = extraEdit->text().toLongLong();
                if (garantie <= 0) garantie = 12;
                nouveau = std::make_unique<ProduitElectronique>(
                    idEdit->text().toStdString(), nomEdit->text().toStdString(), catEdit->text().toStdString(),
                    qteEdit->value(), seuilEdit->value(), prix, garantie
                );
            }

            gestionnaire_.ajouterProduit(std::move(nouveau));
            rafraichir();
            statusBar()->showMessage("Produit ajouté avec succès !", 3000);
        } catch (const StockException& e) {
            QMessageBox::warning(this, "Erreur lors de l'ajout", e.what());
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Erreur inattendue", e.what());
        }
    }
}

void MainWindow::onRetirerStockClique() {
    QDialog dialog(this);
    dialog.setWindowTitle("Retirer du stock");
    dialog.resize(320, 200);

    auto* layout = new QFormLayout(&dialog);
    auto* idCombo = new QComboBox(&dialog);
    for (const auto* p : gestionnaire_.listerProduits()) {
        idCombo->addItem(QString::fromStdString(p->id() + " - " + p->nom()), QString::fromStdString(p->id()));
    }

    auto* qteEdit = new QSpinBox(&dialog);
    qteEdit->setRange(1, 10000);
    qteEdit->setValue(1);

    auto* auteurEdit = new QLineEdit("Maman Laouali", &dialog);
    auto* motifEdit = new QLineEdit("Vente client", &dialog);

    layout->addRow("Produit :", idCombo);
    layout->addRow("Quantité à retirer :", qteEdit);
    layout->addRow("Auteur :", auteurEdit);
    layout->addRow("Motif :", motifEdit);

    auto* boutons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(boutons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(boutons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(boutons);

    if (dialog.exec() == QDialog::Accepted) {
        const std::string id = idCombo->currentData().toString().toStdString();
        try {
            gestionnaire_.retirerStock(id, qteEdit->value(), auteurEdit->text().toStdString(), motifEdit->text().toStdString());
            rafraichir();
            statusBar()->showMessage("Stock mis à jour avec succès !", 3000);
        } catch (const StockException& e) {
            QMessageBox::warning(this, "Opération refusée", e.what());
        }
    }
}

void MainWindow::onSauvegarderClique() {
    try {
        gestionnaire_.sauvegarder();
        statusBar()->showMessage("Données sauvegardées avec succès dans le repository !", 4000);
        QMessageBox::information(this, "Sauvegarde", "Le stock a été sauvegardé avec succès.");
    } catch (const StockException& e) {
        QMessageBox::warning(this, "Erreur de sauvegarde", e.what());
    }
}
