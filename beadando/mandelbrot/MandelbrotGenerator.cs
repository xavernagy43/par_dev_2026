using OpenCL.Net;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace Mandelbrot
{
    public class MandelbrotGenerator
    {
        public int[] Generate(int width, int height, int maxIterations, int workerCount)
        {
            ErrorCode error;

            Platform platform = Cl.GetPlatformIDs(out error)[0];
            Device device = Cl.GetDeviceIDs(platform, DeviceType.Gpu, out error)[0];

            Context context = Cl.CreateContext(null, 1, new[] { device }, null, IntPtr.Zero, out error);
            CommandQueue queue = Cl.CreateCommandQueue(context, device, 0, out error);

            string source = GetKernel();
            Program program = Cl.CreateProgramWithSource(context, 1, new[] { source }, null, out error);
            Cl.BuildProgram(program, 1, new[] { device }, "", null, IntPtr.Zero);

            Kernel kernel = Cl.CreateKernel(program, "mandelbrot", out error);

            int[] result = new int[width * height];

            IMem buffer = Cl.CreateBuffer(context,
                MemFlags.WriteOnly,
                result.Length * sizeof(int),
                out error);

            Cl.SetKernelArg(kernel, 0, buffer);
            Cl.SetKernelArg(kernel, 1, width);
            Cl.SetKernelArg(kernel, 2, height);
            Cl.SetKernelArg(kernel, 3, maxIterations);

            IntPtr[] globalWorkSize = new IntPtr[]
            {
                (IntPtr)(width),
                (IntPtr)(height)
            };

            IntPtr[] localWorkSize = new IntPtr[]
            {
                (IntPtr)workerCount,
                (IntPtr)workerCount
            };

            Cl.EnqueueNDRangeKernel(queue, kernel, 2, null,
                globalWorkSize, localWorkSize, 0, null, out _);

            Cl.EnqueueReadBuffer(queue, buffer, Bool.True, IntPtr.Zero,
                new IntPtr(result.Length * sizeof(int)),
                result, 0, null, out _);

            return result;
        }

        private string GetKernel()
        {
            return @"
            __kernel void mandelbrot(
                __global int* output,
                int width,
                int height,
                int maxIterations)
            {
                int x = get_global_id(0);
                int y = get_global_id(1);

                int index = y * width + x;

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

                output[index] = i;
            }";
        }
    }
}
