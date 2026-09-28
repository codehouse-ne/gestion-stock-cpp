cmake_minimum_required(VERSION 3.16)
project(gestion_stock LANGUAGES CXX)

# --- Norme C++17 imposee ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Compilation stricte : les warnings aident a ne pas ecrire du code douteux ---
add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)

# --- Bibliotheque "stock" : tout sauf main.cpp ---
file(GLOB_RECURSE STOCK_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM STOCK_SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)

# Si la liste est VIDE (aucun .cpp metier pour l'instant), on doit quand meme
# donner quelque chose a compiler a la bibliotheque. Solution : un fichier
# .cpp "vide" de maintien que l'on supprimera a l'etape 1.
if(NOT STOCK_SOURCES)
    set(STOCK_SOURCES ${CMAKE_SOURCE_DIR}/src/persistance/vide.cpp)
endif()

add_library(stock ${STOCK_SOURCES})
target_include_directories(stock PUBLIC src)



# --- L'executable : juste la racine de composition ---
add_executable(gestion_stock src/main.cpp)
target_link_libraries(gestion_stock PRIVATE stock)

# --- Tests : Catch2 v3 telecharge automatiquement ---
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
