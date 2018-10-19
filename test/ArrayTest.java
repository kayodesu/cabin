
public class ArrayTest {
    public static void main(String[] args) {
        System.out.print("ArrayTest: ");
        
        int[] a1 = new int[10]; // newarray
        String[] a2 = new String[10]; // anewarray
        int[][] a3 = new int[10][10]; // multianewarray
        Integer[][][] a4 = new Integer[20][3][4]; // multianewarray
        a4[4][2][1] = 4;
        int x = a1.length; // arraylength
        a1[0] = 100; // iastore
        int y = a1[0]; // iaload
        a2[0] = "abc"; // aastore
        String s = a2[0]; // aaload
        
        System.out.println(a1[0]);
        System.out.println(a4[4][2][1]);
        System.out.println(x);
        System.out.println(s);
        
        int[] arr = { 22, 84, 77, 11, 95, 9, 78, 56, 36, 97, 65, 36, 10, 24, 92 };
        bubbleSort(arr);
        for (int i = 0; i < arr.length - 1; i++) {
            if (arr[i+1] < arr[i]) {
                System.out.println("failed.");
                return;
            }
        }
        
        System.out.println("pass.");
    }
    
    
    /**
     * 冒泡排序
     */
    private static void bubbleSort(int[] arr) {  
        boolean swapped = true;
        int j = 0;
        int tmp;
        while (swapped) {
            swapped = false;
            j++;
            for (int i = 0; i < arr.length - j; i++) {
                if (arr[i] > arr[i + 1]) {
                    tmp = arr[i];
                    arr[i] = arr[i + 1];
                    arr[i + 1] = tmp;
                    swapped = true;
                }
            }
        }
    }
}

    