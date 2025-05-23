#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define CSV_FILE "yazarlar.csv"

typedef struct yazar {
    char yazarAdi[50];
    char yazarSoyadi[50];
    int  yazarId;
} yazar;

typedef struct yazarNode {
    yazar data;
    struct yazarNode *next;
} yazarNode;

/**
 * Yeni bir yazar düðümü oluþturur, yazarCount'u bir artýrýr, ID atar.
 */
yazarNode *createYazar(int *yazarCount) {
    yazarNode *node = malloc(sizeof *node);
    if (!node) {
        perror("malloc createYazar");
        return NULL;
    }
    printf("Yazar adi: ");
    scanf(" %49s", node->data.yazarAdi);
    printf("Yazar soyadi: ");
    scanf(" %49s", node->data.yazarSoyadi);

    (*yazarCount)++;
    node->data.yazarId = *yazarCount;
    node->next = NULL;
    return node;
}

/**
 * Yazar listesini CSV dosyasýna kaydeder (üstüne yazar).
 */
void saveToCSV(const char *filename, yazarNode *head) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Dosya acilirken hata: %s\n", strerror(errno));
        return;
    }
    // Baþlýk satýrý (isteðe baðlý)
    fprintf(fp, "ID,Adi,Soyadi\n");
    for (yazarNode *it = head; it; it = it->next) {
        fprintf(fp, "%d,%s,%s\n",
                it->data.yazarId,
                it->data.yazarAdi,
                it->data.yazarSoyadi);
    }
    fclose(fp);
}

/**
 * CSV dosyasýndan yükleme yapar. ID sayacýný ve listeyi oluþturur.
 * Returns: 1 if loaded, 0 if file created fresh.
 */
int loadFromCSV(const char *filename, yazarNode **headPtr, yazarNode **tailPtr, int *yazarCount) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        // Dosya yoksa oluþtur
        fp = fopen(filename, "w");
        if (!fp) {
            fprintf(stderr, "Dosya olusturulamadi: %s\n", strerror(errno));
            return 0;
        }
        fprintf(fp, "ID,Adi,Soyadi\n");
        fclose(fp);
        return 0;
    }
    // Oku ve düðüm yarat
    char line[256];
    // Ýlk satýrý atla (baþlýk)
    if (!fgets(line, sizeof line, fp)) {
        fclose(fp);
        return 1;
    }
    while (fgets(line, sizeof line, fp)) {
        int id;
        char adi[50], soyadi[50];
        if (sscanf(line, "%d,%49s,%49s", &id, adi, soyadi) == 3) {
            yazarNode *node = malloc(sizeof *node);
            if (!node) continue;
            node->data.yazarId = id;
            strncpy(node->data.yazarAdi, adi, sizeof node->data.yazarAdi);
            strncpy(node->data.yazarSoyadi, soyadi, sizeof node->data.yazarSoyadi);
            node->next = NULL;
            if (!*headPtr) {
                *headPtr = *tailPtr = node;
            } else {
                (*tailPtr)->next = node;
                *tailPtr = node;
            }
            if (id > *yazarCount) *yazarCount = id;
        }
    }
    fclose(fp);
    return 1;
}

/**
 * Ýsimle silme iþlemi. Düðümleri günceller.
 */
void yazarDeleter(yazarNode **headPtr, int *yazarCount) {
    if (!*headPtr) {
        printf("Liste bos, silinecek yazar yok.\n");
        return;
    }
    char target[50];
    printf("Silmek istediginiz yazar adini giriniz: ");
    scanf(" %49s", target);
    yazarNode *cur = *headPtr, *prev = NULL;
    while (cur && strcmp(cur->data.yazarAdi, target) != 0) {
        prev = cur;
        cur = cur->next;
    }
    if (!cur) {
        printf("'%s' adli yazar bulunamadi.\n", target);
        return;
    }
    if (!prev) *headPtr = cur->next;
    else prev->next = cur->next;
    printf("Yazar '%s' silindi (ID=%d).\n",
           cur->data.yazarAdi, cur->data.yazarId);
    free(cur);
    // Güncel listeyi CSV'ye yaz
    saveToCSV(CSV_FILE, *headPtr);
}

/**
 * Ýsimle güncelleme iþlemi. Ad veya soyad deðiþikliði yapar.
 */
void yazarUpgrader(yazarNode **headPtr) {
    if (!*headPtr) {
        printf("Liste bos, guncellenecek yazar yok.\n");
        return;
    }
    char target[50];
    printf("Duzenlemek istediginiz yazar adini giriniz: ");
    scanf(" %49s", target);
    yazarNode *cur = *headPtr;
    while (cur && strcmp(cur->data.yazarAdi, target) != 0) cur = cur->next;
    if (!cur) {
        printf("'%s' adli yazar bulunamadi.\n", target);
        return;
    }
    int secim;
    printf("Degistirmek istediginiz ozelligi seciniz:\n");
    printf("1) Yazar adi\n2) Yazar soyadi\nSecim: ");
    scanf("%d", &secim);
    if (secim == 1) {
        printf("Yeni yazar adini giriniz: ");
        scanf(" %49s", cur->data.yazarAdi);
    } else if (secim == 2) {
        printf("Yeni yazar soyadini giriniz: ");
        scanf(" %49s", cur->data.yazarSoyadi);
    } else {
        printf("Gecersiz secim.\n");
        return;
    }
    printf("Yazar (ID=%d) guncellendi: %s %s\n",
           cur->data.yazarId,
           cur->data.yazarAdi,
           cur->data.yazarSoyadi);
    saveToCSV(CSV_FILE, *headPtr);
}

/**
 * Tüm listeyi serbest býrakmak için yardýmcý fonksiyon
 */
void freeList(yazarNode *head) {
    while (head) {
        yazarNode *tmp = head;
        head = head->next;
        free(tmp);
    }
}

int main(void) {
    int yazarCount = 0;
    yazarNode *head = NULL, *tail = NULL;
    int choice;

    // CSV'den yükle veya oluþtur
    loadFromCSV(CSV_FILE, &head, &tail, &yazarCount);

    do {
        printf("\n=== Yazar Islemleri Menu ===\n");
        printf("1. Yazar Ekle\n");
        printf("2. Yazar Sil\n");
        printf("3. Mevcut Yazarlar\n");
        printf("4. Yazar Guncelle\n");
        printf("0. Cikis\n");
        printf("Seciminiz: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                yazarNode *newNode = createYazar(&yazarCount);
                if (newNode) {
                    if (!head) head = tail = newNode;
                    else {
                        tail->next = newNode;
                        tail = newNode;
                    }
                    printf("Yazar eklendi: %s %s (ID=%d)\n",
                           newNode->data.yazarAdi,
                           newNode->data.yazarSoyadi,
                           newNode->data.yazarId);
                    saveToCSV(CSV_FILE, head);
                }
                break;
            }
            case 2:
                yazarDeleter(&head, &yazarCount);
                // tail güncellemesi
                if (!head) tail = NULL;
                else {
                    tail = head;
                    while (tail->next) tail = tail->next;
                }
                break;
            case 3:
                if (!head) printf("Liste bos.\n");
                else {
                    printf("\n--- Mevcut Yazarlar ---\n");
                    for (yazarNode *it = head; it; it = it->next) {
                        printf("ID=%d, %s %s\n",
                               it->data.yazarId,
                               it->data.yazarAdi,
                               it->data.yazarSoyadi);
                    }
                }
                break;
            case 4:
                yazarUpgrader(&head);
                break;
            case 0:
                printf("Programdan cikiliyor...\n");
                break;
            default:
                printf("Gecersiz secim, tekrar deneyin.\n");
        }
    } while (choice != 0);

    freeList(head);
    return 0;
}

