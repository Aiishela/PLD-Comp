int main (){
    int var1 = 0;
    int var2 = 14;
    int a ;
    while (var1 < var2) {
        var1++;
        a = var1 - var2--;
    }
    return var1 + var2 + a;    
}