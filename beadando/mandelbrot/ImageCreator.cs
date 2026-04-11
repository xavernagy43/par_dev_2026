using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Mandelbrot
{
    public class ImageCreator
    {
        public static Bitmap CreateImage(int[] data, int width, int height, int max)
        {
            Bitmap bmp = new Bitmap(width, height);

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    int i = data[y * width + x];

                    Color color = i == max
                        ? Color.Black
                        : Color.FromArgb(i % 255, 0, 255 - (i % 255));

                    bmp.SetPixel(x, y, color);
                }
            }

            return bmp;
        }

        public static void Save(Bitmap bmp, string fileName)
        {
            bmp.Save(fileName, ImageFormat.Png);
        }
    }
}
