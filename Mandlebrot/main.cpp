#include <iostream>
#include <SFML/Graphics.hpp>
#include "ComplexPlane.h"
using namespace std;
using namespace sf;

//main is a fucking mess fix dis shit sometime after .cpp file is done


int main()
{
    int pixelWidth = VideoMode::getDesktopMode().width / 2;
    int pixelHeight = VideoMode::getDesktopMode().height / 2;

    VideoMode vm(pixelWidth, pixelHeight);

    RenderWindow window(vm, "Mandelbrot Set", Style::Default);
    ComplexPlane CompPlane(pixelWidth, pixelHeight);

    sf::Font font;
    sf::Text text;
    if (!font.loadFromFile("yoster.ttf"))
    {
        // error..
    }

    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    text.setPosition(10, 10);



    while (window.isOpen())
    {

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                //handles event to close window
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    CompPlane.zoomOut();
                    CompPlane.setCenter({ event.mouseButton.x, event.mouseButton.y });
                }
                else if (event.mouseButton.button == sf::Mouse::Left)
                {
                    CompPlane.zoomIn();
                    CompPlane.setCenter({ event.mouseButton.x, event.mouseButton.y });
                }
            } //end of mouse clicks
            if (event.type == sf::Event::MouseMoved)
            {
                CompPlane.setMouseLocation({ event.mouseButton.x, event.mouseButton.y });
            } //end of mouse move
            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                window.close();
            } //check if window will close from keyboard click

            //update scene segment
            CompPlane.updateRender();
            CompPlane.loadText(text);

            //draw scene segment
            window.clear();
            window.draw(CompPlane);
            window.draw(text);
            window.display();
        }
    } //end of while loop
}
