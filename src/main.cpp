#include <SFML/Graphics.hpp>
#include "menu.hpp"

int main() {
    sf::RenderWindow janela(sf::VideoMode(800, 600), "DeckBound");
    janela.setFramerateLimit(60);

    const sf::Color corFundo(20, 20, 20);

    while (janela.isOpen()) {
        int retorno = 0;
        sf::Event evento;

        while (janela.pollEvent(evento)) {
            int resultado = processarClique(janela, evento);
            if (resultado != 0) {
                retorno = resultado;
                break;
            }
        }

        switch (retorno) {
            case 1: // Exit
            case 4: // Fechar janela
                janela.close();
                break;
            case 2: // Options
            case 3: // Play
            case 5: // Credits
                // As telas correspondentes ainda serão implementadas.
                break;
        }

        if (!janela.isOpen()) break;

        janela.clear(corFundo);
        desenharMenu(janela);
        janela.display();
    }

    return 0;
}
