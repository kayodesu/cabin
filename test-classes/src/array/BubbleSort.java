import java.util.Arrays;

/**
 * Status: Pass
 */
public class BubbleSort {
    public static void main(String[] args) {
        int[] arr = { 22, 84, 77, 11, 95, 9, 78, 56, 36, 97, 65, 36, 10, 24, 92 };
        bubbleSort(arr);
        System.out.println(Arrays.toString(arr));
    }

    private static void bubbleSort(int[] arr) {
        for (int i = 0; i < arr.length - 1; i++){
            for (int j = 0; j < arr.length - 1 - i; j++){
                if(arr[j] > arr[j + 1]){
                    int temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }
    }
}
