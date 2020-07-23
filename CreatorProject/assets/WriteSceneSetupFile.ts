// Learn TypeScript:
//  - https://docs.cocos.com/creator/manual/en/scripting/typescript.html
// Learn Attribute:
//  - https://docs.cocos.com/creator/manual/en/scripting/reference/attributes.html
// Learn life-cycle callbacks:
//  - https://docs.cocos.com/creator/manual/en/scripting/life-cycle-callbacks.html

const { ccclass, property, executeInEditMode } = cc._decorator;
const PROJECT_NAME = "GunBound/";

class CppData
{
    public name: string = "";
    public position: cc.Vec2 = null;
    public anchor: cc.Vec2 = null;

    public spriteName: string = PROJECT_NAME;
    public colliderPoints: cc.Vec2[] = [];
    public circleCollider_radius: number = 0;
    public circleCollider_offset: cc.Vec2 = null;
}

@ccclass
@executeInEditMode
export default class WriteSceneSetupFile extends cc.Component
{
    public cppData: CppData[] = [];

    @property({ multiline: true })
    public result: string = "";

    @property([cc.Node])
    public collections: cc.Node[] = [];

    onFocusInEditor()
    {
        this.cppData = [];
        this.result = "";

        for (const node of this.collections)
        {
            const data = new CppData();

            data.name = node.name;
            data.position = node.getPosition();

            data.anchor = node.getAnchorPoint();

            const sprite = node.getComponent(cc.Sprite);
            if (sprite)
            {
                data.spriteName = PROJECT_NAME;
            }

            const polygon = node.getComponent(cc.PolygonCollider);
            if (polygon)
            {
                data.colliderPoints = polygon.points;
            }
            else
            {
                const circle = node.getComponent(cc.CircleCollider);
                if (circle)
                {
                    data.circleCollider_radius = circle.radius;
                    data.circleCollider_offset = circle.offset;
                }
            }

            this.cppData.push(data);
        }

        for (const data of this.cppData)
        {
            const variableName = data.name.toUpperCase();
            this.result += `const std::string ${variableName}_NAME {"${data.name}"};\n`;
            this.result += `const std::string ${variableName}_FILE_PATH {"${data.spriteName}"};\n`;
            this.result += `const Vec2 ${variableName}_POSITION {${data.position.x}, ${data.position.y}};\n`;

            if (data.anchor.x != 0.5 && data.anchor.y != 0.5)
                this.result += `const Vec2 ${variableName}_ANCHOR {${data.anchor.x}, ${data.anchor.y}};\n`;

            if (data.colliderPoints.length > 0)
            {
                let vectors = "";
                for (let i = 0; i < data.colliderPoints.length; i++)
                {
                    const point = data.colliderPoints[i];
                    vectors += `Vec2{${point.x}, ${point.y}}, `;
                    if (i % 5 == 0 && i != 0)
                        vectors += "\n\t";
                }
                this.result += `const std::vector<Vec2> ${variableName}_POLYLINE {\n\t${vectors.substr(0, vectors.length - 2)}\n};\n`;
            }
            if (data.circleCollider_radius)
            {
                this.result += `const float ${variableName}_CIRCLE_COL_RADIUS {${data.circleCollider_radius}};\n`;
                if (data.circleCollider_offset.x != 0 && data.circleCollider_offset.y != 0)
                    this.result += `const Vec2 ${variableName}_CIRCLE_COL_OFFSET {${data.circleCollider_offset.x}, ${data.circleCollider_offset.y}};\n`;
            }

            this.result += "\n\n";
        }

    }
}
