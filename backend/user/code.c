#include <stdio.h>

int main() {
    int _t3 = 0;
    int _t0 = 0;
    int z = 0;
    int y = 0;
    int x = 0;
    x = 7;
    y = 10;
    z = 7;
    _t0 = (x == z);
    if (!_t0) goto L1;
    printf("Equal1\n");
    goto L2;
L1:
L2:
    _t3 = (z == y);
    if (!_t3) goto L4;
    printf("Equal2\n");
    goto L5;
L4:
    printf("Not equal");
L5:
    return 0;
}
