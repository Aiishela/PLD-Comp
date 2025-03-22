int func2() {
    int a = 1+1;
    return 2;
}

int func3() {
    int b = func2();
    return 5;
}

int main() {
    int a;
    a = func2();
    a = func3();
    return a;
}

