int main() {
    for (int i = 0; i < 10; i++) { putchar('A'); } // OK
    int j = 0;
    do { putchar('B'); j++; } while (j < 5); // OK
    return 0;
}