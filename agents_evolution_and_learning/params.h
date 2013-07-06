#ifndef PARAMS_H
#define PARAMS_H

#include <map>
#include <utility>
#include <string>
#include <iostream>

namespace settings{

  /// Интерфесный класс параметра
  class Param{
  public:
    explicit Param(const std::string paramName) : _paramName(paramName) {}
    virtual ~Param() {}
    /// Заполнение параметра из входного потока
    virtual void fill(std::istream& is) = 0;
    std::string name() const { return _paramName; }
    /// Создание копии параметра
    virtual Param* copy() const = 0;
  protected:
    std::string _paramName;
    /// Ярлыки типов данных
    enum TypeLabel{ DOUBLE, UNSIGNED_INT, INT, STRING };
    /// Вывод ярлыка типа
    template<typename T>
    TypeLabel inferLabel(const T& param) const;
  };

  /// Класс параметра с одним полем
  class SingleParam: public Param{
  public:
    /// Создание единичного параметра
    template<typename T>
    SingleParam(const std::string& paramName, T& paramData);
    /// Заполнение параметра из входного потока
    virtual void fill(std::istream& is);
    /// Создание копии параметра
    virtual SingleParam* copy() const { return new SingleParam(*this); };
  private:
    SingleParam();
    void* _pParamData;
    TypeLabel _paramType;
  };

  /// Класс параметра с двумя полями
  class DoubleParam: public Param{
  public:
    /// Создание двойного параметра
    template<typename T>
    DoubleParam(const std::string& paramName, T& paramData_1, T& paramData_2);
    /// Заполнение параметра из входного потока
    virtual void fill(std::istream& is);
    /// Создание копии параметра
    virtual DoubleParam* copy() const { return new DoubleParam(*this); };
  private:
    DoubleParam();
    std::pair<void*, void*> _pParamData;
    TypeLabel _paramType;
  };

  /// Класс списка параметров командной строки
  class PromtParams{
  public:
    ~PromtParams();
    /// Добавление параметра к списку параметров
    void addParam(const Param& param);
    /// Удаление параметра из списка
    bool deleteParam(const std::string& paramName);
    /// Заполнение параметров данными из командной строки
    void fillSettings(int argc, char** argv); 
  private:
    /// Карта параметров
    typedef std::map< std::string, Param*> ParamsMap;
    ParamsMap _params;
  };
  /// Потоковая запись параметров в список
  PromtParams& operator<<(PromtParams& params, const Param& param);

}

/// Вывод ярлыка типа
template<typename T>
settings::Param::TypeLabel settings::Param::inferLabel(const T& param) const{
  if (typeid(param) == typeid(double)) return DOUBLE;
  else if (typeid(param) == typeid(unsigned int)) return UNSIGNED_INT;
  else if (typeid(param) == typeid(int)) return INT;
  else if (typeid(param) == typeid(std::string)) return STRING;
  else {
    std::cout << "Error: settings::Param() - type is not allowed" << std::endl;
    std::abort();
  }
}

/// Создание единичного параметра
template<typename T>
settings::SingleParam::SingleParam(const std::string& paramName, T& paramData) : 
  Param(paramName), 
  _pParamData(static_cast<void*>(&paramData)), 
  _paramType(inferLabel(paramData)){
}

/// Создание двойного параметра
template<typename T>
settings::DoubleParam::DoubleParam(const std::string& paramName, T& paramData_1, T& paramData_2) : 
  Param(paramName), 
  _pParamData(std::make_pair(static_cast<void*>(&paramData_1), static_cast<void*>(&paramData_2))), 
  _paramType(inferLabel(paramData_1)){
}

#endif // PARAMS_H
