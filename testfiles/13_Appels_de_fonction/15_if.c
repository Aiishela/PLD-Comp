int func2() {
    int b = 1+1;
    return b;
}

int func3() {
    int c = func2();
    return c;
}

int main() {
    int a;
    a = func3();
    return a;
}

