#ifndef PARAMPANEL_H
#define PARAMPANEL_H

#include <vector>
#include <string>
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include <hash_map>

using namespace std;
using namespace stdext;

#ifndef BUILD_STATIC
# if defined(PARAMPANEL_LIBRARY)
#  define PARAMPANEL_EXPORT __declspec(dllexport)
# else
#  define PARAMPANEL_EXPORT __declspec(dllimport)
# endif
#else
# define PARAMPANEL_EXPORT
#endif

struct string_less : public binary_function<const string, const string, bool>
{
public:
    result_type operator()(const first_argument_type& _Left, const second_argument_type& _Right) const
    {
        return(_Left.compare(_Right) < 0);
    }
};

typedef hash_map<string, double, hash_compare<string, string_less> > ParamSync;

class PARAMPANEL_EXPORT UserPanel
{
public:
    UserPanel();
    ~UserPanel();
    void exec(vector<string> keys);
    void getModifiedBuffer(ParamSync& data);
    void setParams(ParamSync& data);
};

#endif // PARAMPANEL_H
