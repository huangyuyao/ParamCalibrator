#include <iostream>
using namespace std;
#include<ParamCalibrator.h>

int main()
{
    bool bv = false;
    int iv  = 1;
    unsigned uv = 1;
    float fv = 1.0f;
    double dv = 1.0;

    Calibrator cal;

    cal.watch(Param(&iv), "iv");
    cal.watch(Param(&bv), "bv");
    cal.watch(Param(&dv), "dv");

    cal.start();

    while(1){
        printf("iv=%d\tbv=%d\tdv=%f\n", iv, bv, dv);
        Sleep(2000);
    }

    return 0;
}
