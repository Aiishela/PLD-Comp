int main (){
    int var1 = 0;
    int var2 = 1;
    if(var1 == var2){
        int a = 2;
    } else {
        int a = 2;
    }
    int c = a; // error : a is not defined
    return c + a;    
}