// Copyright (c) 2011 Fei Sun. All Rights Reserved.
// Author: Ofey.sunfei@gmail.com (Fei Sun)
//         tagami.yukihiro@gmail.com (Yukihiro TAGAMI)
// License: GPLv3 <http://www.gnu.org/copyleft/gpl.html>

#include "ce_via_td.h"

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstring>

#include <iostream>
#include <sstream>

#include "rapidxml_print.hpp"

static const char *kCharNum     = "charnum";
static const char *kLinkCharNum = "linkcharnum";
static const char *kTagNum      = "tagnum";
static const char *kAnchorNum   = "anchornum";
static const char *kRatio       = "ratio";
static const char *kRatioSum    = "ratiosum";
static const char *kMark        = "mark";

namespace {

inline int StrToInt(const char *s) {
  return static_cast<int>(strtol(s, NULL, 10));
}

inline double StrToDouble(const char *s) {
  return strtod(s, NULL);
}

void CleanTree(rapidxml::xml_node<> *node) {
  if (node == NULL) { return; }
  
  for (rapidxml::xml_node<> *child = node->first_node(); child;) {
    rapidxml::xml_node<> *next_node = child->next_sibling();
    rapidxml::node_type type = child->type();
    std::string name = child->name();
    if (type == rapidxml::node_comment ||
        name == "script" || name == "style") {
      node->remove_node(child);
    } else {
      CleanTree(child);
    }
    child = next_node;
  }

  return;
}

void CleanTreeByMark(rapidxml::xml_node<> *node) {
  if (node == NULL) { return; }

  for (rapidxml::xml_node<> *child = node->first_node(); child;) {
    rapidxml::xml_node<> *next_node = child->next_sibling();
    rapidxml::xml_attribute<> *attr_mark = child->last_attribute(kMark);
    std::string str_mark = attr_mark == NULL ? "0" : attr_mark->value();
    if (str_mark == "0") {
      node->remove_node(child);
    } else if (str_mark == "2") {
      CleanTreeByMark(child);
    }
    child = next_node;
  }

  return;
}

template <typename T>
void SetAttribute(const char* attr_name, const T value, 
                  rapidxml::xml_document<> *doc,
                  rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return; }

  std::stringstream ss;
  ss << value;
  const std::string &str_value = ss.str();

  rapidxml::xml_attribute<> *attr;
  if (node->first_attribute() != NULL &&
      node->last_attribute(attr_name) != NULL) {
    attr = node->last_attribute(attr_name);
    attr->value(doc->allocate_string(str_value.c_str()));
  } else {
    attr = doc->allocate_attribute(attr_name, doc->allocate_string(str_value.c_str()));
    node->append_attribute(attr);
  }

  return;
}

void SetMarkAttribute(int mark, rapidxml::xml_document<> *doc,
                      rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return; }
  SetAttribute(kMark, mark, doc, node);
  for (rapidxml::xml_node<> *child = node->first_node();
       child; child = child->next_sibling()) { 
    SetMarkAttribute(mark, doc, child);
  }
  return;
}
                   
int CountChar(rapidxml::xml_document<> *doc,
              rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return 0; }
  int char_num = 0;

  if (node->first_node() == NULL) {
    char_num = node->value_size();
  } else {
    for (rapidxml::xml_node<> *child = node->first_node();
         child; child = child->next_sibling()) {
      char_num += CountChar(doc, child);
    }

    const std::string name = node->name();
    if (name == "select") {
      char_num = StrToInt(node->first_node()->last_attribute(kCharNum)->value());
    }
  }
  SetAttribute(kCharNum, char_num, doc, node);

  return char_num;
}

int CountTag(rapidxml::xml_document<> *doc,
             rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return 0; }
  int tag_num = 0;
  rapidxml::node_type type = node->type();

  if (type == rapidxml::node_data) {
    tag_num = -1;
  } else if (node->value_size() == 0 && node->first_node() == NULL) {
    tag_num = 0;
  } else {
    for (rapidxml::xml_node<> *child = node->first_node(); child; child = child->next_sibling()) { 
      tag_num += CountTag(doc, child) + 1;
    }
  }
  SetAttribute(kTagNum, tag_num, doc, node);

  return tag_num;
}

void UpdateLinkChar(rapidxml::xml_document<> *doc,
                    rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return; }
  int link_char_num = 0;

  for (rapidxml::xml_node<> *child = node->first_node();
       child; child = child->next_sibling()) { 
    link_char_num = StrToInt(child->last_attribute(kCharNum)->value());
    SetAttribute(kLinkCharNum, link_char_num, doc, node);
    UpdateLinkChar(doc, child);
  }
  return;
}

int CountLinkChar(rapidxml::xml_document<> *doc,
                     rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return 0; }
  int link_char_num = 0;

  if (node->first_node() != NULL) {
    for (rapidxml::xml_node<> *child = node->first_node();
         child; child = child->next_sibling()) { 
      link_char_num += CountLinkChar(doc, child);
    }

    std::string name = node->name();
    if (name == "a" || name == "input" || name == "button" || name == "option") {
      link_char_num = StrToInt(node->last_attribute(kCharNum)->value());
      UpdateLinkChar(doc, node);
    }

    int char_num = StrToInt(node->last_attribute(kCharNum)->value());
    if (link_char_num > char_num) {
      link_char_num = char_num;
    }
  }
  SetAttribute(kLinkCharNum, link_char_num, doc, node);

  return link_char_num;
}

void UpdateAnchor(rapidxml::xml_document<> *doc,
                   rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return; }
  int anchor_num = 0;

  for (rapidxml::xml_node<> *child = node->first_node(); child; child = child->next_sibling()) { 
    anchor_num = StrToInt(child->last_attribute(kTagNum)->value());
    SetAttribute(kAnchorNum, anchor_num, doc, node);
    UpdateAnchor(doc, child);
  }
  return;
}

int CountAnchor(rapidxml::xml_document<> *doc,
                 rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return 0; }
  int anchor_num = 0;

  if (node->first_node() != NULL) {
    std::string name;
    for (rapidxml::xml_node<> *child = node->first_node();
         child; child = child->next_sibling()) { 
      int child_anchor_num = CountAnchor(doc, child);
      anchor_num += child_anchor_num;
      name = child->name();
      if (name == "a" || name == "input" || name == "button" || name == "option") {
        ++anchor_num;
      } else if (child_anchor_num > 0) {
        int child_tag_num       = StrToInt(child->last_attribute(kTagNum)->value());
        int child_char_num      = StrToInt(child->last_attribute(kCharNum)->value());
        int child_link_char_num = StrToInt(child->last_attribute(kLinkCharNum)->value());
        if (child_anchor_num == child_tag_num &&
            child_char_num == child_link_char_num) {
          ++anchor_num;
        }
      }
    }

    name = node->name();
    if (name == "a" || name == "input" || name == "button" || name == "option") {
      anchor_num = StrToInt(node->last_attribute(kTagNum)->value());
      UpdateAnchor(doc, node);
    }
  }
  SetAttribute(kAnchorNum, anchor_num, doc, node);

  return anchor_num;
}

double ComputeRatio(double density, rapidxml::xml_document<> *doc,
                    rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return 0.0; }

  int char_num = StrToInt(node->last_attribute(kCharNum)->value());

  double ratio = 0.0;
  if (char_num > 0) {
    int tag_num         = StrToInt(node->last_attribute(kTagNum)->value());
    int anchor_num      = StrToInt(node->last_attribute(kAnchorNum)->value());
    int link_char_num   = StrToInt(node->last_attribute(kLinkCharNum)->value());
    int unlink_char_num = char_num - link_char_num;
    int unanchor_num    = tag_num  - anchor_num;

    tag_num         = tag_num > 0         ? tag_num         : 1;
    link_char_num   = link_char_num > 0   ? link_char_num   : 1;
    anchor_num      = anchor_num > 0      ? anchor_num      : 1;
    unanchor_num    = unanchor_num > 0    ? unanchor_num    : 1;
    unlink_char_num = unlink_char_num > 0 ? unlink_char_num : 1;

    double base = log(static_cast<double>(char_num * link_char_num) / unlink_char_num +
                      density * char_num + exp(1.0));
    ratio = (static_cast<double>(char_num) / tag_num) * log(static_cast<double>(char_num * tag_num) / (link_char_num * anchor_num)) / log(base);
  }
  SetAttribute(kRatio, ratio, doc, node);

  double ratio_sum = 0.0;
  if (node->first_node() == NULL) {
    ratio_sum = ratio;
  } else {
    for (rapidxml::xml_node<> *child = node->first_node();
         child; child = child->next_sibling()) { 
      ratio_sum += ComputeRatio(density, doc, child);
    }
  }
  SetAttribute(kRatioSum, ratio_sum, doc, node);

  return ratio;
}


bool FindMaxRatioTag(double max_ratio_sum, rapidxml::xml_document<> *doc,
                     rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return false; }
  double ratio_sum = StrToDouble(node->last_attribute(kRatioSum)->value());
  std::string str_mark = node->last_attribute(kMark)->value();

  double tmp = max_ratio_sum / ratio_sum;
  if (1.0 - DBL_EPSILON <= tmp && tmp <= 1.0 + DBL_EPSILON) {
    if (str_mark != "1") {
      SetMarkAttribute(1, doc, node);
      for (rapidxml::xml_node<> *parent = node->parent();
           parent != doc->first_node(); parent = parent->parent()) {
        SetAttribute(kMark, 2, doc, parent);
      }
    }
    return true;
  } else {
    rapidxml::xml_node<> *child = node->first_node();
    for (; child; child = child->next_sibling()) { 
      if (FindMaxRatioTag(max_ratio_sum, doc, child)) {
        return true;
      }
    }
  }

  return false;
}

double FindMaxRatioSum(rapidxml::xml_document<> *doc,
                       rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return 0.0; }
  double max_ratio_sum = StrToDouble(node->last_attribute(kRatioSum)->value());
  for (rapidxml::xml_node<> *child = node->first_node();
       child; child = child->next_sibling()) { 
    double ratio_sum = FindMaxRatioSum(doc, child);
    if (ratio_sum > max_ratio_sum - DBL_EPSILON) {
      max_ratio_sum = ratio_sum;
    }
  }
  return max_ratio_sum;
}

double FindMaxRatioSum(double threshold, rapidxml::xml_document<> *doc,
                       rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return 0.0; }
  double ratio = StrToDouble(node->last_attribute(kRatio)->value());
  double max_ratio_sum = StrToDouble(node->last_attribute(kRatioSum)->value());

  if (node->first_node() == NULL && ratio >= threshold - DBL_EPSILON) {
    FindMaxRatioTag(max_ratio_sum, doc, node);
  } else if (ratio >= threshold - DBL_EPSILON) {
    for (rapidxml::xml_node<> *child = node->first_node();
         child; child = child->next_sibling()) { 
      double ratio_sum = FindMaxRatioSum(threshold, doc, child);
      if (ratio_sum > max_ratio_sum) {
        max_ratio_sum = ratio_sum;
      }
    }
    FindMaxRatioTag(max_ratio_sum, doc, node);
  }

  return max_ratio_sum;
}

double GetThreshold(double max_ratio_sum, rapidxml::xml_document<> *doc,
                    rapidxml::xml_node<> *node) {
  if (doc == NULL || node == NULL) { return 0.0; }
  double ratio_sum = StrToDouble(node->last_attribute(kRatioSum)->value());
  double threshold = -1.0;

  double tmp = max_ratio_sum / ratio_sum;
  if (1.0 - DBL_EPSILON < tmp && tmp < 1.0 + DBL_EPSILON) {
    SetMarkAttribute(1, doc, node);
    threshold = StrToDouble(node->last_attribute(kRatio)->value());
    for (rapidxml::xml_node<> *parent = node->parent();
         parent != doc->first_node(); parent = parent->parent()) {
      double ratio = StrToDouble(parent->last_attribute(kRatio)->value());
      if (threshold > ratio - DBL_EPSILON) { threshold = ratio; }
      SetAttribute(kMark, 2, doc, parent);
    }

  } else {
    for (rapidxml::xml_node<> *child = node->first_node();
         child; child = child->next_sibling()) { 
      threshold = GetThreshold(max_ratio_sum, doc, child); 
      if (threshold > 0.0) { break; }
    }
  }

  return threshold;
}

void PrintToText(rapidxml::xml_node<> *node, std::string *text) {
  if (node == NULL || text == NULL) { return; }

  rapidxml::node_type type = node->type();
  if (type == rapidxml::node_data) {
    *text += node->value();
  } else {
    for (rapidxml::xml_node<> *child = node->first_node();
         child; child = child->next_sibling()) { 
      PrintToText(child, text);
    }
  }
  return;
}

} // namespace


CEviaTD::CEviaTD() : doc_() {}

CEviaTD::~CEviaTD() {
  doc_.clear();
}

int CEviaTD::Parse(const char *xml) {
  if (xml == NULL) { return -1; }
  size_t xml_len = strlen(xml);
  if (xml_len == 0) { return -1; }
  
  char c_buff[xml_len + 1];
  strncpy(c_buff, xml, xml_len + 1);
  c_buff[xml_len] = '\0';

  extracted_xml_.clear();
  extracted_text_.clear();
  
  try {
    doc_.parse<0>(c_buff);
  } catch(...) {
    return -1;
  }

  rapidxml::xml_node<> *root = doc_.first_node();
  rapidxml::xml_node<> *head = root->first_node();
  rapidxml::xml_node<> *body = head->next_sibling();

  CleanTree(body);

  CountChar(&doc_, body);
  CountTag(&doc_, body);
  CountLinkChar(&doc_, body);
  CountAnchor(&doc_, body);

  double char_num = StrToDouble(body->last_attribute(kCharNum)->value());
  double link_char_num = StrToDouble(body->last_attribute(kLinkCharNum)->value());
  double density = link_char_num / char_num;

  ComputeRatio(density, &doc_, body);

  SetMarkAttribute(0, &doc_, body);

  double max_ratio_sum = FindMaxRatioSum(&doc_, body);
  double threshold = GetThreshold(max_ratio_sum, &doc_, body);
  max_ratio_sum = FindMaxRatioSum(threshold, &doc_, body);

  CleanTreeByMark(body);

  rapidxml::print(std::back_inserter(extracted_xml_), doc_, rapidxml::print_no_indenting);
  PrintToText(root, &extracted_text_);

  return 1;
}

