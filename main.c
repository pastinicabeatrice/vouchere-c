#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	unsigned int nrVoucher;
	char* numeBeneficiar;
	char* dataExpirare;
	float valoare;
} Voucher;

typedef struct Nod {
	Voucher inf;
	struct Nod* next;
	struct Nod* prev;
} Nod;

char* copiereSir(const char* s) {
	char* r = (char*)malloc(strlen(s) + 1);
	strcpy(r, s);
	return r;
}

Nod* inserare(Nod* cap, Nod** coada, Voucher v) {
	Nod* nou = (Nod*)malloc(sizeof(Nod));
	if (nou == NULL) {
		return cap;
	}
	nou->inf.nrVoucher = v.nrVoucher;
	nou->inf.valoare = v.valoare;
	nou->inf.numeBeneficiar = copiereSir(v.numeBeneficiar);
	nou->inf.dataExpirare = copiereSir(v.dataExpirare);
	nou->next = NULL;
	nou->prev = *coada;

	if (cap == NULL) {
		cap = nou;
	}
	else {
		(*coada)->next = nou;
	}
	*coada = nou;
	return cap;
}

void afisareVoucher(Voucher v) {
	printf("Voucher %u | Beneficiar: %s | Expira: %s | Valoare: %.2f\n",
		v.nrVoucher, v.numeBeneficiar, v.dataExpirare, v.valoare);
}

void afisareInainte(Nod* cap) {
	for (Nod* t = cap; t != NULL; t = t->next) {
		afisareVoucher(t->inf);
	}
}

void afisareInapoi(Nod* coada) {
	for (Nod* t = coada; t != NULL; t = t->prev) {
		afisareVoucher(t->inf);
	}
}

Nod* citireFisier(const char* numeFisier, Nod** coada) {
	Nod* cap = NULL;
	*coada = NULL;
	FILE* f = fopen(numeFisier, "r");
	if (f == NULL) {
		printf("Fisierul %s nu a putut fi deschis.\n", numeFisier);
		return NULL;
	}
	Voucher v;
	char nume[100];
	char data[11];
	while (fscanf(f, "%u %99s %10s %f", &v.nrVoucher, nume, data, &v.valoare) == 4) {
		v.numeBeneficiar = nume;
		v.dataExpirare = data;
		cap = inserare(cap, coada, v);
	}
	fclose(f);
	return cap;
}

float valoareBeneficiar(Nod* cap, const char* beneficiar) {
	float total = 0;
	for (Nod* t = cap; t != NULL; t = t->next) {
		if (strcmp(t->inf.numeBeneficiar, beneficiar) == 0) {
			total += t->inf.valoare;
		}
	}
	return total;
}

void eliberareNod(Nod* n) {
	free(n->inf.numeBeneficiar);
	free(n->inf.dataExpirare);
	free(n);
}

Nod* stergereExpirate(Nod* cap, Nod** coada, const char* dataCurenta) {
	Nod* t = cap;
	while (t != NULL) {
		Nod* urm = t->next;
		if (strcmp(t->inf.dataExpirare, dataCurenta) < 0) {
			if (t->prev != NULL) {
				t->prev->next = t->next;
			}
			else {
				cap = t->next;
			}
			if (t->next != NULL) {
				t->next->prev = t->prev;
			}
			else {
				*coada = t->prev;
			}
			eliberareNod(t);
		}
		t = urm;
	}
	return cap;
}

void dezalocare(Nod** cap, Nod** coada) {
	while (*cap != NULL) {
		Nod* urm = (*cap)->next;
		eliberareNod(*cap);
		*cap = urm;
	}
	*coada = NULL;
}

int main() {
	Nod* coada = NULL;
	Nod* cap = citireFisier("vouchere.txt", &coada);

	printf("Vouchere (ordine de inserare):\n");
	afisareInainte(cap);

	printf("\nVouchere (ordine inversa):\n");
	afisareInapoi(coada);

	printf("\nValoare totala pentru Ionescu: %.2f\n", valoareBeneficiar(cap, "Ionescu"));

	const char* azi = "2025-06-01";
	cap = stergereExpirate(cap, &coada, azi);
	printf("\nVouchere valabile la %s:\n", azi);
	afisareInainte(cap);

	dezalocare(&cap, &coada);
	return 0;
}
