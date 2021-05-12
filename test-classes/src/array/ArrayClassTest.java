package array;

/**
 * Status: Pass
 */
public class ArrayClassTest {
    public static void main(String[] args) {
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

        ArrayClassTest[][] arr1 = new ArrayClassTest[3][4];
        System.out.println(arr1.getClass()); // class [[Larray.ArrayClassTest;
        System.out.println(arr1[0].getClass()); // class [Larray.ArrayClassTest;
        System.out.println(ArrayClassTest[].class); // class [Larray.ArrayClassTest;

        System.out.println(arr1[0].getClass() == arr1[1].getClass()); // true
        System.out.println(arr1.getClass() == ArrayClassTest[][].class); // true
        System.out.println(arr1[0].getClass() == ArrayClassTest[].class); // true

        System.out.println(arr1[0].getClass().getClassLoader()); // sun.misc.Launcher$AppClassLoader@18b4aac2
        System.out.println(ArrayClassTest[].class.getClassLoader()); // sun.misc.Launcher$AppClassLoader@18b4aac2
    }
}
