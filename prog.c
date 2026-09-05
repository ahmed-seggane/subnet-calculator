#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint8_t o[4];
} IPv4;

static IPv4 uint32_to_ip(uint32_t v) {
    IPv4 ip;
    ip.o[0] = (v >> 24) & 0xFF;
    ip.o[1] = (v >> 16) & 0xFF;
    ip.o[2] = (v >> 8) & 0xFF;
    ip.o[3] = v & 0xFF;
    return ip;
}

static uint32_t ip_to_uint32(IPv4 ip) {
    return ((uint32_t)ip.o[0] << 24) | ((uint32_t)ip.o[1] << 16) |
           ((uint32_t)ip.o[2] << 8) | (uint32_t)ip.o[3];
}

static void print_ip(const char *label, IPv4 ip) {
    printf("%-22s: %d.%d.%d.%d\n", label, ip.o[0], ip.o[1], ip.o[2], ip.o[3]);
}

static char classe_de(uint8_t premier_octet) {
    if (premier_octet < 128) return 'A';
    if (premier_octet < 192) return 'B';
    if (premier_octet < 224) return 'C';
    if (premier_octet < 240) return 'D';
    return 'E';
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <adresse_ip>/<prefixe>\n", argv[0]);
        fprintf(stderr, "Exemple: %s 192.168.1.10/24\n", argv[0]);
        return 1;
    }

    int a, b, c, d, prefixe;
    int n = sscanf(argv[1], "%d.%d.%d.%d/%d", &a, &b, &c, &d, &prefixe);

    if (n != 5 || a < 0 || a > 255 || b < 0 || b > 255 || c < 0 || c > 255 ||
        d < 0 || d > 255 || prefixe < 0 || prefixe > 32) {
        fprintf(stderr, "Erreur : format invalide. Attendu a.b.c.d/prefixe (0-255 et 0-32)\n");
        return 1;
    }

    IPv4 ip = { { (uint8_t)a, (uint8_t)b, (uint8_t)c, (uint8_t)d } };
    uint32_t ip_val = ip_to_uint32(ip);
    uint32_t masque_val = (prefixe == 0) ? 0 : (0xFFFFFFFFu << (32 - prefixe));
    uint32_t reseau_val = ip_val & masque_val;
    uint32_t diffusion_val = reseau_val | ~masque_val;

    IPv4 masque = uint32_to_ip(masque_val);
    IPv4 reseau = uint32_to_ip(reseau_val);
    IPv4 diffusion = uint32_to_ip(diffusion_val);

    uint64_t nb_adresses = 1ULL << (32 - prefixe);
    uint64_t nb_hotes_utiles = (prefixe >= 31) ? 0 : nb_adresses - 2;

    printf("=== Informations sur le sous-reseau ===\n");
    print_ip("Adresse IP", ip);
    printf("%-22s: /%d\n", "Prefixe (CIDR)", prefixe);
    print_ip("Masque de sous-reseau", masque);
    printf("%-22s: %c\n", "Classe", classe_de(ip.o[0]));
    print_ip("Adresse reseau", reseau);
    print_ip("Adresse de diffusion", diffusion);

    if (nb_hotes_utiles > 0) {
        IPv4 hote_min = uint32_to_ip(reseau_val + 1);
        IPv4 hote_max = uint32_to_ip(diffusion_val - 1);
        print_ip("Premiere adresse hote", hote_min);
        print_ip("Derniere adresse hote", hote_max);
    } else {
        printf("%-22s: aucune (prefixe /%d)\n", "Plage hotes", prefixe);
    }

    printf("%-22s: %llu\n", "Nombre d'hotes utiles", (unsigned long long)nb_hotes_utiles);

    return 0;
}
