#ifndef PARAMTUNNEL_H
#define PARAMTUNNEL_H

#include <stdio.h>
#include <typeinfo>
#include <windows.h>
#include <process.h>
#include <UserPanel.h>

#define ApplyAllType(f) f(bool) f(int) f(unsigned) f(float) f(double) f(short) f(unsigned short) f(char) f(unsigned char) f(long) f(unsigned long)
class Param
{
public:
    Param(){ addr = NULL; }
#define MAKE_PARAM_CONSTRUCTOR(T) Param(T * data){ addr = (void*) data; type = typeid(*data).hash_code();}
    ApplyAllType(MAKE_PARAM_CONSTRUCTOR)
#undef MAKE_PARAM_CONSTRUCTOR
    ~Param(){}
    void set(double value){
        if (false){}
#define make_condition(T) else if (typeid(T).hash_code() == type){(*(T*)addr) = (T) value;}
        ApplyAllType(make_condition)
#undef make_condition
    }
    double get(){
        if (false){}
#define make_condition(T) else if (typeid(T).hash_code() == type){return (*(T*)addr);}
        ApplyAllType(make_condition)
#undef make_condition
        else{ return 0; }
    }

    string getType(){
        if(false){}
#define make_condition(T) else if (typeid(T).hash_code() == type){ return string(#T);}
        ApplyAllType(make_condition)
#undef make_condition
        else{ return "UNKNOWN";}
    }

private:
    void * addr;
    size_t type;
};
#undef ApplyAllType

typedef hash_map<string, Param, hash_compare<string, string_less> > ParamTable;

class Calibrator
{
public:
    Calibrator(){
        hthGUI = NULL;
        hthSYNC = NULL;
    }
    ~Calibrator(){
        if (NULL != hthGUI){
            CloseHandle(hthGUI);
            hthGUI = NULL;
        }
        if (NULL != hthSYNC){
            CloseHandle(hthSYNC);
            hthSYNC = NULL;
        }
    }
    void start(){
        unsigned thrdAddrGui, thrdAddrSync;
        hthGUI = (HANDLE)_beginthreadex(0, 0, gui, (void*)this, CREATE_SUSPENDED, &thrdAddrGui);
        hthSYNC = (HANDLE)_beginthreadex(0, 0, sync, (void*)this, CREATE_SUSPENDED, &thrdAddrSync);
        if (NULL == hthGUI || NULL == hthSYNC){
            printf("[Fatal Error]: 参数调试器线程创建失败\n");
        }
        else{
            ResumeThread(hthGUI);
            ResumeThread(hthSYNC);
        }
    }
    bool watch(Param param, string id){
        params[id] = param;
        return true;
    }
private:
    ParamTable params;
    UserPanel viewer;
    HANDLE hthGUI, hthSYNC;

    static unsigned __stdcall gui(void *_pThis){
        Calibrator * pThis = (Calibrator*)_pThis;
        vector<string> keys;
        for (ParamTable::iterator param = pThis->params.begin(); param != pThis->params.end(); ++param){
            string type = param->second.getType();
            string name = param->first;
            keys.push_back(name + "|" + type);
        }
        pThis->viewer.exec(keys);
        return 0;
    }

    static unsigned __stdcall sync(void *_pThis){
        Calibrator * pThis = (Calibrator*)_pThis;
        ParamSync sync_data;
        Sleep(500);
        while (1){
            pThis->viewer.getModifiedBuffer(sync_data);
            for (ParamSync::iterator item = sync_data.begin(); item != sync_data.end(); ++item){
                pThis->params[item->first].set(item->second);
            }
            for (ParamTable::iterator param = pThis->params.begin(); param != pThis->params.end(); ++param){
                sync_data[param->first] = param->second.get();
            }
            pThis->viewer.setParams(sync_data);
            Sleep(200);
        }
        return 0;
    }
};
#endif // PARAMTUNNEL_H
