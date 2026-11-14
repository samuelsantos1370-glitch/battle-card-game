#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "menu.hpp"

namespace {
        constexpr float LARGURA_VIEW = 1280.f;
        constexpr float ALTURA_VIEW = 720.f;

        class Botao {
                private:
                        sf::Texture textura;
                        sf::Sprite sprite;
                        bool carregado = false;

                public:
                        bool inicializar(const std::string& caminho, float x, float y) {
                                carregado = false;

                                if (!textura.loadFromFile(caminho)) {
                                        std::cerr << "Erro ao carregar: " << caminho << std::endl;
                                        return false;
                                }

                                sprite.setTexture(textura);

                                sf::FloatRect bounds = sprite.getLocalBounds();
                                sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
                                sprite.setPosition(x, y);

                                carregado = true;
                                return true;
                        }

                        bool estaSobre(const sf::Vector2f& posicaoMouse) const {
                                return carregado && sprite.getGlobalBounds().contains(posicaoMouse);
                        }

                        void atualizarVisual(const sf::Vector2f& posicaoMouse) {
                                if (!carregado) {
                                        return;
                                }

                                if (estaSobre(posicaoMouse)) {
                                        sprite.setColor(sf::Color(200, 200, 200));
                                } else {
                                        sprite.setColor(sf::Color::White);
                                }
                        }

                        void desenhar(sf::RenderWindow& window) const {
                                if (carregado) {
                                        window.draw(sprite);
                                }
                        }
                };

        class Menu {
                private:
                        Botao btnPlay;
                        Botao btnOptions;
                        Botao btnExit;
                        bool inicializado = false;

                public:
                        void inicializar() {
                                if (inicializado) {
                                        return;
                                }

                                const float centroX = LARGURA_VIEW / 2.f;

                                btnPlay.inicializar("assets/textures/play_button.png", centroX, 320.f);
                                btnOptions.inicializar("assets/textures/options_button.png", centroX, 440.f);
                                btnExit.inicializar("assets/textures/exit_button.png", centroX, 560.f);

                                inicializado = true;
                        }

                        int processarClique(sf::RenderWindow& window, const sf::Event& evento) {
                                if (evento.type == sf::Event::Closed) {
                                        return 4;
                                }

                                if (evento.type != sf::Event::MouseButtonPressed ||
                                        evento.mouseButton.button != sf::Mouse::Left) {
                                        return 0;
                                }

                                sf::View view = criarView(window);
                                sf::Vector2i pixelClique(evento.mouseButton.x, evento.mouseButton.y);
                                sf::Vector2f posicaoClique = window.mapPixelToCoords(pixelClique, view);

                                if (btnPlay.estaSobre(posicaoClique)) {
                                        return 3;
                                }

                                if (btnOptions.estaSobre(posicaoClique)) {
                                        return 2;
                                }

                                if (btnExit.estaSobre(posicaoClique)) {
                                        return 1;
                                }

                                return 0;
                        }

                        void atualizar(const sf::Vector2f& posicaoMouse) {
                                btnPlay.atualizarVisual(posicaoMouse);
                                btnOptions.atualizarVisual(posicaoMouse);
                                btnExit.atualizarVisual(posicaoMouse);
                        }

                        void desenhar(sf::RenderWindow& window) const {
                                btnPlay.desenhar(window);
                                btnOptions.desenhar(window);
                                btnExit.desenhar(window);
                        }

                        static sf::View criarView(const sf::RenderWindow& window) {
                                sf::View view(sf::FloatRect(0.f, 0.f, LARGURA_VIEW, ALTURA_VIEW));
                                sf::Vector2u tamanho = window.getSize();

                                if (tamanho.x == 0 || tamanho.y == 0) {
                                        return view;
                                }

                                const float proporcaoJanela =
                                        static_cast<float>(tamanho.x) / static_cast<float>(tamanho.y);
                                const float proporcaoView = LARGURA_VIEW / ALTURA_VIEW;

                                sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);

                                if (proporcaoJanela > proporcaoView) {
                                        viewport.width = proporcaoView / proporcaoJanela;
                                        viewport.left = (1.f - viewport.width) / 2.f;
                                } else {
                                        viewport.height = proporcaoJanela / proporcaoView;
                                        viewport.top = (1.f - viewport.height) / 2.f;
                                }

                                view.setViewport(viewport);
                                return view;
                        }
        };

        Menu& obterMenu() {
                static Menu menu;
                return menu;
        }
}

int processarEventoMenu(sf::RenderWindow& window, const sf::Event& evento) {
        Menu& menu = obterMenu();
        menu.inicializar();
        return menu.processarClique(window, evento);
}

void desenharMenu(sf::RenderWindow& window) {
        Menu& menu = obterMenu();
        menu.inicializar();

        sf::View view = Menu::criarView(window);
        window.setView(view);

        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f posicaoMouse = window.mapPixelToCoords(mousePixel, view);

        menu.atualizar(posicaoMouse);
        menu.desenhar(window);
}
