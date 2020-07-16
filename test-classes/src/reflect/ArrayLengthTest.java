package reflect;

import java.lang.reflect.Array;

public class NewArrayTest {
    public static void main(String[] args) {
        newNullArray();
    }

    public static void newNullArray() {
        try {
            Object x = null;
            Array.newArray(x, 3);
        } catch (NullPointerException e) {
            System.out.println(e.getMessage() == null);
        }
    }
}
