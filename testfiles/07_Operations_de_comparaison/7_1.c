//Test qui marche
int main() {
    int c = 0;
    int a = 15;
    int b = 24;
    int d = 24;
    int e;
    a = a != b; // a = 1
    c = a == b; // c = 0
    d = a == d;  // c = 1
    c = c > a;  // c = 0
    e = c < 1;

    return a + b + c + d + e;
}