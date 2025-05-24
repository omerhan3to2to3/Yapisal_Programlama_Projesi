#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define YAZAR_CSV_FILE "yazarlar.csv"
#define OGRENCI_CSV_FILE "Ogrenciler.csv"
#define KITAP_CSV_FILE "Kitaplar.csv"
#define KITAP_ORNEKLERI_CSV_FILE "KitapOrnekleri.csv"
#define KITAP_YAZAR_CSV_FILE "KitapYazar.csv" // Yeni CSV dosyasý

// Struct definitions (önceki gibi)
typedef struct yazar {
    char yazarAdi[50];
    char yazarSoyadi[50];
    int  yazarId;
} yazar;

typedef struct ogrenci{
	char ogrenciAdi[50];
	char ogrenciSoyadi[50];
	int kutupPuani;
	unsigned int ogrenciNo:27;
}ogrenci;

typedef struct yazarNode {
    yazar data;
    struct yazarNode *next;
} yazarNode;

typedef struct ogrenciNode {
    ogrenci data;
    struct ogrenciNode *next;
    struct ogrenciNode *prev;
} ogrenciNode;

typedef struct KitapOrnegi {
    char etiketNo[25];
    char durum[20];
} KitapOrnegi;

typedef struct KitapOrnegiNode {
    KitapOrnegi data;
    struct KitapOrnegiNode *next;
} KitapOrnegiNode;

typedef struct Kitap {
    char kitapAdi[100];
    char isbn[14];
    int adet;
    KitapOrnegiNode *ornekler_head;
} Kitap;

typedef struct KitapNode {
    Kitap data;
    struct KitapNode *next;
} KitapNode;

// --- Kitap-Yazar Eþleþtirme Struct ---
typedef struct KitapYazarEslestirme {
    char isbn[14];
    int yazarId;
} KitapYazarEslestirme;

// Global dinamik dizi ve sayaçlarý
KitapYazarEslestirme *g_kitapYazarEslestirmeleri = NULL;
int g_kitapYazarEslestirmeSayisi = 0;
int g_kitapYazarEslestirmeKapasitesi = 0;


// Function Prototypes (Yazar, Ogrenci, Kitap - önceki gibi)
// ... (önceki prototipler buraya gelecek) ...
yazarNode *createYazar(int *yazarCount);
void saveYazarlarToCSV(const char *filename, yazarNode *head);
int loadYazarlarFromCSV(const char *filename, yazarNode **headPtr, yazarNode **tailPtr, int *yazarCount);
void yazarDeleter(yazarNode **headPtr, yazarNode **tailPtr, int *yazarCount); // Güncellenecek
void yazarUpgrader(yazarNode **headPtr);
void listYazarlar(yazarNode *head);
void freeYazarList(yazarNode *head);
void yazarMenu(yazarNode **yazarHeadPtr, yazarNode **yazarTailPtr, int *yazarCountPtr);

int isOgrenciNoUnique(unsigned int ogrenciNo, ogrenciNode *head);
ogrenciNode *createOgrenciNode(ogrenciNode *currentHead);
void saveOgrencilerToCSV(const char *filename, ogrenciNode *head);
void loadOgrencilerFromCSV(const char *filename, ogrenciNode **headPtr, ogrenciNode **tailPtr);
void ogrenciDeleter(ogrenciNode **headPtr, ogrenciNode **tailPtr);
void ogrenciUpgrader(ogrenciNode **headPtr, ogrenciNode **tailPtr);
void listOgrenciler(ogrenciNode *head);
void freeOgrenciList(ogrenciNode *head);
void ogrenciMenu(ogrenciNode **ogrenciHeadPtr, ogrenciNode **ogrenciTailPtr);

void freeKitapOrnegiList(KitapOrnegiNode *head);
int isISBNValid(const char *isbn);
int isISBNUnique(const char *isbn, KitapNode *head);
KitapNode *createKitap(KitapNode *kitapHeadGlobal);
void saveKitaplarToCSV(const char *filename, KitapNode *head);
void saveKitapOrnekleriToCSV(const char *filename_ornekler, KitapNode *head_kitaplar);
void loadKitaplarFromCSV(const char *filename, KitapNode **headPtr, KitapNode **tailPtr);
void loadKitapOrnekleriFromCSV(const char *filename_ornekler, KitapNode *kitapHead);
void kitapDeleter(KitapNode **headPtr, KitapNode **tailPtr); // Güncellenecek
void kitapUpgrader(KitapNode **headPtr, KitapNode **tailPtr);
void listKitaplarDetail(KitapNode *head);
void freeKitapList(KitapNode *head);
void kitapMenu(KitapNode **kitapHeadPtr, KitapNode **kitapTailPtr, ogrenciNode *ogrenciListHead);

// --- Kitap-Yazar Eþleþtirme Fonksiyon Prototipleri ---
void initKitapYazarDizisi();
int addKitapYazarEslestirmeDizisine(const char* isbn, int yazarId);
void removeKitapYazarEslestirmeDizisinden(const char* isbn, int yazarId);
void removeKitapYazarEslestirmeleriByISBN(const char* isbn);
void updateYazarIdInEslestirmeler(int eskiYazarId, int yeniYazarId);
void loadKitapYazarEslestirmeleri(const char *filename);
void saveKitapYazarEslestirmeleri(const char *filename);
void freeKitapYazarDizisi();
KitapNode* findKitapByISBN(KitapNode* head, const char* isbn);
yazarNode* findYazarByID(yazarNode* head, int id);
void eslestirKitapYazar(KitapNode *kitapHead, yazarNode *yazarHead);
void kaldirKitapYazarEslestirmesi(KitapNode *kitapHead, yazarNode *yazarHead);
void listeleKitabinYazarlari(KitapNode *kitapHead, yazarNode *yazarHead);
void listeleYazarinKitaplari(KitapNode *kitapHead, yazarNode *yazarHead);
void listeleTumKitapYazarEslestirmeleri(KitapNode *kitapHead, yazarNode *yazarHead);
void kitapYazarMenu(KitapNode *kitapHead, yazarNode *yazarHead);

// --- Helper Fonksiyonlar ---
KitapNode* findKitapByISBN(KitapNode* head, const char* isbn) {
    for (KitapNode* current = head; current != NULL; current = current->next) {
        if (strcmp(current->data.isbn, isbn) == 0) {
            return current;
        }
    }
    return NULL;
}

yazarNode* findYazarByID(yazarNode* head, int id) {
    if (id == -1) return NULL; // Silinmiþ yazar için özel durum
    for (yazarNode* current = head; current != NULL; current = current->next) {
        if (current->data.yazarId == id) {
            return current;
        }
    }
    return NULL;
}

// --- Kitap-Yazar Eþleþtirme Fonksiyonlarý ---
void initKitapYazarDizisi() {
    g_kitapYazarEslestirmeleri = NULL;
    g_kitapYazarEslestirmeSayisi = 0;
    g_kitapYazarEslestirmeKapasitesi = 0;
}

int addKitapYazarEslestirmeDizisine(const char* isbn, int yazarId) {
    if (g_kitapYazarEslestirmeSayisi >= g_kitapYazarEslestirmeKapasitesi) {
        int yeniKapasite = (g_kitapYazarEslestirmeKapasitesi == 0) ? 10 : g_kitapYazarEslestirmeKapasitesi * 2;
        KitapYazarEslestirme* temp = realloc(g_kitapYazarEslestirmeleri, yeniKapasite * sizeof(KitapYazarEslestirme));
        if (!temp) {
            perror("realloc addKitapYazarEslestirmeDizisine");
            return 0; // Baþarýsýz
        }
        g_kitapYazarEslestirmeleri = temp;
        g_kitapYazarEslestirmeKapasitesi = yeniKapasite;
    }
    strncpy(g_kitapYazarEslestirmeleri[g_kitapYazarEslestirmeSayisi].isbn, isbn, 13);
    g_kitapYazarEslestirmeleri[g_kitapYazarEslestirmeSayisi].isbn[13] = '\0';
    g_kitapYazarEslestirmeleri[g_kitapYazarEslestirmeSayisi].yazarId = yazarId;
    g_kitapYazarEslestirmeSayisi++;
    return 1; // Baþarýlý
}

void removeKitapYazarEslestirmeDizisinden(const char* isbn, int yazarId) {
    int i, j;
    int bulundu = 0;
    for (i = 0; i < g_kitapYazarEslestirmeSayisi; i++) {
        if (strcmp(g_kitapYazarEslestirmeleri[i].isbn, isbn) == 0 && g_kitapYazarEslestirmeleri[i].yazarId == yazarId) {
            bulundu = 1;
            // Kaydýrma ile silme
            for (j = i; j < g_kitapYazarEslestirmeSayisi - 1; j++) {
                g_kitapYazarEslestirmeleri[j] = g_kitapYazarEslestirmeleri[j + 1];
            }
            g_kitapYazarEslestirmeSayisi--;
            i--; // Ayný indeksi tekrar kontrol et (eðer birden fazla ayný kayýt varsa - olmamalý)
        }
    }
    if (!bulundu) {
       // printf("Uyari: Kaldirilmak istenen eslestirme (ISBN: %s, YazarID: %d) bulunamadi.\n", isbn, yazarId);
    }
}

void removeKitapYazarEslestirmeleriByISBN(const char* isbn) {
    int i, j;
    for (i = 0; i < g_kitapYazarEslestirmeSayisi; i++) {
        if (strcmp(g_kitapYazarEslestirmeleri[i].isbn, isbn) == 0) {
            for (j = i; j < g_kitapYazarEslestirmeSayisi - 1; j++) {
                g_kitapYazarEslestirmeleri[j] = g_kitapYazarEslestirmeleri[j + 1];
            }
            g_kitapYazarEslestirmeSayisi--;
            i--; // Ayný indeksi tekrar kontrol et
        }
    }
}

void updateYazarIdInEslestirmeler(int eskiYazarId, int yeniYazarId) {
    for (int i = 0; i < g_kitapYazarEslestirmeSayisi; i++) {
        if (g_kitapYazarEslestirmeleri[i].yazarId == eskiYazarId) {
            g_kitapYazarEslestirmeleri[i].yazarId = yeniYazarId;
        }
    }
}

void loadKitapYazarEslestirmeleri(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fp = fopen(filename, "w"); // Dosya yoksa oluþtur
        if (!fp) {
            fprintf(stderr, "KitapYazar CSV dosyasi olusturulamadi: %s\n", strerror(errno));
            return;
        }
        fprintf(fp, "ISBN,YazarID\n"); // Baþlýk ekle
        fclose(fp);
        return; // Boþ dosya, yüklenecek bir þey yok
    }

    char line[100];
    // Baþlýk satýrýný atla
    if (!fgets(line, sizeof line, fp)) {
        fclose(fp);
        return;
    }

    // Önce mevcut diziyi temizle (program birden fazla kez çaðrýlýrsa diye)
    freeKitapYazarDizisi();
    initKitapYazarDizisi();


    while (fgets(line, sizeof line, fp)) {
        char isbn_str[14];
        int yazarId_val;
        if (sscanf(line, "%13[^,],%d", isbn_str, &yazarId_val) == 2) {
            addKitapYazarEslestirmeDizisine(isbn_str, yazarId_val);
        }
    }
    fclose(fp);
}

void saveKitapYazarEslestirmeleri(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "KitapYazar CSV dosyasi acilirken hata: %s\n", strerror(errno));
        return;
    }
    fprintf(fp, "ISBN,YazarID\n"); // Baþlýk
    for (int i = 0; i < g_kitapYazarEslestirmeSayisi; i++) {
        fprintf(fp, "%s,%d\n", g_kitapYazarEslestirmeleri[i].isbn, g_kitapYazarEslestirmeleri[i].yazarId);
    }
    fclose(fp);
}

void freeKitapYazarDizisi() {
    if (g_kitapYazarEslestirmeleri) {
        free(g_kitapYazarEslestirmeleri);
        g_kitapYazarEslestirmeleri = NULL;
    }
    g_kitapYazarEslestirmeSayisi = 0;
    g_kitapYazarEslestirmeKapasitesi = 0;
}

void eslestirKitapYazar(KitapNode *kitapHead, yazarNode *yazarHead) {
    char isbn_input[14];
    int yazarId_input;

    printf("Eslestirilecek kitabin ISBN'ini giriniz: ");
    scanf(" %13s", isbn_input);
    while(getchar() != '\n');

    if (!findKitapByISBN(kitapHead, isbn_input)) {
        printf("Hata: '%s' ISBN numarali kitap bulunamadi.\n", isbn_input);
        return;
    }

    printf("Eslestirilecek yazarin ID'sini giriniz: ");
    if (scanf("%d", &yazarId_input) != 1) {
        printf("Gecersiz yazar ID girisi.\n");
        while(getchar() != '\n');
        return;
    }
    while(getchar() != '\n');

    if (!findYazarByID(yazarHead, yazarId_input)) {
        printf("Hata: '%d' ID'li yazar bulunamadi.\n", yazarId_input);
        return;
    }

    // Mevcut eþleþme var mý kontrol et
    for (int i = 0; i < g_kitapYazarEslestirmeSayisi; i++) {
        if (strcmp(g_kitapYazarEslestirmeleri[i].isbn, isbn_input) == 0 &&
            g_kitapYazarEslestirmeleri[i].yazarId == yazarId_input) {
            printf("Bu kitap ve yazar zaten eslestirilmis.\n");
            return;
        }
    }

    if (addKitapYazarEslestirmeDizisine(isbn_input, yazarId_input)) {
        printf("Kitap (ISBN: %s) ve Yazar (ID: %d) basariyla eslestirildi.\n", isbn_input, yazarId_input);
        saveKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE);
    } else {
        printf("Eslestirme yapilirken bir hata olustu.\n");
    }
}

void kaldirKitapYazarEslestirmesi(KitapNode *kitapHead, yazarNode *yazarHead) {
    char isbn_input[14];
    int yazarId_input;

    printf("Eslestirmesi kaldirilacak kitabin ISBN'ini giriniz: ");
    scanf(" %13s", isbn_input);
    while(getchar() != '\n');

    printf("Eslestirmesi kaldirilacak yazarin ID'sini giriniz: ");
     if (scanf("%d", &yazarId_input) != 1) {
        printf("Gecersiz yazar ID girisi.\n");
        while(getchar() != '\n');
        return;
    }
    while(getchar() != '\n');

    int bulundu = 0;
    for (int i = 0; i < g_kitapYazarEslestirmeSayisi; i++) {
        if (strcmp(g_kitapYazarEslestirmeleri[i].isbn, isbn_input) == 0 &&
            g_kitapYazarEslestirmeleri[i].yazarId == yazarId_input) {
            bulundu = 1;
            break;
        }
    }

    if (!bulundu) {
        printf("Belirtilen kitap (ISBN: %s) ve yazar (ID: %d) icin eslestirme bulunamadi.\n", isbn_input, yazarId_input);
        return;
    }

    removeKitapYazarEslestirmeDizisinden(isbn_input, yazarId_input);
    printf("Kitap (ISBN: %s) ve Yazar (ID: %d) eslestirmesi kaldirildi.\n", isbn_input, yazarId_input);
    saveKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE);
}

void listeleKitabinYazarlari(KitapNode *kitapHead, yazarNode *yazarHead) {
    char isbn_input[14];
    printf("Yazarlarini listelemek istediginiz kitabin ISBN'ini giriniz: ");
    scanf(" %13s", isbn_input);
    while(getchar() != '\n');

    KitapNode* kitap = findKitapByISBN(kitapHead, isbn_input);
    if (!kitap) {
        printf("Hata: '%s' ISBN numarali kitap bulunamadi.\n", isbn_input);
        return;
    }

    printf("\n--- Kitap: %s (ISBN: %s) ---\n", kitap->data.kitapAdi, kitap->data.isbn);
    printf("Yazarlar:\n");
    int yazar_bulundu_mu = 0;
    for (int i = 0; i < g_kitapYazarEslestirmeSayisi; i++) {
        if (strcmp(g_kitapYazarEslestirmeleri[i].isbn, isbn_input) == 0) {
            yazarNode* yazar = findYazarByID(yazarHead, g_kitapYazarEslestirmeleri[i].yazarId);
            if (yazar) {
                printf("  ID: %d, Ad: %s, Soyad: %s\n", yazar->data.yazarId, yazar->data.yazarAdi, yazar->data.yazarSoyadi);
            } else if (g_kitapYazarEslestirmeleri[i].yazarId == -1) {
                printf("  ID: -1 (Yazar Silinmis)\n");
            } else {
                printf("  ID: %d (Yazar bilgisi bulunamadi - potansiyel veri tutarsizligi)\n", g_kitapYazarEslestirmeleri[i].yazarId);
            }
            yazar_bulundu_mu = 1;
        }
    }
    if (!yazar_bulundu_mu) {
        printf("Bu kitaba atanmis yazar bulunmamaktadir.\n");
    }
}

void listeleYazarinKitaplari(KitapNode *kitapHead, yazarNode *yazarHead) {
    int yazarId_input;
    printf("Kitaplarini listelemek istediginiz yazarin ID'sini giriniz: ");
    if (scanf("%d", &yazarId_input) != 1) {
        printf("Gecersiz yazar ID girisi.\n");
        while(getchar() != '\n');
        return;
    }
    while(getchar() != '\n');

    yazarNode* yazar = findYazarByID(yazarHead, yazarId_input);
    if (!yazar && yazarId_input != -1) { // -1 ID'si "silinmiþ yazar" için geçerli olabilir
        printf("Hata: '%d' ID'li yazar bulunamadi.\n", yazarId_input);
        return;
    }
    if (yazarId_input == -1) {
         printf("\n--- Yazar ID: -1 (Silinmis Yazarlara Ait Kitaplar) ---\n");
    } else {
        printf("\n--- Yazar: %s %s (ID: %d) ---\n", yazar->data.yazarAdi, yazar->data.yazarSoyadi, yazar->data.yazarId);
    }

    printf("Kitaplar:\n");
    int kitap_bulundu_mu = 0;
    for (int i = 0; i < g_kitapYazarEslestirmeSayisi; i++) {
        if (g_kitapYazarEslestirmeleri[i].yazarId == yazarId_input) {
            KitapNode* kitap = findKitapByISBN(kitapHead, g_kitapYazarEslestirmeleri[i].isbn);
            if (kitap) {
                printf("  ISBN: %s, Ad: %s\n", kitap->data.isbn, kitap->data.kitapAdi);
            } else {
                printf("  ISBN: %s (Kitap bilgisi bulunamadi - potansiyel veri tutarsizligi)\n", g_kitapYazarEslestirmeleri[i].isbn);
            }
            kitap_bulundu_mu = 1;
        }
    }
    if (!kitap_bulundu_mu) {
        printf("Bu yazara atanmis kitap bulunmamaktadir.\n");
    }
}

void listeleTumKitapYazarEslestirmeleri(KitapNode *kitapHead, yazarNode *yazarHead) {
    if (g_kitapYazarEslestirmeSayisi == 0) {
        printf("Kayitli Kitap-Yazar eslestirmesi bulunmamaktadir.\n");
        return;
    }
    printf("\n--- Tum Kitap-Yazar Eslestirmeleri ---\n");
    printf("%-15s %-30s %-10s %-30s\n", "ISBN", "Kitap Adi", "Yazar ID", "Yazar Adi Soyadi");
    printf("--------------------------------------------------------------------------------------\n");
    for (int i = 0; i < g_kitapYazarEslestirmeSayisi; i++) {
        KitapNode* kitap = findKitapByISBN(kitapHead, g_kitapYazarEslestirmeleri[i].isbn);
        yazarNode* yazar = findYazarByID(yazarHead, g_kitapYazarEslestirmeleri[i].yazarId);
        char kitap_adi_str[101] = "Kitap Bulunamadi";
        char yazar_ad_soyad_str[101] = "Yazar Bulunamadi";

        if (kitap) strncpy(kitap_adi_str, kitap->data.kitapAdi, 100);
        kitap_adi_str[100] = '\0';


        if (yazar) {
            snprintf(yazar_ad_soyad_str, 100, "%s %s", yazar->data.yazarAdi, yazar->data.yazarSoyadi);
        } else if (g_kitapYazarEslestirmeleri[i].yazarId == -1) {
            strcpy(yazar_ad_soyad_str, "(Yazar Silinmis)");
        }


        printf("%-15s %-30.30s %-10d %-30.30s\n",
               g_kitapYazarEslestirmeleri[i].isbn,
               kitap_adi_str,
               g_kitapYazarEslestirmeleri[i].yazarId,
               yazar_ad_soyad_str);
    }
}

// --- Yazar Fonksiyonlarý (yazarDeleter güncellendi) ---
// ... (createYazar, saveYazarlarToCSV, loadYazarlarFromCSV, yazarUpgrader, listYazarlar, freeYazarList önceki gibi)
// ... (önceki yazar fonksiyonlarý buraya eklenecek) ...
yazarNode *createYazar(int *yazarCount) {
    yazarNode *node = malloc(sizeof *node);
    if (!node) {
        perror("malloc createYazar");
        return NULL;
    }
    printf("Yazar adi: ");
    scanf(" %49s", node->data.yazarAdi);
    while(getchar() != '\n');
    printf("Yazar soyadi: ");
    scanf(" %49s", node->data.yazarSoyadi);
    while(getchar() != '\n');

    (*yazarCount)++;
    node->data.yazarId = *yazarCount;
    node->next = NULL;
    return node;
}

void saveYazarlarToCSV(const char *filename, yazarNode *head) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Yazar dosyasi acilirken hata: %s\n", strerror(errno));
        return;
    }
    fprintf(fp, "ID,Adi,Soyadi\n");
    for (yazarNode *it = head; it; it = it->next) {
        fprintf(fp, "%d,%s,%s\n",
                it->data.yazarId,
                it->data.yazarAdi,
                it->data.yazarSoyadi);
    }
    fclose(fp);
}

int loadYazarlarFromCSV(const char *filename, yazarNode **headPtr, yazarNode **tailPtr, int *yazarCount) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fp = fopen(filename, "w");
        if (!fp) {
            fprintf(stderr, "Yazar dosyasi olusturulamadi: %s\n", strerror(errno));
            return 0;
        }
        fprintf(fp, "ID,Adi,Soyadi\n");
        fclose(fp);
        *headPtr = NULL;
        *tailPtr = NULL;
        *yazarCount = 0;
        return 0;
    }
    char line[256];
    if (!fgets(line, sizeof line, fp)) { // Skip header
        fclose(fp);
        if (*headPtr == NULL) *tailPtr = NULL; 
        return 1;
    }
    *headPtr = NULL; *tailPtr = NULL; *yazarCount = 0; 
    while (fgets(line, sizeof line, fp)) {
        int id;
        char adi[50], soyadi[50];
        if (sscanf(line, "%d,%49[^,],%49[^\n]", &id, adi, soyadi) == 3) {
            yazarNode *node = malloc(sizeof *node);
            if (!node) {
                perror("malloc loadYazarlarFromCSV");
                continue;
            }
            node->data.yazarId = id;
            strncpy(node->data.yazarAdi, adi, sizeof(node->data.yazarAdi) - 1);
            node->data.yazarAdi[sizeof(node->data.yazarAdi) - 1] = '\0';
            
            char *nl = strrchr(soyadi, '\n');
            if (nl) *nl = '\0';

            strncpy(node->data.yazarSoyadi, soyadi, sizeof(node->data.yazarSoyadi) - 1);
            node->data.yazarSoyadi[sizeof(node->data.yazarSoyadi) - 1] = '\0';
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
    if (*headPtr == NULL) *tailPtr = NULL; 
    return 1;
}
void yazarDeleter(yazarNode **headPtr, yazarNode **tailPtr, int *yazarCount) {
    if (!*headPtr) {
        printf("Liste bos, silinecek yazar yok.\n");
        return;
    }
    char target_ad[50]; // Yazar adýyla silme
    printf("Silmek istediginiz yazar adini giriniz: ");
    scanf(" %49s", target_ad);
    while(getchar() != '\n');

    yazarNode *cur = *headPtr, *prev = NULL;
    while (cur && strcmp(cur->data.yazarAdi, target_ad) != 0) {
        prev = cur;
        cur = cur->next;
    }

    if (!cur) {
        printf("'%s' adli yazar bulunamadi.\n", target_ad);
        return;
    }

    int silinenYazarID = cur->data.yazarId;

    if (!prev) {
        *headPtr = cur->next;
        if (!*headPtr) *tailPtr = NULL;
    } else {
        prev->next = cur->next;
        if (!cur->next) *tailPtr = prev;
    }

    printf("Yazar '%s %s' (ID=%d) silindi.\n", cur->data.yazarAdi, cur->data.yazarSoyadi, silinenYazarID);
    free(cur);
    saveYazarlarToCSV(YAZAR_CSV_FILE, *headPtr);

    // KitapYazarEslestirmeleri'ni güncelle
    updateYazarIdInEslestirmeler(silinenYazarID, -1);
    saveKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE);
    printf("Kitap-Yazar eslestirmeleri guncellendi (Yazar ID: %d -> -1).\n", silinenYazarID);
}
void yazarUpgrader(yazarNode **headPtr) {
    if (!*headPtr) {
        printf("Liste bos, guncellenecek yazar yok.\n");
        return;
    }
    char target[50];
    printf("Duzenlemek istediginiz yazar adini giriniz: ");
    scanf(" %49s", target);
     while(getchar() != '\n'); 
    yazarNode *cur = *headPtr;
    while (cur && strcmp(cur->data.yazarAdi, target) != 0) cur = cur->next;
    if (!cur) {
        printf("'%s' adli yazar bulunamadi.\n", target);
        return;
    }
    int secim;
    printf("Degistirmek istediginiz ozelligi seciniz:\n");
    printf("1) Yazar adi\n2) Yazar soyadi\nSecim: ");
    if (scanf("%d", &secim) != 1) {
        printf("Gecersiz secim.\n");
        while(getchar()!='\n');
        return;
    }
     while(getchar() != '\n'); 

    if (secim == 1) {
        printf("Yeni yazar adini giriniz: ");
        scanf(" %49s", cur->data.yazarAdi);
         while(getchar() != '\n'); 
    } else if (secim == 2) {
        printf("Yeni yazar soyadini giriniz: ");
        scanf(" %49s", cur->data.yazarSoyadi);
         while(getchar() != '\n'); 
    } else {
        printf("Gecersiz secim.\n");
        return;
    }
    printf("Yazar (ID=%d) guncellendi: %s %s\n",
           cur->data.yazarId,
           cur->data.yazarAdi,
           cur->data.yazarSoyadi);
    saveYazarlarToCSV(YAZAR_CSV_FILE, *headPtr);
}

void listYazarlar(yazarNode *head) {
    if (!head) {
        printf("Yazar listesi bos.\n");
    } else {
        printf("\n--- Mevcut Yazarlar ---\n");
        printf("%-5s %-20s %-20s\n", "ID", "Adi", "Soyadi");
        printf("-------------------------------------------------\n");
        for (yazarNode *it = head; it; it = it->next) {
            printf("%-5d %-20s %-20s\n",
                   it->data.yazarId,
                   it->data.yazarAdi,
                   it->data.yazarSoyadi);
        }
    }
}

void freeYazarList(yazarNode *head) {
    while (head) {
        yazarNode *tmp = head;
        head = head->next;
        free(tmp);
    }
}
// --- Öðrenci Fonksiyonlarý (önceki gibi) ---
// ... (buraya eklenecek) ...
int isOgrenciNoUnique(unsigned int ogrenciNo, ogrenciNode *head) {
    for (ogrenciNode *current = head; current != NULL; current = current->next) {
        if (current->data.ogrenciNo == ogrenciNo) {
            return 0; 
        }
    }
    return 1; 
}

ogrenciNode *createOgrenciNode(ogrenciNode *currentHead) {
    ogrenciNode *node = malloc(sizeof(ogrenciNode));
    if (!node) {
        perror("malloc createOgrenciNode");
        return NULL;
    }

    printf("Ogrenci adi: ");
    scanf(" %49s", node->data.ogrenciAdi);
     while(getchar() != '\n');
    printf("Ogrenci soyadi: ");
    scanf(" %49s", node->data.ogrenciSoyadi);
     while(getchar() != '\n');

    unsigned int tempOgrenciNo;
    int isValidNo = 0;
    do {
        printf("Ogrenci No (8 haneli): ");
        if (scanf("%u", &tempOgrenciNo) != 1) {
            printf("Gecersiz giris. Sayi bekleniyor.\n");
            while (getchar() != '\n'); 
            continue;
        }
         while(getchar() != '\n');
        if (tempOgrenciNo < 10000000 || tempOgrenciNo > 99999999) {
            printf("Ogrenci No 8 haneli olmalidir (10000000 - 99999999).\n");
        } else if (!isOgrenciNoUnique(tempOgrenciNo, currentHead)) {
            printf("Bu Ogrenci Numarasi zaten kayitli. Lutfen farkli bir numara girin.\n");
        } else {
            isValidNo = 1;
        }
    } while (!isValidNo);
    node->data.ogrenciNo = tempOgrenciNo;

    node->data.kutupPuani = 100; 
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void saveOgrencilerToCSV(const char *filename, ogrenciNode *head) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Ogrenci dosyasi acilirken hata: %s\n", strerror(errno));
        return;
    }
    fprintf(fp, "OgrenciNo,Adi,Soyadi,KutupPuani\n");
    for (ogrenciNode *it = head; it; it = it->next) {
        fprintf(fp, "%u,%s,%s,%d\n",
                it->data.ogrenciNo,
                it->data.ogrenciAdi,
                it->data.ogrenciSoyadi,
                it->data.kutupPuani);
    }
    fclose(fp);
}

void loadOgrencilerFromCSV(const char *filename, ogrenciNode **headPtr, ogrenciNode **tailPtr) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fp = fopen(filename, "w");
        if (!fp) {
            fprintf(stderr, "Ogrenci dosyasi olusturulamadi: %s\n", strerror(errno));
            return;
        }
        fprintf(fp, "OgrenciNo,Adi,Soyadi,KutupPuani\n");
        fclose(fp);
        *headPtr = NULL; 
        *tailPtr = NULL;
        return; 
    }

    char line[256];
    if (!fgets(line, sizeof line, fp)) { 
        fclose(fp);
        if (*headPtr == NULL) *tailPtr = NULL;
        return;
    }

    *headPtr = NULL; 
    *tailPtr = NULL;

    while (fgets(line, sizeof line, fp)) {
        unsigned int no;
        char adi[50], soyadi[50];
        int puan;
        
        if (sscanf(line, "%u,%49[^,],%49[^,],%d", &no, adi, soyadi, &puan) == 4) {
            ogrenciNode *node = malloc(sizeof(ogrenciNode));
            if (!node) {
                perror("malloc loadOgrencilerFromCSV");
                continue;
            }
            node->data.ogrenciNo = no;
            strncpy(node->data.ogrenciAdi, adi, sizeof(node->data.ogrenciAdi) - 1);
            node->data.ogrenciAdi[sizeof(node->data.ogrenciAdi) - 1] = '\0';
            
            strncpy(node->data.ogrenciSoyadi, soyadi, sizeof(node->data.ogrenciSoyadi) - 1);
            node->data.ogrenciSoyadi[sizeof(node->data.ogrenciSoyadi) - 1] = '\0';
            
            node->data.kutupPuani = puan;
            node->next = NULL;
            node->prev = NULL;

            if (!*headPtr) {
                *headPtr = *tailPtr = node;
            } else {
                (*tailPtr)->next = node;
                node->prev = *tailPtr;
                *tailPtr = node;
            }
        }
    }
    fclose(fp);
    if (*headPtr == NULL) *tailPtr = NULL;
}

void ogrenciDeleter(ogrenciNode **headPtr, ogrenciNode **tailPtr) {
    if (!*headPtr) {
        printf("Ogrenci listesi bos, silinecek ogrenci yok.\n");
        return;
    }
    unsigned int targetNo;
    printf("Silmek istediginiz ogrencinin numarasini giriniz: ");
    if (scanf("%u", &targetNo) != 1) {
        printf("Gecersiz giris.\n");
        while(getchar() != '\n'); 
        return;
    }
     while(getchar() != '\n');

    ogrenciNode *current = *headPtr;
    while (current != NULL && current->data.ogrenciNo != targetNo) {
        current = current->next;
    }

    if (!current) {
        printf("'%u' numarali ogrenci bulunamadi.\n", targetNo);
        return;
    }

    if (current == *headPtr) { 
        *headPtr = current->next;
        if (*headPtr) {
            (*headPtr)->prev = NULL;
        } else { 
            *tailPtr = NULL;
        }
    } else if (current == *tailPtr) { 
        *tailPtr = current->prev;
        if (*tailPtr) { 
            (*tailPtr)->next = NULL;
        } else { 
            *headPtr = NULL; 
        }
    } else { 
        current->prev->next = current->next;
        current->next->prev = current->prev;
    }

    printf("Ogrenci '%s %s' (No: %u) silindi.\n",
           current->data.ogrenciAdi, current->data.ogrenciSoyadi, current->data.ogrenciNo);
    free(current);
    saveOgrencilerToCSV(OGRENCI_CSV_FILE, *headPtr);
}

void ogrenciUpgrader(ogrenciNode **headPtr, ogrenciNode **tailPtr) {
    if (!*headPtr) {
        printf("Ogrenci listesi bos, guncellenecek ogrenci yok.\n");
        return;
    }
    unsigned int targetNo;
    printf("Duzenlemek istediginiz ogrencinin numarasini giriniz: ");
     if (scanf("%u", &targetNo) != 1) {
        printf("Gecersiz giris.\n");
        while(getchar() != '\n'); 
        return;
    }
     while(getchar() != '\n');

    ogrenciNode *current = *headPtr;
    while (current != NULL && current->data.ogrenciNo != targetNo) {
        current = current->next;
    }

    if (!current) {
        printf("'%u' numarali ogrenci bulunamadi.\n", targetNo);
        return;
    }

    printf("Bulunan Ogrenci: %s %s, No: %u, Puan: %d\n", current->data.ogrenciAdi, current->data.ogrenciSoyadi, current->data.ogrenciNo, current->data.kutupPuani);
    printf("Degistirmek istediginiz ozelligi seciniz:\n");
    printf("1) Ogrenci adi\n");
    printf("2) Ogrenci soyadi\n");
    printf("3) Ogrenci No\n");
    printf("4) Kutup Puani\n");
    printf("Secim: ");
    int secim;
    if (scanf("%d", &secim) != 1) {
        printf("Gecersiz secim.\n");
        while(getchar()!='\n'); 
        return;
    }
     while(getchar()!='\n'); 

    switch (secim) {
        case 1:
            printf("Yeni ogrenci adini giriniz: ");
            scanf(" %49s", current->data.ogrenciAdi);
             while(getchar()!='\n'); 
            break;
        case 2:
            printf("Yeni ogrenci soyadini giriniz: ");
            scanf(" %49s", current->data.ogrenciSoyadi);
             while(getchar()!='\n'); 
            break;
        case 3: {
            unsigned int newOgrenciNo;
            int isValidNo = 0;
            do {
                printf("Yeni Ogrenci No (8 haneli): ");
                if (scanf("%u", &newOgrenciNo) != 1) {
                    printf("Gecersiz giris. Sayi bekleniyor.\n");
                    while (getchar() != '\n');
                    continue;
                }
                 while(getchar()!='\n'); 
                if (newOgrenciNo == current->data.ogrenciNo) { 
                     printf("Yeni numara mevcut numara ile ayni. Degisiklik yapilmadi.\n");
                     isValidNo = 1; 
                } else if (newOgrenciNo < 10000000 || newOgrenciNo > 99999999) {
                    printf("Ogrenci No 8 haneli olmalidir (10000000 - 99999999).\n");
                } else if (!isOgrenciNoUnique(newOgrenciNo, *headPtr)) { // Pass head of student list
                    printf("Bu Ogrenci Numarasi zaten kayitli. Lutfen farkli bir numara girin.\n");
                } else {
                    current->data.ogrenciNo = newOgrenciNo;
                    isValidNo = 1;
                }
            } while (!isValidNo);
            break;
        }
        case 4:
            printf("Yeni kutup puanini giriniz: ");
            if (scanf("%d", current->data.kutupPuani) != 1) {
                 printf("Gecersiz puan girisi.\n");
                 while(getchar()!='\n');
                 return; 
            }
             while(getchar()!='\n'); 
            break;
        default:
            printf("Gecersiz secim.\n");
            return;
    }
    printf("Ogrenci (No=%u) guncellendi: %s %s, Puan: %d\n",
           current->data.ogrenciNo,
           current->data.ogrenciAdi,
           current->data.ogrenciSoyadi,
           current->data.kutupPuani);
    saveOgrencilerToCSV(OGRENCI_CSV_FILE, *headPtr);
}

void listOgrenciler(ogrenciNode *head) {
    if (!head) {
        printf("Ogrenci listesi bos.\n");
    } else {
        printf("\n--- Mevcut Ogrenciler ---\n");
        printf("%-10s %-20s %-20s %-5s\n", "Ogr. No", "Adi", "Soyadi", "Puan");
        printf("------------------------------------------------------------\n");
        for (ogrenciNode *it = head; it; it = it->next) {
            printf("%-10u %-20s %-20s %-5d\n",
                   it->data.ogrenciNo,
                   it->data.ogrenciAdi,
                   it->data.ogrenciSoyadi,
                   it->data.kutupPuani);
        }
    }
}

void freeOgrenciList(ogrenciNode *head) {
    ogrenciNode *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}
// --- Kitap Fonksiyonlarý (kitapDeleter güncellendi) ---
// ... (createKitap, saveKitaplarToCSV, vb. önceki gibi) ...
// ... (önceki kitap fonksiyonlarý buraya eklenecek) ...
void freeKitapOrnegiList(KitapOrnegiNode *head) {
    KitapOrnegiNode *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

int isISBNValid(const char *isbn) {
    if (strlen(isbn) != 13) return 0;
    for (int i = 0; i < 13; i++) {
        if (!isdigit(isbn[i])) return 0;
    }
    return 1;
}

int isISBNUnique(const char *isbn, KitapNode *head) {
    for (KitapNode *current = head; current != NULL; current = current->next) {
        if (strcmp(current->data.isbn, isbn) == 0) {
            return 0; // Not unique
        }
    }
    return 1; // Unique
}

KitapNode *createKitap(KitapNode *kitapHeadGlobal) {
    KitapNode *newNode = malloc(sizeof(KitapNode));
    if (!newNode) {
        perror("malloc createKitapNode");
        return NULL;
    }
    newNode->data.ornekler_head = NULL;
    newNode->next = NULL;

    printf("Kitap Adi: ");
    scanf(" %99[^\n]", newNode->data.kitapAdi); 
    getchar(); 

    char tempIsbn[100]; 
    do {
        printf("ISBN (13 haneli): ");
        scanf(" %13s", tempIsbn);
        while(getchar()!='\n'); 
        if (!isISBNValid(tempIsbn)) {
            printf("Gecersiz ISBN formati. 13 rakam olmalidir.\n");
        } else if (!isISBNUnique(tempIsbn, kitapHeadGlobal)) {
            printf("Bu ISBN numarasi zaten kayitli.\n");
        } else {
            break; 
        }
    } while (1);
    strcpy(newNode->data.isbn, tempIsbn);

    int adetInput;
    do {
        printf("Adet: ");
        if (scanf("%d", &adetInput) != 1 || adetInput <= 0) {
            printf("Gecersiz adet. Pozitif bir sayi giriniz.\n");
            while(getchar()!='\n'); 
        } else {
             while(getchar()!='\n'); 
            break;
        }
    } while(1);
    newNode->data.adet = adetInput;

    KitapOrnegiNode *ornek_tail = NULL;
    for (int i = 1; i <= newNode->data.adet; ++i) {
        KitapOrnegiNode *ornekNode = malloc(sizeof(KitapOrnegiNode));
        if (!ornekNode) {
            perror("malloc KitapOrnegiNode");
            freeKitapOrnegiList(newNode->data.ornekler_head);
            free(newNode);
            return NULL;
        }
        sprintf(ornekNode->data.etiketNo, "%s_%d", newNode->data.isbn, i);
        strcpy(ornekNode->data.durum, "RAFTA");
        ornekNode->next = NULL;

        if (!newNode->data.ornekler_head) {
            newNode->data.ornekler_head = ornek_tail = ornekNode;
        } else {
            ornek_tail->next = ornekNode;
            ornek_tail = ornekNode;
        }
    }
    return newNode;
}

void saveKitaplarToCSV(const char *filename, KitapNode *head) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Kitaplar CSV dosyasi acilirken hata: %s\n", strerror(errno));
        return;
    }
    fprintf(fp, "KitapAdi,ISBN,Adet\n");
    for (KitapNode *it = head; it; it = it->next) {
        fprintf(fp, "%s,%s,%d\n",
                it->data.kitapAdi,
                it->data.isbn,
                it->data.adet);
    }
    fclose(fp);
}

void saveKitapOrnekleriToCSV(const char *filename_ornekler, KitapNode *head_kitaplar) {
    FILE *fp = fopen(filename_ornekler, "w");
    if (!fp) {
        fprintf(stderr, "Kitap Ornekleri CSV dosyasi acilirken hata: %s\n", strerror(errno));
        return;
    }
    fprintf(fp, "EtiketNo,Durum,AnaISBN\n");
    if (head_kitaplar) { 
        for (KitapNode *kitap_it = head_kitaplar; kitap_it; kitap_it = kitap_it->next) {
            for (KitapOrnegiNode *ornek_it = kitap_it->data.ornekler_head; ornek_it; ornek_it = ornek_it->next) {
                fprintf(fp, "%s,%s,%s\n",
                        ornek_it->data.etiketNo,
                        ornek_it->data.durum,
                        kitap_it->data.isbn); 
            }
        }
    }
    fclose(fp);
}

void loadKitaplarFromCSV(const char *filename, KitapNode **headPtr, KitapNode **tailPtr) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fp = fopen(filename, "w"); 
        if (!fp) {
            fprintf(stderr, "Kitaplar CSV dosyasi olusturulamadi: %s\n", strerror(errno));
            return;
        }
        fprintf(fp, "KitapAdi,ISBN,Adet\n");
        fclose(fp);
        *headPtr = NULL;
        *tailPtr = NULL;
        return;
    }

    char line[512]; 
    if (!fgets(line, sizeof line, fp)) { 
        fclose(fp);
        if (*headPtr == NULL) *tailPtr = NULL;
        return;
    }

    *headPtr = NULL;
    *tailPtr = NULL;

    while (fgets(line, sizeof line, fp)) {
        char kitapAdi[100], isbn[14];
        int adet;

        if (sscanf(line, "%99[^,],%13[^,],%d", kitapAdi, isbn, &adet) == 3) {
            KitapNode *node = malloc(sizeof(KitapNode));
            if (!node) {
                perror("malloc loadKitaplarFromCSV");
                continue;
            }
            strncpy(node->data.kitapAdi, kitapAdi, sizeof(node->data.kitapAdi) - 1);
            node->data.kitapAdi[sizeof(node->data.kitapAdi) - 1] = '\0';
            strncpy(node->data.isbn, isbn, sizeof(node->data.isbn) - 1);
            node->data.isbn[sizeof(node->data.isbn) - 1] = '\0';
            node->data.adet = adet;
            node->data.ornekler_head = NULL; 
            node->next = NULL;

            if (!*headPtr) {
                *headPtr = *tailPtr = node;
            } else {
                (*tailPtr)->next = node;
                *tailPtr = node;
            }
        }
    }
    fclose(fp);
    if (*headPtr == NULL) *tailPtr = NULL;
}

void loadKitapOrnekleriFromCSV(const char *filename_ornekler, KitapNode *kitapHead) {
    FILE *fp = fopen(filename_ornekler, "r");
    if (!fp) {
        fp = fopen(filename_ornekler, "w"); 
        if (!fp) {
            fprintf(stderr, "Kitap Ornekleri CSV dosyasi olusturulamadi: %s\n", strerror(errno));
            return;
        }
        fprintf(fp, "EtiketNo,Durum,AnaISBN\n");
        fclose(fp);
        return;
    }

    char line[256];
    if (!fgets(line, sizeof line, fp)) { 
        fclose(fp);
        return;
    }

    while (fgets(line, sizeof line, fp)) {
        char etiketNo[25], durum[20], anaISBN[14];

        if (sscanf(line, "%24[^,],%19[^,],%13[^\n]", etiketNo, durum, anaISBN) == 3) {
            KitapNode *parentKitap = kitapHead;
            while (parentKitap) {
                if (strcmp(parentKitap->data.isbn, anaISBN) == 0) {
                    break;
                }
                parentKitap = parentKitap->next;
            }

            if (parentKitap) {
                KitapOrnegiNode *ornekNode = malloc(sizeof(KitapOrnegiNode));
                if (!ornekNode) {
                    perror("malloc loadKitapOrnekleriFromCSV");
                    continue;
                }
                strncpy(ornekNode->data.etiketNo, etiketNo, sizeof(ornekNode->data.etiketNo) -1);
                ornekNode->data.etiketNo[sizeof(ornekNode->data.etiketNo)-1] = '\0';
                strncpy(ornekNode->data.durum, durum, sizeof(ornekNode->data.durum)-1);
                ornekNode->data.durum[sizeof(ornekNode->data.durum)-1] = '\0';
                ornekNode->next = NULL;

                if (!parentKitap->data.ornekler_head) {
                    parentKitap->data.ornekler_head = ornekNode;
                } else {
                    KitapOrnegiNode *temp_tail = parentKitap->data.ornekler_head;
                    while (temp_tail->next) {
                        temp_tail = temp_tail->next;
                    }
                    temp_tail->next = ornekNode;
                }
            } 
        }
    }
    fclose(fp);
}
void kitapDeleter(KitapNode **headPtr, KitapNode **tailPtr) {
    if (!*headPtr) {
        printf("Kitap listesi bos.\n");
        return;
    }

    char targetISBN[14];
    printf("Kopyasini silmek istediginiz kitabin ISBN numarasini giriniz: ");
    scanf(" %13s", targetISBN);
    while (getchar() != '\n');

    KitapNode *kitap_to_modify_prev = NULL;
    KitapNode *kitap_to_modify = *headPtr;
    while (kitap_to_modify && strcmp(kitap_to_modify->data.isbn, targetISBN) != 0) {
        kitap_to_modify_prev = kitap_to_modify;
        kitap_to_modify = kitap_to_modify->next;
    }

    if (!kitap_to_modify) {
        printf("ISBN '%s' ile kitap bulunamadi.\n", targetISBN);
        return;
    }

    if (!kitap_to_modify->data.ornekler_head || kitap_to_modify->data.adet == 0) {
        printf("Bu kitabin ('%s') silinecek kopyasi bulunmamaktadir.\n", kitap_to_modify->data.kitapAdi);
        if (kitap_to_modify->data.adet == 0 && !kitap_to_modify->data.ornekler_head) {
             printf("Kitabin hic kopyasi olmadigindan ana kayit da siliniyor...\n");
             char silinenKitapISBN[14];
             strcpy(silinenKitapISBN, kitap_to_modify->data.isbn);

             if (!kitap_to_modify_prev) {
                *headPtr = kitap_to_modify->next;
                if (!*headPtr) *tailPtr = NULL;
            } else {
                kitap_to_modify_prev->next = kitap_to_modify->next;
                if (!kitap_to_modify->next) *tailPtr = kitap_to_modify_prev;
            }
            free(kitap_to_modify);
            removeKitapYazarEslestirmeleriByISBN(silinenKitapISBN); // Kitap-Yazar eþleþtirmelerini sil
            saveKitaplarToCSV(KITAP_CSV_FILE, *headPtr);
            saveKitapOrnekleriToCSV(KITAP_ORNEKLERI_CSV_FILE, *headPtr);
            saveKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE); // Eþleþtirmeleri kaydet
        }
        return;
    }

    printf("Kitap: '%s' (ISBN: %s) - Kopyalar:\n", kitap_to_modify->data.kitapAdi, kitap_to_modify->data.isbn);
    int kopya_sayac = 0;
    for (KitapOrnegiNode *ornek_iter = kitap_to_modify->data.ornekler_head; ornek_iter; ornek_iter = ornek_iter->next) {
        kopya_sayac++;
        printf("  %d. Etiket: %-20s Durum: %s\n", kopya_sayac, ornek_iter->data.etiketNo, ornek_iter->data.durum);
    }
     if (kopya_sayac == 0) { 
         printf("Bu kitabin silinecek kopyasi bulunmamaktadir.\n");
         return;
    }

    char targetEtiketNo[25];
    printf("Silmek istediginiz kopyanin Etiket Numarasini giriniz (örn: %s_1): ", targetISBN);
    scanf(" %24s", targetEtiketNo);
    while (getchar() != '\n');

    KitapOrnegiNode *ornek_prev = NULL;
    KitapOrnegiNode *ornek_to_delete = kitap_to_modify->data.ornekler_head;
    while (ornek_to_delete && strcmp(ornek_to_delete->data.etiketNo, targetEtiketNo) != 0) {
        ornek_prev = ornek_to_delete;
        ornek_to_delete = ornek_to_delete->next;
    }

    if (!ornek_to_delete) {
        printf("Etiket No '%s' ile kopya bulunamadi.\n", targetEtiketNo);
        return;
    }

    if (strcmp(ornek_to_delete->data.durum, "RAFTA") != 0) {
        printf("Kopya '%s' su anda RAFTA degil (Durum: %s). Silinemez.\n", targetEtiketNo, ornek_to_delete->data.durum);
        return;
    }

    if (!ornek_prev) {
        kitap_to_modify->data.ornekler_head = ornek_to_delete->next;
    } else {
        ornek_prev->next = ornek_to_delete->next;
    }
    printf("Kopya '%s' basariyla silindi.\n", ornek_to_delete->data.etiketNo);
    free(ornek_to_delete);
    kitap_to_modify->data.adet--;

    if (kitap_to_modify->data.adet == 0) {
        printf("Kitabin ('%s') tum kopyalari silindi. Ana kitap kaydi da sistemden kaldiriliyor.\n", kitap_to_modify->data.kitapAdi);
        char silinenKitapISBN[14];
        strcpy(silinenKitapISBN, kitap_to_modify->data.isbn);

        if (!kitap_to_modify_prev) {
            *headPtr = kitap_to_modify->next;
            if (!*headPtr) *tailPtr = NULL;
        } else {
            kitap_to_modify_prev->next = kitap_to_modify->next;
            if (!kitap_to_modify->next) *tailPtr = kitap_to_modify_prev;
        }
        free(kitap_to_modify);
        removeKitapYazarEslestirmeleriByISBN(silinenKitapISBN); // Kitap-Yazar eþleþtirmelerini sil
        saveKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE);    // Eþleþtirmeleri kaydet
    }

    saveKitaplarToCSV(KITAP_CSV_FILE, *headPtr);
    saveKitapOrnekleriToCSV(KITAP_ORNEKLERI_CSV_FILE, *headPtr);
}
void kitapUpgrader(KitapNode **headPtr, KitapNode **tailPtr) {
    if (!*headPtr) {
        printf("Kitap listesi bos.\n");
        return;
    }
    char targetISBN[14];
    printf("Duzenlemek istediginiz kitabin ISBN numarasini giriniz: ");
    scanf(" %13s", targetISBN);
    while(getchar() != '\n');

    KitapNode *current = *headPtr;
    while (current && strcmp(current->data.isbn, targetISBN) != 0) {
        current = current->next;
    }

    if (!current) {
        printf("ISBN '%s' ile kitap bulunamadi.\n", targetISBN);
        return;
    }

    printf("Duzenlenecek Kitap: %s, ISBN: %s, Adet: %d\n", current->data.kitapAdi, current->data.isbn, current->data.adet);
    printf("Degistirmek istediginiz ozellik:\n");
    printf("1) Kitap Adi\n");
    printf("2) Adet\n");
    printf("Secim: ");
    int secim;
    if (scanf("%d", &secim) != 1) {
        printf("Gecersiz secim.\n");
        while(getchar() != '\n');
        return;
    }
    while(getchar() != '\n'); 

    if (secim == 1) {
        printf("Yeni Kitap Adi: ");
        scanf(" %99[^\n]", current->data.kitapAdi);
        getchar(); 
    } else if (secim == 2) {
        int newAdet;
        printf("Yeni Adet: ");
        if (scanf("%d", &newAdet) != 1 || newAdet < 0) { 
            printf("Gecersiz adet.\n");
            while(getchar() != '\n');
            return;
        }
        while(getchar() != '\n');

        if (newAdet > current->data.adet) { 
            int diff = newAdet - current->data.adet;
            int max_copy_index = 0;
            KitapOrnegiNode *ornek_iter = current->data.ornekler_head;
            KitapOrnegiNode *ornek_list_tail = NULL;

            if(ornek_iter){ 
                ornek_list_tail = ornek_iter;
                while(ornek_list_tail->next){
                    ornek_list_tail = ornek_list_tail->next;
                }
                ornek_iter = current->data.ornekler_head; 
                while(ornek_iter){
                    const char *last_underscore = strrchr(ornek_iter->data.etiketNo, '_');
                    if(last_underscore){
                        int current_index = atoi(last_underscore + 1);
                        if(current_index > max_copy_index) max_copy_index = current_index;
                    }
                    ornek_iter = ornek_iter->next;
                }
            }

            for (int i = 0; i < diff; ++i) {
                KitapOrnegiNode *newOrnek = malloc(sizeof(KitapOrnegiNode));
                if (!newOrnek) {
                    perror("malloc newOrnek kitapUpgrader");
                    return;
                }
                max_copy_index++; 
                sprintf(newOrnek->data.etiketNo, "%s_%d", current->data.isbn, max_copy_index);
                strcpy(newOrnek->data.durum, "RAFTA");
                newOrnek->next = NULL;
                if (!current->data.ornekler_head) {
                    current->data.ornekler_head = ornek_list_tail = newOrnek;
                } else {
                    ornek_list_tail->next = newOrnek;
                    ornek_list_tail = newOrnek;
                }
            }
            current->data.adet = newAdet;
        } else if (newAdet < current->data.adet) { 
            int diff_remove = current->data.adet - newAdet;
            int rafta_count = 0;
            for (KitapOrnegiNode *ornek_iter = current->data.ornekler_head; ornek_iter; ornek_iter = ornek_iter->next) {
                if (strcmp(ornek_iter->data.durum, "RAFTA") == 0) {
                    rafta_count++;
                }
            }
            if (rafta_count < diff_remove) {
                printf("Yeterli sayida 'RAFTA' kopya yok. Adet azaltilamiyor. (Silinecek: %d, Rafta: %d)\n", diff_remove, rafta_count);
                return; 
            }
            
            for (int i = 0; i < diff_remove; ) {
                KitapOrnegiNode *ornek_cur = current->data.ornekler_head;
                KitapOrnegiNode *ornek_prev_ptr = NULL; 
                int removed_one_in_iteration = 0;

                while(ornek_cur){
                    if(strcmp(ornek_cur->data.durum, "RAFTA") == 0){
                        if(ornek_prev_ptr == NULL){ 
                            current->data.ornekler_head = ornek_cur->next;
                        } else {
                            ornek_prev_ptr->next = ornek_cur->next;
                        }
                        printf("Otomatik silinen RAFTA kopya: %s\n", ornek_cur->data.etiketNo);
                        free(ornek_cur);
                        i++; 
                        removed_one_in_iteration = 1;
                        break; 
                    }
                    ornek_prev_ptr = ornek_cur;
                    ornek_cur = ornek_cur->next;
                }
                 if (!removed_one_in_iteration && i < diff_remove) {
                    printf("Hata: Beklenen sayýda RAFTA kopya silinemedi. (i=%d, diff_remove=%d)\n", i, diff_remove);
                    current->data.adet -= i; 
                    saveKitaplarToCSV(KITAP_CSV_FILE, *headPtr);
                    saveKitapOrnekleriToCSV(KITAP_ORNEKLERI_CSV_FILE, *headPtr);
                    return; 
                }
            }
            current->data.adet = newAdet; 
        }
        if (newAdet == 0 && current->data.ornekler_head != NULL) {
             // Adet 0 ama hala örnekler var (hepsi ödünç alýnmýþ). Bu normal.
        } else if (newAdet == 0 && current->data.ornekler_head == NULL) {
            printf("Kitabin adedi 0'a dusuruldu ve hic kopyasi kalmadi.\n");
            // Ýsteðe baðlý: Ana kitabý silmek için kitapDeleter benzeri mantýk çaðrýlabilir
            // veya kullanýcýya sorulabilir. Þimdilik sadece adet güncelleniyor.
            // Ancak, bu durum kitapDeleter tarafýndan daha iyi yönetilir.
        }
    } else {
        printf("Gecersiz secim.\n");
        return;
    }
    printf("Kitap guncellendi: %s, ISBN: %s, Adet: %d\n", current->data.kitapAdi, current->data.isbn, current->data.adet);
    saveKitaplarToCSV(KITAP_CSV_FILE, *headPtr);
    saveKitapOrnekleriToCSV(KITAP_ORNEKLERI_CSV_FILE, *headPtr);
}


void listKitaplarDetail(KitapNode *head) {
    if (!head) {
        printf("Kitap listesi bos.\n");
        return;
    }
    printf("\n--- Mevcut Kitaplar ve Kopyalari ---\n");
    for (KitapNode *kitap_it = head; kitap_it; kitap_it = kitap_it->next) {
        printf("--------------------------------------------------------------------\n");
        printf("Kitap Adi: %s\n", kitap_it->data.kitapAdi);
        printf("ISBN     : %s\n", kitap_it->data.isbn);
        printf("Toplam Adet: %d\n", kitap_it->data.adet);
        printf("Kopyalar:\n");
        if (!kitap_it->data.ornekler_head) {
            printf("  (Bu kitaba ait kopya bulunmamaktadir.)\n");
        } else {
            printf("  %-20s %-15s\n", "Etiket No", "Durum");
            printf("  -----------------------------------\n");
            for (KitapOrnegiNode *ornek_it = kitap_it->data.ornekler_head; ornek_it; ornek_it = ornek_it->next) {
                printf("  %-20s %-15s\n", ornek_it->data.etiketNo, ornek_it->data.durum);
            }
        }
    }
    printf("--------------------------------------------------------------------\n");
}

void freeKitapList(KitapNode *head) {
    KitapNode *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        freeKitapOrnegiList(tmp->data.ornekler_head); 
        free(tmp);
    }
}
// --- Menü Fonksiyonlarý ---
// ... (yazarMenu, ogrenciMenu, kitapMenu önceki gibi, ana menü güncellenecek)
void yazarMenu(yazarNode **yazarHeadPtr, yazarNode **yazarTailPtr, int *yazarCountPtr) {
    int choice;
    do {
        printf("\n=== Yazar Islemleri Menu ===\n");
        printf("1. Yazar Ekle\n");
        printf("2. Yazar Sil\n");
        printf("3. Mevcut Yazarlar\n");
        printf("4. Yazar Guncelle\n");
        printf("0. Ana Menu'ye Don\n");
        printf("Seciminiz: ");
        if (scanf("%d", &choice) != 1) {
            printf("Gecersiz giris. Sayi bekleniyor.\n");
            while (getchar() != '\n'); 
            choice = -1; 
            continue;
        }
        while (getchar() != '\n'); 

        switch (choice) {
            case 1: {
                yazarNode *newNode = createYazar(yazarCountPtr);
                if (newNode) {
                    if (!*yazarHeadPtr) {
                         *yazarHeadPtr = *yazarTailPtr = newNode;
                    } else {
                        (*yazarTailPtr)->next = newNode;
                        *yazarTailPtr = newNode;
                    }
                    printf("Yazar eklendi: %s %s (ID=%d)\n",
                           newNode->data.yazarAdi,
                           newNode->data.yazarSoyadi,
                           newNode->data.yazarId);
                    saveYazarlarToCSV(YAZAR_CSV_FILE, *yazarHeadPtr);
                }
                break;
            }
            case 2:
                yazarDeleter(yazarHeadPtr, yazarTailPtr, yazarCountPtr);
                break;
            case 3:
                listYazarlar(*yazarHeadPtr);
                break;
            case 4:
                yazarUpgrader(yazarHeadPtr);
                break;
            case 0:
                printf("Ana menu'ye donuluyor...\n");
                break;
            default:
                printf("Gecersiz secim, tekrar deneyin.\n");
        }
    } while (choice != 0);
}

void ogrenciMenu(ogrenciNode **ogrenciHeadPtr, ogrenciNode **ogrenciTailPtr) {
    int choice;
    do {
        printf("\n=== Ogrenci Islemleri Menu ===\n");
        printf("1. Ogrenci Ekle\n");
        printf("2. Ogrenci Sil\n");
        printf("3. Mevcut Ogrenciler\n");
        printf("4. Ogrenci Guncelle\n");
        printf("0. Ana Menu'ye Don\n");
        printf("Seciminiz: ");
        if (scanf("%d", &choice) != 1) {
            printf("Gecersiz giris. Sayi bekleniyor.\n");
            while (getchar() != '\n'); 
            choice = -1; 
            continue;
        }
        while (getchar() != '\n'); 

        switch (choice) {
            case 1: {
                ogrenciNode *newNode = createOgrenciNode(*ogrenciHeadPtr); 
                if (newNode) {
                    if (!*ogrenciHeadPtr) {
                        *ogrenciHeadPtr = *ogrenciTailPtr = newNode;
                    } else {
                        (*ogrenciTailPtr)->next = newNode;
                        newNode->prev = *ogrenciTailPtr;
                        *ogrenciTailPtr = newNode;
                    }
                    printf("Ogrenci eklendi: %s %s (No=%u, Puan=%d)\n",
                           newNode->data.ogrenciAdi,
                           newNode->data.ogrenciSoyadi,
                           newNode->data.ogrenciNo,
                           newNode->data.kutupPuani);
                    saveOgrencilerToCSV(OGRENCI_CSV_FILE, *ogrenciHeadPtr);
                }
                break;
            }
            case 2:
                ogrenciDeleter(ogrenciHeadPtr, ogrenciTailPtr);
                break;
            case 3:
                listOgrenciler(*ogrenciHeadPtr);
                break;
            case 4:
                ogrenciUpgrader(ogrenciHeadPtr, ogrenciTailPtr);
                break;
            case 0:
                printf("Ana menu'ye donuluyor...\n");
                break;
            default:
                printf("Gecersiz secim, tekrar deneyin.\n");
        }
    } while (choice != 0);
}

void kitapMenu(KitapNode **kitapHeadPtr, KitapNode **kitapTailPtr, ogrenciNode *ogrenciListHead) {
    int choice;
    do {
        printf("\n=== Kitap Islemleri Menu ===\n");
        printf("1. Kitap Ekle\n");
        printf("2. Kitap Kopyasi Sil\n"); 
        printf("3. Kitaplari Listele (Detayli)\n");
        printf("4. Kitap Guncelle\n");
        printf("0. Ana Menu'ye Don\n");
        printf("Seciminiz: ");
        if (scanf("%d", &choice) != 1) {
            printf("Gecersiz giris. Sayi bekleniyor.\n");
            while (getchar() != '\n');
            choice = -1;
            continue;
        }
        while (getchar() != '\n'); 

        switch (choice) {
            case 1: {
                KitapNode *newNode = createKitap(*kitapHeadPtr);
                if (newNode) {
                    if (!*kitapHeadPtr) {
                        *kitapHeadPtr = *kitapTailPtr = newNode;
                    } else {
                        (*kitapTailPtr)->next = newNode;
                        *kitapTailPtr = newNode;
                    }
                    printf("Kitap '%s' (ISBN: %s) %d adet kopya ile eklendi.\n",
                           newNode->data.kitapAdi, newNode->data.isbn, newNode->data.adet);
                    saveKitaplarToCSV(KITAP_CSV_FILE, *kitapHeadPtr);
                    saveKitapOrnekleriToCSV(KITAP_ORNEKLERI_CSV_FILE, *kitapHeadPtr);
                }
                break;
            }
            case 2:
                kitapDeleter(kitapHeadPtr, kitapTailPtr);
                break;
            case 3:
                listKitaplarDetail(*kitapHeadPtr);
                break;
            case 4:
                kitapUpgrader(kitapHeadPtr, kitapTailPtr);
                break;
            case 0:
                printf("Ana menu'ye donuluyor...\n");
                break;
            default:
                printf("Gecersiz secim, tekrar deneyin.\n");
        }
    } while (choice != 0);
}
void kitapYazarMenu(KitapNode *kitapHead, yazarNode *yazarHead) {
    int choice;
    do {
        printf("\n=== Kitap-Yazar Eslestirme Islemleri ===\n");
        printf("1. Kitap-Yazar Eslestir\n");
        printf("2. Kitap-Yazar Eslestirmesini Kaldir\n");
        printf("3. Bir Kitabin Yazarlarini Listele\n");
        printf("4. Bir Yazarin Kitaplarini Listele\n");
        printf("5. Tum Eslestirmeleri Listele\n");
        printf("0. Ana Menu'ye Don\n");
        printf("Seciminiz: ");
        if (scanf("%d", &choice) != 1) {
            printf("Gecersiz giris. Sayi bekleniyor.\n");
            while (getchar() != '\n');
            choice = -1;
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                eslestirKitapYazar(kitapHead, yazarHead);
                break;
            case 2:
                kaldirKitapYazarEslestirmesi(kitapHead, yazarHead);
                break;
            case 3:
                listeleKitabinYazarlari(kitapHead, yazarHead);
                break;
            case 4:
                listeleYazarinKitaplari(kitapHead, yazarHead);
                break;
            case 5:
                listeleTumKitapYazarEslestirmeleri(kitapHead, yazarHead);
                break;
            case 0:
                printf("Ana menu'ye donuluyor...\n");
                break;
            default:
                printf("Gecersiz secim, tekrar deneyin.\n");
        }
    } while (choice != 0);
}


// --- Main Function ---
int main(void) {
    int yazarCount = 0;
    yazarNode *yazarHead = NULL, *yazarTail = NULL;
    ogrenciNode *ogrenciHead = NULL, *ogrenciTail = NULL;
    KitapNode *kitapHead = NULL, *kitapTail = NULL;

    initKitapYazarDizisi(); // Dinamik diziyi baþlat

    loadYazarlarFromCSV(YAZAR_CSV_FILE, &yazarHead, &yazarTail, &yazarCount);
    loadOgrencilerFromCSV(OGRENCI_CSV_FILE, &ogrenciHead, &ogrenciTail);
    loadKitaplarFromCSV(KITAP_CSV_FILE, &kitapHead, &kitapTail);
    if(kitapHead) {
        loadKitapOrnekleriFromCSV(KITAP_ORNEKLERI_CSV_FILE, kitapHead);
    }
    loadKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE); // Eþleþtirmeleri yükle

    int anaMenuChoice;
    do {
        printf("\n=========== ANA MENU ===========\n");
        printf("1. Yazar Islemleri\n");
        printf("2. Ogrenci Islemleri\n");
        printf("3. Kitap Islemleri\n");
        printf("4. Kitap-Yazar Eslestirme Islemleri\n"); // Yeni menü seçeneði
        printf("0. Cikis\n");
        printf("Seciminiz: ");
        if (scanf("%d", &anaMenuChoice) != 1) {
            printf("Gecersiz giris. Sayi bekleniyor.\n");
            while (getchar() != '\n');
            anaMenuChoice = -1;
            continue;
        }
        while (getchar() != '\n');

        switch (anaMenuChoice) {
            case 1:
                yazarMenu(&yazarHead, &yazarTail, &yazarCount);
                break;
            case 2:
                ogrenciMenu(&ogrenciHead, &ogrenciTail);
                break;
            case 3:
                kitapMenu(&kitapHead, &kitapTail, ogrenciHead);
                break;
            case 4:
                kitapYazarMenu(kitapHead, yazarHead); // Yeni menüyü çaðýr
                break;
            case 0:
                printf("Programdan cikiliyor...\n");
                break;
            default:
                printf("Gecersiz secim, tekrar deneyin.\n");
        }
    } while (anaMenuChoice != 0);

    freeYazarList(yazarHead);
    freeOgrenciList(ogrenciHead);
    freeKitapList(kitapHead);
    freeKitapYazarDizisi(); // Dinamik diziyi serbest býrak

    return 0;
}
