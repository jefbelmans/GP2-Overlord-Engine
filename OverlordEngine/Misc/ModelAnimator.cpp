#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		//auto passedTicks = ...
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;

		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0)
				m_TickCount += m_CurrentClip.duration;
		}
		else
		{
			m_TickCount += passedTicks;
			if(m_TickCount > m_CurrentClip.duration)
				m_TickCount -= m_CurrentClip.duration;
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		for (auto key : m_CurrentClip.keys)
		{
			if (key.tick < m_TickCount)
				keyA = key;
			else if (key.tick > m_TickCount)
			{
				keyB = key;
				break;
			}
		}

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		auto blendFactor = (m_TickCount - keyA.tick) / (keyB.tick - keyA.tick);

		//Clear the m_Transforms vector
		m_Transforms.clear();
		//FOR every boneTransform in a key (So for every bone)
		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
		//	Decompose both transforms
		//	Lerp between all the transformations (Position, Scale, Rotation)
		//	Compose a transformation matrix with the lerp-results
		//	Add the resulting matrix to the m_Transforms vector

		for (UINT i = 0; i < keyA.boneTransforms.size(); ++i)
		{
			auto transformA = XMLoadFloat4x4(&keyA.boneTransforms[i]);
			auto transformB = XMLoadFloat4x4(&keyB.boneTransforms[i]);
			XMVECTOR posA, posB, scaleA, scaleB;
			XMVECTOR rotA, rotB;
			XMMatrixDecompose(&scaleA, &rotA, &posA, transformA);
			XMMatrixDecompose(&scaleB, &rotB, &posB, transformB);
			XMVECTOR pos = XMVectorLerp(posA, posB, blendFactor);
			XMVECTOR scale = XMVectorLerp(scaleA, scaleB, blendFactor);
			XMVECTOR rot = XMQuaternionSlerp(rotA, rotB, blendFactor);
			XMMATRIX transformMat = XMMatrixAffineTransformation(scale, XMVectorZero(), rot, pos);
			XMFLOAT4X4 transform;
			XMStoreFloat4x4(&transform, transformMat);
			m_Transforms.push_back(transform);
		}
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	m_ClipSet = false;
	auto clips = m_pMeshFilter->GetAnimationClips();
	for (UINT i = 0; i < clips.size(); ++i)
	{
		if (clips[i].name == clipName)
		{
			SetAnimation(clips[i]);
			return;
		}
	}

	Reset();
	Logger::LogWarning(L"ModelAnimator::SetAnimation(const std::wstring& clipName) > AnimationClip with name: " + clipName + L" not found!");
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	m_ClipSet = false;
	if (clipNumber >= m_pMeshFilter->GetAnimationClips().size())
	{
		Reset();
		Logger::LogWarning(L"ModelAnimator::SetAnimation(UINT clipNumber) > AnimationClip with number: " + std::to_wstring(clipNumber) + L" not found!");
		return;
	}

	SetAnimation(m_pMeshFilter->GetAnimationClips()[clipNumber]);
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	m_ClipSet = true;
	m_CurrentClip = clip;
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//If pause is true, set m_IsPlaying to false
	if (pause) m_IsPlaying = false;

	//Set m_TickCount to zero
	//Set m_AnimationSpeed to 1.0f
	m_TickCount = 0;
	m_AnimationSpeed = 1.0f;

	//If m_ClipSet is true
	//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
	//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	//Else
	//	Create an IdentityMatrix 
	//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)

	if (m_ClipSet)
	{
		m_Transforms.assign(m_CurrentClip.keys[0].boneTransforms.begin(), m_CurrentClip.keys[0].boneTransforms.end());
	}
	else
	{
		XMFLOAT4X4 identity{};
		identity._11 = 1.0f;
		identity._22 = 1.0f;
		identity._33 = 1.0f;
		identity._44 = 1.0f;

		m_Transforms.assign(m_pMeshFilter->m_BoneCount, identity);
	}
}
