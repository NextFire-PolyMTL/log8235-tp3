// Fill out your copyright notice in the Description page of Project Settings.

#include "TargetLKPInfo.h"

TargetLKPInfo::TargetLKPInfo()
    : m_currentLKPState(ELKPState::LKPState_Invalid),
      m_pos(FVector::ZeroVector),
      m_isPoweredUp(false),
      m_lastUpdatedTimeStamp(-1.f)
{
}

TargetLKPInfo::~TargetLKPInfo()
{
}
