package array;

import java.util.Arrays;

/**
 * Status: Pass
 */
public class BigArrayTest {
    public static void main(String[] args) {
        int[][][][][] arr = new int[3][4][5][6][7];
        for (int i = 0; i < arr.length; i++) {
            arr[i][0][0][0][0] = 3;
        }
        System.out.println(Arrays.deepToString(arr));
        System.out.println(arr.getClass());
    }
}
