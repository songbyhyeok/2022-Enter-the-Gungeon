#include "SceneManager.h"
#include "TimeHandler.h"
#include "KeyController.h"

unique_ptr<Scene> SceneManager::RunningScene = nullptr;
unique_ptr<Scene> SceneManager::ReadyScene = nullptr;
map<SCENE_NAME, unique_ptr<Scene>> SceneManager::BlockedScenes;

DWORD CALLBACK loadingThread(LPVOID pvParam)
{
	// ������ ���� ���� (���)
	/*
		1. ȣ��� �Լ��� ��ȯ�� �� (�츮�� ����� ���)
		2. ȣ��� �Լ� ���ο��� exitthread()�� ȣ������ ��
		3. ������ ���μ����� �ٸ� ���μ������� terminatethread()�� ȣ������ ��
		4. ���� �����尡 ���Ե� ���μ����� ����� ��
	*/

	switch (TimeHandler::getInstance().startAlarm((*dynamic_cast<LoadingScene*>(SceneManager::RunningScene.get())).getMinLoadingTime()))
	{
	case true:
		SceneManager::changeSceneByReadyScene();
		break;
	case false:
		assert(-1);
		break;
	}
	
	return 0;
}

void SceneManager::update()
{
	RunningScene->update(TimeHandler::getInstance().getDeltaTime());
}

void SceneManager::render(const HDC& hdc)
{
	RunningScene->render(hdc);
}

void SceneManager::release(unique_ptr<Scene>& scene, bool isStopPlayingSound, bool isDeletingPtrData, bool isDeletingPtr)
{
	if (scene)
	{
		if (isStopPlayingSound)
			scene->stopCurrentSound();
		if (isDeletingPtrData)
			scene->release();		
		if (isDeletingPtr)
			scene.reset();
	}
}

void SceneManager::allRelease()
{
	release(RunningScene, true, true, true);
	release(ReadyScene, true, true, true);
	allDeleteBlockedScenes();
}

void SceneManager::requestOptionInit()
{
	ReadyScene = move(createSceneBy(SCENE_NAME::OPTION));
	ReadyScene->init();
	addScene(ReadyScene->getSceneID(), ReadyScene); // �ɼ� ���� ����� ������ ����.
}

void SceneManager::toTitleScene(const SCENE_NAME& newSceneName)
{	
	(*ReadyScene).release();
	changeScene(newSceneName);
}

void SceneManager::changeSceneByReadyScene(const SCENE_NAME& newSceneName)
{
	if (checkScene(ReadyScene))
	{
		if (compareSceneNameBy(ReadyScene->getSceneID(), SCENE_NAME::OPTION))
		{
			swapReadySceneAndOptionScene();
			return;
		}

		// ����, ���� ������ �ɼ� ������ �� Ÿ��Ʋ ������ �̵��� ��� �Ѿ�� �ڵ�.
		if (ReadyScene.get()->getSceneID() == SCENE_NAME::TITLE && RunningScene.get()->getSceneID() == SCENE_NAME::OPTION)
		{
			ReadyScene.get()->reset();
		}

		moveSceneBy(ReadyScene, RunningScene); 
		if (compareSceneNameBy(RunningScene->getSceneID(), SCENE_NAME::LOADING)) //�̹� ü���� ���� �� �� �̻� ȣ���� ���¿��� ü������(�⺻, �ε�)�� �ٽ� �ݵ� ��Ȳ
		{
			ReadyScene = move(findScene(newSceneName));
			switch (ReadyScene != nullptr)
			{
			case true:
				deleteScene(newSceneName);
				break;
			case false:
				ReadyScene = move(createSceneBy(newSceneName));
				break;
			}
		}

		RunningScene->startScene();
		return;
	}
}

/*
* ü���� ���� �ٲ� ���� �� �غ� ������ �ε��� �� ���ڸ� �޾� �˰����� ���� �� ������ �˴ϴ�.
* �˰����� ũ�� 4���� ������ ����� ���� �����Ǿ� �ֽ��ϴ�.
*
* �� �˰����� �ý��� ���α׷��� �����ٸ� ���� ������ �״�� �����ؼ� ��������ϴ�.
*
* ��� ���� RunningScene, ReadyScene, BlockedScene
* RunningScene -> ���� ���� ���� ���μ���
* ReadyScene -> ���� ���μ����� �̸� ����
* BlockedScene -> ������� ���� ���� �����ϰ� �ٽ� �ʿ� �� ������. �ӵ� ���� ����
*
* �˰��� ����
* ü������ �� ��, �ε� ��
1. ���׾� null
���� create(�ε�)
����� create (����)
���׾� init -> start
������ ����
���� �� init
���� ���� <> ����
���ҵ� ���� ������ �� -> ����� ��
���� -> empty

2 ���׾��� != nullptr (�̹� �� �� ����� ����)
���� �� = findScene (�ε�)
���� �� init
���� ���� <> ����
���ҵ� ���� ������ �� -> ����� ��(���� ��)
if (����(�� �ε�) == �ε�) �̶�� true �ش�
���� = find �� ��
���׾� ��ŸƮ(���ε�)
������ ����
���� �� init(����)
���� ����<>����
���ҵ� ���� ������ �� ����� ��(�� �ε�)
���׾�(����) ��ŸƮ

ü������ �� ��
1. ���׾� null
���׾� = create ����
���� �� init
���� �� ��ŸƮ

2 ���� �� = findScene(����)
���� �� init
���� ����, ����
���� �� -> ������
���׾� ��ŸƮ ����
*/

// �ٷ� ���ο� ���� �θ��ڴ�.
void SceneManager::changeScene(const SCENE_NAME& newSceneName) 
{
	switch (RunningScene == nullptr)
	{
	case true:
		RunningScene = move(createSceneBy(newSceneName));
		if (checkScene(RunningScene))
		{
			RunningScene->startScene();
		}
		break;
	case false:
		ReadyScene = move(findScene(newSceneName));
		switch (ReadyScene != nullptr)
		{
		case true:
			deleteScene(newSceneName);
			break;
		case false:
			ReadyScene = move(createSceneBy(newSceneName));
			break;
		}

		changeSceneByReadyScene(newSceneName);
		break;
	}
}

void SceneManager::changeScene(const SCENE_NAME& newSceneName, const SCENE_NAME& newLoadingSceneName)
{
	switch (RunningScene == nullptr)
	{
	case true:
		RunningScene = move(createSceneBy(newLoadingSceneName));
		ReadyScene = move(createSceneBy(newSceneName));
		if (checkScene(RunningScene))
		{
			RunningScene->startScene();
		}
		break;
	case false:
		ReadyScene = move(findScene(newLoadingSceneName));
		switch (ReadyScene != nullptr)
		{
		case true:
			deleteScene(newLoadingSceneName);
			break;
		case false:
			ReadyScene = move(createSceneBy(newLoadingSceneName));
			break;
		}

		changeSceneByReadyScene(newSceneName);
		break;
	}

	HANDLE		hThread;
	DWORD		loadThreadID;
	CloseHandle(CreateThread(NULL, 0, loadingThread, NULL, 0, &loadThreadID));
}

// 1.���׾��� �ε����� �ƴϰ� ������� �ɼǾ��̶�� �ʱ�ȭ �� ��������� ������� �ʴ´�.
// 2.���׾��� �ɼǾ��̰� ������� Scene�̶�� init�� ���� �ʴ´�.
// 3.���׾��� ������� ������ �׳� �����Ѵ�.
void SceneManager::swapReadySceneAndOptionScene()
{
	switch (compareSceneNameBy(ReadyScene->getSceneID(), SCENE_NAME::OPTION))
	{
	case true:
	{
		auto& optionScene = *dynamic_cast<OptionScene*>(ReadyScene.get());
		// ���� ���� ����, ���� ���� ��� �ɼǿ� Ÿ��Ʋ ��ư ����
		switch (RunningScene->getSceneID())
		{
		case SCENE_NAME::GAME:
			optionScene.requestToMakeTitleButton();
			optionScene.notifyToScenePartner("Game");
			break;
		case SCENE_NAME::MAPTOOL:
			optionScene.requestToMakeTitleButton();
			optionScene.notifyToScenePartner("MapTool");
			break;
		case SCENE_NAME::TITLE:
			optionScene.requestToDeleteTitleButton();
			break;
		}

		controlRunningSceneMusic(false);
		swapSceneBy(RunningScene, ReadyScene);
	}
		break;
	case false:
		switch (compareSceneNameBy(RunningScene->getSceneID(), SCENE_NAME::OPTION))
		{
		case true:
		{
			// �ɼ� ������ ������ ���� �ִٸ� �� ������ �Ѵ�.
			auto& optionScene = *dynamic_cast<OptionScene*>(RunningScene.get());
			if (optionScene.isDataRecorded())
			{
				optionScene.setDataChangeRecode(false);
				ReadyScene->reset();
			}

			/*unique_ptr<Scene> loadingScene = move(findScene(SCENE_NAME::LOADING));
			if (loadingScene != nullptr)
			{
				deleteScene(SCENE_NAME::LOADING);
				loadingScene->reset();
				addScene(SCENE_NAME::LOADING, loadingScene);
			}*/
		}
		
			swapSceneBy(RunningScene, ReadyScene);
			controlRunningSceneMusic(true);

			addScene(ReadyScene->getSceneID(), ReadyScene); // �ɼǾ��� ���������� ����.
			break;
		case false:
			assert(false);
			break;
		}
		break;
	}
}

bool SceneManager::checkScene(const unique_ptr<Scene>& scene)
{
	if (scene->getSceneID() != SCENE_NAME::OPTION && scene->getSceneID() != SCENE_NAME::LOADING)
	{		
		int16 gammaCount = 0;

		OptionScene* optScenePtr = nullptr;
		unique_ptr<Scene> optionScenePtr = findScene(SCENE_NAME::OPTION);
		switch (optionScenePtr == nullptr)
		{
		case true:
			if ((*RunningScene).getSceneID() != SCENE_NAME::OPTION)
				assert(false);
	
			optScenePtr = dynamic_cast<OptionScene*>(&(*RunningScene));
			gammaCount = optScenePtr->getGammaCount();
			optScenePtr->setScenePtrFromOption(&(*scene));
			break;
		case false:
			optScenePtr = dynamic_cast<OptionScene*>(&(*optionScenePtr));
			gammaCount = optScenePtr->getGammaCount();
			optScenePtr->setScenePtrFromOption(&(*scene));
			deleteScene(SCENE_NAME::OPTION);
			addScene(SCENE_NAME::OPTION, optionScenePtr);
			break;
		}

		switch (scene->getSceneID())
		{
		case SCENE_NAME::TITLE:
			switch (scene->isModifiable())
			{
			case true:
				dynamic_cast<TitleScene*>(&(*scene))->setGamma(gammaCount);
				break;
			case false:
				dynamic_cast<TitleScene*>(&(*scene))->setGammaCount(gammaCount);
				break;
			}
			break;
		case SCENE_NAME::GAME:
			switch (scene->isModifiable())
			{
			case true:
				dynamic_cast<GameScene*>(&(*scene))->setGamma(gammaCount);
				break;
			case false:
				dynamic_cast<GameScene*>(&(*scene))->setGammaCount(gammaCount);
				break;
			}
			break;
		case SCENE_NAME::MAPTOOL:
			switch (scene->isModifiable())
			{
			case true:
				dynamic_cast<MapToolScene*>(&(*scene))->setGamma(gammaCount);
				break;
			case false:
				dynamic_cast<MapToolScene*>(&(*scene))->setGammaCount(gammaCount);
				break;
			}
			break;
		}	
	}

	if (compareSceneNameBy(scene->getSceneID(), SCENE_NAME::TITLE))
	{
		TitleScene& titleScene = *dynamic_cast<TitleScene*>(scene.get());
		// �ε����� ���Ե� �Լ��� ȣ��� ���
		if (compareSceneNameBy(RunningScene->getSceneID(), SCENE_NAME::LOADING) && compareSceneNameBy(ReadyScene->getSceneID(), SCENE_NAME::TITLE))
		{
			titleScene.setPlaySpeedOfOpeningVideo(titleScene.getToLoadingSpeed());
		}
		// �ε����� ���Ե��� ���� �Լ�
		else if (compareSceneNameBy(RunningScene->getSceneID(), SCENE_NAME::TITLE) || compareSceneNameBy(ReadyScene->getSceneID(), SCENE_NAME::TITLE))
		{	
			titleScene.setPlaySpeedOfOpeningVideo(titleScene.getToBaseSpeed());
		}		
	}

	if (scene->init() == false)
		assert(false);

	return true;
}

void SceneManager::swapSceneBy(unique_ptr<Scene>& sceneA, unique_ptr<Scene>& sceneB)
{
	unique_ptr<Scene> tempSceneA = move(sceneA);
	sceneA = move(sceneB);
	sceneB = move(tempSceneA);
}

void SceneManager::moveSceneBy(unique_ptr<Scene>& fromScene, unique_ptr<Scene>& toScene)
{
	swapSceneBy(fromScene, toScene);
	auto& swappedScene = fromScene;
	if (swappedScene)
	{
		release(swappedScene, true, false, false);
		addScene(swappedScene->getSceneID(), swappedScene);
	}
}

unique_ptr<Scene> SceneManager::createSceneBy(const SCENE_NAME& sceneName)
{
	unique_ptr<Scene> scene;
	switch (sceneName)
	{
	case SCENE_NAME::GAME:
		scene = make_unique<GameScene>();
		break;
	case SCENE_NAME::MAPTOOL:
		scene = make_unique<MapToolScene>();
		break;
	case SCENE_NAME::OPTION:
		scene = make_unique<OptionScene>();
		break;
	case SCENE_NAME::TITLE:
		scene = make_unique<TitleScene>();
		break;
	case SCENE_NAME::LOADING:
		scene = make_unique<LoadingScene>();
		break;
	default:
		assert(false);
		break;
	}

	return move(scene);
}

unique_ptr<Scene> SceneManager::findScene(SCENE_NAME sceneName)
{
	const auto& iter = BlockedScenes.find(move(sceneName));
	if (iter == BlockedScenes.end())
	{
		return nullptr;
	}

	return move(iter->second);
}

void SceneManager::addScene(SCENE_NAME sceneName, unique_ptr<Scene>& scene)
{
	assert(scene);

	BlockedScenes.insert(make_pair(move(sceneName), move(scene)));
}

void SceneManager::deleteScene(SCENE_NAME sceneName)
{
	const auto& iter = BlockedScenes.find(move(sceneName));
	if (iter == BlockedScenes.end())
	{
		assert(false);
		return;
	}

	BlockedScenes.erase(iter);
}

void SceneManager::allDeleteBlockedScenes()
{
	for (auto& scene : BlockedScenes)
	{
		if (scene.second)
			release(scene.second, true, true, true);
	}

	BlockedScenes.clear();
}


