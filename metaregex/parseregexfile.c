#include <stdio.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC
#include "../lib/pcre2.h"
#include "../main.h"
#include <stdlib.h>

#define MAX_FACET_ADDED_SPACE 0xFFF

char* create_regex_facets(const char *regex, size_t *szregex)
{
	int error, ncaptures, namecount, name_entry_size, n, rc;
	PCRE2_SIZE erroroffset, * povector;

	char* pstr;

	uint32_t ovectorcount;

	PCRE2_SPTR name_table, tabptr;

	pcre2_match_data* pmatch_data = pcre2_match_data_create(0xFFFF, 0);

	pcre2_match_context* match_context = pcre2_match_context_create(0);

	pcre2_code* pcode;

	PCRE2_UCHAR* pnewsubstr[2];

	char errormsg[0xFF];

	struct calloutinfo nametable;

	size_t szstrin = *szregex;

	*szregex += MAX_FACET_ADDED_SPACE;

	*pnewsubstr = malloc(*szregex);

	pcode = pcre2_compile("[(]\\?<(\\w+?)>", PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

	pcre2_substitute(pcode, regex, szstrin, 0, PCRE2_SUBSTITUTE_GLOBAL, pmatch_data, match_context, "(?<$1facet>", PCRE2_ZERO_TERMINATED, *pnewsubstr, szregex);

	pcode = pcre2_compile("[(]\\?C\\d++[)]", PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

	szstrin = *szregex;

	*szregex += MAX_FACET_ADDED_SPACE;

	1[pnewsubstr] = malloc(*szregex);

	pcre2_substitute(pcode, *pnewsubstr, szstrin, 0, PCRE2_SUBSTITUTE_GLOBAL, pmatch_data, match_context, "", 0, 1[pnewsubstr], szregex);

	pcode = pcre2_compile("[(]\\?&((\\w(?!facet))++)[)]", PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

	szstrin = *szregex;

	*szregex += MAX_FACET_ADDED_SPACE;

	free(*pnewsubstr); *pnewsubstr = malloc(*szregex);

	pcre2_substitute(pcode, 1[pnewsubstr], szstrin, 0, PCRE2_SUBSTITUTE_GLOBAL, pmatch_data, match_context, "(?&$1facet)", PCRE2_ZERO_TERMINATED, *pnewsubstr, szregex);

	pcre2_match_context_free(match_context), pcre2_match_data_free(pmatch_data); free(1[pnewsubstr]);

	return *pnewsubstr;
}

int callout_regex(pcre2_callout_block* a, void* b)
{
	struct calloutinfo* ptable = b;
	int n = getnameloc("filename", *ptable);

	void addregexfile(const char* content);

	size_t szfilepattern = a->offset_vector[2 * n + 1] - a->offset_vector[2 * n], szfacet;

	char* filename = malloc(szfilepattern + 1);

	sprintf(filename, "%.*s", (unsigned int)szfilepattern, a->subject + a->offset_vector[2 * n]);

	char* pfilepattern = openfile(filename, &szfilepattern), *pregexfacet = (szfacet = szfilepattern, create_regex_facets(pfilepattern, &szfacet));

	addregexfile(pfilepattern, szfilepattern);

	addregexfile(pregexfacet, szfacet);

	return 0;
}

char* glueregexfile(char* filename)
{
	void beginregex();
	const char* retrievefinalregex(const char* rest, size_t szrest);
	size_t plen;
	size_t szfilepattern, szregexpattern, szfacet;
	char* pfilepattern = openfile(filename, &szfilepattern), *pregexpattern = openfile("./metaregex/regex.regex", &szregexpattern), *facet;

	beginregex();

	compile_pattern_and_execute(pregexpattern, pfilepattern, callout_regex, szregexpattern, szfilepattern, 0, &plen, 0);

	szfacet = szfilepattern - plen;

	facet = create_regex_facets(pfilepattern + plen, &szfacet);

	addregexfile(facet, szfacet);

	return retrievefinalregex(pfilepattern + plen, szfilepattern - plen);
}