#include<cstdio>
int main(){
    int T=10000;
    while(T--){
        FILE *fp;
        fp=fopen("try.in","w");
        fprintf(fp,"h");
        fclose(fp);
    }
}
