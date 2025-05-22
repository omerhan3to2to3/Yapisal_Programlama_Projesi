#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
typedef struct yazar{
	char yazarAdi[50];
	char yazarSoyadi[50];
	int yazarId;
}yazar;
typedef struct yazarNode{
	yazar y;
	struct yazarNode *next;
}yazarNode;

int main(int argc, char *argv[]) {
    int *yazarCount = malloc(sizeof *yazarCount);
    if (!yazarCount) {
        perror("yazarcount malloc error");
        return EXIT_FAILURE;
    }
    *yazarCount = 0;
	return 0;
}
yazarNode *createYazar(int *yazarCount){
	yazarNode *yazar= (yazarNode*)malloc(sizeof(yazarNode));
    if (!yazar) {
        perror("malloc create yazarnode ");
        return NULL;
    }
	printf("yazar adi giriniz ");
	scanf("%s", yazar->y.yazarAdi);
	printf("yazar soydi giriniz ");
	scanf("%s", yazar->y.yazarSoyadi);
	(*yazarCount)++;
	yazar->y.yazarId = (*yazarCount);	
	yazar->next = NULL;
	return yazar;
}
