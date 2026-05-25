#include "../include/network.h"
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

static struct mac get_mac(char *mac_string) {
  struct mac mac = {{0, 0, 0, 0, 0, 0}};

  // à corriger
  //   for (size_t i = 0; i < SIZE_MAC_STRING - 3; i += 3) {
  //     char byte_string[2];
  //     sprintf(byte_string, mac_string + i, 2);
  //     printf("string %s\n", byte_string);
  //   }

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
