using UnityEngine;
using UnityEngine.UI;

public class GameSetup : MonoBehaviour
{
    void Start()
    {
        CreateEnvironment();
        CreateUI();
        CreatePlayer();
        CreateStations();
    }

    void CreateEnvironment()
    {
        GameObject floor = GameObject.CreatePrimitive(PrimitiveType.Plane);
        floor.name = "Floor";
        floor.transform.localScale = new Vector3(2f, 1f, 2f);
        floor.transform.position = new Vector3(0, 0, 0);
        floor.GetComponent<Renderer>().material = new Material(Shader.Find("Standard")) { color = new Color(0.28f, 0.28f, 0.28f) };

        float wallH = 4f, roomW = 12f, roomD = 12f;

        GameObject wBack = CreateWall("Wall_Back", new Vector3(0, wallH / 2, -roomD / 2), new Vector3(roomW, wallH, 0.3f));
        GameObject wLeft = CreateWall("Wall_Left", new Vector3(-roomW / 2, wallH / 2, 0), new Vector3(0.3f, wallH, roomD));
        GameObject wRight = CreateWall("Wall_Right", new Vector3(roomW / 2, wallH / 2, 0), new Vector3(0.3f, wallH, roomD));

        GameObject ceiling = GameObject.CreatePrimitive(PrimitiveType.Cube);
        ceiling.name = "Ceiling";
        ceiling.transform.localScale = new Vector3(roomW, 0.2f, roomD);
        ceiling.transform.position = new Vector3(0, wallH, 0);
        ceiling.GetComponent<Renderer>().material = new Material(Shader.Find("Standard")) { color = new Color(0.38f, 0.38f, 0.38f) };

        GameObject lightGO = new GameObject("DirectionalLight");
        Light light = lightGO.AddComponent<Light>();
        light.type = LightType.Directional;
        light.intensity = 1f;
        lightGO.transform.rotation = Quaternion.Euler(55, -30, 0);
    }

    GameObject CreateWall(string name, Vector3 pos, Vector3 scale)
    {
        GameObject w = GameObject.CreatePrimitive(PrimitiveType.Cube);
        w.name = name;
        w.transform.localScale = scale;
        w.transform.position = pos;
        w.GetComponent<Renderer>().material = new Material(Shader.Find("Standard")) { color = new Color(0.5f, 0.5f, 0.5f) };
        return w;
    }

    void CreatePlayer()
    {
        GameObject player = new GameObject("Player");
        player.transform.position = new Vector3(0, 1, 4);

        CharacterController cc = player.AddComponent<CharacterController>();
        cc.height = 1.8f;
        cc.radius = 0.4f;
        cc.center = new Vector3(0, 0.9f, 0);

        player.AddComponent<PlayerController>();

        GameObject cam = new GameObject("MainCamera");
        cam.transform.SetParent(player.transform);
        cam.transform.localPosition = new Vector3(0, 0.7f, 0);
        Camera camera = cam.AddComponent<Camera>();
        camera.fieldOfView = 75;
        camera.nearClipPlane = 0.05f;
        cam.tag = "MainCamera";

        player.AddComponent<PlayerInteraction>();
    }

    void CreateStations()
    {
        GameObject grill = new GameObject("GrillStation");
        grill.transform.position = new Vector3(-3.5f, 0.6f, -3);
        BoxCollider gc = grill.AddComponent<BoxCollider>();
        gc.size = new Vector3(1.8f, 1.2f, 1.8f);
        gc.isTrigger = false;
        grill.AddComponent<GrillStation>();

        GameObject fryer = new GameObject("FryerStation");
        fryer.transform.position = new Vector3(0, 0.6f, -3);
        BoxCollider fc = fryer.AddComponent<BoxCollider>();
        fc.size = new Vector3(1.8f, 1.2f, 1.8f);
        fryer.AddComponent<FryerStation>();

        GameObject assembly = new GameObject("AssemblyStation");
        assembly.transform.position = new Vector3(3.5f, 0.6f, -3);
        BoxCollider ac = assembly.AddComponent<BoxCollider>();
        ac.size = new Vector3(2.4f, 1.2f, 2f);
        assembly.AddComponent<AssemblyStation>();

        GameObject buns = new GameObject("BunProvider");
        buns.transform.position = new Vector3(5f, 0.6f, -0.5f);
        BoxCollider bc = buns.AddComponent<BoxCollider>();
        bc.size = new Vector3(1.2f, 1.2f, 1.2f);
        buns.AddComponent<BunProvider>();
    }

    void CreateUI()
    {
        GameObject canvas = new GameObject("Canvas");
        Canvas c = canvas.AddComponent<Canvas>();
        c.renderMode = RenderMode.ScreenSpaceOverlay;
        canvas.AddComponent<CanvasScaler>();
        canvas.AddComponent<GraphicRaycaster>();

        GameObject msg = new GameObject("MessageText");
        msg.transform.SetParent(canvas.transform);
        Text text = msg.AddComponent<Text>();
        text.font = Resources.GetBuiltinResource<Font>("LegacyRuntime.ttf");
        text.fontSize = 38;
        text.alignment = TextAnchor.MiddleCenter;
        text.color = Color.white;
        RectTransform rt = msg.GetComponent<RectTransform>();
        rt.anchorMin = new Vector2(0.5f, 0.85f);
        rt.anchorMax = new Vector2(0.5f, 0.85f);
        rt.pivot = new Vector2(0.5f, 0.5f);
        rt.sizeDelta = new Vector2(900, 100);
        rt.anchoredPosition = Vector2.zero;
        msg.AddComponent<Outline>();

        GameObject ch = new GameObject("Crosshair");
        ch.transform.SetParent(canvas.transform);
        Text chText = ch.AddComponent<Text>();
        chText.font = Resources.GetBuiltinResource<Font>("LegacyRuntime.ttf");
        chText.fontSize = 28;
        chText.alignment = TextAnchor.MiddleCenter;
        chText.color = Color.white;
        chText.text = "+";
        RectTransform chRt = ch.GetComponent<RectTransform>();
        chRt.anchorMin = new Vector2(0.5f, 0.5f);
        chRt.anchorMax = new Vector2(0.5f, 0.5f);
        chRt.pivot = new Vector2(0.5f, 0.5f);
        chRt.sizeDelta = new Vector2(30, 30);
        chRt.anchoredPosition = Vector2.zero;

        canvas.AddComponent<UIManager>();
        canvas.tag = "UI";
    }
}
