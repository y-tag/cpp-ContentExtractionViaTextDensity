// Copyright (c) 2011 Fei Sun. All Rights Reserved.
// Author: Ofey.sunfei@gmail.com (Fei Sun)
//         tagami.yukihiro@gmail.com (Yukihiro TAGAMI)
// License: GPLv3 <http://www.gnu.org/copyleft/gpl.html>

#ifndef HTML_TIDY_H
#define HTML_TIDY_H

#include <string>

class HTMLTidy {
  public:
    static int tidy(const std::string &input, const std::string &in_enc,
                    std::string *output);
  private:
    HTMLTidy();
    ~HTMLTidy();
    HTMLTidy(const HTMLTidy&);
    void operator=(const HTMLTidy&);
};

#endif // HTMLTidy_H
