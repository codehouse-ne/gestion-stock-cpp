#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

#include "service/GestionnaireStock.h"

// Fenêtre principale de l'application de Gestion de Stock (Qt6).
// Conçue pour refléter fidèlement la maquette validée par le trinôme.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(GestionnaireStock& gestionnaire, QWidget* parent = nullptr);
    ~MainWindow() override = default;

    // Met à jour toutes les informations affichées (cartes KPI + tableau)
    void rafraichir();

private slots:
    void onNouveauProduitClique();
    void onRetirerStockClique();
    void onSauvegarderClique();
    void onRechercheTexteChange(const QString& texte);

private:
    void initialiserInterface();
    QWidget* creerPanneauNavigation();
    QWidget* creerCartesSynthese();
    QWidget* creerSectionTableau();
    QWidget* creerBarreActions();

    GestionnaireStock& gestionnaire_;

    // Widgets mis à jour dynamiquement
    QLabel* etiquetteValeurTotale_{nullptr};
    QLabel* etiquetteNombreProduits_{nullptr};
    QLabel* etiquetteAlertesSeuil_{nullptr};
    QLabel* etiquetteResumeTableau_{nullptr};
    QLabel* etiquetteTotalPied_{nullptr};
    QTableWidget* tableauProduits_{nullptr};
    QLineEdit* champRecherche_{nullptr};
};
