int globalVar = 100; // OK
int main() {
    globalVar = globalVar + 10; // OK
    return globalVar;
}