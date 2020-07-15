// Learn TypeScript:
//  - https://docs.cocos.com/creator/manual/en/scripting/typescript.html
// Learn Attribute:
//  - https://docs.cocos.com/creator/manual/en/scripting/reference/attributes.html
// Learn life-cycle callbacks:
//  - https://docs.cocos.com/creator/manual/en/scripting/life-cycle-callbacks.html

const { ccclass, property, executeInEditMode } = cc._decorator;

@ccclass
@executeInEditMode
export default class PolygonShapeToVector2 extends cc.Component
{
    @property()
    vector2Points: string = "";

    @property(cc.PolygonCollider)
    polygonCollider: cc.PolygonCollider = null;

    onFocusInEditor()
    {
        if (!this.polygonCollider)
        {
            this.polygonCollider = this.getComponent(cc.PolygonCollider);
            if (!this.polygonCollider)
                this.polygonCollider = this.addComponent(cc.PolygonCollider);
        }

        if (this.polygonCollider.points.length == 0)
            cc.log("Hãy generate point cho PolyCollider của Node: " + this.node.name);

        let result = "";
        for (const vec of this.polygonCollider.points)
        {
            result += `Vec2{${Math.round(vec.x)},${Math.round(vec.y)}},`;
        }
        this.vector2Points = result;
        //copyToClipboard(result);
    }
}

export const copyToClipboard = (input: string) =>
{
    document.addEventListener('copy', (e: ClipboardEvent) =>
    {
        e.clipboardData.setData('text/plain', input);
        e.preventDefault();
        document.removeEventListener('copy', this.e);
    });
    document.execCommand('copy');
};
