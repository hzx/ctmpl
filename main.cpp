#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <algorithm>
#include <sstream>


char buf[4096];


class Param {
public:
  Param(const char* param) {
    char *delim = strchr(param, '=');
    if (delim == NULL) {
      std::cout << "bad param: '" << param << "'" << std::endl;
    }
    name.assign(param, delim-&param[0]);
    /* value.assign(delim+1, &param[strlen(param)]-delim); */
    value.assign(delim+1);
  }

  std::string name;
  std::string value;
  std::string content;
};


typedef std::vector<Param> ParamVector;


class Parser {
public:
  Parser() {
  }

  Param* getParam(ParamVector &params, std::string &name) {
    for (ParamVector::iterator i = params.begin(); i < params.end(); ++i) {
      if (i->name == name) {
        return &(*i);
      }
    }
    return NULL;
  }

  char* findOpenBraces(char *str) {
    for (char *i = str; i != &str[strlen(str)]; ++i) {
      if (*i == '{' && *(i+1) == '{') {
        return i;
      }
    }
    return NULL;
  }

  char* findCloseBraces(char *str) {
    for (char *i = str; i != &str[strlen(str)]; ++i) {
      if (*i == '}' && *(i+1) == '}') {
        return i;
      }
    }
    return NULL;
  }

  const char* renderTmpl(const char *tmpl, ParamVector &params) {
    std::ifstream itmpl(tmpl);
    std::ostringstream buf;
    buf << itmpl.rdbuf();

    std::string templateContent = buf.str();

    /* std::cout << "tmpl: " << tmpl << std::endl; */
    /* std::cout << buf.str(); */

    std::ostringstream tmplbuf;

    size_t rtmplSize = templateContent.size();
    for (ParamVector::iterator i = params.begin(); i != params.end(); ++i) {
      rtmplSize += i->content.size();
    }

    std::string rtmpl;
    std::string paramName;
    rtmpl.reserve(rtmplSize);

    char *tmplpos = const_cast<char*>(templateContent.c_str());
    char *cur = NULL;
    char *endPos = NULL;
    for (ParamVector::iterator i = params.begin(); i != params.end(); ++i) {
      // parse tmpl
      cur = strstr(tmplpos, "{{");
      /* cur = findOpenBraces(tmplpos); */
      if (cur == NULL) {
        std::cout << "cur == NULL" << std::endl;
        std::cout << "paramName = " << i->name << std::endl;
        std::cout << "paramValue = " << i->value << std::endl;
        std::cout << "tmplpos: " << std::endl << tmplpos;
        return NULL;
      }
      endPos = strstr(cur, "}}");
      /* endPos = findCloseBraces(cur); */
      if (endPos == NULL) {
        std::cout << "endPos == NULL" << std::endl;
        std::cout << "paramName = " << i->name << std::endl;
        std::cout << "paramValue = " << i->value << std::endl;
        return NULL;
      }
      // copy curr tmpl part and param content
      rtmpl.append(tmplpos, cur-tmplpos);
      paramName.assign(cur+2, endPos);

      Param *param = getParam(params, paramName);
      /* std::cout << param->name << std::endl; */
      /* std::cout << param->content << std::endl; */

      rtmpl.append(param->content);

      tmplpos = endPos + 2;
    }
    rtmpl.append(tmplpos);

    /* std::cout << rtmpl << std::endl; */

    return rtmpl.c_str();
  }
private:
};


int main(int argc, char **argv) {
  /* std::cout << "argc = " << argc << std::endl; */

  char *dest = argv[1];
  char *src = argv[2];

  std::cout << "ctmpl: render template" << std::endl;
  std::cout << "dest: " << dest << std::endl;
  std::cout << "src: " << src << std::endl;

  /* std::cout << "dest = " << dest << std::endl; */
  /* std::cout << "src = " << src << std::endl; */

  ParamVector params;
  if (argc >= 3) {
    for (int i = 3; i < argc; ++i) {
      const char *raw = argv[i];
      params.push_back(Param(raw));
    }
  }

  for (ParamVector::iterator i = params.begin(); i != params.end(); ++i) {
    std::ifstream inp(i->value.c_str());
    std::ostringstream out;
    out << inp.rdbuf();
    i->content = out.str();
  }

  /* for (ParamVector::iterator i = params.begin(); i != params.end(); ++i) { */
  /*   std::cout << i->content; */
  /* } */

  Parser parser;
  const char *rtmpl = parser.renderTmpl(src, params);
  if (rtmpl != NULL) {
    std::ofstream out(dest);
    out << rtmpl;
  } else {
    std::cout << "rtmpl is null" << std::endl;
  }

  return 0;
}
