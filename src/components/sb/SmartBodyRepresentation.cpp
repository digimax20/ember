#include "SmartBodyRepresentation.h"
#include "SmartBodyBehaviors.h"

#include "sb/SBScene.h"
#include "sb/SBCharacter.h"
#include "sb/SBSkeleton.h"
#include "sr/sr_quat.h"

#include <OgreSkeletonInstance.h>
#include <OgreEntity.h>
#include <OgreBone.h>

namespace Ember
{

//Constructor that create the SmartBody character.
SmartBodyRepresentation::SmartBodyRepresentation(SmartBody::SBScene& scene, const Ogre::Entity& entity, const std::string& group,
	const std::string& skName, const std::vector<SmartBodyBehaviors*>& behaviors)
:	mScene(scene),
	mEntity(entity), mOgreSkeleton(*entity.getSkeleton()), 
	mCharacter(*scene.createCharacter(entity.getName(), group)), mSkeleton(*scene.createSkeleton(skName)),
	mManualMode(false)
{
	//Associate the skeleton to the character.
	mCharacter.setSkeleton(&mSkeleton);

	//Add controllers.
	mCharacter.createStandardControllers();

	//Add retarget constraints.
	for (int i = 0, n = behaviors.size(); i < n; i ++)
	{
		behaviors[i]->addConstraints(mCharacter);
	} 
}

SmartBodyRepresentation::~SmartBodyRepresentation(void)
{
	//Free the memory.
	mScene.removeCharacter(mCharacter.getName());
}

void SmartBodyRepresentation::setManualControl(bool mode /*= true */)
{
	Ogre::Skeleton::BoneIterator it = mOgreSkeleton.getBoneIterator(); 

	while (it.hasMoreElements()) 
	{ 
		it.getNext()->setManuallyControlled(mode);
	}

	mManualMode = mode;
}

void SmartBodyRepresentation::updateBonePositions(void)
{
	//Check that mManuallyControl is set to true.
	if (!mManualMode)
		setManualControl(true);
	
	//Each bone must be set at the same position than its reference in SmartBody.
	for (int j = 0, n = mSkeleton.getNumJoints(); j < n; j++)
	{
		//We get the joint by its index. 
		SmartBody::SBJoint *joint = mSkeleton.getJoint(j);

		//If it exists, we get the corresponding bone by the name of the joint (which is the one in Ogre, due to the jointMap).
		if (mOgreSkeleton.hasBone(joint->getName()))
		{	
			Ogre::Bone *bone = mOgreSkeleton.getBone(joint->getName());

			//We get the position of the joint.
			SrQuat orientation = joint->quat()->value();

			//The position vector are relative to the original position.
			Ogre::Vector3 position(joint->getPosition().x, joint->getPosition().y, joint->getPosition().z);
			Ogre::Quaternion quaternion(orientation.w, orientation.x, orientation.y, orientation.z);

			//We update the bone positions in Ogre.
			bone->setPosition(bone->getInitialPosition() + position);
			bone->setOrientation(quaternion);
		}
	}
}

bool SmartBodyRepresentation::isManuallyControlled(void)
{
	return mManualMode;
}

}