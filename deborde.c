#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *t = malloc(5 * sizeof(int));
    if (t == NULL) return 1;
    t[5] = 42;                    /* une case trop loin */
    printf("%d\n", t[5]);
    free(t);
    return 0;
}
