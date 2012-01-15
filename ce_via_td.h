// Copyright (c) 2011 Fei Sun. All Rights Reserved.
// Author: Ofey.sunfei@gmail.com (Fei Sun)
//         tagami.yukihiro@gmail.com (Yukihiro TAGAMI)
// License: GPLv3 <http://www.gnu.org/copyleft/gpl.html>

#ifndef CE_VIA_TD_H
#define CE_VIA_TD_H

#include <string>

#include "rapidxml.hpp"

class CEviaTD {
  public:
    CEviaTD();
    ~CEviaTD();
    int Parse(const char *xml);
    const char* GetExtractedXml()  const { return extracted_xml_.c_str(); };
    const char* GetExtractedText() const { return extracted_text_.c_str(); };
  private:
    rapidxml::xml_document<> doc_;
    std::string extracted_xml_;
    std::string extracted_text_;
    CEviaTD(const CEviaTD&);
    void operator=(const CEviaTD&);
};

#endif // CE_VIA_TD_H
