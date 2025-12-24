#include "ppm2Ascii.h"

// IO FUNCTIONS
void readImage(const char *fileName, struct image *imageBuffer)
{
    struct image *Image;
    char buff[16];
    int c, RGBMaxVal;

    FILE *fp = fopen(fileName, "rb");
    if (!fp)
    {
        fprintf(stderr, "Unable to open file '%s'\n", fileName);
        exit(1);
    }

    if (!fgets(buff, sizeof(buff), fp))
    {
        perror(fileName);
        exit(1);
    }

    if (buff[0] != 'P' || buff[1] != '6')
    {
        fprintf(stderr, "Invalid image format (must be 'P6')\n");
        exit(1);
    }

    Image = (struct image *)malloc(sizeof(struct image));
    if (!Image)
    {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    c = getc(fp);
    while (c == '#')
    {
        while (getc(fp) != '\n')
            ;
        c = getc(fp);
    }

    ungetc(c, fp);
    if (fscanf(fp, "%ld %ld", &Image->width, &Image->height) != 2)
    {
        fprintf(stderr, "Invalid image size (error loading '%s')\n", fileName);
        exit(1);
    }

    if (fscanf(fp, "%d", &RGBMaxVal) != 1)
    {
        fprintf(stderr, "Invalid rgb component (error loading '%s')\n", fileName);
        exit(1);
    }

    if (RGBMaxVal != 255)
    {
        fprintf(stderr, "'%s' does not have 8-bits components\n", fileName);
        exit(1);
    }

    while (fgetc(fp) != '\n')
        ;
    Image->pixelData = (struct pixel *)malloc(Image->width * Image->height * sizeof(struct pixel));

    if (!Image)
    {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    if (fread(Image->pixelData, 3 * Image->width, Image->height, fp) != Image->height)
    {
        fprintf(stderr, "Error loading image '%s'\n", fileName);
        exit(1);
    }

    fclose(fp);
    *imageBuffer = *Image;
}

void writeImage(const char *fileName, struct image *Image)
{
    FILE *fp;
    fp = fopen(fileName, "wb");
    if (!fp)
    {
        fprintf(stderr, "Unable to open file '%s'\n", fileName);
        exit(1);
    }

    fprintf(fp, "P6\n");

    fprintf(fp, "%ld %ld\n", Image->width, Image->height);

    fprintf(fp, "255\n");

    fwrite(Image->pixelData, 3 * Image->width, Image->height, fp);
    fclose(fp);
}

// IMAGE MANIP
void convertGrayscale(struct image *Image)
{

    if (Image)
    {

        for (size_t i = 0; i < Image->width * Image->height; i++)
        {
            int alpha = (Image->pixelData[i].R + Image->pixelData[i].G + Image->pixelData[i].B) / 3;

            Image->pixelData[i].R = alpha;
            Image->pixelData[i].G = alpha;
            Image->pixelData[i].B = alpha;
        }
    }
}

void imageScaleDown(struct image *Image)
{
    int counter = 0;
    while (Image->width > 32)
    {
        int newWidth = Image->width / 2;
        int newHeight = Image->height / 2;

        for (size_t y = 0; y < Image->height; y += 2)
        {
            for (size_t x = 0; x < Image->width; x += 2)
            {

                int oldIdx = y * Image->width + x;
                int newIdx = (y / 2) * newWidth + (x / 2);

                Image->pixelData[newIdx] = Image->pixelData[oldIdx];
            }
        }
        Image->width = newWidth;
        Image->height = newHeight;

        printf("ITERATIONS: %d\n", counter);
        counter++;
    }
}

void outputToTerminal(struct image *Image)
{
    if (Image)
    {
        const char *chars = " .:-=+*#%@";

        for (size_t y = 0; y < Image->height; y++)
        {
            for (size_t x = 0; x < Image->width; x++)
            {
                int Idx = y * Image->width + x;

                int charIndex = Image->pixelData[Idx].R * (strlen(chars) - 1) / 255;
                putchar(chars[charIndex]);
                putchar(chars[charIndex]);
            }
            putchar('\n');
        }
        putchar('\n');
    }
}

void ppm2AsciiWrapper(char *fileName)
{
    struct image Image;
    readImage(fileName, &Image);
    convertGrayscale(&Image);
    imageScaleDown(&Image);
    outputToTerminal(&Image);
    free(Image.pixelData);
}