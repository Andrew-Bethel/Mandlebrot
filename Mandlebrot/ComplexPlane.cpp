#include "ComplexPlane.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <cmath>
#include <string>
#include <complex>
#include <thread>

using namespace sf;
using namespace std;

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
	m_pixel_size.x = pixelWidth;
	m_pixel_size.y = pixelHeight;
	m_aspectRatio = (float)pixelHeight / (float)pixelWidth; //careful of integer divide
	m_plane_center = {0, 0};
	m_zoomCount = 0;
	m_state = State::CALCULATING;
	m_vArray = VertexArray(Points, pixelWidth * pixelHeight);
	
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
	target.draw(m_vArray);
}

void ComplexPlane::RenderNew(int a, int b)
{
	//cout << k << endl;
	for (int i = a; i < b; i++)
	{
		for (int j = 0; j < m_pixel_size.x; ++j)
		{
			// Set the position variable in the element of VertexArray that corresponds to the screen coordinate j,i 
			// mapping the two-dimensional position at j,i to its 1D array 
			m_vArray[j + i * m_pixel_size.x].position = { (float)j,(float)i };

			// Use ComplexPlane::mapPixelToCoords to find the Vector2f coordinate in the complex plane
			Vector2f coord = mapPixelToCoords(Vector2i(j, i));

			// Call ComplexPlane::countIterations with the Vector2f coordinate as its argument
			size_t iterations = countIterations(coord);

			// store the RGB values for the current pixel
			Uint8 r, g, b;

			// pass iterations & RGB values by reference
			iterationsToRGB(iterations, r, g, b);

			// Set the color variable in the element of VertexArray that corresponds to the screen coordinate j,i
			m_vArray[j + i * m_pixel_size.x].color = { r, g, b };
		}
	}
}


void ComplexPlane::updateRender()
{

	unsigned int nthreads = std::thread::hardware_concurrency();

	if (m_state == State::CALCULATING)
	{
		/*for (int i = 0; i < m_pixel_size.y; i++)
		{
			for (int j = 0; j < m_pixel_size.x; j++)
			{
				m_vArray[j + i * m_pixel_size.x].position = { (float)j,(float)i };
				Vector2f coordinate = mapPixelToCoords({ j, i });
				size_t count = countIterations(coordinate);

				Uint8 r;
				Uint8 g;
				Uint8 b;

				iterationsToRGB(count, r, g, b);
				m_vArray[j + i * m_pixel_size.x].color = { r,g,b };
			}
		}*/
		
		thread t1(&ComplexPlane::RenderNew, this, 0, 3 * m_pixel_size.y / 12);
		thread t2(&ComplexPlane::RenderNew, this, 3 * m_pixel_size.y / 12, 6 * m_pixel_size.y / 12);
		thread t3(&ComplexPlane::RenderNew, this, 6 * m_pixel_size.y / 12, 9 * m_pixel_size.y / 12);
		thread t4(&ComplexPlane::RenderNew, this, 9 * m_pixel_size.y / 12,  m_pixel_size.y );

		t1.join();
		t2.join();
		t3.join();
		t4.join();

		m_state = State::DISPLAYING;
	}
}

void ComplexPlane::zoomIn()
{
	m_zoomCount++;
	float xCoord = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
	float yCoord = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
	m_plane_size = { xCoord, yCoord };
	m_state = State::CALCULATING;
}

void ComplexPlane::zoomOut()
{
	m_zoomCount--;
	float xCoord = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
	float yCoord = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
	m_plane_size = { xCoord, yCoord };
	m_state = State::CALCULATING;
}

void ComplexPlane::setCenter(Vector2i mousePixel)
{
	Vector2f newCenter = mapPixelToCoords(mousePixel);
	m_plane_center = newCenter;
	m_state = State::CALCULATING;
}


void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
	m_mouseLocation = mapPixelToCoords(mousePixel);
}

void ComplexPlane::loadText(Text& text)
{
	stringstream ss;

	// Add to stringstream
	ss << "Mandelbrot Set\n";
	ss << "Center: (" << m_plane_center.x << ", " << m_plane_center.y << ")\n";
	ss << "Cursor: (" << m_mouseLocation.x << ", " << m_mouseLocation.y << ")\n";
	ss << "Left-click to Zoom in\n";
	ss << "Right-click to Zoom out\n";

	// create a text string
	text.setString(ss.str());

}

int ComplexPlane::countIterations(Vector2f coord)
{
	complex<double> c(coord.x, coord.y);
	complex<double>x(0, 0);
	size_t count = 0;
	while (abs(x) < 2 && count < MAX_ITER)
	{
		x = pow(x, 2) + c;
		count++;
	}
	return count;
}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{
	if (count == MAX_ITER)	//At MAX_ITER color the pixel black
	{
		r = 0;
		g = 0;
		b = 0;
	}
	else if (count < 10)		//purple
	{
		r = 127;
		g = 0;
		b = 255;
	}
	else if (count < 20)		//purple
	{
		r = 0;
		g = 0;
		b = 255;
	}

	else if (count < 30)		//just green
	{
		r = 0;
		g = 255;
		b = 0;
	}
	else if (count < 40)		//just blue
	{
		r = 0;
		g = 0;
		b = 255;
	}
	else if (count < 50)		//yellow
	{
		r = 255;
		g = 255;
		b = 0;
	}
	else if (count < 63)		//fully red
	{
		r = 255;
		g = 0;
		b = 0;
	}
}

Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel)
{
	float xRatio = static_cast<float>(mousePixel.x) / static_cast<float>(m_pixel_size.x);
	float yRatio = static_cast<float>(mousePixel.y) / static_cast<float>(m_pixel_size.y);

	// Calculate the corresponding coordinates in the complex plane
	float xCoord = m_plane_center.x - 0.5f * m_plane_size.x + xRatio * m_plane_size.x;
	float yCoord = m_plane_center.y - 0.5f * m_plane_size.y + yRatio * m_plane_size.y;

	return { xCoord, yCoord };
}