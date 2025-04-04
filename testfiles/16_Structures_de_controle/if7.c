int main (){
    int var1 = 0;
    int var2 = 1;
    if(var1 == var2){
        int a = 2;
        int c = 4;
    } else {
        int a = 2;
        c = 2; // error : c is not declared
    }
    return c + a;    
}