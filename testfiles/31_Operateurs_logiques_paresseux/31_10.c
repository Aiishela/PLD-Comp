int main() {
    int x = 5;
    int y = 0;

    int z = x > 0 && y++ > 0;

    return z; // Doit retourner 1
}
