#include <stdio.h>
#include <stdint.h>

/* ------------------------------------------------------------------
   Fabrique le masque à partir de la longueur de préfixe.
   Principe : on part de 32 bits à 1, et on décale à gauche.
   Les bits libérés à droite sont remplis de zéros -> partie hôte.
   ------------------------------------------------------------------ */
uint32_t masque_depuis_prefixe(int prefixe)
{
    /* LE PIÈGE dont je te parlais : décaler un entier 32 bits de 32
       positions est un comportement INDÉFINI en C. Le résultat peut
       être n'importe quoi selon la machine, sans le moindre message
       d'erreur. On traite donc /0 séparément. */
    if (prefixe == 0) {
        return 0;
    }
    return 0xFFFFFFFF << (32 - prefixe);
}

/* ------------------------------------------------------------------
   Transforme "192.168.1.10/26" en (uint32_t, préfixe).
   Retourne 1 si tout s'est bien passé, 0 si l'entrée est invalide.
   ------------------------------------------------------------------ */
int parser_cidr(const char *entree, uint32_t *ip, int *prefixe)
{
    /* On lit LARGE (unsigned int) pour pouvoir détecter un dépassement.
       Lire directement dans un type étroit ferait déborder 999 en silence. */
    unsigned int a, b, c, d, p;

    /* sscanf retourne le NOMBRE de valeurs converties.
       On en attend 5 : si on en obtient moins, le format est mauvais. */
    if (sscanf(entree, "%u.%u.%u.%u/%u", &a, &b, &c, &d, &p) != 5) {
        return 0;
    }

    /* ... ENSUITE on valide les plages. */
    if (a > 255 || b > 255 || c > 255 || d > 255 || p > 32) {
        return 0;
    }

    /* Assemblage des 4 octets en un seul entier 32 bits.
       Chaque octet est poussé à sa place, puis fusionné avec un OU. */
    *ip = ((uint32_t)a << 24)
        | ((uint32_t)b << 16)
        | ((uint32_t)c <<  8)
        |  (uint32_t)d;

    *prefixe = (int)p;
    return 1;
}

/* ------------------------------------------------------------------
   Affiche un uint32_t en notation pointée.
   >> amène l'octet voulu tout à droite, & 0xFF efface le reste.
   ------------------------------------------------------------------ */
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
    uint32_t reseau = ip & masque;   /* le fameux ET logique */

    printf("Adresse saisie : ");  afficher_ip(ip);      printf("/%d\n", prefixe);
    printf("Masque         : ");  afficher_ip(masque);  printf("\n");
    printf("Adresse reseau : ");  afficher_ip(reseau);  printf("\n");

    return 0;
}