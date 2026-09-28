#pragma once
#include <stdexcept>
#include <string>

// Classe de base : toute erreur du domaine derive de StockException.
// Pourquoi une base commune ? Pour attraper "toute erreur du stock"
// avec un seul catch (const StockException&) dans le menu.
struct StockException : std::runtime_error {
    explicit StockException(const std::string& message)
        : std::runtime_error(message) {}
};

struct StockInsuffisantException : StockException {
    explicit StockInsuffisantException(const std::string& message)
        : StockException(message) {}
};

struct QuantiteInvalideException : StockException {
    explicit QuantiteInvalideException(const std::string& message)
        : StockException(message) {}
};

struct ProduitDejaExistantException : StockException {
    explicit ProduitDejaExistantException(const std::string& message)
        : StockException(message) {}
};

struct ProduitIntrouvableException : StockException {
    explicit ProduitIntrouvableException(const std::string& message)
        : StockException(message) {}
};

struct FormatFichierInvalideException : StockException {
    explicit FormatFichierInvalideException(const std::string& message)
        : StockException(message) {}
};

struct ErreurPersistanceException : StockException {
    explicit ErreurPersistanceException(const std::string& message)
        : StockException(message) {}
};
