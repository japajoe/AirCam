#ifndef COLORFORMAT_HPP
#define COLORFORMAT_HPP

namespace AirCam
{
    class ColorFormat
    {
    public:
        static void YUYVToRGB(unsigned char *yuv_buffer, unsigned char *rgb_buffer, int width, int height)
        {
            int x;
            int z=0;
            unsigned char* ptr = rgb_buffer;
            unsigned char* yuyv = yuv_buffer;
            
            for(x = 0; x < width * height; x++)
            {
                int r, g, b;
                int y, u, v;

                if (!z)
                    y = yuyv[0] << 8;
                else
                    y = yuyv[2] << 8;
                u = yuyv[1] - 128;
                v = yuyv[3] - 128;

                r = (y + (359 * v)) >> 8;
                g = (y - (88 * u) - (183 * v)) >> 8;
                b = (y + (454 * u)) >> 8;

                *(ptr++) = (r > 255) ? 255 : ((r < 0) ? 0 : r);
                *(ptr++) = (g > 255) ? 255 : ((g < 0) ? 0 : g);
                *(ptr++) = (b > 255) ? 255 : ((b < 0) ? 0 : b);

                if(z++)
                {
                    z = 0;
                    yuyv += 4;
                }
            }
        }
    };
}

#endif
