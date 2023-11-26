#include "..\Public\RandomManager.h"

IMPLEMENT_SINGLETON(CRandomManager)

CRandomManager::CRandomManager()
{
}

const _float& CRandomManager::Random_Float(_float fMin, _float fMax) 
{
	std::random_device Random;
	std::mt19937_64 gen(Random());

	std::uniform_real_distribution<float> xfloatProbality(fMin, fMax);

	m_fRandom_float = xfloatProbality(gen);

	return m_fRandom_float;
}

const _int& CRandomManager::Random_Int(_int iMin, _int iMax)
{
	std::random_device Random;
	std::mt19937_64 gen(Random());

	std::uniform_int_distribution<int> xintProbality(iMin, iMax);

	m_iRandom_int = xintProbality(gen);

	return m_iRandom_int;
}

const _bool& CRandomManager::Random_Coin(_float fProbality)
{
	if (1.f <= fProbality)
		fProbality = 1.f;
	else if (0.f >= fProbality)
		fProbality = 0.f;

	std::random_device Random;
	std::mt19937_64 gen(Random());

	std::bernoulli_distribution coinFlip(fProbality);

	m_bRandom_Bool = coinFlip(gen);

	return m_bRandom_Bool;
}

void CRandomManager::Free()
{
	__super::Free();
}
