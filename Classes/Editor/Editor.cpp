#include "Editor.h"
#include <typeinfo>

Editor* Editor::create(Scene* scene)
{
	auto editor = new Editor();
	if (editor && editor->init(scene))
	{
		editor->autorelease();
		return editor;
	}

	CC_SAFE_DELETE(editor);
	return nullptr;
}

bool Editor::init(Scene* scene)
{
	if (!Layer::init())
		return false;

	this->scene = scene;
	this->camera = scene->getDefaultCamera();


	// Camera Movement
	cam_Pos = camera->getPosition();
	const auto size = Director::getInstance()->getVisibleSize();
	middleScenePoint = { size.width / 2, size.height / 2 };

	const auto cameraMove = EventListenerMouse::create();
	cameraMove->onMouseDown = CC_CALLBACK_1(Editor::cam_OnMouseDown, this);
	cameraMove->onMouseUp = CC_CALLBACK_1(Editor::cam_OnMouseUp, this);
	cameraMove->onMouseMove = CC_CALLBACK_1(Editor::cam_OnMouseMove, this);
	scene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(cameraMove, this);

	// Hierarchy Panel
	hierarchyPanel = ui::ScrollView::create();
	this->addChild(hierarchyPanel);
	hierarchyPanel->setAnchorPoint(Vec2{ 0,1 });
	hierarchyPanel->setContentSize(Size{ 300, 500 });
	hierarchyPanel->setInnerContainerSize(Size{ 300, 1000 });

	hierarchyPanel->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);
	hierarchyPanel->setBackGroundColor(Color3B::BLACK);
	hierarchyPanel->setScrollBarWidth(4);
	hierarchyPanel->setScrollBarPositionFromCorner(Vec2(2, 2));
	hierarchyPanel->setScrollBarColor(Color3B::WHITE);
	hierarchyPanel->setGlobalZOrder(200);

	getAllSceneChildren();

	gizmoBox = DrawNode::create(4);
	this->addChild(gizmoBox);
	const auto gizmoMove = EventListenerMouse::create();
	gizmoMove->onMouseDown = CC_CALLBACK_1(Editor::gizmo_OnMouseDown, this);
	gizmoMove->onMouseUp = CC_CALLBACK_1(Editor::gizmo_OnMouseUp, this);
	gizmoMove->onMouseMove = CC_CALLBACK_1(Editor::gizmo_OnMouseMove, this);
	scene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(gizmoMove, gizmoBox);

	// Properties Panel
	propertiesPanel = ui::ListView::create();
	this->addChild(propertiesPanel);
	propertiesPanel->setContentSize(Size{ 300,500 });
	propertiesPanel->setInnerContainerSize(Size{ 300, 1000 });
	propertiesPanel->setAnchorPoint(Vec2::ONE);
	propertiesPanel->setGlobalZOrder(201);
	propertiesPanel->setBackGroundColor(Color3B::BLACK);
	propertiesPanel->setItemsMargin(10);

	propLbl_WorldPos = Label::create();
	propLbl_LocalPos = Label::create();
	propLbl_Size = Label::create();

	propertiesPanel->addChild(propLbl_WorldPos);
	propertiesPanel->addChild(propLbl_LocalPos);
	propertiesPanel->addChild(propLbl_Size);

	return true;
}

void Editor::getAllSceneChildren()
{
	for (const auto child : scene->getChildren())
	{
		auto name = child->getName();
		name += std::string{ " - " } + typeid(child).name();

		auto btn = HierarchyButton::create();
		btn->setAnchorPoint(Vec2{ 0,1 });
		btn->setTitleText(name);
		btn->setNodeSelf(child);
		btn->addTouchEventListener(CC_CALLBACK_2(Editor::onButtonClick, this));

		addToScrollView(btn);
	}
}

void Editor::onButtonClick(Ref* ref, ui::Widget::TouchEventType touch)
{
	if (touch == ui::Widget::TouchEventType::ENDED)
	{
		gizmoBox->clear();
		const auto btn = dynamic_cast<HierarchyButton*>(ref);
		currentSelect = btn->getNodeSelf();
		const auto bound = currentSelect->getBoundingBox();
		auto size = bound.size;
		const auto localPos = currentSelect->getPosition();
		const auto worldPos = currentSelect->convertToWorldSpaceAR(Vec2::ZERO);
		const auto origin = this->getParent()->convertToNodeSpace(currentSelect->getParent()->convertToWorldSpaceAR(bound.origin));

		if (size.width || size.height == 0)
		{
			size.width = size.height = 100;
		}
		gizmoBox->drawSolidRect(origin, Vec2{ origin.x + size.width / 2, origin.y + size.height / 2 }, Color4F{ 1,0,0,0.7f });

		// Properties
		std::stringstream stream{};
		stream << "World Pos: (" << worldPos.x << " , " << worldPos.y << ")";
		propLbl_WorldPos->setString(stream.str());

		stream.clear();
		stream << "Local Pos: (" << localPos.x << " , " << localPos.y << ")";
		propLbl_LocalPos->setString(stream.str());

		stream.clear();
		stream << "Size: width = " << size.width << " , height = " << size.height;
		propLbl_Size->setString(stream.str());
	}
}

void Editor::addToScrollView(Node* node)
{
	const auto selfSize = node->getBoundingBox().size;
	Vec2 pos;
	const auto childrenCount = hierarchyPanel->getChildrenCount();
	if (childrenCount == 0)
	{
		pos.y = hierarchyPanel->getInnerContainerSize().height;
	}
	else
	{
		const auto children = hierarchyPanel->getChildren();
		const auto lastChildren = children.at(childrenCount - 1);
		auto lastSize = lastChildren->getBoundingBox().size;
		pos = lastChildren->getPosition();

		pos.y -= lastSize.height / 2;
	}
	pos.y -= selfSize.height / 2;
	hierarchyPanel->addChild(node);
	node->setPosition(pos);
}

#pragma region Camera Movement

void Editor::cam_OnMouseDown(EventMouse* ev)
{
	if (ev->getMouseButton() == EventMouse::MouseButton::BUTTON_MIDDLE)
	{
		cam_isMoveCmd = true;
		cam_tmpOrigin = ev->getLocationInView();
	}
}

void Editor::cam_OnMouseUp(EventMouse* ev)
{
	if (ev->getMouseButton() == EventMouse::MouseButton::BUTTON_MIDDLE)
	{
		cam_isMoveCmd = false;
		cam_Pos = camera->getPosition();
	}
}


void Editor::cam_OnMouseMove(EventMouse* ev)
{
	if (!cam_isMoveCmd)
		return;

	const auto newPos = cam_Pos + (cam_tmpOrigin - ev->getLocationInView());
	camera->setPosition(newPos);
	hierarchyPanel->setPosition(Vec2{ newPos.x - middleScenePoint.x, newPos.y + middleScenePoint.y });
	//propertiesPanel->setPosition(Vec2{ newPos.x + middleScenePoint.x, newPos.y + middleScenePoint.y });
}

#pragma endregion

#pragma region Gizmo Movement

void Editor::gizmo_OnMouseDown(EventMouse* ev)
{
	if (!currentSelect)
		return;

	if (ev->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
	{
		gizmo_isMoveCmd = true;
		gizmo_tmpOrigin = ev->getLocationInView();
		gizmo_Pos = currentSelect->getPosition();
	}
}

void Editor::gizmo_OnMouseUp(EventMouse* ev)
{
	if (ev->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
	{
		gizmo_isMoveCmd = false;
	}
}


void Editor::gizmo_OnMouseMove(EventMouse* ev)
{
	if (!gizmo_isMoveCmd)
		return;

	const auto newPos = gizmo_Pos + (ev->getLocationInView() - gizmo_tmpOrigin);
	currentSelect->setPosition(newPos);
}

#pragma endregion
