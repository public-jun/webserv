#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>

enum EDrctvCd {
    SRVR,
    LCTN,
    LSTN,
    SRVR_NM,
    MX_CLNT_BDY_SZ,
    ALLWD_MTHD,
    ALIAS,
    AUTO_INDX,
    INDX,
    RTRN,
    ERR_PG,
    CGI,
    ROOT,
    UPLD_PATH,
    MAIN,
};

enum EPosType {
    BEGIN,
    END,
};

enum EAllowedMothodType {
    GET,
    POST,
    DELETE,
};

enum EAutoIndexType {
    ON,
    OFF,
};

typedef std::vector<const std::string> str_vec;
typedef str_vec::iterator              str_vec_itr;

class Utils {
public:
    Utils();
    Utils(const Utils& src);
    ~Utils();
    Utils& operator=(const Utils& src);

    static void FindEndBrace(str_vec_itr& it);
};

#endif
