#ifndef FILE_HELPER_H_
#define FILE_HELPER_H_
#include <cstdio>
#include <cstdint>
template <uint8_t N = 1>
void freadFloat(void *p, FILE *fp)
{
    fread(p, sizeof(float), N, fp);
}

template <typename T>
void freadUint(T &n, size_t size, FILE *fp)
{
    fread(&n, size, 1, fp);
}

template <typename T>
void freadInt(T &n, size_t size, FILE *fp)
{
    switch (size)
    {
    case 1:
    {
        int8_t i;
        fread(&i, 1, 1, fp);
        n = i;
        return;
    }
    case 2:
    {
        int16_t i;
        fread(&i, 2, 1, fp);
        n = i;
        return;
    }
    case 4:
    {
        int32_t i;
        fread(&i, 4, 1, fp);
        n = i;
        return;
    }
    }
    fread(&n, size, 1, fp);
}

#endif
