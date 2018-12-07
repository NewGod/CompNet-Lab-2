#include<cstdio>
int main(){
    FILE *fp;

    char returnData[64];

    fp = popen("/sbin/ifconfig", "r");

    while (fgets(returnData, 64, fp) != NULL)
    {
        printf("%s", returnData);
    }

    pclose(fp);
    return 0;
}
