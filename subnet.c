#include <stdio.h>
#include <stdint.h>

uint32_t masque_depuis_prefixe(int prefixe)
{
    /* decaler un entier 32 bits de 32 positions est indefini en C */
    if (prefixe == 0) {
        return 0;
    }
    return 0xFFFFFFFF << (32 - prefixe);
}

/* retourne 1 si l'entree est valide, 0 sinon */
int parser_cidr(const char *entree, uint32_t *ip, int *prefixe)
{
    /* lire large pour qu'un octet hors plage reste detectable */
    unsigned int a, b, c, d, p;

    /* sscanf retourne le nombre de valeurs converties */
    if (sscanf(entree, "%u.%u.%u.%u/%u", &a, &b, &c, &d, &p) != 5) {
        return 0;
    }

    if (a > 255 || b > 255 || c > 255 || d > 255 || p > 32) {
        return 0;
    }

    *ip = ((uint32_t)a << 24)
        + ((uint32_t)b << 16)
        + ((uint32_t)c <<  8)
        +  (uint32_t)d;

    *prefixe = (int)p;
    return 1;
}

void afficher_ip(uint32_t ip)
{
    printf("%u.%u.%u.%u",
           (ip >> 24) & 0xFF,
           (ip >> 16) & 0xFF,
           (ip >>  8) & 0xFF,
            ip        & 0xFF);
}

int main(int argc, char *argv[])
{
    uint32_t ip;
    int prefixe;

    if (argc != 2) {
        fprintf(stderr, "Usage : %s a.b.c.d/prefixe\n", argv[0]);
        return 1;
    }

    if (!parser_cidr(argv[1], &ip, &prefixe)) {
        fprintf(stderr, "Erreur : format invalide. "
                        "Attendu a.b.c.d/prefixe (0-255 et 0-32)\n");
        return 1;
    }

    uint32_t masque = masque_depuis_prefixe(prefixe);
    uint32_t reseau = ip & masque;
    uint32_t broadcast = reseau | ~masque;

    
    unsigned int premier = (ip >> 24) & 0xFF;
    char classe = (premier < 128) ? 'A'
                : (premier < 192) ? 'B'
                : (premier < 224) ? 'C'
                : (premier < 240) ? 'D' : 'E';

    printf("Adresse saisie    : ");  afficher_ip(ip);         printf("/%d\n", prefixe);
    printf("Classe            : %c\n", classe);
    printf("Masque            : ");  afficher_ip(masque);     printf("\n");
    printf("Adresse reseau    : ");  afficher_ip(reseau);     printf("\n");
    printf("Adresse broadcast : ");  afficher_ip(broadcast);  printf("\n");

    return 0;
}