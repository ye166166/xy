using UnityEngine;

public class BunProvider : MonoBehaviour, IInteractable
{
    private Material bunMat;

    void Start()
    {
        bunMat = new Material(Shader.Find("Standard")) { color = new Color(0.82f, 0.6f, 0.22f) };
        CreateVisual();
    }

    void CreateVisual()
    {
        GameObject visual = new GameObject("BunProviderVisual");
        visual.transform.SetParent(transform);
        visual.transform.localPosition = Vector3.zero;

        GameObject counter = GameObject.CreatePrimitive(PrimitiveType.Cube);
        counter.name = "Counter";
        counter.transform.SetParent(visual.transform);
        counter.transform.localPosition = new Vector3(0, -0.25f, 0);
        counter.transform.localScale = new Vector3(0.9f, 0.5f, 0.9f);
        counter.GetComponent<Renderer>().material = new Material(Shader.Find("Standard")) { color = new Color(0.55f, 0.55f, 0.55f) };

        for (int i = 0; i < 3; i++)
        {
            GameObject display = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            display.name = "BunDisplay" + i;
            display.transform.SetParent(visual.transform);
            display.transform.localPosition = new Vector3(0, 0.08f + i * 0.1f, 0);
            display.transform.localScale = new Vector3(0.45f, 0.08f, 0.45f);
            display.GetComponent<Renderer>().material = bunMat;
            Collider col = display.GetComponent<Collider>();
            if (col != null) col.enabled = false;
        }
    }

    public void Interact(PlayerInteraction player)
    {
        if (player.HasItem())
        {
            player.ShowMessage("Drop what you're holding first.");
            return;
        }

        GameObject bun = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
        bun.name = "TopBun";
        bun.transform.localScale = new Vector3(0.55f, 0.1f, 0.55f);
        bun.GetComponent<Renderer>().material = bunMat;

        FoodItem food = bun.AddComponent<FoodItem>();
        food.itemType = FoodType.TopBun;
        food.SetState(FoodState.Cooked);
        Rigidbody rb = bun.AddComponent<Rigidbody>();
        rb.mass = 0.3f;

        player.PickUpItem(bun);
        player.ShowMessage("Picked up top bun.");
    }
}
