import java.lang.reflect.Array;
import java.util.Arrays;
import java.util.Random;

public class ArrayTest {

    @Utils.TestMethod(pass = false)
    public static void newArray() {
        var o1 = (int[]) Array.newInstance(int.class, 5);
        System.out.println(o1.getClass().getName());
        System.out.println(o1.length);
        System.out.println(o1.getClass().getName() + ", " + o1.length);

        var o2 = (int[][]) Array.newInstance(int[].class, 5);
        System.out.println(o2.getClass().getName() + ", " + o2.length);
        var o3 = (int[][][]) Array.newInstance(int[][].class, 5);
        System.out.println(o3.getClass().getName() + ", " + o3.length);
    }

    @Utils.TestMethod(pass = false)
    public static void arrayInit() {
        // The references inside the array are
        // automatically initialized to null.
        System.out.println(Arrays.toString(new Object[5]));

        // The primitives inside the array are
        // automatically initialized to zero.
        System.out.println(Arrays.toString(new int[5]));
    }

    @Utils.TestMethod(pass = true)
    public static void zeroLenArray() {
        int[] arr = new int[0];
        System.out.println(arr.getClass().getName());
        System.out.println(arr.length); // 0
    }

    // test multi dimensional array
    @Utils.TestMethod(pass = false)
    public static void multiDimArray() {
        int[][][] y = {
                {
                        {1},
                        {1, 2},
                        {1, 2, 3}
                }
        };

        System.out.println(y.length);       // 1
        System.out.println(y[0].length);    // 3
        System.out.println(y[0][0].length); // 1
        System.out.println(y[0][1].length); // 2
        System.out.println(y[0][2].length); // 3

        System.out.println(Arrays.deepToString(y));
    }

    // test multi dimensional array
    @Utils.TestMethod(pass = false)
    public static void newMultiDimArray() {
        int[][][] x = new int[2][3][5];
        System.out.println(x.length); // 2
        System.out.println(x[0].length); // 3
        System.out.println(x[1][2].length); // 5

        x[1][2][3] = 7;
        System.out.println(x[1][2][3]); // 7
    }

    @Utils.TestMethod(pass = false)
    public static void bigArray() {
        int[][][][][] arr = new int[3][4][5][6][7];
        for (int i = 0; i < arr.length; i++) {
            arr[i][0][0][0][0] = 3;
        }
        System.out.println(Arrays.deepToString(arr));
        System.out.println(arr.getClass());
    }

    @Utils.TestMethod(pass = false)
    public static void longArray() {
        int len = 123456789;
        int[] arr = new int[len];
        for (int i = 0; i < len; i++) {
            arr[i] = i;
        }

        for (int i = 0; i < len; i++) {
            if (arr[i] != i) {
                System.out.println("Fail");
                return;
            }
        }

        System.out.println("Pass");
    }

    // 粗糙数组
    @Utils.TestMethod(pass = false)
    public static void raggedArray() {
        Random random = new Random();

        // 3-D array with varied-length vectors:
        int[][][] a = new int[random.nextInt(5) + 2][][];
        for (int i = 0; i < a.length; i++) {
            a[i] = new int[random.nextInt(4)][];
            for (int j = 0; j < a[i].length; j++) {
                a[i][j] = new int[random.nextInt(3)];
            }
        }
        System.out.println(Arrays.deepToString(a));
    }

    @Utils.TestMethod(pass = false)
    public static void arrayCast() {
        try {
            int[][] arr = new int[2][3];
            Object[] o = (Object[]) arr; // OK
            int[][][] arr1 = new int[2][3][4];
            Object[] o1 = (Object[]) arr1; // OK
            Object[][] o2 = (Object[][]) arr1; // OK
        } catch (ClassCastException e) {
            System.out.println("Fail");
            return;
        }
        System.out.println("Pass");
    }

    @Utils.TestMethod(pass = false)
    public static void arrayCopy() {
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

    @Utils.TestMethod(pass = false)
    public static void arrayClass() {
        int[][] arr = new int[3][4];
        System.out.println(arr.getClass()); // class [[I
        System.out.println(arr[0].getClass()); // class [I
        System.out.println(int[].class); // class [I

        System.out.println(arr[0].getClass() == arr[1].getClass()); // true
        System.out.println(arr.getClass() == int[][].class); // true
        System.out.println(arr[0].getClass() == int[].class); // true

        System.out.println(arr[0].getClass().getClassLoader()); // null
        System.out.println(int[].class.getClassLoader()); // null

        System.out.println("------------------------");

        ArrayTest[][] arr1 = new ArrayTest[3][4];
        System.out.println(arr1.getClass()); // class [[LArrayTest;
        System.out.println(arr1[0].getClass()); // class [LArrayTest;
        System.out.println(ArrayTest[].class); // class [LArrayTest;

        System.out.println(arr1[0].getClass() == arr1[1].getClass()); // true
        System.out.println(arr1.getClass() == ArrayTest[][].class); // true
        System.out.println(arr1[0].getClass() == ArrayTest[].class); // true

        System.out.println(arr1[0].getClass().getClassLoader()); // jdk.internal.loader.ClassLoaders$AppClassLoader@6d06d69c
        System.out.println(ArrayTest[].class.getClassLoader()); // jdk.internal.loader.ClassLoaders$AppClassLoader@6d06d69c
    }

    @Utils.TestMethod(pass = false)
    public static void testArray() {
        int[] a1 = new int[10]; // newarray
        String[] a2 = new String[10]; // anewarray
        int[][] a3 = new int[10][10]; // multianewarray
        int[][][] a4 = new int[20][3][4]; // multianewarray
        a3[5][7] = 6;
        a4[4][2][1] = 4;
        int x = a1.length; // arraylength
        a1[0] = 100; // iastore
        int y = a1[0]; // iaload
        a2[0] = "abc"; // aastore
        String s = a2[0]; // aaload

        System.out.println(y); // 100
        System.out.println(a3[5][7]); // 6
        System.out.println(a4[4][2][1]); // 4
        System.out.println(x); // 10
        System.out.println(s); // abc
    }

    public static void main(String[] args) {
        Utils.invokeAllTestMethods(ArrayTest.class);
    }
}
