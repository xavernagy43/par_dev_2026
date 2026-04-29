using System;

namespace Mandelbrot
{
    public class MandelbrotGeneratorCPU
    {
        public int[] Generate(int width, int height, int maxIterations)
        {
            int[] result = new int[width * height];
            
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    double a = (x - width / 2.0) * 4.0 / width;
                    double b = (y - height / 2.0) * 4.0 / height;

                    double real = 0;
                    double imag = 0;

                    int i = 0;

                    while (real * real + imag * imag <= 4 && i < maxIterations)
                    {
                        double temp = real * real - imag * imag + a;
                        imag = 2 * real * imag + b;
                        real = temp;
                        i++;
                    }

                    result[y * width + x] = i;
                }
            }

            return result;    
        }
    }
}