int main() {
    int j = 0;
    int i = 0;
    putchar('A');
    putchar('\n');
    putchar('A');
    putchar('\n');
    putchar('A');
    putchar('\n');

    while (j < 5){
        putchar('B');
        putchar('\n');
        i++;
        j++;
    } // OK
    return i;
}