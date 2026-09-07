#include <stdio.h>

int main(void)
{
    const char *tests[] = {
        "192.168.1.10/26",
        "10.0.0.1/8",
        "192.168.1",
        "bonjour",
        "999.1.1.1/24",
        "192.168.1.1/24nimportequoi",
        "   192.168.1.1/24",
        "192.168.1.1/",
        ""
    };

    
    int nb = sizeof(tests) / sizeof(tests[0]);


    
    for (int i = 0; i < nb; i++) {
        /* initialisées à 0 pour voir lesquelles sscanf a remplies */
        unsigned int a = 0, b = 0, c = 0, d = 0, p = 0;

        int converties = sscanf(tests[i], "%u.%u.%u.%u/%u",
                                &a, &b, &c, &d, &p);

        printf("  entree : \"%s\"\n", tests[i]);
        printf("  converties : %d\n", converties);
        printf("  a=%u b=%u c=%u d=%u p=%u\n\n", a, b, c, d, p);
    }

    return 0;
}