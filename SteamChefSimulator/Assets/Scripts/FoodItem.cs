using UnityEngine;

public enum FoodType
{
    Steak,
    Fries,
    TopBun
}

public enum FoodState
{
    Raw,
    Cooked,
    Burnt
}

public class FoodItem : MonoBehaviour
{
    public FoodType itemType;
    public FoodState currentState = FoodState.Raw;

    public void SetState(FoodState newState)
    {
        currentState = newState;
    }
}
