int main() {
    int a = 3;
    int b = 2;
    int c;
    c = 5;
    a *= b + 5 / 14 + (c * 4);
    a /= b;
    b /= c;
    return a + b;
}