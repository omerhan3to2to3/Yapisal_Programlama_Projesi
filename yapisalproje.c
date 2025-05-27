#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#define YAZAR_CSV_FILE "yazarlar.csv"
#define OGRENCI_CSV_FILE "Ogrenciler.csv"
#define KITAP_CSV_FILE "Kitaplar.csv"
#define KITAP_ORNEKLERI_CSV_FILE "KitapOrnekleri.csv"
#define KITAP_YAZAR_CSV_FILE "KitapYazar.csv"
#define ODUNC_ISLEMLERI_CSV_FILE "OduncIslemleri.csv"

// Struct definitions
typedef struct yazar {
    char yazarAdi[50];
    char yazarSoyadi[50];
    int  yazarId;
} yazar;

typedef struct ogrenci{
	char ogrenciAdi[50];
	char ogrenciSoyadi[50];
	int kutupPuani;
	unsigned int ogrenciNo;
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

typedef struct KitapYazarEslestirme {
    char isbn[14];
    int yazarId;
} KitapYazarEslestirme;

typedef struct OduncIslemi {
    unsigned int ogrenciNo;
    char kitapEtiketNo[25];
    int islemTuru;
    char tarih[11];
} OduncIslemi;

// --- Ana Veri Yapýsý ---
typedef struct KutuphaneVeri {
    yazarNode *yazarHead;
    yazarNode *yazarTail;
    int yazarCount;

    ogrenciNode *ogrenciHead;
    ogrenciNode *ogrenciTail;

    KitapNode *kitapHead;
    KitapNode *kitapTail;

    KitapYazarEslestirme *kitapYazarEslestirmeleri;
    int kitapYazarEslestirmeSayisi;
    int kitapYazarEslestirmeKapasitesi;

    OduncIslemi *oduncIslemleri;
    int oduncIslemiSayisi;
    int oduncIslemiKapasitesi;
} KutuphaneVeri;


// --- Fonksiyon Prototipleri ---
// Helper Fonksiyon Prototipleri
KitapNode* findKitapByISBN(KitapNode* head, const char* isbn);
yazarNode* findYazarByID(yazarNode* head, int id);
ogrenciNode* findOgrenciByNo(ogrenciNode* head, unsigned int no);
KitapOrnegiNode* findKitapOrnegiByEtiket(KitapNode* kitapHead, const char* etiketNo, KitapNode** parentKitapPtr);

// Yazar Fonksiyon Prototipleri
yazarNode *createYazar(KutuphaneVeri *veri);
void saveYazarlarToCSV(const char *filename, const KutuphaneVeri *veri);
void loadYazarlarFromCSV(const char *filename, KutuphaneVeri *veri);
void yazarDeleter(KutuphaneVeri *veri);
void yazarUpgrader(KutuphaneVeri *veri);
void listYazarlar(const KutuphaneVeri *veri);
void freeYazarList(yazarNode *head); // Bu doðrudan liste baþýný alabilir
void yazarMenu(KutuphaneVeri *veri);

// Ogrenci Fonksiyon Prototipleri
void cezaliOgrencileriListele(const KutuphaneVeri *veri);
void kitapTeslimEtmemisOgrencileriListele(const KutuphaneVeri *veri);
int isOgrenciNoUnique(unsigned int ogrenciNo, const KutuphaneVeri *veri);
ogrenciNode *createOgrenciNode(const KutuphaneVeri *veri);
void saveOgrencilerToCSV(const char *filename, const KutuphaneVeri *veri);
void loadOgrencilerFromCSV(const char *filename, KutuphaneVeri *veri);
void ogrenciDeleter(KutuphaneVeri *veri);
void ogrenciUpgrader(KutuphaneVeri *veri);
void listOgrenciler(const KutuphaneVeri *veri);
void freeOgrenciList(ogrenciNode *head); // Bu doðrudan liste baþýný alabilir
void ogrenciMenu(KutuphaneVeri *veri);
void listeleOgrencininHareketleri(unsigned int ogrenciNo, const KutuphaneVeri *veri, const KutuphaneVeri *tumVeri);
void goruntuleOgrenciDetay(ogrenciNode *ogr, const KutuphaneVeri *veri, const KutuphaneVeri *tumVeri);
void goruntuleOgrenciByID(KutuphaneVeri *veri);
void goruntuleOgrenciByAdSoyad(KutuphaneVeri *veri);
void ogrenciBilgisiGoruntuleMenu(KutuphaneVeri *veri);

// Kitap Fonksiyon Prototipleri
void zamanindaTeslimEdilmeyenKitaplariListele(const KutuphaneVeri *veri);
void raftakiKitaplariListele(const KutuphaneVeri *veri);
void goruntuleTekKitapDetay(KitapNode *kitap); // Yardýmcý fonksiyon
void goruntuleKitapByAd(const char* arananAd, KutuphaneVeri *veri);
void kitapBilgisiGoruntuleMenu(KutuphaneVeri *veri);
void freeKitapOrnegiList(KitapOrnegiNode *head);
int isISBNValid(const char *isbn);
int isISBNUnique(const char *isbn, const KutuphaneVeri *veri);
KitapNode *createKitap(const KutuphaneVeri *veri);
void saveKitaplarToCSV(const char *filename, const KutuphaneVeri *veri);
void saveKitapOrnekleriToCSV(const char *filename_ornekler, const KutuphaneVeri *veri);
void loadKitaplarFromCSV(const char *filename, KutuphaneVeri *veri);
void loadKitapOrnekleriFromCSV(const char *filename_ornekler, KutuphaneVeri *veri);
void kitapDeleter(KutuphaneVeri *veri);
void kitapUpgrader(KutuphaneVeri *veri);
void listKitaplarDetail(const KutuphaneVeri *veri);
void freeKitapList(KitapNode *head); // Bu doðrudan liste baþýný alabilir
void kitapMenu(KutuphaneVeri *veri);

// Kitap-Yazar Eþleþtirme Fonksiyon Prototipleri
void kitabinYazariniGuncelle(KutuphaneVeri *veri);
void initKitapYazarDizisi(KutuphaneVeri *veri);
int addKitapYazarEslestirmeDizisine(KutuphaneVeri *veri, const char* isbn, int yazarId);
void removeKitapYazarEslestirmeDizisinden(KutuphaneVeri *veri, const char* isbn, int yazarId);
void removeKitapYazarEslestirmeleriByISBN(KutuphaneVeri *veri, const char* isbn);
void updateYazarIdInEslestirmeler(KutuphaneVeri *veri, int eskiYazarId, int yeniYazarId);
void loadKitapYazarEslestirmeleri(const char *filename, KutuphaneVeri *veri);
void saveKitapYazarEslestirmeleri(const char *filename, const KutuphaneVeri *veri);
void freeKitapYazarDizisi(KutuphaneVeri *veri);
void eslestirKitapYazar(KutuphaneVeri *veri);
void kaldirKitapYazarEslestirmesi(KutuphaneVeri *veri);
void listeleKitabinYazarlari(const KutuphaneVeri *veri);
void listeleYazarinKitaplari(const KutuphaneVeri *veri);
void listeleTumKitapYazarEslestirmeleri(const KutuphaneVeri *veri);
void kitapYazarMenu(KutuphaneVeri *veri);

// Odunc Islemleri Fonksiyon Prototipleri
void getCurrentDate(char *date_str);
int parseDate(const char *date_str, int *day, int *month, int *year);
int calculateDayDifference(int d1, int m1, int y1, int d2, int m2, int y2);
void initOduncIslemleriDizisi(KutuphaneVeri *veri);
int addOduncIslemiDizisine(KutuphaneVeri *veri, unsigned int ogrenciNo, const char* kitapEtiketNo, int islemTuru, const char* tarih);
void loadOduncIslemleri(const char *filename, KutuphaneVeri *veri);
void saveOduncIslemleri(const char *filename, const KutuphaneVeri *veri);
void freeOduncIslemleriDizisi(KutuphaneVeri *veri);
void oduncAlKitap(KutuphaneVeri *veri);
void teslimEtKitap(KutuphaneVeri *veri);
void listeleTumOduncIslemleri(const KutuphaneVeri *veri);
void oduncIslemleriMenu(KutuphaneVeri *veri);

// --- Helper Fonksiyon Tanýmlarý ---
// ... (Bu fonksiyonlar KutuphaneVeri* almaz, doðrudan liste baþlarýyla çalýþýr) ...
// ... (Önceki yanýttaki findKitapByISBN, findYazarByID, findOgrenciByNo, findKitapOrnegiByEtiket tanýmlarý buraya) ...
KitapNode* findKitapByISBN(KitapNode* head, const char* isbn) {
    for (KitapNode* current = head; current != NULL; current = current->next) {
        if (strcmp(current->data.isbn, isbn) == 0) {
            return current;
        }
    }
    return NULL;
}

yazarNode* findYazarByID(yazarNode* head, int id) {
    if (id == -1) return NULL;
    for (yazarNode* current = head; current != NULL; current = current->next) {
        if (current->data.yazarId == id) {
            return current;
        }
    }
    return NULL;
}

ogrenciNode* findOgrenciByNo(ogrenciNode* head, unsigned int no) {
    for (ogrenciNode* current = head; current != NULL; current = current->next) {
        if (current->data.ogrenciNo == no) {
            return current;
        }
    }
    return NULL;
}

KitapOrnegiNode* findKitapOrnegiByEtiket(KitapNode* kitapHead, const char* etiketNo, KitapNode** parentKitapPtr) {
    if (parentKitapPtr) *parentKitapPtr = NULL;
    for (KitapNode* k_node = kitapHead; k_node != NULL; k_node = k_node->next) {
        for (KitapOrnegiNode* ko_node = k_node->data.ornekler_head; ko_node != NULL; ko_node = ko_node->next) {
            if (strcmp(ko_node->data.etiketNo, etiketNo) == 0) {
                if (parentKitapPtr) *parentKitapPtr = k_node;
                return ko_node;
            }
        }
    }
    return NULL;
}


// --- Tarih Fonksiyonlarý ---
// ... (Önceki yanýttaki getCurrentDate, parseDate, calculateDayDifference tanýmlarý buraya) ...
void getCurrentDate(char *date_str) {
    time_t t = time(NULL);
    struct tm tm_info = *localtime(&t);
    sprintf(date_str, "%02d.%02d.%d", tm_info.tm_mday, tm_info.tm_mon + 1, tm_info.tm_year + 1900);
}

int parseDate(const char *date_str, int *day, int *month, int *year) {
    if (sscanf(date_str, "%d.%d.%d", day, month, year) == 3) {
        if (*day >= 1 && *day <= 31 && *month >= 1 && *month <= 12 && *year > 1900 && *year < 2200) {
            return 1;
        }
    }
    return 0;
}

int calculateDayDifference(int d1, int m1, int y1, int d2, int m2, int y2) {
    struct tm date1_tm = {0}, date2_tm = {0};
    time_t time1_t, time2_t;

    date1_tm.tm_year = y1 - 1900; date1_tm.tm_mon = m1 - 1; date1_tm.tm_mday = d1;
    date2_tm.tm_year = y2 - 1900; date2_tm.tm_mon = m2 - 1; date2_tm.tm_mday = d2;

    time1_t = mktime(&date1_tm);
    time2_t = mktime(&date2_tm);

    if (time1_t == (time_t)(-1) || time2_t == (time_t)(-1)) {
        return -9999; // Hata durumu için özel bir deðer
    }
    double difference_seconds = difftime(time2_t, time1_t);
    return (int)(difference_seconds / (60 * 60 * 24));
}


// --- Yazar Fonksiyon Tanýmlarý ---
yazarNode *createYazar(KutuphaneVeri *veri) {
    yazarNode *node = malloc(sizeof *node);
    if (!node) {
        perror("malloc createYazar");
        return NULL;
    }
    printf("Yazar adi: ");
    scanf(" %49[^\n]", node->data.yazarAdi);
    while(getchar() != '\n');
    printf("Yazar soyadi: ");
    scanf(" %49[^\n]", node->data.yazarSoyadi);
    while(getchar() != '\n');

    (veri->yazarCount)++;
    node->data.yazarId = veri->yazarCount;
    node->next = NULL;
    return node;
}

void saveYazarlarToCSV(const char *filename, const KutuphaneVeri *veri) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Yazar dosyasi acilirken hata: %s\n", strerror(errno));
        return;
    }
    fprintf(fp, "ID,Adi,Soyadi\n");
    for (yazarNode *it = veri->yazarHead; it; it = it->next) {
        fprintf(fp, "%d,%s,%s\n",
                it->data.yazarId,
                it->data.yazarAdi,
                it->data.yazarSoyadi);
    }
    fclose(fp);
}

void loadYazarlarFromCSV(const char *filename, KutuphaneVeri *veri) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fp = fopen(filename, "w");
        if (!fp) {
            fprintf(stderr, "Yazar dosyasi olusturulamadi: %s\n", strerror(errno));
            return;
        }
        fprintf(fp, "ID,Adi,Soyadi\n");
        fclose(fp);
        veri->yazarHead = NULL;
        veri->yazarTail = NULL;
        veri->yazarCount = 0;
        return;
    }
    char line[256];
    if (!fgets(line, sizeof line, fp)) {
        fclose(fp);
        if (veri->yazarHead == NULL){
		veri->yazarTail = NULL;
		}
        return;
    }
    veri->yazarHead = NULL; 
	veri->yazarTail = NULL; 
	veri->yazarCount = 0;
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

            if (!veri->yazarHead) {
                veri->yazarHead = veri->yazarTail = node;
            } else {
                (veri->yazarTail)->next = node;
                veri->yazarTail = node;
            }
            if (id > veri->yazarCount) veri->yazarCount = id;
        }
    }
    fclose(fp);
    if (veri->yazarHead == NULL){
    veri->yazarTail = NULL;
	}
}

void yazarDeleter(KutuphaneVeri *veri) {
    if (!veri->yazarHead) {
        printf("Liste bos, silinecek yazar yok.\n");
        return;
    }
    char target_ad[50];
    printf("Silmek istediginiz yazar adini giriniz: ");
    scanf(" %49[^\n	]", target_ad);
    while(getchar() != '\n');

    yazarNode *cur = veri->yazarHead, *prev = NULL;
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
        veri->yazarHead = cur->next;
        if (!veri->yazarHead) veri->yazarTail = NULL;
    } else {
        prev->next = cur->next;
        if (!cur->next) veri->yazarTail = prev;
    }

    printf("Yazar '%s %s' (ID=%d) silindi.\n", cur->data.yazarAdi, cur->data.yazarSoyadi, silinenYazarID);
    free(cur);
    saveYazarlarToCSV(YAZAR_CSV_FILE, veri);

    updateYazarIdInEslestirmeler(veri, silinenYazarID, -1);
    saveKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE, veri);
    printf("Kitap-Yazar eslestirmeleri guncellendi (Yazar ID: %d -> -1).\n", silinenYazarID);
}

void yazarUpgrader(KutuphaneVeri *veri) {
    if (!veri->yazarHead) {
        printf("Liste bos, guncellenecek yazar yok.\n");
        return;
    }
    char target[50];
    printf("Duzenlemek istediginiz yazar adini giriniz: ");
    scanf(" %49[^\n]", target);
     while(getchar() != '\n');
    yazarNode *cur = veri->yazarHead;
    while (cur && strcmp(cur->data.yazarAdi, target) != 0){
		cur = cur->next;
		}
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
        scanf(" %49[^\n]", cur->data.yazarAdi);
         while(getchar() != '\n');
    } else if (secim == 2) {
        printf("Yeni yazar soyadini giriniz: ");
        scanf(" %49[^\n]", cur->data.yazarSoyadi);
         while(getchar() != '\n');
    } else {
        printf("Gecersiz secim.\n");
        return;
    }
    printf("Yazar (ID=%d) guncellendi: %s %s\n",
           cur->data.yazarId,
           cur->data.yazarAdi,
           cur->data.yazarSoyadi);
    saveYazarlarToCSV(YAZAR_CSV_FILE, veri);
}

void listYazarlar(const KutuphaneVeri *veri) {
    if (!veri->yazarHead) {
        printf("Yazar listesi bos.\n");
    } else {
        printf("\n--- Mevcut Yazarlar ---\n");
        printf("%-5s %-20s %-20s\n", "ID", "Adi", "Soyadi");
        printf("-------------------------------------------------\n");
        for (yazarNode *it = veri->yazarHead; it; it = it->next) {
            printf("%-5d %-20s %-20s\n",
                   it->data.yazarId,
                   it->data.yazarAdi,
                   it->data.yazarSoyadi);
        }
    }
}

void freeYazarList(yazarNode *head) {
    yazarNode *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}



// --- Ogrenci Fonksiyon Tanýmlarý ---

void cezaliOgrencileriListele(const KutuphaneVeri *veri) {
    if (!veri->ogrenciHead) {
        printf("Sistemde kayitli ogrenci bulunmamaktadir.\n");
        return;
    }

    printf("\n--- Cezali Ogrenciler (Kutup Puani Negatif Olanlar) ---\n");
    printf("%-10s %-20s %-20s %-5s\n", "Ogr. No", "Adi", "Soyadi", "Puan");
    printf("------------------------------------------------------------\n");

    int cezaliOgrenciBulundu = 0;
    ogrenciNode *current = veri->ogrenciHead;

    while (current != NULL) {
        if (current->data.kutupPuani < 0) {
            printf("%-10u %-20s %-20s %-5d\n",
                   current->data.ogrenciNo,
                   current->data.ogrenciAdi,
                   current->data.ogrenciSoyadi,
                   current->data.kutupPuani);
            cezaliOgrenciBulundu = 1;
        }
        current = current->next;
    }

    if (!cezaliOgrenciBulundu) {
        printf("Sistemde cezali durumda (puani negatif) ogrenci bulunmamaktadir.\n");
    }
    printf("------------------------------------------------------------\n");
}

void kitapTeslimEtmemisOgrencileriListele(const KutuphaneVeri *veri) {
    if (veri->oduncIslemiSayisi == 0 && veri->kitapHead == NULL) { // Hem ödünç iþlemi hem de kitap yoksa daha kesin olur
        printf("Sistemde odunc alinabilecek kitap veya odunc kaydi bulunmamaktadir.\n");
        return;
    }

    printf("\n--- Kitap Teslim Etmemis Ogrenciler ve Kitaplari ---\n");
    printf("%-10s %-25s %-20s %-15s\n", "Ogr. No", "Ogrenci Ad Soyad", "Kitap Etiket No", "Odunc Tarihi");
    printf("----------------------------------------------------------------------------\n");

    int teslimEdilmemisKitapBulundu = 0;

    for (KitapNode *k_node = veri->kitapHead; k_node != NULL; k_node = k_node->next) {
        for (KitapOrnegiNode *ko_node = k_node->data.ornekler_head; ko_node != NULL; ko_node = ko_node->next) {
            if (strcmp(ko_node->data.durum, "RAFTA") != 0) {
                unsigned int oduncAlanOgrenciNo = 0;
                if (sscanf(ko_node->data.durum, "%u", &oduncAlanOgrenciNo) == 1 && oduncAlanOgrenciNo > 0) {
                    ogrenciNode *ogr = findOgrenciByNo(veri->ogrenciHead, oduncAlanOgrenciNo);
                    char ogr_ad_soyad[101] = "Ogrenci Bulunamadi";
                    if (ogr) {
                        snprintf(ogr_ad_soyad, 100, "%s %s", ogr->data.ogrenciAdi, ogr->data.ogrenciSoyadi);
                        ogr_ad_soyad[100] = '\0';
                    }

                    char sonOduncTarihi[11] = "Bilinmiyor";
                    for (int j = veri->oduncIslemiSayisi - 1; j >= 0; j--) {
                        if ((*(veri->oduncIslemleri + j)).ogrenciNo == oduncAlanOgrenciNo &&
                            strcmp((*(veri->oduncIslemleri + j)).kitapEtiketNo, ko_node->data.etiketNo) == 0 &&
                            (*(veri->oduncIslemleri + j)).islemTuru == 0) {
                            strcpy(sonOduncTarihi, (*(veri->oduncIslemleri + j)).tarih);
                            break; 
                        }
                    }

                    printf("%-10u %-25.25s %-20s %-15s\n",
                           oduncAlanOgrenciNo,
                           ogr_ad_soyad,
                           ko_node->data.etiketNo,
                           sonOduncTarihi);
                    teslimEdilmemisKitapBulundu = 1;
                }
            }
        }
    }

    if (!teslimEdilmemisKitapBulundu) {
        printf("Su anda odunc alinmis ve teslim edilmemis kitap bulunmamaktadir.\n");
    }
    printf("----------------------------------------------------------------------------\n");
}

void listeleOgrencininHareketleri(unsigned int ogrenciNo, const KutuphaneVeri *veriOdunc, const KutuphaneVeri *veriKitap) {
    printf("  Kitap Hareketleri:\n");
    int hareketBulundu = 0;
    printf("    %-20s %-15s %-12s\n", "Kitap Etiket No", "Islem Turu", "Tarih");
    printf("    -----------------------------------------------------\n");

    for (int i = 0; i < veriOdunc->oduncIslemiSayisi; i++) {
        if ((*(veriOdunc->oduncIslemleri + i)).ogrenciNo == ogrenciNo) {
            char islemTuruStr[15];
            if ((*(veriOdunc->oduncIslemleri + i)).islemTuru == 0) {
                strcpy(islemTuruStr, "Odunc Alma");
            } else {
                strcpy(islemTuruStr, "Teslim Etme");
            }
            printf("    %-20s %-15s %-12s\n",
                   (*(veriOdunc->oduncIslemleri + i)).kitapEtiketNo,
                   islemTuruStr,
                   (*(veriOdunc->oduncIslemleri + i)).tarih);
            hareketBulundu = 1;
        }
    }
    if (!hareketBulundu) {
        printf("    Bu ogrenciye ait kitap hareketi bulunmamaktadir.\n");
    }
}

void goruntuleOgrenciDetay(ogrenciNode *ogr, const KutuphaneVeri *veri, const KutuphaneVeri *tumVeriForKitapHareketleri) {
    if (ogr) {
        printf("\n--- Ogrenci Bilgileri ---\n");
        printf("  Ad Soyad : %s %s\n", ogr->data.ogrenciAdi, ogr->data.ogrenciSoyadi);
        printf("  Numara   : %u\n", ogr->data.ogrenciNo);
        printf("  Puan     : %d\n", ogr->data.kutupPuani);
        listeleOgrencininHareketleri(ogr->data.ogrenciNo, veri, tumVeriForKitapHareketleri);
        printf("---------------------------\n");
    }
}

void goruntuleOgrenciByID(KutuphaneVeri *veri) {
    unsigned int id_input;
    printf("Bilgilerini goruntulemek istediginiz ogrencinin ID (Numara) bilgisini giriniz: ");
    if (scanf("%u", &id_input) != 1) {
        printf("Gecersiz ID girisi.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    ogrenciNode *ogr = findOgrenciByNo(veri->ogrenciHead, id_input);
    if (ogr) {
        goruntuleOgrenciDetay(ogr, veri, veri); // tumVeriForKitapHareketleri için de veri'yi kullanýyoruz
    } else {
        printf("'%u' ID'li ogrenci bulunamadi.\n", id_input);
    }
}

void goruntuleOgrenciByAdSoyad(KutuphaneVeri *veri) {
    char ad_input[50];
    char soyad_input[50];

    printf("Bilgilerini goruntulemek istediginiz ogrencinin adini giriniz: ");
    scanf(" %49s", ad_input); // Baþýna boþluk, önceki newline'larý atlar
    while (getchar() != '\n');

    printf("Soyadini giriniz: ");
    scanf(" %49s", soyad_input);
    while (getchar() != '\n');

    ogrenciNode *bulunanlar[100]; // Maksimum 100 eþleþen öðrenci varsayýmý (dinamik de yapýlabilir)
    int bulunan_sayisi = 0;
    ogrenciNode *current = veri->ogrenciHead;

    while (current != NULL) {
        if (strcmp(current->data.ogrenciAdi, ad_input) == 0 && strcmp(current->data.ogrenciSoyadi, soyad_input) == 0) {
            if (bulunan_sayisi < 100) {
                bulunanlar[bulunan_sayisi++] = current;
            } else {
                printf("Uyari: Cok fazla eslesen ogrenci bulundu, sadece ilk 100 tanesi listelenecek.\n");
                return;
            }
        }
        current = current->next;
    }

    if (bulunan_sayisi == 0) {
        printf("'%s %s' ad soyadina sahip ogrenci bulunamadi.\n", ad_input, soyad_input);
    } else if (bulunan_sayisi == 1) {
        goruntuleOgrenciDetay(bulunanlar[0], veri, veri);
    } else {
        printf("\nBirden fazla '%s %s' adinda ogrenci bulundu. Lutfen birini secin:\n", ad_input, soyad_input);
        for (int i = 0; i < bulunan_sayisi; i++) {
            printf("%d. ID: %u, Puan: %d\n", i + 1, bulunanlar[i]->data.ogrenciNo, bulunanlar[i]->data.kutupPuani);
        }
        int secim;
        printf("Seciminiz (1-%d): ", bulunan_sayisi);
        if (scanf("%d", &secim) == 1 && secim >= 1 && secim <= bulunan_sayisi) {
            while (getchar() != '\n');
            goruntuleOgrenciDetay(bulunanlar[secim - 1], veri, veri);
        } else {
            printf("Gecersiz secim.\n");
            while (getchar() != '\n');
        }
    }
}

int isOgrenciNoUnique(unsigned int ogrenciNo, const KutuphaneVeri *veri) {
    for (ogrenciNode *current = veri->ogrenciHead; current != NULL; current = current->next) {
        if (current->data.ogrenciNo == ogrenciNo) {
            return 0;
        }
    }
    return 1;
}

ogrenciNode *createOgrenciNode(const KutuphaneVeri *veri) {
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
        } else if (!isOgrenciNoUnique(tempOgrenciNo, veri)) {
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

void saveOgrencilerToCSV(const char *filename, const KutuphaneVeri *veri) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Ogrenci dosyasi acilirken hata: %s\n", strerror(errno));
        return;
    }
    fprintf(fp, "OgrenciNo,Adi,Soyadi,KutupPuani\n");
    for (ogrenciNode *it = veri->ogrenciHead; it; it = it->next) {
        fprintf(fp, "%u,%s,%s,%d\n",
                it->data.ogrenciNo,
                it->data.ogrenciAdi,
                it->data.ogrenciSoyadi,
                it->data.kutupPuani);
    }
    fclose(fp);
}
//sscanfi kontrol et
void loadOgrencilerFromCSV(const char *filename, KutuphaneVeri *veri) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fp = fopen(filename, "w");
        if (!fp) {
            fprintf(stderr, "Ogrenci dosyasi olusturulamadi: %s\n", strerror(errno));
            return;
        }
        fprintf(fp, "OgrenciNo,Adi,Soyadi,KutupPuani\n");
        fclose(fp);
        veri->ogrenciHead = NULL;
        veri->ogrenciTail = NULL;
        return;
    }

    char line[256];
    if (!fgets(line, sizeof line, fp)) {
        fclose(fp);
        if (veri->ogrenciHead == NULL) { 
            veri->ogrenciTail = NULL;
        }
        return; 
    }

    veri->ogrenciHead = NULL;
    veri->ogrenciTail = NULL;

    while (fgets(line, sizeof line, fp)) {
        unsigned int no_val;
        char adi[50], soyadi[50];
        int puan;

        if (sscanf(line, "%u,%49[^,],%49[^,],%d", &no_val, adi, soyadi, &puan) == 4) {
            ogrenciNode *node = malloc(sizeof(ogrenciNode));

            if (node) {
                node->data.ogrenciNo = no_val;
                strncpy(node->data.ogrenciAdi, adi, sizeof(node->data.ogrenciAdi) - 1);
                node->data.ogrenciAdi[sizeof(node->data.ogrenciAdi) - 1] = '\0';

                char *nl = strrchr(soyadi, '\n');
                if (nl) *nl = '\0';
                
                strncpy(node->data.ogrenciSoyadi, soyadi, sizeof(node->data.ogrenciSoyadi) - 1);
                node->data.ogrenciSoyadi[sizeof(node->data.ogrenciSoyadi) - 1] = '\0';

                node->data.kutupPuani = puan;
                node->next = NULL;
                node->prev = NULL;

                if (!veri->ogrenciHead) {
                    veri->ogrenciHead = veri->ogrenciTail = node;
                } else { // Listede eleman varsa, sona ekle
                    (veri->ogrenciTail)->next = node;
                    node->prev = veri->ogrenciTail;
                    veri->ogrenciTail = node;
                }
            } else {
                perror("malloc loadOgrencilerFromCSV");
            }
        }
    }
    fclose(fp);
    if (veri->ogrenciHead == NULL) {
        veri->ogrenciTail = NULL;
    }
}

void ogrenciDeleter(KutuphaneVeri *veri) {
    if (!veri->ogrenciHead) {
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

    ogrenciNode *current = veri->ogrenciHead;
    while (current != NULL && current->data.ogrenciNo != targetNo) {
        current = current->next;
    }

    if (!current) {
        printf("'%u' numarali ogrenci bulunamadi.\n", targetNo);
        return;
    }

    if (current == veri->ogrenciHead) {
        veri->ogrenciHead = current->next;
        if (veri->ogrenciHead) {
            (veri->ogrenciHead)->prev = NULL;
        } else {
            veri->ogrenciTail = NULL;
        }
    } else if (current == veri->ogrenciTail) {
        veri->ogrenciTail = current->prev;
        if (veri->ogrenciTail) {
            (veri->ogrenciTail)->next = NULL;
        } else {
            veri->ogrenciHead = NULL;
        }
    } else {
        current->prev->next = current->next;
        current->next->prev = current->prev;
    }

    printf("Ogrenci '%s %s' (No: %u) silindi.\n",
           current->data.ogrenciAdi, current->data.ogrenciSoyadi, current->data.ogrenciNo);
    free(current);
    saveOgrencilerToCSV(OGRENCI_CSV_FILE, veri);
}

void ogrenciUpgrader(KutuphaneVeri *veri) {
    if (!veri->ogrenciHead) {
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

    ogrenciNode *current = veri->ogrenciHead;
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
            scanf(" %49[^\n]", current->data.ogrenciAdi);
             while(getchar()!='\n');
            break;
        case 2:
            printf("Yeni ogrenci soyadini giriniz: ");
            scanf(" %49[^\n]", current->data.ogrenciSoyadi);
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
                } else if (!isOgrenciNoUnique(newOgrenciNo, veri)) {
                    printf("Bu Ogrenci Numarasi zaten kayitli. Lutfen farkli bir numara girin.\n");
                } else {
                    current->data.ogrenciNo = newOgrenciNo;
                    isValidNo = 1;
                }
            } while (!isValidNo);
            break;
        }
        default:
            printf("Gecersiz secim.\n");
            return;
    }
    printf("Ogrenci (No=%u) guncellendi: %s %s, Puan: %d\n",
           current->data.ogrenciNo,
           current->data.ogrenciAdi,
           current->data.ogrenciSoyadi,
           current->data.kutupPuani);
    saveOgrencilerToCSV(OGRENCI_CSV_FILE, veri);
}

void listOgrenciler(const KutuphaneVeri *veri) {
    if (!veri->ogrenciHead) {
        printf("Ogrenci listesi bos.\n");
    } else {
        printf("\n--- Mevcut Ogrenciler ---\n");
        printf("%-10s %-20s %-20s %-5s\n", "Ogr. No", "Adi", "Soyadi", "Puan");
        printf("------------------------------------------------------------\n");
        for (ogrenciNode *it = veri->ogrenciHead; it; it = it->next) {
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
    for (size_t i = 0; i < strlen(isbn) ; i++) {
        if (!isdigit(isbn[i])) return 0;
    }
    return 1;
}

int isISBNUnique(const char *isbn, const KutuphaneVeri *veri) {
    for (KitapNode *current = veri->kitapHead; current != NULL; current = current->next) {
        if (strcmp(current->data.isbn, isbn) == 0) {
            return 0;
        }
    }
    return 1;
}

KitapNode *createKitap(const KutuphaneVeri *veri) {
    KitapNode *newNode = malloc(sizeof(KitapNode));
    KitapNode *resultNode = NULL; // Dönecek deðeri tutmak için

    if (!newNode) {
        perror("malloc createKitapNode");
    } else {
        newNode->data.ornekler_head = NULL;
        newNode->next = NULL;

        printf("Kitap Adi: ");
        scanf(" %99[^\n]", newNode->data.kitapAdi);
        while(getchar()!='\n'); 

        char tempIsbn[100];
        int isbnGecerliMi = 0; 

        while (!isbnGecerliMi) {
            printf("ISBN (13 haneli): ");
            scanf(" %13s", tempIsbn);
            while(getchar()!='\n'); // Tamponu temizle

            if (!isISBNValid(tempIsbn)) {
                printf("Gecersiz ISBN formati. 13 rakam olmalidir.\n");
            } else if (!isISBNUnique(tempIsbn, veri)) { 
                printf("Bu ISBN numarasi zaten kayitli.\n");
            } else {
                isbnGecerliMi = 1; 
            }
        }
        strcpy(newNode->data.isbn, tempIsbn);

        int adetInput;
        int adetGecerliMi = 0; 

        while(!adetGecerliMi) {
            printf("Adet: ");
            if (scanf("%d", &adetInput) != 1 || adetInput <= 0) {
                printf("Gecersiz adet. Pozitif bir sayi giriniz.\n");
                while(getchar()!='\n');
            } else {
                while(getchar()!='\n');
                adetGecerliMi = 1; 
            }
        }
        newNode->data.adet = adetInput;

        KitapOrnegiNode *ornek_tail = NULL;
        int ornekOlusturmaBasarili = 1; // Örnek oluþturma döngüsü için bayrak

        for (int i = 1; i <= newNode->data.adet && ornekOlusturmaBasarili; ++i) {
            KitapOrnegiNode *ornekNode = malloc(sizeof(KitapOrnegiNode));
            if (!ornekNode) {
                perror("malloc KitapOrnegiNode");
                freeKitapOrnegiList(newNode->data.ornekler_head); // Önceden ayrýlanlarý serbest býrak
                free(newNode); // Ana düðümü serbest býrak
                newNode = NULL; // newNode'u NULL yaparak en sondaki return'un NULL dönmesini saðla
                ornekOlusturmaBasarili = 0; // Döngüyü ve iþlemi sonlandýr
            } else {
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
        }

        if (ornekOlusturmaBasarili) {
            resultNode = newNode;
        }
    } 
    return resultNode; // En sonda tek bir return noktasý
}

void raftakiKitaplariListele(const KutuphaneVeri *veri) {
    if (!veri->kitapHead) {
        printf("Sistemde kayitli kitap bulunmamaktadir.\n");
        return;
    }

    printf("\n--- Raftaki Kitaplar ve Kopyalari ---\n");
    int raftaKitapBulunduGenel = 0; // Genel olarak rafta kitap bulundu mu?

    for (KitapNode *kitap_it = veri->kitapHead; kitap_it; kitap_it = kitap_it->next) {
        int buKitaptaRaftaKopyaVar = 0; // Bu spesifik kitabýn rafta kopyasý var mý?
        KitapOrnegiNode *raftakiKopyalarHead = NULL; // Bu kitaba ait raftaki kopyalarý geçici tutmak için
        KitapOrnegiNode *raftakiKopyalarTail = NULL;

        for (KitapOrnegiNode *ornek_it = kitap_it->data.ornekler_head; ornek_it; ornek_it = ornek_it->next) {
            if (strcmp(ornek_it->data.durum, "RAFTA") == 0) {
                buKitaptaRaftaKopyaVar = 1;
                raftaKitapBulunduGenel = 1; // En az bir tane rafta kitap bulundu
            }
        }

        if (buKitaptaRaftaKopyaVar) {
            printf("--------------------------------------------------------------------\n");
            printf("Kitap Adi: %s\n", kitap_it->data.kitapAdi);
            printf("ISBN     : %s\n", kitap_it->data.isbn);
            printf("Raftaki Kopyalar:\n");
            printf("  %-20s\n", "Etiket No");
            printf("  --------------------\n");

            for (KitapOrnegiNode *ornek_it = kitap_it->data.ornekler_head; ornek_it; ornek_it = ornek_it->next) {
                if (strcmp(ornek_it->data.durum, "RAFTA") == 0) {
                    printf("  %-20s\n", ornek_it->data.etiketNo);
                }
            }
        }
    }

    if (!raftaKitapBulunduGenel) {
        printf("Su anda rafta bulunan hicbir kitap kopyasi yoktur.\n");
    }
    printf("--------------------------------------------------------------------\n");
}

void saveKitaplarToCSV(const char *filename, const KutuphaneVeri *veri) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Kitaplar CSV dosyasi acilirken hata: %s\n", strerror(errno));
        return;
    }
    fprintf(fp, "KitapAdi,ISBN,Adet\n");
    for (KitapNode *it = veri->kitapHead; it; it = it->next) {
        fprintf(fp, "%s,%s,%d\n",
                it->data.kitapAdi,
                it->data.isbn,
                it->data.adet);
    }
    fclose(fp);
}

void saveKitapOrnekleriToCSV(const char *filename_ornekler, const KutuphaneVeri *veri) {
    FILE *fp = fopen(filename_ornekler, "w");
    if (!fp) {
        fprintf(stderr, "Kitap Ornekleri CSV dosyasi acilirken hata: %s\n", strerror(errno));
        return;
    }
    fprintf(fp, "EtiketNo,Durum,AnaISBN\n");
    if (veri->kitapHead) {
        for (KitapNode *kitap_it = veri->kitapHead; kitap_it; kitap_it = kitap_it->next) {
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

void loadKitaplarFromCSV(const char *filename, KutuphaneVeri *veri) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fp = fopen(filename, "w");
        if (!fp) {
            fprintf(stderr, "Kitaplar CSV dosyasi olusturulamadi: %s\n", strerror(errno));
            return;
        }
        fprintf(fp, "KitapAdi,ISBN,Adet\n");
        fclose(fp);
        veri->kitapHead = NULL;
        veri->kitapTail = NULL;
        return;
    }

    char line[512];
    if (!fgets(line, sizeof line, fp)) {
        fclose(fp);
        if (veri->kitapHead == NULL) veri->kitapTail = NULL;
        return;
    }

    veri->kitapHead = NULL;
    veri->kitapTail = NULL;

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

            if (!veri->kitapHead) {
                veri->kitapHead = veri->kitapTail = node;
            } else {
                (veri->kitapTail)->next = node;
                veri->kitapTail = node;
            }
        }
    }
    fclose(fp);
     if (veri->kitapHead == NULL) veri->kitapTail = NULL;
}

void goruntuleTekKitapDetay(KitapNode *kitap_it) {
    if (!kitap_it) return;

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
    printf("--------------------------------------------------------------------\n");
}

void goruntuleKitapByAd(const char* arananAd, KutuphaneVeri *veri) {
    if (!veri->kitapHead) {
        printf("Sistemde kayitli kitap bulunmamaktadir.\n");
        return;
    }

    KitapNode *bulunanlar[100]; // Maksimum 100 eþleþen kitap varsayýmý
    int bulunan_sayisi = 0;
    KitapNode *current = veri->kitapHead;

    // Aranan adýn kitap adýnda geçip geçmediðini kontrol et (büyük/küçük harf duyarsýz yapýlabilir)
    // Þimdilik strstr ile basit bir alt dize aramasý yapalým.
    while (current != NULL) {
        // strstr büyük/küçük harf duyarlýdýr.
        // Daha geliþmiþ bir arama için, her iki string'i de küçük harfe çevirip karþýlaþtýrabilirsiniz.
        if (strstr(current->data.kitapAdi, arananAd) != NULL) {
            if (bulunan_sayisi < 100) {
                bulunanlar[bulunan_sayisi++] = current;
            } else {
                printf("Uyari: Cok fazla eslesen kitap bulundu, sadece ilk 100 tanesi dikkate alinacak.\n");
                return; // Daha fazla arama yapma
            }
        }
        current = current->next;
    }

    if (bulunan_sayisi == 0) {
        printf("'%s' adini iceren kitap bulunamadi.\n", arananAd);
    } else if (bulunan_sayisi == 1) {
        printf("\nBulunan Kitap Detaylari:\n");
        goruntuleTekKitapDetay(bulunanlar[0]);
    } else {
        printf("\n'%s' adini iceren birden fazla kitap bulundu. Lutfen birini secin:\n", arananAd);
        for (int i = 0; i < bulunan_sayisi; i++) {
            printf("%d. %s (ISBN: %s)\n", i + 1, bulunanlar[i]->data.kitapAdi, bulunanlar[i]->data.isbn);
        }
        int secim;
        printf("Seciminiz (1-%d): ", bulunan_sayisi);
        if (scanf("%d", &secim) == 1 && secim >= 1 && secim <= bulunan_sayisi) {
            while (getchar() != '\n');
            printf("\nSecilen Kitap Detaylari:\n");
            goruntuleTekKitapDetay(bulunanlar[secim - 1]);
        } else {
            printf("Gecersiz secim.\n");
            while (getchar() != '\n');
        }
    }
}

void kitapBilgisiGoruntuleMenu(KutuphaneVeri *veri) {
    char ad_input[100];
    printf("\nBilgilerini goruntulemek istediginiz kitabin adini (veya bir kýsmýný) giriniz: ");
    scanf(" %99[^\n]", ad_input);
    while(getchar() != '\n');

    goruntuleKitapByAd(ad_input, veri);
}

void loadKitapOrnekleriFromCSV(const char *filename_ornekler, KutuphaneVeri *veri) {
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
            KitapNode *parentKitap = findKitapByISBN(veri->kitapHead, anaISBN); // Helper kullanýldý

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

void kitapDeleter(KutuphaneVeri *veri) {
    if (!veri->kitapHead) {
        printf("Kitap listesi bos.\n");
        return;
    }

    char targetISBN[14];
    printf("Kopyasini silmek istediginiz kitabin ISBN numarasini giriniz: ");
    scanf(" %13s", targetISBN);
    while (getchar() != '\n');

    KitapNode *kitap_to_modify_prev = NULL;
    KitapNode *kitap_to_modify = veri->kitapHead;
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
                veri->kitapHead = kitap_to_modify->next;
                if (!veri->kitapHead) veri->kitapTail = NULL;
            } else {
                kitap_to_modify_prev->next = kitap_to_modify->next;
                if (!kitap_to_modify->next) veri->kitapTail = kitap_to_modify_prev;
            }
            free(kitap_to_modify);
            removeKitapYazarEslestirmeleriByISBN(veri, silinenKitapISBN);
            saveKitaplarToCSV(KITAP_CSV_FILE, veri);
            saveKitapOrnekleriToCSV(KITAP_ORNEKLERI_CSV_FILE, veri);
            saveKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE, veri);
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
            veri->kitapHead = kitap_to_modify->next;
            if (!veri->kitapHead) veri->kitapTail = NULL;
        } else {
            kitap_to_modify_prev->next = kitap_to_modify->next;
            if (!kitap_to_modify->next) veri->kitapTail = kitap_to_modify_prev;
        }
        free(kitap_to_modify);
        removeKitapYazarEslestirmeleriByISBN(veri, silinenKitapISBN);
        saveKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE, veri);
    }

    saveKitaplarToCSV(KITAP_CSV_FILE, veri);
    saveKitapOrnekleriToCSV(KITAP_ORNEKLERI_CSV_FILE, veri);
}

void kitapUpgrader(KutuphaneVeri *veri) {
    if (!veri->kitapHead) {
        printf("Kitap listesi bos.\n");
        return;
    }
    char targetISBN[14];
    printf("Duzenlemek istediginiz kitabin ISBN numarasini giriniz: ");
    scanf(" %13s", targetISBN);
    while(getchar() != '\n');

    KitapNode *current = findKitapByISBN(veri->kitapHead, targetISBN);

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
        while(getchar() != '\n'); // scanf'ten sonra tamponu temizle
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
            // ... (Adet artýrma mantýðý önceki gibi, break/continue içermiyordu) ...
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
                    return; // Hata durumunda erken çýkýþ
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

            int i = 0; 
            while (i < diff_remove) { 
                KitapOrnegiNode *ornek_cur = current->data.ornekler_head;
                KitapOrnegiNode *ornek_prev_ptr = NULL;
                int removed_one_in_this_pass = 0; 

                while(ornek_cur && !removed_one_in_this_pass){
                    if(strcmp(ornek_cur->data.durum, "RAFTA") == 0){
                        if(ornek_prev_ptr == NULL){ 
                            current->data.ornekler_head = ornek_cur->next;
                        } else { 
                            ornek_prev_ptr->next = ornek_cur->next;
                        }
                        printf("Otomatik silinen RAFTA kopya: %s\n", ornek_cur->data.etiketNo);
                        free(ornek_cur);
                        ornek_cur = NULL; 
                        i++; 
                        removed_one_in_this_pass = 1; 
                    } else {
                        ornek_prev_ptr = ornek_cur;
                        if (ornek_cur) ornek_cur = ornek_cur->next; 
                    }
                } 

                if (!removed_one_in_this_pass && i < diff_remove) {
                    printf("Hata: Beklenen sayýda RAFTA kopya silinemedi (muhtemelen hepsi ödünçte veya bir mantýk hatasý var). (i=%d, diff_remove=%d)\n", i, diff_remove);
                    current->data.adet = current->data.adet - i; // Sadece gerçekten silinenler kadar azalt
                    saveKitaplarToCSV(KITAP_CSV_FILE, veri);
                    saveKitapOrnekleriToCSV(KITAP_ORNEKLERI_CSV_FILE, veri);
                    return; 
                }
            } 
            current->data.adet = newAdet; 
        }

        if (newAdet == 0 && current->data.ornekler_head == NULL) {
            printf("Kitabin adedi 0'a dusuruldu ve hic kopyasi kalmadi.\n");
        }
    } else {
        printf("Gecersiz secim.\n");
        return;
    }

    printf("Kitap guncellendi: %s, ISBN: %s, Adet: %d\n", current->data.kitapAdi, current->data.isbn, current->data.adet);
    saveKitaplarToCSV(KITAP_CSV_FILE, veri);
    saveKitapOrnekleriToCSV(KITAP_ORNEKLERI_CSV_FILE, veri);
}

void listKitaplarDetail(const KutuphaneVeri *veri) {
    if (!veri->kitapHead) {
        printf("Kitap listesi bos.\n");
        return;
    }
    printf("\n--- Mevcut Kitaplar ve Kopyalari ---\n");
    for (KitapNode *kitap_it = veri->kitapHead; kitap_it; kitap_it = kitap_it->next) {
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

// --- Kitap-Yazar Fonksiyon Tanýmlarý ---
// ... (initKitapYazarDizisi, addKitapYazarEslestirmeDizisine, vb. KutuphaneVeri* alacak þekilde güncellenmeli) ...
// ... Önceki yanýttaki gibi, ancak global deðiþkenler yerine veri->... kullanýlacak ...
void kitabinYazariniGuncelle(KutuphaneVeri *veri) {
    char isbn_input[14];
    int secim;
    int yazarId_input;

    printf("Yazarini guncellemek istediginiz kitabin ISBN'ini giriniz: ");
    scanf(" %13s", isbn_input);
    while(getchar() != '\n');

    KitapNode* kitap = findKitapByISBN(veri->kitapHead, isbn_input);
    if (!kitap) {
        printf("Hata: '%s' ISBN numarali kitap bulunamadi.\n", isbn_input);
        return;
    }

    int devamEt = 1;
    while (devamEt) {
        printf("\n--- Kitap: %s (ISBN: %s) ---\n", kitap->data.kitapAdi, kitap->data.isbn);
        printf("Mevcut Yazarlar:\n");
        int yazar_sira_no = 0;
        int mevcut_yazar_idler[100]; // Kitabýn mevcut yazarlarýnýn ID'lerini tutmak için
        int mevcut_yazar_sayisi = 0;
        int silinmis_yazar_var_mi = 0;

        for (int i = 0; i < veri->kitapYazarEslestirmeSayisi; i++) {
            if (strcmp((*(veri->kitapYazarEslestirmeleri + i)).isbn, isbn_input) == 0) {
                yazar_sira_no++;
                yazarNode* yazar = findYazarByID(veri->yazarHead, (*(veri->kitapYazarEslestirmeleri + i)).yazarId);
                if (yazar) {
                    printf("%d. ID: %d, Ad: %s, Soyad: %s\n", yazar_sira_no, yazar->data.yazarId, yazar->data.yazarAdi, yazar->data.yazarSoyadi);
                } else if ((*(veri->kitapYazarEslestirmeleri + i)).yazarId == -1) {
                    printf("%d. ID: -1 (Yazar Silinmis - Bu eslestirme kaldirilabilir)\n", yazar_sira_no);
                    silinmis_yazar_var_mi = 1;
                } else {
                    printf("%d. ID: %d (Yazar bilgisi bulunamadi - Bu eslestirme kaldirilabilir)\n", yazar_sira_no, (*(veri->kitapYazarEslestirmeleri + i)).yazarId);
                    // Bu durum, yazar veritabanýndan silinmiþ ama eslestirme kalmýþsa oluþabilir.
                    // -1'den farklý bir ID ise ve yazar bulunamýyorsa, bu bir veri tutarsýzlýðý olabilir.
                }
                if (mevcut_yazar_sayisi < 100) { // Güvenlik için sýnýr kontrolü
                    mevcut_yazar_idler[mevcut_yazar_sayisi++] = (*(veri->kitapYazarEslestirmeleri + i)).yazarId;
                }
            }
        }

        if (yazar_sira_no == 0) {
            printf("Bu kitaba atanmis yazar bulunmamaktadir.\n");
        }
        if (silinmis_yazar_var_mi){
            printf("Uyari: Bu kitapla iliskili silinmis yazar kayitlari (-1 ID) bulunmaktadir.\n");
        }


        printf("\nYapmak istediginiz islemi secin:\n");
        printf("1. Mevcut Bir Yazari Kaldir\n");
        printf("2. Yeni Bir Yazar Ekle\n");
        printf("0. Guncellemeyi Bitir ve Kaydet\n");
        printf("Seciminiz: ");

        if (scanf("%d", &secim) != 1) {
            printf("Gecersiz giris. Sayi bekleniyor.\n");
            while (getchar() != '\n');
            continue; // Döngünün baþýna dön
        }
        while(getchar() != '\n');

        switch (secim) {
            case 1: // Mevcut Bir Yazarý Kaldýr
                if (yazar_sira_no == 0) {
                    printf("Kaldirilacak yazar bulunmamaktadir.\n");
                    break;
                }
                printf("Kaldirmak istediginiz yazarin ID'sini giriniz: ");
                if (scanf("%d", &yazarId_input) != 1) {
                    printf("Gecersiz yazar ID girisi.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                int yazarKitaptaMevcut = 0;
                for(int i=0; i < mevcut_yazar_sayisi; ++i){
                    if(mevcut_yazar_idler[i] == yazarId_input){
                        yazarKitaptaMevcut = 1;
                        break;
                    }
                }

                if (yazarKitaptaMevcut) {
                    removeKitapYazarEslestirmeDizisinden(veri, isbn_input, yazarId_input);
                    printf("Yazar (ID: %d) kitaptan kaldirildi.\n", yazarId_input);
                } else {
                    printf("Hata: Girdiginiz ID (%d) bu kitabin mevcut yazarlari arasinda bulunmuyor veya gecersiz.\n", yazarId_input);
                }
                break;

            case 2: // Yeni Bir Yazar Ekle
                printf("Eklemek istediginiz yeni yazarin ID'sini giriniz: ");
                if (scanf("%d", &yazarId_input) != 1) {
                    printf("Gecersiz yazar ID girisi.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                if (!findYazarByID(veri->yazarHead, yazarId_input)) {
                    printf("Hata: '%d' ID'li yazar sistemde bulunamadi.\n", yazarId_input);
                    break;
                }

                int zatenVar = 0;
                for (int i = 0; i < veri->kitapYazarEslestirmeSayisi; i++) {
                    if (strcmp((*(veri->kitapYazarEslestirmeleri + i)).isbn, isbn_input) == 0 &&
                        (*(veri->kitapYazarEslestirmeleri + i)).yazarId == yazarId_input) {
                        zatenVar = 1;
                        break;
                    }
                }
                if (zatenVar) {
                    printf("Hata: Yazar (ID: %d) zaten bu kitapla eslestirilmis.\n", yazarId_input);
                } else {
                    if (addKitapYazarEslestirmeDizisine(veri, isbn_input, yazarId_input)) {
                        printf("Yeni yazar (ID: %d) kitaba basariyla eklendi.\n", yazarId_input);
                    } else {
                        printf("Yeni yazar eklenirken hata olustu.\n");
                    }
                }
                break;

            case 0: // Guncellemeyi Bitir
                devamEt = 0;
                saveKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE, veri);
                printf("Kitap-Yazar eslestirmeleri guncellendi ve kaydedildi.\n");
                break;

            default:
                printf("Gecersiz secim.\n");
                break;
        }
    }
}

void initKitapYazarDizisi(KutuphaneVeri *veri) {
    veri->kitapYazarEslestirmeleri = NULL;
    veri->kitapYazarEslestirmeSayisi = 0;
    veri->kitapYazarEslestirmeKapasitesi = 0;
}

int addKitapYazarEslestirmeDizisine(KutuphaneVeri *veri, const char* isbn, int yazarId) {
    if (veri->kitapYazarEslestirmeSayisi >= veri->kitapYazarEslestirmeKapasitesi) {
        int yeniKapasite; 
		if (veri->kitapYazarEslestirmeKapasitesi == 0) {
    		yeniKapasite = 10;
		} else {
		    yeniKapasite = veri->kitapYazarEslestirmeKapasitesi * 2;
		}
        KitapYazarEslestirme* temp = realloc(veri->kitapYazarEslestirmeleri, yeniKapasite * sizeof(KitapYazarEslestirme));
        if (!temp) {
            perror("realloc addKitapYazarEslestirmeDizisine");
            return 0;
        }
        veri->kitapYazarEslestirmeleri = temp;
        veri->kitapYazarEslestirmeKapasitesi = yeniKapasite;
    }
    strncpy((*(veri->kitapYazarEslestirmeleri + veri->kitapYazarEslestirmeSayisi)).isbn, isbn, 13);
    (*(veri->kitapYazarEslestirmeleri + veri->kitapYazarEslestirmeSayisi)).isbn[13] = '\0';
    (*(veri->kitapYazarEslestirmeleri + veri->kitapYazarEslestirmeSayisi)).yazarId = yazarId;
    (veri->kitapYazarEslestirmeSayisi)++;
    return 1;
}

void removeKitapYazarEslestirmeDizisinden(KutuphaneVeri *veri, const char* isbn, int yazarId) {
    int i, j;
    for (i = 0; i < veri->kitapYazarEslestirmeSayisi; i++) {
        if (strcmp((*(veri->kitapYazarEslestirmeleri + i)).isbn, isbn) == 0 && (*(veri->kitapYazarEslestirmeleri + i)).yazarId == yazarId) {
            for (j = i; j < veri->kitapYazarEslestirmeSayisi - 1; j++) {
                *(veri->kitapYazarEslestirmeleri + j) = *(veri->kitapYazarEslestirmeleri + j + 1);
            }
            (veri->kitapYazarEslestirmeSayisi)--;
            i--;
        }
    }
}

void removeKitapYazarEslestirmeleriByISBN(KutuphaneVeri *veri, const char* isbn) {
    int i, j;
    for (i = 0; i < veri->kitapYazarEslestirmeSayisi; i++) {
        if (strcmp((*(veri->kitapYazarEslestirmeleri + i)).isbn, isbn) == 0) {
            for (j = i; j < veri->kitapYazarEslestirmeSayisi - 1; j++) {
                 *(veri->kitapYazarEslestirmeleri + j) = *(veri->kitapYazarEslestirmeleri + j + 1);
            }
            (veri->kitapYazarEslestirmeSayisi)--;
            i--;
        }
    }
}

void updateYazarIdInEslestirmeler(KutuphaneVeri *veri, int eskiYazarId, int yeniYazarId) {
    for (int i = 0; i < veri->kitapYazarEslestirmeSayisi; i++) {
        if ((*(veri->kitapYazarEslestirmeleri + i)).yazarId == eskiYazarId) {
            (*(veri->kitapYazarEslestirmeleri + i)).yazarId = yeniYazarId;
        }
    }
}

void loadKitapYazarEslestirmeleri(const char *filename, KutuphaneVeri *veri) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fp = fopen(filename, "w");
        if (!fp) {
            fprintf(stderr, "KitapYazar CSV dosyasi olusturulamadi: %s\n", strerror(errno));
            return;
        }
        fprintf(fp, "ISBN,YazarID\n");
        fclose(fp);
        return;
    }

    char line[100];
    if (!fgets(line, sizeof line, fp)) {
        fclose(fp);
        return;
    }

    freeKitapYazarDizisi(veri); // Öncekini temizle
    initKitapYazarDizisi(veri);

    while (fgets(line, sizeof line, fp)) {
        char isbn_str[14];
        int yazarId_val;
        if (sscanf(line, "%13[^,],%d", isbn_str, &yazarId_val) == 2) {
            addKitapYazarEslestirmeDizisine(veri, isbn_str, yazarId_val);
        }
    }
    fclose(fp);
}

void saveKitapYazarEslestirmeleri(const char *filename, const KutuphaneVeri *veri) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "KitapYazar CSV dosyasi acilirken hata: %s\n", strerror(errno));
        return;
    }
    fprintf(fp, "ISBN,YazarID\n");
    for (int i = 0; i < veri->kitapYazarEslestirmeSayisi; i++) {
        fprintf(fp, "%s,%d\n", (*(veri->kitapYazarEslestirmeleri + i)).isbn, (*(veri->kitapYazarEslestirmeleri + i)).yazarId);
    }
    fclose(fp);
}

void freeKitapYazarDizisi(KutuphaneVeri *veri) {
    if (veri->kitapYazarEslestirmeleri) {
        free(veri->kitapYazarEslestirmeleri);
        veri->kitapYazarEslestirmeleri = NULL;
    }
    veri->kitapYazarEslestirmeSayisi = 0;
    veri->kitapYazarEslestirmeKapasitesi = 0;
}

void eslestirKitapYazar(KutuphaneVeri *veri) {
    char isbn_input[14];
    int yazarId_input;

    printf("Eslestirilecek kitabin ISBN'ini giriniz: ");
    scanf(" %13s", isbn_input);
    while(getchar() != '\n');

    if (!findKitapByISBN(veri->kitapHead, isbn_input)) {
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

    if (!findYazarByID(veri->yazarHead, yazarId_input)) {
        printf("Hata: '%d' ID'li yazar bulunamadi.\n", yazarId_input);
        return;
    }

    for (int i = 0; i < veri->kitapYazarEslestirmeSayisi; i++) {
        if (strcmp((*(veri->kitapYazarEslestirmeleri + i)).isbn, isbn_input) == 0 &&
            (*(veri->kitapYazarEslestirmeleri + i)).yazarId == yazarId_input) {
            printf("Bu kitap ve yazar zaten eslestirilmis.\n");
            return;
        }
    }

    if (addKitapYazarEslestirmeDizisine(veri, isbn_input, yazarId_input)) {
        printf("Kitap (ISBN: %s) ve Yazar (ID: %d) basariyla eslestirildi.\n", isbn_input, yazarId_input);
        saveKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE, veri);
    } else {
        printf("Eslestirme yapilirken bir hata olustu.\n");
    }
}

void kaldirKitapYazarEslestirmesi(KutuphaneVeri *veri) {
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

    int bulundu = 0; // Eþleþmenin bulunup bulunmadýðýný takip etmek için bayrak
    int i = 0;       // Döngü sayacý

    while (i < veri->kitapYazarEslestirmeSayisi && !bulundu) {
        if (strcmp((*(veri->kitapYazarEslestirmeleri + i)).isbn, isbn_input) == 0 && (*(veri->kitapYazarEslestirmeleri + i)).yazarId == yazarId_input) {
			bulundu = 1; // Eþleþme bulundu, bayraðý set et
        }
        i++; 
    }

    if (!bulundu) {
        printf("Belirtilen kitap (ISBN: %s) ve yazar (ID: %d) icin eslestirme bulunamadi.\n", isbn_input, yazarId_input);
        return; // Eþleþme yoksa fonksiyondan çýk
    }

    removeKitapYazarEslestirmeDizisinden(veri, isbn_input, yazarId_input);
    printf("Kitap (ISBN: %s) ve Yazar (ID: %d) eslestirmesi kaldirildi.\n", isbn_input, yazarId_input);
    saveKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE, veri);
}

void listeleKitabinYazarlari(const KutuphaneVeri *veri) {
    char isbn_input[14];
    printf("Yazarlarini listelemek istediginiz kitabin ISBN'ini giriniz: ");
    scanf(" %13s", isbn_input);
    while(getchar() != '\n');

    KitapNode* kitap = findKitapByISBN(veri->kitapHead, isbn_input);
    if (!kitap) {
        printf("Hata: '%s' ISBN numarali kitap bulunamadi.\n", isbn_input);
        return;
    }

    printf("\n--- Kitap: %s (ISBN: %s) ---\n", kitap->data.kitapAdi, kitap->data.isbn);
    printf("Yazarlar:\n");
    int yazar_bulundu_mu = 0;
    for (int i = 0; i < veri->kitapYazarEslestirmeSayisi; i++) {
        if (strcmp((*(veri->kitapYazarEslestirmeleri + i)).isbn, isbn_input) == 0) {
            yazarNode* yazar = findYazarByID(veri->yazarHead, (*(veri->kitapYazarEslestirmeleri + i)).yazarId);
            if (yazar) {
                printf("  ID: %d, Ad: %s, Soyad: %s\n", yazar->data.yazarId, yazar->data.yazarAdi, yazar->data.yazarSoyadi);
            } else if ((*(veri->kitapYazarEslestirmeleri + i)).yazarId == -1) {
                printf("  ID: -1 (Yazar Silinmis)\n");
            } else {
                printf("  ID: %d (Yazar bilgisi bulunamadi)\n", (*(veri->kitapYazarEslestirmeleri + i)).yazarId);
            }
            yazar_bulundu_mu = 1;
        }
    }
    if (!yazar_bulundu_mu) {
        printf("Bu kitaba atanmis yazar bulunmamaktadir.\n");
    }
}

void listeleYazarinKitaplari(const KutuphaneVeri *veri) {
    int yazarId_input;
    printf("Kitaplarini listelemek istediginiz yazarin ID'sini giriniz: ");
    if (scanf("%d", &yazarId_input) != 1) {
        printf("Gecersiz yazar ID girisi.\n");
        while(getchar() != '\n');
        return;
    }
    while(getchar() != '\n');

    yazarNode* yazar = findYazarByID(veri->yazarHead, yazarId_input);
    if (!yazar && yazarId_input != -1) {
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
    for (int i = 0; i < veri->kitapYazarEslestirmeSayisi; i++) {
        if ((*(veri->kitapYazarEslestirmeleri + i)).yazarId == yazarId_input) {
            KitapNode* kitap = findKitapByISBN(veri->kitapHead, (*(veri->kitapYazarEslestirmeleri + i)).isbn);
            if (kitap) {
                printf("  ISBN: %s, Ad: %s\n", kitap->data.isbn, kitap->data.kitapAdi);
            } else {
                printf("  ISBN: %s (Kitap bilgisi bulunamadi)\n", (*(veri->kitapYazarEslestirmeleri + i)).isbn);
            }
            kitap_bulundu_mu = 1;
        }
    }
    if (!kitap_bulundu_mu) {
        printf("Bu yazara atanmis kitap bulunmamaktadir.\n");
    }
}

void listeleTumKitapYazarEslestirmeleri(const KutuphaneVeri *veri) {
    if (veri->kitapYazarEslestirmeSayisi == 0) {
        printf("Kayitli Kitap-Yazar eslestirmesi bulunmamaktadir.\n");
        return;
    }
    printf("\n--- Tum Kitap-Yazar Eslestirmeleri ---\n");
    printf("%-15s %-30s %-10s %-30s\n", "ISBN", "Kitap Adi", "Yazar ID", "Yazar Adi Soyadi");
    printf("--------------------------------------------------------------------------------------\n");
    for (int i = 0; i < veri->kitapYazarEslestirmeSayisi; i++) {
        KitapNode* kitap = findKitapByISBN(veri->kitapHead, (*(veri->kitapYazarEslestirmeleri + i)).isbn);
        yazarNode* yazar = findYazarByID(veri->yazarHead, (*(veri->kitapYazarEslestirmeleri + i)).yazarId);
        char kitap_adi_str[101] = "Kitap Bulunamadi";
        char yazar_ad_soyad_str[101] = "Yazar Bulunamadi";

        if (kitap) strncpy(kitap_adi_str, kitap->data.kitapAdi, 100);
        kitap_adi_str[100] = '\0';

        if (yazar) {
            snprintf(yazar_ad_soyad_str, 100, "%s %s", yazar->data.yazarAdi, yazar->data.yazarSoyadi);
        } else if ((*(veri->kitapYazarEslestirmeleri + i)).yazarId == -1) {
            strcpy(yazar_ad_soyad_str, "(Yazar Silinmis)");
        }

        printf("%-15s %-30.30s %-10d %-30.30s\n",
               (*(veri->kitapYazarEslestirmeleri + i)).isbn,
               kitap_adi_str,
               (*(veri->kitapYazarEslestirmeleri + i)).yazarId,
               yazar_ad_soyad_str);
    }
}

// --- Odunc Islemleri Fonksiyon Tanýmlarý ---
// ... (initOduncIslemleriDizisi, addOduncIslemiDizisine, vb. KutuphaneVeri* alacak þekilde güncellenmeli) ...
// ... Önceki yanýttaki gibi, ancak global deðiþkenler yerine veri->... kullanýlacak ...
void initOduncIslemleriDizisi(KutuphaneVeri *veri) {
    veri->oduncIslemleri = NULL;
    veri->oduncIslemiSayisi = 0;
    veri->oduncIslemiKapasitesi = 0;
}

int addOduncIslemiDizisine(KutuphaneVeri *veri, unsigned int ogrenciNo, const char* kitapEtiketNo, int islemTuru, const char* tarih) {
    if (veri->oduncIslemiSayisi >= veri->oduncIslemiKapasitesi) {
        int yeniKapasite;
		if (veri->oduncIslemiKapasitesi == 0) {
    		yeniKapasite = 20;
		} else {
    		yeniKapasite = veri->oduncIslemiKapasitesi * 2;
		}
        OduncIslemi* temp = realloc(veri->oduncIslemleri, yeniKapasite * sizeof(OduncIslemi));
        if (!temp) {
            perror("realloc addOduncIslemiDizisine");
            return 0;
        }
        veri->oduncIslemleri = temp;
        veri->oduncIslemiKapasitesi = yeniKapasite;
    }
    (*(veri->oduncIslemleri + veri->oduncIslemiSayisi)).ogrenciNo = ogrenciNo;
    strncpy((*(veri->oduncIslemleri + veri->oduncIslemiSayisi)).kitapEtiketNo, kitapEtiketNo, 24);
    (*(veri->oduncIslemleri + veri->oduncIslemiSayisi)).kitapEtiketNo[24] = '\0';
    (*(veri->oduncIslemleri + veri->oduncIslemiSayisi)).islemTuru = islemTuru;
    strncpy((*(veri->oduncIslemleri + veri->oduncIslemiSayisi)).tarih, tarih, 10);
    (*(veri->oduncIslemleri + veri->oduncIslemiSayisi)).tarih[10] = '\0';
    (veri->oduncIslemiSayisi)++;
    return 1;
}

void loadOduncIslemleri(const char *filename, KutuphaneVeri *veri) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fp = fopen(filename, "w");
        if (!fp) {
            fprintf(stderr, "OduncIslemleri CSV dosyasi olusturulamadi: %s\n", strerror(errno));
            return;
        }
        fprintf(fp, "OgrenciNo,KitapEtiketNo,IslemTuru,Tarih\n");
        fclose(fp);
        return;
    }

    char line[200];
    if (!fgets(line, sizeof line, fp)) {
        fclose(fp);
        return;
    }
    freeOduncIslemleriDizisi(veri); // Öncekini temizle
    initOduncIslemleriDizisi(veri);

    while (fgets(line, sizeof line, fp)) {
        unsigned int ogrNo;
        char etiket[25];
        int islem;
        char tarih_str[11];
        if (sscanf(line, "%u,%24[^,],%d,%10[^\n]", &ogrNo, etiket, &islem, tarih_str) == 4) {
            addOduncIslemiDizisine(veri, ogrNo, etiket, islem, tarih_str);
        }
    }
    fclose(fp);
}

void saveOduncIslemleri(const char *filename, const KutuphaneVeri *veri) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "OduncIslemleri CSV dosyasi acilirken hata: %s\n", strerror(errno));
        return;
    }
    fprintf(fp, "OgrenciNo,KitapEtiketNo,IslemTuru,Tarih\n");
    for (int i = 0; i < veri->oduncIslemiSayisi; i++) {
        fprintf(fp, "%u,%s,%d,%s\n",
                (*(veri->oduncIslemleri + i)).ogrenciNo,
                (*(veri->oduncIslemleri + i)).kitapEtiketNo,
                (*(veri->oduncIslemleri + i)).islemTuru,
                (*(veri->oduncIslemleri + i)).tarih);
    }
    fclose(fp);
}

void freeOduncIslemleriDizisi(KutuphaneVeri *veri) {
    if (veri->oduncIslemleri) {
        free(veri->oduncIslemleri);
        veri->oduncIslemleri = NULL;
    }
    veri->oduncIslemiSayisi = 0;
    veri->oduncIslemiKapasitesi = 0;
}

void oduncAlKitap(KutuphaneVeri *veri) {
    unsigned int ogrenciNo_input;
    char etiketNo_input[25];
    char odunc_tarih_input[11]; // Kullanýcýdan alýnacak tarih

    printf("Kitap odunc alacak ogrencinin numarasini giriniz: ");
    if (scanf("%u", &ogrenciNo_input) != 1) {
        printf("Gecersiz ogrenci numarasi girisi.\n");
        while(getchar() != '\n'); return;
    }
    while(getchar() != '\n');

    ogrenciNode* ogrenci = findOgrenciByNo(veri->ogrenciHead, ogrenciNo_input);
    if (!ogrenci) {
        printf("Hata: '%u' numarali ogrenci bulunamadi.\n", ogrenciNo_input);
        return;
    }
    if (ogrenci->data.kutupPuani < 0) {
        printf("Hata: Ogrencinin (%u) kutuphane puani (%d) negatif. Kitap odunc alamaz.\n", ogrenciNo_input, ogrenci->data.kutupPuani);
        return;
    }

    printf("Odunc almak istediginiz kitabin Etiket Numarasini giriniz: ");
    scanf(" %24s", etiketNo_input);
    while(getchar() != '\n');

    KitapNode* parentKitap = NULL;
    KitapOrnegiNode* kitapOrnegi = findKitapOrnegiByEtiket(veri->kitapHead, etiketNo_input, &parentKitap);
    if (!kitapOrnegi) {
        printf("Hata: '%s' etiket numarali kitap kopyasi bulunamadi.\n", etiketNo_input);
        return;
    }

    if (strcmp(kitapOrnegi->data.durum, "RAFTA") != 0) {
        printf("Hata: Kitap kopyasi '%s' su anda RAFTA degil. Durum: %s\n", etiketNo_input, kitapOrnegi->data.durum);
        int tum_kopyalar_odunc_mu = 1;
        if (parentKitap) {
            KitapOrnegiNode* ko = parentKitap->data.ornekler_head;
            while (ko != NULL && tum_kopyalar_odunc_mu) {
                if (strcmp(ko->data.durum, "RAFTA") == 0) {
                    tum_kopyalar_odunc_mu = 0;
                }
                ko = ko->next;
            }
             if (tum_kopyalar_odunc_mu) {
                 printf("UYARI: '%s' (ISBN: %s) adli kitabin tum kopyalari odunc alinmis. ISLEM BASARISIZ.\n", parentKitap->data.kitapAdi, parentKitap->data.isbn);
            }
        }
        return;
    }

    int gun, ay, yil;
    int tarihGecerli = 0;
    do {
        printf("Odunc alma tarihini giriniz (GG.AA.YYYY): ");
        scanf(" %10s", odunc_tarih_input);
        while(getchar() != '\n');
        if (parseDate(odunc_tarih_input, &gun, &ay, &yil)) {
            tarihGecerli = 1;
        } else {
            printf("Gecersiz tarih formati! Lutfen GG.AA.YYYY formatinda giriniz.\n");
        }
    } while (!tarihGecerli);


    sprintf(kitapOrnegi->data.durum, "%u", ogrenci->data.ogrenciNo);

    if (addOduncIslemiDizisine(veri, ogrenci->data.ogrenciNo, etiketNo_input, 0, odunc_tarih_input)) {
        printf("Kitap '%s', ogrenci '%s %s' (No: %u) tarafindan %s tarihinde odunc alindi.\n",
               etiketNo_input, ogrenci->data.ogrenciAdi, ogrenci->data.ogrenciSoyadi, ogrenci->data.ogrenciNo, odunc_tarih_input);
        saveKitapOrnekleriToCSV(KITAP_ORNEKLERI_CSV_FILE, veri);
        saveOduncIslemleri(ODUNC_ISLEMLERI_CSV_FILE, veri);
    } else {
        printf("Odunc alma islemi kaydedilirken hata olustu.\n");
        strcpy(kitapOrnegi->data.durum, "RAFTA");
    }
}

void teslimEtKitap(KutuphaneVeri *veri) {
    unsigned int ogrenciNo_input;
    char etiketNo_input[25];
    char teslim_tarih_input[11]; 
    char ogrenciNo_str[20];

    printf("Kitap teslim edecek ogrencinin numarasini giriniz: ");
    if (scanf("%u", &ogrenciNo_input) != 1) {
        printf("Gecersiz ogrenci numarasi girisi.\n");
        while(getchar() != '\n'); return;
    }
    while(getchar() != '\n');
    sprintf(ogrenciNo_str, "%u", ogrenciNo_input);

    ogrenciNode* ogrenci = findOgrenciByNo(veri->ogrenciHead, ogrenciNo_input);
    if (!ogrenci) {
        printf("Hata: '%u' numarali ogrenci bulunamadi.\n", ogrenciNo_input);
        return;
    }

    printf("Teslim etmek istediginiz kitabin Etiket Numarasini giriniz: ");
    scanf(" %24s", etiketNo_input);
    while(getchar() != '\n');

    KitapOrnegiNode* kitapOrnegi = findKitapOrnegiByEtiket(veri->kitapHead, etiketNo_input, NULL);
    if (!kitapOrnegi) {
        printf("Hata: '%s' etiket numarali kitap kopyasi bulunamadi.\n", etiketNo_input);
        return;
    }

    if (strcmp(kitapOrnegi->data.durum, "RAFTA") == 0) {
        printf("Hata: Kitap kopyasi '%s' zaten RAFTA.\n", etiketNo_input);
        return;
    }
    if (strcmp(kitapOrnegi->data.durum, ogrenciNo_str) != 0) {
        printf("Hata: Kitap kopyasi '%s', bu ogrenci (%u) tarafindan odunc alinmamis. Mevcut odunc alan: %s\n",
               etiketNo_input, ogrenciNo_input, kitapOrnegi->data.durum);
        return;
    }

    char oduncAlmaTarihi_str[11] = "";
    int oduncKaydiBulundu = 0;
    int i = veri->oduncIslemiSayisi - 1;
    while (i >= 0 && !oduncKaydiBulundu) {
        if ((*(veri->oduncIslemleri + i)).ogrenciNo == ogrenciNo_input &&
            strcmp((*(veri->oduncIslemleri + i)).kitapEtiketNo, etiketNo_input) == 0 &&
            (*(veri->oduncIslemleri + i)).islemTuru == 0) {
            strcpy(oduncAlmaTarihi_str, (*(veri->oduncIslemleri + i)).tarih);
            oduncKaydiBulundu = 1;
        }
        i--;
    }

    if (!oduncKaydiBulundu) {
        printf("Uyari: Bu kitap kopyasi (%s) icin ogrenciye (%u) ait odunc alma kaydi bulunamadi. Ceza kontrolu yapilamayacak.\n", etiketNo_input, ogrenciNo_input);
    }

    int gun, ay, yil;
    int tarihGecerli = 0;
    do {
        printf("Teslim tarihini giriniz (GG.AA.YYYY): ");
        scanf(" %10s", teslim_tarih_input);
        while(getchar() != '\n');
        if (parseDate(teslim_tarih_input, &gun, &ay, &yil)) {
            if (oduncKaydiBulundu) { 
                int odunc_d, odunc_m, odunc_y_val;
                if (parseDate(oduncAlmaTarihi_str, &odunc_d, &odunc_m, &odunc_y_val)) {
                    if (calculateDayDifference(odunc_d, odunc_m, odunc_y_val, gun, ay, yil) < 0) {
                        printf("Hata: Teslim tarihi, odunc alma tarihinden (%s) once olamaz.\n", oduncAlmaTarihi_str);
                    } else {
                        tarihGecerli = 1;
                    }
                } else { 
                    tarihGecerli = 1; // Þimdilik devam et
                }
            } else { 
                tarihGecerli = 1;
            }
        } else {
            printf("Gecersiz tarih formati! Lutfen GG.AA.YYYY formatinda giriniz.\n");
        }
    } while (!tarihGecerli);

    strcpy(kitapOrnegi->data.durum, "RAFTA");

    if (oduncKaydiBulundu) {
        int odunc_d, odunc_m, odunc_y_val;
        int teslim_d_val, teslim_m_val, teslim_y_val;
        if (parseDate(oduncAlmaTarihi_str, &odunc_d, &odunc_m, &odunc_y_val) &&
            parseDate(teslim_tarih_input, &teslim_d_val, &teslim_m_val, &teslim_y_val)) { // Kullanýcýdan alýnan teslim tarihi
            int gunFarki = calculateDayDifference(odunc_d, odunc_m, odunc_y_val, teslim_d_val, teslim_m_val, teslim_y_val);
            if (gunFarki > 15) {
                ogrenci->data.kutupPuani -= 10;
                printf("UYARI: Kitap 15 gunden fazla surede teslim edildi. Ogrenciye -10 ceza puani verildi. Yeni Puan: %d\n", ogrenci->data.kutupPuani);
                saveOgrencilerToCSV(OGRENCI_CSV_FILE, veri);
            }
        }
    }

    if (addOduncIslemiDizisine(veri, ogrenci->data.ogrenciNo, etiketNo_input, 1, teslim_tarih_input)) {
        printf("Kitap '%s', ogrenci '%s %s' (No: %u) tarafindan %s tarihinde teslim edildi.\n",
               etiketNo_input, ogrenci->data.ogrenciAdi, ogrenci->data.ogrenciSoyadi, ogrenci->data.ogrenciNo, teslim_tarih_input);
        saveKitapOrnekleriToCSV(KITAP_ORNEKLERI_CSV_FILE, veri);
        saveOduncIslemleri(ODUNC_ISLEMLERI_CSV_FILE, veri);
    } else {
        printf("Teslim etme islemi kaydedilirken hata olustu.\n");
        sprintf(kitapOrnegi->data.durum, "%u", ogrenci->data.ogrenciNo);
    }
}

void zamanindaTeslimEdilmeyenKitaplariListele(const KutuphaneVeri *veri) {
    if (veri->oduncIslemiSayisi < 2 && veri->kitapHead == NULL) { // En az bir ödünç ve bir teslim iþlemi olmalý + kitap kontrolü
        printf("Sistemde yeterli odunc alma/teslim kaydi veya kitap bulunmamaktadir.\n");
        return;
    }

    printf("\n--- Zamaninda Teslim Edilmeyen (Gecikmeli Teslim Edilmis) Kitaplar ---\n");
    printf("%-10s %-25s %-20s %-15s %-15s %-10s\n",
           "Ogr. No", "Ogrenci Ad Soyad", "Kitap Etiket No", "Odunc Tarihi", "Teslim Tarihi", "Gecikme");
    printf("---------------------------------------------------------------------------------------------------------\n");

    int gecikmisKitapBulunduGenel = 0;
    char mevcutTarihStr[11];
    getCurrentDate(mevcutTarihStr);
    int m_gun_mevcut, m_ay_mevcut, m_yil_mevcut;
    parseDate(mevcutTarihStr, &m_gun_mevcut, &m_ay_mevcut, &m_yil_mevcut); // Mevcut tarih parse ediliyor, hata kontrolü þimdilik atlandý

    // 1. Henüz Teslim Edilmemiþ ve Gecikmiþ Kitaplar (Mevcut Mantýk)
    for (KitapNode *k_node = veri->kitapHead; k_node != NULL; k_node = k_node->next) {
        for (KitapOrnegiNode *ko_node = k_node->data.ornekler_head; ko_node != NULL; ko_node = ko_node->next) {
            if (strcmp(ko_node->data.durum, "RAFTA") != 0) { // Kitap ödünçteyse
                unsigned int oduncAlanOgrenciNo = 0;
                if (sscanf(ko_node->data.durum, "%u", &oduncAlanOgrenciNo) == 1 && oduncAlanOgrenciNo > 0) {
                    char sonOduncTarihi[11] = "";
                    int oduncKaydiBulunduIcDongu = 0;

                    int j = veri->oduncIslemiSayisi - 1;
                    while (j >= 0 && !oduncKaydiBulunduIcDongu) {
                        if ((*(veri->oduncIslemleri + j)).ogrenciNo == oduncAlanOgrenciNo &&
                            strcmp((*(veri->oduncIslemleri + j)).kitapEtiketNo, ko_node->data.etiketNo) == 0 &&
                            (*(veri->oduncIslemleri + j)).islemTuru == 0) {
                            strcpy(sonOduncTarihi, (*(veri->oduncIslemleri + j)).tarih);
                            oduncKaydiBulunduIcDongu = 1;
                        }
                        j--;
                    }

                    if (oduncKaydiBulunduIcDongu) {
                        int o_gun, o_ay, o_yil;
                        if (parseDate(sonOduncTarihi, &o_gun, &o_ay, &o_yil)) {
                            int gunFarkiMevcut = calculateDayDifference(o_gun, o_ay, o_yil, m_gun_mevcut, m_ay_mevcut, m_yil_mevcut);
                            if (gunFarkiMevcut > 15) {
                                ogrenciNode *ogr = findOgrenciByNo(veri->ogrenciHead, oduncAlanOgrenciNo);
                                char ogr_ad_soyad[101] = "Ogrenci Bulunamadi";
                                if (ogr) {
                                    snprintf(ogr_ad_soyad, 100, "%s %s", ogr->data.ogrenciAdi, ogr->data.ogrenciSoyadi);
                                    ogr_ad_soyad[100] = '\0';
                                }
                                printf("%-10u %-25.25s %-20s %-15s %-15s %3d gun*\n", // * iþareti hala teslim edilmediðini belirtir
                                       oduncAlanOgrenciNo,
                                       ogr_ad_soyad,
                                       ko_node->data.etiketNo,
                                       sonOduncTarihi,
                                       "HENUZ TESLIM EDILMEDI", // Teslim tarihi yok
                                       gunFarkiMevcut - 15);
                                gecikmisKitapBulunduGenel = 1;
                            }
                        }
                    }
                }
            }
        }
    }


    // 2. Geçmiþte Gecikmeli Teslim Edilmiþ Kitaplar
    // Her bir "teslim etme" (islemTuru == 1) iþlemini kontrol et
    for (int i = 0; i < veri->oduncIslemiSayisi; i++) {
        if ((*(veri->oduncIslemleri + i)).islemTuru == 1) { // Bu bir teslim etme iþlemi
            OduncIslemi teslimIslemi = *(veri->oduncIslemleri + i);
            char oduncAlmaTarihi_str[11] = "";
            int ilgiliOduncKaydiBulundu = 0;

            // Bu teslim iþlemine ait ödünç alma iþlemini bul (ayný öðrenci, ayný kitap, teslimden önceki en son ödünç alma)
            int j = i - 1; // Teslim iþleminden önceki kayýtlara bak
            while (j >= 0 && !ilgiliOduncKaydiBulundu) {
                if ((*(veri->oduncIslemleri + j)).ogrenciNo == teslimIslemi.ogrenciNo &&
                    strcmp((*(veri->oduncIslemleri + j)).kitapEtiketNo, teslimIslemi.kitapEtiketNo) == 0 &&
                    (*(veri->oduncIslemleri + j)).islemTuru == 0) { // Ödünç alma iþlemi
                    strcpy(oduncAlmaTarihi_str, (*(veri->oduncIslemleri + j)).tarih);
                    ilgiliOduncKaydiBulundu = 1; // En son eþleþen ödünç almayý bulduk
                }
                j--;
            }

            if (ilgiliOduncKaydiBulundu) {
                int odunc_d, odunc_m, odunc_y;
                int teslim_d, teslim_m, teslim_y;

                if (parseDate(oduncAlmaTarihi_str, &odunc_d, &odunc_m, &odunc_y) &&
                    parseDate(teslimIslemi.tarih, &teslim_d, &teslim_m, &teslim_y)) {

                    int gunFarki = calculateDayDifference(odunc_d, odunc_m, odunc_y, teslim_d, teslim_m, teslim_y);

                    if (gunFarki > 15) { // 15 günden fazla sürmüþse
                        ogrenciNode *ogr = findOgrenciByNo(veri->ogrenciHead, teslimIslemi.ogrenciNo);
                        char ogr_ad_soyad[101] = "Ogrenci Bulunamadi";
                        if (ogr) {
                            snprintf(ogr_ad_soyad, 100, "%s %s", ogr->data.ogrenciAdi, ogr->data.ogrenciSoyadi);
                            ogr_ad_soyad[100] = '\0';
                        }

                        printf("%-10u %-25.25s %-20s %-15s %-15s %3d gun\n",
                               teslimIslemi.ogrenciNo,
                               ogr_ad_soyad,
                               teslimIslemi.kitapEtiketNo,
                               oduncAlmaTarihi_str,
                               teslimIslemi.tarih,
                               gunFarki - 15); // Gecikme süresi
                        gecikmisKitapBulunduGenel = 1;
                    }
                }
            }
        }
    }


    if (!gecikmisKitapBulunduGenel) {
        printf("Zamaninda teslim edilmemis (15 gunden fazla gecikmis veya gecikmeli teslim edilmis) kitap bulunmamaktadir.\n");
    } else {
        printf("Not: '*' isareti, kitabin hala teslim edilmedigini ve mevcut tarihe gore gecikmeli oldugunu belirtir.\n");
    }
    printf("---------------------------------------------------------------------------------------------------------\n");
}

void listeleTumOduncIslemleri(const KutuphaneVeri *veri) {
    if (veri->oduncIslemiSayisi == 0) {
        printf("Kayitli odunc alma/teslim islemi bulunmamaktadir.\n");
        return;
    }
    printf("\n--- Tum Odunc Alma/Teslim Islemleri ---\n");
    printf("%-10s %-25s %-20s %-12s %-15s\n", "Ogr. No", "Ogrenci Ad Soyad", "Kitap Etiket No", "Islem Turu", "Tarih");
    printf("------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < veri->oduncIslemiSayisi; i++) {
        ogrenciNode* ogr = findOgrenciByNo(veri->ogrenciHead, (*(veri->oduncIslemleri + i)).ogrenciNo);
        char ogr_ad_soyad[101] = "Ogrenci Bulunamadi";
        if(ogr) {
            snprintf(ogr_ad_soyad, 100, "%s %s", ogr->data.ogrenciAdi, ogr->data.ogrenciSoyadi);
            ogr_ad_soyad[100] = '\0'; 
        }

        char islemTuruStr[15]; 
        if ((*(veri->oduncIslemleri + i)).islemTuru == 0) {
            strcpy(islemTuruStr, "Odunc Alma");
        } else {
            strcpy(islemTuruStr, "Teslim Etme");
        }

        printf("%-10u %-25.25s %-20s %-12s %-15s\n",
               (*(veri->oduncIslemleri + i)).ogrenciNo,
               ogr_ad_soyad,
               (*(veri->oduncIslemleri + i)).kitapEtiketNo,
               islemTuruStr, // Deðiþtirilen kýsým
               (*(veri->oduncIslemleri + i)).tarih);
    }
}

// --- Menü Fonksiyonlarý ---
void ogrenciBilgisiGoruntuleMenu(KutuphaneVeri *veri) {
    int choice;
    printf("\n--- Ogrenci Bilgisi Goruntuleme ---\n");
    printf("1. ID (Numara) ile Ara\n");
    printf("2. Ad Soyad ile Ara\n");
    printf("0. Ogrenci Islemleri Menu'sune Don\n");
    printf("Seciminiz: ");

    if (scanf("%d", &choice) != 1) {
        printf("Gecersiz giris. Sayi bekleniyor.\n");
        while (getchar() != '\n');
        choice = -1; // Hatalý giriþi iþaretle
    } else {
        while (getchar() != '\n'); // Baþarýlý giriþ sonrasý tamponu temizle
    }

    switch (choice) {
        case 1:
            goruntuleOgrenciByID(veri);
            break;
        case 2:
            goruntuleOgrenciByAdSoyad(veri);
            break;
        case 0:
            printf("Ogrenci islemleri menusune donuluyor...\n");
            break;
        default:
            printf("Gecersiz secim.\n");
            break;
    }
}

void yazarMenu(KutuphaneVeri *veri) {
    int choice;
    int validInput; 

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
            validInput = 0;        
            choice = -1;           
        } else {
            while (getchar() != '\n'); 
            validInput = 1;         
        }

        if (validInput) { 
            switch (choice) {
                case 1: {
                    yazarNode *newNode = createYazar(veri);
                    if (newNode) {
                        if (!veri->yazarHead) {
                             veri->yazarHead = veri->yazarTail = newNode;
                        } else {
                            (veri->yazarTail)->next = newNode;
                            veri->yazarTail = newNode;
                        }
                        printf("Yazar eklendi: %s %s (ID=%d)\n",
                               newNode->data.yazarAdi,
                               newNode->data.yazarSoyadi,
                               newNode->data.yazarId);
                        saveYazarlarToCSV(YAZAR_CSV_FILE, veri);
                    }
                    break;
                }
                case 2:
                    yazarDeleter(veri);
                    break;
                case 3:
                    listYazarlar(veri);
                    break;
                case 4:
                    yazarUpgrader(veri);
                    break;
                case 0:
                    printf("Ana menu'ye donuluyor...\n");
                    break;
                default:
                    printf("Gecersiz secim, tekrar deneyin.\n");
            }
        }
    } while (choice != 0);
}

// --- Öðrenci Menüsü Güncellemesi ---

// --- `ogrenciMenu` Fonksiyonuna Yeni Seçenek Ekleme ---

void ogrenciMenu(KutuphaneVeri *veri) {
    int choice;
    int validInput;

    do {
        printf("\n=== Ogrenci Islemleri Menu ===\n");
        printf("1. Ogrenci Ekle\n");
        printf("2. Ogrenci Sil\n");
        printf("3. Mevcut Ogrenciler\n");
        printf("4. Ogrenci Guncelle\n");
        printf("5. Ogrenci Bilgisi Goruntule\n");
        printf("6. Kitap Teslim Etmemis Ogrencileri Listele\n");
        printf("7. Cezali Ogrencileri Listele\n");
        printf("8. Odunc Alma/Teslim Islemleri\n"); // YENÝ SEÇENEK
        printf("0. Ana Menu'ye Don\n");
        printf("Seciminiz: ");

        if (scanf("%d", &choice) != 1) {
            printf("Gecersiz giris. Sayi bekleniyor.\n");
            while (getchar() != '\n');
            validInput = 0;
            choice = -1;
        } else {
            while (getchar() != '\n');
            validInput = 1;
        }

        if (validInput) {
            switch (choice) {
                case 1: {
                    ogrenciNode *newNode = createOgrenciNode(veri);
                    if (newNode) {
                        if (!veri->ogrenciHead) {
                            veri->ogrenciHead = veri->ogrenciTail = newNode;
                        } else {
                            (veri->ogrenciTail)->next = newNode;
                            newNode->prev = veri->ogrenciTail;
                            veri->ogrenciTail = newNode;
                        }
                        printf("Ogrenci eklendi: %s %s (No=%u, Puan=%d)\n",
                               newNode->data.ogrenciAdi,
                               newNode->data.ogrenciSoyadi,
                               newNode->data.ogrenciNo,
                               newNode->data.kutupPuani);
                        saveOgrencilerToCSV(OGRENCI_CSV_FILE, veri);
                    }
                    break;
                }
                case 2:
                    ogrenciDeleter(veri);
                    break;
                case 3:
                    listOgrenciler(veri);
                    break;
                case 4:
                    ogrenciUpgrader(veri);
                    break;
                case 5:
                    ogrenciBilgisiGoruntuleMenu(veri);
                    break;
                case 6:
                    kitapTeslimEtmemisOgrencileriListele(veri);
                    break;
                case 7:
                    cezaliOgrencileriListele(veri);
                    break;
                case 8: 
                    oduncIslemleriMenu(veri); 
                    break;
                case 0:
                    printf("Ana menu'ye donuluyor...\n");
                    break;
                default:
                    printf("Gecersiz secim, tekrar deneyin.\n");
            }
        }
    } while (choice != 0);
}

void kitapMenu(KutuphaneVeri *veri) {
    int choice;
    int validInput;

    do {
        printf("\n=== Kitap Islemleri Menu ===\n");
        printf("1. Kitap Ekle\n");
        printf("2. Kitap Kopyasi Sil\n");
        printf("3. Tum Kitaplari Listele (Detayli)\n");
        printf("4. Kitap Guncelle\n");
        printf("5. Kitap Bilgisi Goruntule (Ada Gore)\n");
        printf("6. Sadece Raftaki Kitaplari Listele\n");
        printf("7. Zamaninda Teslim Edilmeyen Kitaplari Listele\n");
        printf("8. Kitap-Yazar Eslestirme Islemleri\n"); // YENÝ SEÇENEK
        printf("0. Ana Menu'ye Don\n");
        printf("Seciminiz: ");

        if (scanf("%d", &choice) != 1) {
            printf("Gecersiz giris. Sayi bekleniyor.\n");
            while (getchar() != '\n');
            validInput = 0;
            choice = -1;
        } else {
            while (getchar() != '\n');
            validInput = 1;
        }

        if (validInput) {
            switch (choice) {
                case 1: {
                    KitapNode *newNode = createKitap(veri);
                    if (newNode) {
                        if (!veri->kitapHead) {
                            veri->kitapHead = veri->kitapTail = newNode;
                        } else {
                            (veri->kitapTail)->next = newNode;
                            veri->kitapTail = newNode;
                        }
                        printf("Kitap '%s' (ISBN: %s) %d adet kopya ile eklendi.\n",
                               newNode->data.kitapAdi, newNode->data.isbn, newNode->data.adet);
                        saveKitaplarToCSV(KITAP_CSV_FILE, veri);
                        saveKitapOrnekleriToCSV(KITAP_ORNEKLERI_CSV_FILE, veri);
                    }
                    break;
                }
                case 2:
                    kitapDeleter(veri);
                    break;
                case 3:
                    listKitaplarDetail(veri);
                    break;
                case 4:
                    kitapUpgrader(veri);
                    break;
                case 5:
                    kitapBilgisiGoruntuleMenu(veri);
                    break;
                case 6:
                    raftakiKitaplariListele(veri);
                    break;
                case 7:
                    zamanindaTeslimEdilmeyenKitaplariListele(veri);
                    break;
                case 8: // YENÝ CASE
                    kitapYazarMenu(veri); // Mevcut Kitap-Yazar eþleþtirme menüsünü çaðýr
                    break;
                case 0:
                    printf("Ana menu'ye donuluyor...\n");
                    break;
                default:
                    printf("Gecersiz secim, tekrar deneyin.\n");
            }
        }
    } while (choice != 0);
}

void kitapYazarMenu(KutuphaneVeri *veri) {
    int choice;
    int validInput = 0;

    do {
        printf("\n=== Kitap-Yazar Eslestirme Islemleri ===\n");
        printf("1. Kitap-Yazar Eslestir\n");
        printf("2. Kitap-Yazar Eslestirmesini Kaldir\n");
        printf("3. Kitabin Yazarini Guncelle\n"); // YENÝ SEÇENEK
        printf("4. Bir Kitabin Yazarlarini Listele\n"); // Numara güncellendi (önceki 3)
        printf("5. Bir Yazarin Kitaplarini Listele\n"); // Numara güncellendi (önceki 4)
        printf("6. Tum Eslestirmeleri Listele\n");    // Numara güncellendi (önceki 5)
        printf("0. Ana Menu'ye Don\n");
        printf("Seciminiz: ");

        if (scanf("%d", &choice) != 1) {
            printf("Gecersiz giris. Sayi bekleniyor.\n");
            while (getchar() != '\n');
            validInput = 0;
            choice = -1;
        } else {
            while (getchar() != '\n');
            validInput = 1;
        }

        if (validInput) {
            switch (choice) {
                case 1:
                    eslestirKitapYazar(veri);
                    break;
                case 2:
                    kaldirKitapYazarEslestirmesi(veri);
                    break;
                case 3: // YENÝ CASE
                    kitabinYazariniGuncelle(veri);
                    break;
                case 4: // Numara güncellendi (önceki 3)
                    listeleKitabinYazarlari(veri);
                    break;
                case 5: // Numara güncellendi (önceki 4)
                    listeleYazarinKitaplari(veri);
                    break;
                case 6: // Numara güncellendi (önceki 5)
                    listeleTumKitapYazarEslestirmeleri(veri);
                    break;
                case 0:
                    printf("Ana menu'ye donuluyor...\n");
                    break;
                default:
                    printf("Gecersiz secim, tekrar deneyin.\n");
            }
        }
    } while (choice != 0);
}

void oduncIslemleriMenu(KutuphaneVeri *veri) {
    int choice;
    int validInput;
    do {
        printf("\n=== Odunc Alma/Teslim Islemleri ===\n");
        printf("1. Kitap Odunc Al\n");
        printf("2. Kitap Teslim Et\n");
        printf("3. Tum Odunc/Teslim Kayitlarini Listele\n");
        printf("4. Zamaninda Teslim Edilmeyen Kitaplari Listele\n"); // YENÝ SEÇENEK
        printf("0. Ana Menu'ye Don\n");
        printf("Seciminiz: ");
        if (scanf("%d", &choice) != 1) {
            printf("Gecersiz giris. Sayi bekleniyor.\n");
            while (getchar() != '\n');
            validInput = 0;
            choice = -1;
        } else {
            while (getchar() != '\n');
            validInput = 1;
        }

        if (validInput) {
            switch (choice) {
                case 1:
                    oduncAlKitap(veri);
                    break;
                case 2:
                    teslimEtKitap(veri);
                    break;
                case 3:
                    listeleTumOduncIslemleri(veri);
                    break;
                case 4: // YENÝ CASE
                    zamanindaTeslimEdilmeyenKitaplariListele(veri);
                    break;
                case 0:
                    printf("Ana menu'ye donuluyor...\n");
                    break;
                default:
                    printf("Gecersiz secim, tekrar deneyin.\n");
            }
        }
    } while (choice != 0);
}
// --- Main Function ---
int main(void) {
    KutuphaneVeri kutuphaneSistemi;

    // KutuphaneVeri struct'ýnýn alanlarýný baþlangýçta NULL veya 0 yap
    kutuphaneSistemi.yazarHead = NULL;
    kutuphaneSistemi.yazarTail = NULL;
    kutuphaneSistemi.yazarCount = 0;
    kutuphaneSistemi.ogrenciHead = NULL;
    kutuphaneSistemi.ogrenciTail = NULL;
    kutuphaneSistemi.kitapHead = NULL;
    kutuphaneSistemi.kitapTail = NULL;
    // Diðer dinamik diziler için init fonksiyonlarý çaðrýlacak

    initKitapYazarDizisi(&kutuphaneSistemi);
    initOduncIslemleriDizisi(&kutuphaneSistemi);

    loadYazarlarFromCSV(YAZAR_CSV_FILE, &kutuphaneSistemi);
    loadOgrencilerFromCSV(OGRENCI_CSV_FILE, &kutuphaneSistemi);
    loadKitaplarFromCSV(KITAP_CSV_FILE, &kutuphaneSistemi);
    if(kutuphaneSistemi.kitapHead) { // Sadece kitap varsa örnekleri yükle
        loadKitapOrnekleriFromCSV(KITAP_ORNEKLERI_CSV_FILE, &kutuphaneSistemi);
    }
    loadKitapYazarEslestirmeleri(KITAP_YAZAR_CSV_FILE, &kutuphaneSistemi);
    loadOduncIslemleri(ODUNC_ISLEMLERI_CSV_FILE, &kutuphaneSistemi);

    int anaMenuChoice;
    do {
        printf("\n=========== KUTUPHANE OTOMASYONU ANA MENU ===========\n");
        printf("1. Yazar Islemleri\n");
        printf("2. Ogrenci Islemleri\n");
        printf("3. Kitap Islemleri\n");
        printf("4. Odunc Alma/Teslim Islemleri\n");
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
                yazarMenu(&kutuphaneSistemi);
                break;
            case 2:
                ogrenciMenu(&kutuphaneSistemi);
                break;
            case 3:
                kitapMenu(&kutuphaneSistemi);
                break;
            case 4:
                oduncIslemleriMenu(&kutuphaneSistemi);
                break;
            case 0:
                printf("Programdan cikiliyor...\n");
                break;
            default:
                printf("Gecersiz secim, tekrar deneyin.\n");
        }
    } while (anaMenuChoice != 0);

    // Free iþlemleri
    freeYazarList(kutuphaneSistemi.yazarHead);
    freeOgrenciList(kutuphaneSistemi.ogrenciHead);
    freeKitapList(kutuphaneSistemi.kitapHead);
    freeKitapYazarDizisi(&kutuphaneSistemi);
    freeOduncIslemleriDizisi(&kutuphaneSistemi);

    return 0;
}
