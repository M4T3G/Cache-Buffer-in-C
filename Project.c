#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Düðüm tanýmý
struct node {
  struct node *prev;
  struct node *next;
  char adres[50];
  int istek;
};


// Listeye baþa eleman ekler
void pushFront(struct node **head, char *val, int *dugumSayisi) {
  if(control(*head,val)==-1){    //burada o adres önceden gecmis mi diye kontrol ediyor.gecmemisse if icine girip yeni dugum ekliyor.
    struct node *new_node = malloc(sizeof(struct node));
    if (new_node == NULL) {
      printf("Bellek tahsis hatasi.\n");
      return;
    }
    strcpy(new_node->adres, val);
    new_node->next = *head;
    new_node->prev = NULL;
    if (*head != NULL) {        //burada eger ilk dugum bos degilse 2. dugum olarak ekliyor.
      (*head)->prev = new_node;   
    }
    *head = new_node;     //ilk dugum bossa direk gelen dugumu bas dugum yapýyor.
    new_node->istek = 1;  //ilk kez geldigi icin istek sayisini 1 yapiyor.
    (*dugumSayisi)++;     //toplam dugum sayisini artiriyor.
  }
}


// Listeyi yazdýrýr
void printList(struct node *node) {
  while (node != NULL) {
    printf("%s(%d) -> ", node->adres,node->istek);
    node = node->next;
  }
  printf("\n");
}


// Verilen adresi iceren dugumu siler
void deleteNode(struct node **head, char *val, int *dugumSayisi) {
  // Liste bossa veya adres bulunamadiysa hata mesaji yazdir ve geri don
  if (head == NULL || *head == NULL) {
    printf("Liste boþ veya adres bulunamadý.\n");
    return;
  }
  struct node *current = *head;
  // Silinecek dugum listenin basiysa, bas ogreticiyi guncelle ve hafizayi serbest birak
  if (strcmp(current->adres, val) == 0) {
    *head = current->next;
    if (*head != NULL) {
      (*head)->prev = NULL;
    }
    (*dugumSayisi)--;
    free(current);
    return;
  }
  // Silinecek dugumu bulmak icin listeyi gez
  while (current->next != NULL && strcmp(current->next->adres, val) != 0) {
    current = current->next;
  }
  // Silinecek dugum bulunamadýysa hata mesaji yazdir ve geri don
  if (current->next == NULL) {
    printf("Adres bulunamadý.\n");
    return;
  }
  // Silinecek dugumu (bas dugum haric) sil
  struct node *temp = current->next;
  current->next = temp->next;
  if (temp->next != NULL) {
    temp->next->prev = current;
  }
  (*dugumSayisi)--;
  free(temp);
}


// Bir adresteki sorgu sayisini ekrana getirir
void query(struct node *head, char *val) {
  if (head == NULL) {
    printf("Cache uzerinde hic adres yok.\n");
    return;
  }
  while (head != NULL) {
    if (strcmp(head->adres, val) == 0) {
      printf("Istek sayisi=%d\n", head->istek);
      return;
    }
    head = head->next;
  }
  printf("Istenen adres cache uzerinde yok.\n");
}


// Listede adres önceden geçmiþ mi diye kontrol eder.Gecmemisse -1 dondurur. Node olarak ekleme islemi ustteydi. Gecmisse burada istek sayisini 1 artiriyor.
int control(struct node *node, char *val) {
  while (node != NULL) {
    if (strcmp(node->adres, val) == 0) {
      node->istek++;
      return 1;
    }
    node = node->next;
  }
  return -1;
}


// Esik degeri asilmissa o dugumu basa tasir
void thresholdControl(struct node **head, char *val, int esik) {
  struct node *current = *head;
  while (current != NULL) {
    if (strcmp(current->adres, val) == 0 && current->istek > esik) {
      // Istenilen adresi ve esik degerini gecen dugum bulundu
      // Dugum zaten bas dugum ise, degisiklik gerekmez
      if (current == *head) {
        return;
      }
      // Dugumu basa tasir:
      if (current->prev != NULL) {
        current->prev->next = current->next;
      }
      if (current->next != NULL) {
        current->next->prev = current->prev;
      }
      current->next = *head;
      *head = current;
      // Bas dugum degistiyse, prev isaretcisini de ayarla
      if (current->prev != NULL) {
        current->prev->prev = NULL;
      } else {  // Bas dugumden onceki dugum ise
        (*head)->prev = NULL;  // Head'in prev'ini NULL yap
      }

      return;
    }
    current = current->next;
  }
}


// Kapasite kontrolü yapar ve kapasiteyi aþmýþsa son düðümü siler
void deleteLastNode(struct node **head,int *dugumSayisi) {
  if (*head == NULL) {
    return;
  }
  struct node *current = *head;
  while (current->next != NULL) {
    current = current->next;
  }
  if (current->prev != NULL) {
    current->prev->next = NULL;
  } else {  // Tek düðüm kalmýþsa head'i de NULL yap
    *head = NULL;
  }
  (*dugumSayisi)--;
  free(current);
}


// Dosyadan okuma
void fileRead(char *dosyaAdi, struct node **head, int *dugumSayisi, int *kapasite, int esik) {
  FILE *dosya = fopen(dosyaAdi, "r");
  char satir[100];
  if (dosya == NULL) {
    printf("Dosya açma hatasý\n");
    return;
  }
  while (fgets(satir, sizeof(satir), dosya) != NULL) {
    satir[strcspn(satir, "\n")] = '\0'; // Satir sonundaki yeni satir karakterini kaldirir
    pushFront(head, satir, dugumSayisi);  //ekleme fonksiyonuna gitti
    thresholdControl(head, satir, esik);  //esik kontrolu yapilir
    if(*dugumSayisi > *kapasite){          //kapasiteyi gecme kontrolu yapilir
      deleteLastNode(head, dugumSayisi);    //gecmisse son dugum silinir
    }
    printList(*head);             //Her adimda listeyi ekrana yazdirir.
  }
  printf("Listenin son hali:\n");
  printList(*head);
  fclose(dosya);
}

// Belleði serbest býrakýr
void clearList(struct node **head) {
  struct node *current = *head;
  while (current != NULL) {
    struct node *temp = current;
    current = current->next;
    free(temp);
  }
  *head = NULL;
}


int main() {
  struct node *head = NULL; // Baþlangýçta listeyi boþ olarak oluþtur
  int secenek, esik, kapasite, dugumSayisi = 0, secenek2, tekrar = 1;   //ilk secenek tekrardan cache olusturmak icin,2. ise dugum islemleri icin.
  char yeniAdres[50], dosyaAdi[50];
  do {
    printf("\nCache kapasitesini giriniz.\n");
    scanf("%d", &kapasite);
    printf("Cache esik degerini giriniz.\n");
    scanf("%d", &esik);
    printf("Dosyadan okumak icin 1, kendiniz adres girmek icin 2'ye basiniz.\n");
    scanf("%d", &secenek);
    if (secenek == 1) {
      printf("Dosya adi giriniz.\n");
      scanf("%s", dosyaAdi);
      fileRead(dosyaAdi, &head, &dugumSayisi, &kapasite, esik);
      clearList(&head);
      dugumSayisi = 0;
    } else {
      do {
        printf("Dugum eklemek icin 1, dugum silmek icin 2, sayac sorgulama icin 3, listeyi gormek icin 4, tum cache temizlemek icin 5'e basiniz.\n");
        scanf("%d", &secenek2);
        if (secenek2 == 1) {
          printf("Eklemek istediginiz adresi giriniz.\n");
          scanf("%s", yeniAdres);
          pushFront(&head, yeniAdres, &dugumSayisi);
          thresholdControl(&head,yeniAdres,esik);
          if(dugumSayisi>kapasite){
          	deleteLastNode(&head,&dugumSayisi);
		  }
		  printList(head);
        }
        if (secenek2 == 2) {
          printf("Silmek istediginiz adresi giriniz.\n");
          scanf("%s", yeniAdres);
          deleteNode(&head, yeniAdres,&dugumSayisi);
          printList(head);
        }
        if (secenek2 == 3) {
          printf("Sorgulamak istediginiz adresi giriniz.\n");
          scanf("%s", yeniAdres);
          query(head, yeniAdres);
        }
        if (secenek2 == 4) {
          printList(head);
        }
      } while (secenek2 != 5);
      printf("Cikmak icin 0'a basiniz.Baska sayiya basarsaniz yeni cache acilicaktir.\n");
      scanf("%d", &tekrar);
      if (tekrar != 0) {
        clearList(&head); // Belleði serbest býrak
        dugumSayisi = 0; // Düðüm sayýsýný sýfýrla
      }
    }
  } while (tekrar != 0);
  return 0;
}

