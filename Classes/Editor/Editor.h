#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class Editor : public Layer
{
public:
	static Editor* create(Scene* scene);
	bool init(Scene* scene);

private:
	Scene* scene{};
	Vec2 middleScenePoint{};

#pragma region Hierarchy
	
	ui::ScrollView* hierarchyPanel{};
	Node* currentSelect{};
	void addToScrollView(Node* node);
	void getAllSceneChildren();
	void onButtonClick(Ref*, ui::Widget::TouchEventType);

	bool gizmo_isMoveCmd{ false };
	Vec2 gizmo_tmpOrigin{};
	Vec2 gizmo_Pos{};
	DrawNode* gizmoBox{};
	void gizmo_OnMouseDown(EventMouse* ev);
	void gizmo_OnMouseUp(EventMouse* ev);
	void gizmo_OnMouseMove(EventMouse* ev);

#pragma endregion

#pragma region Properties

	ui::ListView* propertiesPanel{};

	Label* propLbl_WorldPos{};
	Label* propLbl_LocalPos{};
	Label* propLbl_Size{};

#pragma endregion

#pragma region Camera
	bool cam_isMoveCmd{ false };
	Vec2 cam_tmpOrigin{};
	Vec2 cam_Pos{};

	Camera* camera{};

	void cam_OnMouseDown(EventMouse* ev);
	void cam_OnMouseUp(EventMouse* ev);
	void cam_OnMouseMove(EventMouse* ev);
	
#pragma endregion

};

class HierarchyButton : public ui::Button
{
public:
	CREATE_FUNC(HierarchyButton);
	CC_SYNTHESIZE(Node* ,nodeSelf, NodeSelf);
};