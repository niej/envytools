#include "rnn.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

uint64_t *strides = 0;
int stridesnum = 0;
int stridesmax = 0;

int startcol = 64;

void seekcol (int src, int dst) {
	if (dst <= src)
		printf ("\t");
	else {
		int n = dst/8 - src/8;
		if (n) {
			while (n--)
				printf ("\t");
			n = dst&7;
		} else
			n = dst-src;
		while (n--)
			printf (" ");
	}
}

void printdef (char *name, char *suf, int type, uint64_t val) {
	int len;
	if (suf)
		printf ("#define %s__%s%n", name, suf, &len);
	else
		printf ("#define %s%n", name, &len);
	if (type == 0 && val > 0xffffffffull)
		seekcol (len, startcol-8);
	else
		seekcol (len, startcol);
	switch (type) {
		case 0:
			if (val > 0xffffffffull)
				printf ("0x%016"PRIx64"ULL\n", val);
			else
				printf ("0x%08"PRIx64"\n", val);
			break;
		case 1:
			printf ("%"PRIu64"\n", val);
			break;
	}
}

void printvalue (struct rnnvalue *val, int shift) {
	if (val->varinfo.dead)
		return;
	if (val->valvalid)
		printdef (val->fullname, 0, 0, val->value << shift);
}

void printbitfield (struct rnnbitfield *bf, int shift);

void printtypeinfo (struct rnntypeinfo *ti, char *prefix, int shift) {
	if (ti->shr)
		printdef (prefix, "SHR", 1, ti->shr);
	int i;
	for (i = 0; i < ti->valsnum; i++)
		printvalue(ti->vals[i], shift);
	for (i = 0; i < ti->bitfieldsnum; i++)
		printbitfield(ti->bitfields[i], shift);
}

void printbitfield (struct rnnbitfield *bf, int shift) {
	if (bf->varinfo.dead)
		return;
	if (bf->typeinfo.type == RNN_TTYPE_BOOLEAN) {
		printdef (bf->fullname, 0, 0, bf->mask << shift);
	} else {
		printdef (bf->fullname, "MASK", 0, bf->mask << shift);
		printdef (bf->fullname, "SHIFT", 1, bf->low + shift);
	}
	printtypeinfo (&bf->typeinfo, bf->fullname, bf->low + shift);
}

void printdelem (struct rnndelem *elem, uint64_t offset) {
	if (elem->varinfo.dead)
		return;
	if (elem->length != 1)
		RNN_ADDARRAY(strides, elem->stride);
	if (elem->name) {
		if (stridesnum) {
			int len, total;
			printf ("#define %s(%n", elem->fullname, &total);
			int i;
			for (i = 0; i < stridesnum; i++) {
				if (i) {
					printf(", ");
					total += 2;
				}
				printf ("i%d%n", i, &len);
				total += len;
			}
			printf (")");
			total++;
			seekcol (total, startcol-1);
			printf ("(0x%08"PRIx64"", offset + elem->offset);
			for (i = 0; i < stridesnum; i++)
				printf (" + %#" PRIx64 "*(i%d)", strides[i], i);
			printf (")\n");
		} else
			printdef (elem->fullname, 0, 0, offset + elem->offset);
		if (elem->stride)
			printdef (elem->fullname, "ESIZE", 0, elem->stride);
		if (elem->length != 1)
			printdef (elem->fullname, "LEN", 0, elem->length);
		printtypeinfo (&elem->typeinfo, elem->fullname, 0);
	}
	printf ("\n");
	int j;
	for (j = 0; j < elem->subelemsnum; j++) {
		printdelem(elem->subelems[j], offset + elem->offset);
	}
	if (elem->length != 1) stridesnum--;
}

int main(int argc, char **argv) {
	rnn_init();
	struct rnndb *db = rnn_newdb();
	rnn_parsefile (db, argv[1]);
	rnn_prepdb (db);
	int i;
	for (i = 0; i < db->enumsnum; i++) {
		if (db->enums[i]->isinline)
			continue;
		printf ("/* enum %s */\n", db->enums[i]->fullname);
		int j;
		for (j = 0; j < db->enums[i]->valsnum; j++)
			printvalue (db->enums[i]->vals[j], 0);
		printf ("\n");
	}
	for (i = 0; i < db->bitsetsnum; i++) {
		if (db->bitsets[i]->isinline)
			continue;
		printf ("/* bitset %s */\n", db->bitsets[i]->fullname);
		int j;
		for (j = 0; j < db->bitsets[i]->bitfieldsnum; j++)
			printbitfield (db->bitsets[i]->bitfields[j], 0);
		printf ("\n");
	}
	for (i = 0; i < db->domainsnum; i++) {
		printf ("/* domain %s of width %d */\n", db->domains[i]->fullname, db->domains[i]->width);
		if (db->domains[i]->size)
			printdef (db->domains[i]->fullname, "SIZE", 0, db->domains[i]->size);
		int j;
		for (j = 0; j < db->domains[i]->subelemsnum; j++) {
			printdelem(db->domains[i]->subelems[j], 0);
		}
		printf ("\n");
	}
	return db->estatus;
}
