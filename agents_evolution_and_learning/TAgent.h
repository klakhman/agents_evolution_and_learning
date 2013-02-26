#ifndef TAGENT_H
#define TAGENT_H

#include "TNeuralNetwork.h"
#include "TPoolNetwork.h"
#include "TEnvironment.h"

#include <iostream>
#include <vector>

/*
Класс нейросетевого агента.
ВСЕ АГЕНТЫ ДОЛЖНЫ НАСЛЕДОВАТЬ ОТ ДАННОГО КЛАССА. В СЛУЧАЕ НЕОБХОДИМОСТИ НУЖНО ПЕРЕОПРЕДЕЛЯТЬ СЛЕДУЮЩИЕ ВИРТУАЛЬНЫЕ ФУНКЦИИ:

virtual int getActionResolution() const;
virtual int calculateOutputResolution(int inputResolution) const;
virtual void generateMinimalAgent(int inputResolution);
virtual std::vector<double> decodeAction(double outputVector[]);
virtual void life(TEnvironment& environment, int agentLifeTime, bool rewardCalculate = true);
И КОНЕЧНО ДЕСТРУКТОР

*/
class tests;

class TAgent{
	// Геном агента из которого транслируется контроллер
	TPoolNetwork* genome;
	// Контроллер агент
	TNeuralNetwork* neuralController;
	// Награда, набранная агентом
	double reward;
	// Родители агента (первый более приспособленный, второй - менее)
	int parents[2];
	// Записанная жизнь агента
	std::vector< std::vector<double> > agentLife;
	// Декодирование идентификатора совершаемого агентом действия (!!!! по идее декодирование действия, как и кодирование состояния среды во входной вектор, так и определение необходимой размерности выходного вектора должны совершаться некоторым сторонним от агента образом, так как это средоспецифические функции, но при этом не принадлежащие среде !!!!!)
	virtual std::vector<double> decodeAction(double outputVector[]);
	// Подсчет размера выходного вектора нейроконтроллера и генома агента на основе размера входного вектора среды
	virtual int calculateOutputResolution(int inputResolution) const;
	// Запрещаем копирующий конструктор (чтобы экземпляры нельзя было передавать по значению)
	TAgent(const TAgent&);

	// Построение первичного контроллера агента
	void buildPrimaryNeuralController();
	// Функция отбора активирующихся нейронов
	void neuronsSelection(double neuronsSummaryPotential[]);
	// Функция отбора наиболее активных синапсов
	void synapsesSelection(double synapsesSummaryPotential[]);
	// Функция отбора предикторных связей
	void predConnectionsSelection(double predictorSignificance[]);

	// Детекция рассогласования на нейроне
	// 0 - отсутствие рассогласования, 1 - рассогласование типа "предсказана активация - ее нет", 2 - рассогласование типа "предсказано молчание - есть активация"
	int mismatchDetection(int neuronNumber);
	// Процедура нахождения наиболее активного "спящего" нейрона в пуле
	int findMostActiveSilentNeuron(int poolNumber);
	// Процедура модификации синаптических связей при обучении рассогласованного нейрона
	void modifySynapsesStructure(int mismatchedNeuron, int activatedNeuron, int mismatchType);
	// Процедура модификации структуры предикторных связей (переносим связи, которые предсказали активацию с рассогласованного нейрона на включающийся)
	void modifyPredConnectionsStructure(int mismatchedNeuron, int activatedNeuron);
	// Процедура самообучения рассогласованного нейрона
	void selfLearnNeuron(int neuronNumber, int mismatchType);

public:
	// Структура настройки первичного системогенеза
	struct SPrimarySystemogenesisSettings{
		int primarySystemogenesisMode; // Режим первичного системогенеза (false - нет системогенеза (линейный системогенез), true - есть полнеценный первичный системогенез)
		int initialPoolCapacity; // Изначальная размерность каждого пула
		double initialDevelopSynapseProbability; // Вероятность образования синапса связи в процессе построения первичной сети
		double initialDevelopPredConnectionProbability; // Вероятность образования предикторной связи в процессе построения первичной сети
		int primarySystemogenesisTime; // Время (в тактах), в течение которого происходит первичный системогенез
		double spontaneousActivityProb; // Вероятность спонтанной активации нейрона
		double activeNeuronsPercent; // Процент наиболее активных нейронов, которые становятся активными после ПС
		double synapsesActivityTreshold; //! Порог общего сигнала, прошедшего по синапсу в ПС, по которому происходит отбор связей между активными нейронами (или процент синапсов которые надо отобрать)
		double significanceTreshold; // Порог значимости предсказания, осуществляемого по пред. связи в ПС, по которому происходит отбор связей между активными нейронами
	} primarySystemogenesisSettings;
	
	// Структура настроек процесса обучения
	struct SLearningSettings{
		int learningMode; // Режим обучения (false - нет обучения, true - есть обучение)
		double mismatchSignificanceTreshold; // Порог значимости предсказания (доли предсказания), при котором нейрон считается рассогласованным
	} learningSettings;

	// Конструктор по умолчанию
	TAgent(){
		parents[0] = 0;
		parents[1] = 0;
		reward = 0;
		neuralController = new TNeuralNetwork;
		genome = new TPoolNetwork;
		primarySystemogenesisSettings.primarySystemogenesisMode = 0;
		primarySystemogenesisSettings.initialPoolCapacity = 1;
		primarySystemogenesisSettings.initialDevelopSynapseProbability = 1;
		primarySystemogenesisSettings.initialDevelopPredConnectionProbability = 1;
		primarySystemogenesisSettings.primarySystemogenesisTime = 0;
		primarySystemogenesisSettings.spontaneousActivityProb = 0;
		primarySystemogenesisSettings.activeNeuronsPercent = 0;
		primarySystemogenesisSettings.synapsesActivityTreshold = 0;
		primarySystemogenesisSettings.significanceTreshold = 0;

		learningSettings.learningMode = 0;
		learningSettings.mismatchSignificanceTreshold = 0.5;
	}

	// Деструктор
	virtual ~TAgent(){
		if (neuralController) delete neuralController;
		if (genome) delete genome;
	}
	// Геттеры и сеттеры
	double getReward() const { return reward; }
	int getMoreFitParent() const { return parents[0]; }
	int getLessFitParent() const { return parents[1]; }
	void setMoreFitParent(int moreFitParent) { parents[0] = moreFitParent; }
	void setLessFitParent(int lessFitParent) { parents[1] = lessFitParent; }
	TPoolNetwork* getPointerToAgentGenome() const { return genome; }
	TNeuralNetwork* getPointerToAgentController() const { return neuralController; }
	// Так как настройки режимов системогенеза и обучения очень важны, то делаем для них отдельные геттеры и сеттеры для удобства
	int getSystemogenesisMode() const { return primarySystemogenesisSettings.primarySystemogenesisMode; }
	void setSystemogenesisMode(int mode) { primarySystemogenesisSettings.primarySystemogenesisMode = mode; }
	int getLearningMode() const { return learningSettings.learningMode; }
	void setLearningMode(int mode) { learningSettings.learningMode = mode; }

	// Размерность составного действия агента (кол-во идентификаторов действия в один такт времени)
	virtual int getActionResolution() const { return 1; }
	// Получение указателя на запись жизни агента
	const std::vector< std::vector<double> >& getPointerToAgentLife() const { return agentLife; }

	// Загрузка нейроконтроллера агента
	void loadController(std::istream& is);
	// Загрузка генома нейрононтроллера
	void loadGenome(std::istream& is, bool extra = false);
  // Загрузка генома из файлов старого формата
  void loadOldFormatGenome(std::istream& is, int inputResolution, int outputResolution);
	// Выгрузка нейроконтроллера агета в файл или на экран
	void uploadController(std::ostream& os) const;
	// Выгрузка генома агента в файл или на экран (extra = true - печать полных сведений о пуле (вместе с номерами родительских пулов и временами появления в эволюции))
	void uploadGenome(std::ostream& os, bool extra = false) const;
	// Генерация случайного минимального возможного генома агента
	virtual void generateMinimalAgent(int inputResolution);
	// Моделирование жизни агента (rewardCalculate - опциональный признак автоматического подсчета награды, которую агент достиг в течение жизни (можно выключать для оптимизации для больших сред))
	virtual void life(TEnvironment& environment, int agentLifeTime, bool rewardCalculate = true);
	// Оператор присваивания (фактически полное копирование агента, включая геном, но не включая контроллер - создание новых структур)
	TAgent& operator=(const TAgent& sourceAgent);

	// Временная процедура печати жизни агента
	//void printLife(TEnvironment& environment, int agentLifeTime);

	// Линейная процедра первичного системогеназа (когда происходит однозначная трансляция генотипа) - используется, когда нет ни настоящего системогенеза, ни обучения
	void linearSystemogenesis();
	// Основной метод перчиного системогенеза 
	void primarySystemogenesis();

  // Альтернативный системогенез (активным становится только один нейрон в пуле с детерминированной структурой связей)
	void alternativeSystemogenesis();

	// Основной метод обучения нейроконтроллера на одном такте времени
	void learning();

  // Процедура случайного обучения агента - случайное включение нейронов с некоторой вероятностью (для контроля качества разработанного алгоритма обучения)
  void randomLearning();

  friend class tests;
};

#endif // TAGENT_H
