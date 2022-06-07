#ifndef UTILS_HPP
# define UTILS_HPP

#include <vector>
#include <iostream>

enum e_drctv_cd {
  SRVR,
  LCTN,
  LSTN,
  SRVR_NM,
  MX_CLNT_BDY_SZ,
  ALLWD_MTHD,
  ROOT,
  AUTO_INDX,
  INDX,
  RTRN,
  ERR_PG,
  CGI,
  MAIN,
};

enum e_pos_type {
  BEGIN,
  END,
};

enum e_allowed_mothod_type {
  GET,
  POST,
  DELETE,
};

enum e_auto_index_type {
  ON,
  OFF,
};

typedef std::vector<const std::string> str_vec;
typedef str_vec::iterator str_vec_itr;

class Utils {
public:
  Utils();
  Utils(const Utils &src);
  ~Utils();
  Utils &operator=(const Utils &src);

  static void findEndBrace(str_vec_itr &it);
};


#endif
