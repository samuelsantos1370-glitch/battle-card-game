#include <SFML/Graphics.hpp>
#include "menu.hpp"

int main() {
        sf::RenderWindow janela(sf::VideoMode(800, 600), "teste movimento");
        janela.setFramerateLimit(60);

        const sf::Color corFundo(20, 20, 20);

        while (janela.isOpen()) {
                int retorno = 0;
                sf::Event evento;

                while (janela.pollEvent(evento)) {
                        int resultado = processarEventoMenu(janela, evento);

                        if (resultado != 0) {
                                retorno = resultado;
                                break;
                        }
                }

                switch (retorno) {
                        case 2:
                                break;

                        case 3:
                                break;

                        case 1:
                        case 4:
                                janela.close();
                                break;
                }

                if (!janela.isOpen()) {
                        break;
                }

                janela.clear(corFundo);
                desenharMenu(janela);
                janela.display();
        }

        return 0;
}
