#include "RestrictedHypercubeEnv.h"

#include <algorithm>
#include <vector>
#include <string>

using namespace std;

/// Метод задания случайного состояния среды.
/**
* Данный метод не только задает случайное состояние среды, но также и перезагружает среду: обнуляет текущую жизнь агент и награду, восстанавливает все цели.
*/
void RestrictedHypercubeEnv::setRandomEnvironmentState(){
  THypercubeEnvironment::setRandomEnvironmentState();
  copy(currentEnvironmentVector, currentEnvironmentVector + environmentResolution, startEnvironmentVector.begin());
  resetEnvironment();
}

// Вспомогательная процедура для сортировке в методе воздействия на среду (при определении досигнутых целей)
class AimsNumViaCompl{
public:
  unsigned int aimNumber;
  unsigned int aimComplexity;
  AimsNumViaCompl(unsigned int _aimNumber, unsigned int _aimComplexity):
    aimNumber(_aimNumber), aimComplexity(_aimComplexity) {}
  friend bool operator<(const AimsNumViaCompl& left, const AimsNumViaCompl& right);
};

bool operator<(const AimsNumViaCompl& left, const AimsNumViaCompl& right) {
  return (left.aimComplexity < right.aimComplexity);
}

/// Метод воздействия на среду 
/**
* В отличие от обычной среды-гиперкуба более вычислительно затратен, так как он проверяет было ли достижения какой-либо цели и если было, но цель еще не восстановилась, то запрещает такой переход.
* Общий дополнительный формальный алгоритм: действие разрешено, если хотя бы одна цель его допускает (есть такая цель, которая достигнута на этом действии, но уже восстановлена)
* или ни одна цель его не запрещает (не достигается ни одной цели). Кроме данного условия, если достигается более длинная цель в иерархии, то сбрасывается она, а более общие цели не сбрасываются 
* и награда за их достижение не добавляется к накопленной.
* \param [in] action - вектор необходимого воздействия на среду.
* \return Показатель успешности воздействия на среду (0 - действие успешно, 1 - неуспешно).
*/
int RestrictedHypercubeEnv::forceEnvironment(const vector<double>& action){
  // Признак успешности совершаемого действия
	bool actionSuccess = false;
	//Направление изменения
	bool changeDirection = (action.at(0) > 0);
	// Номер изменяемого бита
	int changeBit = static_cast<int>(fabs(action.at(0)) + 0.1); // Добавляем 0.1, чтобы избежать неверного трактования double
  // Сразу записываем совершенное действие в запись жизни (если оно не успешно, то потом заменим его на 0)
  agentLifeRecord.push_back(action);
  // Теперь проверяем можно ли совершить данное действие 
  // Если был передан ноль, то агент бездействует (сразу выходим)
  if (changeBit == 0) return true;
	if (currentEnvironmentVector[changeBit -1] != changeDirection){
    // Если бит в противоположной позиции, то действие может быть совершено и мы проверяем не противоречит ли это какой-нибудь цели
    vector<int> reachedAims = testReachingAims(agentLifeRecord, agentLifeRecord.size());
    if (reachedAims.size()){
      // Так как на одном шаге могут быть достигнуты цели _только_ из одной ветки, то нам надо просто выбрать максимальную восстановленную
      vector<AimsNumViaCompl> aimsHierarchy;
      for (vector<int>::iterator aim = reachedAims.begin(); aim != reachedAims.end(); ++aim)
        aimsHierarchy.push_back(AimsNumViaCompl((*aim), aimsSet.at(*aim - 1).aimComplexity));
      sort(aimsHierarchy.begin(), aimsHierarchy.end());
      actionSuccess = false;
      // Проходимся начиная с конца (так как более длинные цели в конце)
      for (vector<AimsNumViaCompl>::reverse_iterator aim = aimsHierarchy.rbegin(); aim != aimsHierarchy.rend(); ++aim)
        if (agentLifeRecord.size() - reachedAimsTimes.at((*aim).aimNumber - 1) >= static_cast<unsigned int>(rewardRecoveryTime)){
          actionSuccess = true;
          reachedAimsTimes.at((*aim).aimNumber - 1) = agentLifeRecord.size();
          currentAgentReward += aimsSet.at((*aim).aimNumber - 1).reward;
          break;
        }
    }
    else
      actionSuccess = true;
	}
	else 
    actionSuccess = false;
  // Теперь либо меняем вектор состояния среды (если действие успешно), либо переписываем действие (если оно неуспешно)
  if (actionSuccess)
    currentEnvironmentVector[changeBit - 1] = changeDirection;
  else
    agentLifeRecord.at(agentLifeRecord.size() - 1).at(0) = 0;
	randomizeEnvironment();

	return actionSuccess;
}

/// Метод задания вектора среды (по значению целого вектора)
/**
* \param [in] environmentVector - вектор среды, который необходимо задать
*/
void RestrictedHypercubeEnv::setEnvironmentVector(double environmentVector[]) {
  THypercubeEnvironment::setEnvironmentVector(environmentVector);
  copy(currentEnvironmentVector, currentEnvironmentVector + environmentResolution, startEnvironmentVector.begin());
  resetEnvironment();
}

/// Метода задания вектора среды (по номеру - начиная с нуля) 
/**
* \param [in] stateNumber - требуемый номер состояния среды, начиная с нуля
*/
void RestrictedHypercubeEnv::setEnvironmentState(double stateNumber){
  THypercubeEnvironment::setEnvironmentState(stateNumber);
  copy(currentEnvironmentVector, currentEnvironmentVector + environmentResolution, startEnvironmentVector.begin());
  resetEnvironment();
}

/// Метод загрузки структуры среды из файла (вместе с целями среды).
/**
* \param [in] environmentFilename - имя файла со структурой целей в среде.
*/
void RestrictedHypercubeEnv::loadEnvironment(string environmentFilename){
  THypercubeEnvironment::loadEnvironment(environmentFilename);
  startEnvironmentVector.resize(environmentResolution);
  reachedAimsTimes.resize(aimsSet.size());
}

// Вспомогательная функция для генерации пула целей некоторой сложности на основе стартовых целей меньшей сложности
void generateComplexityLevelAims(vector<RestrictedHypercubeEnv::TAim>& desiredComplexityAims, const vector<RestrictedHypercubeEnv::TAim>& parentComplexityAims, 
                                 unsigned int environmentResolution, unsigned int desiredComplexity, unsigned int aimsQuantity){
  for (unsigned int aimN = 0; aimN < aimsQuantity; ++aimN){
    bool aimUnique = true;
    do{
      unsigned int parentAimN = service::uniformDiscreteDistribution(1, parentComplexityAims.size());
      RestrictedHypercubeEnv::TAim aim = 
        RestrictedHypercubeEnv::generateSelfConsistentAim(environmentResolution, desiredComplexity, parentComplexityAims.at(parentAimN - 1)); 
      aimUnique = true;
      for (vector<RestrictedHypercubeEnv::TAim>::const_iterator compareAim = desiredComplexityAims.begin(); compareAim != desiredComplexityAims.end(); ++compareAim)
        if (RestrictedHypercubeEnv::equalAims(*compareAim, aim)){
          aimUnique = false;
          break;
        }
      if (aimUnique)
        desiredComplexityAims.push_back(aim);
    } while (!aimUnique);
  }
}

/// Статический метод генерации среды по заданным параметрам
/**
* "Фабричный" метод генерации среды. 
* В отличие от генерации обычной среды-гиперкуба здесь происходит генерация по заданному количеству целей 
* (вычисляемому по необходимому коэффициенту заполненности). Распределение количества целей по длинам происходит 
* пропорционально - кол-во целей длины k' пропорционально кол-ву целей длины k'-1 в соответствии с некоторым 
* коэффициентом. Сначала происходит генерация целей длины minAimComplexity, за тем к ним в начало (т.е. слева) 
* добавляются действия, чтобы сделать иерархию.
* \param [in] _environmentResolution - размерность среды.
* \param [in] requiredOccupancyCoef - требуемая плотность целей в среде.
* \param [in] maxAimComplexity - максимальная сложность целей в среде.
* \param [in] minAimComplexity - минимальная сложность целей в среде.
* \param [in] eps - точность генерации среды в соответствии с заданной 
* плотностью целей (в данной реализации среды это значение не релеватно).
* \return Указатель на созданную среду с иерархией целей.
*/
RestrictedHypercubeEnv* RestrictedHypercubeEnv::generateEnvironment(unsigned int _environmentResolution, double requiredOccupancyCoef, 
                                                                    unsigned int maxAimComplexity/*=5*/, unsigned int minAimComplexity/*=2*/, 
                                                                    double eps/*=0.0005*/){                                                                     
  // Желамое распределение целей по длинам
  vector<unsigned int> desiredAimsDistr(maxAimComplexity - minAimComplexity + 1, 0);
  //  !!! Как-то заполняем желаемое распределение !!!
  const unsigned int increaseCoef = 5;
  const unsigned int firstComplAimsQ = 25;
  desiredAimsDistr.at(0) = firstComplAimsQ;
  for (vector<unsigned int>::iterator complexity = desiredAimsDistr.begin() + 1; complexity != desiredAimsDistr.end(); ++complexity)
    *complexity = (*(complexity - 1)) * increaseCoef;
  //const unsigned int _desiredAimsDistr[] = {1, 5, 25, 125};
  //copy(_desiredAimsDistr, _desiredAimsDistr + sizeof(_desiredAimsDistr)/sizeof(*_desiredAimsDistr), desiredAimsDistr.begin()); 
  // Для оптимизации вычислений (за счет пространства) необходимо хранить сгрупированные по сложности цели
  vector< vector<TAim> > aimsViaComplexity(maxAimComplexity - minAimComplexity + 1);
  // Генерируем необходимое кол-во целей первой длины
  vector<TAim>& currentComplexityAims = aimsViaComplexity.at(0);
  while (currentComplexityAims.size() < desiredAimsDistr.at(0)){
    TAim aim = generateSelfConsistentAim(_environmentResolution, minAimComplexity);
    bool aimUnique = true;
    for (vector<TAim>::const_iterator compareAim = currentComplexityAims.begin(); compareAim != currentComplexityAims.end(); ++compareAim)
      if (equalAims(*compareAim, aim)){
        aimUnique = false;
        break;
      }
    if (aimUnique)
      currentComplexityAims.push_back(aim);
  }
  unsigned int currentComplexity = minAimComplexity + 1;
  // Теперь необходимо сгенерировать продолжения целей (идем по уровням)
  while (currentComplexity <= maxAimComplexity){
    vector<TAim>& currentComplexityAims = aimsViaComplexity.at(currentComplexity - minAimComplexity);
    const vector<TAim>& parentComplexityAims = aimsViaComplexity.at(currentComplexity - 1 - minAimComplexity);
    generateComplexityLevelAims(currentComplexityAims, parentComplexityAims, _environmentResolution, 
                                currentComplexity, desiredAimsDistr.at(currentComplexity - minAimComplexity));
    ++currentComplexity;
  }
  // Необходимо создать среду и записать все цели
  RestrictedHypercubeEnv* environment = new RestrictedHypercubeEnv;
  environment->environmentResolution = _environmentResolution;
  for (vector< vector<TAim> >::const_iterator level = aimsViaComplexity.begin(); level != aimsViaComplexity.end(); ++level)
    for (vector<TAim>::const_iterator aim = level->begin(); aim != level->end(); ++aim)
      environment->addAim(*aim);
  return environment;
}

/// Метод генерации или продолжения непротиворечивой цели
/**
* В случае, если методу не передается стартовая цель, то он генерирует обычную цель. Если же ему передается некоторая стартовая цель, то он генерирует цель, 
* которая является расширением стартовой (с дополнительными действиями в начале).
* \param [in] environmentResolution - размерность среды
* \param [in] desiredComplexity - необходимая сложность цели (в случае продолжения - сложность полной цели)
* \param [in] currentAim - опциональный параметр стартовой цели
* \return Непротиворечивая новая цель или продолжение стартовой цели
*/
RestrictedHypercubeEnv::TAim RestrictedHypercubeEnv::generateSelfConsistentAim(unsigned int environmentResolution, unsigned int desiredComplexity,
                                                                               const TAim& currentAim/*=TAim()*/){
  TAim desiredAim;
  desiredAim.aimComplexity = desiredComplexity;
  // Копируем текущую цель в конец новой (если есть какая-то цель)
  if (currentAim.aimComplexity)
    copy(currentAim.actionsSequence, currentAim.actionsSequence + currentAim.aimComplexity, desiredAim.actionsSequence + desiredAim.aimComplexity - currentAim.aimComplexity);
  // Генерируем действия цели справа налево
  for (int currentAction = desiredAim.aimComplexity - currentAim.aimComplexity; currentAction > 0; --currentAction){
    int currentBit = 0; // Новый бит
		bool currentDirection = false; // Направление изменения
		bool stop = false; // Признак того, что сгенерированный бит противоречит предыдущим
		do {
		  stop = false;
			currentBit = service::uniformDiscreteDistribution(1, environmentResolution);
			currentDirection = (service::uniformDiscreteDistribution(0, 1) == 1);
			// Находим первое действие в цели, которое использует тот же бит, что и сгенерированное
			int currentCompareAction = 0;
      for (currentCompareAction = currentAction + 1; currentCompareAction <= desiredAim.aimComplexity; ++currentCompareAction){
        if (desiredAim.actionsSequence[currentCompareAction - 1].bitNumber == currentBit){
          if (desiredAim.actionsSequence[currentCompareAction - 1].desiredValue != currentDirection)
					  stop = true;
					break; // Не идем дальше по цели
				}
			}
			// Если мы дошли до конца и не встретили нужного бита
			if (currentCompareAction > desiredAim.aimComplexity) stop = true;
		} while (!stop); // Конец генерирования действий цели
		desiredAim.actionsSequence[currentAction - 1].bitNumber = currentBit;
		desiredAim.actionsSequence[currentAction - 1].desiredValue = currentDirection;
	}
  //desiredAim.reward = 20 + 10*(desiredAim.aimComplexity - 2);
  desiredAim.reward = 20;
  return desiredAim;
}

/// Проверка существования идентичной цели в среде
/**
* \param [in] aim - цель, существование которой нужно проверить.
* \return True - если такая цель есть в среде; False - в противном случае.
*/
bool RestrictedHypercubeEnv::checkAimExistence(const TAim& aim){
  for (vector<TAim>::const_iterator curAim = aimsSet.begin(); curAim != aimsSet.end(); ++curAim)
    if (equalAims(*curAim, aim))
      return true;
  return false;
}

/// Проверка идентичности двух целей
/**
* \param [in] firstAim - первая цель для сравнения.
* \param [in] secondAim - вторая цель для сравнения.
* \return True - если цели равны; False - в противном случае.
*/
bool RestrictedHypercubeEnv::equalAims(const TAim& firstAim, const TAim& secondAim){
  if (firstAim.aimComplexity != secondAim.aimComplexity)
    return false;
  else
    for (int action = 0; action < firstAim.aimComplexity; ++action)
      if ((firstAim.actionsSequence[action].bitNumber != secondAim.actionsSequence[action].bitNumber) ||
        (firstAim.actionsSequence[action].desiredValue != secondAim.actionsSequence[action].desiredValue))
        return false;
  return true;
}

/// Проверка является ли одна цель родительской для другой цели
/**
* \param [in] potentialParentAim - потенциальная родительская цель (более общая).
* \param [in] subAim - потенциальное расширение родительской цели.
* \return True - если первая переданная цель является родительской для второй, False - в противном случае.
*/
bool RestrictedHypercubeEnv::isParentAim(const TAim& potentialParentAim, const TAim& subAim){
  // Длина родительской цели не может быть больше или равна длине подцели
  if (potentialParentAim.aimComplexity >= subAim.aimComplexity)
    return false;
  const unsigned int parAimCompl = potentialParentAim.aimComplexity;
  const unsigned int subAimCompl = subAim.aimComplexity;
  for (unsigned int lastAction = 1; lastAction <= potentialParentAim.aimComplexity; ++lastAction)
    if ((potentialParentAim.actionsSequence[parAimCompl - lastAction].bitNumber != subAim.actionsSequence[subAimCompl - lastAction].bitNumber) ||
        (potentialParentAim.actionsSequence[parAimCompl - lastAction].desiredValue != subAim.actionsSequence[subAimCompl - lastAction].desiredValue))
      return false;
  return true;
}

/// Метод перезагрузки среды (обнуление жизни агента и времен восстановления награды)
void RestrictedHypercubeEnv::resetEnvironment(){
  agentLifeRecord.clear();
  currentAgentReward = 0;
  fill(reachedAimsTimes.begin(), reachedAimsTimes.end(), -rewardRecoveryTime);
}