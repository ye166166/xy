using UnityEngine;

public class AssemblyStation : MonoBehaviour, IInteractable
{
    private enum AssemblyState { WaitingForSteak, WaitingForFries, WaitingForTopBun, Complete }

    private AssemblyState currentState;
    private GameObject bottomBun;
    private GameObject placedSteak;
    private GameObject placedFries;
    private GameObject topBun;
    private PlayerInteraction currentPlayer;
    private UIManager ui;

    private Material bunMat;

    void Start()
    {
        bunMat = new Material(Shader.Find("Standard")) { color = new Color(0.82f, 0.6f, 0.22f) };
        ui = FindObjectOfType<UIManager>();
        CreateVisual();
        ResetAssembly();
    }

    void CreateVisual()
    {
        GameObject visual = new GameObject("AssemblyVisual");
        visual.transform.SetParent(transform);
        visual.transform.localPosition = Vector3.zero;

        GameObject counter = GameObject.CreatePrimitive(PrimitiveType.Cube);
        counter.name = "Counter";
        counter.transform.SetParent(visual.transform);
        counter.transform.localPosition = new Vector3(0, -0.25f, 0);
        counter.transform.localScale = new Vector3(2.2f, 0.5f, 1.6f);
        counter.GetComponent<Renderer>().material = new Material(Shader.Find("Standard")) { color = new Color(0.72f, 0.72f, 0.72f) };
    }

    public void Interact(PlayerInteraction player)
    {
        currentPlayer = player;

        if (currentState == AssemblyState.Complete)
        {
            player.ShowMessage("Burger complete! Press [R] to reset.");
            return;
        }

        if (!player.HasItem())
        {
            string[] hints = { "Place a cooked steak here.", "Place cooked fries here.", "Place a top bun here." };
            player.ShowMessage(hints[(int)currentState]);
            return;
        }

        GameObject held = player.GetCarriedItem();
        FoodItem food = held.GetComponent<FoodItem>();
        if (food == null) return;

        switch (currentState)
        {
            case AssemblyState.WaitingForSteak:
                if (food.itemType == FoodType.Steak && food.currentState == FoodState.Cooked)
                {
                    player.DropItem();
                    PlaceItem(held, ref placedSteak, 0.38f);
                    currentState = AssemblyState.WaitingForFries;
                    player.ShowMessage("Steak on! Now add fries.");
                }
                else player.ShowMessage("Need a cooked steak!");
                break;

            case AssemblyState.WaitingForFries:
                if (food.itemType == FoodType.Fries && food.currentState == FoodState.Cooked)
                {
                    player.DropItem();
                    PlaceItem(held, ref placedFries, 0.42f);
                    currentState = AssemblyState.WaitingForTopBun;
                    player.ShowMessage("Fries on! Now add the top bun.");
                }
                else player.ShowMessage("Need cooked fries!");
                break;

            case AssemblyState.WaitingForTopBun:
                if (food.itemType == FoodType.TopBun)
                {
                    player.DropItem();
                    PlaceItem(held, ref topBun, 0.55f);
                    currentState = AssemblyState.Complete;
                    player.ShowMessage("Burger complete! Press [R] to make another.");
                }
                else player.ShowMessage("Need the top bun!");
                break;
        }
    }

    void PlaceItem(GameObject item, ref GameObject slot, float height)
    {
        slot = item;
        item.transform.position = transform.position + new Vector3(0, height, 0);
        item.transform.rotation = Quaternion.identity;
        item.transform.SetParent(transform);
        Rigidbody rb = item.GetComponent<Rigidbody>();
        if (rb != null) { rb.isKinematic = true; rb.useGravity = false; }
        Collider col = item.GetComponent<Collider>();
        if (col != null) col.enabled = false;
    }

    void Update()
    {
        if (currentState == AssemblyState.Complete && Input.GetKeyDown(KeyCode.R))
        {
            ResetAssembly();
            if (ui != null) ui.ShowMessage("Assembly reset. Start a new burger!");
        }
    }

    void ResetAssembly()
    {
        if (bottomBun != null) Destroy(bottomBun);
        if (placedSteak != null) Destroy(placedSteak);
        if (placedFries != null) Destroy(placedFries);
        if (topBun != null) Destroy(topBun);
        placedSteak = null;
        placedFries = null;
        topBun = null;
        SpawnBottomBun();
        currentState = AssemblyState.WaitingForSteak;
    }

    void SpawnBottomBun()
    {
        bottomBun = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
        bottomBun.name = "BottomBun";
        bottomBun.transform.localScale = new Vector3(0.55f, 0.1f, 0.55f);
        bottomBun.transform.position = transform.position + new Vector3(0, 0.3f, 0);
        bottomBun.transform.rotation = Quaternion.identity;
        bottomBun.transform.SetParent(transform);
        bottomBun.GetComponent<Renderer>().material = bunMat;
        Collider col = bottomBun.GetComponent<Collider>();
        if (col != null) col.enabled = false;
    }
}
