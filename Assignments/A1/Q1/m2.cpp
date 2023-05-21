#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int mandelbrot(double real, double imag) {
	int limit = 1000;
	double zReal = real;
	double zImag = imag;

	for (int i = 0; i < limit; ++i) 
	{
		double r2 = zReal * zReal;
		double i2 = zImag * zImag;
		
		if (r2 + i2 > 2.0) return 0;

		zImag = 2.0 * zReal * zImag + imag;
		zReal = r2 - i2 + real;
	}
	return 1;
}

int main() {
	
	// int width = 20; // N = number of characters fitting horizontally on my screen 
	// int heigth = 16; // M = number of characters fitting vertically on my screen
	// int K = 1000;

	int width, heigth, K;
	cin >> width >> heigth >> K;

	vector<vector<int>> v(heigth, vector<int>(width, 0));
		
	double x_start = -1.5;
	double x_fin = 1.0;
	double y_start = -1.0;
	double y_fin = 1.0;
	
    double dx = (x_fin - x_start)/(width - 1);
	double dy = (y_fin - y_start)/(heigth - 1);

	for (int i = 0; i < heigth; i++) 
    {
		for (int j = 0; j < width; j++) 
        {

			// cout << "i: " << i << ", j: " << j << endl;
			// cout << "flat: " << i*width+j << endl;

			double x = x_start + j*dx; // current real value
			double y = y_fin - i*dy; // current imaginary value

			// cout << "x: " << x << ", y: " << y << endl;
			
			int value = mandelbrot(x,y);

            v[i][j] = value;
			
		}
		// cout << endl;
	}

	for(int i=0; i<heigth; i++)
	{
		for(int j=0; j<width; j++)
		{
			cout << v[i][j] << " ";
		}
		cout << endl;
	}

	return 0;
}