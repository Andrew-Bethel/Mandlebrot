#pragma once
#ifndef COMPLEXPLANE_H
#define COMPLEXPLANE_H
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>


// Make code easier to type with "using namespace"
using namespace sf;
using namespace std;

// global constants
const unsigned int MAX_ITER = 64;
const float BASE_WIDTH = 4.0;
const float BASE_HEIGHT = 4.0;
const float BASE_ZOOM = 0.5;

// enum class state
/* - When the program starts or when the user clicks the mouse to move,
   we will be CALCULATING the complex plane display

   - will set the state variable back to DISPLAYING after it is done so
   we only have to do it once per click*/
enum class State { CALCULATING, DISPLAYING };


class ComplexPlane : public sf::Drawable
{
private:
	// private member variables
	VertexArray m_vArray;
	State m_state;
	Vector2f m_mouseLocation;
	Vector2i m_pixel_size;
	Vector2f m_plane_center;
	Vector2f m_plane_size;
	int m_zoomCount;
	float m_aspectRatio;

	int countIterations(Vector2f coord);
	void iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b);
	Vector2f mapPixelToCoords(Vector2i mousePixel);




public:
	// public member functions 

	// constructor
	ComplexPlane(int pixelWidth, int pixelHeight);
	void draw(RenderTarget& target, RenderStates states) const;
	void zoomIn();
	void zoomOut();
	void setCenter(Vector2i mousePixel);
	void setMouseLocation(Vector2i mousePixel);
	void loadText(Text& text);
	void updateRender();
	void RenderNew(int a, int b);






};


#endif