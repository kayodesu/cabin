package array;

import java.util.Arrays;

/**
 * Status: Pass
 */
public class ArrayCopyTest {

    public static void main(String[] args) {
        int len = 12345678;
        int[] arr = new int[len];
        for (int i = 0; i < len; i++) {
            arr[i] = i;
        }

        int[] copy = Arrays.copyOf(arr, arr.length);
        for (int i = 0; i < len; i++) {
            if (copy[i] != i) {
                System.out.println("Fail");
                return;
            }
        }

        System.out.println("Pass");
    }
}
