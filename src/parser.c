#include "../include/network.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_FILES_PATH "./data/configs/"
#define MAX_FILENAME_LENGTH 30
#define MAX_LINE_LENGTH 1000
#define INDEX_TYPE_APPAREIL 0
#define INDEX_NB_DEVICE 0
#define INDEX_NB_LINK 2
#define INDEX_LINE_NET_INFO 0
#define INDEX_MAC_STRING_START 2
#define SIZE_MAC_STRING 17

typedef enum type_appareil {
  NONE,
  STATION,
  SWITCH,
} type_appareil;

static type_appareil get_type(char type) { return strtol(&type, NULL, 10); }

void display_mac(struct mac *mac) {
  char mac_string[SIZE_MAC_STRING];
  for (size_t i = 0; i < 6; i++) {
    printf("%02X:", mac->mac_array[i]);
    if (i == 5) {
      printf("%02X\n", mac->mac_array[i]);
    }
  }
}

void display_ip(ip *ip) {
  for (size_t i = 0; i < 4; i++) {
    uint8_t byte = (*ip) >> i * 8;
    printf("%d.", byte);
  }
  printf("\n");
}

static struct mac get_mac(char *mac_string) {
  // on initialise une struct de mac vide
  struct mac mac = {{0, 0, 0, 0, 0, 0}};

  size_t byteIndex = 0;
  // on parcours les char de la ligne pour récuperer le mac
  // on utilise SIZE_MAC_STRING parce que le mac est toujours de la même taille
  // (6 octet)
  for (size_t i = 0; i < SIZE_MAC_STRING; i += 3) {

    // on initialise un tableau de 3 char pour les 2 char de l'octet en hexa et
    // le char de fin '\0'
    char byte_string[3];

    // on recupère les deux lettres en hexa
    strncpy(byte_string, mac_string + i, 2);
    // on ajoute le char de fin de chaine
    byte_string[2] = '\0';

    // on ajoute le byte dans la struct
    mac.mac_array[byteIndex] = (uint8_t)strtol(byte_string, NULL, 16);

    byteIndex++;
  }
  display_mac(&mac);

  return mac;
}
void init_net_from_file(const char *filename) {

  char filepath[MAX_FILENAME_LENGTH];
  snprintf(filepath, MAX_FILENAME_LENGTH, "%s%s", CONFIG_FILES_PATH, filename);

  FILE *file = fopen(filepath, "r");

  if (file == NULL) {
    perror("Erreur de fopen dans init_net_from_file");
    exit(EXIT_FAILURE);
  }

  char line[MAX_LINE_LENGTH];
  size_t line_counter = 0;
  size_t nb_devices;
  size_t nb_links;

  // ajouter déclaration reseau ici
  //

  while (fgets(line, MAX_LINE_LENGTH, file)) {

    if (line_counter == INDEX_LINE_NET_INFO) {
      nb_devices = strtol(&line[INDEX_NB_DEVICE], NULL, 10);
      nb_links = strtol(&line[INDEX_NB_LINK], NULL, 10);
      goto end_while;
    }

    // on ajoute les appareils
    if (line_counter < nb_devices + 1) {
      int type = get_type(line[INDEX_TYPE_APPAREIL]);
      char mac_string[SIZE_MAC_STRING];
      strncpy(mac_string, &line[INDEX_MAC_STRING_START], SIZE_MAC_STRING);
      get_mac(mac_string);

      switch (type) {
      case STATION:
        break;
      case SWITCH:

        break;
      }
    }

    // on ajoute les liaisons
    if (line_counter < nb_links + nb_devices + 1) {
    }

  end_while:
    line_counter++;
  }
  printf("station %ld, liaison %ld\n test :", nb_devices, nb_links);

  fclose(file);
}

void test() { printf("test"); }
