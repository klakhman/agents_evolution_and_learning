//
//  TBehaviorAnalysis.h
//  agents_evolution_and_learning
//
//  Created by Nikita Pestrov on 08.11.12.
//  Copyright (c) 2012 Neurointelligence and Neuromorphe lab. All rights reserved.
//

#ifndef TBehaviorAnalysis_H
#define TBehaviorAnalysis_H

#include <iostream>
/*
 РљР»Р°СЃСЃ Р°РЅР°Р»РёР·Р° РїРѕРІРµРґРµРЅС‡РµСЃРєРёС… С†РёРєР»РѕРІ Р°РіРµРЅС‚Р°
*/
class TBehaviorAnalysis{
  //РњР°РєСЃРёРјР°Р»СЊРЅРѕ РІРѕР·РјРѕР¶РЅР°СЏ РґР»РёРЅР° С†РёРєР»Р°(С‡СѓС‚СЊ РјРµРЅСЊС€Рµ РїРѕР»РѕРІРёРЅС‹ РґР»РёРЅС‹ Р¶РёР·РЅРё Р°РіРµРЅС‚Р°)
  static const int MaxCycleLength = 120;
  //РњРёРЅРёРјР°Р»СЊРЅР°СЏ РґР»РёРЅР° С†РёРєР»Р°
  static const int MinCycleLength = 2;
  //Р§РёСЃР»Рѕ РїРѕРІС‚РѕСЂРѕРІ, РЅРµРѕР±РѕС…РѕРґРёРјРѕРµ РґР»СЏ РІРµСЂРёС„РёРєР°С†РёРё РЅР°С…РѕР¶РґРµРЅРёСЏ С†РёРєР»Р°
  static const int SufficientRepeatNumber = 4;
  //РњР°РєСЃРёРјР°Р»СЊРЅРѕ РІРѕР·РјРѕР¶РЅРѕРµ С‡РёСЃР»Рѕ С†РёРєР»РѕРІ РІРїСЂРёРЅС†РёРїРµ
  static const int MaxCyclesQuantity = 3500;
  
  //РЎС‚СЂСѓРєС‚СѓСЂР° С†РёРєР»Р°
  struct TCycle{
    //РџРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕСЃС‚СЊ РґРµР№СЃС‚РІРёР№ РІ С†РёРєР»Рµ
    int CycleSequence[MaxCycleLength];
    //Р”Р»РёРЅР° С†РёРєР»Р°
    int CycleLength;
    //РќР°РіСЂР°РґР°, РїРѕР»СѓС‡Р°РµРјР°СЏ Р·Р° РїСЂРѕС…РѕР¶РґРµРЅРёРµ С†РёРєР»Р°
    double CycleReward;
  };
  
};

#endif /* defined(__agents_evolution_and_learning__TBehaviorAnalysis__) */
