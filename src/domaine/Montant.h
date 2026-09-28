#pragma once
#include <cstdint>
#include <string>
#include "domaine/Exceptions.h"

// Valeur monetaire en FCFA, stockee dans un entier 64 bits.
// Pourquoi pas double ? 0.1 + 0.2 != 0.3 en virgule flottante :
// les arrondis rendraient les totaux du stock faux.
class Montant {
public:
    explicit Montant(std::int64_t francs)
        : francs_(francs) {
        if (francs < 0) {
            throw QuantiteInvalideException("Un montant ne peut pas etre negatif");
        }
    }

    std::int64_t valeur() const noexcept { return francs_; }

    Montant operator+(const Montant& autre) const noexcept {
        return Montant(francs_ + autre.francs_); // addition d'entiers : exacte
    }

    Montant operator*(int facteur) const {
        if (facteur < 0) {
            throw QuantiteInvalideException("Facteur negatif interdit");
        }
        return Montant(francs_ * facteur);
    }

    std::string versTexte() const {
        return std::to_string(francs_) + " FCFA";
    }

private:
    std::int64_t francs_;
};
