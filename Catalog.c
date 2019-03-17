/// Microprose .CAT fastfiles reversing
// (c) ogamespec@gmail.com

typedef struct _Catalog
{
	union
	{
		struct
		{
			FILE * file; 		// 0
			int entriesCount; 	// 4
			CatalogEntry * dir; 		// 8
			CatalogEntry * lastFoundEntry; 		// 0xc

			// +0x10
			char catname[...]; 

		};

		uint8_t raw[0x45];
	}
} Catalog; 	

typedef struct _CatalogEntry
{
	uint32_t 	nameHash; 		// 0
	uint32_t	fileOffset; 	// 4
	uint32_t 	size; 			// 8
} CatalogEntry;

Catalog Cats[20]; 			// 0x62C950

int CatCheckNames; 		// 0x5230F8

int LoadCatalog(char * name) 			// 0x439AF0
{
	int index = 0;

	/// Find next free index 

	int maxIndex = 5;
	for (; index< maxIndex; index++)
	{
		if (Cats[index].file == NULL)
			break;
	}

	assert ( i < maxIndex, "Too many open Catalogs: Max %d\n" );

	Catalog * cat = &Cats[index];

	strcpy (cat->catname, name);

	cat->file = fopen ( name, "rb");
	if (!cat->file)
		return 0;

	/// Allocate and read directory

	fread ( &cat->entriesCount, 1, sizeof(cat->entriesCount), cat->file);

	cat->dir = malloc ( cat->entriesCount * sizeof(CatalogEntry) );

	fread ( cat->dir, sizeof(CatalogEntry), cat->entriesCount, cat->file);

	if ( CatCheckNames )
	{
		/// ... "Duplicate short name found in catalogs"
	}

	return index + 1; 		// index to catalogId 
}

bool CloseCatalog(int catalogId) 			// 0x439D3A
{
	int index = catalogId - 1;

	if ( Cats[index].file )
	{
		free (Cats[index].dir );
		fclose (Cats[index].file );

		Cats[index].dir = NULL;
		Cats[index].file = NULL;
		Cats[index].entriesCount = 0;
		return true;
	}
	else
	{
		return false;
	}
}

int CatalogCompareEntries (void *a, void *b) 		// 0x439DF6
{
	CatalogEntry *entry1 = (CatalogEntry *)a, *entry2 = (CatalogEntry *)b;

	return entry1->nameHash > entry2->nameHash ?
		1 :
		(entry1->nameHash < entry2->nameHash ? -1 : 0);
}

CatalogEntry * CatalogSearch (Catalog * cat, char * path) 		// 0x439E46
{
	uint32_t hash = CatalogCalcHash (path);

	if ( cat->lastFoundEntry && cat->lastFoundEntry->nameHash == hash )
	{
		return cat->lastFoundEntry;
	}
	else
	{
		/// void *bsearch(const void *key, const void *base, size_t num, size_t size, int (*compare)(const void*, const void*))

		void * found = bsearch ( &hash, cat->dir, cat->entriesCount, sizeof(CatalogEntry), CatalogCompareEntries);
		cat->lastFoundEntry = found;

		return (CatalogEntry *)found;
	}
}

// Returns size of loaded entry (-1: failed)

int CatalogReadEntry (int catalogId, char * name, void ** ptrOut) 			// 0x439EC1
{
	int index = catalogId - 1;

	Catalog * cat = &Cats[index];

	CatalogEntry * catEntry = CatalogSearch ( cat, name);
	if ( catEntry )
	{
		if ( *ptrOut == NULL )
		{
			*ptrOut = malloc ( catEntry->size + 16 );
		}

		fseek (cat->file, catEntry->fileOffset, SEEK_SET );

		return fread ( *ptrOut, 1, catEntry->size, cat->file );
	}
	else
	{
		return -1;
	}
}

uint32_t CatalogCalcHash (char * path) 			// 0x439F6A
{
	int count = 3;
	int sum1 = 0;
	int sum2 = 0;

	_splitpath ( path, drive, dir, fname, ext);
	strcat (fname, ext);

	uint32_t hash = (fname[0] ^ fname[1]) << 24;

	uint8_t * ptr = fname;

	while ( *ptr )
	{
		character = *ptr++;

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

	hash |= (sum1 * sum2) 0xFFFFFF;

	return hash;
}
