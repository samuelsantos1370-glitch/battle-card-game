#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "menu.hpp"

namespace {

// Resolução virtual original do menu, em formato paisagem.
sf::View resolucao(sf::FloatRect(0.f, 0.f, 1280.f, 720.f));
std::unordered_map<std::string, std::unique_ptr<sf::Texture>> texturas;

void ajustarView(sf::RenderWindow& window) {
    const float proporcaoVirtual = 1280.f / 720.f;
    const sf::Vector2u tamanho = window.getSize();
    const float proporcaoJanela = static_cast<float>(tamanho.x) /
                                  static_cast<float>(tamanho.y);

    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
    if (proporcaoJanela > proporcaoVirtual) {
        viewport.width = proporcaoVirtual / proporcaoJanela;
        viewport.left = (1.f - viewport.width) / 2.f;
    } else {
        viewport.height = proporcaoJanela / proporcaoVirtual;
        viewport.top = (1.f - viewport.height) / 2.f;
    }

    resolucao.setViewport(viewport);
    window.setView(resolucao);
}

class botao {
private:
    sf::Sprite sprite;
    bool carregado = false;

public:
    bool carregar(const std::string& nome, const std::string& path) {
        auto textura = std::make_unique<sf::Texture>();
        if (!textura->loadFromFile(path)) {
            std::cerr << "Erro ao carregar textura: " << path << '\n';
            return false;
        }

        texturas[nome] = std::move(textura);
        return true;
    }

    void inicializar(const std::string& nome, float x, float y) {
        auto it = texturas.find(nome);
        if (it == texturas.end()) {
            std::cerr << "Textura nao carregada: " << nome << '\n';
            carregado = false;
            return;
        }

        sprite.setTexture(*it->second);

        // A imagem de Options tem margens transparentes grandes.
        // Recortar apenas a área útil mantém a arte original e melhora o clique.
        if (nome == "options") {
            sprite.setTextureRect(sf::IntRect(104, 126, 404, 163));
        }

        float escala = 0.85f;
        if (nome == "options") escala = 0.90f;
        if (nome == "credits") escala = 1.05f;

        sprite.setScale(escala, escala);
        const sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        sprite.setPosition(x, y);
        sprite.setColor(sf::Color::White);
        carregado = true;
    }

    bool estaSobre(const sf::Vector2f& posicaoMouse) const {
        return carregado && sprite.getGlobalBounds().contains(posicaoMouse);
    }

    void holding(const sf::Vector2f& posicaoMouse) {
        if (!carregado) return;

        if (estaSobre(posicaoMouse)) {
            sprite.setColor(sf::Color(215, 215, 255));
        } else {
            sprite.setColor(sf::Color::White);
        }
    }

    void desenhar(sf::RenderWindow& window) {
        if (carregado) window.draw(sprite);
    }
};

class menu {
private:
    botao play;
    botao options;
    botao exit;
    botao credits;

    sf::Texture texturaFundo;
    sf::Sprite spriteFundo;
    sf::Texture texturaTitulo;
    sf::Sprite spriteTitulo;
    bool carregado = false;

    bool carregarFundo() {
        if (!texturaFundo.loadFromFile("assets/textures/background.jpg")) {
            std::cerr << "Erro ao carregar assets/textures/background.jpg\n";
            return false;
        }

        spriteFundo.setTexture(texturaFundo);
        const sf::Vector2u tamanho = texturaFundo.getSize();
        spriteFundo.setScale(1280.f / static_cast<float>(tamanho.x),
                             720.f / static_cast<float>(tamanho.y));
        return true;
    }

    bool carregarTitulo() {
        if (!texturaTitulo.loadFromFile("assets/textures/title.png")) {
            std::cerr << "Erro ao carregar assets/textures/title.png\n";
            return false;
        }

        spriteTitulo.setTexture(texturaTitulo);
        const sf::FloatRect bounds = spriteTitulo.getLocalBounds();
        spriteTitulo.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        spriteTitulo.setScale(1.15f, 1.15f);
        spriteTitulo.setPosition(640.f, 145.f);
        return true;
    }

public:
    bool carregarMenu() {
        if (!play.carregar("play", "assets/textures/play.png")) return false;
        if (!options.carregar("options", "assets/textures/options.png")) return false;
        if (!exit.carregar("exit", "assets/textures/exit.png")) return false;
        if (!credits.carregar("credits", "assets/textures/credits.png")) return false;
        if (!carregarFundo() || !carregarTitulo()) return false;

        // Mantém os quatro botões e as artes originais; só ajusta escala e posição.
        play.inicializar("play", 426.f, 365.f);
        options.inicializar("options", 854.f, 365.f);
        credits.inicializar("credits", 426.f, 545.f);
        exit.inicializar("exit", 854.f, 545.f);

        carregado = true;
        return true;
    }

    int processarClique(sf::RenderWindow& window, const sf::Event& evento) {
        if (evento.type == sf::Event::Closed) return 4;

        if (evento.type != sf::Event::MouseButtonPressed ||
            evento.mouseButton.button != sf::Mouse::Left) {
            return 0;
        }

        ajustarView(window);
        const sf::Vector2i pixelClique(evento.mouseButton.x, evento.mouseButton.y);
        const sf::Vector2f posicaoClique = window.mapPixelToCoords(pixelClique, resolucao);

        if (play.estaSobre(posicaoClique)) return 3;
        if (options.estaSobre(posicaoClique)) return 2;
        if (exit.estaSobre(posicaoClique)) return 1;
        if (credits.estaSobre(posicaoClique)) return 5;
        return 0;
    }

    void desenhar(sf::RenderWindow& window) {
        if (!carregado) return;

        ajustarView(window);
        const sf::Vector2i pixelMouse = sf::Mouse::getPosition(window);
        const sf::Vector2f posicaoMouse = window.mapPixelToCoords(pixelMouse, resolucao);

        play.holding(posicaoMouse);
        options.holding(posicaoMouse);
        credits.holding(posicaoMouse);
        exit.holding(posicaoMouse);

        window.draw(spriteFundo);
        window.draw(spriteTitulo);
        play.desenhar(window);
        options.desenhar(window);
        credits.desenhar(window);
        exit.desenhar(window);
    }
};

menu& obterMenu() {
    static menu instancia;
    return instancia;
}

bool garantirMenuCarregado() {
    static bool tentativaFeita = false;
    static bool sucesso = false;
    if (!tentativaFeita) {
        sucesso = obterMenu().carregarMenu();
        tentativaFeita = true;
    }
    return sucesso;
}

} // namespace

int processarClique(sf::RenderWindow& window, sf::Event& evento) {
    if (!garantirMenuCarregado()) return 0;
    return obterMenu().processarClique(window, evento);
}

void desenharMenu(sf::RenderWindow& window) {
    if (!garantirMenuCarregado()) return;
    obterMenu().desenhar(window);
}
