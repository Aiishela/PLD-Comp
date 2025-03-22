int main() {
    int x = 5; // OK
    if (x > 0) {
        int y = 10; // OK
    }
    printf("%d", y); // Erreur (y hors de portée)
}