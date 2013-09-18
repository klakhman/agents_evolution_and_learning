#include "params.h"
#include <sstream>

using namespace std;

/// Заполнение параметра из входного потока
void settings::SingleParam::fill(istream& is){
  switch (_paramType){
    case DOUBLE:
      is >> *static_cast<double*>(_pParamData); 
      break;
    case UNSIGNED_INT:
      is >> *static_cast<unsigned int*>(_pParamData);
      break;
    case INT:
      is >> *static_cast<int*>(_pParamData);
      break;
    case STRING:
      is >> *static_cast<string*>(_pParamData);
    break;
  }
}

/// Заполнение параметра из входного потока
void settings::DoubleParam::fill(istream& is){
  switch (_paramType){
    case DOUBLE:
      is >> *static_cast<double*>(_pParamData.first); 
      is >> *static_cast<double*>(_pParamData.second);
      break;
    case UNSIGNED_INT:
      is >> *static_cast<unsigned int*>(_pParamData.first);
      is >> *static_cast<unsigned int*>(_pParamData.second);
      break;
    case INT:
      is >> *static_cast<int*>(_pParamData.first);
      is >> *static_cast<int*>(_pParamData.second);
      break;
    case STRING:
      is >> *static_cast<string*>(_pParamData.first);
      is >> *static_cast<string*>(_pParamData.second);
    break;
  }
}

/// Добавление параметра к списку параметров
void settings::PromtParams::addParam(const Param& param){
  const string paramName = param.name();
  if (_params.find(paramName) == _params.end())
    _params.insert(std::make_pair(paramName, param.copy()));
  else {
    std::cout << "Error: service::PromtParams::addParam - parameter is already in the list" << std::endl;
    std::abort();
  }
}

/// Удаление параметра из списка
bool settings::PromtParams::deleteParam(const string& paramName){
  ParamsMap::iterator param;
  if ((param = _params.find(paramName)) != _params.end()){
    delete param->second;
    _params.erase(param);
    return true;
  }
  else
    return false;
}

/// Заполнение параметров данными из командной строки
void settings::PromtParams::fillSettings(int argc, char** argv){
  stringstream promtBuf;
  for (int paramN = 1; paramN < argc; ++ paramN)
    promtBuf << argv[paramN] << " ";
  while (!promtBuf.eof()){
    ParamsMap::iterator param;
    string paramName;
    promtBuf >> paramName;
    // В самом начале имени параметра должен идти символ "-"
    if (paramName.length() > 1) {
      paramName = paramName.substr(1);
      if ((param = _params.find(paramName)) != _params.end())
        param->second->fill(promtBuf);
    }
  }
}

settings::PromtParams::~PromtParams(){
  for (ParamsMap::iterator param = _params.begin(); param != _params.end(); ++param)
    delete param->second;
}

/// Потоковая запись параметров в список
settings::PromtParams& settings::operator<<(PromtParams& params, const Param& param){
  params.addParam(param);
  return params;
}
