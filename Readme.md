# CatView

.CAT file viewer, used by Microprose game Magic: The Gathering (1997)

![Cat View](/images/CatView_Demo.jpg)

## CAT Format

.CATs are sort of fast-files: bunch of binary files are merged as single file and their offsets are stored in .CAT Directory table.

File header:

```c++
struct 
{
    uint32_t EntriesCount;

    CatalogEntry Directory[0];      // variable size = EntriesCount * sizeof(CatalogEntry)

    /// ... Merged files after directory

} CATFile;
```

Catalog directory entry format:

```c++
typedef struct _CatalogEntry
{
    uint32_t    nameHash;       // 0
    uint32_t    fileOffset;     // 4
    uint32_t    size;           // 8
} CatalogEntry;
```

## Hashing algorithm

Who knows the name? :)

```c++
uint32_t CalcHash (char *str)
{
    int count = 3;
    int sum1 = 0;
    int sum2 = 0;   

    uint32_t initialHash = (str[0] ^ str[1]) << 24;

    while (*str)
    {
        char character = *str++;

        if ( count & 1)
        {
            sum1 += (count * character);
        }
        else
        {
            sum2 += (count * character);
        }

        count++;
    }

    return initialHash | ( (sum1 * sum2) 0xFFFFFF );
}
```
