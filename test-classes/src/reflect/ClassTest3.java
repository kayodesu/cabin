package reflect;

import java.util.Arrays;

/**
 * @author Yo Ka
 */
public class IntClassTest {
    public static void main(String[] args) {
        System.out.println(Object.class); // class java.lang.Object
        System.out.println(Object.class.getInterfaces().length); // 0
        System.out.println(Object.class.getSuperclass()); // null

        System.out.println();

        System.out.println(String.class); // class java.lang.String
        // [interface java.io.Serializable, interface java.lang.Comparable, interface java.lang.CharSequence]
        System.out.println(Arrays.toString(String.class.getInterfaces()));
        System.out.println(String.class.getSuperclass()); // class java.lang.Object

        System.out.println();

        System.out.println(int.class); // int
        System.out.println(int.class.getInterfaces().length); // 0
        System.out.println(int.class.getSuperclass()); // null

        System.out.println();

        System.out.println(int[].class); // class [I
        // [interface java.lang.Cloneable, interface java.io.Serializable]
        System.out.println(Arrays.toString(int[].class.getInterfaces()));
        System.out.println(int[].class.getSuperclass()); // class java.lang.Object

        System.out.println();

        System.out.println(I.class); // interface reflect.IntClassTest$I
        System.out.println(I.class.getInterfaces().length); // 0
        System.out.println(I.class.getSuperclass()); // null
    }

    static interface I {}
}
