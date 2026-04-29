using Mandelbrot;
using System;
using System.Diagnostics;
using System.Drawing;

namespace MandelbrotApp
{
    class Program
    {
        static void Main(string[] args)
        {
            int width = 1920;
            int height = 1080;
            int maxIterations = 500;
            int workerGroup = 4;

            Console.WriteLine("Mandelbrot generálás indul...\n");

            DateTime startTime = DateTime.Now;
            Console.WriteLine($"Kezdés ideje: {startTime}");

            //GPU mandelbrot
            MandelbrotGenerator mandelbrot = new MandelbrotGenerator();

            //CPU mandelbrot 
            MandelbrotGeneratorCPU mandelbrotCPU = new MandelbrotGeneratorCPU();

            Stopwatch sw = Stopwatch.StartNew();

            int[] data = mandelbrot.Generate(width, height, maxIterations, workerGroup);

            //int[] data = mandelbrotCPU.Generate(width, height, maxIterations, workerGroup);

            sw.Stop();

            Bitmap image = ImageCreator.CreateImage(data, width, height, maxIterations);

            

            DateTime endTime = DateTime.Now;
            Console.WriteLine($"Befejezés ideje: {endTime}");

            ImageCreator.Save(image, "mandelbrot.png");

            Console.WriteLine("\nParaméterek:");
            Console.WriteLine($"Felbontás: {width}x{height}");
            Console.WriteLine($"WorkerGroup: {workerGroup}");

            Console.WriteLine("\nFutási idő (ms):");
            Console.WriteLine(sw.ElapsedMilliseconds);
        }
    }
}