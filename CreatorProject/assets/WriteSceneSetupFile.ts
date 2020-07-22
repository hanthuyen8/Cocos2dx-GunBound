// Learn TypeScript:
//  - https://docs.cocos.com/creator/manual/en/scripting/typescript.html
// Learn Attribute:
//  - https://docs.cocos.com/creator/manual/en/scripting/reference/attributes.html
// Learn life-cycle callbacks:
//  - https://docs.cocos.com/creator/manual/en/scripting/life-cycle-callbacks.html

import WriteThisToFile from "./WriteThisToFile"


const { ccclass, property, executeInEditMode } = cc._decorator;

class CppData
{
    public name: string = "";
    public position: cc.Vec2 = null;
    public anchor: cc.Vec2 = null;

    public spriteName: string = "";
    public colliderPoints: cc.Vec2[] = [];
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
                data.spriteName = sprite.spriteFrame.getTexture().url;
            }

            const polygon = node.getComponent(cc.PolygonCollider)
            if (polygon)
            {
                data.colliderPoints = polygon.points;
            }

            this.cppData.push(data);
        }

        for (const data of this.cppData)
        {
            const variableName = data.name.toUpperCase();
            this.result += `const std::string ${variableName}_FILE_PATH {"${data.name}"};\n`;
            this.result += `const Vec2 ${variableName}_POSITION {${data.position.x}, ${data.position.y}};\n`;

            if (data.anchor.x != 0.5 && data.anchor.y != 0.5)
                this.result += `const Vec2 ${variableName}_ANCHOR {${data.anchor.x}, ${data.anchor.y}};\n`

            if (data.spriteName)
                this.result += `const std::string ${variableName}_SPRITE_PATH {${data.spriteName}};\n`;

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

            this.result += "\n\n";
        }

    }
}
