cmake_minimum_required(VERSION 3.16)
project(gestion_stock LANGUAGES CXX)

# --- Norme C++17 imposée ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Support Qt6 : MOC, UIC, RCC automatiques ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

# --- Compilation stricte : les warnings aident à ne pas écrire du code douteux ---
add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)

# --- Bibliothèque "stock" : cœur métier, service, persistance et interface ---
file(GLOB_RECURSE STOCK_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM STOCK_SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_library(stock ${STOCK_SOURCES})
target_include_directories(stock PUBLIC src)
target_link_libraries(stock PUBLIC Qt6::Widgets)

# --- L'exécutable : racine de composition et lancement de l'application ---
add_executable(gestion_stock src/main.cpp)
target_link_libraries(gestion_stock PRIVATE stock Qt6::Widgets)

# --- Tests : Catch2 v3 téléchargé automatiquement ---
option(STOCK_BUILD_TESTS "Construire les tests" ON)
if(STOCK_BUILD_TESTS)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0)
    FetchContent_MakeAvailable(catch2)

    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
    add_executable(stock_tests ${TEST_SOURCES})
    target_link_libraries(stock_tests PRIVATE stock Catch2::Catch2WithMain)

    include(CTest)
    add_test(NAME stock_tests COMMAND stock_tests)
endif()
